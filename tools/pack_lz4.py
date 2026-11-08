#!/usr/bin/env python3
"""Wrap embedded data files in LZ4 ("SM63LZ4W", include/lz4w.h) so they cost
less MEM1; the game unpacks them into MEM2 at boot. Files already wrapped
are left alone.

    python pack_lz4.py assets/generated/player_art.bin assets/generated/font_atlas.bin
"""
import struct, sys
from pathlib import Path
import lz4.block


def wrap(data):
    if data[:8] == b'SM63LZ4W':
        return data
    packed = lz4.block.compress(data, store_size=False)
    return b'SM63LZ4W' + struct.pack('>III', len(data), len(packed), 0) + packed


def unwrap(data):
    if data[:8] != b'SM63LZ4W':
        return data
    raw, packed = struct.unpack_from('>II', data, 8)
    return lz4.block.decompress(data[20:20 + packed], uncompressed_size=raw)


if __name__ == '__main__':
    for name in sys.argv[1:]:
        p = Path(name)
        before = p.read_bytes()
        after = wrap(before)
        p.write_bytes(after)
        print(p.name, len(before), '->', len(after))
