#!/usr/bin/env python3
"""Extract unambiguous poses from a user-supplied Runouw checkout, without downloads.
Does not guess Flash motion tweens, composite FLUDD clips, or effect scripting.
"""
import argparse,json
from pathlib import Path
from build_player_art import build
def source_manifest(source):
    root=source/'Super_Mario_63_2010/LIBRARY/Char/BITS Mario'
    # Canonical left-facing artwork and offsets from the original bitmap instances.
    # IDs match art_pack.h. Full tween/equipment exports may be added separately.
    clips=[
     {'id':0,'frames':[{'file':'Mario Slide (None).png','mirror':True,'offset_twips':[-261,-101],'duration':1}]},
     {'id':1,'frames':[{'file':'Mario Air (none) - 2.png','offset_twips':[-190,-320],'duration':1}]},
     {'id':2,'frames':[{'file':'Mario Air (none) - 1.png','offset_twips':[-223,-283],'duration':1}]},
     {'id':3,'frames':[{'file':'Mario Air (none) - 2.png','offset_twips':[-190,-320],'duration':1}]},
     {'id':6,'frames':[{'file':f'Mario (none) - Swimming - {i}.png','offset_twips':[-180,-290 if i==3 else -300],'duration':1} for i in (3,4)]},
     {'id':7,'frames':[{'file':f'Mario (none) - Swimming - {i}.png','offset_twips':[-160,-300 if i==1 else -290],'duration':1} for i in (1,2)]},
    ]
    return root,clips

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('source',type=Path);parser.add_argument('output',type=Path)
    args=parser.parse_args();root,clips=source_manifest(args.source)
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_bytes(build({'clips':clips},root))
    print('Built private Down/air/swim poses. Use export_source_clips.py for rendered bare triple/backflip clips.')
