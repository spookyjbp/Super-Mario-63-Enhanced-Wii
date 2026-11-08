"""The controller picture signs built from the approved design (docs/sign-design,
tools/controller_signs.py): checked on the generated art pack.

* every controller page keeps the original page art outside the keys;
* the keys are gone (the controls page's arrow and Z/X/C keycaps, the stomp
  page's keycaps);
* the moving parts (walking Marios, the stomp demo, the refill drops) animate;
* Toad's talk prompt and the message box exist for every controller.
"""
import sys, unittest
from pathlib import Path
import numpy as np

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tests'))
from test_art_repair import course_layers

LAYERS = {layer: image for area, layer, image in course_layers() if area == '*'}


def anim_layers():
    """Animated layers ("<label>~"): [(x, y, frame image)] per tile."""
    import struct, lz4.block
    from test_art_repair import GEN, rgb5a3
    d = (ROOT / 'assets/data/course_8e1_art.bin').read_bytes()
    n = struct.unpack_from('>8sIIf', d)[2]; q = 20; out = {}
    def string():
        nonlocal q
        L = struct.unpack_from('>H', d, q)[0]; s = d[q + 2:q + 2 + L].decode(); q += 2 + L; return s
    for _ in range(n):
        area, name = string(), string()
        q += 24
        count = struct.unpack_from('>I', d, q)[0]; q += 4
        tiles = []
        for _ in range(count):
            tx, ty, tw, th, size, packed = struct.unpack_from('>HHHHII', d, q); q += 16
            if name.endswith('~'):
                tiles.append((tx, ty, rgb5a3(lz4.block.decompress(d[q:q + packed], uncompressed_size=size), tw, th)))
            q += packed; q += (-q) % 4
        if name.endswith('~'):
            out[name] = tiles
    return out


ANIM = anim_layers()
KINDS = 3
# (x0, y0, x1, y1) of each page that must stay the original art
KEPT = {'controls': [(0, 0, 487, 70)],                       # "Controls" title
        'stomp': [(0, 20, 380, 140)],                         # the text and arrow
        'bomb': [(0, 0, 350, 239)],
        'hover': [(0, 70, 573, 288), (0, 0, 370, 70)],
        'zoom': [(0, 0, 487, 200)]}


def mean_diff(a, b):
    return np.abs(a.astype(int) - b.astype(int)).mean()


class PictureSigns(unittest.TestCase):
    def test_pages_keep_the_original_art(self):
        for label, boxes in KEPT.items():
            orig = LAYERS[label]
            for k in range(KINDS):
                page = LAYERS[f'{label}@{k}']
                self.assertEqual(page.shape, orig.shape, label)
                for x0, y0, x1, y1 in boxes:
                    self.assertLess(mean_diff(page[y0:y1, x0:x1], orig[y0:y1, x0:x1]), 1.0, (label, k, x0, y0))
                self.assertGreater(mean_diff(page, orig), 1.0, (label, k))   # something was replaced

    def test_keys_are_gone(self):
        # the original keycaps: flat grey faces (172 after RGB5A3) with lighter rims; the new
        # buttons use other tones (GameCube colours, a darker grey face)
        def keycap_grey(region):
            rgb = region[..., :3].astype(int)
            flat = (rgb.max(-1) - rgb.min(-1)) < 8
            v = rgb.mean(-1)
            return ((region[..., 3] > 240) & flat & (np.abs(v - 172) < 4)).sum()
        zxc = (250, 160, 475, 250)                       # Z / X / C keycaps on the controls page
        x0, y0, x1, y1 = zxc
        self.assertGreater(keycap_grey(LAYERS['controls'][y0:y1, x0:x1]), 1500)
        for k in range(KINDS):
            page = LAYERS[f'controls@{k}']
            self.assertLess(keycap_grey(page[y0:y1, x0:x1]), 400, k)   # only anti-aliased edges (the original: ~4800)

    def test_pages_animate(self):
        """The moving parts play over the still page, one frame per tile, as long
        as the original's cycle: the controls page's walking Marios (80), the
        stomp boxes' ground-pound demo (101), the refill page's drops (123)."""
        def red(region):   # Mario's cap and shirt
            rgb = region[..., :3].astype(int)
            return ((rgb[..., 0] > 180) & (rgb[..., 1] < 80) & (rgb[..., 2] < 80) & (region[..., 3] > 200)).sum()
        for label, count in (('controls', 80), ('stomp', 101), ('refill', 123)):
            frames = ANIM[label + '~']
            self.assertEqual(len(frames), count, label)
            pictures = {f[2].tobytes() for f in frames}
            self.assertGreater(len(pictures), count // 8, label)            # it moves
            page = LAYERS[label]
            for x, y, im in frames:   # every frame lies on the page
                self.assertLessEqual(x + im.shape[1], -(-page.shape[1] // 4) * 4)
                self.assertLessEqual(y + im.shape[0], -(-page.shape[0] // 4) * 4)
        # the boxes' Mario is only in the animation, in both boxes
        for x0, x1 in ((12, 160), (324, 472)):
            self.assertEqual(red(LAYERS['stomp'][147:275, x0:x1]), 0)
            seen = 0
            for x, y, im in ANIM['stomp~']:
                canvas = np.zeros(LAYERS['stomp'].shape[:2] + (4,), np.uint8)
                h, w = im.shape[:2]
                canvas[y:y + h, x:x + w] = im[:canvas.shape[0] - y, :canvas.shape[1] - x]
                seen += red(canvas[147:275, x0:x1]) > 30
            self.assertGreater(seen, 50, x0)
        # the controls page's walking Marios are not in the still page
        self.assertLess(red(LAYERS['controls@0'][90:160, 0:230]), red(LAYERS['controls'][90:160, 0:230]) + 1)

    def test_faint_screens(self):
        """The faint's screens fit the port's 320 KiB frame slot: the star wipes
        (Blackout 13 frames, StarIn 12) and the lives screen (59)."""
        for name, count in (('StarOut~', 13), ('StarIn~', 12), ('LifeLost~', 59), ('WipeRight~', 17), ('WipeRight2~', 18),
                            ('WipeLeft~', 17), ('WipeLeft2~', 17), ('WipeDown~', 16), ('WipeDown2~', 17)):
            frames = ANIM[name]
            self.assertEqual(len(frames), count, name)
            for x, y, im in frames:
                self.assertLessEqual(im.shape[0] * im.shape[1] * 2, 320 * 1024, name)
        # the lives screen shows something (the Mario icon and number, the explosion)
        self.assertGreater(sum(im.shape[0] * im.shape[1] > 64 for _, _, im in ANIM['LifeLost~']), 40)

    def test_area_text(self):
        """AreaTextClip (area_text_art.py): the bar and "Mushroom Kingdom (Tutorial)"
        blur in over 7514 frames 2-11, hold, and blur out over 85-94; each
        distinct frame is one tile that fits the 320 KiB frame slot; the hold is
        one tile with the bar fully shown (frame 45's colour matrix: a see-through
        dark green, ~57 % alpha)."""
        import re
        frames = ANIM['AreaText~']
        head = (ROOT / 'assets' / 'generated' / 'area_text_layout.h').read_text()
        table = [int(v) for v in re.search(r'\{\n    ([\d, ]+)', head).group(1).split(', ')]
        self.assertEqual(len(table), 94)
        self.assertEqual(len(set(table[11:84])), 1)                    # frames 12-84: the hold
        self.assertEqual(max(table) + 1, len(frames))
        for x, y, im in frames:
            self.assertLessEqual(im.shape[0] * im.shape[1] * 2, 320 * 1024)
        hold = frames[table[40]][2]
        rgb = hold[..., :3].astype(int)
        bar = (abs(hold[..., 3].astype(int) - 145) < 20) & (rgb[..., 1] > rgb[..., 0] + 40) & (rgb[..., 1] > rgb[..., 2] + 40)
        self.assertGreater(bar.sum(), 15000)
        white = (hold[..., 3] > 240) & (rgb.min(-1) > 230)
        self.assertGreater(white.sum(), 600)                           # the text

    def test_talk_prompt_and_message_box(self):
        self.assertIn('Message', LAYERS)
        for k in range(KINDS):
            for name in (f'Prompt@{k}', f'TalkZ@{k}'):
                img = LAYERS[name]
                self.assertGreater((img[..., 3] > 200).sum(), 200, name)
            z = LAYERS[f'TalkZ@{k}']
            half = z.shape[1] // 2
            # two poses of the same button, tilted differently
            self.assertGreater(mean_diff(z[:, :half], z[:, half:2 * half]), 1.0)


if __name__ == '__main__':
    unittest.main()
