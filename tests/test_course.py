"""Course exporter: AS2 parameter parsing and mask run encoding."""
import struct, sys, unittest
from pathlib import Path
from PIL import Image
ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tools'))
import export_course as C


class ParamTests(unittest.TestCase):
    def test_sign_text_and_functions(self):
        action = '''onClipEvent (load) {
	//signmessage = "old";
	picture = true;
	signmessage = "Line one."+newline+"Line \\"two\\"";
	othercamcenter = _root.Course.BackGFX.target3;
	completefunction = function () {
		_root.Course.Platforms.gotoAndStop(2);
	};
}'''
        p = C.load_params([action])
        self.assertEqual(p['signmessage'], 'Line one.\nLine "two"')
        self.assertEqual(p['picture'], 'true')
        self.assertEqual(p['othercamcenter'], '_root.Course.BackGFX.target3')
        self.assertEqual(p['completefunction'], '_root.Course.Platforms.gotoAndStop(2);')

    def test_nested_assignments_are_not_parameters(self):
        action = 'onClipEvent (load) {\n\tM1Sta = function () {\n\t\tx = 5;\n\t};\n\tM1Txt = "Hi "+_root.CurrentPlayer;\n}'
        p = C.load_params([action, 'onClipEvent (enterFrame) {\n}'])
        self.assertEqual(p, {'M1Sta': 'function', 'M1Txt': 'Hi Mario', '_enterFrame': 'true'})


class MaskTests(unittest.TestCase):
    def test_runs_threshold_alpha_like_hittest(self):
        im = Image.new('RGBA', (6, 2))
        for x, a in enumerate([0, 128, 255, 127, 200, 200]):
            im.putpixel((x, 0), (0, 0, 0, a))
        self.assertEqual(C.mask_rows(im), [[(1, 2), (4, 2)], []])

    def test_pack_layout_has_row_offsets(self):
        area = {'id': 'T', 'masks': [{'layer': 'Platforms', 'frame': 1, 'x0': -2, 'y0': 3, 'width': 6, 'height': 2,
                                       'rows': [[(1, 2)], []]}], 'water': [(1, 2, 3, 4)], 'objects': []}
        raw = C.build([area])
        self.assertEqual(raw[:8], b'SM63CRS1')
        self.assertEqual(len(raw) % 32, 0)
        p = 16 + 4 + 2 + 1 + 4  # area size, id string, mask count
        layer, frame, _, x0, y0, w, h = struct.unpack_from('>BBhiiII', raw, p)
        self.assertEqual((layer, frame, x0, y0, w, h), (0, 1, -2, 3, 6, 2))
        offsets = struct.unpack_from('>2I', raw, p + 20)
        self.assertEqual(offsets, (0, 6))  # row 0: count + one run; row 1 follows


if __name__ == '__main__':
    unittest.main()
