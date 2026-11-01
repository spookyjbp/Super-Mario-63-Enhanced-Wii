"""Validate checked-in GX pixels with an independent tile decoder."""
import importlib.util
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parent.parent
spec = importlib.util.spec_from_file_location('atlas_builder', ROOT / 'tools/build_mario_atlas.py')
builder = importlib.util.module_from_spec(spec)
spec.loader.exec_module(builder)


class AtlasTests(unittest.TestCase):
    def test_gx_tiles_preserve_rgba(self):
        image, data, header = builder.build()
        decoded = bytearray()
        for y in range(image.height):
            for x in range(image.width):
                tile = ((y // 4) * (image.width // 4) + x // 4) * 64
                pixel = ((y % 4) * 4 + x % 4) * 2
                offset = tile + pixel
                decoded.extend((data[offset + 1], data[offset + 32],
                                data[offset + 33], data[offset]))
        self.assertEqual(bytes(decoded), image.tobytes())
        self.assertEqual(len(data), 256 * 256 * 4)
        self.assertIn('mario_frames', header)
        self.assertEqual(data, (ROOT / 'assets/generated/mario_rgba8.bin').read_bytes())


if __name__ == '__main__':
    unittest.main()
