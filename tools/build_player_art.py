#!/usr/bin/env python3
"""Build a private artwork pack from user-owned/exported frames. No downloads."""
import argparse
import json
import struct
from pathlib import Path
from PIL import Image,ImageOps
from build_mario_atlas import rgba8_tiles

def build(data,root):
    size=512;atlas=Image.new('RGBA',(size,size));clips=[];frames=[];x=y=2;row=0;seen=set()
    for clip in data['clips']:
        ident=clip['id']
        if type(ident) is not int or not 0<=ident<64 or ident in seen:raise ValueError('Invalid/duplicate clip ID')
        seen.add(ident);first=len(frames)
        for frame in clip['frames']:
            im=Image.open(root/frame['file']).convert('RGBA')
            if frame.get('mirror',False):im=ImageOps.mirror(im)
            if not 1<=im.width<=128 or not 1<=im.height<=128:raise ValueError('Frame too large')
            if x+im.width+2>size:x=2;y+=row+4;row=0
            if y+im.height+2>size:raise ValueError('Atlas full; use fewer frames')
            ox,oy=frame['offset_twips'];duration=frame.get('duration',1)
            if any(type(v) is not int for v in (ox,oy,duration)) or not -2048<=ox<=2048 or not -2048<=oy<=2048 or not 1<=duration<=65535:
                raise ValueError('Invalid offset/duration')
            atlas.paste(im,(x,y));frames.append((x,y,im.width,im.height,ox,oy,duration,0))
            x+=im.width+4;row=max(row,im.height)
        count=len(frames)-first
        if not count:raise ValueError('Empty clip')
        clips.append((ident,first,count,int(clip.get('loop',False))|2*int(clip.get('equipped',False))))
    if not 1<=len(frames)<=256:raise ValueError('Invalid frame count')
    output=bytearray(struct.pack('>8s6I',b'SM63ART1',1,size,size,len(clips),len(frames),size*size*4))
    for c in clips:output.extend(struct.pack('>4H',*c))
    for f in frames:output.extend(struct.pack('>4H2h2H',*f))
    output.extend(bytes((-len(output))%32));output.extend(rgba8_tiles(atlas))
    return bytes(output)

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('manifest',type=Path);parser.add_argument('output',type=Path)
    args=parser.parse_args();data=json.loads(args.manifest.read_text())
    if data.get('canonical_facing')!='left':raise ValueError('Export frames facing LEFT, as in the original XFL clips')
    result=build(data,args.manifest.parent)
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_bytes(result)
    print('Wrote private player artwork pack; not included in GitHub artifacts')
