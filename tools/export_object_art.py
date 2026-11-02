#!/usr/bin/env python3
"""Render the original tutorial objects' animations into compressed art packs.

Each clip is a DefineSprite rendered by FFDec frame by frame (nested clips
advance with the timeline, as Flash plays them), offset by where the object
places it, with the invisible `Blank Movieclip` hit boxes (SWF 839 and 896)
removed. Clip ids are generated into `assets/generated/object_art_ids.h`.
Art is rendered at ZOOM (1.6 screen px per course px) and packed as art pack
v3 (GX RGB5A3, LZ4); clips are split over several 1024x1024 packs as needed.

    python export_object_art.py <checkout> <ffdec.jar> <out_dir>
"""
import argparse, hashlib, json, re, struct, subprocess
from pathlib import Path
from PIL import Image
import swf_states as S
import export_course as C
import export_course_art as A
from build_player_art import build, _pack
import numpy as np
import art_repair
import swf_bitmap_fix

ZOOM = 1.6
BLANKS = {839, 896}
# (clip name, sprite id, frames (None = all), step, placement (tx, ty, sx, sy) in
#  the object, depths to remove). Placement is the child's matrix in the object.
# Sources: SWF structure, see docs/course-objects.md.
SPEC = [
    ('COIN_SPIN', 17, None, 1, None, ()),             # GoldCoin 963 d4 'clip'
    ('COIN_COLLECT', 963, range(3, 17), 1, (0, 0, 1, 1), ()),  # frames 4..17
    ('BCOIN_SPIN', 3485, None, 1, None, ()),          # BlueCoin 3495 d4 'clip'
    ('BCOIN_COLLECT', 3495, range(3, 17), 1, (0, 0, 1, 1), ()),
    ('SIGN', 986, [0], 1, (0, 0, 1, 1), (3,)),                 # without 'aa' (d3)
    ('SIGN_Z', 981, None, 1, None, ()),                        # 'aa' prompt, placed from 986
    ('BUTTERFLY', 2323, None, 1, None, ()),           # ButterflyCode 'clip'
    ('GOONIE_FLY', 2300, None, 1, (0, 0, 1, 1), ()),          # GoonieCode frame 'fly'
    ('GOONIE_RIDE', 2301, None, 1, (0, 0, 1, 1), ()),         # frame 'ride'
    ('GOONIE_DIVE', 2304, None, 1, (0, 8, 1, 1), ()),         # frame 'dive'
    ('TOAD_IDLE', 2501, None, 4, None, (2,)),         # Toad 'body' (sampled 1/4), its 'head' (d2) drawn apart
    ('TOAD_WALK', 2516, None, 1, None, (2,)),         # Toad Main Clip frame 'walk' body, without 'head'
    # Toad 'head' (2498): mood segments Smile 1, Talk 69, Sad 99, Mad 160, Shocked 185,
    # Happy 270, Confused 299, Crazy 360 (frames 1-397); placed per body frame at runtime.
    ('TOAD_HEAD', 2498, list(range(0, 397)), 1, (0, 0, 1, 1), ()),
    ('TOAD_BZ', 2505, None, 3, None, ()),                      # Toad 'bz' talk prompt
    ('LOG', 3317, [0], 1, (0, 0, 1, 1), ()),                  # Falling Log (graphics)
    ('ARROW', 3410, None, 1, (0, 0, 1, 1), ()),               # YI Arrow Animation
    ('GOOMBA_IDLE', 2380, [0], 1, None, ()),                   # Goomba 'clip' frame Idle
    ('GOOMBA_JUMP', 2344, None, 1, None, ()),
    ('GOOMBA_WALKR', 2360, None, 1, None, ()),
    ('GOOMBA_WALKL', 2369, None, 1, None, ()),
    ('GOOMBA_SQUISH', 2377, None, 1, None, ()),
    ('GOOMBA_TUMBLE', 2379, None, 1, None, ()),
    ('CHEEP_SWIM', 2216, None, 1, None, ()),
    ('CHEEP_CHASE', 2217, None, 1, None, ()),
    ('CHEEP_TURN', 2220, None, 1, None, ()),
    ('CHEEP_JUMP', 2229, None, 1, None, ()),
    ('CHEEP_SPIN', 2230, None, 1, None, ()),
    ('CHEEP_GONE', 2232, None, 1, None, ()),
    ('BOMB_WALK', 3280, None, 1, None, ()),
    ('BOMB_TUMBLE', 3282, None, 1, None, ()),
    ('BOMB_EXPLODE', 3294, None, 1, None, ()),
    ('BOMB_EXPLODE2', 3309, None, 1, None, ()),
    ('KOOPA_WALK', 3367, None, 1, None, ()),
    ('KOOPA_TURN', 3377, None, 1, None, ()),
    ('KOOPA_FLY', 3396, None, 1, None, ()),            # Koopa clip frame 'Flying' (Paratroopa)
    ('KOOPA_SHELL', 3401, None, 1, None, ()),
    ('BOX', 2808, None, 1, (0, 0, 1, 1), (1,)),               # Breakable box; 'stuff' (d1) is "none" at blocknum 1
    ('FLUDD_BOX', 972, None, 1, (0, 0, 1, 1), ()),            # Hover FLUDD box
    ('BOTTLE', 3995, None, 1, (0, 0, 1, 1), ()),
    ('GREEN_PL', 951, None, 1, (0, 0, 1, 1), ()),             # Green Circle Platform 'pl' (frame = scale)
    ('PATH_DOT', 28, None, 1, (0, 0, 1, 1), ()),              # attachMovie("Path dot") around the circle
    ('HNOZZLE', 42, None, 1, (0, 0, 1, 1), ()),               # Hover nozzle pickup from the FLUDD box
    ('TIP_LOG', 3476, None, 1, (0, 0, 1, 1), ()),
    ('ROT_BLOCK', 3063, [0, 1], 1, (0, 0, 1, 1), ()),        # frame = blocktype (tutorial uses 1, 2)
    # Stats Clip (800) frame 1 without its dynamic parts: the shine and star coin
    # icons with their "=" (the coin counter's "=" is HUD_DIGITS frame 10, so it
    # can sit at the right edge on 16:9).
    ('HUD_STATIC', 800, [0], 1, (0, 0, 1, 1),
     (1, 3, 9, 45, 65, 70, 71, 72, 74, 75, 76, 78, 82, 84, 86, 98, 104, 110, 112, 113, 118, 120, 122)),
    ('HUD_PIE', 111, None, 1, (0, 0, 1, 1), ()),               # POWER meter, frame = CharHP + 1
    ('HUD_TANK', 160, [0], 1, (0, 0, 1, 1), (6, 32, 35)),      # FLUDD tank without level, gauge, number
    ('HUD_TANK_LEVEL', 149, None, 10, (1.1, 30.5, 0.5, 0.5), ()),  # 'cl', frame = Waterpercent * 5
    ('HUD_POWER_BAR', 153, None, 5, (-17.35, -14.85, 1, 1), ()),   # frame = Power / TotalPower * 100 + 1
    ('HUD_TALK_BOX', 186, [0], 1, (0, 0, 1, 1), (2, 4)),       # 'tm' Toad message cloud (text drawn by the port)
    ('HUD_TALK_Z', 184, None, 1, (143.45, 1, 1.5, 1.5), ()),   # its 'bz' Z prompt
    # Shine Sprite 977/976: only on 8-E1-2-2 frames reached once Star[39] is set,
    # so a new save never shows it in the tutorial; not exported.
]


def child_matrix(sprites, parent, child):
    for f in sprites[parent]['stage']:
        for d, e in f.items():
            if e.get('id') == child and e.get('matrix'):
                m = e['matrix']
                return (m['tx'] / 20, m['ty'] / 20, m['sx'], m['sy'])
    raise ValueError(f'{child} not placed in {parent}')


PARENTS = {2516: 2517, 17: 963, 3485: 3495, 2323: 2324, 2501: 2517, 981: 986, 2505: 2517, 2380: 2386, 2344: 2380, 2360: 2380, 2369: 2380, 2377: 2380, 2379: 2380,
           2385: 2386, 2216: 2233, 2217: 2233, 2220: 2233, 2229: 2233, 2230: 2233, 2232: 2233,
           3280: 3310, 3282: 3310, 3294: 3310, 3309: 3310, 3367: 3402, 3377: 3402, 3396: 3402, 3401: 3402, 976: 977}
# Chains whose own placement in the object must also be applied.
GRAND = {2380: 2386, 2233: 2234, 3310: 3311, 3402: 3403}


def placement(sprites, ident):
    """Accumulated (tx, ty, sx, sy) from the object origin to sprite `ident`."""
    tx, ty, sx, sy = 0.0, 0.0, 1.0, 1.0
    chain = []
    while ident in PARENTS:
        chain.append((PARENTS[ident], ident)); ident = PARENTS[ident]
    while ident in GRAND:
        chain.append((GRAND[ident], ident)); ident = GRAND[ident]
    for parent, child in reversed(chain):
        ctx, cty, csx, csy = child_matrix(sprites, parent, child)
        tx, ty = tx + ctx * sx, ty + cty * sy
        sx, sy = sx * csx, sy * csy
    return tx, ty, sx, sy


def strip_blanks(raw, start, strip_depths):
    """Remove Blank Movieclip placements everywhere and per-sprite depths."""
    out = bytearray(raw[:start])
    for code, pos, length in S.tags(raw, start, len(raw)):
        data = raw[pos:pos + length]
        if code == 39:
            ident, frames = struct.unpack_from('<HH', data)
            body, blank = bytearray(), set()
            drop = set(strip_depths.get(ident, ()))
            for c, p, n in S.tags(raw, pos + 4, pos + length):
                d = raw[p:p + n]
                if c in (26, 70):
                    flags = d[0]
                    depth = struct.unpack_from('<H', d, 2 if c == 70 else 1)[0]
                    if flags & 0x02:
                        cid = struct.unpack_from('<H', d, 3 if c == 26 else 4)[0]
                        if cid in BLANKS: blank.add(depth)
                        else: blank.discard(depth)
                    if depth in blank or depth in drop:
                        continue
                elif c == 28 and struct.unpack_from('<H', d)[0] in blank | drop:
                    continue
                body += A.tag(c, d)
            data = struct.pack('<HH', ident, frames) + bytes(body)
        out += A.tag(code, data)
    return out


def digits(jar, swf_path, work, cid):
    """HUD counters: DefineEditText fields in font 187 (Arial, 16 px), a black
    50% copy 2 px down-right under the white one. Frames 0-9 are the digits,
    drawn from the text field's top-left (2 px gutter); frame 10 is "="."""
    from PIL import ImageDraw, ImageFont
    font_dir = work / 'font'
    if not list(font_dir.glob('187_*.ttf')):
        subprocess.run(['java', '-Djava.awt.headless=true', '-jar', str(jar.resolve()), '-selectid', '187', '-format',
                        'font:ttf', '-export', 'font', str(font_dir), str(swf_path)], check=True, capture_output=True)
    font = ImageFont.truetype(str(next(font_dir.glob('187_*.ttf'))), round(16 * ZOOM))
    out_png = work / 'digits'
    out_png.mkdir(exist_ok=True)
    frames = []
    for d in '0123456789=':
        w = round(14 * ZOOM); h = round(24 * ZOOM)
        im = Image.new('RGBA', (w, h))
        dr = ImageDraw.Draw(im)
        # FFDec draws these fields bold with a dark edge; a stroke reproduces that look.
        dr.text((round(2 * ZOOM) + 2, round(2 * ZOOM) + 2), d, font=font, fill=(0, 0, 0, 128), stroke_width=2,
                stroke_fill=(0, 0, 0, 128))
        dr.text((2, 2), d, font=font, fill=(255, 255, 255, 255), stroke_width=2, stroke_fill=(0, 0, 0, 255))
        path = out_png / f'{d}.png'
        im.save(path)
        frames.append({'file': str(path.resolve()), 'duration': 1, 'offset_twips': [round(2 * 20), round(2 * 20)]})
    return {'id': cid, 'name': 'HUD_DIGITS', 'loop': False, 'frames': frames}


def export(source, jar, out_dir):
    swf_file = source / 'Publish/Super_Mario_63_2010.swf'
    swf = swf_file.read_bytes()
    if hashlib.sha256(swf).hexdigest() != C.SOURCE_SHA:
        raise ValueError('Unsupported SWF revision')
    raw, start = S.body(swf)
    sprites = S.sprites(swf)
    work = Path(__file__).resolve().parent.parent / 'private-art' / 'rendered-objects'
    work.mkdir(parents=True, exist_ok=True)
    strip = {}
    for name, ident, frames, step, place, depths in SPEC:
        if depths: strip.setdefault(ident, set()).update(depths)
    patched = swf_bitmap_fix.fix_body(strip_blanks(raw, start, strip), start)  # FFDec half-texel offset
    swf_path = work / 'objects.swf'
    swf_path.write_bytes(b'FWS' + swf[3:4] + struct.pack('<I', len(patched) + 8) + bytes(patched))
    ids = sorted({s[1] for s in SPEC})
    cmd = ['java', '-Xmx3g', '-Djava.awt.headless=true', f'-Duser.home={work.resolve()}/ffdec-home', '-jar',
           str(jar.resolve())]
    with (work / 'export.log').open('a') as log:  # every frame (PNG), first frame registration (SVG)
        subprocess.run(cmd + swf_bitmap_fix.FFDEC_RENDER + ['-zoom', str(ZOOM), '-onerror', 'abort', '-selectid',
                              ','.join(map(str, ids)), '-export', 'sprite', str(work / 'png'), str(swf_path)],
                       check=True, stdout=log, stderr=log)
        subprocess.run(cmd + ['-onerror', 'abort', '-selectid', ','.join(map(str, ids)), '-select',
                              ','.join(f'{i}:1' for i in ids), '-format', 'sprite:svg', '-export', 'sprite',
                              str(work / 'svg'), str(swf_path)], check=True, stdout=log, stderr=log)
    clips, report = [], []
    for cid, (name, ident, frames, step, place, depths) in enumerate(SPEC):
        tx, ty, sx, sy = place if place else placement(sprites, ident)
        if sx <= 0 or sy <= 0:
            raise ValueError(f'{name}: mirrored placement {sx},{sy}')
        ox, oy = A.origin(work, ident)  # course px of the canvas (0,0) in the sprite
        # FFDec pads the PNG canvas around filters (Toad's head, the Stats clip):
        # the SVG is the unpadded frame, so move the origin by half the difference.
        svg_text = (C.sprite_dir(work / 'svg', ident) / '1.svg').read_text()[:2000]
        sw = float(re.search(r'width="([\d.]+)px"', svg_text).group(1))
        sh = float(re.search(r'height="([\d.]+)px"', svg_text).group(1))
        with Image.open(C.sprite_dir(work / 'png', ident) / '1.png') as im0:
            mx, my = (im0.width / ZOOM - sw) / 2, (im0.height / ZOOM - sh) / 2
        if mx > 1 or my > 1:
            ox, oy = ox - mx, oy - my
        count = sprites[ident]['frames']
        picks = list(frames) if frames is not None else list(range(count))
        picks = picks[::step]
        out, repaired_px = [], 0
        for k in picks:
            png = C.sprite_dir(work / 'png', ident) / f'{k + 1}.png'
            # FFDec cracks (e.g. the transparent column inside the sign board): art_repair.py
            fixed, changed = art_repair.repair(np.asarray(Image.open(png).convert('RGBA')))
            repaired_px += changed
            png = work / 'repaired' / f'{name}_{k}.png'
            png.parent.mkdir(exist_ok=True)
            Image.fromarray(fixed).save(png)
            if abs(sx - 1) > 0.01 or abs(sy - 1) > 0.01:  # child placed scaled: resample once
                im = Image.open(png).convert('RGBA')
                scaled = work / 'scaled' / f'{name}_{k}.png'
                scaled.parent.mkdir(exist_ok=True)
                im.resize((max(1, round(im.width * sx)), max(1, round(im.height * sy))), Image.LANCZOS).save(scaled)
                png = scaled
            out.append({'file': str(png.resolve()), 'duration': step,
                        'offset_twips': [round((ox * sx + tx) * 20), round((oy * sy + ty) * 20)]})
        clips.append({'id': cid, 'name': name, 'loop': frames is None, 'frames': out})
        report.append({'name': name, 'sprite': ident, 'frames': len(out), 'placement': [tx, ty],
                       'repaired_px': repaired_px})
    clips.append(digits(jar, swf_path, work, len(clips)))
    report.append({'name': 'HUD_DIGITS', 'font': 187})
    # Split into packs that each fit one 1024x1024 texture.
    packs, current = [], []
    for clip in clips:
        trial = current + [clip]
        try:
            build({'clips': [dict(c, frames=[dict(f, offset_twips=[f['offset_twips'][0] * 0, 0]) for f in c['frames']])
                             for c in trial]}, Path('/'), compressed=False, px_twips=20 / ZOOM)
            current = trial
        except ValueError:
            packs.append(current); current = [clip]
    packs.append(current)
    header = ['/* Generated by tools/export_object_art.py; do not edit. */', '#ifndef SM63_OBJECT_ART_IDS_H',
              '#define SM63_OBJECT_ART_IDS_H', f'#define OBJECT_ART_ZOOM {ZOOM}f',
              f'#define OBJECT_ART_PACKS {len(packs)}', 'enum {']
    for p, pack in enumerate(packs):
        blob = build({'clips': pack}, Path('/'), compressed=True, px_twips=20 / ZOOM)
        (out_dir / f'objects_8e1_{p}.bin').write_bytes(blob)
        for c in pack:
            header.append(f'    OBJ_{c["name"]} = {c["id"]}, /* pack {p} */')
    header += ['    OBJ_CLIP_COUNT = %d' % len(clips), '};',
               '/* Toad head (2498) registration per TOAD_IDLE / TOAD_WALK frame, course px from',
               '   the Toad Main Clip origin (the body placement plus the head matrix in it). */']
    for name, ident, step in (('idle', 2501, 4), ('walk', 2516, 1)):
        btx, bty, _, _ = placement(sprites, ident)
        last, rows = {}, []
        for n, f in enumerate(sprites[ident]['stage']):
            for d, e in f.items():
                if e.get('matrix') or d not in last:
                    last[d] = e
            if n % step:
                continue
            m = next(e for e in last.values() if e.get('name') == 'head')['matrix']
            rows.append(f'{{{btx + m["tx"] / 20:.2f}f, {bty + m["ty"] / 20:.2f}f}}')
        header.append(f'static const float toad_head_{name}[][2] = {{' + ', '.join(rows) + '};')
    header += [ 'static const unsigned char object_art_pack_of[] = {',
               '    ' + ', '.join(str(next(p for p, pk in enumerate(packs) if any(c['id'] == i for c in pk)))
                                  for i in range(len(clips))), '};', '#endif', '']
    (out_dir / 'object_art_ids.h').write_text('\n'.join(header))
    (work / 'objects.json').write_text(json.dumps(report, indent=1))
    return packs, report


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('source', type=Path); p.add_argument('ffdec_jar', type=Path); p.add_argument('out_dir', type=Path)
    a = p.parse_args()
    packs, report = export(a.source, a.ffdec_jar, a.out_dir)
    for i, pk in enumerate(packs):
        print(f'pack {i}: {len(pk)} clips, {sum(len(c["frames"]) for c in pk)} frames')
