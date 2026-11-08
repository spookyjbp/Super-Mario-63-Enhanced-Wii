"""The level water (assets/generated/water_places.h, tools/water_art.py): the
original Water Code strips of each tutorial area with their onClipEvent(load)
values, at BGquality "MEDIUM" (3/4 of the points), and the water blocks the
strips mask in 8-E1-1-2 and 8-E1-2."""
import re, unittest
from pathlib import Path

HEADER = (Path(__file__).resolve().parent.parent / 'assets' / 'generated' / 'water_places.h').read_text()


class Water(unittest.TestCase):
    def test_strips(self):
        rows = re.findall(r'\{"([^"]+)", ([-\d.]+)f, ([-\d.]+)f, ([\d.]+)f, ([\d.]+)f, (\d+)u, [\d.]+f, [\d.]+f, '
                          r'\{(\d+), (\d+), (\d+), (\d+)\}, (\d), (-?\d)\}', HEADER)
        got = [(a, float(x), float(y), float(L), float(d), int(n), int(s), int(b)) for a, x, y, L, d, n, *_, s, b in rows]
        self.assertEqual(got, [
            ('8-E1-1', 169.7, 158.75, 120, 400, 11, 0, -1),      # polygons 14, graphics = true
            ('8-E1-1', 448.0, 158.75, 410, 400, 30, 1, -1),      # polygons 40
            ('8-E1-1', 941.0, 158.75, 100, 400, 8, 1, -1),       # polygons 11
            ('8-E1-1-2', 679.3, 139.15, 490, 200, 24, 1, 0),     # polygons 32, over Smaller Water Block
            ('8-E1-2', 1380.8, -63.0, 360, 400, 23, 1, 1),       # polygons 30, over Giant Water Block
            # the Castle Grounds (CastleWater): each over a Giant Water Block; the
            # little pool by the waterfall is tilted 5.6 degrees in the original
            ('Castle', 2122.5, -249.9, 100, 400, 8, 1, 2),        # polygons 11
            ('Castle', -970.0, 163.0, 220, 400, 19, 0, 3),        # polygons 25, graphics = true
            ('Castle', 1154.6, 230.9, 800, 400, 66, 1, 4)])       # polygons 88
        for r in rows:   # 802's default colour, 50 % alpha
            self.assertEqual(tuple(map(int, r[6:10])), (40, 128, 215, 128))

    def test_blocks(self):
        self.assertIn('#define WATER_BLOCKS 5', HEADER)
        self.assertIn('#define WATER_BLOCK_FRAMES 133', HEADER)
        self.assertEqual(len(re.findall(r'\{(-?1), 0\.30\d+f, -778\.75f, -372\.85f, 64\.0f', HEADER)), 10)
        words = re.search(r'water_tile\[(\d+)\]', HEADER)
        self.assertEqual(int(words.group(1)), 32 * 32)
        texels = re.findall(r'0x([0-9a-f]{4})', HEADER.split('water_tile')[1])
        self.assertTrue(all(int(t, 16) & 0x8000 for t in texels))   # opaque bitmap (the layers carry the alpha)


if __name__ == '__main__':
    unittest.main()
