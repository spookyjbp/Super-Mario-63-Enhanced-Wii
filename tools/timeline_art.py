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


def flatten(tl, sprite, frame, matrix=IDENT, cx=NO_CX, path=(), out=None, hold=None, filters=()):
    """[(path, symbol, matrix, cx, filters, clip)] drawn back to front for
    `sprite`'s frame `frame`; symbol = (character, ratio). `path` is the chain
    of depths. `hold`: {sprite: frame} for clips the caller drives itself.
    A filtered clip holding one child passes its filters down to it; other
    filtered or masking clips stay one piece (character, frame)."""
    if out is None:
        out = []
    fr = tl[sprite]['frames'][frame]
    for depth in sorted(fr):
        e = fr[depth]
        ident = e['id']
        m, c = mul(matrix, e['matrix']), cx_mul(cx, e['cx'])
        kind = tl.get(ident, {}).get('kind', 'other')
        p = path + (depth,)
        fl = tuple(filters) + tuple(e['filters'])
        if kind == 'sprite' and tl[ident]['frames']:
            f = hold[ident] if hold and ident in hold else child_frame(tl, ident, frame - e['born'])
            # blur is near enough linear to apply to each child instead of the group
            blur_only = all(t == 1 for t, _ in fl)
            if (fl and not (blur_only or _single_child(tl, ident))) or e['clip']:
                out.append((p, (ident, f), m, c, fl, e['clip']))
            else:
                flatten(tl, ident, f, m, c, p, out, hold, fl)
        else:
            ratio = round(e.get('ratio', 0) / 65535 * RATIO_STEPS) * 65535 // RATIO_STEPS if kind == 'morph' else 0
            out.append((p, (ident, ratio), m, c, fl, e['clip']))
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


def collect(tl, clip, frames, hold=None, extra=None):
    """Per frame [(bake key, weight 0..1, matrix, cx)] and the bake keys.
    `extra(frame, draws)` may edit a frame's draws (script-driven motion)."""
    raw = []
    for f in frames:
        d = flatten(tl, clip, f, hold=hold)
        if extra:
            d = extra(f, d)
        raw.append([(s, quantise_filters(fl), m, c) for p, s, m, c, fl, cl in d])
    # pieces with many filter variants: KEYS key blurs, cross-faded
    variants = {}
    for fr in raw:
        for s, fl, m, c in fr:
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
        for s, fl, m, c in fr:
            if s not in keyed:
                draws.append(((s, fl), 1.0, m, c))
                continue
            b, ks = variants[s][fl], keyed[s]
            j = 0
            while j + 1 < len(ks) - 1 and ks[j + 1][1] <= b:
                j += 1
            lo, hi = ks[j], ks[min(j + 1, len(ks) - 1)]
            t = 0.0 if hi[1] == lo[1] else min(1.0, max(0.0, (b - lo[1]) / (hi[1] - lo[1])))
            draws.append(((s, lo[0]), 1.0, m, c))
            if t > 0.02:
                draws.append(((s, hi[0]), t, m, c))      # the next key over it
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
        blur = _blur(fl)
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


def pack_pages(images, size=1024):
    """Shelf-pack images into size x size pages: [(page, x, y)] and the pages."""
    from PIL import Image
    order = sorted(range(len(images)), key=lambda i: -images[i].height)
    place = [None] * len(images)
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
    # trim the last page's unused height (to a multiple of 4)
    last = pages[-1]
    bb = last.getchannel('A').getbbox()
    if bb:
        pages[-1] = last.crop((0, 0, size, min(size, -(-(bb[3] + 1) // 4) * 4)))
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


def build_pack(tl, jar, work, swf, clips):
    """A timeline pack ("SM63TLP1") of several clips sharing their pieces.
    clips: [(name, sprite, frames, hold, extra)]. Big-endian:
      header: magic, u32 pages, u32 bakes, u32 clips, u32 total draws
      pages: u16 w, u16 h, u32 packed size, LZ4 RGBA8 tiles
      bakes: u16 page, u16 x, u16 y, u16 w, u16 h, f32 ox, f32 oy, f32 px (stage px per texel)
      clips: char[16] name, u32 first frame, u32 frames
      frames: u32 first draw, u32 draws (over all clips)
      draws: u16 bake, u8 weight, u8 0, f32 a b c d tx ty, s16 mul rgba (8.8), s16 add rgba
    """
    import lz4.block
    from PIL import Image
    all_frames, keys, clip_rows = [], [], []
    for name, sprite, frames, hold, extra in clips:
        fr, ks = collect(tl, sprite, frames, hold, extra)
        clip_rows.append((name, len(all_frames), len(fr)))
        all_frames += fr
        for k in ks:
            if k not in keys:
                keys.append(k)
    baked = bake_all(tl, jar, work, swf, keys)
    images = []
    for k in keys:
        im = baked[k][0]
        w4, h4 = -(-im.width // 4) * 4, -(-im.height // 4) * 4
        if (w4, h4) != im.size:
            c = Image.new('RGBA', (w4, h4)); c.paste(im, (0, 0)); im = c
        images.append(im)
    place, pages = pack_pages(images)
    out = bytearray(b'SM63TLP1')
    total = sum(len(f) for f in all_frames)
    out += struct.pack('>4I', len(pages), len(keys), len(clip_rows), total)
    for pg in pages:
        blob = lz4.block.compress(rgba8_tiles(pg), store_size=False)
        out += struct.pack('>HHI', pg.width, pg.height, len(blob)) + blob
        out += b'\0' * (-len(out) % 4)
    index = {k: i for i, k in enumerate(keys)}
    for k, (pg, x, y), im in zip(keys, place, images):
        _, ox, oy, z = baked[k]
        out += struct.pack('>5H3f', pg, x, y, im.width, im.height, ox, oy, 1 / z)
    for name, first, n in clip_rows:
        out += name.encode()[:16].ljust(16, b'\0') + struct.pack('>II', first, n)
    d = 0
    for fr in all_frames:
        out += struct.pack('>II', d, len(fr)); d += len(fr)
    for fr in all_frames:
        for k, wgt, m, c in fr:
            mul = [max(-32768, min(32767, round(v * 256))) for v in c[:4]]
            add = [max(-32768, min(32767, round(v))) for v in c[4:]]
            out += struct.pack('>HBB6f4h4h', index[k], round(wgt * 255), 0, *m, *mul, *add)
    return bytes(out), {'pages': len(pages), 'bakes': len(keys), 'draws': total}
