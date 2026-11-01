"""Bugs #5 / #8: FFDec rasterisation defects are repaired before the art is packed.

* unit tests of tools/art_repair.py on synthetic images (cracks and stray lines
  are filled; real 1 px art lines, short lines and wide gaps are kept);
* the generated course art has no full-width line across the sky
  (CastleColoredFarBG rows 173 / 283) and no transparent crack inside opaque art;
* the generated sign (Coded Sign 986) has no see-through column in its board.
"""
import re, struct, sys, unittest
from pathlib import Path
import numpy as np
import lz4.block

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tools'))
import art_repair as R

GEN = ROOT / 'assets' / 'generated'


def rgb5a3(raw, w, h):
    px = np.frombuffer(raw, dtype='>u2').reshape(h // 4, w // 4, 4, 4).transpose(0, 2, 1, 3).reshape(h, w)
    px = px.astype(np.uint32)
    op = (px & 0x8000) != 0
    r = np.where(op, ((px >> 10) & 31) * 255 // 31, ((px >> 8) & 15) * 17)
    g = np.where(op, ((px >> 5) & 31) * 255 // 31, ((px >> 4) & 15) * 17)
    b = np.where(op, (px & 31) * 255 // 31, (px & 15) * 17)
    a = np.where(op, 255, ((px >> 12) & 7) * 255 // 7)
    return np.dstack([r, g, b, a]).astype(np.uint8)


def course_layers():
    d = (GEN / 'course_8e1_art.bin').read_bytes()
    _, _, n, _ = struct.unpack_from('>8sIIf', d)
    q = 20

    def string():
        nonlocal q
        length = struct.unpack_from('>H', d, q)[0]
        s = d[q + 2:q + 2 + length].decode()
        q += 2 + length
        return s
    for _ in range(n):
        area, layer = string(), string()
        q += 16
        width, height = struct.unpack_from('>II', d, q)
        q += 8
        count = struct.unpack_from('>I', d, q)[0]
        q += 4
        image = np.zeros((height, width, 4), np.uint8)
        for _ in range(count):
            tx, ty, tw, th, size, packed = struct.unpack_from('>HHHHII', d, q)
            q += 16
            raw = lz4.block.decompress(d[q:q + packed], uncompressed_size=size)
            q += packed
            q += (-q) % 4
            image[ty:ty + th, tx:tx + tw] = rgb5a3(raw, tw, th)
        yield area, layer, image


def full_width_lines(image):
    """Rows that differ from both neighbours (which agree) across >= 90% of the width."""
    f = image.astype(int)
    up, row, dn = f[:-2], f[1:-1], f[2:]
    opaque = (up[..., 3] > 240) & (row[..., 3] > 240) & (dn[..., 3] > 240)
    sides = np.abs(up[..., :3] - dn[..., :3]).max(-1) <= R.SIDE_DIFF
    off = np.abs(row[..., :3] - (up[..., :3] + dn[..., :3]) / 2).max(-1) > R.COLOUR_DIFF
    hit = (opaque & sides & off).mean(1) > 0.9
    return list(np.flatnonzero(hit) + 1)


def cracks(image, length=R.RUN):
    """1-3 px transparent lines between opaque pixels, >= length px long (both axes)."""
    total = 0
    for a in (image, image.transpose(1, 0, 2)):
        al = a[..., 3].astype(int)
        for g in (1, 2, 3):
            ends = (al[:-g - 1] >= R.OPAQUE) & (al[g + 1:] >= R.OPAQUE)
            run = np.stack([al[1 + k:al.shape[0] - g + k] for k in range(g)])
            gap = (run.min(0) < 100) & ((255 - run).sum(0) / 255 < R.MAX_GAP)
            total += int(R._runs(ends & gap, length).sum())
    return total


class RepairUnitTests(unittest.TestCase):
    def solid(self, h=40, w=40, colour=(100, 150, 60)):
        a = np.zeros((h, w, 4), np.uint8)
        a[...] = (*colour, 255)
        return a

    def test_crack_is_filled(self):
        a = self.solid()
        a[5:35, 20, 3] = 0           # 1 px see-through column (like the sign board)
        a[12, 3:30, 3] = 60          # horizontal crack, partial alpha
        b, n = R.repair(a)
        self.assertGreater(n, 0)
        self.assertTrue((b[..., 3] == 255).all())
        self.assertTrue((b[..., :3] == a[0, 0, :3]).all())

    def test_sub_two_pixel_crack_between_colours_is_filled(self):
        a = self.solid()
        a[:, :20, :3] = (0, 0, 0)     # outline | crack | board, as in sign 986 columns 1-3
        a[2:30, 20, 3], a[2:30, 21, 3], a[2:30, 22, 3] = 153, 0, 163
        b, _ = R.repair(a)
        self.assertTrue((b[2:30, 20:23, 3] == 255).all())

    def test_full_width_stray_line_is_removed(self):
        a = self.solid(200, 300, (188, 196, 248))
        a[100, :, :3] = (248, 248, 248)  # CastleColoredFarBG row 173
        b, _ = R.repair(a)
        self.assertEqual(full_width_lines(b), [])
        self.assertTrue((np.abs(b[100].astype(int) - a[99]) <= 1).all())

    def test_real_art_lines_are_kept(self):
        a = self.solid(200, 200)
        a[50, 10:60, :3] = 0          # 50 px black outline: real art (<= 52 px in the course)
        a[80:140, 70, :3] = (33, 115, 49)  # highlight column
        a[:, 150:154, 3] = 0          # 4 px wide transparent gap: real gap
        a[:, 120:123, 3] = 0          # 3 px wide gap between outlines (8-E1-2 BackGFX): real gap
        a[:, 130:132, 3] = 0          # 2 px fully clear: real gap
        a[10:14, 100, 3] = 0          # 4 px long crack: too short to be a seam
        b, _ = R.repair(a)
        self.assertTrue((b == a).all())

    def test_bleed_colours_only_transparent_texels(self):
        a = np.zeros((8, 8, 4), np.uint8)
        a[2:6, 2:6] = (200, 100, 50, 255)
        b = R.bleed(a)
        self.assertTrue((b[..., 3] == a[..., 3]).all())
        self.assertTrue((b[2:6, 2:6] == a[2:6, 2:6]).all())
        self.assertTrue((b[1, 1, :3] == (200, 100, 50)).all())   # no black fringe for bilinear
        self.assertTrue((b[0, 0, :3] == (200, 100, 50)).all())


class GeneratedArtTests(unittest.TestCase):
    def test_course_layers_have_no_sky_lines_or_cracks(self):
        seen = set()
        for area, layer, image in course_layers():
            if area == '*':  # picture-sign pages: line drawings, not repaired
                continue
            key = (layer, image.shape, int(image[..., 3].sum()))
            if key in seen:
                continue
            seen.add(key)
            with self.subTest(area=area, layer=layer):
                self.assertEqual(full_width_lines(image), [])
                self.assertEqual(cracks(image), 0)

    def test_sign_board_is_solid(self):
        header = (GEN / 'object_art_ids.h').read_text()
        clip, pack = map(int, re.search(r'OBJ_SIGN = (\d+), /\* pack (\d+)', header).groups())
        d = (GEN / f'objects_8e1_{pack}.bin').read_bytes()
        _, version, w, h, nclips, nframes, size = struct.unpack_from('>8s6I', d)
        clips = [struct.unpack_from('>4H', d, 32 + 8 * i) for i in range(nclips)]
        base = 32 + 8 * nclips
        first = next(c[1] for c in clips if c[0] == clip)
        x, y, fw, fh = struct.unpack_from('>4H2h2H', d, base + 16 * first)[:4]
        q = base + 16 * nframes
        q += (-q) % 32
        raw_size = struct.unpack_from('>I', d, q)[0]
        atlas = rgb5a3(lz4.block.decompress(d[q + 4:q + 4 + size - 4], uncompressed_size=raw_size), w, h)
        sign = atlas[y:y + fh, x:x + fw]
        board = sign[:int(fh * 0.55)]               # the board (the post starts lower)
        self.assertEqual(cracks(sign), 0)
        # no transparent texel strictly inside the board's outline
        inner = board[3:-3, 1:-3, 3]   # from column 1: the old crack was columns 1-3
        self.assertGreaterEqual(int(inner.min()), 255 * 4 // 7)


if __name__ == '__main__':
    unittest.main()
