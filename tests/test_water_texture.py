"""Phase 2 item 5: the port-made water textures tile without seams (a water
volume is one repeating quad) and stay translucent."""
import re, sys, unittest
from pathlib import Path
import numpy as np

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tests'))
from test_art_repair import rgb5a3

HEADER = (ROOT / 'assets' / 'generated' / 'water_tex.h').read_text()


def texture(name, w, h):
    body = re.search(name + r'\[\d+\][^{]*\{([^}]*)\}', HEADER).group(1)
    data = bytes(int(v) for v in body.replace('\n', ' ').split(',') if v.strip())
    return rgb5a3(data, w, h).astype(int)


def define(name):
    return int(re.search(r'#define ' + name + r' (\d+)', HEADER).group(1))


class WaterTextureTests(unittest.TestCase):
    def test_body_tiles_both_ways(self):
        b = texture('water_body_tex', define('WATER_BODY_W'), define('WATER_BODY_H'))
        inner = np.abs(np.diff(b, axis=1)).max()                 # largest step inside the tile
        self.assertLessEqual(np.abs(b[:, 0] - b[:, -1]).max(), inner)
        self.assertLessEqual(np.abs(b[0] - b[-1]).max(), np.abs(np.diff(b, axis=0)).max())
        self.assertTrue(((b[..., 3] > 100) & (b[..., 3] < 255)).all())   # translucent everywhere

    def test_surface_frames_tile_and_animate(self):
        w, h, n = define('WATER_SURFACE_W'), define('WATER_SURFACE_H'), define('WATER_SURFACE_FRAMES')
        s = texture('water_surface_tex', w, h * n)
        frames = [s[i * h:(i + 1) * h] for i in range(n)]
        for f in frames:
            self.assertLessEqual(np.abs(f[:, 0, 3] - f[:, -1, 3]).max(), np.abs(np.diff(f[..., 3], axis=1)).max())
            self.assertEqual(f[0, :, 3].max(), 0)                # transparent above the crest
            self.assertGreater(f[-1, :, 3].min(), 100)           # meets the body at the bottom
        self.assertTrue(any((frames[i] != frames[0]).any() for i in range(1, n)))


if __name__ == '__main__':
    unittest.main()
