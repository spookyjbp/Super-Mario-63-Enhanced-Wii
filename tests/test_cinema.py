"""The cutscenes' tools: the ActionScript interpreter (tools/as2.py) keeps the
SWF 8 rules the recordings depend on, and the data files
(tools/export_cinema.py) are whole and fit the MEM2 area the port gives them."""
import struct, sys, unittest
from pathlib import Path
ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tools'))
import as2
import export_cinema as E


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


class DataFiles(unittest.TestCase):
    def check(self, name, frames, course):
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

    def test_intro(self):
        self.check('cinema_intro.bin', 1050, '8-E1-1')

    def test_intro2(self):
        self.check('cinema_intro2.bin', 7800, 'Castle')


if __name__ == '__main__':
    unittest.main()
