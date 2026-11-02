import sys, math; sys.path.insert(0,'/tmp/claude-0/mock')
from sets import button, gc, wii, GREY_FACE, GREY_RIM, WHITE
from controls_page import combo, combo_or, label, DPAD_GC
import marker
from PIL import Image, ImageDraw, ImageFont
F='/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf'
SG='/tmp/claude-0/sg'
def align(page,bg):
    import numpy as np
    a=np.asarray(page,dtype=np.int16); best=None
    for dy in range(-2,3):
        for dx in range(-2,3):
            c=Image.new('RGBA',page.size); c.paste(bg,(dx,dy)); b=np.asarray(c,dtype=np.int16)
            m=(b[...,3]>0)&(a[...,3]>0); e=np.abs(a-b).sum(-1)[m].mean()
            if best is None or e<best[0]: best=(e,c)
    return best[1]
def clear(page,bg,box): page.paste(bg.crop(box),box[:2])
def arrow(direction,size,color=(255,255,255,255)):
    S=size*4; im=Image.new('RGBA',(S,S)); d=ImageDraw.Draw(im)
    pts=[(0.5,0.92),(0.5,0.12)] ; head=[(0.22,0.4),(0.5,0.1),(0.78,0.4)]
    for st in (pts,head):
        q=[(x*S,y*S) for x,y in st]; d.line(q,fill=(60,60,60,255),width=int(S*0.26),joint='curve')
    for st in (pts,head):
        q=[(x*S,y*S) for x,y in st]; d.line(q,fill=color,width=int(S*0.15),joint='curve')
    im=im.resize((size,size),Image.LANCZOS)
    return im if direction=='up' else im.transpose(Image.FLIP_TOP_BOTTOM)
def row(parts,gap=6):
    W=sum(p.width for p in parts)+gap*(len(parts)-1); H=max(p.height for p in parts)
    o=Image.new('RGBA',(W,H)); x=0
    for p in parts: o.alpha_composite(p,(x,(H-p.height)//2)); x+=p.width+gap
    return o
# Buttons per controller (size argument = height in px)
B={
 'GameCube Controller': dict(move=lambda s: DPAD_GC(s), spin=lambda s: gc('B','circle',s,seed=3,tilt=4),
     stomp=lambda s: gc('Z','pill',int(s*0.7),2.0,seed=4,tilt=-2), fludd=lambda s: gc('R','trigger',s,1.3,seed=5,tilt=3),
     zin=lambda s: row([gc('C','circle',s,seed=8),arrow('up',int(s*0.8))],2), zout=lambda s: row([gc('C','circle',s,seed=8),arrow('down',int(s*0.8))],2)),
 'Classic Controller': dict(move=lambda s: DPAD_GC(s), spin=lambda s: wii('y','circle',s,seed=3,tilt=4),
     stomp=lambda s: combo_or([wii('L','trigger',int(s*0.85),1.25,seed=4,tilt=-3),wii('R','trigger',int(s*0.85),1.25,seed=6,tilt=3)],'or'),
     fludd=lambda s: wii('ZR','pill',int(s*0.75),1.9,seed=5,tilt=3),
     zin=lambda s: row([wii('R','stick',s,seed=8),arrow('up',int(s*0.8))],2), zout=lambda s: row([wii('R','stick',s,seed=8),arrow('down',int(s*0.8))],2)),
 'Wii Remote': dict(move=lambda s: DPAD_GC(s), spin=lambda s: wii('1','circle',s,seed=3,tilt=4),
     stomp=lambda s: combo([wii('A','circle',s,seed=4,tilt=-2),wii('B','trigger',s,1.2,seed=6,tilt=3)]),
     fludd=lambda s: wii('B','trigger',s,1.2,seed=5,tilt=3),
     zin=lambda s: wii('+','circle',s,seed=7), zout=lambda s: wii('-','circle',s,seed=8)),
}
def fit(im,w,h):
    k=min(w/im.width,h/im.height,1.0)
    return im.resize((max(1,int(im.width*k)),max(1,int(im.height*k))),Image.LANCZOS) if k<1 else im
def put(page,im,cx,cy): page.alpha_composite(im,(int(cx-im.width/2),int(cy-im.height/2)))
def bomb(c):
    import numpy as np
    o=Image.open(f'{SG}/orig_bomb.png').convert('RGBA'); bg=align(o,Image.open(f'{SG}/out2/406.png').convert('RGBA'))
    a=np.asarray(o).astype(int); x0,y0,x1,y1=372,30,482,114
    reg=a[y0:y1,x0:x1]
    rgb=reg[...,:3]; flat=(rgb.max(-1)-rgb.min(-1))<10; v=rgb.mean(-1)
    # the keycap's own tones only (rim ~200, face ~160, white letter), opaque
    grey=(reg[...,3]>240)&flat&((abs(v-200)<16)|(abs(v-160)<16)|(v>244))
    from PIL import ImageFilter
    m=Image.fromarray((grey*255).astype(np.uint8)).filter(ImageFilter.MaxFilter(3))
    o.paste(bg.crop((x0,y0,x1,y1)),(x0,y0),m)
    # the explosion (bitmap 412 in shape 413) was partly under the old X keycap: drawn whole again
    ex=Image.open('/tmp/claude-0/mock/explosion_412.png').convert('RGBA').resize((75,59),Image.NEAREST)
    o.alpha_composite(ex,(359,80))
    put(o,fit(B[c]['spin'](54),64,58),456,94); return o
def hover(c):
    o=Image.open(f'{SG}/orig_hover.png').convert('RGBA'); bg=align(o,Image.open(f'{SG}/out2/560.png').convert('RGBA'))
    clear(o,bg,(374,3,446,64)); put(o,fit(B[c]['fludd'](54),70,58),410,33); return o
def stomp(c):
    # keycaps hidden in the render itself (Z keycap 416, the boxes' keycap 417 in 424)
    o=Image.open('/tmp/claude-0/mock/hide/stomp_bare.png').convert('RGBA')
    # the boxes' Mario ground-pound demo (clip 424, 101 frames; animated in game): a mid-pound frame here
    import glob
    fs=sorted(glob.glob('/tmp/claude-0/mock/hide/a424/*/*.png'),key=lambda f:int(f.split('/')[-1][:-4]))
    for frame,(bx,by) in ((44,(12,147)),(56,(324,147))):
        clip=Image.open(fs[frame]).convert('RGBA')
        o.alpha_composite(clip,(bx-24,by-8))
    put(o,fit(B[c]['stomp'](56),86,60),436,38)
    for cx in (139,453):
        put(o,fit(B[c]['stomp'](32),40,30),cx,171)
    return o
def zoom(c):
    o=Image.open(f'{SG}/orig_zoom.png').convert('RGBA'); bg=align(o,Image.open(f'{SG}/out2/319.png').convert('RGBA'))
    clear(o,bg,(60,218,228,286)); clear(o,bg,(300,206,440,286))
    blue=(0,51,255,255)
    def word(t):
        ft=ImageFont.truetype(F,30); d=ImageDraw.Draw(Image.new('RGBA',(1,1))); bb=d.textbbox((0,0),t,font=ft,stroke_width=2)
        im=Image.new('RGBA',(bb[2]-bb[0]+4,bb[3]-bb[1]+4)); ImageDraw.Draw(im).text((2-bb[0],2-bb[1]),t,font=ft,fill=blue,stroke_width=2,stroke_fill=(255,255,255,255)); return im
    put(o,row([fit(B[c]['zout'](46),110,50),word('Out')]),140,250)
    put(o,row([fit(B[c]['zin'](46),110,50),word('In')]),372,246)
    return o
def start(c):
    o=Image.open(f'{SG}/orig_Start.png').convert('RGBA'); bg=align(o,Image.open(f'{SG}/out2/255.png').convert('RGBA'))
    clear(o,bg,(0,0,o.width,o.height))
    white=(255,255,255,255); ft=ImageFont.truetype(F,27)
    def line(parts,y):
        ims=[]
        for p in parts:
            if isinstance(p,str):
                d=ImageDraw.Draw(Image.new('RGBA',(1,1))); bb=d.textbbox((0,0),p,font=ft)
                im=Image.new('RGBA',(bb[2]-bb[0]+2,40)); ImageDraw.Draw(im).text((1-bb[0],33),p,font=ft,fill=white,anchor='ls'); ims.append(im)
            else: ims.append(p)
        r=row(ims,7); put(o,r,o.width/2,y)
    line(['Welcome to the Mushroom'],30); line(['Kingdom!  The First thing you need'],64)
    line(['to know is how to move (',fit(B[c]['move'](40),44,40),'),'],100)
    line(['and how to read...'],138)
    line(['...signs by pressing',fit(B[c]['stomp'](44),120,46),'.  Try'],258)
    line(['reading that sign over there.'],302)
    A=Image.new('RGBA',(o.width*4,o.height*4)); a=ImageDraw.Draw(A)
    def bez(p0,p1,p2,p3,n=40): return [tuple(4*((1-t)**3*p0[i]+3*(1-t)**2*t*p1[i]+3*(1-t)*t*t*p2[i]+t**3*p3[i]) for i in (0,1)) for t in (k/n for k in range(n+1))]
    path=bez((505,300),(578,298),(570,226),(505,212))+bez((505,212),(455,200),(395,200),(350,212))[1:]
    a.line(path,fill=white,width=16,joint='curve'); a.polygon([(326*4,218*4),(352*4,196*4),(354*4,232*4)],fill=white)
    o.alpha_composite(A.resize(o.size,Image.LANCZOS)); return o
if __name__=='__main__':
    back=Image.open('/tmp/claude-0/b8/cmp_0_new.png').convert('RGBA').resize((854,480))
    ft=ImageFont.truetype(F,20)
    for name,fn in (('Start',start),('bomb',bomb),('hover',hover),('stomp',stomp),('zoom',zoom)):
        tiles=[]
        for c in ['original']+list(B):
            pg=Image.open(f'{SG}/orig_{name}.png').convert('RGBA') if c=='original' else fn(c)
            scr=back.copy(); scr.alpha_composite(pg,((854-pg.width)//2,(480-pg.height)//2))
            t=scr.crop(((854-pg.width)//2-14,(480-pg.height)//2-34,(854+pg.width)//2+14,(480+pg.height)//2+10))
            ImageDraw.Draw(t).text((8,6),'Original (keyboard)' if c=='original' else c,font=ft,fill=(255,255,255,255),stroke_width=3,stroke_fill=(0,0,0,255))
            tiles.append(t)
        w,h=tiles[0].size; W=Image.new('RGB',(w*2+10,h*2+10),(255,255,255))
        for i,t in enumerate(tiles): W.paste(t.convert('RGB'),((w+10)*(i%2),(h+10)*(i//2)))
        W.save(f'/tmp/claude-0/mock/sign_{name}.png')
