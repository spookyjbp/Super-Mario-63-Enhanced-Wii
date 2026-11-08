#!/usr/bin/env python3
"""The tutorial's level water (the `<area>Water` clips), for the port to draw
as the original does (assets/generated/water_places.h):

* every Water Code (Dynamic Water, sprite 802) placement: a spring strip of
  `polygons` points (BGquality "MEDIUM", the game's default: 3/4 of them,
  Math.round) from x -50 spanning WaterLength + 100, filled down to
  WaterDeepness. Its onClipEvent(load) values over 802's defaults; `graphics =
  true` turns off Mario's splashes on it.
* where the strip is a mask (8-E1-1-2's and 8-E1-2's Water clips put it on a
  mask layer over a water block), its own fill is not drawn: the block shows
  through it instead. A water block (Smaller Water Block 6284, Giant Water
  Block 2065) is two copies of one 800 x 500 shape (6283 / 2064 holding 6282 /
  2063) filled with the 32 x 32 bitmap 2062 repeated at 2x, each at 30 % alpha,
  sliding diagonally in opposite directions over 133 frames (one mirrored in
  the giant block). Their per-frame offsets and the pattern origin are listed;
  the bitmap is `water_tile` (RGB5A3).
Without a mask the strip is filled with its colour at its alpha (40, 128, 215
at 50 % by default), as in 8-E1-1.
"""
import base64, math, re, struct, subprocess
from pathlib import Path
import numpy as np
from PIL import Image
import swf_states as S

WATER = 802
DEFAULTS = {'WaterDeepness': 400, 'WaterLength': 500, 'polygons': 50, 'randomwave': 1, 'waterstength': 0.3,
            'redcolor': 40, 'greencolor': 128, 'bluecolor': 215, 'alpha': 50}


def as_round(v):
    """Math.round: halves round up."""
    return int(np.floor(v + 0.5))


def params(scripts, sprite, depth):
    vals, graphics = dict(DEFAULTS), False
    for d in scripts.glob(f'DefineSprite_{sprite}*/frame_1/PlaceObject*_{WATER}_{depth}'):
        for f in d.glob('*.as'):
            text = f.read_text(errors='ignore')
            for k, v in re.findall(r'(\w+)\s*=\s*([-\d.]+)\s*;?', text):
                if k in vals:
                    vals[k] = float(v)
            graphics = graphics or re.search(r'graphics\s*=\s*true', text) is not None
    return vals, graphics


def placements(raw, start, ident):
    """[(depth, character, clip_depth, cxform alpha multiplier)] of sprite `ident`'s PlaceObject2 tags."""
    out = []
    for code, pos, length in S.tags(raw, start, len(raw)):
        if code != 39 or struct.unpack_from('<H', raw, pos)[0] != ident:
            continue
        for c, p, n in S.tags(raw, pos + 4, pos + length):
            if c != 26:
                continue
            flags = raw[p]
            depth = struct.unpack_from('<H', raw, p + 1)[0]
            q = p + 3
            char = None
            if flags & 0x02:
                char = struct.unpack_from('<H', raw, q)[0]; q += 2
            if flags & 0x04:
                b = S.Bits(raw, q); S.matrix(b); q = b.byte
            alpha = None
            if flags & 0x08:
                b = S.Bits(raw, q)
                has_add, has_mult, nb = b.ub(1), b.ub(1), b.ub(4)
                if has_mult:
                    mult = [b.sb(nb) for _ in range(4)]
                    alpha = mult[3] / 256
                if has_add:
                    for _ in range(4): b.sb(nb)
                b.align(); q = b.byte
            if flags & 0x10:
                q += 2
            if flags & 0x20:
                q = raw.index(0, q) + 1
            clip = struct.unpack_from('<H', raw, q)[0] if flags & 0x40 else 0
            out.append((depth, char, clip, alpha))
        return out
    return out


def pattern(jar, work, swf_path, shape):
    """(bitmap RGBA, origin x, origin y, scale) of a shape's repeating bitmap fill (FFDec SVG)."""
    d = work / 'water-svg'
    subprocess.run(['java', '-Xmx2g', '-Djava.awt.headless=true', f'-Duser.home={work.resolve()}/ffdec-home', '-jar',
                    str(jar.resolve()), '-selectid', str(shape), '-format', 'shape:svg', '-export', 'shape', str(d),
                    str(swf_path)], check=True, capture_output=True)
    text = (d / f'{shape}.svg').read_text()
    t = [float(v) for v in re.search(r'patternTransform="matrix\(([^)]*)\)"', text).group(1).split(',')]
    if abs(t[1]) > 1e-6 or abs(t[2]) > 1e-6 or abs(t[0] - t[3]) > 1e-6:
        raise ValueError(f'Shape {shape}: pattern not uniformly scaled')
    png = base64.b64decode(re.search(r'base64,([A-Za-z0-9+/=]+)', text).group(1))
    path = work / f'water-tile-{shape}.png'
    path.write_bytes(png)
    return np.asarray(Image.open(path).convert('RGBA')), t[4], t[5], t[0]


def rgb5a3_words(a):
    """Linear RGB5A3 texels (big-endian u16) in GX 4x4 tile order."""
    a = a.astype(np.uint16)
    r, g, b, al = a[..., 0], a[..., 1], a[..., 2], a[..., 3]
    op = 0x8000 | ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3)
    tr = ((al >> 5) << 12) | ((r >> 4) << 8) | ((g >> 4) << 4) | (b >> 4)
    px = np.where(al >= 0xE0, op, tr)
    h, w = px.shape
    return px.reshape(h // 4, 4, w // 4, 4).transpose(0, 2, 1, 3).ravel()


def build(work, jar, swf_path, swf, areas, table, sprites, scripts):
    raw, start = S.body(swf)
    rows, blocks, tile = [], [], None
    for a in areas:
        ident = table.get(a + 'Water')
        if ident is None:
            continue
        stage = sprites[ident]['stage'][0]
        places = {d: (c, clip, al) for d, c, clip, al in placements(raw, start, ident)}
        masked = {}   # mask depth -> masked depths' blocks
        for d, (c, clip, al) in places.items():
            if clip:
                masked[d] = [dd for dd in places if d < dd <= clip]
        for d, e in sorted(stage.items()):
            if e.get('id') != WATER:
                continue
            m = e['matrix']
            if abs(m['sx'] - 1) > 1e-3 or m['r0'] or m['r1']:
                # the Castle's little pool by the waterfall (WaterLength 100) is
                # tilted 5.6 degrees: the port's strips are level, so it is
                # drawn level (the same length and place)
                if not (a == 'Castle' and abs(m['r1']) < 0.1 and abs(math.hypot(m['sx'], m['r1']) - 1) < 1e-2):
                    raise ValueError(f'{a}: Water Code placed scaled or rotated')
            p, graphics = params(scripts, ident, d)
            block = -1
            for bd in masked.get(d, []):
                bid = stage[bd]['id']
                bm = stage[bd]['matrix']
                frames = sprites[bid]['stage']
                layers = []
                for ld, le in sorted(frames[0].items()):
                    holder = le['id']
                    (shape_depth, shape_e), = sprites[holder]['stage'][0].items()
                    bitmap, px, py, scale = pattern(jar, work, swf_path, shape_e['id'])
                    if tile is None:
                        tile = bitmap
                    elif not (tile == bitmap).all():
                        raise ValueError('Water blocks use different bitmaps')
                    sm = shape_e['matrix']
                    alpha = next(al for dd, cc, cl, al in placements(raw, start, bid) if dd == ld and al is not None)
                    offs = []
                    last = None
                    for f in frames:
                        mm = f.get(ld, {}).get('matrix') or last
                        last = mm
                        offs.append((mm['tx'] / 20, mm['ty'] / 20))
                    layers.append({'sx': round(le['matrix']['sx']), 'alpha': alpha, 'offs': offs,
                                   'origin': (sm['tx'] / 20 + px, sm['ty'] / 20 + py), 'scale': scale})
                blocks.append({'area': a, 'x': bm['tx'] / 20, 'y': bm['ty'] / 20, 'layers': layers})
                block = len(blocks) - 1
            rows.append((a, m['tx'] / 20, m['ty'] / 20, p, graphics, block))
    if tile is None:
        tile = np.zeros((4, 4, 4), np.uint8)
    frames = max((len(l['offs']) for b in blocks for l in b['layers']), default=1)
    out = ['/* Generated by tools/water_art.py (via export_course_art.py); do not edit.',
           '   The level water: Water Code (802) strips and the water blocks they mask. */',
           '#ifndef SM63_WATER_PLACES_H', '#define SM63_WATER_PLACES_H',
           'typedef struct { const char *area; float x, y, length, deep; unsigned points; float stiffness, randomwave;',
           '                 unsigned char rgba[4]; unsigned char splash; signed char block; } WaterPlace;',
           'typedef struct { signed char sx; float alpha, origin_x, origin_y, tile; float offs[%d][2]; } WaterBlockLayer;' % frames,
           'typedef struct { const char *area; float x, y; WaterBlockLayer layers[2]; } WaterBlock;',
           '#define WATER_BLOCK_FRAMES %d' % frames,
           'static const WaterPlace water_places[] = {']
    for a, x, y, p, graphics, block in rows:
        out.append('    {"%s", %.2ff, %.2ff, %.1ff, %.1ff, %du, %.3ff, %.3ff, {%d, %d, %d, %d}, %d, %d},' % (
            a, x, y, p['WaterLength'], p['WaterDeepness'], as_round(p['polygons'] * 0.75), p['waterstength'],
            p['randomwave'], int(p['redcolor']), int(p['greencolor']), int(p['bluecolor']),
            round(p['alpha'] * 255 / 100), 0 if graphics else 1, block))
    out.append('};')
    out.append('#define WATER_PLACES (sizeof(water_places) / sizeof(water_places[0]))')
    out.append('static const WaterBlock water_blocks[] = {')
    for b in blocks:
        if len(b['layers']) != 2:
            raise ValueError('Water block without two layers')
        ls = []
        for l in b['layers']:
            offs = ', '.join('{%.2ff, %.2ff}' % o for o in l['offs'])
            ls.append('{%d, %.4ff, %.2ff, %.2ff, %.1ff, {%s}}' % (l['sx'], l['alpha'], l['origin'][0], l['origin'][1],
                                                              32 * l['scale'], offs))
        out.append('    {"%s", %.2ff, %.2ff, {%s}},' % (b['area'], b['x'], b['y'], ', '.join(ls)))
    if not blocks:
        out.append('    {0}')
    out.append('};')
    out.append('#define WATER_BLOCKS %d' % len(blocks))
    h, w = tile.shape[:2]
    words = rgb5a3_words(tile)
    out.append('#define WATER_TILE_W %d' % w)
    out.append('#define WATER_TILE_H %d' % h)
    out.append('static const unsigned short water_tile[%d] __attribute__((aligned(32))) = {' % len(words))
    for i in range(0, len(words), 16):
        out.append('    ' + ', '.join('0x%04x' % v for v in words[i:i + 16]) + ',')
    out += ['};', '#endif', '']
    return '\n'.join(out)
