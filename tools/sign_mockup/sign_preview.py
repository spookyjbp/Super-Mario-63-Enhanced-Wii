"""Previews of the sign prompt and message layout as the port draws them
(course_render.c + include/sign_layout.h), from the generated art pack."""
import re, struct, sys
from pathlib import Path
import numpy as np
import lz4.block
from PIL import Image, ImageDraw, ImageFont
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tests'))
from test_art_repair import rgb5a3
K = 1.6
GEN = ROOT / 'assets' / 'generated'
LAYOUT = (GEN / 'ui_layout.h').read_text()
PLACES = {m.group(1): (float(m.group(2)), float(m.group(3))) for m in re.finditer(r'\{"([^"]+)", ([\d.]+)f, ([\d.]+)f\}', LAYOUT)}
DEF = {m.group(1): float(m.group(2)) for m in re.finditer(r'#define (MESSAGE_TEXT_\w+) ([\d.]+)f', LAYOUT)}
RECTS = [tuple(float(v) for v in m.groups()) for m in re.finditer(r'\{([\d.]+)f, ([\d.]+)f, ([\d.]+)f, ([\d.]+)f\}', LAYOUT)]
PROMPT_GAP, TEXT_GAP = 4.0, 6.0


def layers():
    d = (ROOT / 'assets/data/course_8e1_art.bin').read_bytes()
    n = struct.unpack_from('>8sIIf', d)[2]; q = 20; out = {}
    def string():
        nonlocal q
        L = struct.unpack_from('>H', d, q)[0]; s = d[q + 2:q + 2 + L].decode(); q += 2 + L; return s
    for _ in range(n):
        area, name = string(), string()
        ox, oy, _, _ = struct.unpack_from('>ffff', d, q); q += 16
        w, h = struct.unpack_from('>II', d, q); q += 8
        count = struct.unpack_from('>I', d, q)[0]; q += 4
        im = np.zeros((h, w, 4), np.uint8)
        for _ in range(count):
            tx, ty, tw, th, size, packed = struct.unpack_from('>HHHHII', d, q); q += 16
            raw = lz4.block.decompress(d[q:q + packed], uncompressed_size=size); q += packed; q += (-q) % 4
            im[ty:ty + th, tx:tx + tw] = rgb5a3(raw, tw, th)
        if area == '*':
            out[name] = (Image.fromarray(im), ox, oy)
    return out


def sx(x, W=854): return W / 2 + (x - 225) * K


def picture(L, backdrop, label, k):
    page, pox, poy = L.get(f'{label}@{k}', L[label])
    pr, ox, oy = L[f'Prompt@{k}']
    scr = backdrop.copy()
    scr.alpha_composite(page, (round(sx(pox)), round(poy * K)))
    # every picture sign (sign_layout.h): under the page, else the original's place
    # (or, without one, centred at the screen's bottom edge)
    rx, ry = PLACES.get(label, (pox + page.width / K / 2 - pr.width / K / 2 - ox, 300 - 2 - pr.height / K - oy))
    top = poy + page.height / K + PROMPT_GAP
    if top + pr.height / K <= 298: x, y = pox + page.width / K / 2 - pr.width / K / 2, top
    else: x, y = rx + ox, ry + oy
    scr.alpha_composite(pr, (round(sx(x)), round(y * K)))
    return scr


def message(L, backdrop, text, k):
    box, bx, by = L['Message']
    scr = backdrop.copy()
    r = RECTS[5]
    scr.alpha_composite(box.resize((round((r[2] - r[0]) * K), round((r[3] - r[1]) * K)), Image.LANCZOS), (round(sx(r[0])), round(r[1] * K)))
    font = ImageFont.truetype('/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf', 27)  # Arial metrics (155 lacks 'q')
    x0, x1 = sx(DEF['MESSAGE_TEXT_X0'] + 2), sx(DEF['MESSAGE_TEXT_X1'] - 2)
    lines = []
    for para in text.split('\n'):
        cur = ''
        for w in para.split(' '):
            t = (cur + ' ' + w).strip()
            if font.getlength(t) <= x1 - x0 or not cur: cur = t
            else: lines.append(cur); cur = w
        lines.append(cur)
    asc, desc = font.getmetrics()
    lh = (asc + desc + 2 * K) / K
    pr, ox, oy = L[f'Prompt@{k}']
    rx, ry = PLACES['Message']
    prompt_top = ry + oy
    top0 = DEF['MESSAGE_TEXT_Y0'] + 2
    avail, block = prompt_top - TEXT_GAP - top0, len(lines) * lh
    top = top0 if block >= avail else top0 + (avail - block) / 2
    d = ImageDraw.Draw(scr)
    for i, ln in enumerate(lines):
        d.text(((x0 + x1) / 2, top * K + asc + i * lh * K), ln, font=font, fill=(255, 255, 255, 255), anchor='ms')
    scr.alpha_composite(pr, (round(sx(rx + ox)), round((ry + oy) * K)))
    return scr
