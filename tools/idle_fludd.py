#!/usr/bin/env python3
"""FLUDD on Mario's idle animations (a port addition, user request 2026-11-08).

In the original only Mario (None) Walk shows the Idle (1050) and Tired (1067)
animations, which end in Sleep (1450); the FLUDD walk clips have none. The
port plays them with FLUDD too, built the way the original's own clips draw
FLUDD on Mario's no-FLUDD poses (Mario Talk, Hurt 3, Boss Cinema, Start Course
Animation, Overworld Mario - 1): a side pose gets Fludd Side behind it and
Fludd Side Front (the nozzle) in front; a pose facing the camera Fludd Front
Code behind it; a pose facing away Fludd Backwards in front.

For every pose these clips use, FLUDD keeps the place it has on that pose in
an original clip (REFS): F = P * P_ref^-1 * F_ref, with P the pose's matrix
in the idle clip. Poses the original never draws with FLUDD (ALIASES) take
the nearest paired pose, its body aligned onto theirs (`align`).

The FLUDD sprites are placed into the SWF sprites themselves, so FFDec renders
them with the rest; their Fluddpow scripts are frozen per FLUDD state by
export_movement_clips.patched_swf (no FLUDD: their empty frames).
"""
import math, struct
import xml.etree.ElementTree as ET
from pathlib import Path
import numpy as np
from PIL import Image
import swf_states as S

NS = '{http://ns.adobe.com/xfl/2008/}'
LIBRARY = 'Super_Mario_63_2010/LIBRARY/Char'
CLIPS = {1050: 'Mario Idle Animation.xml', 1067: 'Mario Tired Animation.xml', 1450: 'Mario Sleep.xml'}
# The FLUDD symbols (by content: 333 tank + handle, 342 the Dynamic Fludd Nozzle
# Clip, 1104 the front base (red-brown tank feet), 755 the back base).
SYMBOLS = {'Fludd Side': 333, 'Fludd Side Front': 342, 'Fludd Front Code': 1104, 'Fludd Backwards': 755}
IN_FRONT = {'Fludd Side Front', 'Fludd Backwards'}
DEPTH = {'Fludd Side': 1, 'Fludd Front Code': 2, 'Fludd Side Front': 60, 'Fludd Backwards': 61}
SHIFT = 10   # the clips' own depths move up by this
WING_CODE = 1003
# pose -> (original clip, frame index) drawing it with FLUDD
REFS = {
    'Overworld - Mario 3 - 3': ('Mario Talk.xml', 0),
    'Mario (none) - Shaking Head - 2': ('Mario Talk.xml', 20),
    'Mario (none) - Shaking Head - 3': ('Mario Talk.xml', 24),
    'Mario (none) - Shaking Head - 1': ('Mario Talk.xml', 32),
    'Mario (none) - Looking Down - 2': ('Mario Talk.xml', 45),
    'Mario (none) - Angry - 2': ('Mario CLIPS/Mario Boss Cinema.xml', 9),
    'Mario (none) - Angry - 1': ('Mario CLIPS/Mario Boss Cinema.xml', 35),
    'Mario (none) Land - 1': ('Mario CLIPS/Mario Boss Cinema.xml', 44),
    'Mario (none) - Shaking Head 2 - 2': ('Mario CLIPS/Mario Start Course Animation.xml', 1),
    'Overworld - Mario 1 - 7': ('Mario CLIPS/Overworld Mario - 1.xml', 8),
    'Mario (none) Hurt Fall Back -1': ('Mario CLIPS/Mario Hurt 3.xml', 0),
    'Mario (none) Hurt Fall Back -3': ('Mario CLIPS/Mario Hurt 3.xml', 18),
    'Mario (none) Hurt Fall Forward - 1': ('Mario CLIPS/Mario Hurt 3.xml', 11),
    'Mario (none) Hurt Fall Forward - 2': ('Mario CLIPS/Mario Hurt 3.xml', 12),
}
# pose -> (paired pose, how to align: 'back' = feet and the line of the back
# (standing side poses, FLUDD rests on the back), or the part of the paired
# body matched onto the pose (legs / upper / all) and the rotations tried)
ALIASES = {
    'Mario (None) - Fixing Hat - 1': ('Overworld - Mario 3 - 3', 'back', (0,)),
    'Mario (None) - Fixing Hat - 2': ('Overworld - Mario 3 - 3', 'back', (0,)),
    'Mario (none) - Shaking Head 2 - 1': ('Mario (none) - Shaking Head 2 - 2', 'all', (0,)),
    'Mario (none) - Thinking - 1': ('Mario (none) - Shaking Head 2 - 2', 'all', (0,)),
    'Mario (none) Planning - 1': ('Mario (none) - Shaking Head 2 - 2', 'all', (0,)),
    'Mario (none) Planning - 2': ('Mario (none) - Shaking Head 2 - 2', 'all', (0,)),
    'Mario (none) Planning - 3': ('Mario (none) - Shaking Head 2 - 2', 'all', (0,)),
    # drawn facing the other way: the paired body mirrored first
    'Mario (none) Dive - 2': ('Mario (none) Hurt Fall Forward - 2', 'all', (-10, -5, 0, 5, 10), True),
    **{f'Mario (none) - Tired - {i}': ('Overworld - Mario 3 - 3', 'upper', tuple(range(-40, 41, 5))) for i in range(1, 9)},
}


def mul(m, n):
    """Flash matrices (a, b, c, d, tx, ty): m after n."""
    a, b, c, d, tx, ty = m
    A, B, C, D, TX, TY = n
    return (a * A + c * B, b * A + d * B, a * C + c * D, b * C + d * D, a * TX + c * TY + tx, b * TX + d * TY + ty)


def inv(m):
    a, b, c, d, tx, ty = m
    det = a * d - b * c
    ia, ib, ic, id_ = d / det, -b / det, -c / det, a / det
    return (ia, ib, ic, id_, -(ia * tx + ic * ty), -(ib * tx + id_ * ty))


def _matrix(e):
    m = e.find(f'{NS}matrix/{NS}Matrix')
    a = m.attrib if m is not None else {}
    return tuple(float(a.get(k, v)) for k, v in (('a', 1), ('b', 0), ('c', 0), ('d', 1), ('tx', 0), ('ty', 0)))


def timeline(path):
    """Per frame: [(item name, matrix)] of library items shown (top layer first)."""
    root = ET.parse(path).getroot()
    layers = list(root.iter(NS + 'DOMLayer'))
    count = max(int(f.get('index')) + int(f.get('duration') or 1) for l in layers for f in l.iter(NS + 'DOMFrame'))
    out = []
    for t in range(count):
        items = []
        for layer in layers:
            for f in layer.iter(NS + 'DOMFrame'):
                i, n = int(f.get('index')), int(f.get('duration') or 1)
                if i <= t < i + n:
                    items += [(e.get('libraryItemName').strip().split('/')[-1], _matrix(e))
                              for e in f.iter() if e.get('libraryItemName')]
        out.append(items)
    return out


def _mask(source, pose):
    im = Image.open(source / LIBRARY / 'BITS Mario' / f'{pose}.png').convert('RGBA')
    return np.asarray(im)[:, :, 3] > 0


def _back(m):
    """(the back's x at the waist, the soles' y) of a side pose's bitmap (its
    back on the left, as drawn)."""
    h = m.shape[0]
    rows = np.nonzero(m.any(axis=1))[0]
    waist = [np.nonzero(m[y])[0].min() for y in range(int(h * 0.45), int(h * 0.75)) if m[y].any()]
    return float(np.median(waist)), float(rows.max() + 1)


def align(source, pose, ref, region, rots, mirror=False):
    """Matrix taking the paired pose's bitmap onto `pose`'s: the paired body
    (its legs, upper 70% or all of it) rotated about its centre and moved by
    half-pixel steps to cover as much of `pose`, with as little outside it and
    as little of `pose` left bare where it lies."""
    P, R = _mask(source, pose), _mask(source, ref)
    if mirror:
        flip = (-1, 0, 0, 1, R.shape[1], 0)
        return mul(_align(P, R[:, ::-1], region, rots), flip)
    return _align(P, R, region, rots)


def _align(P, R, region, rots):
    if region == 'back':
        (px, py), (rx_, ry_) = _back(P), _back(R)
        return (1, 0, 0, 1, px - rx_, py - ry_)
    h = R.shape[0]
    sel = np.zeros_like(R)
    if region == 'legs': sel[h // 2:, :] = True
    elif region == 'upper': sel[:int(h * 0.7), :] = True
    else: sel[:] = True
    ys, xs = np.nonzero(R & sel)
    cx, cy = xs.mean() + .5, ys.mean() + .5
    ph, pw = P.shape
    best = None
    for th in rots:
        c, s = math.cos(math.radians(th)), math.sin(math.radians(th))
        X, Y = xs + .5 - cx, ys + .5 - cy
        rx, ry = c * X - s * Y, s * X + c * Y
        for dy in np.arange(-12, ph + 12, 0.5):
            for dx in np.arange(-12, pw + 12, 0.5):
                qx, qy = np.floor(rx + dx).astype(int), np.floor(ry + dy).astype(int)
                ok = (qx >= 0) & (qx < pw) & (qy >= 0) & (qy < ph)
                hit = np.zeros_like(ok)
                hit[ok] = P[qy[ok], qx[ok]]
                if not ok.any():
                    continue
                # covered minus outside minus the pose's own pixels left bare around it
                x0, x1 = max(qx.min(), 0), min(qx.max(), pw - 1)
                y0, y1 = max(qy.min(), 0), min(qy.max(), ph - 1)
                bare = int(P[y0:y1 + 1, x0:x1 + 1].sum()) - int(hit.sum())
                score = int(hit.sum()) * 2 - len(hit) - max(bare, 0)
                if best is None or score > best[0]:
                    best = (score, th, dx, dy)
    _, th, dx, dy = best
    c, s = math.cos(math.radians(th)), math.sin(math.radians(th))
    return (c, s, -s, c, dx - (c * cx - s * cy), dy - (s * cx + c * cy))


def _swf_pose(stage):
    """The pose's matrix in a frame of the SWF sprite (Wing Code aside)."""
    shown = [e for e in stage.values() if e.get('id') not in (WING_CODE, None)]
    if len(shown) != 1:
        raise ValueError(f'expected one pose, found {shown}')
    m = shown[0].get('matrix', {})
    return shown[0]['id'], (m.get('sx', 1.0), m.get('r0', 0.0), m.get('r1', 0.0), m.get('sy', 1.0),
                            m.get('tx', 0) / 20, m.get('ty', 0) / 20)


def placements(source, sprites):
    """{sprite id: per frame {depth: (character, matrix)}} of the FLUDD sprites.
    `sprites`: swf_states.sprites of the SWF (its frames between keyframes,
    the tweens, move the pose: P(t) = M(t) * M(key)^-1 * P(key))."""
    ref = {}
    for pose, (clip, frame) in REFS.items():
        items = timeline(source / LIBRARY / clip)[frame]
        body = [m for n, m in items if n == pose]
        fl = [(n, m) for n, m in items if n in SYMBOLS]
        if len(body) != 1 or not fl:
            raise ValueError(f'{clip} frame {frame}: expected {pose} with FLUDD')
        ref[pose] = [(n, mul(inv(body[0]), m)) for n, m in fl]   # FLUDD in the pose bitmap's space
    for pose, (other, region, rots, *mirror) in ALIASES.items():
        A = align(source, pose, other, region, rots, *mirror)
        ref[pose] = [(n, mul(A, m)) for n, m in ref[other]]
    out = {}
    for ident, name in CLIPS.items():
        frames, key = [], 0
        line = timeline(source / LIBRARY / 'Mario CLIPS' / name)
        if len(line) != sprites[ident]['frames']:
            raise ValueError(f'{name}: {len(line)} frames, sprite {ident} {sprites[ident]["frames"]}')
        for t, items in enumerate(line):
            poses = [(n, m) for n, m in items if n in ref]
            unknown = [n for n, _ in items if n not in ref and n != 'Wing Code']
            if unknown or len(poses) != 1:
                raise ValueError(f'{name}: no FLUDD placement for {unknown or poses}')
            if t == 0 or items != line[t - 1]:
                key = t
            pose, P = poses[0]
            (kc, km), (c, m_t) = _swf_pose(sprites[ident]['stage'][key]), _swf_pose(sprites[ident]['stage'][t])
            if c == kc:
                P = mul(mul(m_t, inv(km)), P)
            frames.append({DEPTH[n]: (SYMBOLS[n], mul(P, m)) for n, m in ref[pose]})
        out[ident] = frames
    return out


class _Bits:
    def __init__(self): self.bits = []
    def put(self, value, n):
        self.bits += [(value >> (n - 1 - i)) & 1 for i in range(n)]
    def data(self):
        b = self.bits + [0] * (-len(self.bits) % 8)
        return bytes(int(''.join(map(str, b[i:i + 8])), 2) for i in range(0, len(b), 8))


def _nbits(*values):
    return max(max((v if v >= 0 else ~v).bit_length() + 1 for v in values), 1)


def swf_matrix(m):
    a, b, c, d, tx, ty = m
    w = _Bits()
    sx, sy, r0, r1 = (round(v * 65536) for v in (a, d, b, c))
    if (sx, sy) != (65536, 65536):
        n = _nbits(sx, sy); w.put(1, 1); w.put(n, 5); w.put(sx & (1 << n) - 1, n); w.put(sy & (1 << n) - 1, n)
    else:
        w.put(0, 1)
    if (r0, r1) != (0, 0):
        n = _nbits(r0, r1); w.put(1, 1); w.put(n, 5); w.put(r0 & (1 << n) - 1, n); w.put(r1 & (1 << n) - 1, n)
    else:
        w.put(0, 1)
    x, y = round(tx * 20), round(ty * 20)
    n = _nbits(x, y) if (x, y) != (0, 0) else 0
    w.put(n, 5)
    if n:
        w.put(x & (1 << n) - 1, n); w.put(y & (1 << n) - 1, n)
    return w.data()


def _tag(code, data):
    if len(data) < 63:
        return struct.pack('<H', code << 6 | len(data)) + data
    return struct.pack('<HI', code << 6 | 63, len(data)) + data


def inject(raw, start, source):
    """The SWF body with the FLUDD sprites placed in CLIPS (their own depths
    moved up by SHIFT)."""
    fake = b'FWS' + bytes([8]) + struct.pack('<I', len(raw) + 8) + bytes(raw)
    place = placements(source, S.sprites(fake))
    out = bytearray(raw[:start])
    for code, pos, length in S.tags(raw, start, len(raw)):
        data = raw[pos:pos + length]
        if code == 39 and struct.unpack_from('<H', data)[0] in place:
            ident, count = struct.unpack_from('<HH', data)
            frames = place[ident]
            if len(frames) != count:
                raise ValueError(f'sprite {ident}: {count} frames, XFL {len(frames)}')
            body, shown, frame = bytearray(), {}, 0
            for c, p, n in S.tags(raw, pos + 4, pos + length):
                d = bytearray(raw[p:p + n])
                if c in (26, 70):
                    q = 2 if c == 70 else 1
                    struct.pack_into('<H', d, q, struct.unpack_from('<H', d, q)[0] + SHIFT)
                elif c == 28:
                    struct.pack_into('<H', d, 0, struct.unpack_from('<H', d, 0)[0] + SHIFT)
                elif c == 1:
                    want = frames[frame]
                    for depth in sorted(shown):
                        if depth not in want or want[depth][0] != shown[depth][0]:
                            body += _tag(28, struct.pack('<H', depth))
                    for depth, (char, m) in sorted(want.items()):
                        if depth in shown and shown[depth][0] == char:
                            if shown[depth][1] != m:
                                body += _tag(26, struct.pack('<BH', 0x05, depth) + swf_matrix(m))
                        else:
                            body += _tag(26, struct.pack('<BHH', 0x06, depth, char) + swf_matrix(m))
                    shown = dict(want)
                    frame += 1
                body += _tag(c, bytes(d))
            data = struct.pack('<HH', ident, count) + bytes(body)
        out += _tag(code, data)
    return out
