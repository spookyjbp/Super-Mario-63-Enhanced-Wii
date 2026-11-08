"""The pause menu art (tools/pause_art.py) and the trophy texture
(tools/export_trophies.py)."""
import re, struct, unittest
from pathlib import Path
import numpy as np
ROOT = Path(__file__).resolve().parent.parent
GEN = ROOT / 'assets' / 'generated'


def rgba8(data, w, h):
    """GX RGBA8 tiles to an h x w x 4 array."""
    out = np.zeros((h, w, 4), np.uint8)
    blk = 0
    for ty in range(0, h, 4):
        for tx in range(0, w, 4):
            b = data[blk * 64:(blk + 1) * 64]
            for i in range(16):
                y, x = ty + i // 4, tx + i % 4
                out[y, x] = (b[2 * i + 1], b[32 + 2 * i], b[32 + 2 * i + 1], b[2 * i])
            blk += 1
    return out


class PauseArtTests(unittest.TestCase):
    def test_trophies(self):
        """Four cups (Bronze, Silver, Gold, Platinum) in square cells: clear
        corners, a solid cup in the middle, each its own colour."""
        d = (GEN / 'trophies.bin').read_bytes()
        self.assertEqual(d[:8], b'SM63TRO1')
        cell, count = struct.unpack_from('>II', d, 8)
        self.assertEqual(count, 4)
        im = rgba8(d[32:], cell * count, cell)
        hues = []
        for k in range(count):
            c = im[:, k * cell:(k + 1) * cell]
            self.assertEqual(c[0, 0, 3], 0)
            self.assertEqual(c[-1, -1, 3], 0)
            self.assertGreater((c[..., 3] > 200).mean(), 0.3)
            solid = c[c[..., 3] > 200][:, :3].astype(int)
            hues.append(solid.mean(0))
        bronze, silver, gold, plat = hues
        self.assertGreater(bronze[0], bronze[2] + 40)     # warm
        self.assertLess(abs(silver[0] - silver[2]), 30)   # grey
        self.assertGreater(gold[1], gold[2] + 60)         # yellow
        self.assertGreater(plat[2], plat[0] + 40)         # blue

    def test_pause_layout(self):
        """Every page has its four buttons; the parts the renderer names exist."""
        h = (GEN / 'pause_layout.h').read_text()
        parts = re.findall(r'\{"(\w+)", \d+, \d+, \d+, \d+', h)
        for name in ('btn_achievements', 'btn_back', 'title_achievements', 'exit_quit', 'exit_exitlevel', 'opt_mute',
                     'noz_none', 'noz_hover_locked', 'mute_on_4', 'mute_off_4', 'coin_16',
                     # the row-style pages: each page's original title, the Star Map's icons
                     'title_paused', 'title_starmap', 'title_fludd', 'title_options', 'title_exit',
                     'shine_off', 'shine_on', 'shine_all', 'scoin_off', 'scoin_on'):
            self.assertIn(name, parts)
        rows = re.findall(r'\{\{PP_(\w+), [-\d.]+f, [-\d.]+f\}, \{PP_(\w+)[^/]*/\* (\w+) \*/', h)
        self.assertEqual([r[-1] for r in rows], ['default', 'starmap', 'fludd', 'options', 'exit', 'refresh'])


    def test_parts_fit_their_memory(self):
        """course_render.c uploads PauseParts into PAUSE_PARTS_BYTES (RGB5A3)."""
        src = (ROOT / 'source/platform/course_render.c').read_text()
        a, b = map(int, re.search(r'#define PAUSE_PARTS_BYTES \((\d+)u \* (\d+)\)', src).groups())
        import sys
        sys.path.insert(0, str(ROOT / 'tools'))
        art = (ROOT / 'assets/data/course_8e1_art.bin').read_bytes()
        at = art.find(b'PauseParts')
        self.assertGreater(at, 0)
        w, h = struct.unpack_from('>II', art, at + len('PauseParts') + 16)
        self.assertLessEqual(w * h * 2, a * b)


if __name__ == '__main__':
    unittest.main()
