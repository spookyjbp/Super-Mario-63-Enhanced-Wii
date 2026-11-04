#!/usr/bin/env python3
"""Animated picture signs: the moving parts of the tutorial's picture pages,
played over the still page as in the original.

* controls: the two walking Marios (357, a 16-frame walk) that 371 slides left
  and right over its 40 frames (nested clips loop on their own: 80 frames
  repeat). Their FLUDD (333, 342) shows frame 2 (none) while Fluddpow is "",
  which it is on the tutorial's first area where the sign stands.
* stomp: Mario's ground-pound demo in both boxes (424, 101 frames; the
  keyboard keycap 417 stays hidden).
* refill: the water drops (577, 123 frames, twice).

The page itself is rendered without these (ANIM_HIDE) and each frame of the
animation becomes one tile of a "<label>~" layer: the frame's pixels cropped
to their bounds, at their position on the page.
"""
import math
import numpy as np
from PIL import Image
import ui_art
import controller_signs

ZOOM = 1.6
# Removed from the page renders: drawn by the animation instead.
ANIM_HIDE = {371: {357}, 619: {424, 577}}
# FLUDD on the walking Marios: Fluddpow "" shows frame 2, which is empty.
NO_FLUDD = {357: {333, 342}}
SPEC = {   # label: (animated sprite, its parent in the page or None, frames of the cycle)
    'controls': (357, 371, 80),
    'stomp': (424, None, 101),
    'refill': (577, None, 123),
}


def _instances(sprites, page_frame, sprite, parent, t):
    """[(tx, ty, sx, sy)] of `sprite` in 619 coords on the cycle's frame t (0-based)."""
    out = []
    if parent is None:
        for d, e in sorted(page_frame.items()):
            if e.get('id') == sprite:
                m = e['matrix']
                out.append((m['tx'] / 20, m['ty'] / 20, m['sx'], m['sy']))
        return out
    pm = next(e['matrix'] for e in page_frame.values() if e.get('id') == parent)
    pf = sprites[parent]['stage'][t % sprites[parent]['frames']]
    for d, e in sorted(pf.items()):
        if e.get('id') == sprite:
            m = e['matrix']
            out.append((pm['tx'] / 20 + pm['sx'] * m['tx'] / 20, pm['ty'] / 20 + pm['sy'] * m['ty'] / 20,
                        pm['sx'] * m['sx'], pm['sy'] * m['sy']))
    return out


def build(work, jar, swf_bytes, sprites, label, page_original, origin):
    """Frames [(x, y, image)] in page px. page_original: the page as FFDec renders
    it (the animation at its first frame) for alignment; origin: 619 coords of
    the page image's (0, 0)."""
    sprite, parent, cycle = SPEC[label]
    page_frame = sprites[619]['stage'][sprites[619]['labels'][label]]
    hidden = dict(controller_signs.HIDE)
    for k, v in NO_FLUDD.items():
        hidden[k] = set(hidden.get(k, set())) | v
    swf = work / f'anim-{label}.swf'
    swf.write_bytes(ui_art.hide(swf_bytes, hidden))
    n = sprites[sprite]['frames']
    scales = sorted({abs(round(sx, 4)) for t in range(cycle) for _, _, sx, _ in
                     _instances(sprites, page_frame, sprite, parent, t)})
    renders = {}
    for s in scales:   # rendered at the size it is drawn (pixel art stays sharp)
        old = ui_art.ZOOM
        ui_art.ZOOM = ZOOM * s
        try:
            d = ui_art.ffdec(jar, work, swf, sprite, range(1, n + 1), work / f'anim-{label}-{s}')
        finally:
            ui_art.ZOOM = old
        renders[s] = [Image.open(d / f'{f}.png').convert('RGBA') for f in range(1, n + 1)]
    # every frame's PNG shares one canvas (frame 1's SVG gives its origin, padding corrected)
    sv = ui_art.ffdec(jar, work, swf, sprite, [1], work / f'anim-{label}-svg', svg=True)
    s0 = scales[0]
    sox, soy = ui_art.png_origin(sv / '1.svg', renders[s0][0].size, ZOOM * s0)

    def placed(t):
        out = []
        for tx, ty, sx, sy in _instances(sprites, page_frame, sprite, parent, t):
            s = abs(round(sx, 4))
            im = renders[s][t % n]
            z = ZOOM * s
            if sx < 0:
                im = im.transpose(Image.FLIP_LEFT_RIGHT)
                left = tx + sx * (sox + im.width / z)
            else:
                left = tx + sx * sox
            top = ty + sy * soy
            out.append((im, (left - origin[0]) * ZOOM, (top - origin[1]) * ZOOM))
        return out
    # FFDec's PNG canvas can differ from the SVG's by a few px: align each
    # instance once on the first frame against the page as rendered with it.
    first = placed(0)
    fix = []
    for im, x, y in first:
        try:
            X, Y = controller_signs.snap(page_original, im, round(x), round(y), reach=3)
        except TypeError:   # too few opaque pixels to align on: keep the computed place
            X, Y = round(x), round(y)
        print(f'anim {label}: {im.size} at {x:.1f},{y:.1f} -> {X - round(x)},{Y - round(y)}')
        fix.append((X - round(x), Y - round(y)))
    frames = []
    W, H = -(-page_original.width // 4) * 4, -(-page_original.height // 4) * 4
    for t in range(cycle):
        canvas = Image.new('RGBA', (W, H))
        for (im, x, y), (dx, dy) in zip(placed(t), fix):
            X, Y = round(x) + dx, round(y) + dy
            sx0, sy0 = max(0, -X), max(0, -Y)
            canvas.alpha_composite(im.crop((sx0, sy0, im.width, im.height)), (X + sx0, Y + sy0))
        bb = canvas.getchannel('A').getbbox()
        if bb is None:
            frames.append((0, 0, Image.new('RGBA', (4, 4))))
            continue
        x0, y0 = bb[0] // 4 * 4, bb[1] // 4 * 4
        x1, y1 = min(W, -(-bb[2] // 4) * 4), min(H, -(-bb[3] // 4) * 4)
        frames.append((x0, y0, canvas.crop((x0, y0, x1, y1))))
    return frames
