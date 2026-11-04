#!/usr/bin/env python3
"""The "Press <button> to Continue" prompt per controller (design approved
2026-11-02, docs/sign-design/prompt_preview.png).

The original prompt (sprite 250, shape 244 at its brightest frame) is one shape
with the teal words (solid fill) over a white glow (gradient fills). It is
rendered twice, each time with the other kind of fill made transparent, so the
two layers come apart cleanly. The words "Press" and "to Continue" are kept;
the controller's read button takes the "Z"'s place (1.6 x its cap height,
centred on it, the original word spacing); Classic shows "L or R" with "or"
cut from the prompt's own letters, the Wii Remote "A + B" with a "+" drawn in
the same teal. The glow is stretched smoothly to the new width.
"""
import struct
import numpy as np
from PIL import Image, ImageDraw
import swf_states as S
import swf_bitmap_fix as F
import sign_buttons
from swf_states import Bits

PROMPT, SHAPE, FRAME = 250, 244, 50
SCALE = 1.6


class _Alphas(F._Shape):
    """The alpha byte of every solid fill and gradient stop: (kind, offset)."""
    def __init__(self, data, version):
        super().__init__(data, version)
        self.alphas = []

    def fill_style(self, b):
        kind = self.d[b.byte]
        if kind == 0x00:
            b.byte += 1
            self.alphas.append(('solid', b.byte + 3))
            b.byte += 4
            return
        if kind in (0x10, 0x12, 0x13):
            b.byte += 1
            F._read_matrix(b)
            head = self.d[b.byte]
            b.byte += 1
            for _ in range(head & 15):
                self.alphas.append(('gradient', b.byte + 4))
                b.byte += 5
            if kind == 0x13:
                b.byte += 2
            return
        super().fill_style(b)


def keep_fills(swf, shape_id, keep):
    """SWF with shape_id's other fills made transparent (keep: 'solid' or 'gradient')."""
    raw, start = S.body(swf)
    out = bytearray(raw[:start])
    found = False
    for code, pos, length in S.tags(raw, start, len(raw)):
        data = raw[pos:pos + length]
        if code in F.SHAPE_TAGS and struct.unpack_from('<H', data)[0] == shape_id:
            s = _Alphas(data, F.SHAPE_TAGS[code])
            b = Bits(data, 2)
            F._rect(b)
            if s.v == 4:
                F._rect(b)
                b.byte += 1
            s.fill_styles(b); s.line_styles(b); s.records(b)
            d = bytearray(data)
            for kind, off in s.alphas:
                if kind != keep:
                    d[off] = 0
            data, found = bytes(d), True
        out += F._tag(code, data)
    if not found:
        raise ValueError(f'Shape {shape_id} not found')
    return b'FWS' + swf[3:4] + struct.pack('<I', len(out) + 8) + bytes(out)


def runs(mask):
    cols = np.flatnonzero(mask.any(0))
    out, s = [], cols[0]
    for p, q in zip(cols, cols[1:]):
        if q - p > 1:
            out.append((s, p + 1)); s = q
    out.append((s, cols[-1] + 1))
    return out


def words(text):
    a = np.asarray(text)[..., 3] > 40
    cols = np.flatnonzero(a.any(0))
    out, s = [], cols[0]
    for p, q in zip(cols, cols[1:]):
        if q - p > 5:
            out.append((s, p + 1)); s = q
    out.append((s, cols[-1] + 1))
    if len(out) != 4:
        raise ValueError(f'Expected "Press Z to Continue", found {len(out)} words')
    return out


def letters(text, x0, x1):
    a = np.asarray(text)[..., 3] > 60
    return [(x0 + s, x0 + e) for s, e in runs(a[:, x0:x1])]


def _buttons(kind, h):
    c = sign_buttons
    if kind == 'gamecube':
        return [c.STYLE[kind]['stomp'](h)]
    if kind == 'classic':
        return [c.wii('L', 'trigger', int(h * 0.85), 1.25, seed=4, tilt=-3), 'or',
                c.wii('R', 'trigger', int(h * 0.85), 1.25, seed=6, tilt=3)]
    return [c.wii('A', 'circle', h, seed=4, tilt=-2), '+', c.wii('B', 'trigger', h, 1.2, seed=6, tilt=3)]


def _tight(im):
    return im.crop(Image.fromarray(np.asarray(im)[..., 3] > 40).getbbox())


def build(text, glow, kind, scale=SCALE):
    """(image, left, top): the prompt for one controller; (left, top) is where the
    image's (0, 0) goes in the original prompt's canvas, keeping its centre."""
    (p0, p1), (z0, z1), (t0, t1), (c0, c1) = words(text)
    A = np.asarray(text)[..., 3] > 40
    rows = np.flatnonzero(A[:, z0:z1].any(1))
    cap0, cap1 = rows[0], rows[-1] + 1                       # the Z: cap height
    space = z0 - p1                                          # the original word spacing around "Z"
    r = letters(text, p0, p1)[1]                             # the r of Press
    o = letters(text, t0, t1)[1]                             # the o of to
    xr = np.flatnonzero(A[:, r[0]:r[1]].any(1))
    xmid, xh = (xr[0] + xr[-1] + 1) / 2, xr[-1] + 1 - xr[0]  # x-height
    a = np.asarray(text)
    teal = tuple(int(v) for v in a[np.unravel_index(a[..., 3].argmax(), a.shape[:2])][:3])

    def joiner(word):
        if word == 'or':
            rr = text.crop((r[0] - 1, 0, r[1] + 1, text.height))
            oo = text.crop((o[0] - 1, 0, o[1] + 1, text.height))
            im = Image.new('RGBA', (rr.width + oo.width - 1, text.height))
            im.alpha_composite(oo, (0, 0)); im.alpha_composite(rr, (oo.width - 1, 0))
            return im
        k, s = 4, int(xh * 1.1)
        im = Image.new('RGBA', (s * k, text.height * k))
        d = ImageDraw.Draw(im)
        cy, w = xmid * k, int(2.4 * k)
        d.line([(s * k * 0.1, cy), (s * k * 0.9, cy)], fill=teal + (255,), width=w)
        d.line([(s * k / 2, cy - s * k * 0.4), (s * k / 2, cy + s * k * 0.4)], fill=teal + (255,), width=w)
        return im.resize((s, text.height), Image.LANCZOS)
    parts = [('j', joiner(b)) if isinstance(b, str) else ('b', _tight(b))
             for b in _buttons(kind, int(round((cap1 - cap0) * scale)))]
    gap = max(3, space // 2)
    bw = sum(p.width for _, p in parts) + gap * (len(parts) - 1)
    bh = max(p.height for k, p in parts if k == 'b')
    btn = Image.new('RGBA', (bw, max(bh, text.height)))
    x = 0
    for k, p in parts:
        y = (btn.height - p.height) // 2 if k == 'b' else round(btn.height / 2 - (cap0 + cap1) / 2)
        btn.alpha_composite(p, (x, y))
        x += p.width + gap
    left = text.crop((0, 0, p1, text.height))
    right = text.crop((t0, 0, text.width, text.height))
    width = left.width + space + btn.width + space + right.width
    H = max(text.height, btn.height + 4)
    top = (H - text.height) // 2
    out = Image.new('RGBA', (width, H))
    tw_old, tw_new = c1 - p0, width - p0 - (text.width - c1)
    g = glow.resize((round(glow.width * tw_new / tw_old), glow.height), Image.BICUBIC)
    out.alpha_composite(g, (round(width / 2 - g.width / 2), top))
    out.alpha_composite(left, (0, top))
    out.alpha_composite(right, (left.width + space + btn.width + space, top))
    out.alpha_composite(btn, (left.width + space, round(top + (cap0 + cap1) / 2 - btn.height / 2)))
    # keep the original's centre (the middle of "Press .. Continue")
    shift = width / 2 - (p0 + c1) / 2
    return out, -shift, -top


def render_layers(ffdec, jar, work, swf):
    """(text, glow) renders of 250 at FRAME (same canvas as the original's render)."""
    imgs = {}
    for keep in ('solid', 'gradient'):
        path = work / f'prompt-{keep}.swf'
        path.write_bytes(keep_fills(swf, SHAPE, keep))
        d = ffdec(jar, work, path, PROMPT, [FRAME], work / f'prompt-{keep}')
        imgs[keep] = Image.open(d / f'{FRAME}.png').convert('RGBA')
    return imgs['solid'], imgs['gradient']
