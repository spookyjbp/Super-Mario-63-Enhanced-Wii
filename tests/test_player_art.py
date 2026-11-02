"""Pack builder (format v2) and movement exporter transforms."""
import struct, sys, tempfile, unittest
from pathlib import Path
from PIL import Image
ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tools'))
from build_player_art import build
import export_movement_clips as E


def tag(code, data):
    return struct.pack('<H', (code << 6) | 63) + struct.pack('<I', len(data)) + data


class BuilderTests(unittest.TestCase):
    def test_trim_dedupe_and_header(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            im = Image.new('RGBA', (8, 8)); im.putpixel((3, 2), (1, 2, 3, 255)); im.save(root / 'a.png')
            im.save(root / 'b.png')
            data = {'clips': [{'id': 0, 'frames': [{'file': 'a.png', 'offset_twips': [-20, 0]}]},
                              {'id': 255, 'frames': [{'file': 'b.png', 'offset_twips': [0, 0]}]}]}
            raw = build(data, root)
            magic, version, w, h, clips, frames, size = struct.unpack('>8s6I', raw[:32])
            self.assertEqual((magic, version, clips, frames), (b'SM63ART1', 2, 2, 2))
            self.assertEqual(len(raw) % 32, 0)
            f0 = struct.unpack('>4H2h2H', raw[32 + 16:32 + 32]); f1 = struct.unpack('>4H2h2H', raw[64:80])
            self.assertEqual(f0[2:6], (1, 1, -20 + 60, 40))      # trimmed to one pixel, offset moved
            self.assertEqual(f0[:2], f1[:2])                      # identical image stored once
            data['clips'][1]['id'] = 256  # MAX_CLIPS (include/art_pack.h ART_CLIP_COUNT)
            with self.assertRaises(ValueError): build(data, root)

    def test_reject_empty_clips(self):
        with self.assertRaises(ValueError): build({'clips': [{'id': 0, 'frames': []}]}, ROOT)


class ExporterTests(unittest.TestCase):
    def test_placement_matches_flash_matrix(self):
        im = Image.new('RGBA', (2, 3)); im.putpixel((0, 0), (255, 0, 0, 255))
        ident = {'sx': 1, 'sy': 1, 'r0': 0, 'r1': 0, 'tx': 100, 'ty': 0}
        out, off = E.place(im, ident, (0, 0))
        self.assertEqual((out.size, off), ((2, 3), (100, -60)))       # + Char (0,-60)
        transpose = {'sx': 0, 'sy': 0, 'r0': 1, 'r1': 1, 'tx': 0, 'ty': 0}
        out, off = E.place(im, transpose, (-20, -40))                 # canvas at (20,40) twips
        self.assertEqual(out.size, (3, 2)); self.assertEqual(off, (40, 20 - 60))
        self.assertEqual(out.getpixel((0, 0)), (255, 0, 0, 255))
        flipped = E.y_flipped(transpose)                               # _yscale *= -1
        out, off = E.place(im, flipped, (0, 0))
        self.assertEqual((out.size, off), ((3, 2), (-60, -60)))
        self.assertEqual(out.getpixel((2, 0)), (255, 0, 0, 255))
        with self.assertRaises(ValueError): E.place(im, dict(ident, sx=0.5), (0, 0))

    def test_collapse_keeps_only_selected_frame(self):
        place_a = tag(26, bytes([0x02]) + struct.pack('<HH', 1, 10))  # depth 1, char 10
        remove = tag(28, struct.pack('<H', 1))
        place_b = tag(26, bytes([0x02]) + struct.pack('<HH', 1, 11))
        show, action, end = tag(1, b''), tag(12, b'\x00'), tag(0, b'')
        body = struct.pack('<HH', 7, 2) + place_a + action + show + remove + place_b + show + end
        raw = b'\x00' * 4 + tag(39, body)
        code, pos, length = next(E.S.tags(raw, 4, len(raw)))
        collapsed = E.collapse(raw, pos, length, 7, 1)
        self.assertEqual(struct.unpack_from('<HH', collapsed), (7, 1))
        inner = list(E.S.tags(collapsed, 4, len(collapsed)))
        self.assertEqual([c for c, _, _ in inner], [26, 28, 26, 1, 0])   # no DoAction, one frame

    def test_fludd_script_rules(self):
        empty = {}
        sprites = {
            1: {'frames': 1, 'labels': {}, 'stage': [{1: {'id': 2}}]},            # root clip
            2: {'frames': 2, 'labels': {}, 'stage': [{1: {'id': 9}}, empty]},     # Fludd Side
            3: {'frames': 4, 'labels': {'H': 1, 'T': 2, 'R': 3}, 'stage': [{}, {}, {}, {}]},
            4: {'frames': 4, 'labels': {}, 'stage': [{1: {'id': 9}}, {1: {'id': 9}}, {1: {'id': 9}}, empty]},
        }
        sprites[1]['stage'] = [{1: {'id': 2}, 2: {'id': 3}, 3: {'id': 4}}]
        raw = b'\x00' * 4 + b''.join(tag(39, struct.pack('<HH', i, 1) + b'Fluddpow') for i in (2, 3, 4))
        choice = E.fludd_frames(sprites, raw, 4, {1})
        self.assertEqual(choice[2], {'': 1, 'H': 0, 'R': 0, 'T': 0})
        self.assertEqual(choice[3]['T'], 2)
        self.assertEqual(choice[4], {'': 3, 'T': 2, 'R': 1, 'H': 0})
        sprites[2]['frames'] = 3
        with self.assertRaises(ValueError): E.fludd_frames(sprites, raw, 4, {1})

    def test_clip_ids_match_wii_layout(self):
        self.assertEqual(E.clip_id('walk', ''), 0)
        self.assertEqual(E.clip_id('Ground', 'T'), 12 * 4 + 3)
        self.assertEqual(E.VARIANTS, ['', 'H', 'R', 'T'])  # Nozzle enum order + none


if __name__ == '__main__':
    unittest.main()
