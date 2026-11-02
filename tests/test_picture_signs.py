"""The controller picture signs built from the approved design (docs/sign-design,
tools/controller_signs.py): checked on the generated art pack.

* every controller page keeps the original page art outside the keys;
* the keys are gone (the controls page's arrow and Z/X/C keycaps, the stomp
  page's keycaps) and the boxes show Mario's ground-pound demo;
* Toad's talk prompt and the message box exist for every controller.
"""
import sys, unittest
from pathlib import Path
import numpy as np

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tests'))
from test_art_repair import course_layers

LAYERS = {layer: image for area, layer, image in course_layers() if area == '*'}
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

    def test_stomp_boxes_show_the_demo(self):
        def red(region):   # Mario's cap and shirt
            rgb = region[..., :3].astype(int)
            return ((rgb[..., 0] > 180) & (rgb[..., 1] < 80) & (rgb[..., 2] < 80) & (region[..., 3] > 200)).sum()
        boxes = [(12, 147, 160, 275), (324, 147, 472, 275)]
        for x0, y0, x1, y1 in boxes:
            self.assertEqual(red(LAYERS['stomp'][y0:y1, x0:x1]), 0)      # frame 1: no Mario in the box yet
            for k in range(KINDS):
                self.assertGreater(red(LAYERS[f'stomp@{k}'][y0:y1, x0:x1]), 30, (k, x0))

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
