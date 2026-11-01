#!/usr/bin/env python3
"""Repairs for FFDec rasterisation defects in exported art, and bilinear-safe
transparent texels.

FFDec (the renderer used to turn the original SWF into PNGs) leaves defects that
the Flash Player does not draw:

* thin see-through cracks inside solid art. Its repeating bitmap fills leave a
  1-2 px transparent strip where a tile meets the edge of its shape (the wood
  board of Coded Sign 986 has a transparent column next to its left outline),
  and adjacent shapes that share an edge leave a hairline gap between them
  (bushes, ground pieces of the course layers);
* stray edge rows of repeating bitmap fills: the cloud band of
  CastleColoredFarBG (bitmap 2139, fill type 0x42) starts with a row sampled
  from the far edge of the tile, a white line across the whole sky.

`repair` finds these as straight runs (>= RUN px long) of 1-3 px wide lines
whose two sides agree and fills them from the two sides. Real art lines are
kept: colour lines must be a single pixel wide, at least COLOUR_RUN px long and
between two sides of the same colour; alpha cracks must be bounded by opaque
pixels on both sides and miss less than MAX_GAP px of coverage (real gaps in the
art are wider).

`bleed` gives fully transparent texels the colour of their visible neighbours
(alpha stays 0). GX filters non-premultiplied texels, so transparent black
texels darken every edge that is sampled between two texels.
"""
import numpy as np

RUN = 6           # minimum length of an alpha crack
COLOUR_RUN = 96   # minimum length of a 1 px colour line (art lines here are <= 52)
OPAQUE = 240
MAX_GAP = 1.9     # px of missing coverage in a crack
COLOUR_DIFF = 24  # a colour line differs from its sides by more than this
SIDE_DIFF = 16    # ...while its two sides agree within this


def _runs(mask, length):
    """Keep True runs of at least `length` along axis 1."""
    out = np.zeros_like(mask)
    for y in np.where(mask.any(1))[0]:
        row = mask[y]
        edges = np.flatnonzero(np.diff(np.concatenate(([0], row.astype(np.int8), [0]))))
        for s, e in zip(edges[::2], edges[1::2]):
            if e - s >= length:
                out[y, s:e] = True
    return out


def _pass(a):
    """One pass over horizontal lines (cracks/lines spanning along x, thin in y)."""
    h = a.shape[0]
    f = a.astype(np.float32)
    al = f[..., 3]
    changed = 0
    for g in (1, 2, 3):
        if h < g + 2:
            break
        P = f[:h - g - 1]            # row above the line
        Q = f[g + 1:]                # row below the line
        run = [f[1 + k:h - g + k] for k in range(g)]
        pa, qa = P[..., 3], Q[..., 3]
        ra = np.stack([r[..., 3] for r in run])
        both = (pa >= OPAQUE) & (qa >= OPAQUE)
        # An FFDec crack is at most ~1.6 px of missing coverage (1 stage px at
        # zoom 1.6); wider see-through lines (real gaps between outlines) are kept.
        missing = (255 - ra).sum(0) / 255
        crack = both & (ra.min(0) < 200) & (missing < MAX_GAP)
        colour = np.zeros_like(crack)
        if g == 1:
            r = run[0]
            sides = np.abs(P[..., :3] - Q[..., :3]).max(-1) <= SIDE_DIFF
            off = np.abs(r[..., :3] - (P[..., :3] + Q[..., :3]) / 2).max(-1) > COLOUR_DIFF
            colour = both & (r[..., 3] >= OPAQUE) & sides & off
        mask = _runs(crack, RUN) | _runs(colour, COLOUR_RUN)
        if not mask.any():
            continue
        ys, xs = np.nonzero(mask)
        for k in range(g):
            t = (k + 1) / (g + 1)
            fill = P[ys, xs] * (1 - t) + Q[ys, xs] * t
            a[ys + 1 + k, xs] = np.round(fill).astype(np.uint8)
        f = a.astype(np.float32)
        changed += int(mask.sum()) * g
    return changed


def repair(image):
    """Returns (repaired RGBA uint8 array, pixels changed)."""
    a = np.array(image, dtype=np.uint8, copy=True)
    total = 0
    for _ in range(3):
        n = _pass(a)
        t = np.ascontiguousarray(a.transpose(1, 0, 2))
        n += _pass(t)
        a = np.ascontiguousarray(t.transpose(1, 0, 2))
        total += n
        if not n:
            break
    return a, total


def bleed(image, passes=2):
    """Fully transparent texels take the mean colour of their non-transparent
    8-neighbours (alpha stays 0)."""
    a = np.array(image, dtype=np.uint8, copy=True)
    known = a[..., 3] > 0
    for _ in range(passes):
        if known.all() or not known.any():
            break
        rgb = a[..., :3].astype(np.float32) * known[..., None]
        acc = np.zeros_like(rgb)
        cnt = np.zeros(known.shape, np.float32)
        pr = np.pad(rgb, ((1, 1), (1, 1), (0, 0)))
        pk = np.pad(known.astype(np.float32), 1)
        h, w = known.shape
        for dy in (-1, 0, 1):
            for dx in (-1, 0, 1):
                if dy or dx:
                    acc += pr[1 + dy:1 + dy + h, 1 + dx:1 + dx + w]
                    cnt += pk[1 + dy:1 + dy + h, 1 + dx:1 + dx + w]
        fill = (~known) & (cnt > 0)
        a[fill, :3] = np.round(acc[fill] / cnt[fill, None]).astype(np.uint8)
        known = known | fill  # newly coloured texels seed the next pass
    return a
