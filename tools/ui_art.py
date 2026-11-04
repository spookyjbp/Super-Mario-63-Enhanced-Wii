#!/usr/bin/env python3
"""The original message box (Stats.Message, sprite 253) and its "Press Z to
Continue" prompt (sprite 250), for the port's text signs.

* Message box: frame 6 of 253 (fully open) without its text field (251, drawn
  by the port in font 155) and prompt (250), as one picture layer "Message".
  Frames 2-6 open it and 7-11 close it (a morph of the same box): their
  rectangles go to ui_layout.h and the port scales the box into them.
* Prompt: 250 fades in after 38 frames and pulses (frames 80-135 loop). Each
  controller gets its own version with the keyboard "Z" replaced by its
  read/stomp button (prompt_art.py), "Prompt@<n>". The per-frame brightness of
  the original goes to ui_layout.h.
* Prompt positions: in the message box and on every picture page (each page of
  619 places 250 itself), stage px.
"""
import re, struct, subprocess
import xml.etree.ElementTree as ET
from pathlib import Path
import numpy as np
from PIL import Image
import swf_states as S
import swf_bitmap_fix as F
import sign_buttons
import prompt_art

MESSAGE, MESSAGE_TEXT, PROMPT, PAGES = 253, 251, 250, 619
TALK_Z, TALK_Z_AT, TALK_Z_SCALE = 184, (143.45, 1.0), 1.5   # Stats.tm's 'bz' (in 186): a "Z" key that wobbles
TALK_Z_TILT = (9.0, -7.0)    # 184: 182 on frames 1-24, 183 on 25-50: the key tilted these degrees (CCW), measured
TALK_Z_HEIGHT = 38           # screen px of the controller button replacing the key
ZOOM = 1.6


def hide(swf, hidden):
    """SWF with the given {sprite: {character ids}} placements made fully transparent
    (an alpha-0 colour transform), so sprite bounds and every other part stay as they are."""
    raw, start = S.body(swf)

    def alpha0():
        w = F.Writer(); w.ub(1, 0); w.ub(1, 1); w.ub(4, 10)
        for v in (256, 256, 256, 0):
            w.ub(10, v & 0x3ff)
        return w.flush()
    out = bytearray(raw[:start])
    for code, pos, length in S.tags(raw, start, len(raw)):
        data = raw[pos:pos + length]
        if code == 39 and struct.unpack_from('<H', data)[0] in hidden:
            sid, frames = struct.unpack_from('<HH', data)
            chars, body, depth_char = hidden[sid], bytearray(), {}
            for c2, p2, l2 in S.tags(raw, pos + 4, pos + length):
                d = raw[p2:p2 + l2]
                if c2 == 26:
                    flags, depth, q = d[0], struct.unpack_from('<H', d, 1)[0], 3
                    if flags & 0x02:
                        depth_char[depth] = struct.unpack_from('<H', d, 3)[0]; q += 2
                    if depth_char.get(depth) in chars and not flags & 0x08:
                        if flags & 0x04:
                            b = S.Bits(d, q); S.matrix(b); q = b.byte
                        d = bytes([flags | 0x08]) + d[1:q] + alpha0() + d[q:]
                body += F._tag(c2, d)
            data = struct.pack('<HH', sid, frames) + bytes(body)
        out += F._tag(code, data)
    return b'FWS' + swf[3:4] + struct.pack('<I', len(out) + 8) + bytes(out)


def ffdec(jar, work, swf_path, ident, frames, out, svg=False):
    cmd = ['java', '-Xmx3g', '-Djava.awt.headless=true', f'-Duser.home={work.resolve()}/ffdec-home', '-jar',
           str(jar.resolve())] + F.FFDEC_RENDER + ['-onerror', 'abort', '-selectid', str(ident), '-select',
                                                   ','.join(f'{ident}:{f}' for f in frames)]
    if svg:
        cmd += ['-format', 'sprite:svg']
    else:
        cmd += ['-zoom', str(ZOOM)]
    with (work / 'export.log').open('a') as log:
        subprocess.run(cmd + ['-export', 'sprite', str(out), str(swf_path)], check=True, stdout=log, stderr=log)
    return next(out.glob(f'DefineSprite_{ident}*'))


def svg_origin(path):
    """Stage px of the frame canvas (0, 0) (the sprite's registration at (0, 0))."""
    g = ET.parse(path).getroot().find('{http://www.w3.org/2000/svg}g')
    t = [float(v) for v in re.findall(r'[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?', g.get('transform'))]
    return -t[4], -t[5]


def png_origin(svg_path, png_size, zoom=None):
    """svg_origin for FFDec's PNG of the same frame: the PNG canvas is padded
    evenly around filters (glows, blurs), so its (0, 0) sits half the size
    difference further up-left."""
    zoom = zoom or ZOOM
    root = ET.parse(svg_path).getroot()
    sw = float(re.search(r'[\d.]+', root.get('width')).group())
    sh = float(re.search(r'[\d.]+', root.get('height')).group())
    ox, oy = svg_origin(svg_path)
    return ox - (png_size[0] / zoom - sw) / 2, oy - (png_size[1] / zoom - sh) / 2


def build(work, jar, swf, sprites):
    """Returns (layers [(name, image, ox, oy)], ui_layout.h text)."""
    work.mkdir(parents=True, exist_ok=True)
    hidden = work / 'ui.swf'
    hidden.write_bytes(hide(swf, {MESSAGE: {MESSAGE_TEXT, PROMPT}}))
    box_dir = ffdec(jar, work, hidden, MESSAGE, range(1, 12), work / 'message')
    svg_dir = ffdec(jar, work, hidden, MESSAGE, [6], work / 'message-svg', svg=True)
    with Image.open(box_dir / '6.png') as p6:
        ox, oy = png_origin(svg_dir / '6.svg', p6.size)
    rects = []
    for f in range(1, 12):
        im = Image.open(box_dir / f'{f}.png').convert('RGBA')
        bb = im.getchannel('A').getbbox() if f > 1 else None   # frame 1 is the empty start
        rects.append((0, 0, 0, 0) if not bb else (ox + bb[0] / ZOOM, oy + bb[1] / ZOOM, ox + bb[2] / ZOOM, oy + bb[3] / ZOOM))
    full = Image.open(box_dir / '6.png').convert('RGBA')
    bb = full.getchannel('A').getbbox()
    layers = [('Message', full.crop(bb), rects[5][0], rects[5][1])]
    # the prompt
    prompt_dir = ffdec(jar, work, hidden, PROMPT, range(1, sprites[PROMPT]['frames'] + 1), work / 'prompt')
    psvg = ffdec(jar, work, hidden, PROMPT, [50], work / 'prompt-svg', svg=True)
    with Image.open(prompt_dir / '50.png') as p50:
        pox, poy = png_origin(psvg / '50.svg', p50.size)
    env = []
    for f in range(1, sprites[PROMPT]['frames'] + 1):
        env.append(int(np.asarray(Image.open(prompt_dir / f'{f}.png').convert('RGBA'))[..., 3].max()))
    peak = max(env)
    if env[prompt_art.FRAME - 1] != peak:
        raise ValueError('The prompt is not at its brightest on frame %d' % prompt_art.FRAME)
    # the approved redo (prompt_art.py): words and glow apart, the button in the Z's place
    text, glow = prompt_art.render_layers(ffdec, jar, work, swf)
    for k, kind in enumerate(sign_buttons.CONTROLLERS):
        im, left, top = prompt_art.build(text, glow, kind)
        layers.append((f'Prompt@{k}', im, pox + left / ZOOM, poy + top / ZOOM))
    # Toad's talk cloud prompt (bz, 184): the key is replaced by the controller's
    # button in the key's two poses, right-aligned with the key, at its centre.
    zpng = ffdec(jar, work, hidden, TALK_Z, [1], work / 'talkz')
    zsvg = ffdec(jar, work, hidden, TALK_Z, [1], work / 'talkz-svg', svg=True)
    with Image.open(zpng / '1.png') as z1:
        zox, zoy = png_origin(zsvg / '1.svg', z1.size)
    zb = Image.open(zpng / '1.png').convert('RGBA').getchannel('A').getbbox()
    to_tm = lambda px, o, at: at + TALK_Z_SCALE * (o + px / ZOOM)
    z_right = to_tm(zb[2], zox, TALK_Z_AT[0])
    z_cy = (to_tm(zb[1], zoy, TALK_Z_AT[1]) + to_tm(zb[3], zoy, TALK_Z_AT[1])) / 2
    for k, kind in enumerate(sign_buttons.CONTROLLERS):
        btn = sign_buttons.STYLE[kind]['stomp'](TALK_Z_HEIGHT)
        poses = [btn.rotate(a, Image.BICUBIC, expand=True) for a in TALK_Z_TILT]
        cw, ch = max(p.width for p in poses), max(p.height for p in poses)
        im = Image.new('RGBA', (2 * cw, ch))
        for i, pose in enumerate(poses):
            im.alpha_composite(pose, (i * cw + (cw - pose.width) // 2, (ch - pose.height) // 2))
        cx = z_right - btn.width / 2 / ZOOM
        layers.append((f'TalkZ@{k}', im, cx - cw / 2 / ZOOM, z_cy - ch / 2 / ZOOM))   # tm px of the left cell
    # where the prompt is placed: in the message box, and on each picture page (619 at stage (225, 150))
    places = {}
    for d, e in sprites[MESSAGE]['stage'][5].items():
        if e.get('id') == PROMPT:
            places['Message'] = (e['matrix']['tx'] / 20, e['matrix']['ty'] / 20)
    for label, f in sprites[PAGES]['labels'].items():
        for d, e in sprites[PAGES]['stage'][f].items():
            if e.get('id') == PROMPT:
                places[label] = (225 + e['matrix']['tx'] / 20, 150 + e['matrix']['ty'] / 20)
    h = ['/* Generated by tools/ui_art.py (via export_course_art.py); do not edit.',
         '   Stage px (450 x 300). The message box (Stats.Message 253) and its prompt (250). */',
         '#ifndef SM63_UI_LAYOUT_H', '#define SM63_UI_LAYOUT_H',
         '/* 253 frames 1-11: 2-6 open, 6 stays (text shown), 7-11 close (box rectangles). */',
         'static const float message_box_rects[11][4] = {']
    h += ['    {%.2ff, %.2ff, %.2ff, %.2ff},' % r for r in rects]
    h += ['};', '/* The text field (251, font 155 size 17, centred, leading 2): stage rect. */']
    tf = None
    for d, e in sprites[MESSAGE]['stage'][5].items():
        if e.get('id') == MESSAGE_TEXT:
            tf = (e['matrix']['tx'] / 20, e['matrix']['ty'] / 20)
    h.append('#define MESSAGE_TEXT_X0 %.2ff' % (tf[0] + 1090 / 20))
    h.append('#define MESSAGE_TEXT_X1 %.2ff' % (tf[0] + 8027 / 20))
    h.append('#define MESSAGE_TEXT_Y0 %.2ff' % (tf[1] - 40 / 20))
    h.append(f'/* Prompt brightness per frame of 250 (0..255; frames 80-{len(env)} loop). */')
    h.append(f'#define PROMPT_FRAMES {len(env)}')
    h.append('#define PROMPT_LOOP 79')
    h.append('static const unsigned char prompt_alpha[PROMPT_FRAMES] = {' +
             ', '.join(str(round(v * 255 / peak)) for v in env) + '};')
    h.append('/* Where 250 is placed (its registration), stage px: "Message" or a picture page label. */')
    h.append('typedef struct { const char *page; float x, y; } PromptPlace;')
    h.append('static const PromptPlace prompt_places[] = {')
    h += ['    {"%s", %.2ff, %.2ff},' % (k, x, y) for k, (x, y) in sorted(places.items())]
    h += ['};', '#define PROMPT_PLACES (sizeof(prompt_places) / sizeof(prompt_places[0]))', '#endif', '']
    return layers, '\n'.join(h)
