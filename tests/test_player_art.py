import importlib.util
from pathlib import Path
import sys,tempfile,unittest
from PIL import Image
ROOT=Path(__file__).resolve().parent.parent
sys.path.insert(0,str(ROOT/'tools'))
from build_player_art import build
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
if __name__=='__main__':unittest.main()
