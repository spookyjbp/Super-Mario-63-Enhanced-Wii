#!/usr/bin/env python3
"""Render the original tutorial objects' animations into compressed art packs.

Each clip is a DefineSprite rendered by FFDec frame by frame (nested clips
advance with the timeline, as Flash plays them), offset by where the object
places it, with the invisible `Blank Movieclip` hit boxes (SWF 839 and 896)
removed. Clip ids are generated into `assets/generated/object_art_ids.h`.
Art is rendered at ZOOM (1.6 screen px per course px) and packed as art pack
v3 (GX RGB5A3, LZ4); clips are split over several 1024x1024 packs as needed.

    python export_object_art.py <checkout> <ffdec.jar> <out_dir>
"""
import argparse, hashlib, json, re, struct, subprocess
from pathlib import Path
from PIL import Image
import swf_states as S
import export_course as C
import export_course_art as A
from build_player_art import build, _pack
import numpy as np
import art_repair
import swf_bitmap_fix

ZOOM = 1.6
BLANKS = {839, 896}
# (clip name, sprite id, frames (None = all), step, placement (tx, ty, sx, sy) in
#  the object, depths to remove). Placement is the child's matrix in the object.
# Sources: SWF structure, see docs/course-objects.md.
SPEC = [
    ('COIN_SPIN', 17, None, 1, None, ()),             # GoldCoin 963 d4 'clip'
    ('COIN_COLLECT', 963, range(3, 17), 1, (0, 0, 1, 1), ()),  # frames 4..17
    ('BCOIN_SPIN', 3485, None, 1, None, ()),          # BlueCoin 3495 d4 'clip'
    ('BCOIN_COLLECT', 3495, range(3, 17), 1, (0, 0, 1, 1), ()),
    ('SIGN', 986, [0], 1, (0, 0, 1, 1), (3,)),                 # without 'aa' (d3)
    ('SIGN_Z', 981, None, 1, None, ()),                        # 'aa' prompt, placed from 986
    ('BUTTERFLY', 2323, None, 1, None, ()),           # ButterflyCode 'clip'
    ('GOONIE_FLY', 2300, None, 1, (0, 0, 1, 1), ()),          # GoonieCode frame 'fly'
    ('GOONIE_RIDE', 2301, None, 1, (0, 0, 1, 1), ()),         # frame 'ride'
    ('GOONIE_DIVE', 2304, None, 1, (0, 8, 1, 1), ()),         # frame 'dive'
    ('TOAD_IDLE', 2501, None, 4, None, (2,)),         # Toad 'body' (sampled 1/4), its 'head' (d2) drawn apart
    ('TOAD_WALK', 2516, None, 1, None, (2,)),         # Toad Main Clip frame 'walk' body, without 'head'
    # Toad 'head' (2498): mood segments Smile 1, Talk 69, Sad 99, Mad 160, Shocked 185,
    # Happy 270, Confused 299, Crazy 360 (frames 1-397); placed per body frame at runtime.
    ('TOAD_HEAD', 2498, list(range(0, 397)), 1, (0, 0, 1, 1), ()),
    ('TOAD_BZ', 2505, None, 3, None, ()),                      # Toad 'bz' talk prompt
    ('LOG', 3317, [0], 1, (0, 0, 1, 1), ()),                  # Falling Log (graphics)
    ('ARROW', 3410, None, 1, (0, 0, 1, 1), ()),               # YI Arrow Animation
    ('GOOMBA_IDLE', 2380, [0], 1, None, ()),                   # Goomba 'clip' frame Idle
    ('GOOMBA_JUMP', 2344, None, 1, None, ()),
    ('GOOMBA_WALKR', 2360, None, 1, None, ()),
    ('GOOMBA_WALKL', 2369, None, 1, None, ()),
    ('GOOMBA_SQUISH', 2377, None, 1, None, ()),
    ('GOOMBA_TUMBLE', 2379, None, 1, None, ()),
    ('CHEEP_SWIM', 2216, None, 1, None, ()),
    ('CHEEP_CHASE', 2217, None, 1, None, ()),
    ('CHEEP_TURN', 2220, None, 1, None, ()),
    ('CHEEP_JUMP', 2229, None, 1, None, ()),
    ('CHEEP_SPIN', 2230, None, 1, None, ()),
    ('CHEEP_GONE', 2232, None, 1, None, ()),
    ('BOMB_WALK', 3280, None, 1, None, ()),
    ('BOMB_TUMBLE', 3282, None, 1, None, ()),
    ('BOMB_EXPLODE', 3294, None, 1, None, ()),
    ('BOMB_EXPLODE2', 3309, None, 1, None, ()),
    ('KOOPA_WALK', 3367, None, 1, None, ()),
    ('KOOPA_TURN', 3377, None, 1, None, ()),
    ('KOOPA_FLY', 3396, None, 1, None, ()),            # Koopa clip frame 'Flying' (Paratroopa)
    ('KOOPA_SHELL', 3401, None, 1, None, ()),
    ('BOX', 2808, None, 1, (0, 0, 1, 1), (1,)),               # Breakable box; 'stuff' (d1) is "none" at blocknum 1
    ('FLUDD_BOX', 972, None, 1, (0, 0, 1, 1), ()),            # Hover FLUDD box
    ('BOTTLE', 3995, None, 1, (0, 0, 1, 1), ()),
    ('GREEN_PL', 951, None, 1, (0, 0, 1, 1), ()),             # Green Circle Platform 'pl' (frame = scale)
    ('PATH_DOT', 28, None, 1, (0, 0, 1, 1), ()),              # attachMovie("Path dot") around the circle
    ('HNOZZLE', 42, None, 1, (0, 0, 1, 1), ()),               # Hover nozzle pickup from the FLUDD box
    ('TIP_LOG', 3476, None, 1, (0, 0, 1, 1), ()),
    ('ROT_BLOCK', 3063, [0, 1], 1, (0, 0, 1, 1), ()),        # frame = blocktype (tutorial uses 1, 2)
    # Stats Clip (800) frame 1 without its dynamic parts: the shine and star coin
    # icons with their "=" (the coin counter's "=" is HUD_DIGITS frame 10, so it
    # can sit at the right edge on 16:9).
    ('HUD_STATIC', 800, [0], 1, (0, 0, 1, 1),
     (1, 3, 9, 45, 65, 70, 71, 72, 74, 75, 76, 78, 82, 84, 86, 98, 104, 110, 112, 113, 118, 120, 122)),
    ('HUD_PIE', 111, None, 1, (0, 0, 1, 1), ()),               # POWER meter, frame = CharHP + 1
    ('HUD_TANK', 160, [0], 1, (0, 0, 1, 1), (6, 32, 35)),      # FLUDD tank without level, gauge, number
    ('HUD_TANK_LEVEL', 149, None, 10, (1.1, 30.5, 0.5, 0.5), ()),  # 'cl', frame = Waterpercent * 5
    ('HUD_POWER_BAR', 153, None, 5, (-17.35, -14.85, 1, 1), ()),   # frame = Power / TotalPower * 100 + 1
    # The 0 HP faint (not in the player art pack, which is full): Mario Main's
    # hurt1 (1411) from 'falling' (frame 10) to its end, and hurt3 (1430) from
    # 'end' (24); placed as in Mario Main (plus Char's -3 px). Faint/'Faint'
    # (1703 'nes') hides Mario while MarioDeathGFX (1702) plays at his place.
    # One clip per FLUDD state (none, Hover, Rocket, Turbo: the port's Nozzle
    # order), the nested FLUDD sprites set as their Fluddpow scripts would.
    *[(f'MARIO_FAINT1_{k}', 1411, list(range(9, 107)), 1, (2.0, -17.0, 1, 1), ()) for k in range(4)],
    *[(f'MARIO_FAINT3_{k}', 1430, list(range(23, 70)), 1, (0.35, -2.65, 1, 1), ()) for k in range(4)],
    ('MARIO_DEATH', 1702, None, 1, (0, 0, 1, 1), ()),
    # Mario Main 'EJump' (Enemy Jump Animation 1445, held on a stomped Goomba) and
    # 'Drown' (1692, to its frame 88 LifeLost()), per FLUDD state like the faint;
    # Mario (None) Walk's 'Idle' (Idle Animation 1050, after 92 still frames) and
    # 'Idle2' (Tired Animation 1067, at CharHP <= 3), and Main 'Sleep' (1450, which
    # both end in), per FLUDD state: the original has them only without FLUDD; the
    # port adds FLUDD to them (tools/idle_fludd.py). Placed as in Mario Main
    # (Walk 1068 at (0.35, -12.65)) plus Char's -3 px.
    *[(f'MARIO_EJUMP_{k}', 1445, None, 1, (1.35, -14.45, 1, 1), ()) for k in range(4)],
    *[(f'MARIO_DROWN_{k}', 1692, list(range(0, 88)), 1, (0.35, -3.0, 1, 1), ()) for k in range(4)],
    *[(f'MARIO_IDLE_{k}', 1050, None, 1, (2.35, -17.7, 1, 1), ()) for k in range(4)],
    *[(f'MARIO_TIRED_{k}', 1067, None, 1, (2.35, -17.8, 1, 1), ()) for k in range(4)],
    *[(f'MARIO_SLEEP_{k}', 1450, None, 1, (0.0, -14.2, 1, 1), ()) for k in range(4)],
    # Drowning: the Stats clip's water meter (113 in 114, frame = WaterHP + 1) and
    # the WaterBubble (2589) CharCode attaches in FrontGFX; the splash on falling
    # into water (Med-Splash 2609, removed on its frame 10) and the Turbo's
    # water-running splashes (Run-Splash1 2579 / 2 2587).
    ('HUD_WATER_PIE', 113, None, 1, (0, 0, 1, 1), ()),
    ('WATER_BUBBLE', 2589, list(range(0, 39)), 1, (0, 0, 1, 1), ()),
    ('MED_SPLASH', 2609, list(range(0, 9)), 1, (0, 0, 1, 1), ()),
    ('RUN_SPLASH1', 2579, list(range(0, 17)), 1, (0, 0, 1, 1), ()),
    ('RUN_SPLASH2', 2587, list(range(0, 7)), 1, (0, 0, 1, 1), ()),
    # the Castle Grounds' 1 Up Code (2209): the mushroom (1-Up Animation 2207 at
    # (-0.45, 0)) and "collect" (frames 4-20 without the 'box')
    ('ONEUP_IDLE', 2207, None, 1, (-0.45, 0, 1, 1), ()),
    ('ONEUP_COLLECT', 2209, list(range(3, 20)), 1, (0, 0, 1, 1), (5,)),
    ('HUD_TALK_BOX', 186, [0], 1, (0, 0, 1, 1), (2, 4)),       # 'tm' Toad message cloud (text drawn by the port)
    ('HUD_TALK_Z', 184, None, 1, (143.45, 1, 1.5, 1.5), ()),   # its 'bz' Z prompt
    # Shine Sprite 977/976: only on 8-E1-2-2 frames reached once Star[39] is set,
    # so a new save never shows it in the tutorial; not exported.
]


# Clips rendered once per FLUDD state (their Fluddpow sprites set per variant).
VARIANT_SPRITES = [1411, 1430, 1445, 1692, 1050, 1067, 1450]
VARIANT_NAME = re.compile(r'MARIO_(?:FAINT\d|EJUMP|DROWN|IDLE|TIRED|SLEEP)_(\d)$')
# FFDec sizes a sprite's canvas from its content, and once the FLUDD sprites are
# frozen per state that canvas changed from state to state (and the Hover nozzle
# fell outside it, cut off). Each variant sprite gets an invisible box (course
# px, x0 y0 x1 y1) its canvas always covers, and each state its own origin.
VARIANT_BOX = (-64, -80, 64, 48)
BOX_SHAPE = 64999


def boxed(swf, idents, box=VARIANT_BOX):
    """The SWF with an invisible rectangle (DefineShape3, alpha 0) placed under
    everything in sprites `idents`."""
    from idle_fludd import _Bits, _tag
    x0, y0, x1, y1 = (round(v * 20) for v in box)
    w = _Bits()
    n = max(abs(v) for v in (x0, y0, x1, y1)).bit_length() + 1
    w.put(n, 5)
    for v in (x0, x1, y0, y1):
        w.put(v & (1 << n) - 1, n)
    rect = w.data()
    w = _Bits()
    w.put(1, 4); w.put(0, 4)                                   # NumFillBits 1, NumLineBits 0
    w.put(0b000101, 6); w.put(n, 5); w.put(x0 & (1 << n) - 1, n); w.put(y0 & (1 << n) - 1, n); w.put(1, 1)
    for dx, dy in ((x1 - x0, 0), (0, y1 - y0), (x0 - x1, 0), (0, y0 - y1)):
        v = dx or dy
        m = max(abs(v).bit_length() + 1, 2)
        w.put(1, 1); w.put(1, 1); w.put(m - 2, 4); w.put(0, 1); w.put(1 if dy else 0, 1); w.put(v & (1 << m) - 1, m)
    w.put(0, 6)
    shape = struct.pack('<H', BOX_SHAPE) + rect + bytes([1, 0x00, 0, 0, 0, 0, 0]) + w.data()
    raw, start = S.body(swf)
    out, done = bytearray(raw[:start]), False
    for code, pos, length in S.tags(raw, start, len(raw)):
        data = raw[pos:pos + length]
        if code == 39 and struct.unpack_from('<H', data)[0] in idents:
            if not done:
                out += _tag(32, shape); done = True
            ident, count = struct.unpack_from('<HH', data)
            place = _tag(26, struct.pack('<BHH', 0x02, 16000, BOX_SHAPE))   # invisible: any free depth
            data = struct.pack('<HH', ident, count) + place + data[4:]
        out += _tag(code, data)
    if not done:
        raise ValueError('variant sprites not found')
    return b'FWS' + swf[3:4] + struct.pack('<I', len(out) + 8) + bytes(out)


def svg_origin(svg_dir, ident, png_dir):
    """Course px of the canvas (0,0) in the sprite (A.origin), moved by half
    the padding FFDec adds around filters to the PNG canvas."""
    import xml.etree.ElementTree as ET
    path = C.sprite_dir(svg_dir, ident) / '1.svg'
    g = ET.parse(path).getroot().find('{http://www.w3.org/2000/svg}g')
    t = [float(v) for v in re.findall(r'[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?', g.get('transform'))]
    if t[:4] != [1, 0, 0, 1]:
        raise ValueError('Unexpected registration')
    ox, oy = -t[4], -t[5]
    text = path.read_text()[:2000]
    sw = float(re.search(r'width="([\d.]+)px"', text).group(1))
    sh = float(re.search(r'height="([\d.]+)px"', text).group(1))
    with Image.open(C.sprite_dir(png_dir, ident) / '1.png') as im0:
        mx, my = (im0.width / ZOOM - sw) / 2, (im0.height / ZOOM - sh) / 2
    if mx > 1 or my > 1:
        ox, oy = ox - mx, oy - my
    return ox, oy


def child_matrix(sprites, parent, child):
    for f in sprites[parent]['stage']:
        for d, e in f.items():
            if e.get('id') == child and e.get('matrix'):
                m = e['matrix']
                return (m['tx'] / 20, m['ty'] / 20, m['sx'], m['sy'])
    raise ValueError(f'{child} not placed in {parent}')


PARENTS = {2516: 2517, 17: 963, 3485: 3495, 2323: 2324, 2501: 2517, 981: 986, 2505: 2517, 2380: 2386, 2344: 2380, 2360: 2380, 2369: 2380, 2377: 2380, 2379: 2380,
           2385: 2386, 2216: 2233, 2217: 2233, 2220: 2233, 2229: 2233, 2230: 2233, 2232: 2233,
           3280: 3310, 3282: 3310, 3294: 3310, 3309: 3310, 3367: 3402, 3377: 3402, 3396: 3402, 3401: 3402, 976: 977}
# Chains whose own placement in the object must also be applied.
GRAND = {2380: 2386, 2233: 2234, 3310: 3311, 3402: 3403}


def placement(sprites, ident):
    """Accumulated (tx, ty, sx, sy) from the object origin to sprite `ident`."""
    tx, ty, sx, sy = 0.0, 0.0, 1.0, 1.0
    chain = []
    while ident in PARENTS:
        chain.append((PARENTS[ident], ident)); ident = PARENTS[ident]
    while ident in GRAND:
        chain.append((GRAND[ident], ident)); ident = GRAND[ident]
    for parent, child in reversed(chain):
        ctx, cty, csx, csy = child_matrix(sprites, parent, child)
        tx, ty = tx + ctx * sx, ty + cty * sy
        sx, sy = sx * csx, sy * csy
    return tx, ty, sx, sy


# Characters removed wherever they are placed in a sprite (by id, not depth):
# Toad's head moves from depth 2 to depth 1 on walk frames 16-20, so a depth
# rule left a smiling head baked into those body frames (the "normal face for
# a frame" flicker on the walking, angry Toad).
# Characters removed from a sprite: Toad's head from his bodies; Wing Code (1003, the
# Wing Cap wings, frame 2 = none without the cap) from the Idle Animation.
STRIP_CHARS = {2516: {2498}, 2501: {2498}, 1050: {1003}}


def strip_blanks(raw, start, strip_depths, strip_chars=STRIP_CHARS):
    """Remove Blank Movieclip placements everywhere, per-sprite depths and per-sprite characters."""
    out = bytearray(raw[:start])
    for code, pos, length in S.tags(raw, start, len(raw)):
        data = raw[pos:pos + length]
        if code == 39:
            ident, frames = struct.unpack_from('<HH', data)
            body, blank = bytearray(), set()
            drop = set(strip_depths.get(ident, ()))
            chars = strip_chars.get(ident, set())
            for c, p, n in S.tags(raw, pos + 4, pos + length):
                d = raw[p:p + n]
                if c in (26, 70):
                    flags = d[0]
                    depth = struct.unpack_from('<H', d, 2 if c == 70 else 1)[0]
                    if flags & 0x02:
                        cid = struct.unpack_from('<H', d, 3 if c == 26 else 4)[0]
                        if cid in BLANKS or cid in chars: blank.add(depth)
                        else: blank.discard(depth)
                    if depth in blank or depth in drop:
                        continue
                elif c == 28 and struct.unpack_from('<H', d)[0] in blank | drop:
                    continue
                body += A.tag(c, d)
            data = struct.pack('<HH', ident, frames) + bytes(body)
        out += A.tag(code, data)
    return out


def digits(jar, swf_path, work, cid):
    """HUD counters: DefineEditText fields in font 187 (Arial, 16 px), a black
    50% copy 2 px down-right under the white one. Frames 0-9 are the digits,
    drawn from the text field's top-left (2 px gutter); frame 10 is "="."""
    from PIL import ImageDraw, ImageFont
    font_dir = work / 'font'
    if not list(font_dir.glob('187_*.ttf')):
        subprocess.run(['java', '-Djava.awt.headless=true', '-jar', str(jar.resolve()), '-selectid', '187', '-format',
                        'font:ttf', '-export', 'font', str(font_dir), str(swf_path)], check=True, capture_output=True)
    font = ImageFont.truetype(str(next(font_dir.glob('187_*.ttf'))), round(16 * ZOOM))
    out_png = work / 'digits'
    out_png.mkdir(exist_ok=True)
    frames = []
    for d in '0123456789=':
        w = round(14 * ZOOM); h = round(24 * ZOOM)
        im = Image.new('RGBA', (w, h))
        dr = ImageDraw.Draw(im)
        # FFDec draws these fields bold with a dark edge; a stroke reproduces that look.
        dr.text((round(2 * ZOOM) + 2, round(2 * ZOOM) + 2), d, font=font, fill=(0, 0, 0, 128), stroke_width=2,
                stroke_fill=(0, 0, 0, 128))
        dr.text((2, 2), d, font=font, fill=(255, 255, 255, 255), stroke_width=2, stroke_fill=(0, 0, 0, 255))
        path = out_png / f'{d}.png'
        im.save(path)
        frames.append({'file': str(path.resolve()), 'duration': 1, 'offset_twips': [round(2 * 20), round(2 * 20)]})
    return {'id': cid, 'name': 'HUD_DIGITS', 'loop': False, 'frames': frames}


# The talk / read prompts over signs (981: keycap 979 / 980 on two legs, 978) and
# Toad ('bz' 2505: keycap 2504 holding 2503, on a tail 2502): the keyboard "Z"
# is replaced by the controller's read button (sign_buttons 'stomp': GameCube
# Z, Classic "L or R", Wii Remote "A + B"), turned like the key in each pose.
PROMPT_KEYS = {'SIGN_Z': (981, {981: {979, 980}}), 'TOAD_BZ': (2505, {2504: {2503}})}


def key_angle(mask):
    """The keycap's tilt (degrees, PIL's sense): the rotation that makes its box smallest."""
    im = Image.fromarray((mask * 255).astype(np.uint8))
    best = None
    for a10 in range(-200, 201, 5):
        bb = im.rotate(-a10 / 10, Image.NEAREST, expand=True).getbbox()
        area = (bb[2] - bb[0]) * (bb[3] - bb[1])
        if best is None or area < best[0]:
            best = (area, a10 / 10, bb)
    return best[1], min(best[2][2] - best[2][0], best[2][3] - best[2][1])


def unscaled(frame):
    """The frame's picture before export() resampled it for a scaled placement."""
    p = Path(frame['file'])
    return p.parent.parent / 'repaired' / p.name if p.parent.name == 'scaled' else p


def prompt_variants(work, name, out_frames, keyless_dir, pad=56):
    """Frames [(file, offset)] per controller: the frame without its key, the
    button in the key's place (its centre, tilt and size)."""
    import sign_buttons
    variants = []
    # the keyless canvas is smaller (FFDec fits it to the clip's bounds): find
    # where it sits in the full one, where everything left over lines up
    o0 = np.asarray(Image.open(unscaled(out_frames[0])).convert('RGBA'))[..., 3].astype(int)
    first = Path(out_frames[0]['file']).stem.rsplit('_', 1)[1]
    b0 = np.asarray(Image.open(keyless_dir / f'{int(first) + 1}.png').convert('RGBA'))[..., 3].astype(int)
    if b0.shape[0] > o0.shape[0] or b0.shape[1] > o0.shape[1]:
        raise ValueError(f'{name}: keyless frame larger than the clip')
    best = None
    for dy in range(o0.shape[0] - b0.shape[0] + 1):
        for dx in range(o0.shape[1] - b0.shape[1] + 1):
            e = np.abs(o0[dy:dy + b0.shape[0], dx:dx + b0.shape[1]] - b0).sum()
            if best is None or e < best[0]:
                best = (e, dx, dy)
    _, sdx, sdy = best
    for k, kind in enumerate(sign_buttons.CONTROLLERS):
        frames = []
        for n, f in enumerate(out_frames):
            orig = Image.open(unscaled(f)).convert('RGBA')
            ratio = Image.open(f['file']).width / orig.width
            src = Path(f['file']).stem.rsplit('_', 1)[1]
            bare = Image.open(keyless_dir / f'{int(src) + 1}.png').convert('RGBA')
            full = Image.new('RGBA', orig.size)
            full.alpha_composite(bare, (sdx, sdy))
            bare = full
            c = Image.new('RGBA', (orig.width + 2 * pad, orig.height + 2 * pad))
            c.alpha_composite(bare, (pad, pad))
            a, b = np.asarray(orig)[..., 3].astype(int), np.asarray(bare)[..., 3].astype(int)
            key = (a - b) > 60
            if key.sum() > 30:
                ys, xs = np.nonzero(key)
                cx, cy = (xs.min() + xs.max() + 1) / 2, (ys.min() + ys.max() + 1) / 2
                angle, side = key_angle(key)
                btn = sign_buttons.STYLE[kind]['stomp'](max(8, int(side * 0.92)))
                if btn.width > side * 2.4:
                    btn = btn.resize((int(side * 2.4), max(1, int(btn.height * side * 2.4 / btn.width))), Image.LANCZOS)
                btn = btn.rotate(angle, Image.BICUBIC, expand=True)
                c.alpha_composite(btn, (int(round(pad + cx - btn.width / 2)), int(round(pad + cy - btn.height / 2))))
            if abs(ratio - 1) > 0.01:   # the clip is placed scaled (export(): resample once)
                c = c.resize((max(1, round(c.width * ratio)), max(1, round(c.height * ratio))), Image.LANCZOS)
            path = work / 'prompts' / f'{name}_{k}_{n}.png'
            path.parent.mkdir(exist_ok=True)
            c.save(path)
            frames.append(dict(f, file=str(path.resolve()),
                               offset_twips=[f['offset_twips'][0] - round(pad * ratio / ZOOM * 20),
                                             f['offset_twips'][1] - round(pad * ratio / ZOOM * 20)]))
        variants.append(frames)
    return variants


def export(source, jar, out_dir):
    swf_file = source / 'Publish/Super_Mario_63_2010.swf'
    swf = swf_file.read_bytes()
    if hashlib.sha256(swf).hexdigest() != C.SOURCE_SHA:
        raise ValueError('Unsupported SWF revision')
    raw, start = S.body(swf)
    sprites = S.sprites(swf)
    work = Path(__file__).resolve().parent.parent / 'private-art' / 'rendered-objects'
    work.mkdir(parents=True, exist_ok=True)
    strip = {}
    for name, ident, frames, step, place, depths in SPEC:
        if depths: strip.setdefault(ident, set()).update(depths)
    import idle_fludd
    injected = idle_fludd.inject(raw, start, source)   # FLUDD on the idle clips
    patched = swf_bitmap_fix.fix_body(strip_blanks(injected, start, strip), start)  # FFDec half-texel offset
    swf_path = work / 'objects.swf'
    swf_path.write_bytes(b'FWS' + swf[3:4] + struct.pack('<I', len(patched) + 8) + bytes(patched))
    ids = sorted({s[1] for s in SPEC})
    cmd = ['java', '-Xmx3g', '-Djava.awt.headless=true', f'-Duser.home={work.resolve()}/ffdec-home', '-jar',
           str(jar.resolve())]
    with (work / 'export.log').open('a') as log:  # every frame (PNG), first frame registration (SVG)
        subprocess.run(cmd + swf_bitmap_fix.FFDEC_RENDER + ['-zoom', str(ZOOM), '-onerror', 'abort', '-selectid',
                              ','.join(map(str, ids)), '-export', 'sprite', str(work / 'png'), str(swf_path)],
                       check=True, stdout=log, stderr=log)
        subprocess.run(cmd + ['-onerror', 'abort', '-selectid', ','.join(map(str, ids)), '-select',
                              ','.join(f'{i}:1' for i in ids), '-format', 'sprite:svg', '-export', 'sprite',
                              str(work / 'svg'), str(swf_path)], check=True, stdout=log, stderr=log)
    # The Mario clips per FLUDD state (export_movement_clips' Fluddpow choices),
    # each in its box, with its own registration.
    import export_movement_clips as M
    vbase = boxed(swf_path.read_bytes(), set(VARIANT_SPRITES))
    vraw, vstart = S.body(vbase)
    choice = M.fludd_frames(S.sprites(vbase), vraw, vstart, VARIANT_SPRITES)
    sel = ','.join(map(str, VARIANT_SPRITES))
    for k, v in enumerate(M.VARIANTS):
        vswf = work / f'objects-fludd{k}.swf'
        vswf.write_bytes(M.patched_swf(vbase, choice, v))
        with (work / 'export.log').open('a') as log:
            subprocess.run(cmd + swf_bitmap_fix.FFDEC_RENDER + ['-zoom', str(ZOOM), '-onerror', 'abort', '-selectid',
                                  sel, '-export', 'sprite', str(work / f'png-fludd{k}'), str(vswf)],
                           check=True, stdout=log, stderr=log)
            subprocess.run(cmd + ['-onerror', 'abort', '-selectid', sel, '-select', ','.join(f'{i}:1' for i in VARIANT_SPRITES),
                                  '-format', 'sprite:svg', '-export', 'sprite', str(work / f'svg-fludd{k}'), str(vswf)],
                           check=True, stdout=log, stderr=log)
    clips, report = [], []
    for cid, (name, ident, frames, step, place, depths) in enumerate(SPEC):
        tx, ty, sx, sy = place if place else placement(sprites, ident)
        if sx <= 0 or sy <= 0:
            raise ValueError(f'{name}: mirrored placement {sx},{sy}')
        # course px of the canvas (0,0) in the sprite; FFDec pads the PNG canvas
        # around filters (Toad's head, the Stats clip): the SVG is the unpadded
        # frame, so the origin moves by half the difference
        fl = VARIANT_NAME.match(name)
        png_dir = work / (f'png-fludd{fl.group(1)}' if fl else 'png')
        ox, oy = svg_origin(work / (f'svg-fludd{fl.group(1)}' if fl else 'svg'), ident, png_dir)
        count = sprites[ident]['frames']
        picks = list(frames) if frames is not None else list(range(count))
        picks = picks[::step]
        out, repaired_px = [], 0
        for k in picks:
            png = C.sprite_dir(png_dir, ident) / f'{k + 1}.png'
            # FFDec cracks (e.g. the transparent column inside the sign board): art_repair.py
            fixed, changed = art_repair.repair(np.asarray(Image.open(png).convert('RGBA')))
            repaired_px += changed
            png = work / 'repaired' / f'{name}_{k}.png'
            png.parent.mkdir(exist_ok=True)
            Image.fromarray(fixed).save(png)
            if abs(sx - 1) > 0.01 or abs(sy - 1) > 0.01:  # child placed scaled: resample once
                im = Image.open(png).convert('RGBA')
                scaled = work / 'scaled' / f'{name}_{k}.png'
                scaled.parent.mkdir(exist_ok=True)
                im.resize((max(1, round(im.width * sx)), max(1, round(im.height * sy))), Image.LANCZOS).save(scaled)
                png = scaled
            out.append({'file': str(png.resolve()), 'duration': step,
                        'offset_twips': [round((ox * sx + tx) * 20), round((oy * sy + ty) * 20)]})
        clips.append({'id': cid, 'name': name, 'loop': frames is None, 'frames': out})
        report.append({'name': name, 'sprite': ident, 'frames': len(out), 'placement': [tx, ty],
                       'repaired_px': repaired_px})
    # the prompts per controller (PROMPT_KEYS): the clips rendered without their keys
    import ui_art
    for name, (ident, hide) in PROMPT_KEYS.items():
        kswf = work / f'objects-nokey-{name}.swf'
        kswf.write_bytes(ui_art.hide(swf_path.read_bytes(), hide))
        with (work / 'export.log').open('a') as log:
            subprocess.run(cmd + swf_bitmap_fix.FFDEC_RENDER + ['-zoom', str(ZOOM), '-onerror', 'abort', '-selectid',
                                  str(ident), '-export', 'sprite', str(work / f'png-nokey-{name}'), str(kswf)],
                           check=True, stdout=log, stderr=log)
        base = next(c for c in clips if c['name'] == name)
        for k, frames in enumerate(prompt_variants(work, name, base['frames'],
                                                   C.sprite_dir(work / f'png-nokey-{name}', ident))):
            clips.append({'id': len(clips), 'name': f'{name}_{k}', 'loop': base['loop'], 'frames': frames})
            report.append({'name': f'{name}_{k}', 'sprite': ident, 'frames': len(frames)})
    clips.append(digits(jar, swf_path, work, len(clips)))
    report.append({'name': 'HUD_DIGITS', 'font': 187})
    # Split into packs that each fit one 1024x1024 texture.
    packs, current = [], []
    for clip in clips:
        trial = current + [clip]
        try:
            build({'clips': [dict(c, frames=[dict(f, offset_twips=[f['offset_twips'][0] * 0, 0]) for f in c['frames']])
                             for c in trial]}, Path('/'), compressed=False, px_twips=20 / ZOOM)
            current = trial
        except ValueError:
            packs.append(current); current = [clip]
    packs.append(current)
    header = ['/* Generated by tools/export_object_art.py; do not edit. */', '#ifndef SM63_OBJECT_ART_IDS_H',
              '#define SM63_OBJECT_ART_IDS_H', f'#define OBJECT_ART_ZOOM {ZOOM}f',
              f'#define OBJECT_ART_PACKS {len(packs)}', 'enum {']
    for p, pack in enumerate(packs):
        blob = build({'clips': pack}, Path('/'), compressed=True, px_twips=20 / ZOOM)
        (out_dir / f'objects_8e1_{p}.bin').write_bytes(blob)
        for c in pack:
            header.append(f'    OBJ_{c["name"]} = {c["id"]}, /* pack {p} */')
    header += ['    OBJ_CLIP_COUNT = %d' % len(clips), '};',
               '/* Toad head (2498) registration per TOAD_IDLE / TOAD_WALK frame, course px from',
               '   the Toad Main Clip origin (the body placement plus the head matrix in it). */']
    for name, ident, step in (('idle', 2501, 4), ('walk', 2516, 1)):
        btx, bty, _, _ = placement(sprites, ident)
        last, rows = {}, []
        for n, f in enumerate(sprites[ident]['stage']):
            for d, e in f.items():
                if e.get('matrix') or d not in last:
                    last[d] = e
            if n % step:
                continue
            m = next(e for e in last.values() if e.get('name') == 'head')['matrix']
            rows.append(f'{{{btx + m["tx"] / 20:.2f}f, {bty + m["ty"] / 20:.2f}f}}')
        header.append(f'static const float toad_head_{name}[][2] = {{' + ', '.join(rows) + '};')
    header += [ 'static const unsigned char object_art_pack_of[] = {',
               '    ' + ', '.join(str(next(p for p, pk in enumerate(packs) if any(c['id'] == i for c in pk)))
                                  for i in range(len(clips))), '};', '#endif', '']
    (out_dir / 'object_art_ids.h').write_text('\n'.join(header))
    (work / 'objects.json').write_text(json.dumps(report, indent=1))
    return packs, report


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('source', type=Path); p.add_argument('ffdec_jar', type=Path); p.add_argument('out_dir', type=Path)
    a = p.parse_args()
    packs, report = export(a.source, a.ffdec_jar, a.out_dir)
    for i, pk in enumerate(packs):
        print(f'pack {i}: {len(pk)} clips, {sum(len(c["frames"]) for c in pk)} frames')
