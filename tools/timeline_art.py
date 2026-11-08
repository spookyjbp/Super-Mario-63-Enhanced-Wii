#!/usr/bin/env python3
"""Flash timelines replayed by the port (the opening, the title screen, the
main menu's backdrop): each frame of a clip flattened into a list of symbol
draws (symbol, 2 x 3 matrix, colour transform) so the Wii draws the same
pictures without storing whole frames.

parse(swf)            every sprite's timeline: per frame, depth -> placement
                      (character, matrix, colour transform, clip depth,
                      filters), labels, stop() frames
flatten(...)          a frame of a clip with its child clips playing as Flash
                      plays them (from their placement, holding on stop()),
                      down to shapes, text and bitmaps
"""
import struct
import swf_states as S

SHAPES = {2, 22, 32, 83}           # DefineShape 1-4
TEXTS = {11, 33, 37}               # DefineText, DefineText2, DefineEditText
RATIO_STEPS = 40                   # morph shapes are baked at 1/40 steps of their tween
IDENT = (1, 0, 0, 1, 0, 0)         # a, b, c, d, tx, ty (px)
NO_CX = (1, 1, 1, 1, 0, 0, 0, 0)   # multipliers r g b a, additions r g b a (0..255)


def _cxform(b):
    has_add, has_mult, n = b.ub(1), b.ub(1), b.ub(4)
    mul, add = [1, 1, 1, 1], [0, 0, 0, 0]
    if has_mult:
        mul = [b.sb(n) / 256 for _ in range(4)]
    if has_add:
        add = [b.sb(n) for _ in range(4)]
    b.align()
    return tuple(mul + add)


def _matrix(b):
    m = S.matrix(b)
    return (m['sx'], m['r0'], m['r1'], m['sy'], m['tx'] / 20, m['ty'] / 20)


def _filters(data, q):
    """[(type, raw bytes)] of a FILTERLIST; returns (filters, end)."""
    n = data[q]; q += 1
    out = []
    sizes = {0: 23, 1: 9, 2: 15, 3: 27, 6: 80}   # DropShadow, Blur, Glow, Bevel, ColorMatrix
    for _ in range(n):
        t = data[q]; q += 1
        if t in sizes:
            out.append((t, bytes(data[q:q + sizes[t]]))); q += sizes[t]
        elif t == 5:   # Convolution
            mx, my = data[q], data[q + 1]
            size = 2 + 8 + mx * my * 4 + 4 + 1
            out.append((t, bytes(data[q:q + size]))); q += size
        elif t in (4, 7):   # gradient glow / bevel
            k = data[q]
            size = 1 + k * 5 + 19
            out.append((t, bytes(data[q:q + size]))); q += size
        else:
            raise ValueError(f'filter type {t}')
    return out, q


def _has_stop(action):
    q = 0
    while q < len(action) and action[q]:
        code = action[q]; q += 1
        if code == 0x07:
            return True
        if code >= 0x80:
            q += 2 + struct.unpack_from('<H', action, q)[0]
    return False


def parse(swf):
    """{id: {'kind': 'sprite'|'shape'|'text'|'other', 'frames': [{depth: placement}],
    'labels': {name: index}, 'stops': {index}}} (frame indices from 0)."""
    raw, start = S.body(swf)
    out = {}
    for code, pos, length in S.tags(raw, start, len(raw)):
        if code in (46, 84):   # DefineMorphShape 1 / 2: drawn at the placement's ratio
            out[struct.unpack_from('<H', raw, pos)[0]] = {'kind': 'morph'}
        elif code in SHAPES or code in TEXTS or code in (7, 34):   # shapes, texts, buttons
            ident = struct.unpack_from('<H', raw, pos)[0]
            out[ident] = {'kind': 'shape' if code in SHAPES else 'text' if code in TEXTS else 'other'}
            if code in SHAPES:
                b = S.Bits(raw, pos + 2); n = b.ub(5)
                x0, x1, y0, y1 = (b.sb(n) / 20 for _ in range(4))
                out[ident]['bounds'] = (x0, y0, x1, y1)
        elif code in (6, 20, 21, 35, 36, 90):
            out[struct.unpack_from('<H', raw, pos)[0]] = {'kind': 'bitmap'}
        if code != 39:
            continue
        ident = struct.unpack_from('<H', raw, pos)[0]
        frames, labels, stops, display = [], {}, set(), {}
        for c, p, n in S.tags(raw, pos + 4, pos + length):
            if c in (26, 70):
                flags = raw[p]; flags2 = raw[p + 1] if c == 70 else 0
                q = p + (2 if c == 70 else 1)
                depth = struct.unpack_from('<H', raw, q)[0]; q += 2
                if flags2 & 0x08:
                    q = raw.index(0, q) + 1
                old = display.get(depth)
                e = dict(old) if old and flags & 0x01 else {'matrix': IDENT, 'cx': NO_CX, 'filters': [], 'clip': 0}
                if flags & 0x02:
                    new = struct.unpack_from('<H', raw, q)[0]; q += 2
                    if not (old and flags & 0x01 and old['id'] == new):
                        e['born'] = len(frames)
                    e['id'] = new
                if flags & 0x04:
                    b = S.Bits(raw, q); e['matrix'] = _matrix(b); q = b.byte
                if flags & 0x08:
                    b = S.Bits(raw, q); e['cx'] = _cxform(b); q = b.byte
                if flags & 0x10:
                    e['ratio'] = struct.unpack_from('<H', raw, q)[0]; q += 2
                if flags & 0x20:
                    end = raw.index(0, q); e['name'] = raw[q:end].decode('latin-1'); q = end + 1
                if flags & 0x40:
                    e['clip'] = struct.unpack_from('<H', raw, q)[0]; q += 2
                if flags2 & 0x01:
                    e['filters'], q = _filters(raw, q)
                if flags2 & 0x02:
                    e['blend'] = raw[q]; q += 1
                if flags & 0x80:
                    e['actions'] = True
                if 'id' in e:
                    e.setdefault('born', len(frames))
                    display[depth] = e
            elif c in (5, 28):
                display.pop(struct.unpack_from('<H', raw, p + (2 if c == 5 else 0))[0], None)
            elif c == 43:
                labels[raw[p:raw.index(0, p)].decode('latin-1')] = len(frames)
            elif c == 12 and _has_stop(raw[p:p + n]):
                stops.add(len(frames))
            elif c == 1:
                frames.append({d: dict(e) for d, e in display.items()})
        out[ident] = {'kind': 'sprite', 'frames': frames, 'labels': labels, 'stops': stops}
    return out


def mul(m, n):
    """m applied after n (both a, b, c, d, tx, ty)."""
    a, b, c, d, tx, ty = m
    a2, b2, c2, d2, tx2, ty2 = n
    return (a * a2 + c * b2, b * a2 + d * b2, a * c2 + c * d2, b * c2 + d * d2,
            a * tx2 + c * ty2 + tx, b * tx2 + d * ty2 + ty)


def cx_mul(outer, inner):
    """inner's colour transform, then outer's."""
    om, oa = outer[:4], outer[4:]
    im, ia = inner[:4], inner[4:]
    return tuple([om[i] * im[i] for i in range(4)] + [om[i] * ia[i] + oa[i] for i in range(4)])


def child_frame(tl, sprite, age):
    """The frame a child clip shows `age` parent frames after it was placed
    (playing from frame 0, holding on stop(), looping at the end)."""
    sp = tl[sprite]
    n = len(sp['frames'])
    f = 0
    for _ in range(age):
        if f in sp['stops']:
            break
        f = f + 1 if f + 1 < n else 0
    return f


def _single_child(tl, ident):
    fr = tl[ident]['frames']
    return all(len(f) <= 1 for f in fr)


def _apply(m, x, y):
    return m[0] * x + m[2] * y + m[4], m[1] * x + m[3] * y + m[5]


def flatten(tl, sprite, frame, matrix=IDENT, cx=NO_CX, path=(), out=None, hold=None, filters=(), clips=(), skip=()):
    """[(path, symbol, matrix, cx, filters, clips)] drawn back to front for
    `sprite`'s frame `frame`; symbol = (character, ratio); clips = the convex
    polygons (stage px) of the masks over it. `path` is the chain of depths.
    `hold`: {sprite: frame} for clips the caller drives itself. A filtered
    clip holding one child (or only blurred) passes its filters down; other
    filtered clips stay one piece (character, frame). Masks are shapes: their
    bounding rectangles clip what they cover."""
    if out is None:
        out = []
    fr = tl[sprite]['frames'][frame]
    masks = []   # (last depth covered, polygon)
    for depth in sorted(fr):
        e = fr[depth]
        ident = e['id']
        if ident in skip or e.get('name') in skip:
            continue
        m, c = mul(matrix, e['matrix']), cx_mul(cx, e['cx'])
        kind = tl.get(ident, {}).get('kind', 'other')
        p = path + (depth,)
        if e['clip']:
            bx = tl.get(ident, {}).get('bounds')
            if bx is None:
                raise ValueError(f'mask {ident} is not a shape')
            x0, y0, x1, y1 = bx
            masks.append((e['clip'], [_apply(m, x, y) for x, y in ((x0, y0), (x1, y0), (x1, y1), (x0, y1))]))
            continue
        here = clips + tuple(poly for last, poly in masks if depth <= last)
        fl = tuple(filters) + tuple(e['filters'])
        if kind == 'sprite' and tl[ident]['frames']:
            f = hold[ident] if hold and ident in hold else child_frame(tl, ident, frame - e['born'])
            # blur is near enough linear to apply to each child instead of the group
            blur_only = all(t == 1 for t, _ in fl)
            if fl and not (blur_only or _single_child(tl, ident)):
                out.append((p, (ident, f), m, c, fl, here))
            else:
                flatten(tl, ident, f, m, c, p, out, hold, fl, here, skip)
        else:
            ratio = round(e.get('ratio', 0) / 65535 * RATIO_STEPS) * 65535 // RATIO_STEPS if kind == 'morph' else 0
            out.append((p, (ident, ratio), m, c, fl, here))
    return out


def clip_poly(poly, clip):
    """Sutherland-Hodgman: convex polygon `poly` [(x, y)] inside convex `clip`."""
    def inside(p, a, b, sgn):
        return sgn * ((b[0] - a[0]) * (p[1] - a[1]) - (b[1] - a[1]) * (p[0] - a[0])) >= 0
    def cross(p, q, a, b):
        d1 = (q[0] - p[0], q[1] - p[1]); d2 = (b[0] - a[0], b[1] - a[1])
        den = d1[0] * d2[1] - d1[1] * d2[0]
        if abs(den) < 1e-12:
            return q
        t = ((a[0] - p[0]) * d2[1] - (a[1] - p[1]) * d2[0]) / den
        return (p[0] + t * d1[0], p[1] + t * d1[1])
    area = sum(clip[i][0] * clip[(i + 1) % len(clip)][1] - clip[(i + 1) % len(clip)][0] * clip[i][1]
               for i in range(len(clip)))
    sgn = 1 if area > 0 else -1
    out = list(poly)
    for i in range(len(clip)):
        a, b = clip[i], clip[(i + 1) % len(clip)]
        src, out = out, []
        for j in range(len(src)):
            pj, pk = src[j], src[(j + 1) % len(src)]
            if inside(pk, a, b, sgn):
                if not inside(pj, a, b, sgn):
                    out.append(cross(pj, pk, a, b))
                out.append(pk)
            elif inside(pj, a, b, sgn):
                out.append(cross(pj, pk, a, b))
        if not out:
            return []
    return out


# ---- baking: every piece a clip needs, rendered by FFDec as frames of one sprite ----

BAKE_SPRITE = 65000


def _place3(char, filters=(), ratio=None):
    """A PlaceObject3 body: depth 1, the character at the origin (identity
    matrix), with its filters and morph ratio."""
    flags = 0x02 | 0x04 | (0x10 if ratio is not None else 0)
    flags2 = 0x01 if filters else 0
    out = bytes([flags, flags2]) + struct.pack('<HH', 1, char) + b'\x00'   # empty MATRIX record
    if ratio is not None:
        out += struct.pack('<H', ratio)
    if filters:
        out += bytes([len(filters)]) + b''.join(bytes([t]) + d for t, d in filters)
    return out


def bake_swf(swf, bakes):
    """The SWF with sprite BAKE_SPRITE whose frame k shows bakes[k] =
    (char, ratio or None, filters) alone at the origin."""
    import swf_bitmap_fix as F
    raw, start = S.body(swf)
    body = bytearray()
    for k, (char, ratio, filters) in enumerate(bakes):
        if k:
            body += F._tag(28, struct.pack('<H', 1))            # RemoveObject2 depth 1
        body += F._tag(70, _place3(char, filters, ratio))
        body += F._tag(1, b'')                                  # ShowFrame
    body += F._tag(0, b'')
    sprite = F._tag(39, struct.pack('<HH', BAKE_SPRITE, len(bakes)) + bytes(body))
    out = bytearray(raw[:start])
    for code, pos, length in S.tags(raw, start, len(raw)):
        if code == 0:
            out += sprite
        out += F._tag(code, raw[pos:pos + length])
    return b'FWS' + swf[3:4] + struct.pack('<I', len(out) + 8) + bytes(out)


def render_bakes(jar, work, swf, bakes, zoom):
    """[(RGBA image, ox, oy)] per bake: (ox, oy) = stage px of the image's
    corner from the piece's registration point."""
    import ui_art
    from PIL import Image
    path = work / 'bakes.swf'
    path.write_bytes(bake_swf(swf, bakes))
    old = ui_art.ZOOM
    ui_art.ZOOM = zoom
    try:
        frames = list(range(1, len(bakes) + 1))
        d = ui_art.ffdec(jar, work, path, BAKE_SPRITE, frames, work / 'bakes-png')
        sv = ui_art.ffdec(jar, work, path, BAKE_SPRITE, frames, work / 'bakes-svg', svg=True)
        out = []
        for f in frames:
            im = Image.open(d / f'{f}.png').convert('RGBA')
            ox, oy = ui_art.png_origin(sv / f'{f}.svg', im.size, zoom)
            bb = im.getchannel('A').getbbox()
            if bb is None:
                out.append((Image.new('RGBA', (4, 4)), 0.0, 0.0))
                continue
            out.append((im.crop(bb), ox + bb[0] / zoom, oy + bb[1] / zoom))
    finally:
        ui_art.ZOOM = old
    return out


def quantise_filters(filters):
    """Filters with their animated amounts rounded to steps, so frames close
    in blur / glow share one bake (a step is under a screen pixel)."""
    out = []
    for t, d in filters:
        d = bytearray(d)
        if t == 1:   # Blur: blurX, blurY (16.16), passes
            for o in (0, 4):
                v = struct.unpack_from('<I', d, o)[0] / 65536
                struct.pack_into('<I', d, o, int(round(v * 2) / 2 * 65536))
        elif t == 2:   # Glow: RGBA, blurX, blurY, strength (8.8), flags
            for i in range(4):
                d[i] = min(255, round(d[i] / 17) * 17)
            for o in (4, 8):
                v = struct.unpack_from('<I', d, o)[0] / 65536
                struct.pack_into('<I', d, o, int(round(v * 2) / 2 * 65536))
            v = struct.unpack_from('<H', d, 12)[0] / 256
            struct.pack_into('<H', d, 12, int(round(v * 10) / 10 * 256))
        out.append((t, bytes(d)))
    return tuple(out)


def detail(im):
    """How much fine detail an image has (mean absolute Laplacian of its
    premultiplied colour, 0..255): soft glows and gradients score low."""
    import numpy as np
    a = np.asarray(im).astype(float)
    p = a[..., :3] * a[..., 3:] / 255
    lap = np.abs(4 * p[1:-1, 1:-1] - p[:-2, 1:-1] - p[2:, 1:-1] - p[1:-1, :-2] - p[1:-1, 2:])
    cover = a[1:-1, 1:-1, 3] > 8
    return float(lap[cover].mean()) if cover.any() else 0.0


# ---- a clip as the port's timeline pack ----

VIEW_W, VIEW_H = 270, 150   # what the 16:9 screen shows around a clip at the stage centre (stage px)
TILE = 508                  # big pieces are split into tiles (two to a page row)
MAX_VARIANTS = 6        # a piece with more filter variants is baked at key blurs and cross-faded
KEYS = 7
ZOOMS = ((2, 0.2), (8, 0.4), (25, 0.8), (1e9, 1.6))   # detail below -> bake zoom


def _blur(filters):
    for t, d in filters:
        if t == 1:
            return struct.unpack_from('<I', d, 0)[0] / 65536
    for t, d in filters:
        if t == 2:
            return struct.unpack_from('<I', d, 4)[0] / 65536
    return 0.0


def collect(tl, clip, frames, hold=None, groups=None, skip=()):
    """Per frame [(bake key, weight 0..1, matrix, cx, group, clips)] and the
    bake keys. `groups`: {top depth: group} tags draws under those depths so
    the port can drive them (script-driven clips). `skip`: characters or
    instance names left out (drawn by the port on their own). A shape or
    text character is a one-frame clip of itself."""
    raw = []
    if tl.get(clip, {}).get('kind') != 'sprite':
        k = (clip, 0)
        return [[((k, ()), 1.0, IDENT, NO_CX, 0, ())]], [(k, ())]
    for f in frames:
        d = flatten(tl, clip, f, hold=hold, skip=skip)
        raw.append([(s, quantise_filters(fl), m, c, (groups or {}).get(p[0], 0), cl)
                    for p, s, m, c, fl, cl in d])
    # pieces with many filter variants: KEYS key blurs, cross-faded
    variants = {}
    for fr in raw:
        for s, fl, m, c, g, cl in fr:
            variants.setdefault(s, {})[fl] = _blur(fl)
    keyed = {}
    for s, vs in variants.items():
        if len(vs) <= MAX_VARIANTS:
            continue
        ordered = sorted(vs.items(), key=lambda kv: kv[1])
        idx = sorted({round(i * (len(ordered) - 1) / (KEYS - 1)) for i in range(KEYS)})
        keyed[s] = [ordered[i] for i in idx]       # [(filters, blur)]
    out = []
    for fr in raw:
        draws = []
        for s, fl, m, c, g, cl in fr:
            if s not in keyed:
                draws.append(((s, fl), 1.0, m, c, g, cl))
                continue
            b, ks = variants[s][fl], keyed[s]
            j = 0
            while j + 1 < len(ks) - 1 and ks[j + 1][1] <= b:
                j += 1
            lo, hi = ks[j], ks[min(j + 1, len(ks) - 1)]
            t = 0.0 if hi[1] == lo[1] else min(1.0, max(0.0, (b - lo[1]) / (hi[1] - lo[1])))
            draws.append(((s, lo[0]), 1.0, m, c, g, cl))
            if t > 0.02:
                draws.append(((s, hi[0]), t, m, c, g, cl))      # the next key over it
        out.append(draws)
    keys = []
    for fr in out:
        for k, *_ in fr:
            if k not in keys:
                keys.append(k)
    return out, keys


def bake_all(tl, jar, work, swf, keys):
    """{key: (RGBA image, ox, oy, zoom)}: each piece at the resolution its
    detail needs (a low render first decides)."""
    bakes = [(s[0], s[1] if tl.get(s[0], {}).get('kind') == 'morph' else None, fl) for s, fl in keys]
    work.mkdir(parents=True, exist_ok=True)
    (work / 'probe').mkdir(exist_ok=True)
    probe = render_bakes(jar, work / 'probe', swf, bakes, 0.4)
    zoom = []
    for (s, fl), (im, ox, oy) in zip(keys, probe):
        z = next(z for lim, z in ZOOMS if detail(im) < lim)
        blur = max((struct.unpack_from('<I', d, 0)[0] / 65536 for t, d in fl if t == 1), default=0)   # Blur only
        if blur > 1.5:   # a blur of b px leaves nothing finer: ~3 / b texels per px is enough
            z = min(z, max(0.2, min(ZOOMS, key=lambda zz: abs(zz[1] - 3 / blur))[1]))
        zoom.append(z)
    out = [None] * len(keys)
    for z in sorted(set(zoom)):
        idx = [i for i, zz in enumerate(zoom) if zz == z]
        if z == 0.4:
            rr = [probe[i] for i in idx]
        else:
            (work / str(z)).mkdir(exist_ok=True)
            rr = render_bakes(jar, work / str(z), swf, [bakes[i] for i in idx], z)
        for i, (im, ox, oy) in zip(idx, rr):
            out[i] = (im, ox, oy, z)
    return dict(zip(keys, out))


def pack_pages(images, size=1024, order=None):
    """Shelf-pack images into size x size pages: [(page, x, y)] and the pages."""
    from PIL import Image
    order = sorted(order if order is not None else range(len(images)), key=lambda i: -images[i].height)
    place = {}
    pages, x, y, row = [], 0, 0, 0
    pages.append(Image.new('RGBA', (size, size)))
    for i in order:
        im = images[i]
        w, h = im.width + 2, im.height + 2
        if w > size or h > size:
            raise ValueError(f'piece {im.size} too big for a {size} page')
        if x + w > size:
            x, y, row = 0, y + row, 0
        if y + h > size:
            pages.append(Image.new('RGBA', (size, size)))
            x = y = row = 0
        pages[-1].paste(im, (x + 1, y + 1))
        place[i] = (len(pages) - 1, x + 1, y + 1)
        x += w; row = max(row, h)
    # every page cut to the height it uses (to a multiple of 4)
    used = {}
    for i, (pg, x, y) in place.items():
        used[pg] = max(used.get(pg, 0), y + images[i].height + 1)
    pages = [pg.crop((0, 0, size, min(size, -(-used.get(k, 4) // 4) * 4))) for k, pg in enumerate(pages)]
    return place, pages


def rgba8_tiles(im):
    """GX RGBA8: 4x4 tiles, AR block then GB block."""
    import numpy as np
    a = np.asarray(im.convert('RGBA'))
    h, w = a.shape[:2]
    t = a.reshape(h // 4, 4, w // 4, 4, 4).transpose(0, 2, 1, 3, 4).reshape(-1, 16, 4)
    ar = np.stack([t[..., 3], t[..., 0]], -1).reshape(-1, 32)
    gb = np.stack([t[..., 1], t[..., 2]], -1).reshape(-1, 32)
    return np.concatenate([ar, gb], 1).tobytes()


def build_pack(tl, jar, work, swf, clips, load_groups=None):
    """A timeline pack ("SM63TLP2") of several clips sharing their pieces.
    clips: [(name, sprite, frames, hold, groups[, skip])]; `load_groups` maps a
    clip name to its load group (the pages the port uploads together; a piece
    used by two groups is stored in each). Big-endian:
      header: magic, u32 pages, u32 bakes, u32 clips, u32 frames, u32 draw bytes
      pages: u16 w, u16 h, u32 packed size, u8 format (0 RGBA8, 1 RGB5A3), u8 clip,
             2 pad, LZ4 texture tiles (4-byte aligned)
      bakes: u16 page, u16 x, u16 y, u16 w, u16 h, f32 ox, f32 oy, f32 px (stage px per texel)
      clips: char[16] name, u32 first frame, u32 frames
      frames: u32 draw offset (bytes into the draw data), u32 draws
      u32 packed size, LZ4 draw data:
      draws: u16 bake, u8 weight, u8 group, u8 vertices (0: the bake's quad
             through the matrix), 3 pad, f32 a b c d tx ty, s16 mul rgba (8.8),
             s16 add rgba, then vertices x f32 (x, y, u, v): stage px and
             texture coordinates within the bake (0..1)
    Coordinates are the clip's own (registration at 0, 0)."""
    import lz4.block
    from PIL import Image
    all_frames, keys, clip_rows, whole = [], [], [], set()
    for spec in clips:
        name, sprite, frames, hold, groups = spec[:5]
        skip = spec[5] if len(spec) > 5 else ()
        if len(spec) > 6 and spec[6] == 'whole':   # moved by the port (scrolled): keep all of it
            whole.add(len(clip_rows))
        fr, ks = collect(tl, sprite, frames, hold, groups, skip)
        clip_rows.append((name, len(all_frames), len(fr)))
        all_frames += fr
        for k in ks:
            if k not in keys:
                keys.append(k)
    baked = bake_all(tl, jar, work, swf, keys)
    # keep only what is ever on screen (the 16:9 view around the clip at the
    # stage centre), then split what is still bigger than a page into tiles
    view = [(-VIEW_W, -VIEW_H), (VIEW_W, -VIEW_H), (VIEW_W, VIEW_H), (-VIEW_W, VIEW_H)]
    seen = {}
    whole_frames = {fi for ci in whole for fi in range(clip_rows[ci][1], clip_rows[ci][1] + clip_rows[ci][2])}
    for fi, fr in enumerate(all_frames):
        for k, wgt, m, c, g, cl in fr:
            if fi in whole_frames:
                im = baked[k][0]
                seen[k] = (0, 0, im.width, im.height)
                continue
            A, B, C, D, TX, TY = m
            det = A * D - B * C
            if abs(det) < 1e-9:
                continue
            im, ox, oy, z = baked[k]
            pts = []
            for x, y in view:
                lx = (D * (x - TX) - C * (y - TY)) / det
                ly = (-B * (x - TX) + A * (y - TY)) / det
                pts.append(((lx - ox) * z, (ly - oy) * z))
            x0 = max(0, min(p[0] for p in pts) - 2); y0 = max(0, min(p[1] for p in pts) - 2)
            x1 = min(im.width, max(p[0] for p in pts) + 2); y1 = min(im.height, max(p[1] for p in pts) + 2)
            if x1 <= x0 or y1 <= y0:
                continue
            u = seen.get(k)
            seen[k] = (x0, y0, x1, y1) if u is None else (min(u[0], x0), min(u[1], y0), max(u[2], x1), max(u[3], y1))
    tiles = {}   # key -> [(sub key, x, y)] in the bake's texels
    for k in keys:
        im, ox, oy, z = baked[k]
        if k not in seen:
            tiles[k] = []
            continue
        x0, y0, x1, y1 = (int(v) for v in seen[k])
        x0, y0 = x0 // 4 * 4, y0 // 4 * 4
        x1, y1 = min(im.width, -(-int(x1) // 4) * 4), min(im.height, -(-int(y1) // 4) * 4)
        tl_ = []
        for ty in range(y0, y1, TILE):
            for tx in range(x0, x1, TILE):
                sub = im.crop((tx, ty, min(x1, tx + TILE), min(y1, ty + TILE)))
                if sub.getchannel('A').getbbox() is None:
                    continue
                sk = (k, tx, ty)
                baked[sk] = (sub, ox + tx / z, oy + ty / z, z)
                tl_.append(sk)
        tiles[k] = tl_
    keys = [sk for k in keys for sk in tiles[k]]
    all_frames = [[(sk, wgt, m, c, g, cl) for k, wgt, m, c, g, cl in fr for sk in tiles[k]] for fr in all_frames]
    images = []
    for k in keys:
        im = baked[k][0]
        w4, h4 = -(-im.width // 4) * 4, -(-im.height // 4) * 4
        if (w4, h4) != im.size:
            c = Image.new('RGBA', (w4, h4)); c.paste(im, (0, 0)); im = c
        images.append(im)
    # pages by load group (only the groups on show are decoded), a piece used
    # by several groups stored in each; opaque detailed pieces in RGB5A3,
    # soft or see-through ones in RGBA8
    group_of = [(load_groups or {}).get(name, ci) for ci, (name, first, n) in enumerate(clip_rows)]
    expanded, keymap = [], {}
    for ci, (name, first, n) in enumerate(clip_rows):
        g = group_of[ci]
        for fi in range(first, first + n):
            new = []
            for k, *rest in all_frames[fi]:
                if (k, g) not in keymap:
                    keymap[(k, g)] = len(expanded)
                    expanded.append((k, g))
                    baked[(k, g)] = baked[k]
                new.append(((k, g), *rest))
            all_frames[fi] = new
    image_of = {k: images[i] for i, k in enumerate(keys)}
    keys = [(k, g) for k, g in expanded]
    images = [image_of[k] for k, g in expanded]
    index = {k: i for i, k in enumerate(keys)}
    users = {k: k[1] for k in keys}
    fmt = [1 if baked[k][3] >= 1.6 and opaque_enough(images[i]) else 0 for i, k in enumerate(keys)]
    place, pages, page_fmt, page_clip = {}, [], [], []
    for ci in sorted(set(group_of)):
        for f in (0, 1):
            idx = [i for i, k in enumerate(keys) if users.get(k) == ci and fmt[i] == f]
            if not idx:
                continue
            pl, pg = pack_pages(images, order=idx)
            for i, (p_, x, y) in pl.items():
                place[i] = (len(pages) + p_, x, y)
            pages += pg; page_fmt += [f] * len(pg); page_clip += [ci] * len(pg)
    place = [place.get(i, (0, 0, 0)) for i in range(len(keys))]
    draw_data, frame_rows = bytearray(), []
    for fr in all_frames:
        start, count = len(draw_data), 0
        for k, wgt, m, c, g, cl in fr:
            _, ox, oy, z = baked[k]
            im = images[index[k]]
            verts = []
            if cl:
                kk = 1 / z
                corners = [(ox, oy), (ox + im.width * kk, oy), (ox + im.width * kk, oy + im.height * kk),
                           (ox, oy + im.height * kk)]
                poly = [_apply(m, x, y) for x, y in corners]
                whole = poly
                for clp in cl:
                    poly = clip_poly(poly, clp)
                    if not poly:
                        break
                if not poly:
                    continue                      # wholly masked
                if poly != whole:
                    # texture coordinates by inverting the piece's affine map
                    A, B, C, D, TX, TY = m
                    det = A * D - B * C
                    for x, y in poly:
                        lx = (D * (x - TX) - C * (y - TY)) / det
                        ly = (-B * (x - TX) + A * (y - TY)) / det
                        verts.append((x, y, (lx - ox) / (im.width * kk), (ly - oy) / (im.height * kk)))
            mul_ = [max(-32768, min(32767, round(v * 256))) for v in c[:4]]
            add = [max(-32768, min(32767, round(v))) for v in c[4:]]
            draw_data += struct.pack('>HBBB3x6f4h4h', index[k], round(wgt * 255), g, len(verts), *m, *mul_, *add)
            for v in verts:
                draw_data += struct.pack('>4f', *v)
            count += 1
        frame_rows.append((start, count))
    out = bytearray(b'SM63TLP2')
    out += struct.pack('>5I', len(pages), len(keys), len(clip_rows), len(all_frames), len(draw_data))
    for pg, f, ci in zip(pages, page_fmt, page_clip):
        blob = lz4.block.compress((rgb5a3_tiles if f else rgba8_tiles)(pg), store_size=False)
        out += struct.pack('>HHIBB2x', pg.width, pg.height, len(blob), f, ci) + blob
        out += b'\0' * (-len(out) % 4)
    for k, (pg, x, y), im in zip(keys, place, images):
        _, ox, oy, z = baked[k]
        out += struct.pack('>5H3f', pg, x, y, im.width, im.height, ox, oy, 1 / z)
        out += b'\0' * 2                        # 24 bytes a bake
    for name, first, n in clip_rows:
        out += name.encode()[:16].ljust(16, b'\0') + struct.pack('>II', first, n)
    for start, count in frame_rows:
        out += struct.pack('>II', start, count)
    packed = lz4.block.compress(bytes(draw_data), store_size=False)
    out += struct.pack('>I', len(packed)) + packed
    return bytes(out), {'pages': len(pages), 'bakes': len(keys), 'frames': len(all_frames),
                        'draw bytes': len(draw_data)}


def read_pack(data):
    """The pack back as Python data (for previews and tests)."""
    import lz4.block, numpy as np
    from PIL import Image
    assert data[:8] == b'SM63TLP2'
    npages, nbakes, nclips, nframes, nbytes = struct.unpack_from('>5I', data, 8)
    q = 28
    pages = []
    for _ in range(npages):
        w, h, n, f, ci = struct.unpack_from('>HHIBB', data, q); q += 12
        raw = lz4.block.decompress(data[q:q + n], uncompressed_size=w * h * (2 if f else 4)); q += n
        q += -q % 4
        if f:
            v = np.frombuffer(raw, '>u2').reshape(h // 4, w // 4, 4, 4).transpose(0, 2, 1, 3).reshape(h, w).astype(int)
            op = (v & 0x8000) != 0
            r = np.where(op, (v >> 10 & 31) * 255 // 31, (v >> 8 & 15) * 17)
            g = np.where(op, (v >> 5 & 31) * 255 // 31, (v >> 4 & 15) * 17)
            b = np.where(op, (v & 31) * 255 // 31, (v & 15) * 17)
            a = np.where(op, 255, (v >> 12 & 7) * 255 // 7)
            img = np.stack([r, g, b, a], -1).astype(np.uint8)
        else:
            t = np.frombuffer(raw, np.uint8).reshape(-1, 64)
            ar, gb = t[:, :32].reshape(-1, 16, 2), t[:, 32:].reshape(-1, 16, 2)
            px = np.stack([ar[..., 1], gb[..., 0], gb[..., 1], ar[..., 0]], -1)
            img = px.reshape(h // 4, w // 4, 4, 4, 4).transpose(0, 2, 1, 3, 4).reshape(h, w, 4)
        pages.append(Image.fromarray(img))
    bakes = []
    for _ in range(nbakes):
        bakes.append(struct.unpack_from('>5H3f', data, q)); q += 24
    clips = {}
    for _ in range(nclips):
        name = data[q:q + 16].rstrip(b'\0').decode(); first, n = struct.unpack_from('>II', data, q + 16); q += 24
        clips[name] = (first, n)
    frames = [struct.unpack_from('>II', data, q + 8 * i) for i in range(nframes)]; q += 8 * nframes
    plen = struct.unpack_from('>I', data, q)[0]; q += 4
    dd = lz4.block.decompress(data[q:q + plen], uncompressed_size=nbytes)
    draws_per_frame = []
    for off, n in frames:
        lst = []
        for _ in range(n):
            v = struct.unpack_from('>HBBB3x6f4h4h', dd, off); off += 48
            verts = [struct.unpack_from('>4f', dd, off + 16 * i) for i in range(v[3])]; off += 16 * v[3]
            lst.append((v[0], v[1] / 255, v[2], v[4:10], [x / 256 for x in v[10:14]], list(v[14:18]), verts))
        draws_per_frame.append(lst)
    return pages, bakes, clips, draws_per_frame


def preview(data, frame, scale=1.6, size=(720, 480), origin=(360, 240)):
    """A frame drawn the way the port draws it (stage px * scale)."""
    import numpy as np
    from PIL import Image, ImageDraw
    pages, bakes, clips, frames = read_pack(data)
    canvas = np.zeros((size[1], size[0], 3), float)
    for b, wgt, g, m, mul_, add, verts in frames[frame]:
        pg, x, y, w, h, ox, oy, k = bakes[b]
        im = np.asarray(pages[pg].crop((x, y, x + w, y + h))).astype(float) / 255
        rgb = np.clip(im[..., :3] * mul_[:3] + np.array(add[:3]) / 255, 0, 1)
        a = np.clip(im[..., 3] * mul_[3] + add[3] / 255, 0, 1) * wgt
        src = Image.fromarray((np.dstack([rgb, a]) * 255).astype(np.uint8))
        A, B, C, D, TX, TY = m
        M = np.array([[A * k, C * k, A * ox + C * oy + TX], [B * k, D * k, B * ox + D * oy + TY], [0, 0, 1]])
        S_ = np.array([[scale, 0, origin[0]], [0, scale, origin[1]], [0, 0, 1]])
        inv = np.linalg.inv(S_ @ M)
        warped = np.asarray(src.transform(size, Image.AFFINE, tuple(inv[:2].ravel()), Image.BILINEAR)).astype(float) / 255
        if verts:
            mask = Image.new('L', size)
            ImageDraw.Draw(mask).polygon([(vx * scale + origin[0], vy * scale + origin[1]) for vx, vy, u, v in verts],
                                         fill=255)
            warped[..., 3] *= np.asarray(mask).astype(float) / 255
        canvas = warped[..., :3] * warped[..., 3:] + canvas * (1 - warped[..., 3:])
    return Image.fromarray((canvas * 255).astype(np.uint8))


def rgb5a3_tiles(im):
    import numpy as np
    from water_art import rgb5a3_words
    return rgb5a3_words(np.asarray(im.convert('RGBA'))).astype('>u2').tobytes()


def opaque_enough(im):
    """True when RGB5A3 (opaque 15-bit colour, 3-bit alpha) keeps the piece:
    few see-through-but-not-clear pixels."""
    import numpy as np
    a = np.asarray(im)[..., 3]
    cover = a > 8
    return cover.any() and ((a > 8) & (a < 232)).sum() < 0.08 * cover.sum()
