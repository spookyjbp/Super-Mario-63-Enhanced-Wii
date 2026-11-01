#!/usr/bin/env python3
"""Build a player artwork pack (format version 2) from exported frames.

Frames are trimmed to their opaque bounds, identical images are stored once, and
everything is shelf-packed into one GX RGBA8 texture of at most 1024x1024 (the
Wii's texture limit). Offsets are twips from the clip registration point.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path
from PIL import Image, ImageOps
from build_mario_atlas import rgba8_tiles
try:
    import lz4.block as lz4block
except ImportError:  # only needed for compressed (v3) packs
    lz4block = None

VERSION = 2
MAX_CLIPS = 192
MAX_FRAMES = 4096
OFFSET_LIMIT = 8192
SIZES = [(256, 256), (512, 256), (512, 512), (1024, 512), (1024, 1024)]


def _pack(sizes, width, height):
    """Shelf packing, tallest first. Returns {index: (x, y)} or None."""
    order = sorted(range(len(sizes)), key=lambda i: (-sizes[i][1], -sizes[i][0]))
    x = y = row = 0
    spots = {}
    for i in order:
        w, h = sizes[i]
        if x + w + 2 > width:
            x, y, row = 0, y + row + 2, 0
        if w + 2 > width or y + h + 2 > height:
            return None
        spots[i] = (x + 1, y + 1)
        x += w + 2
        row = max(row, h)
    return spots


def rgb5a3_tiles(image):
    """GX RGB5A3 4x4 tiles, big-endian."""
    import numpy as np  # only needed for compressed (v3) packs
    a = np.asarray(image.convert('RGBA'), dtype=np.uint16)
    h, w = a.shape[:2]
    r, g, b, al = a[..., 0], a[..., 1], a[..., 2], a[..., 3]
    opaque = (0x8000 | ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3)).astype(np.uint16)
    trans = (((al >> 5) << 12) | ((r >> 4) << 8) | ((g >> 4) << 4) | (b >> 4)).astype(np.uint16)
    px = np.where(al >= 0xE0, opaque, trans).astype('>u2')
    return px.reshape(h // 4, 4, w // 4, 4).transpose(0, 2, 1, 3).tobytes()


def build(data, root, compressed=False, px_twips=20):
    """Version 2: raw GX RGBA8 texture. Version 3 (compressed=True): GX RGB5A3
    texture, LZ4 block compressed, preceded by its u32 decoded size.
    px_twips: twips per image pixel, for the offset added when frames are
    trimmed (20 for art rendered at 1 px per px; 20 / zoom otherwise)."""
    images, index, clips, frames, seen = [], {}, [], [], set()
    for clip in data['clips']:
        ident = clip['id']
        if type(ident) is not int or not 0 <= ident < MAX_CLIPS or ident in seen:
            raise ValueError('Invalid/duplicate clip ID')
        seen.add(ident)
        first = len(frames)
        for frame in clip['frames']:
            im = Image.open(root / frame['file']).convert('RGBA')
            if frame.get('mirror', False):
                im = ImageOps.mirror(im)
            ox, oy = frame['offset_twips']
            duration = frame.get('duration', 1)
            if any(type(v) is not int for v in (ox, oy, duration)) or not 1 <= duration <= 65535:
                raise ValueError('Invalid offset/duration')
            box = im.getbbox() or (0, 0, 1, 1)  # fully transparent: one clear pixel
            im = im.crop(box)
            ox, oy = ox + round(box[0] * px_twips), oy + round(box[1] * px_twips)
            if not -OFFSET_LIMIT <= ox <= OFFSET_LIMIT or not -OFFSET_LIMIT <= oy <= OFFSET_LIMIT:
                raise ValueError('Offset out of range')
            if not 1 <= im.width <= 1024 or not 1 <= im.height <= 1024:
                raise ValueError('Frame too large')
            key = hashlib.sha256(im.tobytes() + struct.pack('>2H', *im.size)).digest()
            if key not in index:
                index[key] = len(images)
                images.append(im)
            frames.append((index[key], ox, oy, duration))
        count = len(frames) - first
        if not count:
            raise ValueError('Empty clip')
        clips.append((ident, first, count, int(clip.get('loop', False)) | 2 * int(clip.get('equipped', False))))
    if not 1 <= len(frames) <= MAX_FRAMES:
        raise ValueError('Invalid frame count')
    sizes = [im.size for im in images]
    for width, height in SIZES:
        spots = _pack(sizes, width, height)
        if spots is not None:
            break
    else:
        raise ValueError('Artwork does not fit one 1024x1024 Wii texture')
    atlas = Image.new('RGBA', (width, height))
    for i, im in enumerate(images):
        atlas.paste(im, spots[i])
    if compressed:
        # Bilinear-filtered (objects): transparent texels take their neighbours'
        # colour so edges and the 1 px gutters do not darken (art_repair.bleed).
        import numpy as np
        from art_repair import bleed
        raw = rgb5a3_tiles(Image.fromarray(bleed(np.asarray(atlas))))
        packed = lz4block.compress(raw, store_size=False)
        texture = struct.pack('>I', len(raw)) + packed
        version = 3
    else:
        texture = rgba8_tiles(atlas)
        version = VERSION
    output = bytearray(struct.pack('>8s6I', b'SM63ART1', version, width, height, len(clips), len(frames),
                                   len(texture)))
    for c in clips:
        output.extend(struct.pack('>4H', *c))
    for image, ox, oy, duration in frames:
        x, y = spots[image]
        w, h = sizes[image]
        output.extend(struct.pack('>4H2h2H', x, y, w, h, ox, oy, duration, 0))
    output.extend(bytes((-len(output)) % 32))
    output.extend(texture)
    output.extend(bytes((-len(output)) % 32))
    return bytes(output)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('manifest', type=Path)
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    data = json.loads(args.manifest.read_text())
    if data.get('canonical_facing') != 'left':
        raise ValueError('Export frames facing LEFT, as in the original XFL clips')
    result = build(data, args.manifest.parent)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_bytes(result)
    print('Wrote player artwork pack')
