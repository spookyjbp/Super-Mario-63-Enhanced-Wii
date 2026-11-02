import sys, math, random
sys.path.insert(0,'/tmp/claude-0/mock')
from keycap import wobble, shape_pts
import marker
from PIL import Image, ImageDraw, ImageFont
SS=4
GREY_RIM=(205,205,205,255); GREY_FACE=(158,158,158,255); WHITE=(255,255,255,255)
def tint(c,k): return tuple(min(255,int(v+(255-v)*k)) for v in c[:3])+(255,)
def button(label, shape, size, face, rim, ink=WHITE, seed=1, tilt=0, aspect=1.0, weight=0.26, extra=None):
    S=size*SS; W=int(S*aspect); off=S//6
    im=Image.new('RGBA',(W+2*off,S+2*off)); d=ImageDraw.Draw(im)
    if shape=='stick':
        gate=[(off+W/2+W/2*math.cos(math.pi/8+i*math.pi/4), off+S/2+S/2*math.sin(math.pi/8+i*math.pi/4)) for i in range(8)]
        d.polygon(wobble(gate,S*0.02,seed),fill=rim)
        inner=[(off+W/2+W*0.32*math.cos(2*math.pi*i/40), off+S/2+S*0.32*math.sin(2*math.pi*i/40)) for i in range(40)]
        d.polygon(wobble(inner,S*0.015,seed+3),fill=face)
        if label: marker.draw(d,label,(off+W*0.3,off+S*0.3,off+W*0.7,off+S*0.7),ink,weight,seed)
    elif shape=='dpad':
        t=S/3; arm=[(t,0),(2*t,0),(2*t,t),(S,t),(S,2*t),(2*t,2*t),(2*t,S),(t,S),(t,2*t),(0,2*t),(0,t),(t,t)]
        d.polygon([(x+off,y+off) for x,y in arm],fill=rim)
        i=S*0.07; inner=[(t+i,i),(2*t-i,i),(2*t-i,t+i),(S-i,t+i),(S-i,2*t-i),(2*t-i,2*t-i),(2*t-i,S-i),(t+i,S-i),(t+i,2*t-i),(i,2*t-i),(i,t+i),(t+i,t+i)]
        d.polygon([(x+off,y+off) for x,y in inner],fill=face)
        if extra=='down': d.polygon([(off+S/2,off+S*0.93),(off+S*0.4,off+S*0.75),(off+S*0.6,off+S*0.75)],fill=ink)
    else:
        outer=[(x+off,y+off) for x,y in wobble(shape_pts(shape,W,S),S*0.025,seed)]
        d.polygon(outer,fill=rim)
        ins=S*0.12
        inner=[(x+off+ins,y+off+ins) for x,y in wobble(shape_pts(shape,W-2*ins,S-2*ins),S*0.02,seed+7)]
        d.polygon(inner,fill=face)
        tb=(off+ins+S*0.12,off+ins+S*0.14+(S*0.12 if shape=='trigger' else 0),off+W-ins-S*0.12,off+S-ins-S*0.14)
        if label: marker.draw(d,label,tb,ink,weight,seed)
    im=im.rotate(tilt,resample=Image.BICUBIC)
    return im.resize((im.width//SS,im.height//SS),Image.LANCZOS)
GC_COL={'A':(40,190,120),'B':(220,60,60),'Z':(110,90,220),'C':(240,200,40)}
def gc(label, shape, size, aspect=1.0, seed=1, tilt=0):
    if label in GC_COL:
        f=GC_COL[label]+(255,); ink=(60,50,0,255) if label=='C' else WHITE
        return button(label,shape,size,f,tint(f,0.45),ink,seed,tilt,aspect)
    return button(label,shape,size,GREY_FACE,GREY_RIM,WHITE,seed,tilt,aspect)
def wii(label, shape, size, aspect=1.0, seed=1, tilt=0, extra=None):
    return button(label,shape,size,GREY_FACE,GREY_RIM,WHITE,seed,tilt,aspect,extra=extra)
SETS={
 'GameCube Controller':[('Move',gc('','stick',60)),('Jump',gc('A','circle',64,seed=2,tilt=-3)),('Spin',gc('B','circle',48,seed=3,tilt=4)),
                        ('Stomp / Read',gc('Z','pill',44,2.0,seed=4,tilt=-2)),('FLUDD',gc('R','trigger',52,1.3,seed=5,tilt=3)),
                        ('Nozzle',gc('L','trigger',52,1.3,seed=6,tilt=-3)),('Pause',gc('ST','pill',36,2.2,seed=7)),('Zoom',gc('C','circle',48,seed=8))],
 'Wii Remote + Nunchuk':[('Move',wii('','stick',60)),('Jump',wii('A','circle',60,seed=2,tilt=-3)),('Spin',wii('B','trigger',52,1.2,seed=3,tilt=3)),
                        ('Stomp / Read',wii('Z','square',48,1.4,seed=4,tilt=-2)),('FLUDD',wii('1','circle',44,seed=5,tilt=3)),
                        ('Nozzle',wii('2','circle',44,seed=6,tilt=-3)),('Pause',wii('+','circle',36,seed=7)),('Zoom',wii('-','circle',36,seed=8))],
 'Classic Controller':[('Move',wii('','stick',60)),('Jump',wii('a','circle',48,seed=2,tilt=-3)),('Spin',wii('x','circle',48,seed=3,tilt=3)),
                        ('Stomp / Read',wii('ZR','pill',44,1.9,seed=4,tilt=-2)),('FLUDD',wii('y','circle',48,seed=5,tilt=3)),
                        ('Nozzle',wii('ZL','pill',44,1.9,seed=6,tilt=-3)),('Pause',wii('+','circle',36,seed=7)),('Zoom',wii('','stick',48,seed=8))],
 'Wii Remote':[('Move',wii('','dpad',60)),('Jump',wii('A','circle',60,seed=2,tilt=-3)),('Spin',wii('B','trigger',52,1.2,seed=3,tilt=3)),
               ('Stomp / Read',wii('','dpad',52,extra='down')),('FLUDD',wii('1','circle',44,seed=5,tilt=3)),
               ('Nozzle',wii('2','circle',44,seed=6,tilt=-3)),('Pause',wii('+','circle',36,seed=7)),('Zoom',wii('-','circle',36,seed=8))],
}
if __name__=='__main__':
    F='/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf'
    bg=Image.open('/tmp/claude-0/sg/out2/319.png').convert('RGBA')
    W=Image.new('RGBA',(1000,4*150+10),(255,255,255,255)); d=ImageDraw.Draw(W)
    for r,(name,items) in enumerate(SETS.items()):
        y=10+r*150
        panel=bg.crop((20,20,466,268)).resize((980,140)); W.alpha_composite(panel,(10,y))
        d.text((20,y+4),name,font=ImageFont.truetype(F,20),fill=(0,0,0,255))
        x=20
        for lab,im in items:
            W.alpha_composite(im,(x,y+30+(80-im.height)//2))
            d.text((x+im.width//2,y+122),lab,font=ImageFont.truetype(F,14),fill=(30,30,30,255),anchor='ms')
            x+=max(im.width,95)+20
    W.save('/tmp/claude-0/mock/sets.png')
