"""Object art packs: frames must sit where the original places them (Bug 4: Toad
was drawn 30 px low, inside the ground, because trimmed frames added image
pixels as if they were course pixels)."""
import re, struct, sys, unittest
from pathlib import Path
ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tools'))
from PIL import Image
from build_player_art import build

GEN = ROOT / 'assets' / 'generated'


def ids():
    out, zoom = {}, None
    for line in (GEN / 'object_art_ids.h').read_text().splitlines():
        m = re.match(r'\s+OBJ_(\w+) = (\d+), /\* pack (\d+) \*/', line)
        if m: out[m.group(1)] = (int(m.group(2)), int(m.group(3)))
        z = re.match(r'#define OBJECT_ART_ZOOM ([\d.]+)f', line)
        if z: zoom = float(z.group(1))
    return out, zoom


def frames(pack, clip):
    data = (GEN / f'objects_8e1_{pack}.bin').read_bytes()
    magic, version, w, h, nclips, nframes, size = struct.unpack_from('>8s6I', data)
    clips = [struct.unpack_from('>4H', data, 32 + 8 * i) for i in range(nclips)]
    base = 32 + 8 * nclips
    for ident, first, count, flags in clips:
        if ident == clip:
            return [struct.unpack_from('>4H2h2H', data, base + 16 * (first + k)) for k in range(count)]
    raise KeyError(clip)


class ObjectArtTests(unittest.TestCase):
    def test_toad_stands_on_his_origin(self):
        table, zoom = ids()
        clip, pack = table['TOAD_IDLE']
        x, y, w, h, ox, oy, duration, _ = frames(pack, clip)[0]
        left, top = ox / 20, oy / 20                # course px from the registration point
        bottom = top + h / zoom
        self.assertAlmostEqual(left + w / zoom / 2, 0, delta=3)   # centred
        self.assertAlmostEqual(bottom, 13.9, delta=1.5)           # feet: body placed at y 13.9

    def test_sign_post_ends_at_origin(self):
        table, zoom = ids()
        clip, pack = table['SIGN']
        x, y, w, h, ox, oy, duration, _ = frames(pack, clip)[0]
        self.assertAlmostEqual(oy / 20 + h / zoom, 0, delta=1)

    def test_trim_offset_uses_pixel_scale(self):
        tmp = Path('/tmp') / 'sm63_trim.png'
        im = Image.new('RGBA', (40, 40)); im.putpixel((16, 24), (255, 0, 0, 255)); im.save(tmp)
        blob = build({'clips': [{'id': 0, 'frames': [{'file': str(tmp), 'offset_twips': [0, 0]}]}]}, Path('/'),
                     px_twips=20 / 1.6)
        ox, oy = struct.unpack_from('>2h', blob, 32 + 8 + 8)
        self.assertEqual((ox, oy), (200, 300))  # 16 and 24 image px at zoom 1.6 = 10 and 15 px


if __name__ == '__main__':
    unittest.main()
