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
        want = {'HUD_WATER_PIE': 9, 'WATER_BUBBLE': 39, 'MED_SPLASH': 9}
        for k in range(4):
            want[f'MARIO_EJUMP_{k}'] = 21
            want[f'MARIO_DROWN_{k}'] = 88
            want[f'MARIO_IDLE_{k}'] = 978
            want[f'MARIO_TIRED_{k}'] = 37
            want[f'MARIO_SLEEP_{k}'] = 31
        for name, count in want.items():
            clip, pack = table[name]
            self.assertEqual(len(frames(pack, clip)), count, name)
        for name in ('MARIO_IDLE', 'MARIO_TIRED'):
            for k in range(4):
                clip, pack = table[f'{name}_{k}']
                x, y, w, h, ox, oy, duration, _ = frames(pack, clip)[0]
                self.assertAlmostEqual(oy / 20 + h / zoom, 0, delta=4, msg=f'{name}_{k}')   # feet on the floor
                if k == 0:
                    self.assertAlmostEqual(ox / 20 + w / zoom / 2, 0, delta=6, msg=name)

    def test_fludd_states_share_a_place(self):
        """Every FLUDD state of a clip is placed from its own registration: the
        canvas FFDec gives a sprite changed with the FLUDD state, so states drawn
        from the no-state origin sat up to 8 px off (and the Hover nozzle was cut
        off). Mario's feet sit at the same height in all four."""
        table, zoom = ids()
        for name, frame in (('MARIO_EJUMP', 0), ('MARIO_DROWN', 0), ('MARIO_IDLE', 0), ('MARIO_TIRED', 5)):
            feet = []
            for k in range(4):
                clip, pack = table[f'{name}_{k}']
                x, y, w, h, ox, oy, duration, _ = frames(pack, clip)[frame]
                feet.append(oy / 20 + h / zoom)
            self.assertLess(max(feet) - min(feet), 1.5, f'{name}: {feet}')

    def test_idle_with_fludd(self):
        """The port's idle with FLUDD (tools/idle_fludd.py): the FLUDD states
        carry FLUDD on Mario's back (wider and taller frames); without FLUDD it
        is the original's."""
        table, zoom = ids()
        for name in ('MARIO_IDLE', 'MARIO_TIRED', 'MARIO_SLEEP'):
            plain = frames(*reversed(table[f'{name}_0']))
            for k in (1, 2, 3):
                withf = frames(*reversed(table[f'{name}_{k}']))
                bigger = sum(b[2] * b[3] > a[2] * a[3] * 1.2 for a, b in zip(plain, withf))
                self.assertGreater(bigger, len(plain) * 0.9, f'{name}_{k}')

    def test_packs_fit_their_memory(self):
        """course_render.c decodes every pack into OBJECT_BYTES of MEM2."""
        src = (ROOT / 'source/platform/course_render.c').read_text()
        a, b, c = map(int, re.search(r'#define OBJECT_BYTES \((\d+)u \* (\d+) \* (\d+)\)', src).groups())
        packs = int(re.search(r'#define OBJECT_ART_PACKS (\d+)', (GEN / 'object_art_ids.h').read_text()).group(1))
        used = 0
        for p in range(packs):
            data = (GEN / f'objects_8e1_{p}.bin').read_bytes()
            w, h = struct.unpack_from('>II', data, 12)
            used += (w * h * 2 + 31) // 32 * 32
        self.assertLessEqual(used, a * b * c)
        self.assertIn(f'objects_8e1_{packs - 1}_bin,', src)

    def test_idle_has_no_wings(self):
        """Idle Animation places Wing Code (1003), whose frame 1 is the Wing Cap's
        wings (frame 2, none, without the cap): they must not be in the frames.
        With them the tops reach 41.6 px above the feet; Mario alone ~35."""
        table, zoom = ids()
        clip, pack = table['MARIO_IDLE_0']
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
