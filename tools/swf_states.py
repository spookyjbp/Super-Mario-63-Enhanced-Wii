#!/usr/bin/env python3
"""Read the pinned original SWF and report, for every frame label of the
`Mario Main` character sprite, which child sprites are on stage (depth order,
character id, placement matrix). This replaces hand-found character ids: the
labels ("H1", "Rbackflip", ...) are preserved in the compiled SWF.
"""
import struct, zlib, json, sys
from pathlib import Path


class Bits:
    def __init__(self, data, pos=0):
        self.d, self.byte, self.bit = data, pos, 0

    def ub(self, n):
        v = 0
        for _ in range(n):
            v = (v << 1) | ((self.d[self.byte] >> (7 - self.bit)) & 1)
            self.bit += 1
            if self.bit == 8:
                self.bit, self.byte = 0, self.byte + 1
        return v

    def sb(self, n):
        v = self.ub(n)
        return v - (1 << n) if n and v & (1 << (n - 1)) else v

    def align(self):
        if self.bit:
            self.bit, self.byte = 0, self.byte + 1


def matrix(b):
    m = {'sx': 1.0, 'sy': 1.0, 'r0': 0.0, 'r1': 0.0}
    if b.ub(1):
        n = b.ub(5); m['sx'] = b.sb(n) / 65536; m['sy'] = b.sb(n) / 65536
    if b.ub(1):
        n = b.ub(5); m['r0'] = b.sb(n) / 65536; m['r1'] = b.sb(n) / 65536
    n = b.ub(5); m['tx'] = b.sb(n); m['ty'] = b.sb(n)  # twips
    b.align()
    return m


def cxform(b):
    """Skip a CXFORMWITHALPHA."""
    has_add, has_mult, n = b.ub(1), b.ub(1), b.ub(4)
    if has_mult:
        for _ in range(4): b.sb(n)
    if has_add:
        for _ in range(4): b.sb(n)
    b.align()


def tags(data, pos, end):
    while pos + 2 <= end:
        header = struct.unpack_from('<H', data, pos)[0]; pos += 2
        code, length = header >> 6, header & 63
        if length == 63:
            length = struct.unpack_from('<I', data, pos)[0]; pos += 4
        yield code, pos, length
        pos += length
        if code == 0:
            return


def body(swf):
    raw = zlib.decompress(swf[8:]) if swf[:3] == b'CWS' else swf[8:]
    start = (5 + 4 * (raw[0] >> 3) + 7) // 8 + 4
    return raw, start


def sprites(swf):
    raw, start = body(swf)
    out = {}
    for code, pos, length in tags(raw, start, len(raw)):
        if code != 39:
            continue
        ident, count = struct.unpack_from('<HH', raw, pos)
        frames, labels, display, current_label = [], {}, {}, None
        for c, p, n in tags(raw, pos + 4, pos + length):
            if c in (26, 70):
                flags = raw[p]; q = p + (2 if c == 70 else 1)
                depth = struct.unpack_from('<H', raw, q)[0]; q += 2
                if c == 70 and flags & 0x08 or c == 70 and raw[p + 1] & 0x10:
                    pass
                if c == 70 and raw[p + 1] & 0x08:  # class name
                    q = raw.index(0, q) + 1
                entry = dict(display.get(depth, {}))
                if flags & 0x02:
                    entry = {'id': struct.unpack_from('<H', raw, q)[0]}; q += 2
                if flags & 0x04:
                    b = Bits(raw, q); entry['matrix'] = matrix(b); q = b.byte
                if flags & 0x08:
                    b = Bits(raw, q); cxform(b); q = b.byte
                if flags & 0x10:
                    q += 2  # ratio
                if flags & 0x20:
                    end = raw.index(0, q); entry['name'] = raw[q:end].decode('latin-1'); q = end + 1
                display[depth] = entry
            elif c == 28:  # RemoveObject2
                display.pop(struct.unpack_from('<H', raw, p)[0], None)
            elif c == 43:
                current_label = raw[p:raw.index(0, p)].decode('latin-1')
            elif c == 1:
                if current_label is not None:
                    labels[current_label] = len(frames)
                frames.append({d: dict(e) for d, e in sorted(display.items())})
                current_label = None
        out[ident] = {'frames': count, 'labels': labels, 'stage': frames}
    return out


def mario_main(all_sprites):
    need = {'Dive', 'diveflip', 'H1', 'Rbackflip', 'Tswim2'}
    hits = [i for i, s in all_sprites.items() if need <= set(s['labels'])]
    # Luigi Main shares the labels. Mario's places DefineSprite 1088, the bare
    # Mario backflip (22 frames), which Luigi Main does not place.
    hits = [i for i in hits if any(e.get('id') == 1088 for e in
            all_sprites[i]['stage'][all_sprites[i]['labels']['backflip']].values())]
    if len(hits) != 1:
        raise ValueError(f'Expected one Mario Main sprite, found {hits}')
    return hits[0]


if __name__ == '__main__':
    data = Path(sys.argv[1]).read_bytes()
    all_sprites = sprites(data)
    main = mario_main(all_sprites)
    s = all_sprites[main]
    report = {'mario_main': main, 'labels': {}}
    for label, index in sorted(s['labels'].items(), key=lambda x: x[1]):
        stage = s['stage'][index]
        report['labels'][label] = {
            'frame': index,
            'children': [{'depth': d, 'id': e.get('id'), 'frames': all_sprites.get(e.get('id'), {}).get('frames'),
                          'matrix': e.get('matrix')} for d, e in stage.items()]}
    print(json.dumps(report, indent=1))
