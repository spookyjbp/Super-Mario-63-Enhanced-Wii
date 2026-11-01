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

`repair` fills
* cracks: 1-3 px see-through lines between opaque pixels, missing less than
  MAX_GAP px of coverage (real gaps in the art are wider), forming a straight or
  diagonal line at least RUN px long; filled from their opaque neighbours;
* colour seams: 1 px lines between two agreeing sides, either at least
  COLOUR_RUN px long (real art lines here are <= 52) or cutting across
  differently coloured art (sides changing by more than CROSS along the line,
  where adjoining pieces leak the colour behind them); filled from both sides.
Most stray edge rows are prevented at the source (swf_bitmap_fix.py).

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
CROSS = 64        # a shorter colour line is a seam when its sides change by more than this


def _runs(mask, length, keep=None):
    """Keep True runs of at least `length` along axis 1 (and keep(y, s, e))."""
    out = np.zeros_like(mask)
    for y in np.where(mask.any(1))[0]:
        row = mask[y]
        edges = np.flatnonzero(np.diff(np.concatenate(([0], row.astype(np.int8), [0]))))
        for s, e in zip(edges[::2], edges[1::2]):
            if e - s >= length and (keep is None or keep(y, s, e)):
                out[y, s:e] = True
    return out


def _pass(a):
    """Colour seams along x (thin in y): a 1 px row that differs from the rows
    above and below while those agree, and is either very long (COLOUR_RUN) or
    crosses differently coloured art (CROSS)."""
    f = a.astype(np.float32)
    if f.shape[0] < 3:
        return 0
    P, r, Q = f[:-2], f[1:-1], f[2:]
    both = (P[..., 3] >= OPAQUE) & (Q[..., 3] >= OPAQUE) & (r[..., 3] >= OPAQUE)
    sides = np.abs(P[..., :3] - Q[..., :3]).max(-1) <= SIDE_DIFF
    off = np.abs(r[..., :3] - (P[..., :3] + Q[..., :3]) / 2).max(-1) > COLOUR_DIFF
    colour = both & sides & off

    def crosses(y, s, e):
        """A seam between two placed pieces cuts straight across the art
        (outline, bush, ground...): its sides change colour along it. A real
        1 px line (highlight, mortar, outline) runs inside one area."""
        seg = P[y, s:e, :3]
        return float((seg.max(0) - seg.min(0)).max()) > CROSS
    mask = _runs(colour, COLOUR_RUN) | _runs(colour, RUN, crosses)
    if not mask.any():
        return 0
    ys, xs = np.nonzero(mask)
    a[ys + 1, xs] = np.round((P[ys, xs] + Q[ys, xs]) / 2).astype(np.uint8)
    return int(mask.sum())


def _crack_candidates(a):
    """Pixels of 1-3 px see-through lines between opaque pixels (one axis)."""
    al = a[..., 3].astype(np.float32)
    h = al.shape[0]
    out = np.zeros(al.shape, bool)
    for g in (1, 2, 3):
        if h < g + 2:
            break
        ra = np.stack([al[1 + k:h - g + k] for k in range(g)])
        both = (al[:h - g - 1] >= OPAQUE) & (al[g + 1:] >= OPAQUE)
        # An FFDec crack is at most ~1.6 px of missing coverage (1 stage px at
        # zoom 1.6); wider see-through lines (real gaps between outlines) are kept.
        hit = both & (ra.min(0) < OPAQUE) & ((255 - ra).sum(0) / 255 < MAX_GAP)
        for k in range(g):
            out[1 + k:h - g + k] |= hit & (ra[k] < OPAQUE)
    return out


def _cracks(a, seed=None):
    """Fill cracks: candidate pixels (either axis) forming 8-connected lines at
    least RUN px long, straight or diagonal (pieces meeting on a slope), or
    touching `seed` (pixels filled by the previous call, e.g. where two cracks
    cross). Returns the filled mask."""
    from scipy import ndimage
    cand = _crack_candidates(a) | _crack_candidates(a.transpose(1, 0, 2)).T
    if not cand.any():
        return cand
    lab, n = ndimage.label(cand, structure=np.ones((3, 3)))
    keep = np.zeros(n + 1, bool)
    for i, sl in enumerate(ndimage.find_objects(lab), 1):
        keep[i] = max(sl[0].stop - sl[0].start, sl[1].stop - sl[1].start) >= RUN
    if seed is not None:
        keep[np.unique(lab[ndimage.binary_dilation(seed, structure=np.ones((3, 3))) & cand])] = True
        keep[0] = False
    mask = keep[lab]
    filled = mask.copy()
    h, w = mask.shape
    for _ in range(4):
        if not mask.any():
            break
        f = a.astype(np.float32)
        good = (~mask) & (f[..., 3] >= OPAQUE)
        pf = np.pad(f * good[..., None], ((1, 1), (1, 1), (0, 0)))
        pg = np.pad(good.astype(np.float32), 1)
        acc = np.zeros_like(f); cnt = np.zeros(mask.shape, np.float32)
        for dy in (-1, 0, 1):
            for dx in (-1, 0, 1):
                if dy or dx:
                    acc += pf[1 + dy:1 + dy + h, 1 + dx:1 + dx + w]
                    cnt += pg[1 + dy:1 + dy + h, 1 + dx:1 + dx + w]
        fill = mask & (cnt > 0)
        a[fill] = np.round(acc[fill] / cnt[fill, None]).astype(np.uint8)
        mask &= ~fill
    return filled


def repair(image):
    """Returns (repaired RGBA uint8 array, pixels changed)."""
    a = np.array(image, dtype=np.uint8, copy=True)
    filled = _cracks(a)
    total = int(filled.sum())
    while filled.any():  # crossings left between filled cracks
        filled = _cracks(a, filled)
        total += int(filled.sum())
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
