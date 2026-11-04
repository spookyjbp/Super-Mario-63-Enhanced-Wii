#!/usr/bin/env python3
"""Controller button illustrations for the signs (design approved with the user,
docs/sign-design/): the original keycap style (light rim, darker face, thick
hand-drawn marker letters), each button in its real shape. GameCube buttons in
their real colours, Classic Controller and Wii Remote buttons grey.

STYLE[controller][action](height) -> RGBA image. Controllers: 'gamecube',
'classic', 'wiimote' (include/game.h CONTROLLER_* order); actions: move, jump,
spin, stomp, fludd, nozzle, zoom_in, zoom_out, pause.
"""
import math, random
from PIL import Image, ImageDraw, ImageFont
FONT = '/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf'

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

# Hand-drawn marker letters: strokes on a 0..1 box (x right, y down).
G = {
 'A': [[(0.12,1),(0.5,0),(0.88,1)],[(0.27,0.62),(0.73,0.62)]],
 'B': [[(0.18,0),(0.18,1)],[(0.18,0),(0.6,0),(0.78,0.12),(0.78,0.36),(0.6,0.48),(0.18,0.48)],
       [(0.18,0.48),(0.66,0.48),(0.86,0.62),(0.86,0.86),(0.66,1),(0.18,1)]],
 'Z': [[(0.12,0.02),(0.88,0.02),(0.12,0.98),(0.88,0.98)]],
 'R': [[(0.18,1),(0.18,0)],[(0.18,0),(0.62,0),(0.82,0.14),(0.82,0.38),(0.62,0.52),(0.18,0.52)],[(0.45,0.52),(0.86,1)]],
 'L': [[(0.2,0),(0.2,1),(0.85,1)]],
 'C': [[(0.86,0.18),(0.68,0.02),(0.4,0.02),(0.16,0.25),(0.16,0.75),(0.4,0.98),(0.68,0.98),(0.86,0.82)]],
 'X': [[(0.12,0),(0.88,1)],[(0.88,0),(0.12,1)]],
 'Y': [[(0.12,0),(0.5,0.5),(0.88,0)],[(0.5,0.5),(0.5,1)]],
 'S': [[(0.84,0.15),(0.62,0.02),(0.34,0.02),(0.16,0.18),(0.2,0.4),(0.5,0.5),(0.8,0.6),(0.84,0.82),(0.64,0.98),(0.34,0.98),(0.14,0.85)]],
 'T': [[(0.08,0.02),(0.92,0.02)],[(0.5,0.02),(0.5,1)]],
 'a': [[(0.78,0.35),(0.78,1)],[(0.78,0.5),(0.55,0.32),(0.3,0.36),(0.18,0.62),(0.3,0.95),(0.58,0.98),(0.78,0.82)]],
 'b': [[(0.22,0),(0.22,1)],[(0.22,0.55),(0.45,0.33),(0.7,0.36),(0.84,0.66),(0.7,0.96),(0.45,0.98),(0.22,0.8)]],
 'x': [[(0.18,0.34),(0.82,1)],[(0.82,0.34),(0.18,1)]],
 'y': [[(0.18,0.34),(0.5,0.78)],[(0.84,0.34),(0.45,1.02),(0.3,1.12)]],
 '1': [[(0.3,0.2),(0.55,0.02),(0.55,1)]],
 '2': [[(0.18,0.22),(0.36,0.04),(0.64,0.04),(0.82,0.24),(0.76,0.48),(0.18,1),(0.86,1)]],
 '+': [[(0.12,0.5),(0.88,0.5)],[(0.5,0.12),(0.5,0.88)]],
 '-': [[(0.12,0.5),(0.88,0.5)]],
}
WIDTH = {'1': 0.6, '-': 0.8, '+': 0.9, 'a': 0.8, 'b': 0.8, 'x': 0.8, 'y': 0.8, 'L': 0.75, 'T': 0.9}
def marker_draw(d, text, box, color, weight=0.16, seed=3, tilt=0.0):
    """Marker text centred in box (x0,y0,x1,y1); weight = stroke width / letter height."""
    x0,y0,x1,y1 = box
    h = (y1-y0)
    widths = [WIDTH.get(c,1.0)*h*0.72 for c in text]
    gap = h*0.12
    total = sum(widths)+gap*(len(text)-1)
    if total > (x1-x0):
        k = (x1-x0)/total; h*=k; widths=[w*k for w in widths]; gap*=k; total=(x1-x0)
    x = (x0+x1-total)/2; top = (y0+y1-h)/2
    r = random.Random(seed); lw = max(1, int(h*weight))
    for c, w in zip(text, widths):
        for stroke in G[c]:
            pts=[(x+px*w+r.uniform(-0.02,0.02)*h, top+py*h+r.uniform(-0.02,0.02)*h) for px,py in stroke]
            d.line(pts, fill=color, width=lw, joint='curve')
            for p in (pts[0], pts[-1]):
                d.ellipse([p[0]-lw/2,p[1]-lw/2,p[0]+lw/2,p[1]+lw/2], fill=color)
        x += w+gap


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
        if label: marker_draw(d,label,(off+W*0.3,off+S*0.3,off+W*0.7,off+S*0.7),ink,weight,seed)
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
        if label: marker_draw(d,label,tb,ink,weight,seed)
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

def plus_sign(size, colour=(60, 60, 60, 255)):
    S = size * SS
    im = Image.new('RGBA', (S, S))
    marker_draw(ImageDraw.Draw(im), '+', (S * 0.1, S * 0.1, S * 0.9, S * 0.9), colour, 0.22, seed=9)
    return im.resize((size, size), Image.LANCZOS)


def word(text, size):
    ft = ImageFont.truetype(FONT, size)
    bb = ImageDraw.Draw(Image.new('RGBA', (1, 1))).textbbox((0, 0), text, font=ft, stroke_width=3)
    im = Image.new('RGBA', (bb[2] - bb[0] + 4, bb[3] - bb[1] + 4))
    ImageDraw.Draw(im).text((2 - bb[0], 2 - bb[1]), text, font=ft, fill=(255, 255, 255, 255), stroke_width=3,
                            stroke_fill=(40, 40, 40, 255))
    return im


def row(parts, gap=4):
    W = sum(p.width for p in parts) + gap * (len(parts) - 1)
    H = max(p.height for p in parts)
    out = Image.new('RGBA', (W, H))
    x = 0
    for p in parts:
        out.alpha_composite(p, (x, (H - p.height) // 2))
        x += p.width + gap
    return out


def combo(a, b, colour=(60, 60, 60, 255)):
    """Two buttons pressed together: A + B (a light colour for dark pages)."""
    return row([a, plus_sign(int(a.height * 0.45), colour), b])


def either(a, b):
    """Either button: L or R."""
    return row([a, word('or', 18), b])   # as approved (the word stays 18 px)


def arrow(direction, size):
    S = size * 4
    im = Image.new('RGBA', (S, S))
    d = ImageDraw.Draw(im)
    for width, colour in ((0.26, (60, 60, 60, 255)), (0.15, (255, 255, 255, 255))):
        for st in ([(0.5, 0.92), (0.5, 0.12)], [(0.22, 0.4), (0.5, 0.1), (0.78, 0.4)]):
            d.line([(x * S, y * S) for x, y in st], fill=colour, width=int(S * width), joint='curve')
    im = im.resize((size, size), Image.LANCZOS)
    return im if direction == 'up' else im.transpose(Image.FLIP_TOP_BOTTOM)


DPAD = lambda s: button('', 'dpad', s, GREY_FACE, GREY_RIM)
STYLE = {
    'gamecube': dict(move=DPAD, jump=lambda s: gc('A', 'circle', s, seed=2, tilt=-3),
                     spin=lambda s: gc('B', 'circle', s, seed=3, tilt=4),
                     stomp=lambda s: gc('Z', 'pill', int(s * 0.7), 2.0, seed=4, tilt=-2),
                     fludd=lambda s: gc('R', 'trigger', s, 1.3, seed=5, tilt=3),
                     nozzle=lambda s: gc('L', 'trigger', s, 1.3, seed=6, tilt=-3),
                     zoom_in=lambda s: row([gc('C', 'circle', s, seed=8), arrow('up', int(s * 0.8))], 2),
                     zoom_out=lambda s: row([gc('C', 'circle', s, seed=8), arrow('down', int(s * 0.8))], 2),
                     pause=lambda s: gc('ST', 'pill', int(s * 0.7), 2.2, seed=7)),
    'classic': dict(move=DPAD, jump=lambda s: wii('b', 'circle', s, seed=2, tilt=-3),
                    spin=lambda s: wii('y', 'circle', s, seed=3, tilt=4),
                    stomp=lambda s: either(wii('L', 'trigger', int(s * 0.85), 1.25, seed=4, tilt=-3),
                                           wii('R', 'trigger', int(s * 0.85), 1.25, seed=6, tilt=3)),
                    fludd=lambda s: wii('ZR', 'pill', int(s * 0.75), 1.9, seed=5, tilt=3),
                    nozzle=lambda s: wii('ZL', 'pill', int(s * 0.75), 1.9, seed=6, tilt=-3),
                    zoom_in=lambda s: row([wii('R', 'stick', s, seed=8), arrow('up', int(s * 0.8))], 2),
                    zoom_out=lambda s: row([wii('R', 'stick', s, seed=8), arrow('down', int(s * 0.8))], 2),
                    pause=lambda s: wii('+', 'circle', s, seed=7)),
    'wiimote': dict(move=DPAD, jump=lambda s: wii('2', 'circle', s, seed=2, tilt=-3),
                    spin=lambda s: wii('1', 'circle', s, seed=3, tilt=4),
                    stomp=lambda s: combo(wii('A', 'circle', s, seed=4, tilt=-2), wii('B', 'trigger', s, 1.2, seed=6, tilt=3)),
                    fludd=lambda s: wii('B', 'trigger', s, 1.2, seed=5, tilt=3),
                    nozzle=lambda s: wii('A', 'circle', s, seed=4, tilt=-2),
                    zoom_in=lambda s: wii('+', 'circle', s, seed=7), zoom_out=lambda s: wii('-', 'circle', s, seed=8),
                    pause=lambda s: combo(wii('+', 'circle', s, seed=7), wii('-', 'circle', s, seed=8))),
}
CONTROLLERS = ['gamecube', 'classic', 'wiimote']
