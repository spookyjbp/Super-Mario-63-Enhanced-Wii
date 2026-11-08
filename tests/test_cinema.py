"""The cutscenes' tools: the ActionScript interpreter (tools/as2.py) keeps the
SWF 8 rules the recordings depend on, and the data files
(tools/export_cinema.py) are whole and fit the MEM2 area the port gives them."""
import struct, sys, unittest
from pathlib import Path
ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tools'))
import as2
import export_cinema as E


def black_edges(pack, pages=40):
    """The share of clear texels next to visible ones that are black where
    those neighbours are not (outlines are black: clear texels beside them
    rightly are), in the pack's first pages."""
    import lz4.block, numpy as np
    n = min(pages, struct.unpack_from('>I', pack, 8)[0])
    q, edge, black = 40, 0, 0
    for _ in range(n):
        w, h, size = struct.unpack_from('>HHI', pack, q); q += 16
        t = np.frombuffer(lz4.block.decompress(pack[q:q + size], uncompressed_size=w * h * 2), '>u2')
        q += size; q += -q % 4
        t = t.reshape(h // 4, w // 4, 4, 4).transpose(0, 2, 1, 3).reshape(h, w)
        op = (t & 0x8000) > 0
        clear = ~op & (((t >> 12) & 7) == 0)
        lit = ~clear & (np.where(op, t & 0x7fff, t & 0xfff) != 0)    # visible and not black
        near = np.zeros_like(clear)
        near[1:] |= lit[:-1]; near[:-1] |= lit[1:]; near[:, 1:] |= lit[:, :-1]; near[:, :-1] |= lit[:, 1:]
        e = clear & near
        edge += e.sum(); black += (e & ((t & 0xfff) == 0)).sum()
    return black / max(1, edge)


class Clip(as2.Obj):
    pass


def run(src, **props):
    this = Clip(props)
    root = as2.Obj()
    it = as2.Interp({'_root': root, 'Math': as2.Obj({'abs': as2.Native(lambda t, v: abs(as2.num(v)))})})
    it.run(as2.parse(src), this)
    return this, root, it


class Interpreter(unittest.TestCase):
    def test_numbers_and_strings(self):
        c, _, _ = run('a = 1 + 2 * 3; b = "x" + 1; c = 7 % 3; d = 1 << 4; e = (3 > 2) ? "y" : "n";')
        self.assertEqual((c.get('a'), c.get('b'), c.get('c'), c.get('d'), c.get('e')), (7.0, 'x1', 1.0, 16.0, 'y'))

    def test_undefined_is_nan(self):
        # SWF 8: the water clips' test of _root.Course.Char._y fails, as in the original
        c, _, _ = run('n = _root.Course.Char._y - 5; t = Math.abs(n) < 1000; u = "" + _root.nothing;')
        self.assertTrue(c.get('n') != c.get('n'))
        self.assertIs(c.get('t'), False)
        self.assertEqual(c.get('u'), 'undefined')

    def test_functions_and_scope(self):
        c, _, _ = run('''
            var k = 2;
            function f(x) { var y = x * k; return y + 1; }
            r = f(3);
            g = function () { k = 10; };
            g();
            s = 0;
            for (i = 0; i < 4; i++) { if (i == 2) { continue; } s += i; }
        ''')
        self.assertEqual(c.get('r'), 7.0)
        self.assertEqual(c.get('k'), 10.0)
        self.assertEqual(c.get('s'), 4.0)

    def test_tell_target(self):
        child = Clip({'v': 1.0})
        c, _, _ = run('tellTarget(kid) { v = v + 5; w = 2; }', kid=child)
        self.assertEqual((child.get('v'), child.get('w')), (6.0, 2.0))
        self.assertIs(c.get('w'), as2.UNDEF)

    def test_objects_and_null_call(self):
        c, _, _ = run('o = {ra: 50, rb: 10}; x = o.ra + o.rb; onEnterFrame = null();')
        self.assertEqual(c.get('x'), 60.0)
        self.assertIsNone(c.get('onEnterFrame'))


class ScriptPlayer(unittest.TestCase):
    """tools/cinema_sim.py on a two-frame timeline (no SWF needed)."""
    def make(self):
        import cinema_sim as C, timeline_art as T
        place = lambda x: {'id': 11, 'born': 0, 'matrix': (1, 0, 0, 1, x, 0), 'cx': T.NO_CX, 'filters': [], 'clip': 0,
                           'name': 'cam'}
        tl = {10: {'kind': 'sprite', 'frames': [{1: place(0)}, {1: place(50)}], 'labels': {}, 'stops': set()},
              11: {'kind': 'sprite', 'frames': [{}], 'labels': {}, 'stops': set()}}

        class NoScripts:
            def frame(self, sprite, f): return []
            def clip_events(self, sprite, f, depth): return {}
        sim = C.Sim(tl, NoScripts(), None)
        top = C.Clip(sim, None, None, 0, None, 'root')
        movie = sim.new_clip(10, top, 1, {'id': 10, 'born': 0, 'matrix': T.IDENT, 'cx': T.NO_CX, 'filters': [], 'clip': 0})
        top.children[1] = movie
        return sim, top, movie.children[1]

    def test_reading_does_not_take_a_clip_over(self):
        # the camera script (9862) reads its own _xscale every frame: the
        # timeline must still move it (it froze every cutscene camera)
        sim, top, cam = self.make()
        cam.get('_xscale'); cam.get('_rotation'); cam.get('_x')
        sim.step(top)
        self.assertEqual(cam.get('_x'), 50)

    def test_setting_does(self):
        sim, top, cam = self.make()
        cam.set('_y', 7)
        sim.step(top)
        self.assertEqual((cam.get('_x'), cam.get('_y')), (0, 7))


class Exporter(unittest.TestCase):
    def test_repeat_set_after_start(self):
        # 9979 frame 528: start(0, 1), setVolume(100), then onSoundComplete
        # (the Airship intro went silent after 8 s: the repeat was read at the
        # end of the recording, when the handler had cleared it)
        fn = as2.parse('f = function () { _root.bgsong.attachSound("Airship Theme Repeat"); };')
        it = as2.Interp({'_root': as2.Obj()})
        this = Clip({})
        it.run(fn, this)
        events = [(5390, 'sound_volume', 3, 0.0), (5391, 'sound_stop', 3), (5391, 'sound_start', 4, 'Airship Theme Intro', 0.0, 1.0),
                  (5391, 'sound_volume', 4, 100.0), (5391, 'sound_complete', 4, this.get('f')),
                  (5500, 'sound_volume', 4, 0.0), (5600, 'sound_complete', 4, None)]
        self.assertEqual(E.sound_state(events, 2, 4), ('Airship Theme Repeat', 100.0))

    def test_corrections(self):
        used = set()
        self.assertEqual(E.correct("<b><i>Bowser:</i></b> Your Castle mine, Princess!", used),
                         "<b><i>Bowser:</i></b> Your Castle is mine, Princess!")
        self.assertEqual(len(used), 1)


class DataFiles(unittest.TestCase):
    def check(self, name, frames, course, songs=True):
        data = (ROOT / 'assets/data' / name).read_bytes()
        self.assertEqual(data[:8], b'SM63CIN1')
        version, nframes, nevents, nsounds, ev, snd, pack, size = struct.unpack_from('>8I', data, 8)
        self.assertEqual((version, nframes), (1, frames))
        self.assertEqual(pack + size, len(data))
        p = data[pack:pack + size]
        self.assertEqual(p[:8], b'SM63TLP3')
        cache, chunk = E.cache_bytes(p)
        self.assertLess(cache + chunk + 3 * 8192, E.SCRATCH)
        # the last event is the end, in its course
        q, last = ev, None
        for _ in range(nevents):
            f, kind, n = struct.unpack_from('>IBB', data, q)
            last = (kind, data[q + 8:q + 8 + n])
            q += 8 + (n + 3) // 4 * 4
        self.assertEqual(last[0], E.END)
        self.assertEqual(last[1][8:].decode(), course)
        # every song that plays once and then repeats or loops says so (the
        # music stopped after the intro of Bowser's invasion)
        q = ev
        for _ in range(nevents):
            f, kind, n = struct.unpack_from('>IBB', data, q)
            if kind == E.MUSIC and songs:
                snd, rep, loops = struct.unpack_from('>BBB', data, q + 8)
                self.assertTrue(rep != 255 and loops, f'music at {f} does not go on')
            q += 8 + (n + 3) // 4 * 4
        # clear texels beside visible ones carry a colour (black ones drew
        # lines along every piece's edge)
        self.assertLess(black_edges(p), 0.05)   # 0.98 before

    def test_intro(self):
        self.check('cinema_intro.bin', 1050, '8-E1-1', songs=False)   # its timeline's stream sound

    def test_intro2(self):
        self.check('cinema_intro2.bin', 21498, 'Castle')


if __name__ == '__main__':
    unittest.main()
