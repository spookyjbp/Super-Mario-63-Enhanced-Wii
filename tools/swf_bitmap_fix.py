#!/usr/bin/env python3
"""Compensate FFDec's half-texel offset in non-smoothed bitmap fills.

The Flash Player draws a non-smoothed bitmap fill (fill types 0x42 repeating,
0x43 clipped) by nearest-texel lookup: the pixel at bitmap coordinate u shows
texel floor(u). FFDec (the renderer the exporters use) shows texel
floor(u - 1/2): every pixel-art fill is drawn half a texel (0.8 px at zoom
1.6) right and down, and at the top/left edge of a fill the first row/column
of pixels wraps round to the opposite edge of a repeating tile. That is the
white line across the sky (CastleColoredFarBG, bitmap 2139: its first row is
transparent, its last row white) and the thin ground-coloured lines along the
top and sides of the bush, waterfall and vine strips.
(tests/test_art_repair.py measures it on the shipped render: with the offset
every opaque texel of band 2139 matches, without it 10 % differ.)

`fix_shape` moves each such fill's matrix by (-1/2, -1/2) texel, so FFDec's
floor(u' - 1/2) equals the Flash Player's floor(u). Smoothed fills (0x40 /
0x41) are bilinear, where texel centres at +1/2 are correct, and are left as
they are.

Only the matrix bytes change; the MATRIX record is byte aligned in both the
shape's FILLSTYLEARRAY and in StyleChangeRecord NewStyles, so a longer or
shorter matrix shifts the rest of the shape by whole bytes.
"""
import struct
from swf_states import Bits

SHAPE_TAGS = {2: 1, 22: 2, 32: 3, 83: 4}  # DefineShape .. DefineShape4 -> version


class Writer:
    def __init__(self):
        self.out, self.acc, self.n = bytearray(), 0, 0

    def ub(self, n, v):
        for i in range(n - 1, -1, -1):
            self.acc = (self.acc << 1) | ((v >> i) & 1)
            self.n += 1
            if self.n == 8:
                self.out.append(self.acc)
                self.acc = self.n = 0

    def flush(self):
        if self.n:
            self.out.append(self.acc << (8 - self.n))
            self.acc = self.n = 0
        return bytes(self.out)


def _bits(*values):
    need = 1
    for v in values:
        while not -(1 << (need - 1)) <= v < (1 << (need - 1)):
            need += 1
    return need


def _read_matrix(b):
    m = {}
    if b.ub(1):
        n = b.ub(5); m['sx'] = b.sb(n); m['sy'] = b.sb(n)
    if b.ub(1):
        n = b.ub(5); m['r0'] = b.sb(n); m['r1'] = b.sb(n)
    n = b.ub(5); m['tx'] = b.sb(n); m['ty'] = b.sb(n)
    b.align()
    return m


def _write_matrix(m):
    w = Writer()
    if 'sx' in m:
        n = _bits(m['sx'], m['sy']); w.ub(1, 1); w.ub(5, n); w.ub(n, m['sx'] & ((1 << n) - 1)); w.ub(n, m['sy'] & ((1 << n) - 1))
    else:
        w.ub(1, 0)
    if 'r0' in m:
        n = _bits(m['r0'], m['r1']); w.ub(1, 1); w.ub(5, n); w.ub(n, m['r0'] & ((1 << n) - 1)); w.ub(n, m['r1'] & ((1 << n) - 1))
    else:
        w.ub(1, 0)
    n = _bits(m['tx'], m['ty']) if (m['tx'] or m['ty']) else 0
    w.ub(5, n)
    if n:
        w.ub(n, m['tx'] & ((1 << n) - 1)); w.ub(n, m['ty'] & ((1 << n) - 1))
    return w.flush()


def half_texel(m):
    """Matrix M * T(-1/2, -1/2): x = a u + c v + tx with a, b, c, d in 16.16."""
    a, d = m.get('sx', 65536), m.get('sy', 65536)
    b, c = m.get('r0', 0), m.get('r1', 0)
    out = dict(m)
    out['tx'] = m['tx'] - round((a + c) / 2 / 65536)
    out['ty'] = m['ty'] - round((b + d) / 2 / 65536)
    return out


class _Shape:
    def __init__(self, data, version):
        self.d, self.v, self.edits, self.fixed = data, version, [], 0

    def fill_styles(self, b):
        """Parse a FILLSTYLEARRAY at a byte boundary, recording matrix edits."""
        count = self.d[b.byte]; b.byte += 1
        if count == 0xFF and self.v >= 2:
            count = struct.unpack_from('<H', self.d, b.byte)[0]; b.byte += 2
        for _ in range(count):
            self.fill_style(b)

    def fill_style(self, b):
        kind = self.d[b.byte]; b.byte += 1
        if kind == 0x00:
            b.byte += 4 if self.v >= 3 else 3
        elif kind in (0x10, 0x12, 0x13):
            _read_matrix(b)
            head = self.d[b.byte]; b.byte += 1
            b.byte += (head & 15) * (1 + (4 if self.v >= 3 else 3))
            if kind == 0x13:
                b.byte += 2
        elif kind in (0x40, 0x41, 0x42, 0x43):
            b.byte += 2
            start = b.byte
            m = _read_matrix(b)
            if kind in (0x42, 0x43):
                self.edits.append((start, b.byte, _write_matrix(half_texel(m))))
                self.fixed += 1
        else:
            raise ValueError(f'Unknown fill style {kind:#x}')

    def line_styles(self, b):
        count = self.d[b.byte]; b.byte += 1
        if count == 0xFF:
            count = struct.unpack_from('<H', self.d, b.byte)[0]; b.byte += 2
        for _ in range(count):
            b.byte += 2  # width
            if self.v < 4:
                b.byte += 4 if self.v >= 3 else 3
                continue
            f1, f2 = self.d[b.byte], self.d[b.byte + 1]; b.byte += 2
            if (f1 >> 4) & 3 == 2:  # miter join
                b.byte += 2
            if f1 & 8:              # HasFillFlag
                self.fill_style(b)
            else:
                b.byte += 4

    def records(self, b):
        nf, nl = b.ub(4), b.ub(4)
        while True:
            if b.ub(1):  # edge
                straight, n = b.ub(1), b.ub(4) + 2
                if straight:
                    if b.ub(1):
                        b.sb(n); b.sb(n)
                    else:
                        b.ub(1); b.sb(n)
                else:
                    for _ in range(4):
                        b.sb(n)
                continue
            flags = b.ub(5)
            if not flags:
                return
            if flags & 1:
                n = b.ub(5); b.sb(n); b.sb(n)
            if flags & 2: b.ub(nf)
            if flags & 4: b.ub(nf)
            if flags & 8: b.ub(nl)
            if flags & 16:
                b.align()
                self.fill_styles(b)
                self.line_styles(b)
                nf, nl = b.ub(4), b.ub(4)


def _rect(b):
    n = b.ub(5)
    for _ in range(4):
        b.sb(n)
    b.align()


def fix_shape(code, data):
    """Returns (new tag data, number of fills moved)."""
    s = _Shape(data, SHAPE_TAGS[code])
    b = Bits(data, 2)
    _rect(b)
    if s.v == 4:
        _rect(b)
        b.byte += 1
    s.fill_styles(b)
    s.line_styles(b)
    s.records(b)
    if not s.edits:
        return data, 0
    out, last = bytearray(), 0
    for start, end, new in s.edits:
        out += data[last:start] + new
        last = end
    out += data[last:]
    return bytes(out), s.fixed


def fix_swf_tag(code, data):
    if code in SHAPE_TAGS:
        return fix_shape(code, data)[0]
    return data


def _tag(code, data):
    return struct.pack('<H', (code << 6) | min(len(data), 63)) + (struct.pack('<I', len(data)) if len(data) >= 63 else b'') + data


def fix_body(body, start):
    """Uncompressed SWF body (header through tags) with every shape fixed."""
    from swf_states import tags
    out = bytearray(body[:start])
    for code, pos, length in tags(body, start, len(body)):
        out += _tag(code, fix_swf_tag(code, body[pos:pos + length]))
    return bytes(out)


def fix_swf(swf):
    """A whole uncompressed/compressed SWF -> uncompressed SWF with fixed shapes."""
    from swf_states import body as split
    raw, start = split(swf)
    out = fix_body(raw, start)
    return b'FWS' + swf[3:4] + struct.pack('<I', len(out) + 8) + out


# FFDec option used with these SWFs: supersampled coverage, so an edge pixel's
# colour comes from texels inside the shape (plain antialiasing samples the
# pixel centre, which can lie outside the fill and wrap round the tile).
FFDEC_RENDER = ['-config', 'useMsaaForExport=1']
