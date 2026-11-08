#!/usr/bin/env python3
"""The original pause menu (sprite 795 holding 794), for the art pack
(export_course_art.py) and assets/generated/pause_layout.h.

795 sits at the stage centre (225, 150) in 800; pressing pause plays it: 794
rises 20 px to its place and fades in over frames 2-15 (14 frames); pressing
again plays it backwards. 794's pages are frame labels: default (PAUSED),
starmap, fludd, options, exit; 'refresh' is the empty panel shown while the
page-change clip (661, 13 frames: darkens to black on frame 7, the page
changes on frame 8) plays. Every page has four buttons (box + label sprites):
the page's own button is replaced by Back (666) in its place; a hovered button
eases to 120 % scale, a clicked one to 140 %.

Out of the pages (each a picture layer "Pause@<page>", 794 coordinates):
  * buttons, the Fludd page's nozzle icons, the Options items and the Exit
    page's items are left out and drawn by the port from "PauseParts";
  * the PAUSED page's text fields show what the tutorial shows: Starnamenum 0
    -> level "Castle Grounds", star "No Star Selected", desc StarDescArray[0];
  * its counters (_root.Coins, TotalStars, TotalStarCoins: Arial 16, white
    over 50 % black 2 px down-right) are drawn by the port; the red coin /
    silver star counters (232) show their empty frame 4 (the tutorial has
    neither) and the lives counter (237) its empty frame 2 (TotalStars 0);
  * the Star Map's record-coin fields are empty (no records); its shines are
    lit per group (737) and no group can be complete in the tutorial, so the
    page is still.
The port adds a sixth button between the four, "Achievements", in the
buttons' style (a box and a bevelled label), and a page for it on the empty
panel ('refresh') with a title in the page-title style.
"""
import re, struct
import numpy as np
from PIL import Image, ImageDraw, ImageFilter, ImageFont
import swf_states as S
import swf_bitmap_fix as F
import ui_art

ZOOM = ui_art.ZOOM
MENU, CLIP = 795, 794
PAGES = ['default', 'starmap', 'fludd', 'options', 'exit', 'refresh']   # 794 frames 2..7
BUTTONS = {'fludd': 633, 'options': 637, 'starmap': 641, 'exit': 647, 'back': 666}
NOZZLES = {'none': 757, 'hover': 749, 'rocket': 747, 'turbo': 744}       # frame 1 owned, 2 not
OPTION_ITEMS = {'quality': 769, 'mute': 773, 'speed': 784}
MUTE_ON, MUTE_OFF = 766, 762          # in 773: frames 2 normal, 3 hovered, 4 the current choice
EXIT_ITEMS = {'quit': 788, 'exitlevel': 793}
OPTIONS_BOX = 634                     # the Options button's box shape (637 = 634 + label 636)
COIN = 17                             # the spinning coin by _root.Coins (16 frames)
COUNTERS = {652, 653, 654, 655, 656, 657}
TRANSITION = 661
PANEL = 629                           # 794's panel: three gradient bands over the 450 x 300 stage
PANEL_SCALE = 0.25                    # smooth gradients: stored small, drawn stretched
RED_SILVER, LIVES = 232, 237
LEVEL, STAR, DESC = 648, 650, 651
TUTORIAL = {LEVEL: 'Castle Grounds', STAR: 'No Star Selected', DESC: 'You are within the Castle Grounds'}
RECORD_FIELDS_SPRITE = 737
ACH_COLOUR = ((70, 90, 255), (205, 215, 255))   # the port's button: blue, like the platinum cup


def edittext(data):
    """(variable end offset, initial text start, flags) of a DefineEditText body."""
    b = S.Bits(data, 2); nb = b.ub(5)
    for _ in range(4):
        b.sb(nb)
    b.align(); q = b.byte
    f1, f2 = data[q], data[q + 1]; q += 2
    if f1 & 0x01: q += 2
    if f2 & 0x80: q = data.index(b'\0', q) + 1
    if f1 & 0x01: q += 2
    if f1 & 0x04: q += 4
    if f1 & 0x02: q += 2
    if f2 & 0x20: q += 9
    q = data.index(b'\0', q) + 1
    return q, f1


def set_texts(swf, texts):
    """The SWF with text fields' initial text replaced (HTML fields keep their markup)."""
    raw, start = S.body(swf)
    out = bytearray(raw[:start])
    for code, pos, length in S.tags(raw, start, len(raw)):
        data = raw[pos:pos + length]
        cid = struct.unpack_from('<H', data)[0] if length >= 2 else -1
        if code == 37 and cid in texts:
            q, f1 = edittext(data)
            if not f1 & 0x80:
                raise ValueError(f'field {cid} has no initial text')
            old = data[q:data.index(b'\0', q)].decode('latin1')
            new = texts[cid]
            if '<' in old:   # replace the words, keep the tags
                new = re.sub(r'(<(?:b|font)[^>]*>)([^<]+)(<)', lambda m: m.group(1) + new + m.group(3), old, count=1)
                new = re.sub(r'(</p>).*', r'\1', new, flags=re.S)
            data = data[:q] + new.encode('latin1') + b'\0'
        out += F._tag(code, data)
    return b'FWS' + swf[3:4] + struct.pack('<I', len(out) + 8) + bytes(out)


def sprite_texts(swf, ident):
    """DefineEditText ids placed in sprite `ident`."""
    raw, start = S.body(swf)
    fields = {struct.unpack_from('<H', raw, p)[0] for c, p, l in S.tags(raw, start, len(raw) - 2) if c == 37}
    sp = S.sprites(swf)
    return {e.get('id') for f in sp[ident]['stage'] for e in f.values()} & fields


def render(jar, work, swf_path, ident, frames, tag):
    """{frame: (RGBA image, ox, oy)}: (ox, oy) = sprite px of the image's (0, 0)."""
    d = ui_art.ffdec(jar, work, swf_path, ident, frames, work / tag)
    out = {}
    for f in frames:
        sv = ui_art.ffdec(jar, work, swf_path, ident, [f], work / f'{tag}-svg{f}', svg=True)
        im = Image.open(d / f'{f}.png').convert('RGBA')
        ox, oy = ui_art.png_origin(sv / f'{f}.svg', im.size)
        bb = im.getchannel('A').getbbox()
        if bb is None:
            out[f] = (Image.new('RGBA', (4, 4)), ox, oy)
            continue
        out[f] = (im.crop(bb), ox + bb[0] / ZOOM, oy + bb[1] / ZOOM)
    return out


def bevel_label(text, height, colour, light):
    """A button label in the original labels' style: bold Arial, a vertical
    colour - light - colour gradient, a black outline and a soft dark shadow."""
    font = ImageFont.truetype('/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf', height)
    box = font.getbbox(text)
    pad = max(6, height // 6)
    w, h = box[2] - box[0] + 2 * pad, box[3] - box[1] + 2 * pad
    mask = Image.new('L', (w, h))
    ImageDraw.Draw(mask).text((pad - box[0], pad - box[1]), text, font=font, fill=255)
    m = np.asarray(mask).astype(float) / 255
    t = np.linspace(0, 1, h)[:, None]
    mix = np.clip(1 - np.abs(t - 0.55) * 2.2, 0, 1)   # light across the middle
    rgb = np.array(colour)[None, None] * (1 - mix[..., None]) + np.array(light)[None, None] * mix[..., None]
    grow = max(3, round(height / 9)) | 1
    outline = np.asarray(mask.filter(ImageFilter.MaxFilter(grow))).astype(float) / 255
    shadow = np.asarray(mask.filter(ImageFilter.MaxFilter(grow)).filter(ImageFilter.GaussianBlur(2))).astype(float) / 255
    shadow = np.roll(np.roll(shadow, 2, 0), 2, 1) * 0.6
    a = np.maximum(np.maximum(outline, shadow), m)
    edge = np.array(colour, float) * 0.25          # the outline: the colour, very dark
    col = rgb * m[..., None] + edge[None, None] * (1 - m[..., None])
    out = np.dstack([np.clip(col, 0, 255), a * 255]).astype(np.uint8)
    return Image.fromarray(out)


def ach_button(box):
    """The Achievements button: the Options box recoloured blue, a new label."""
    a = np.asarray(box).astype(float)
    g = a[..., :3].mean(2, keepdims=True) / 255
    lo, hi = np.array(ACH_COLOUR[0]), np.array(ACH_COLOUR[1])
    rgb = lo + (hi - lo) * np.clip((g - 0.25) / 0.75, 0, 1)
    im = Image.fromarray(np.dstack([rgb, a[..., 3:]]).astype(np.uint8))
    label = bevel_label('Achievements', round(im.height * 0.48), ACH_COLOUR[0], (255, 255, 255))
    if label.width > im.width - 6:
        k = (im.width - 6) / label.width
        label = label.resize((round(label.width * k), round(label.height * k)), Image.LANCZOS)
    im.alpha_composite(label, ((im.width - label.width) // 2, (im.height - label.height) // 2))
    return im


def pack(parts, width=1024):
    """Shelf-pack {name: image} -> (atlas, {name: (x, y, w, h)})."""
    x = y = row = 0
    place = {}
    for name in sorted(parts, key=lambda n: -parts[n].height):
        im = parts[name]
        if x + im.width + 2 > width:
            x, y, row = 0, y + row + 2, 0
        place[name] = (x + 1, y + 1, im.width, im.height)
        x += im.width + 2; row = max(row, im.height)
    height = -(-(y + row + 2) // 4) * 4
    atlas = Image.new('RGBA', (width, height))
    for name, (px, py, w, h) in place.items():
        atlas.paste(parts[name], (px, py))
    return atlas, place


def build(work, jar, swf):
    """(variants [(name, image, stage x, stage y)], pause_layout.h text)."""
    work.mkdir(parents=True, exist_ok=True)
    sp = S.sprites(swf)
    texts = dict(TUTORIAL)
    texts.update({c: '' for c in COUNTERS})
    texts.update({c: '' for c in sprite_texts(swf, RECORD_FIELDS_SPRITE)})
    patched = set_texts(swf, texts)
    hidden = set(BUTTONS.values()) | set(NOZZLES.values()) | set(OPTION_ITEMS.values()) | \
        set(EXIT_ITEMS.values()) | {TRANSITION, RED_SILVER, LIVES, COIN, PANEL}
    pages_swf = work / 'pause-pages.swf'
    pages_swf.write_bytes(ui_art.hide(patched, {CLIP: hidden}))
    pages = render(jar, work, pages_swf, CLIP, list(range(2, 8)), 'pages')
    base = 225, 150   # 795 at the stage centre
    variants = []
    for k, name in enumerate(PAGES):
        im, ox, oy = pages[k + 2]
        # only the stage shows: 795's pull-down cloud (above the panel on
        # 'default') sits wholly above the screen once 794 is in place
        L, T = round((-base[0] - ox) * ZOOM), round((-base[1] - oy) * ZOOM)
        l, t = max(0, L), max(0, T)
        r, btm = min(im.width, L + round(450 * ZOOM)), min(im.height, T + round(300 * ZOOM))
        im = im.crop((l, t, max(l + 4, r), max(t + 4, btm)))
        bb = im.getchannel('A').getbbox()      # the page's content alone (the panel is drawn by the port)
        if bb:
            im = im.crop(bb); l += bb[0]; t += bb[1]
        variants.append((f'Pause@{name}', im, base[0] + ox + l / ZOOM, base[1] + oy + t / ZOOM))
    # the parts
    parts_swf = work / 'pause-parts.swf'
    parts_swf.write_bytes(ui_art.hide(patched, {OPTION_ITEMS['mute']: {MUTE_ON, MUTE_OFF}}))
    parts, origin = {}, {}

    def add(name, ident, frame=1):
        r = render(jar, work, parts_swf, ident, [frame], f'part-{ident}-{frame}')[frame]
        parts[name], origin[name] = r[0], (r[1], r[2])
    for n, i in BUTTONS.items():
        add(f'btn_{n}', i)
    for n, i in NOZZLES.items():
        add(f'noz_{n}', i, 1)
        if n != 'none':
            add(f'noz_{n}_locked', i, 2)
    for n, i in OPTION_ITEMS.items():
        add(f'opt_{n}', i)
    for n, i in EXIT_ITEMS.items():
        add(f'exit_{n}', i)
    for f in (2, 3, 4):
        add(f'mute_on_{f}', MUTE_ON, f)
        add(f'mute_off_{f}', MUTE_OFF, f)
    coin = render(jar, work, parts_swf, COIN, list(range(1, 17)), 'part-coin')
    for f, (im, ox, oy) in coin.items():
        parts[f'coin_{f}'], origin[f'coin_{f}'] = im, (ox, oy)
    # the port's Achievements button and page title
    # the Options box shape (634) alone, without its label, on the button's canvas
    import subprocess
    with (work / 'export.log').open('a') as log:
        subprocess.run(['java', '-Xmx2g', '-Djava.awt.headless=true', f'-Duser.home={work.resolve()}/ffdec-home', '-jar',
                        str(jar.resolve()), '-zoom', str(ZOOM), '-selectid', str(OPTIONS_BOX), '-format', 'shape:png',
                        '-export', 'shape', str(work / 'box'), str(parts_swf)], check=True, stdout=log, stderr=log)
    shape = Image.open(work / 'box' / f'{OPTIONS_BOX}.png').convert('RGBA')
    opts = Image.new('RGBA', parts['btn_options'].size)
    opts.alpha_composite(shape, ((opts.width - shape.width) // 2, (opts.height - shape.height) // 2))
    parts['btn_achievements'] = ach_button(opts)
    origin['btn_achievements'] = origin['btn_options']
    title = bevel_label('Achievements', round(28 * ZOOM), ACH_COLOUR[0], (255, 255, 255))
    parts['title_achievements'] = title
    origin['title_achievements'] = (-title.width / 2 / ZOOM, -title.height / 2 / ZOOM)
    # the panel alone (its stage rectangle), small: the port stretches it over the 16:9 screen
    import subprocess as sp_
    with (work / 'export.log').open('a') as log:
        sp_.run(['java', '-Xmx2g', '-Djava.awt.headless=true', f'-Duser.home={work.resolve()}/ffdec-home', '-jar',
                 str(jar.resolve()), '-zoom', str(PANEL_SCALE), '-selectid', str(PANEL), '-format', 'shape:png',
                 '-export', 'shape', str(work / 'panel'), str(parts_swf)], check=True, stdout=log, stderr=log)
        sp_.run(['java', '-Xmx2g', '-Djava.awt.headless=true', f'-Duser.home={work.resolve()}/ffdec-home', '-jar',
                 str(jar.resolve()), '-selectid', str(PANEL), '-format', 'shape:svg', '-export', 'shape',
                 str(work / 'panel-svg'), str(parts_swf)], check=True, stdout=log, stderr=log)
    svg = (work / 'panel-svg' / f'{PANEL}.svg').read_text()
    tx, ty = (float(v) for v in re.search(r'matrix\(1\.0, 0\.0, 0\.0, 1\.0, ([-\d.]+), ([-\d.]+)\)', svg).groups())
    pim = Image.open(work / 'panel' / f'{PANEL}.png').convert('RGBA')
    l, t = round((tx - 225) * PANEL_SCALE), round((ty - 150) * PANEL_SCALE)
    parts['panel'] = pim.crop((l, t, l + round(450 * PANEL_SCALE), t + round(300 * PANEL_SCALE)))
    origin['panel'] = (-225.0, -150.0)
    atlas, place = pack(parts)
    variants.append(('PauseParts', atlas, 0, 0))
    # placements (794 coordinates)
    stage = sp[CLIP]['stage']
    names = {v: k for k, v in BUTTONS.items()}
    lines = ['/* Generated by tools/pause_art.py (via export_course_art.py); do not edit.',
             '   The original pause menu (795 / 794): pages, buttons and items. Coordinates are',
             '   794 px (795 at the stage centre); part rects are PauseParts atlas px; part',
             '   origins are the part image\'s corner from its registration point (794 px). */',
             '#ifndef SM63_PAUSE_LAYOUT_H', '#define SM63_PAUSE_LAYOUT_H',
             f'#define PAUSE_ZOOM {ZOOM}f',
             'typedef struct { const char *name; unsigned short x, y, w, h; float ox, oy; } PausePart;',
             'static const PausePart pause_parts[] = {']
    order = sorted(place)
    for n in order:
        x, y, w, h = place[n]
        lines.append(f'    {{"{n}", {x}, {y}, {w}, {h}, {origin[n][0]:.2f}f, {origin[n][1]:.2f}f}},')
    lines += ['};', f'#define PAUSE_PARTS {len(order)}',
              'enum { ' + ', '.join(f'PP_{n.upper()}' for n in order) + ' };',
              '/* per page (PAGES order): button kind at each slot (0 starmap / top-left, 1 options /',
              '   top-right, 2 fludd / bottom-left, 3 exit / bottom-right) and its place */',
              'typedef struct { signed char kind; float x, y; } PauseSlot;',
              f'static const PauseSlot pause_slots[{len(PAGES)}][4] = {{']
    slot_of = {641: 0, 637: 1, 633: 2, 647: 3}
    for k, name in enumerate(PAGES):
        f = stage[k + 1]
        slots = [None] * 4
        backs = []
        for d, e in f.items():
            i = e.get('id')
            m = e.get('matrix') or {'tx': 0, 'ty': 0}
            if i in slot_of:
                slots[slot_of[i]] = (f'PP_BTN_{names[i].upper()}', m['tx'] / 20, m['ty'] / 20)
            elif i == BUTTONS['back']:
                backs.append((m['tx'] / 20, m['ty'] / 20))
        for b in backs:   # Back takes the empty slot
            j = slots.index(None)
            slots[j] = ('PP_BTN_BACK', b[0], b[1])
        if name == 'refresh':
            slots = [(f'PP_BTN_{n.upper()}', x, y) for n, x, y in
                     [('starmap', -120, 76), ('options', 120, 76.2), ('fludd', -120, 122.2), ('exit', 120, 124)]]
        lines.append('    {' + ', '.join('{%s, %.1ff, %.1ff}' % s for s in slots) + '},  /* %s */' % name)
    lines.append('};')

    def at(frame, ident):
        for d, e in stage[frame - 1].items():
            if e.get('id') == ident:
                m = e.get('matrix') or {'tx': 0, 'ty': 0}
                return m['tx'] / 20, m['ty'] / 20
        raise ValueError(f'{ident} not on 794 frame {frame}')
    for n, i in NOZZLES.items():
        x, y = at(4, i)
        lines.append(f'#define PAUSE_NOZZLE_{n.upper()}_X {x:.1f}f\n#define PAUSE_NOZZLE_{n.upper()}_Y {y:.1f}f')
    for n, i in OPTION_ITEMS.items():
        x, y = at(5, i)
        lines.append(f'#define PAUSE_OPT_{n.upper()}_X {x:.1f}f\n#define PAUSE_OPT_{n.upper()}_Y {y:.1f}f')
    for n, i in EXIT_ITEMS.items():
        x, y = at(6, i)
        lines.append(f'#define PAUSE_EXIT_{n.upper()}_X {x:.1f}f\n#define PAUSE_EXIT_{n.upper()}_Y {y:.1f}f')
    mute = {e.get('id'): e['matrix'] for e in sp[OPTION_ITEMS['mute']]['stage'][0].values() if e.get('matrix')}
    for n, i in (('ON', MUTE_ON), ('OFF', MUTE_OFF)):
        lines.append(f'#define PAUSE_MUTE_{n}_X {mute[i]["tx"] / 20:.1f}f\n#define PAUSE_MUTE_{n}_Y {mute[i]["ty"] / 20:.1f}f')
    # counters: the white field's place (the black copy is 2 px right and down)
    for n, i in (('COINS', 654), ('SHINES', 655), ('STAR_COINS', 657)):
        x, y = at(2, i)
        lines.append(f'#define PAUSE_COUNT_{n}_X {x:.1f}f\n#define PAUSE_COUNT_{n}_Y {y:.1f}f')
    x, y = at(2, COIN)
    lines.append(f'#define PAUSE_COIN_X {x:.1f}f\n#define PAUSE_COIN_Y {y:.1f}f')
    lines += ['#endif', '']
    return variants, '\n'.join(lines)
