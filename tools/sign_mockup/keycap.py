import math, random
from PIL import Image, ImageDraw, ImageFont
SS=4
RIM=(200,200,200,255); FACE=(160,160,160,255); WHITE=(255,255,255,255)
def wobble(points, amp, seed):
    """Low-frequency hand-drawn wobble (smooth, like the original keycaps' edges)."""
    r=random.Random(seed); p1,p2=r.uniform(0,6.28),r.uniform(0,6.28)
    n=len(points); out=[]
    cx=sum(x for x,_ in points)/n; cy=sum(y for _,y in points)/n
    for i,(x,y) in enumerate(points):
        t=2*math.pi*i/n; k=amp*(0.6*math.sin(2*t+p1)+0.4*math.sin(3*t+p2))
        dx,dy=x-cx,y-cy; L=math.hypot(dx,dy) or 1
        out.append((x+dx/L*k, y+dy/L*k))
    return out
def shape_pts(shape,w,h,n=48):
    cx,cy=w/2,h/2
    if shape=='circle':
        return [(cx+w/2*math.cos(2*math.pi*i/n), cy+h/2*math.sin(2*math.pi*i/n)) for i in range(n)]
    if shape=='square':
        e=[]
        for i in range(8): e.append((w*i/8,0))
        for i in range(8): e.append((w,h*i/8))
        for i in range(8): e.append((w-w*i/8,h))
        for i in range(8): e.append((0,h-h*i/8))
        return e
    if shape=='trigger':
        pts=[(w/2+w/2*math.cos(math.pi+math.pi*i/12), h*0.45+h*0.45*math.sin(math.pi+math.pi*i/12)) for i in range(13)]
        return pts+[(w,h),(0,h)]
    if shape=='pill':
        r=h/2; pts=[]
        for i in range(10): a=math.pi/2+math.pi*i/9; pts.append((r+r*math.cos(a), r+r*math.sin(a)))
        for i in range(10): a=-math.pi/2+math.pi*i/9; pts.append((w-r+r*math.cos(a), r+r*math.sin(a)))
        return pts
def glyph(d,label,box,color,width,font):
    ft=ImageFont.truetype(font,int((box[3]-box[1])*0.8))
    bb=d.textbbox((0,0),label,font=ft)
    x=(box[0]+box[2]-(bb[2]-bb[0]))/2-bb[0]; y=(box[1]+box[3]-(bb[3]-bb[1]))/2-bb[1]
    d.text((x,y),label,font=ft,fill=color,stroke_width=width,stroke_fill=color)
def keycap(label, shape='square', size=60, face=FACE, rim=RIM, ink=WHITE, seed=1, tilt=0, aspect=1.0):
    S=size*SS; W=int(S*aspect)
    im=Image.new('RGBA',(W+S//4,S+S//4)); d=ImageDraw.Draw(im)
    off=S//8
    outer=[(x+off,y+off) for x,y in wobble(shape_pts(shape,W,S),S*0.03,seed)]
    d.polygon(outer,fill=rim)
    inset=S*0.13
    inner=[(x+off+inset,y+off+inset) for x,y in wobble(shape_pts(shape,W-2*inset,S-2*inset),S*0.025,seed+7)]
    d.polygon(inner,fill=face)
    if label:
        glyph(d,label,(off+inset,off+inset,off+W-inset,off+S-inset),ink,int(S*0.035),'/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf')
    im=im.rotate(tilt,resample=Image.BICUBIC)
    return im.resize((im.width//SS,im.height//SS),Image.LANCZOS)
