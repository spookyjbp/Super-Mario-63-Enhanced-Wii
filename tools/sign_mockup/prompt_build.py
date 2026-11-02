import sys
sys.path.insert(0,'/home/claude/super-mario-63-enhanced-wii/tools')
import numpy as np
from PIL import Image
import sign_buttons  # build(kind, scale=1.6) is the approved size
W='/tmp/claude-0/p250/work/'
text=Image.open(W+'solid/DefineSprite_250/50.png').convert('RGBA')
glow=Image.open(W+'gradient/DefineSprite_250/50.png').convert('RGBA')

def words(im):
    a=np.asarray(im)[...,3]>40
    cols=np.flatnonzero(a.any(0)); out=[]; s=cols[0]
    for p,q in zip(cols,cols[1:]):
        if q-p>5: out.append((s,p+1)); s=q
    out.append((s,cols[-1]+1)); return out

from PIL import ImageDraw
TEAL=None
def teal():
    a=np.asarray(text); i=np.unravel_index(a[...,3].argmax(), a.shape[:2]); return tuple(int(v) for v in a[i][:3])
def joiner(word, xh):
    """'or' cut from the prompt's own letters (r of Press, o of to); '+' drawn in their colour and weight."""
    if word=='or':
        r=text.crop((54,0,64,text.height)); o=text.crop((135,0,149,text.height))
        im=Image.new('RGBA',(r.width+o.width-1,text.height)); im.alpha_composite(o,(0,0)); im.alpha_composite(r,(o.width-1,0))
        return im
    S=4; s=int(xh*1.1); im=Image.new('RGBA',(s*S,text.height*S)); d=ImageDraw.Draw(im)
    c=teal()+(255,); cy=(XMID)*S; w=2.4*S
    d.line([(s*S*0.1,cy),(s*S*0.9,cy)],fill=c,width=int(w)); d.line([(s*S/2,cy-s*S*0.4),(s*S/2,cy+s*S*0.4)],fill=c,width=int(w))
    return im.resize((s,text.height),Image.LANCZOS)
def buttons(kind, h):
    c=sign_buttons
    if kind=='gamecube': return [c.STYLE[kind]['stomp'](h)]
    if kind=='classic': return [c.wii('L','trigger',int(h*0.85),1.25,seed=4,tilt=-3), 'or', c.wii('R','trigger',int(h*0.85),1.25,seed=6,tilt=3)]
    return [c.wii('A','circle',h,seed=4,tilt=-2), '+', c.wii('B','trigger',h,1.2,seed=6,tilt=3)]
def tight(im):
    return im.crop(Image.fromarray(np.asarray(im)[...,3]>40).getbbox())

def build(kind, scale=1.45):
    w=words(text); assert len(w)==4, w
    (p0,p1),(z0,z1),(t0,t1),(c0,c1)=w
    rows=np.flatnonzero((np.asarray(text)[...,3]>40)[:, z0:z1].any(1)); cap0,cap1=rows[0],rows[-1]+1   # the Z: cap height
    space=z0-p1                                               # the original word spacing around "Z"
    global XMID
    xr=np.flatnonzero((np.asarray(text)[...,3]>40)[:, 56:62].any(1)); XMID=(xr[0]+xr[-1]+1)/2   # middle of the x-height (r)
    parts=[]
    for b in buttons(kind, int(round((cap1-cap0)*scale))):
        parts.append(('j',joiner(b, xr[-1]+1-xr[0])) if isinstance(b,str) else ('b',tight(b)))
    gap=max(3,space//2)
    bw=sum(p.width for _,p in parts)+gap*(len(parts)-1)
    bh=max(p.height for k,p in parts if k=='b')
    btn=Image.new('RGBA',(bw,max(bh,text.height))); x=0
    for k,p in parts:
        if k=='b': btn.alpha_composite(p,(x,(btn.height-p.height)//2))
        else: btn.alpha_composite(p,(x,round(btn.height/2-((cap0+cap1)/2))))
        x+=p.width+gap
    left=text.crop((0,0,p1,text.height)); right=text.crop((t0,0,text.width,text.height))
    width=left.width+space+btn.width+space+right.width
    H=max(text.height, btn.height+4)
    top=(H-text.height)//2
    out=Image.new('RGBA',(width,H))
    # the glow: the original's, stretched to the new text width around the same centre
    tw_old=c1-p0; tw_new=width-p0-(text.width-c1)
    g=glow.resize((round(glow.width*tw_new/tw_old),glow.height),Image.BICUBIC)
    out.alpha_composite(g,(round(width/2-g.width/2),top))
    out.alpha_composite(left,(0,top))
    out.alpha_composite(right,(left.width+space+btn.width+space,top))
    cy=top+(cap0+cap1)/2
    out.alpha_composite(btn,(left.width+space,round(cy-btn.height/2)))
    return out

if __name__=='__main__':
    box=Image.open('/home/claude/super-mario-63-enhanced-wii/private-art/rendered-course-art/ui/message/DefineSprite_253/6.png').convert('RGBA')
    rows=[]
    for kind in sign_buttons.CONTROLLERS:
        p=build(kind)
        # in the message box, scene behind
        scene=Image.new('RGBA',(box.width,170),(200,190,140,255))
        crop=box.crop((0,box.height-170,box.width,box.height)); scene.alpha_composite(crop)
        scene.alpha_composite(p,((scene.width-p.width)//2,95))
        rows.append(scene)
    orig=Image.new('RGBA',(box.width,170),(200,190,140,255)); orig.alpha_composite(box.crop((0,box.height-170,box.width,box.height)))
    o=Image.open('/home/claude/super-mario-63-enhanced-wii/private-art/rendered-course-art/ui/prompt/DefineSprite_250/50.png').convert('RGBA')
    orig.alpha_composite(o,((orig.width-o.width)//2,95))
    rows=[orig]+rows
    G=Image.new('RGBA',(box.width,sum(r.height+6 for r in rows)),(255,255,255,255)); y=0
    for r in rows: G.alpha_composite(r,(0,y)); y+=r.height+6
    G.save('/tmp/claude-0/-home-claude/192740dc-52e7-5be8-a481-0ecad433746e/scratchpad/prompt_redo.png'); print(G.size)
