import importlib.util
from pathlib import Path
import sys,tempfile,unittest
from PIL import Image
ROOT=Path(__file__).resolve().parent.parent
sys.path.insert(0,str(ROOT/'tools'))
from build_player_art import build
from export_source_clips import without_wing_cap
import struct
class PlayerArtTests(unittest.TestCase):
    def test_export_and_indices(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp);Image.new('RGBA',(4,4),(10,20,30,40)).save(root/'frame.png')
            data={'clips':[{'id':0,'frames':[{'file':'frame.png','offset_twips':[-20,0],'duration':2}]}]}
            raw=build(data,root);self.assertEqual(raw[:8],b'SM63ART1')
            self.assertEqual(int.from_bytes(raw[24:28],'big'),1)
            self.assertEqual(len(raw)%32,0)
            data['clips'][0]['id']=64
            with self.assertRaises(ValueError):build(data,root)
    def test_reject_empty_clips(self):
        with self.assertRaises(ValueError):build({'clips':[{'id':0,'frames':[]}]},ROOT)
    def test_static_export_disables_only_wing_cap(self):
        rect_header=b'\x08\x00\x00\x20\x01\x00'
        sprite=struct.pack('<HH',1003,2)+b'WingCap'
        other=struct.pack('<HH',1004,1)+b'Unchanged'
        other_tag=struct.pack('<H',(39<<6)|len(other))+other
        body=rect_header+struct.pack('<H',(39<<6)|len(sprite))+sprite+other_tag+b'\x00\x00'
        original=b'FWS'+bytes([10])+struct.pack('<I',8+len(body))+body
        result=without_wing_cap(original)
        self.assertEqual(struct.unpack_from('<I',result,4)[0],len(result))
        self.assertIn(struct.pack('<HHHHH',1003,2,64,64,0),result)
        self.assertIn(other_tag,result)
        with self.assertRaises(ValueError):without_wing_cap(original.replace(b'WingCap',b'NotWing'))
if __name__=='__main__':unittest.main()
