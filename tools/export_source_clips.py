#!/usr/bin/env python3
"""Optional, private FFDec export of the original compiled bare-Mario tweens.
No ActionScript execution, no downloads, no hand-interpolated motion curves.
Conditional composite FLUDD clips are deliberately not exported as correct art.
"""
import argparse,hashlib,json,re,struct,subprocess,zlib
from pathlib import Path
import xml.etree.ElementTree as ET
from build_player_art import build
from import_source_poses import source_manifest
SOURCE_SHA='6f8dc22d74963cb75928fcac6ad67769384c3088bb419f9995e24b81d8411e1c'

def without_wing_cap(source):
    body=zlib.decompress(source[8:]) if source[:3]==b'CWS' else source[8:]
    start=(5+4*(body[0]>>3)+7)//8+4
    out=bytearray(body[:start]);position=start;found=False
    while position+2<=len(body):
        header=struct.unpack_from('<H',body,position)[0];position+=2
        code,length=header>>6,header&63
        if length==63:length=struct.unpack_from('<I',body,position)[0];position+=4
        data=body[position:position+length];position+=length
        if len(data)!=length:raise ValueError('Truncated SWF tag')
        if code==39 and struct.unpack_from('<H',data)[0]==1003:
            if b'WingCap' not in data:raise ValueError('Unexpected Wing Code symbol')
            # Original Wing Code script switches to blank frame when WingCap=false.
            # Static export does not execute that script, so make that state explicit.
            data=struct.pack('<HHHHH',1003,2,64,64,0);found=True
        length=len(data);out+=struct.pack('<H',(code<<6)|min(length,63))
        if length>=63:out+=struct.pack('<I',length)
        out+=data
    if not found:raise ValueError('Expected Wing Code was not found')
    return b'FWS'+source[3:4]+struct.pack('<I',len(out)+8)+out

def export(source,jar,output):
    swf=(source/'Publish/Super_Mario_63_2010.swf').read_bytes()
    if hashlib.sha256(swf).hexdigest()!=SOURCE_SHA:raise ValueError('Unsupported SWF revision; do not guess character IDs')
    directory=output.parent/'rendered-source';directory.mkdir(parents=True,exist_ok=True)
    patched=directory/'no-wing-cap.swf';patched.write_bytes(without_wing_cap(swf))
    command=['java','-Xmx1g',f'-Duser.home={directory.resolve()}/ffdec-home','-Djava.awt.headless=true','-jar',str(jar.resolve())]
    with (directory/'export.log').open('w') as log:
        subprocess.run(command+['-onerror','abort','-ignorebackground','-selectid','1088,1093','-export','sprite',str(directory/'png'),str(patched)],check=True,stdout=log,stderr=log)
        subprocess.run(command+['-onerror','abort','-ignorebackground','-selectid','1088,1093','-select','1088:1,1093:1','-format','sprite:svg','-export','sprite',str(directory/'svg'),str(patched)],check=True,stdout=log,stderr=log)
    root,clips=source_manifest(source)
    for clip in clips:
        for frame in clip['frames']:frame['file']=str((root/frame['file']).resolve())
    ns={'s':'http://www.w3.org/2000/svg'}
    for ident,clip_id,count in [(1088,5,22),(1093,4,30)]:
        svg=ET.parse(directory/'svg'/f'DefineSprite_{ident}'/'1.svg').getroot()
        transform=svg.find('s:g',ns).get('transform')
        values=[float(v) for v in re.findall(r'[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?',transform)]
        if len(values)!=6 or values[:4]!=[1,0,0,1]:raise ValueError('Unexpected SVG registration transform')
        offset=[round(-values[4]*20),round(-values[5]*20)]
        frames=[]
        for i in range(1,count+1):
            image=directory/'png'/f'DefineSprite_{ident}'/f'{i}.png'
            if not image.exists():raise ValueError('Missing rendered frame')
            frames.append({'file':str(image.resolve()),'offset_twips':offset,'duration':1})
        clips.append({'id':clip_id,'loop':False,'frames':frames})
    data={'canonical_facing':'left','source_swf_sha256':SOURCE_SHA,'clips':clips}
    output.write_bytes(build(data,root))
    (directory/'manifest.json').write_text(json.dumps(data,indent=2)+'\n')
    print('Exported original 22-frame bare backflip and 30-frame triple tween, plus source static poses.')
    print('FLUDD composites remain incomplete: static FFDec export does not execute their conditional scripts.')

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('source',type=Path);p.add_argument('ffdec_jar',type=Path);p.add_argument('output',type=Path)
    a=p.parse_args();a.output.parent.mkdir(parents=True,exist_ok=True);export(a.source,a.ffdec_jar,a.output)
