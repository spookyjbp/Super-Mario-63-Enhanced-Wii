import sys, struct
sys.path.insert(0,'/home/claude/super-mario-63-enhanced-wii/tools')
import swf_states as S, swf_bitmap_fix as F
from swf_states import Bits

class Alphas(F._Shape):
    """Records the alpha byte of every solid fill and gradient stop: (kind, offset)."""
    def __init__(self, data, version):
        super().__init__(data, version); self.alphas = []
    def fill_style(self, b):
        kind = self.d[b.byte]
        if kind == 0x00:
            b.byte += 1; self.alphas.append(('solid', b.byte + 3)); b.byte += 4; return
        if kind in (0x10, 0x12, 0x13):
            b.byte += 1; F._read_matrix(b)
            head = self.d[b.byte]; b.byte += 1
            for _ in range(head & 15):
                self.alphas.append(('gradient', b.byte + 4)); b.byte += 5
            if kind == 0x13: b.byte += 2
            return
        super().fill_style(b)

def split(swf, shape_id, keep):
    """SWF with shape_id's fills of the other kind made transparent (keep: 'solid' or 'gradient')."""
    raw, start = S.body(swf)
    out = bytearray(raw[:start])
    for code, pos, length in S.tags(raw, start, len(raw)):
        data = raw[pos:pos + length]
        if code in F.SHAPE_TAGS and struct.unpack_from('<H', data)[0] == shape_id:
            s = Alphas(data, F.SHAPE_TAGS[code]); b = Bits(data, 2); F._rect(b)
            if s.v == 4: F._rect(b); b.byte += 1
            s.fill_styles(b); s.line_styles(b); s.records(b)
            d = bytearray(data)
            for kind, off in s.alphas:
                if kind != keep: d[off] = 0
            print(shape_id, [k for k, _ in s.alphas])
            data = bytes(d)
        out += F._tag(code, data)
    return b'FWS' + swf[3:4] + struct.pack('<I', len(out) + 8) + bytes(out)

if __name__ == '__main__':
    swf = open('/home/claude/super-mario-63-enhanced-wii/private-art/rendered-course-art/pictures.swf','rb').read()
    for keep in ('solid', 'gradient'):
        open(f'/tmp/claude-0/p250/work/{keep}.swf','wb').write(split(swf, 244, keep))
