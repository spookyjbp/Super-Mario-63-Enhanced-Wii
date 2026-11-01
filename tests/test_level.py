import copy
import importlib.util
import json
from pathlib import Path
import unittest
ROOT=Path(__file__).resolve().parent.parent
spec=importlib.util.spec_from_file_location('level_builder',ROOT/'tools/build_level.py')
builder=importlib.util.module_from_spec(spec);spec.loader.exec_module(builder)

class LevelTests(unittest.TestCase):
    def setUp(self): self.data=json.loads((ROOT/'levels/movement.json').read_text())
    def test_generated_data(self):
        self.assertEqual(builder.compile_level(self.data),(ROOT/'assets/generated/movement_level.h').read_text())
    def test_reject_invalid_tiles(self):
        for row in ['bad','?'*40]:
            data=copy.deepcopy(self.data);data['tiles'][0]=row
            with self.assertRaises(ValueError):builder.compile_level(data)
    def test_reject_invalid_spawn_or_schema(self):
        for key,value in [('spawn',[-1,20]),('spawn',[96,950]),('schema_version',2)]:
            data=copy.deepcopy(self.data);data[key]=value
            with self.assertRaises(ValueError):builder.compile_level(data)
if __name__=='__main__':unittest.main()
