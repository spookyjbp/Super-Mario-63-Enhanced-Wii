import sys; sys.path.insert(0,'/tmp/claude-0/mock')
from sets import button, gc, wii, GREY_FACE, GREY_RIM, WHITE
import marker
from PIL import Image, ImageDraw, ImageFont, ImageFilter
SS=4
def plus_sign(size):
    S=size*SS; im=Image.new('RGBA',(S,S)); d=ImageDraw.Draw(im)
    marker.draw(d,'+',(S*0.1,S*0.1,S*0.9,S*0.9),(60,60,60,255),0.22,seed=9)
    return im.resize((size,size),Image.LANCZOS)
def combo(ims, gap=4):
    p=plus_sign(int(ims[0].height*0.45))
    parts=[ims[0],p,ims[1]]
    W=sum(i.width for i in parts)+gap*2; H=max(i.height for i in parts)
    out=Image.new('RGBA',(W,H)); x=0
    for i in parts: out.alpha_composite(i,(x,(H-i.height)//2)); x+=i.width+gap
    return out
def label(text, size):
    F='/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf'
    ft=ImageFont.truetype(F,size); d=ImageDraw.Draw(Image.new('RGBA',(1,1)))
    bb=d.textbbox((0,0),text,font=ft,stroke_width=3)
    im=Image.new('RGBA',(bb[2]-bb[0]+4,bb[3]-bb[1]+4)); d=ImageDraw.Draw(im)
    d.text((2-bb[0],2-bb[1]),text,font=ft,fill=(255,255,255,255),stroke_width=3,stroke_fill=(40,40,40,255))
    return im
DPAD_GC=lambda s: button('', 'dpad', s, GREY_FACE, GREY_RIM)
CONTROLLERS={
 'GameCube Controller': dict(dpad=DPAD_GC, jump=lambda: gc('A','circle',62,seed=2,tilt=-3), spin=lambda: gc('B','circle',46,seed=3,tilt=4),
                             stomp=lambda: gc('Z','pill',40,2.0,seed=4,tilt=-2), fludd=lambda: gc('R','trigger',50,1.3,seed=5,tilt=3)),
 'Classic Controller': dict(dpad=DPAD_GC, jump=lambda: wii('b','circle',52,seed=2,tilt=-3), spin=lambda: wii('y','circle',52,seed=3,tilt=4),
                            stomp=lambda: combo_or([wii('L','trigger',44,1.25,seed=4,tilt=-3), wii('R','trigger',44,1.25,seed=6,tilt=3)],'or'),
                            fludd=lambda: wii('ZR','pill',42,1.9,seed=5,tilt=3)),
 'Wii Remote': dict(dpad=DPAD_GC, jump=lambda: wii('2','circle',50,seed=2,tilt=-3), spin=lambda: wii('1','circle',50,seed=3,tilt=4),
                    stomp=lambda: combo([wii('A','circle',50,seed=4,tilt=-2), wii('B','trigger',48,1.2,seed=6,tilt=3)]),
                    fludd=lambda: wii('B','trigger',50,1.2,seed=5,tilt=3)),
}
def combo_or(ims, word):
    w=label(word,18)
    parts=[ims[0],w,ims[1]]; W=sum(i.width for i in parts)+8; H=max(i.height for i in parts)
    out=Image.new('RGBA',(W,H)); x=0
    for i in parts: out.alpha_composite(i,(x,(H-i.height)//2)); x+=i.width+4
    return out
def page(name):
    bare=Image.open('/tmp/claude-0/mock/hide/controls_bare.png').convert('RGBA')
    c=CONTROLLERS[name]
    p=bare.copy()
    # Move: a big D-pad keycap where the arrow keys were, with "Move" kept on top
    dp=c['dpad'](170)
    p.alpha_composite(dp,(118-dp.width//2,176-dp.height//2))
    # the diving Mario that sat on the Down arrow key (bitmap in shape 363), same place, under "Move"
    dive=Image.open('/tmp/claude-0/mock/mario363_0.png').convert('RGBA').resize((22,14),Image.NEAREST)
    p.alpha_composite(dive,(107,222))
    move=bare.crop((50,140,195,200)); mask=move.getchannel('A').point(lambda v:255 if v>200 else 0)
    p.paste(move,(50,140),mask)
    # Actions: a 2 x 2 grid (Jump, Spin / Stomp-Read, FLUDD), labels under the buttons
    items=[('Jump',c['jump']()),('Spin',c['spin']()),('Stomp / Read',c['stomp']()),('FLUDD',c['fludd']())]
    cells=[(298,172),(422,172),(298,236),(422,236)]
    for (t,im),(cx,cy) in zip(items,cells):
        if im.width>118: im=im.resize((118,int(im.height*118/im.width)),Image.LANCZOS)
        if im.height>52: im=im.resize((int(im.width*52/im.height),52),Image.LANCZOS)
        p.alpha_composite(im,(int(cx-im.width/2),int(cy-im.height/2)))
        lb=label(t,14); p.alpha_composite(lb,(int(cx-lb.width/2),cy+24))
    return p
if __name__=='__main__':
    back=Image.open('/tmp/claude-0/b8/cmp_0_new.png').convert('RGBA').resize((854,480))
    rows=[]
    for n in CONTROLLERS:
        scr=back.copy(); pg=page(n); scr.alpha_composite(pg,((854-pg.width)//2,(480-pg.height)//2))
        d=ImageDraw.Draw(scr); d.text((10,8),n,font=ImageFont.truetype('/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf',22),fill=(255,255,255,255),stroke_width=3,stroke_fill=(0,0,0,255))
        rows.append(scr.crop((150,60,704,420)))
    orig=back.copy(); o=Image.open('/tmp/claude-0/sg/orig_controls.png').convert('RGBA'); orig.alpha_composite(o,((854-o.width)//2,(480-o.height)//2))
    ImageDraw.Draw(orig).text((10,8),'Original (keyboard)',font=ImageFont.truetype('/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf',22),fill=(255,255,255,255),stroke_width=3,stroke_fill=(0,0,0,255))
    rows=[orig.crop((150,60,704,420))]+rows
    W=Image.new('RGB',(554*2+10,360*2+10),(255,255,255))
    for i,r in enumerate(rows): W.paste(r.convert('RGB'),((554+10)*(i%2),(360+10)*(i//2)))
    W.save('/tmp/claude-0/mock/controls_mock.png')
