#!/usr/bin/env python3
"""Wii controller versions of the tutorial's keyboard picture signs.

The original picture pages (Message Picture 619) teach keyboard keys: arrow
keys and Z on "Start", the Z/X/C keycaps on "controls", X on "bomb", C on
"hover", Z on "stomp" and Minus/Plus on "zoom". For each supported controller
this module keeps the original page artwork and replaces only the keyboard
parts: they are cleared back to the page's own background shape (319, or 255
for "Start"), and the action is redrawn with that controller's button.

Buttons come from the port's real mapping (source/platform/input.c and
source/game/input_map.c, README "Controls"):

    action        GameCube    Wii Remote + Nunchuk   Classic       Wii Remote
    move          Stick       Nunchuk stick          Left stick    Control Pad
    jump          A           A                      a             A
    spin          B           B (or C)               x             B
    stomp / read  Z           Nunchuk Z              ZR            Down (Pad)
    FLUDD         R           1                      y             1
    nozzle        L           2                      ZL            2
    pause         START       +                      +             +
    zoom          C-Stick     - (steps)              Right stick   - (steps)

Pages are named "<label>@<n>", n = the CONTROLLER_* value in include/game.h.
"""
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont

CONTROLLERS = ['gamecube', 'nunchuk', 'classic', 'wiimote']  # include/game.h CONTROLLER_* order
PAGES = ['Start', 'controls', 'bomb', 'hover', 'stomp', 'zoom']
# Each page's background shape in Message Picture 619 (placed on that frame):
# keyboard parts are cleared back to it.
BACKGROUND = {'Start': 255, 'controls': 319, 'bomb': 406, 'hover': 560, 'stomp': 414, 'zoom': 319}
SS = 4  # supersampling for the drawn parts

FONTS = ['/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf',
         '/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf']


def font(size):
    for f in FONTS:
        if Path(f).exists():
            return ImageFont.truetype(f, size)
    raise FileNotFoundError('No bold sans font for the controller signs')


# Button styles: (shape, label, fill, text colour). Shapes: round, small, rect,
# trigger, pill, stick, dpad, dpad_down.
GC_A = ('round', 'A', (40, 190, 120), (255, 255, 255))
GC_B = ('round', 'B', (220, 50, 50), (255, 255, 255))
GC_Z = ('rect', 'Z', (110, 90, 220), (255, 255, 255))
GC_R = ('trigger', 'R', (175, 175, 185), (40, 40, 50))
GC_L = ('trigger', 'L', (175, 175, 185), (40, 40, 50))
GC_START = ('pill', 'START', (175, 175, 185), (40, 40, 50))
GC_STICK = ('stick', '', (175, 175, 185), (40, 40, 50))
GC_CSTICK = ('round', 'C', (240, 200, 40), (60, 50, 0))
WII_A = ('round', 'A', (238, 238, 242), (60, 60, 70))
WII_B = ('trigger', 'B', (238, 238, 242), (60, 60, 70))
WII_1 = ('small', '1', (238, 238, 242), (60, 60, 70))
WII_2 = ('small', '2', (238, 238, 242), (60, 60, 70))
WII_PLUS = ('small', '+', (238, 238, 242), (60, 60, 70))
WII_MINUS = ('small', '\u2212', (238, 238, 242), (60, 60, 70))
WII_PAD = ('dpad', '', (238, 238, 242), (60, 60, 70))
WII_DOWN = ('dpad_down', '', (238, 238, 242), (60, 60, 70))
NUN_STICK = ('stick', '', (238, 238, 242), (60, 60, 70))
NUN_Z = ('rect', 'Z', (238, 238, 242), (60, 60, 70))
CC_A = ('small', 'a', (238, 238, 242), (60, 60, 70))
CC_X = ('small', 'x', (238, 238, 242), (60, 60, 70))
CC_Y = ('small', 'y', (238, 238, 242), (60, 60, 70))
CC_ZR = ('rect', 'ZR', (238, 238, 242), (60, 60, 70))
CC_ZL = ('rect', 'ZL', (238, 238, 242), (60, 60, 70))
CC_LSTICK = ('stick', 'L', (238, 238, 242), (60, 60, 70))
CC_RSTICK = ('stick', 'R', (238, 238, 242), (60, 60, 70))

MAP = {
    'gamecube': {'name': 'GameCube Controller', 'move': [GC_STICK], 'jump': [GC_A], 'spin': [GC_B],
                 'stomp': [GC_Z], 'fludd': [GC_R], 'nozzle': [GC_L], 'pause': [GC_START],
                 'zoom_in': [GC_CSTICK], 'zoom_out': [GC_CSTICK], 'zoom_stick': True},
    'nunchuk': {'name': 'Wii Remote + Nunchuk', 'move': [NUN_STICK], 'jump': [WII_A], 'spin': [WII_B],
                'stomp': [NUN_Z], 'fludd': [WII_1], 'nozzle': [WII_2], 'pause': [WII_PLUS],
                'zoom_in': [WII_MINUS], 'zoom_out': [WII_MINUS], 'zoom_stick': False},
    'classic': {'name': 'Classic Controller', 'move': [CC_LSTICK], 'jump': [CC_A], 'spin': [CC_X],
                'stomp': [CC_ZR], 'fludd': [CC_Y], 'nozzle': [CC_ZL], 'pause': [WII_PLUS],
                'zoom_in': [CC_RSTICK], 'zoom_out': [CC_RSTICK], 'zoom_stick': True},
    'wiimote': {'name': 'Wii Remote', 'move': [WII_PAD], 'jump': [WII_A], 'spin': [WII_B],
                'stomp': [WII_DOWN], 'fludd': [WII_1], 'nozzle': [WII_2], 'pause': [WII_PLUS],
                'zoom_in': [WII_MINUS], 'zoom_out': [WII_MINUS], 'zoom_stick': False},
}
OUTLINE = (20, 20, 24, 255)


def button(style, size, arrow=None):
    """One button illustration, `size` px tall (drawn at SS x, then reduced).
    arrow: 'up' / 'down' adds a direction arrow to a stick."""
    shape, label, fill, ink = style
    S = size * SS
    w = {'rect': 1.3, 'trigger': 1.2, 'pill': 2.3}.get(shape, 1.0)
    W = int(S * w)
    im = Image.new('RGBA', (W, S))
    d = ImageDraw.Draw(im)
    o = max(2, S // 14)  # outline width
    f = tuple(fill) + (255,)
    if shape in ('round', 'small'):
        r = S // 2 if shape == 'round' else int(S * 0.42)
        c = (W // 2, S // 2)
        d.ellipse([c[0] - r, c[1] - r, c[0] + r, c[1] + r], fill=f, outline=OUTLINE, width=o)
        d.ellipse([c[0] - r + o * 2, c[1] - r + o * 2, c[0] + r - o * 2, c[1] - r // 3], fill=(255, 255, 255, 70))
        tsize = int(r * (1.25 if len(label) == 1 else 0.9))
    elif shape == 'rect':
        d.rounded_rectangle([o, S * 0.12, W - o, S * 0.88], radius=S // 6, fill=f, outline=OUTLINE, width=o)
        tsize = int(S * (0.55 if len(label) == 1 else 0.42))
    elif shape == 'trigger':
        d.rounded_rectangle([o, S * 0.25, W - o, S - o], radius=S // 7, fill=f, outline=OUTLINE, width=o)
        d.pieslice([o, o, W - o, S * 0.7], 180, 360, fill=f, outline=OUTLINE, width=o)
        d.rectangle([o * 2, S * 0.3, W - o * 2, S * 0.4], fill=f)
        tsize = int(S * 0.55)
    elif shape == 'pill':
        d.rounded_rectangle([o, S * 0.22, W - o, S * 0.78], radius=S // 3, fill=f, outline=OUTLINE, width=o)
        tsize = int(S * 0.3)
    elif shape == 'stick':
        c = (W // 2, S // 2)
        d.ellipse([o, o, W - o, S - o], fill=(90, 90, 100, 255), outline=OUTLINE, width=o)
        r = int(S * 0.32)
        d.ellipse([c[0] - r, c[1] - r, c[0] + r, c[1] + r], fill=f, outline=OUTLINE, width=o)
        d.ellipse([c[0] - r // 2, c[1] - r // 2, c[0] + r // 2, c[1] + r // 2], outline=(0, 0, 0, 60), width=o)
        tsize = int(S * 0.3)
        if arrow:
            a, h = S * 0.18, S * 0.16
            y0 = o * 2 if arrow == 'up' else S - o * 2
            dy = h if arrow == 'up' else -h
            d.polygon([(c[0], y0), (c[0] - a, y0 + dy), (c[0] + a, y0 + dy)], fill=(255, 220, 40, 255),
                      outline=OUTLINE)
    elif shape in ('dpad', 'dpad_down'):
        t = S // 3
        arm = [(t, o), (2 * t, o), (2 * t, t), (S - o, t), (S - o, 2 * t), (2 * t, 2 * t), (2 * t, S - o),
               (t, S - o), (t, 2 * t), (o, 2 * t), (o, t), (t, t)]
        d.polygon(arm, fill=(70, 70, 80, 255), outline=OUTLINE)
        d.line(arm + [arm[0]], fill=OUTLINE, width=o)
        if shape == 'dpad_down':
            d.rectangle([t + o, 2 * t, 2 * t - o, S - o * 2], fill=(255, 220, 40, 255))
        label, tsize = '', 1
    if label:
        ft = font(max(8, tsize))
        bb = d.textbbox((0, 0), label, font=ft)
        tx = (W - (bb[2] - bb[0])) / 2 - bb[0]
        ty = (S - (bb[3] - bb[1])) / 2 - bb[1] + (S * 0.08 if shape == 'trigger' else 0)
        d.text((tx, ty), label, font=ft, fill=tuple(ink) + (255,))
    return im.resize((max(1, W // SS), size), Image.LANCZOS)


def buttons(styles, size, gap=4, arrow=None):
    ims = [button(s, size, arrow) for s in styles]
    out = Image.new('RGBA', (sum(i.width for i in ims) + gap * (len(ims) - 1), size))
    x = 0
    for i in ims:
        out.alpha_composite(i, (x, 0))
        x += i.width + gap
    return out


def badge(kind, height):
    """Small controller silhouette (bottom corner of the overview pages)."""
    S = height * SS
    W = int(S * (1.9 if kind in ('nunchuk', 'gamecube', 'classic') else 0.7))
    im = Image.new('RGBA', (W, S))
    d = ImageDraw.Draw(im)
    o = max(2, S // 20)
    white, grey = (240, 240, 244, 255), (100, 100, 110, 255)
    if kind == 'gamecube':
        purple = (95, 80, 180, 255)
        d.ellipse([o, S * 0.25, W * 0.42, S - o], fill=purple, outline=OUTLINE, width=o)
        d.ellipse([W * 0.58, S * 0.25, W - o, S - o], fill=purple, outline=OUTLINE, width=o)
        d.rounded_rectangle([W * 0.15, S * 0.1, W * 0.85, S * 0.7], radius=S // 4, fill=purple, outline=OUTLINE,
                            width=o)
        d.ellipse([W * 0.2, S * 0.22, W * 0.36, S * 0.5], fill=grey, outline=OUTLINE, width=o)
        d.ellipse([W * 0.62, S * 0.25, W * 0.76, S * 0.5], fill=(40, 190, 120, 255), outline=OUTLINE, width=o)
        d.ellipse([W * 0.56, S * 0.48, W * 0.66, S * 0.62], fill=(240, 200, 40, 255), outline=OUTLINE, width=o)
    elif kind == 'classic':
        d.ellipse([o, S * 0.3, W * 0.38, S - o], fill=white, outline=OUTLINE, width=o)
        d.ellipse([W * 0.62, S * 0.3, W - o, S - o], fill=white, outline=OUTLINE, width=o)
        d.rounded_rectangle([W * 0.1, S * 0.12, W * 0.9, S * 0.72], radius=S // 5, fill=white, outline=OUTLINE,
                            width=o)
        for cx in (0.3, 0.62):
            d.ellipse([W * cx, S * 0.45, W * cx + S * 0.2, S * 0.65], fill=grey, outline=OUTLINE, width=o)
        for cx, cy in ((0.78, 0.22), (0.72, 0.32), (0.84, 0.32), (0.78, 0.42)):
            d.ellipse([W * cx - S * 0.05, S * cy - S * 0.05, W * cx + S * 0.05, S * cy + S * 0.05], fill=grey)
    elif kind == 'nunchuk':
        d.rounded_rectangle([W * 0.58, o, W * 0.92, S - o], radius=S // 7, fill=white, outline=OUTLINE, width=o)
        d.ellipse([W * 0.68, S * 0.1, W * 0.82, S * 0.22], fill=grey)
        d.ellipse([o, S * 0.3, W * 0.42, S - o], fill=white, outline=OUTLINE, width=o)
        d.ellipse([W * 0.12, S * 0.42, W * 0.3, S * 0.62], fill=grey, outline=OUTLINE, width=o)
        d.line([(W * 0.42, S * 0.6), (W * 0.58, S * 0.7)], fill=OUTLINE, width=o)
    else:  # wiimote
        d.rounded_rectangle([o, o, W - o, S - o], radius=S // 7, fill=white, outline=OUTLINE, width=o)
        t = W // 6
        d.rectangle([W / 2 - t / 2, S * 0.1, W / 2 + t / 2, S * 0.26], fill=grey)
        d.rectangle([W / 2 - t * 1.5, S * 0.155, W / 2 + t * 1.5, S * 0.205], fill=grey)
        d.ellipse([W / 2 - t, S * 0.32, W / 2 + t, S * 0.32 + 2 * t], fill=grey)
    return im.resize((max(1, W // SS), height), Image.LANCZOS)


def arrow_image(direction, size, fill):
    """A plain up/down arrow (stick direction), white edged like the page text."""
    S = size * SS
    im = Image.new('RGBA', (int(S * 0.7), S))
    d = ImageDraw.Draw(im)
    w, o = im.width, SS * 2
    shaft = [(w * 0.35, S * 0.4), (w * 0.65, S * 0.4), (w * 0.65, S - o), (w * 0.35, S - o)]
    head = [(w / 2, o), (w - o, S * 0.45), (o, S * 0.45)]
    for poly in (shaft, head):
        d.polygon(poly, fill=fill, outline=(255, 255, 255, 255))
    d.line(head + [head[0]], fill=(255, 255, 255, 255), width=o)
    im = im.resize((max(1, im.width // SS), size), Image.LANCZOS)
    return im if direction == 'up' else im.transpose(Image.FLIP_TOP_BOTTOM)


def text(draw, xy, s, size, fill, outline=None, anchor='la'):
    ft = font(size)
    if outline:
        draw.text(xy, s, font=ft, fill=fill, stroke_width=max(2, size // 9), stroke_fill=outline, anchor=anchor)
    else:
        draw.text(xy, s, font=ft, fill=fill, anchor=anchor)
    return draw.textbbox(xy, s, font=ft, anchor=anchor)


def align(page, bg):
    """The background shape placed where it lies in the page (FFDec's crops can
    differ by a pixel): the shift (-2..2) that matches the page best."""
    import numpy as np
    a = np.asarray(page, dtype=np.int16)
    best = None
    for dy in range(-2, 3):
        for dx in range(-2, 3):
            c = Image.new('RGBA', page.size)
            c.paste(bg, (dx, dy))
            b = np.asarray(c, dtype=np.int16)
            m = (b[..., 3] > 250) & (a[..., 3] > 250)
            err = np.abs(a[..., :3] - b[..., :3]).sum(-1)[m].mean()
            if best is None or err < best[0]:
                best = (err, c)
    return best[1]


def clear(page, bg, box):
    """Back to the page's own background shape inside box (where it is drawn)."""
    region = bg.crop(box)
    page.paste(region, box[:2], region.getchannel('A').point(lambda v: 255 if v > 0 else 0))


def rich_line(page, x, y, parts, size, fill, outline=None, centre=None):
    """Text and button images on one line; returns the line's width. With centre,
    x is ignored and the line is centred on it."""
    d = ImageDraw.Draw(page)
    ft = font(size)
    widths = []
    for p in parts:
        if isinstance(p, str):
            bb = d.textbbox((0, 0), p, font=ft)
            widths.append(bb[2] - bb[0] + (2 if outline else 0))
        else:
            widths.append(p.width)
    total = sum(widths) + 6 * (len(parts) - 1)
    if centre is not None:
        x = int(centre - total / 2)
    for p, w in zip(parts, widths):
        if isinstance(p, str):
            text(d, (x, y + size * 0.82), p, size, fill, outline, anchor='ls')
        else:
            page.alpha_composite(p, (int(x), int(y + size * 0.55 - p.height / 2)))
        x += w + 6
    return total


def make(label, original, background, kind):
    """original: the rendered original page cropped to its content; background:
    its background shape (319 / 255) at the same size. Returns the new page."""
    m = MAP[kind]
    page = original.copy()
    bg = align(page, background)
    W, H = page.size
    d = ImageDraw.Draw(page)
    if label == 'Start':
        clear(page, bg, (0, 0, W, H))
        white = (255, 255, 255, 255)
        y = 12
        for line in ('Welcome to the Mushroom', 'Kingdom!  The First thing you need'):
            rich_line(page, 0, y, [line], 27, white, centre=W / 2); y += 34
        rich_line(page, 0, y, ['to know is how to move'] + [buttons(m['move'], 34)], 27, white, centre=W / 2); y += 40
        rich_line(page, 0, y, ['and how to read...'], 27, white, centre=W / 2)
        y = 238
        rich_line(page, 0, y, ['...signs by pressing', buttons(m['stomp'], 40), '.  Try'], 27, white, centre=W / 2)
        rich_line(page, 0, y + 44, ['reading that sign over there.'], 27, white, centre=W / 2)
        # the original's curved arrow from the end of the text back toward the first sign
        A = Image.new('RGBA', (W * SS, H * SS))
        a = ImageDraw.Draw(A)

        def bezier(p0, p1, p2, p3, n=40):
            return [tuple(SS * ((1 - t) ** 3 * p0[i] + 3 * (1 - t) ** 2 * t * p1[i] + 3 * (1 - t) * t * t * p2[i]
                                + t ** 3 * p3[i]) for i in (0, 1)) for t in (k / n for k in range(n + 1))]
        path = bezier((500, 297), (575, 296), (565, 228), (500, 214)) + bezier((500, 214), (450, 203), (390, 204), (346, 214))[1:]
        a.line(path, fill=white, width=4 * SS, joint='curve')
        a.polygon([(322 * SS, 219 * SS), (348 * SS, 198 * SS), (350 * SS, 234 * SS)], fill=white)
        page.alpha_composite(A.resize((W, H), Image.LANCZOS))
        page.alpha_composite(badge(kind, 40), (W - 92, H - 50))
    elif label == 'controls':
        clear(page, bg, (0, 66, W, H))
        white, black = (255, 255, 255, 255), (30, 30, 30, 255)
        size = 22
        rows = [('Move', m['move']), ('Jump', m['jump']), ('Spin', m['spin']), ('Stomp / Read', m['stomp']),
                ('F.L.U.D.D.', m['fludd']), ('Switch Nozzle', m['nozzle'])]
        for i, (name, styles) in enumerate(rows):
            col, row = i % 2, i // 2
            x, y = 18 + col * 216, 80 + row * 58
            b = buttons(styles, 42)
            page.alpha_composite(b, (x, y))
            text(d, (x + b.width + 10, y + 30), name, size, white, black, anchor='ls')
        page.alpha_composite(badge(kind, 38), (14, H - 46))
        text(d, (W - 14, H - 14), m['name'], 18, white, black, anchor='rs')
    elif label == 'bomb':
        clear(page, bg, (388, 34, 478, 112))
        page.alpha_composite(buttons(m['spin'], 58), (400, 42))
    elif label == 'hover':
        clear(page, bg, (374, 0, 446, 64))
        b = buttons(m['fludd'], 54)
        page.alpha_composite(b, (382 + (52 - b.width) // 2, 4))
    elif label == 'stomp':
        clear(page, bg, (392, 2, 474, 70))
        b = buttons(m['stomp'], 56)
        page.alpha_composite(b, (402 + (56 - b.width) // 2, 6))
        for x0 in (116, 430):  # the keycap drawn on each box picture: box colour, then the button
            fill = page.getpixel((x0 - 8, 200))
            ImageDraw.Draw(page).rectangle([x0, 151, x0 + 44, 190], fill=fill)
            s = buttons(m['stomp'], 30)
            page.alpha_composite(s, (x0 + 40 - s.width, 155))
    elif label == 'zoom':
        clear(page, bg, (60, 220, 228, 284))
        clear(page, bg, (300, 210, 440, 284))
        blue, white = (0, 51, 255, 255), (255, 255, 255, 255)
        if m['zoom_stick']:
            rich_line(page, 0, 226, [buttons(m['zoom_out'], 44), arrow_image('down', 30, blue), 'Out'], 30, blue,
                      white, centre=140)
            rich_line(page, 0, 220, [buttons(m['zoom_in'], 44), arrow_image('up', 30, blue), 'In'], 30, blue,
                      white, centre=372)
        else:
            rich_line(page, 0, 230, ['Press', buttons(m['zoom_in'], 44), 'to change the zoom'], 26, blue, white,
                      centre=W / 2)
    else:
        raise ValueError(label)
    return page
