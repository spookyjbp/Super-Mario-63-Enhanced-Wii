"""The front end's timelines (tools/timeline_art.py, tools/export_front_end.py):
the pack reads back, every clip the port names exists with its frames, and
each set of pages the port uploads together fits the MEM2 area it borrows."""
import re, struct, sys, unittest
from pathlib import Path
ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tools'))
import timeline_art as T

GEN = ROOT / 'assets' / 'generated'


def reserve():
    m = re.search(r'#define COURSE_MEM2_RESERVE \((\d+)u \* 1024 \* 1024\)', (ROOT / 'include/course_art.h').read_text())
    return int(m.group(1)) * 1024 * 1024


class FrontEndTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.data = (GEN / 'front_end.bin').read_bytes()
        cls.layout = (GEN / 'front_end_layout.h').read_text()

    def test_clips(self):
        pages, bakes, clips, frames = T.read_pack(self.data)
        want = {'opening': 341, 'title': 340, 'title_out': 40, 'menu_page': 5, 'btn_box': 92, 'mbg_tree': 20,
                'erase': 35, 'red_trans': 51, 'file_A': 1, 'credits_txt': 1}
        for name, n in want.items():
            self.assertEqual(clips[name][1], n, name)
        # the header's clip numbers follow the pack's order
        order = list(clips)
        named = re.findall(r'#define FE_CLIP_(\w+) (\d+)\n', self.layout)
        self.assertEqual(len(named), len(order))
        for name, k in named:   # names are stored in 16 bytes
            self.assertEqual(order[int(k)].lower(), name.lower()[:16])
        # every draw names a bake on a page
        for fr in frames:
            for d in fr:
                self.assertLess(d[0], len(bakes))
                self.assertLess(bakes[d[0]][0], len(pages))

    def test_sets_fit_mem2(self):
        npages, nbytes = struct.unpack_from('>I', self.data, 8)[0], struct.unpack_from('>I', self.data, 24)[0]
        q, mem = 28, {}
        for _ in range(npages):
            w, h, n, f, g = struct.unpack_from('>HHIBB', self.data, q)
            mem[g] = mem.get(g, 0) + w * h * (2 if f else 4) + 32
            q += 12 + n; q += -q % 4
        groups = {k: int(v) for k, v in re.findall(r'#define FE_GROUP_(\w+) (\d+)', self.layout)}
        sets = [{'OPENING'}, {'TITLE', 'TITLE_OUT'}, {'MENU', 'TITLE_OUT'}]   # front_render.c load_set()
        for s in sets:
            need = nbytes + 32 + sum(mem.get(groups[g], 0) for g in s)
            self.assertLessEqual(need, reserve(), s)

    def test_opening_matches_ffdec_layout(self):
        """Frame 250 of the opening: the Runouw logo's bright green, centred."""
        im = T.preview(self.data, 250)
        import numpy as np
        a = np.asarray(im).astype(int)
        green = (a[..., 1] > 150) & (a[..., 0] < 120)
        ys, xs = np.nonzero(green)
        self.assertGreater(len(xs), 500)
        self.assertAlmostEqual(xs.mean(), 360, delta=60)


if __name__ == '__main__':
    unittest.main()
