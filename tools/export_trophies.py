#!/usr/bin/env python3
"""The four achievement trophies (Bronze, Silver, Gold, Platinum: Shine Sprite
cups, assets/art/trophies.png, the project owner's art) as one GX RGBA8
texture: the white backdrop made transparent (flood fill from the edges),
each cup trimmed, centred and scaled into a SIZE x SIZE cell, side by side.

Output: <out>/trophies.bin ("SM63TRO1", u32 cell size, u32 count, 16 zero
bytes, then RGBA8 tiles of a (count*SIZE) x SIZE texture).

    python export_trophies.py <out_dir>
"""
import argparse, struct
from collections import deque
from pathlib import Path
import numpy as np
from PIL import Image
from build_mario_atlas import rgba8_tiles

SIZE = 64
HOLE_MIN = 600   # source px
SOURCE = Path(__file__).resolve().parent.parent / 'assets' / 'art' / 'trophies.png'


def cut(path=SOURCE):
    im = np.asarray(Image.open(path).convert('RGB')).astype(int)
    h, w, _ = im.shape
    light = (im.min(2) > 200) & (im.max(2) - im.min(2) < 30)   # the white backdrop and its noise
    outside = np.zeros((h, w), bool)
    q = deque((y, x) for y in range(h) for x in (0, w - 1)) + deque((y, x) for x in range(w) for y in (0, h - 1))
    while q:
        y, x = q.pop()
        if 0 <= y < h and 0 <= x < w and light[y, x] and not outside[y, x]:
            outside[y, x] = True
            q.extend(((y + 1, x), (y - 1, x), (y, x + 1), (y, x - 1)))
    # the holes inside the handles: enclosed, so the fill misses them; they are
    # large pure-white areas walled in by the black outline (the cups' white
    # highlights border coloured metal instead)
    from scipy import ndimage
    white = (im.min(2) > 240) & (im.max(2) - im.min(2) < 12) & ~outside
    labels, n = ndimage.label(white)
    sizes = ndimage.sum(white, labels, range(1, n + 1))
    dark = im.max(2) < 90
    for k, size in enumerate(sizes, 1):
        if size <= HOLE_MIN:
            continue
        comp = labels == k
        ring = ndimage.binary_dilation(comp, iterations=3) & ~comp & ~light
        if ring.any() and dark[ring].mean() > 0.6:   # walled in by the black outline: a hole
            outside |= ndimage.binary_dilation(comp, iterations=1) & light
    alpha = (~outside).astype(np.uint8) * 255
    cols = np.nonzero((~outside).any(0))[0]
    runs, r0 = [], cols[0]
    for a, b in zip(cols, cols[1:]):
        if b - a > 8:
            runs.append((r0, a + 1)); r0 = b
    runs.append((r0, cols[-1] + 1))
    if len(runs) != 4:
        raise ValueError(f'expected 4 trophies, found {len(runs)}')
    rgba = np.dstack([im.astype(np.uint8), alpha])
    cups = []
    for l, r in runs:
        part = Image.fromarray(rgba[:, l:r])
        part = part.crop(part.getbbox())
        k = (SIZE - 2) / max(part.size)
        part = part.resize((max(1, round(part.width * k)), max(1, round(part.height * k))), Image.LANCZOS)
        cell = Image.new('RGBA', (SIZE, SIZE))
        cell.alpha_composite(part, ((SIZE - part.width) // 2, (SIZE - part.height) // 2))
        cups.append(cell)
    return cups


def build(out_dir):
    cups = cut()
    sheet = Image.new('RGBA', (SIZE * len(cups), SIZE))
    for i, c in enumerate(cups):
        sheet.alpha_composite(c, (i * SIZE, 0))
    out_dir.mkdir(parents=True, exist_ok=True)
    (out_dir / 'trophies.bin').write_bytes(b'SM63TRO1' + struct.pack('>II', SIZE, len(cups)) + bytes(16) +
                                           rgba8_tiles(sheet))
    return sheet


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('out_dir', type=Path)
    sheet = build(p.parse_args().out_dir)
    sheet.save('/tmp/trophies_preview.png')
    print('Wrote trophies', sheet.size)
