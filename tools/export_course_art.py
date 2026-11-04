#!/usr/bin/env python3
"""Render original course artwork (BackGFX, FrontGFX, parallax backgrounds) for
the Wii, as the game shows it on a new save (TotalStars = 0, BGquality HIGH).

The pinned SWF is patched in a private copy before FFDec renders it:
* BackGFX is frozen on its resting frame (frame 3: `stop()`), FrontGFX on 1.
* Placed objects (coins, enemies, signs, birds...) are removed; the game draws
  them live. They are matched to their SWF placement by position.
* Scripted colour: every `new Color(this).setTransform` from
  `TotalStars*1.5+25` (Color Back/Front GFX, Castle Colored BG...) is applied as
  the colour transform the script would set at 0 stars, unless the script is
  guarded by `TotalStars !== 0`. Clips that `nextFrame()` unless `_root.Cinema`
  are frozen on frame 2. Rules are read from FFDec's decompiled scripts.

Art is rendered at ZOOM screen pixels per course pixel (300 course lines fill
the 480-line screen), encoded as GX RGB5A3 tiles of at most 1024x1024, and
LZ4-compressed. Output: SM63ART C format, see docs/course-format.md.
"""
import argparse, hashlib, json, re, struct, subprocess
from pathlib import Path
import xml.etree.ElementTree as ET
import numpy as np
from PIL import Image
import lz4.block
import swf_states as S
import xfl
import export_course as C
import art_repair
import swf_bitmap_fix
import controller_signs
import ui_art
import sign_anim
import faint_art


def sign_animated(labels):
    return [l for l in labels if l in sign_anim.SPEC]

ZOOM = 1.6
TILE = 1024
LAYERS = ['FarBackground', 'Background', 'MidBackground', 'CloseBackground', 'BackGFX', 'FrontGFX']
PARALLAX = {'FarBackground': 0.0, 'Background': 0.1, 'MidBackground': 0.2, 'CloseBackground': 0.5,
            'BackGFX': 1.0, 'FrontGFX': 1.0}  # StartChar: BG -b/10, Mid -b/5, Close -b/2
FORMULA = re.compile(r'TotalStars\s*\*\s*1\.5\)?\s*\+\s*25')
GUARD = re.compile(r'if\s*\(\s*_root\.TotalStars\s*!==?\s*0\s*\)')


def star_tint(total_stars=0):
    """colorTrans ra/ga/ba for Color Back/Front GFX scripts (percent)."""
    color = round(total_stars * 1.5) + 25
    return (color * 0.5) + 50 - abs(100 - color) / 3


def course_tables(dom):
    out = {}
    for key, value in re.findall(r'_root\["([^"]+)"\]\s*=\s*"([^"]*)";', dom):
        out[key] = value
    return out


def script_rules(script_dir):
    """{('self', sprite): pct} and {('place', parent, depth): pct}, plus frame-2 sprites."""
    tints, second = {}, set()
    for p in script_dir.rglob('*.as'):
        text = p.read_text(encoding='utf-8', errors='ignore')
        parts = p.relative_to(script_dir).parts
        sprite = re.match(r'DefineSprite_(\d+)', parts[0])
        if not sprite:
            continue
        sid = int(sprite.group(1))
        if 'nextFrame' in text and re.search(r'_root\.Cinema\s*!==?\s*true', text):
            second.add(sid)
        if not (FORMULA.search(text) and 'setTransform' in text) or GUARD.search(text):
            continue
        if len(parts) >= 3 and parts[2].startswith('PlaceObject'):
            depth = int(parts[2].rsplit('_', 1)[1])
            tints[('place', sid, depth)] = star_tint()
        elif parts[-1] == 'DoAction.as':
            tints[('self', sid)] = star_tint()
    return tints, second


def cxform_mult(pct):
    """CXFORMWITHALPHA with only multiply terms (8.8 fixed), byte aligned."""
    m, n = round(pct / 100 * 256), 10
    bits = '0' + '1' + format(n, '04b') + ''.join(format(v & ((1 << n) - 1), f'0{n}b') for v in (m, m, m, 256))
    bits += '0' * (-len(bits) % 8)
    return bytes(int(bits[i:i + 8], 2) for i in range(0, len(bits), 8))


def tag(code, data):
    return struct.pack('<H', (code << 6) | min(len(data), 63)) + (struct.pack('<I', len(data)) if len(data) >= 63 else b'') + data


def place_with_tint(data, pct):
    """PlaceObject2 body with a multiply colour transform added (none present)."""
    flags = data[0]
    if flags & 0x08:
        raise ValueError('Placement already has a colour transform')
    q = 3 + (2 if flags & 0x02 else 0)
    if flags & 0x04:
        b = S.Bits(data, q); S.matrix(b); q = b.byte
    return bytes([flags | 0x08]) + data[1:q] + cxform_mult(pct) + data[q:]


def rewrite_sprite(raw, pos, length, ident, keep_frame, strip, tints, self_tinted):
    """Rewrite a DefineSprite: optionally collapse to frame `keep_frame` (dropping
    scripts and labels), drop stripped depths, and add star tints to placements."""
    out, shown, placed, frames = bytearray(), 0, {}, struct.unpack_from('<H', raw, pos + 2)[0]
    for code, p, n in S.tags(raw, pos + 4, pos + length):
        if keep_frame is not None:
            if code == 1:
                if shown == keep_frame:
                    break
                shown += 1
                continue
            if code in (0, 12, 43):
                continue
        elif code == 0:
            continue
        data = raw[p:p + n]
        if code in (26, 70):
            flags, depth = data[0], struct.unpack_from('<H', data, 2 if code == 70 else 1)[0]
            if depth in strip:
                continue
            if flags & 0x02:
                placed[depth] = struct.unpack_from('<H', data, 3 if code == 26 else 4)[0]
            child = placed.get(depth) if flags & 0x02 else None
            pct = tints.get(('place', ident, depth))
            if pct is None and child in self_tinted:
                pct = tints[('self', child)]
            if pct is not None and flags & 0x02:
                if code == 70:
                    raise ValueError('PlaceObject3 tint not supported')
                data = place_with_tint(data, pct)
        elif code == 28 and struct.unpack_from('<H', data)[0] in strip:
            continue
        out += tag(code, data)
    if keep_frame is not None:
        out += tag(1, b'')
        frames = 1
    out += tag(0, b'')
    return struct.pack('<HH', ident, frames) + bytes(out)


def patched(swf, plan, tints, second, sprites):
    raw, start = S.body(swf)
    self_tinted = {k[1] for k in tints if k[0] == 'self'}
    parents = {k[1] for k in tints if k[0] == 'place'}
    parents |= {i for i, sp in sprites.items() if any(e.get('id') in self_tinted for f in sp['stage'] for e in f.values())}
    out = bytearray(raw[:start])
    for code, pos, length in S.tags(raw, start, len(raw)):
        data = raw[pos:pos + length]
        if code == 39:
            ident = struct.unpack_from('<H', data)[0]
            if ident in plan:
                frame, strip = plan[ident]
                data = rewrite_sprite(raw, pos, length, ident, frame, strip, tints, self_tinted)
            elif ident in second:
                data = rewrite_sprite(raw, pos, length, ident, 1, set(), tints, self_tinted)
            elif ident in parents:
                data = rewrite_sprite(raw, pos, length, ident, None, set(), tints, self_tinted)
        out += tag(code, data)
    # FFDec draws non-smoothed bitmap fills half a texel off (swf_bitmap_fix.py).
    return swf_bitmap_fix.fix_swf(b'FWS' + swf[3:4] + struct.pack('<I', len(out) + 8) + bytes(out))


def rgb5a3(image):
    """GX RGB5A3 4x4 tiles, big-endian (opaque RGB555, else 3-bit alpha ARGB4443)."""
    a = np.asarray(image.convert('RGBA'), dtype=np.uint16)
    h, w = a.shape[:2]
    r, g, b, al = a[..., 0], a[..., 1], a[..., 2], a[..., 3]
    opaque = (0x8000 | ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3)).astype(np.uint16)
    trans = (((al >> 5) << 12) | ((r >> 4) << 8) | ((g >> 4) << 4) | (b >> 4)).astype(np.uint16)
    px = np.where(al >= 0xE0, opaque, trans).astype('>u2')
    tiles = px.reshape(h // 4, 4, w // 4, 4).transpose(0, 2, 1, 3)
    return tiles.tobytes()


def render(jar, swf_path, work, ids, zoom):
    cmd = ['java', '-Xmx3g', '-Djava.awt.headless=true', f'-Duser.home={work.resolve()}/ffdec-home', '-jar',
           str(jar.resolve())]
    sel = ['-selectid', ','.join(map(str, ids)), '-select', ','.join(f'{i}:1' for i in ids)]
    with (work / 'export.log').open('a') as log:
        subprocess.run(cmd + swf_bitmap_fix.FFDEC_RENDER + ['-zoom', str(zoom), '-onerror', 'abort'] + sel +
                       ['-export', 'sprite', str(work / 'png'),
                       str(swf_path)], check=True, stdout=log, stderr=log)
        subprocess.run(cmd + ['-onerror', 'abort'] + sel + ['-format', 'sprite:svg', '-export', 'sprite',
                       str(work / 'svg'), str(swf_path)], check=True, stdout=log, stderr=log)


def origin(work, ident):
    svg = ET.parse(C.sprite_dir(work / 'svg', ident) / '1.svg').getroot()
    t = [float(v) for v in re.findall(r'[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?',
                                      svg.find('{http://www.w3.org/2000/svg}g').get('transform'))]
    if t[:4] != [1, 0, 0, 1]:
        raise ValueError('Unexpected registration')
    return -t[4], -t[5]


def clean(image, repair=True):
    """FFDec seam/crack repair (course layers only: the picture-sign pages are
    line drawings), then colour for transparent texels (GX filters
    bilinearly); see art_repair.py."""
    a = np.asarray(image.convert('RGBA'))
    if repair:
        a, _ = art_repair.repair(a)
    return Image.fromarray(art_repair.bleed(a))


def encode_layer(image, repair=True):
    """Pad to multiples of 4, cut into <=1024 tiles, drop empty tiles."""
    image = clean(image, repair)
    w, h = image.size
    W, H = -(-w // 4) * 4, -(-h // 4) * 4
    canvas = Image.new('RGBA', (W, H)); canvas.paste(image, (0, 0))
    tiles = []
    for ty in range(0, H, TILE):
        for tx in range(0, W, TILE):
            part = canvas.crop((tx, ty, min(W, tx + TILE), min(H, ty + TILE)))
            if part.getchannel('A').getbbox() is None:
                continue
            data = rgb5a3(part)
            tiles.append((tx, ty, part.width, part.height, lz4.block.compress(data, store_size=False), len(data)))
    return W, H, tiles


def export(source, jar, output, areas):
    swf_file = source / 'Publish/Super_Mario_63_2010.swf'
    swf = swf_file.read_bytes()
    if hashlib.sha256(swf).hexdigest() != C.SOURCE_SHA:
        raise ValueError('Unsupported SWF revision')
    xfl.LIB = source / 'Super_Mario_63_2010/LIBRARY'
    raw, start = S.body(swf)
    table, sprites = C.exports(raw, start), S.sprites(swf)
    tables = course_tables((source / 'Super_Mario_63_2010/DOMDocument.xml').read_text(encoding='utf-8'))
    work = output.parent / 'rendered-course-art'
    work.mkdir(parents=True, exist_ok=True)
    scripts = work / 'scripts'
    if not scripts.exists():
        with (work / 'export.log').open('a') as log:
            subprocess.run(['java', '-Xmx3g', '-Djava.awt.headless=true', f'-Duser.home={work.resolve()}/ffdec-home',
                            '-jar', str(jar.resolve()), '-onerror', 'ignore', '-export', 'script', str(scripts),
                            str(swf_file)], check=True, stdout=log, stderr=log)
    tints, second = script_rules(scripts / 'scripts')
    plan, layers = {}, []
    for a in areas:
        for layer in LAYERS:
            link = tables.get(a + layer) if 'Background' in layer else a + layer
            if not link or link not in table:
                continue
            ident = table[link]
            if layer in ('BackGFX', 'FrontGFX'):
                frame = 2 if layer == 'BackGFX' and sprites[ident]['frames'] > 2 else 0
                stage = sprites[ident]['stage'][frame]
                objs = [o for o in C.objects(a, layer) if o['frames'][0] <= frame < o['frames'][1]]
                strip = set()
                for o in objs:
                    for d, e in stage.items():
                        m = e.get('matrix') or {}
                        if abs(m.get('tx', 0) / 20 - o['x']) < 0.06 and abs(m.get('ty', 0) / 20 - o['y']) < 0.06:
                            strip.add(d)
                plan[ident] = (frame, strip)
            else:
                plan.setdefault(ident, (0, set()))
            layers.append((a, layer, ident, link))
    swf_path = work / 'patched.swf'
    swf_path.write_bytes(patched(swf, plan, tints, second, sprites))
    ids = sorted({l[2] for l in layers})
    render(jar, swf_path, work, ids, ZOOM)
    out = bytearray(struct.pack('>8sIIf', b'SM63CAR1', 1, len(layers), ZOOM))
    report = []
    for a, layer, ident, link in layers:
        im = Image.open(C.sprite_dir(work / 'png', ident) / '1.png').convert('RGBA')
        ox, oy = origin(work, ident)
        W, H, tiles = encode_layer(im)
        out += C.pack_string(a) + C.pack_string(layer) + struct.pack('>ffffII', ox, oy, PARALLAX[layer], ZOOM, W, H)
        out += struct.pack('>I', len(tiles))
        for tx, ty, tw, th, blob, size in tiles:
            out += struct.pack('>HHHHII', tx, ty, tw, th, size, len(blob)) + blob
            out += bytes((-len(out)) % 4)
        report.append({'area': a, 'layer': layer, 'sprite': ident, 'linkage': link, 'origin': [ox, oy],
                       'size': [W, H], 'tiles': len(tiles), 'bytes': sum(len(t[4]) for t in tiles),
                       'raw': sum(t[5] for t in tiles), 'stripped': len(plan.get(ident, (0, set()))[1])})
    # Picture-sign pages (Message Picture - Pictures: the sprite whose frame labels
    # include every page a Coded Sign in these areas asks for).
    labels = sorted({o['params']['signmessage'] for a in areas for l in ('BackGFX',) for o in C.objects(a, l)
                     if o['type'] == 'Coded Sign' and o['params'].get('picture') == 'true'})
    pic = [i for i, sp in sprites.items() if set(labels) <= set(sp['labels']) and 'controls' in sp['labels']]
    if len(pic) != 1:
        raise ValueError(f'Picture sprite not unique: {pic}')
    pages = work / 'pictures'
    pictures_swf = work / 'pictures.swf'
    pictures_swf.write_bytes(swf_bitmap_fix.fix_swf(swf))
    cmd = ['java', '-Xmx3g', '-Djava.awt.headless=true', f'-Duser.home={work.resolve()}/ffdec-home', '-jar',
           str(jar.resolve())] + swf_bitmap_fix.FFDEC_RENDER + ['-zoom', str(ZOOM), '-onerror', 'abort', '-selectid', str(pic[0]), '-select',
           ','.join(f'{pic[0]}:{sprites[pic[0]]["labels"][l] + 1}' for l in labels), '-export', 'sprite', str(pages),
           str(pictures_swf)]
    with (work / 'export.log').open('a') as log:
        subprocess.run(cmd, check=True, stdout=log, stderr=log)
    # Wii controller versions of the keyboard pages (controller_signs.py): the
    # page's background shape, rendered alone, is what keyboard parts clear to.
    shapes = sorted(set(controller_signs.BACKGROUND.values()))
    for label, shape in controller_signs.BACKGROUND.items():
        frame = sprites[pic[0]]['stage'][sprites[pic[0]]['labels'][label]]
        if shape not in {e.get('id') for e in frame.values()}:
            raise ValueError(f'Picture page {label} does not place shape {shape}')
    with (work / 'export.log').open('a') as log:
        subprocess.run(cmd[:cmd.index('-selectid')] + ['-selectid', ','.join(map(str, shapes)), '-format', 'shape:png',
                                                         '-export', 'shape', str(work / 'picture-shapes'),
                                                         str(pictures_swf)], check=True, stdout=log, stderr=log)
    # Page placement: Stats.Picture (620) places the page sprite at stage (225, 150);
    # the canvas origin comes from the frame's SVG (the same for every frame).
    psvg = ui_art.ffdec(jar, work, pictures_swf, pic[0], [1], work / 'pictures-svg', svg=True)
    with Image.open(C.sprite_dir(pages, pic[0]) / '1.png') as p1:   # the PNG canvas is padded around filters
        pox, poy = ui_art.png_origin(psvg / '1.svg', p1.size)
    # The keys removed from the render itself (controller_signs.HIDE), the
    # bitmaps the new pages reuse, and the boxes' ground-pound demo (424).
    # Animated pages (sign_anim.py): the page without its moving parts, which
    # play as frames over it; the controller pages are built on that still page.
    still = work / 'pictures-still.swf'
    still.write_bytes(ui_art.hide(pictures_swf.read_bytes(), sign_anim.ANIM_HIDE))
    anim_labels = [l for l in labels if l in sign_anim.SPEC]
    st = ui_art.ffdec(jar, work, still, pic[0], [sprites[pic[0]]['labels'][l] + 1 for l in anim_labels],
                      work / 'pictures-still') if anim_labels else None
    keyless = work / 'pictures-keyless.swf'
    keyless.write_bytes(ui_art.hide(still.read_bytes(), controller_signs.HIDE))
    kl = ui_art.ffdec(jar, work, keyless, pic[0], [sprites[pic[0]]['labels'][l] + 1 for l in ('controls', 'stomp')],
                      work / 'pictures-keyless')
    with (work / 'export.log').open('a') as log:
        subprocess.run(cmd[:cmd.index('-selectid')] + ['-selectid', f'{controller_signs.DIVE_BITMAP},{controller_signs.EXPLOSION_BITMAP}',
                                                         '-format', 'image:png', '-export', 'image', str(work / 'picture-bitmaps'),
                                                         str(pictures_swf)], check=True, stdout=log, stderr=log)
    sign_art = {'dive': Image.open(work / 'picture-bitmaps' / f'{controller_signs.DIVE_BITMAP}.png').convert('RGBA'),
                'explosion': Image.open(work / 'picture-bitmaps' / f'{controller_signs.EXPLOSION_BITMAP}.png').convert('RGBA')}
    variants, anims = [], []
    for label in labels:
        im = Image.open(C.sprite_dir(pages, pic[0]) / f'{sprites[pic[0]]["labels"][label] + 1}.png').convert('RGBA')
        bb = im.getchannel('A').getbbox()
        im = im.crop(bb)  # the page itself; (x, y) its stage position
        x, y = 225 + pox + bb[0] / ZOOM, 150 + poy + bb[1] / ZOOM
        if label in sign_animated(labels):
            frames = sign_anim.build(work, jar, pictures_swf.read_bytes(), sprites, label, im,
                                     (pox + bb[0] / ZOOM, poy + bb[1] / ZOOM))
            anims.append((label + '~', frames, x, y, -(-im.width // 4) * 4, -(-im.height // 4) * 4))
            im = Image.open(st / f'{sprites[pic[0]]["labels"][label] + 1}.png').convert('RGBA').crop(bb)
        variants.append((label, im, x, y))
        if label in ('controls', 'stomp'):
            sign_art[label] = Image.open(kl / f'{sprites[pic[0]]["labels"][label] + 1}.png').convert('RGBA').crop(bb)
        if label in controller_signs.PAGES:
            bg = Image.open(work / 'picture-shapes' / f'{controller_signs.BACKGROUND[label]}.png').convert('RGBA')
            for k, kind in enumerate(controller_signs.CONTROLLERS):
                page = controller_signs.make(label, im, bg, kind, sign_art)
                page.save(work / 'picture-shapes' / f'{label}@{k}.png')
                variants.append((f'{label}@{k}', page, x, y))
    # The message box and its controller prompts (ui_art.py); origins in stage px.
    ui_layers, ui_header = ui_art.build(work / 'ui', jar, pictures_swf.read_bytes(), sprites)
    (output.parent / 'ui_layout.h').write_text(ui_header)
    # The 0 HP faint's screens (faint_art.py): star wipe out / in, the lives screen.
    faint_layers, faint_header = faint_art.build(work / 'faint', jar, pictures_swf.read_bytes(), sprites)
    (output.parent / 'faint_layout.h').write_text(faint_header)
    anims += [(n, fr, ox, oy, W, H, s) for n, fr, ox, oy, W, H, s in faint_layers]
    variants += list(ui_layers)
    count = struct.unpack_from('>I', out, 12)[0]
    for label, im, ox, oy in variants:
        W, H, tiles = encode_layer(im, repair=False)
        out += C.pack_string('*') + C.pack_string(label) + struct.pack('>ffffII', ox, oy, 0, ZOOM, W, H)
        out += struct.pack('>I', len(tiles))
        for tx, ty, tw, th, blob, size in tiles:
            out += struct.pack('>HHHHII', tx, ty, tw, th, size, len(blob)) + blob
            out += bytes((-len(out)) % 4)
        count += 1
        report.append({'area': '*', 'layer': label, 'size': [W, H], 'tiles': len(tiles),
                       'bytes': sum(len(t[4]) for t in tiles), 'raw': sum(t[5] for t in tiles)})
    for label, frames, ox, oy, W, H, *scale in anims:   # one tile per frame, at its place on the page
        out += C.pack_string('*') + C.pack_string(label) + struct.pack('>ffffII', ox, oy, 0, scale[0] if scale else ZOOM, W, H)
        out += struct.pack('>I', len(frames))
        for fx, fy, fim in frames:
            data = rgb5a3(fim)
            blob = lz4.block.compress(data, store_size=False)
            out += struct.pack('>HHHHII', fx, fy, fim.width, fim.height, len(data), len(blob)) + blob
            out += bytes((-len(out)) % 4)
        count += 1
        report.append({'area': '*', 'layer': label, 'size': [W, H], 'tiles': len(frames),
                       'bytes': 0, 'raw': 0})
    out[12:16] = struct.pack('>I', count)
    out += bytes((-len(out)) % 32)
    output.write_bytes(bytes(out))
    (work / 'art.json').write_text(json.dumps({'layers': report, 'tints': {str(k): v for k, v in tints.items()},
                                               'frame2': sorted(second)}, indent=1))
    return report


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('source', type=Path); p.add_argument('ffdec_jar', type=Path); p.add_argument('output', type=Path)
    p.add_argument('areas', nargs='+')
    a = p.parse_args()
    for r in export(a.source, a.ffdec_jar, a.output, a.areas):
        print(r['area'], r['layer'], r['size'], r['tiles'], 'tiles', r['bytes'] // 1024, 'KiB lz4 /', r['raw'] // 1024,
              'KiB', 'stripped', r.get('stripped', 0))
