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

    def test_toad_bodies_have_no_head(self):
        """The head (2498) is drawn on its own with the mood's face; it must not be
        baked into a body frame. Walk frames 16-20 move it to another depth, which
        left a smiling head in them (the angry Toad's face flicker)."""
        table, zoom = ids()
        for name in ('TOAD_WALK', 'TOAD_IDLE'):
            clip, pack = table[name]
            heights = [f[3] / zoom for f in frames(pack, clip)]
            self.assertLess(max(heights), 16, name)   # body only (with a head: ~30 course px)

    def test_mario_extra_clips(self):
        """Clips beyond the player atlas: Enemy Jump (21 frames), Drown (to its
        frame 88 LifeLost: 88), per FLUDD state; Idle (978), Tired (37), Sleep
        (31); the water pie (9), WaterBubble (39 shown), Med-Splash (9 shown).
        Mario stands on his registration point (feet at y 0 within a few px)."""
        table, zoom = ids()
        want = {'MARIO_IDLE': 978, 'MARIO_TIRED': 37, 'MARIO_SLEEP': 31, 'HUD_WATER_PIE': 9, 'WATER_BUBBLE': 39,
                'MED_SPLASH': 9}
        for k in range(4):
            want[f'MARIO_EJUMP_{k}'] = 21
            want[f'MARIO_DROWN_{k}'] = 88
        for name, count in want.items():
            clip, pack = table[name]
            self.assertEqual(len(frames(pack, clip)), count, name)
        for name in ('MARIO_IDLE', 'MARIO_TIRED'):
            clip, pack = table[name]
            x, y, w, h, ox, oy, duration, _ = frames(pack, clip)[0]
            self.assertAlmostEqual(oy / 20 + h / zoom, 0, delta=4, msg=name)   # feet on the floor
            self.assertAlmostEqual(ox / 20 + w / zoom / 2, 0, delta=6, msg=name)

    def test_idle_has_no_wings(self):
        """Idle Animation places Wing Code (1003), whose frame 1 is the Wing Cap's
        wings (frame 2, none, without the cap): they must not be in the frames.
        With them the tops reach 41.6 px above the feet; Mario alone ~35."""
        table, zoom = ids()
        clip, pack = table['MARIO_IDLE']
        self.assertGreater(min(oy / 20 for *_, ox, oy, d, f in frames(pack, clip)), -36)

    def test_prompts_per_controller(self):
        """The read prompt over signs (981) and Toad's 'bz' (2505) show the
        controller's button in place of the keyboard Z: one clip per controller
        (GameCube, Classic, Wii Remote) with ids in that order, the original's
        frame count, and the legs / tail where the original has them."""
        table, zoom = ids()
        for name in ('SIGN_Z', 'TOAD_BZ'):
            base = frames(*reversed(table[name]))
            ids_k = [table[f'{name}_{k}'][0] for k in range(3)]
            self.assertEqual(ids_k, [ids_k[0], ids_k[0] + 1, ids_k[0] + 2], name)
            for k in range(3):
                clip, pack = table[f'{name}_{k}']
                fr = frames(pack, clip)
                self.assertEqual(len(fr), len(base), f'{name}_{k}')
                for a, b in zip(base, fr):
                    self.assertAlmostEqual(a[5] / 20 + a[3] / zoom, b[5] / 20 + b[3] / zoom, delta=2.5,
                                           msg=f'{name}_{k}: bottom moved')

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
