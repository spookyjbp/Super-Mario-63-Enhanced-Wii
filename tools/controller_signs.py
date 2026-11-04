#!/usr/bin/env python3
"""Controller versions of the tutorial's keyboard picture signs (the design
approved with the user, docs/sign-design/).

The original picture pages (Message Picture 619) teach keyboard keys. Each
page keeps the original art; only the keys are replaced by the controller's
buttons (tools/sign_buttons.py: the original keycap style, every button in its
real shape, GameCube buttons in their real colours):

* controls: the arrow keys (327 holds Z/X/C, 329/358/359/363 the arrows, in
  371) are removed from the render itself; a D-pad takes the arrows' place
  with "Move" kept on top, the diving Mario of the Down key (bitmap 362 in
  363) on its Down arm, and a 2 x 2 grid of Jump / Spin / Stomp-Read / FLUDD.
* Start: the text redrawn with the D-pad and read button inline, and the
  original's curved arrow.
* bomb: the X keycap's own pixels cleared; the explosion (bitmap 412, partly
  under the key) drawn whole again; the spin button right of it.
* hover: the C keycap cleared to the page shape; the FLUDD button.
* stomp: the Z keycap (416) and the boxes' keycaps (417 in 424) removed from
  the render; the boxes show Mario's ground-pound demo (424) and the button.
* zoom: Out / In in the original blue lettering with the zoom input.

Buttons come from the port's mapping (source/game/input_map.c):

    action        GameCube    Classic       Wii Remote (sideways)
    move          D-pad       D-pad         Control Pad
    jump          A           b             2
    spin          B           y             1
    stomp / read  Z           L or R        A + B together
    FLUDD         R           ZR            B
    zoom          C-Stick     Right stick   + in, - out

Pages are named "<label>@<n>", n = the CONTROLLER_* value in include/game.h.
"""
import math
import numpy as np
from PIL import Image, ImageDraw, ImageFilter, ImageFont
import sign_buttons as B

CONTROLLERS = B.CONTROLLERS  # include/game.h CONTROLLER_* order
PAGES = ['Start', 'controls', 'bomb', 'hover', 'stomp', 'zoom']
# Each page's background shape in Message Picture 619: keyboard parts clear to it.
BACKGROUND = {'Start': 255, 'controls': 319, 'bomb': 406, 'hover': 560, 'stomp': 414, 'zoom': 319}
# Keys removed from the render itself: {sprite: {characters}} (ui_art.hide).
HIDE = {371: {327, 329, 358, 359, 363}, 619: {416}, 424: {417}}
DIVE_BITMAP, EXPLOSION_BITMAP = 362, 412
DEMO_SPRITE, DEMO_FRAMES = 424, (45, 57)   # the boxes' demo: a mid-pound frame in each box
FONT = B.FONT
# Controls page button heights (as approved)
CONTROL_SIZES = {'gamecube': dict(jump=62, spin=46, stomp=58, fludd=50),
                 'classic': dict(jump=52, spin=52, stomp=52, fludd=56),
                 'wiimote': dict(jump=50, spin=50, stomp=50, fludd=50)}


def align(page, bg):
    """The background shape placed where it lies in the page (FFDec's crops can
    differ by a pixel): the shift (-2..2) that matches the page best."""
    a = np.asarray(page, dtype=np.int16)
    best = None
    for dy in range(-2, 3):
        for dx in range(-2, 3):
            c = Image.new('RGBA', page.size)
            c.paste(bg, (dx, dy))
            b = np.asarray(c, dtype=np.int16)
            m = (b[..., 3] > 0) & (a[..., 3] > 0)
            err = np.abs(a - b).sum(-1)[m].mean()
            if best is None or err < best[0]:
                best = (err, c)
    return best[1]


def snap(page, im, x, y, reach=16):
    """(x, y) moved to where im's opaque pixels (the box outline the page
    already shows) match the page best: FFDec's sprite canvases differ by a few
    pixels between the SVG and PNG exports."""
    a = np.asarray(im).astype(int)
    m = a[..., 3] > 200
    P = np.asarray(page).astype(int)
    best = None
    for dy in range(-reach, reach + 1):
        for dx in range(-reach, reach + 1):
            X, Y = x + dx, y + dy
            x0, y0 = max(0, -X), max(0, -Y)
            x1, y1 = min(im.width, page.width - X), min(im.height, page.height - Y)
            if x1 <= x0 or y1 <= y0:
                continue
            mm = m[y0:y1, x0:x1]
            if mm.sum() < 50:
                continue
            e = np.abs(P[Y + y0:Y + y1, X + x0:X + x1, :3] - a[y0:y1, x0:x1, :3]).sum(-1)[mm].mean()
            if best is None or e < best[0]:
                best = (e, X, Y)
    return best[1], best[2]


def clear(page, bg, box):
    page.paste(bg.crop(box), box[:2])


def fit(im, w, h):
    k = min(w / im.width, h / im.height, 1.0)
    return im.resize((max(1, int(im.width * k)), max(1, int(im.height * k))), Image.LANCZOS) if k < 1 else im


def put(page, im, cx, cy):
    page.alpha_composite(im, (int(cx - im.width / 2), int(cy - im.height / 2)))


def label(text, size):
    ft = ImageFont.truetype(FONT, size)
    bb = ImageDraw.Draw(Image.new('RGBA', (1, 1))).textbbox((0, 0), text, font=ft, stroke_width=3)
    im = Image.new('RGBA', (bb[2] - bb[0] + 4, bb[3] - bb[1] + 4))
    ImageDraw.Draw(im).text((2 - bb[0], 2 - bb[1]), text, font=ft, fill=(255, 255, 255, 255), stroke_width=3,
                            stroke_fill=(40, 40, 40, 255))
    return im


def controls(page, bare, kind, art):
    s, c = CONTROL_SIZES[kind], B.STYLE[kind]
    p = bare.copy()
    # Move: one D-pad where the arrow keys were, "Move" kept on top
    put(p, c['move'](170), 118, 176)
    # the diving Mario that sat on the Down arrow key
    p.alpha_composite(art['dive'].resize((22, 14), Image.NEAREST), (107, 222))
    move = bare.crop((50, 140, 195, 200))
    p.paste(move, (50, 140), move.getchannel('A').point(lambda v: 255 if v > 200 else 0))
    # Actions: Jump, Spin / Stomp-Read, FLUDD, labels under the buttons
    items = [('Jump', c['jump'](s['jump'])), ('Spin', c['spin'](s['spin'])),
             ('Stomp / Read', c['stomp'](s['stomp'])), ('FLUDD', c['fludd'](s['fludd']))]
    for (t, im), (cx, cy) in zip(items, [(298, 172), (422, 172), (298, 236), (422, 236)]):
        put(p, fit(im, 118, 52), cx, cy)
        lb = label(t, 14)
        p.alpha_composite(lb, (int(cx - lb.width / 2), cy + 24))
    return p


def bomb(page, bg, kind, art):
    o = page.copy()
    a = np.asarray(o).astype(int)
    x0, y0, x1, y1 = 372, 30, 482, 114
    reg = a[y0:y1, x0:x1]
    rgb = reg[..., :3]
    flat = (rgb.max(-1) - rgb.min(-1)) < 10
    v = rgb.mean(-1)
    # the keycap's own tones only (rim ~200, face ~160, white letter), opaque
    grey = (reg[..., 3] > 240) & flat & ((abs(v - 200) < 16) | (abs(v - 160) < 16) | (v > 244))
    m = Image.fromarray((grey * 255).astype(np.uint8)).filter(ImageFilter.MaxFilter(3))
    o.paste(bg.crop((x0, y0, x1, y1)), (x0, y0), m)
    o.alpha_composite(art['explosion'].resize((75, 59), Image.NEAREST), (359, 80))
    put(o, fit(B.STYLE[kind]['spin'](54), 64, 58), 456, 94)
    return o


def hover(page, bg, kind, art):
    o = page.copy()
    clear(o, bg, (374, 3, 446, 64))
    put(o, fit(B.STYLE[kind]['fludd'](54), 70, 58), 410, 33)
    return o


def stomp(page, bare, kind, art):
    o = bare.copy()
    for im, x, y in art['demo']:   # may start left of / above the page: clip
        sx, sy = max(0, -x), max(0, -y)
        o.alpha_composite(im.crop((sx, sy, im.width, im.height)), (x + sx, y + sy))
    put(o, fit(B.STYLE[kind]['stomp'](56), 86, 60), 436, 38)
    for cx in (139, 453):
        put(o, fit(B.STYLE[kind]['stomp'](32), 40, 30), cx, 171)
    return o


def zoom(page, bg, kind, art):
    o = page.copy()
    clear(o, bg, (60, 218, 228, 286))
    clear(o, bg, (300, 206, 440, 286))
    blue = (0, 51, 255, 255)

    def word(t):
        ft = ImageFont.truetype(FONT, 30)
        bb = ImageDraw.Draw(Image.new('RGBA', (1, 1))).textbbox((0, 0), t, font=ft, stroke_width=2)
        im = Image.new('RGBA', (bb[2] - bb[0] + 4, bb[3] - bb[1] + 4))
        ImageDraw.Draw(im).text((2 - bb[0], 2 - bb[1]), t, font=ft, fill=blue, stroke_width=2,
                                stroke_fill=(255, 255, 255, 255))
        return im
    c = B.STYLE[kind]
    put(o, B.row([fit(c['zoom_out'](46), 110, 50), word('Out')], 6), 140, 250)
    put(o, B.row([fit(c['zoom_in'](46), 110, 50), word('In')], 6), 372, 246)
    return o


def start(page, bg, kind, art):
    o = page.copy()
    clear(o, bg, (0, 0, o.width, o.height))
    white = (255, 255, 255, 255)
    ft = ImageFont.truetype(FONT, 27)
    c = B.STYLE[kind]

    def line(parts, y):
        ims = []
        for p in parts:
            if isinstance(p, str):
                bb = ImageDraw.Draw(Image.new('RGBA', (1, 1))).textbbox((0, 0), p, font=ft)
                im = Image.new('RGBA', (bb[2] - bb[0] + 2, 40))
                ImageDraw.Draw(im).text((1 - bb[0], 33), p, font=ft, fill=white, anchor='ls')
                ims.append(im)
            else:
                ims.append(p)
        put(o, B.row(ims, 7), o.width / 2, y)
    line(['Welcome to the Mushroom'], 30)
    line(['Kingdom!  The First thing you need'], 64)
    line(['to know is how to move (', fit(c['move'](40), 44, 40), '),'], 100)
    line(['and how to read...'], 138)
    # the Welcome page is dark: the Wii Remote's "+" in white
    stomp = (B.combo(B.wii('A', 'circle', 44, seed=4, tilt=-2), B.wii('B', 'trigger', 44, 1.2, seed=6, tilt=3), white)
             if kind == 'wiimote' else c['stomp'](44))
    line(['...signs by pressing', fit(stomp, 120, 46), '.  Try'], 258)
    line(['reading that sign over there.'], 302)
    A = Image.new('RGBA', (o.width * 4, o.height * 4))
    a = ImageDraw.Draw(A)

    def bez(p0, p1, p2, p3, n=40):
        return [tuple(4 * ((1 - t) ** 3 * p0[i] + 3 * (1 - t) ** 2 * t * p1[i] + 3 * (1 - t) * t * t * p2[i]
                           + t ** 3 * p3[i]) for i in (0, 1)) for t in (k / n for k in range(n + 1))]
    path = bez((505, 300), (578, 298), (570, 226), (505, 212)) + bez((505, 212), (455, 200), (395, 200), (350, 212))[1:]
    a.line(path, fill=white, width=16, joint='curve')
    a.polygon([(326 * 4, 218 * 4), (352 * 4, 196 * 4), (354 * 4, 232 * 4)], fill=white)
    o.alpha_composite(A.resize(o.size, Image.LANCZOS))
    return o


def make(label_, original, background, kind, art):
    """original: the rendered page cropped to its content; background: its
    background shape; art: {'controls', 'stomp': the page rendered without its
    keys (same crop), 'dive', 'explosion': bitmaps, 'demo': [(image, x, y)]}."""
    bg = align(original, background)
    if label_ == 'controls':
        return controls(original, art['controls'], kind, art)
    if label_ == 'stomp':
        return stomp(original, art['stomp'], kind, art)
    return {'Start': start, 'bomb': bomb, 'hover': hover, 'zoom': zoom}[label_](original, bg, kind, art)
