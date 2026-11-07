#!/usr/bin/env python3
"""The original game's text fonts as one GX texture atlas plus metrics.

The bold Arial embedded in the 2010 SWF as font 155 (exported with FFDec), used
by the sign message box (Stats.Message 'mtext', size 17) and Toad's talk cloud
(Stats.tm 'txt2', size 15), and the regular Arial 187 of the hints (Stats
'_root.TextHint', size 11; missing characters from Liberation Sans Regular). Each size is rasterised at the screen size the port draws it (Flash size x 1.6, the
stage scale on the 480-line screen). Characters an embedded subset lacks (font
155 has no 'q') come from Liberation Sans Bold, which has Arial's metrics.

Output: <out>/font_atlas.bin ("SM63FNT1", u32 width, u32 height, then GX
RGBA8 tiles: white glyphs, coverage in alpha) and <out>/font_metrics.h.

    python export_fonts.py <checkout> <ffdec.jar> <out_dir>
"""
import argparse, struct, subprocess
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont
from build_mario_atlas import rgba8_tiles

SCALE = 1.6
FONTS = [  # (name, SWF font id, Flash size)
    ('FONT_SIGN', 155, 17),   # Stats.Message 'mtext' (251): sign messages, centred white
    ('FONT_TALK', 155, 15),   # Stats.tm 'txt2' (185): Toad's talk cloud, left aligned, light yellow
    ('FONT_HINT', 187, 11),   # Stats '_root.TextHint' (621): the hints, regular Arial, white with a black glow
]
FALLBACK = '/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf'
FALLBACK_REGULAR = '/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf'
REGULAR = {187}   # fonts that are not bold
# Flash letterSpacing of each font's field (px added after every character):
# Toad's field 185 has letterSpacing="-1"; 251 and 621 have 0.
LETTER_SPACING = {'FONT_TALK': -1.0}
# Field 185 carries a Bevel (white shadow, black highlight, blur 5, distance 5,
# 45 degrees, inner, strength 1) and a black Glow (blur 2, strength 10). Its
# glyphs are baked as FFDec renders them (talk_fx): FONT_TALK_FX is the whole
# look (outline + bevelled fill), FONT_TALK_FILL the bevelled fill alone, drawn
# second so a glyph's outline never covers its neighbour's fill. Same layout as
# FONT_TALK.
TALK, TALK_SPRITE, TALK_SIZE = 185, 186, 15
TALK_COLOUR = (255, 255, 179)
PER_LINE, LINES = 8, 1   # one line per render: tall brackets would join lines

FIRST, LAST = 32, 126
ATLAS_W = 512


def export_ttf(source, jar, work):
    work.mkdir(parents=True, exist_ok=True)
    if all(list(work.glob(f'{i}_*.ttf')) for i in {i for _, i, _ in FONTS}):
        return
    subprocess.run(['java', '-Xmx2g', '-Djava.awt.headless=true', f'-Duser.home={work.resolve()}/ffdec-home', '-jar',
                    str(jar.resolve()), '-selectid', ','.join(str(i) for i in sorted({i for _, i, _ in FONTS})), '-format', 'font:ttf',
                    '-export', 'font', str(work), str(source / 'Publish/Super_Mario_63_2010.swf')],
                   check=True, capture_output=True)


def swf_layout(source, ident):
    """(ascent, descent) in em of DefineFont3 `ident`'s layout: what Flash lays
    text out with (FFDec's exported TTF carries a smaller ascent)."""
    import swf_states as S
    swf = (source / 'Publish/Super_Mario_63_2010.swf').read_bytes()
    raw, start = S.body(swf)
    for code, pos, length in S.tags(raw, start, len(raw)):
        if code == 75 and struct.unpack_from('<H', raw, pos)[0] == ident:
            d = raw[pos:pos + length]
            flags, nl = d[2], d[4]
            q = 5 + nl
            n = struct.unpack_from('<H', d, q)[0]; q += 2
            wide = flags & 0x08
            code_table = q + struct.unpack_from('<I' if wide else '<H', d, q + n * (4 if wide else 2))[0]
            q = code_table + n * (2 if flags & 0x04 else 1)
            if not flags & 0x80:
                raise ValueError(f'font {ident}: no layout')
            asc, desc = struct.unpack_from('<hh', d, q)
            return asc / 20480, desc / 20480
    raise ValueError(f'font {ident}: no DefineFont3')


def glyph_set(path):
    from fontTools.ttLib import TTFont
    return {chr(c) for c in TTFont(path).getBestCmap()}


def talk_fx(source, jar, work, plain):
    """{char: (RGBA image, ox, oy)} of field 185's glyphs with its filters, as
    FFDec draws them at the port's scale; (ox, oy) from the pen on the baseline."""
    import re
    import numpy as np
    import swf_states as S, swf_bitmap_fix as F, ui_art, water_art
    swf = (source / 'Publish/Super_Mario_63_2010.swf').read_bytes()
    raw, start = S.body(swf)
    others = {c for _, c, _, _ in water_art.placements(raw, start, TALK_SPRITE) if c and c != TALK}
    base = ui_art.hide(swf, {TALK_SPRITE: others})
    chars = [chr(c) for c in range(FIRST + 1, LAST + 1)]
    out, todo = {}, list(chars)
    para = ('<p align="left"><font face="Arial" size="15" color="#ffffb3" letterSpacing="0.000000" '
            'kerning="0"><b>{}</b></font></p>')
    esc = {'<': '&lt;', '>': '&gt;', '&': '&amp;', '"': '&quot;'}
    batch = 0
    while todo:
        rows = [todo[i:i + PER_LINE] for i in range(0, min(len(todo), PER_LINE * LINES), PER_LINE)]
        todo = todo[sum(map(len, rows)):]
        html = ''.join(para.format('   '.join(esc.get(c, c) for c in r)) for r in rows)
        raw2, start2 = S.body(base)
        body = bytearray(raw2[:start2])
        for code, pos, length in S.tags(raw2, start2, len(raw2)):
            data = raw2[pos:pos + length]
            if code == 37 and struct.unpack_from('<H', data)[0] == TALK:
                i = data.index(b'txt2\x00') + 5
                data = data[:i] + html.encode('latin1') + b'\x00'
            body += F._tag(code, data)
        path = work / f'talk{batch}.swf'
        path.write_bytes(b'FWS' + base[3:4] + struct.pack('<I', len(body) + 8) + bytes(body))
        old = ui_art.ZOOM; ui_art.ZOOM = SCALE
        try:
            d = ui_art.ffdec(jar, work, path, TALK_SPRITE, [1], work / f'talk{batch}')
        finally:
            ui_art.ZOOM = old
        im = np.asarray(Image.open(d / '1.png').convert('RGBA'))
        a = im[..., 3] > 6
        # the lines: bands of rows with ink
        ys = np.nonzero(a.any(1))[0]
        bands, b0 = [], ys[0]
        for y0, y1 in zip(ys, ys[1:]):
            if y1 - y0 > 2:
                bands.append((b0, y0 + 1)); b0 = y1
        bands.append((b0, ys[-1] + 1))
        if len(bands) != len(rows):
            raise ValueError(f'talk_fx: {len(bands)} lines, expected {len(rows)}')
        for (t, btm), row in zip(bands, rows):
            cols = np.nonzero(a[t:btm].any(0))[0]
            runs, r0 = [], cols[0]
            for x0, x1 in zip(cols, cols[1:]):
                if x1 - x0 > 7:          # the three spaces leave about 15 px
                    runs.append((r0, x0 + 1)); r0 = x1
            runs.append((r0, cols[-1] + 1))
            if len(runs) != len(row):
                raise ValueError(f'talk_fx: {len(runs)} glyphs for {row!r}')
            for (l, r), ch in zip(runs, row):
                sub = im[:, l:r]
                rows_ink = np.nonzero((sub[..., 3] > 6).any(1))[0]
                rows_ink = rows_ink[(rows_ink >= t - 3) & (rows_ink < btm + 3)]
                top, bot = rows_ink[0], rows_ink[-1] + 1
                # 4 px spare: characters font 155 lacks are drawn by FFDec from
                # another font, a little smaller than FONT_TALK's fallback glyph
                crop = np.pad(im[top:bot, l:r], ((4, 4), (4, 4), (0, 0)))
                # line it up with the plain glyph: the bright fill against its coverage
                g, gox, goy = plain[ch]     # FONT_TALK's glyph (1 px border), corner from the pen
                box = (gox, goy)
                ink = g.getbbox()
                g = g.crop(ink)
                box = (box[0] + ink[0], box[1] + ink[1])
                ga = np.asarray(g).astype(float) / 255
                fill = crop[..., :3].astype(float).mean(2) * crop[..., 3] / 255 / 255
                best = None
                for dy in range(crop.shape[0] - ga.shape[0] + 1):
                    for dx in range(crop.shape[1] - ga.shape[1] + 1):
                        e = ((fill[dy:dy + ga.shape[0], dx:dx + ga.shape[1]] - ga) ** 2).sum()
                        if best is None or e < best[0]:
                            best = (e, dx, dy)
                if best is None:
                    raise ValueError(f'talk_fx: {ch!r} glyph larger than its render {ga.shape} {crop.shape}')
                _, dx, dy = best
                mask = np.zeros(crop.shape[:2])
                mask[dy:dy + ga.shape[0], dx:dx + ga.shape[1]] = ga
                fill_only = crop.copy()
                fill_only[..., 3] = (np.minimum(crop[..., 3] / 255, mask) * 255).round().astype(np.uint8)
                pad = lambda x: np.pad(x, ((1, 1), (1, 1), (0, 0)))
                out[ch] = (Image.fromarray(pad(crop)), Image.fromarray(pad(fill_only)),
                           box[0] - dx - 1, box[1] - dy - 1)
        batch += 1
    return out


def build(source, jar, out_dir, work):
    export_ttf(source, jar, work)
    glyphs = []  # (font index, char, image, ox, oy, advance)
    infos = []
    for fi, (name, ident, size) in enumerate(FONTS):
        path = str(next(work.glob(f'{ident}_*.ttf')))
        px = size * SCALE
        main = ImageFont.truetype(path, round(px))
        fallback = ImageFont.truetype(FALLBACK_REGULAR if ident in REGULAR else FALLBACK, round(px))
        have = glyph_set(path)
        from fontTools.ttLib import TTFont
        t = TTFont(path)
        em = t['head'].unitsPerEm
        asc_em, desc_em = swf_layout(source, ident)
        ascent, descent = asc_em * round(px), desc_em * round(px)
        infos.append((name, round(px), ascent, descent))
        for c in range(FIRST, LAST + 1):
            ch = chr(c)
            f = main if ch in have or ch == ' ' else fallback
            adv = f.getlength(ch) + LETTER_SPACING.get(name, 0) * SCALE
            box = f.getbbox(ch, anchor='ls')
            if box[2] <= box[0] or box[3] <= box[1]:
                glyphs.append((fi, ch, None, 0, 0, adv))
                continue
            im = Image.new('L', (box[2] - box[0] + 2, box[3] - box[1] + 2))
            ImageDraw.Draw(im).text((1 - box[0], 1 - box[1]), ch, font=f, fill=255, anchor='ls')
            glyphs.append((fi, ch, im, box[0] - 1, box[1] - 1, adv))
    talk = FONTS.index(next(f for f in FONTS if f[0] == 'FONT_TALK'))
    fx = talk_fx(source, jar, work, {ch: (im, ox, oy) for gfi, ch, im, ox, oy, _ in glyphs if gfi == talk and im})
    for k, name in enumerate(('FONT_TALK_FX', 'FONT_TALK_FILL')):
        fi = len(infos)
        infos.append((name,) + infos[talk][1:])
        for (gfi, ch, im, ox, oy, adv) in [g for g in glyphs if g[0] == talk]:
            if ch in fx:
                whole, fill, fox, foy = fx[ch]
                glyphs.append((fi, ch, whole if k == 0 else fill, fox, foy, adv))
            else:
                glyphs.append((fi, ch, None, 0, 0, adv))
    # shelf pack, 1 px gaps
    x = y = row = 0
    spots = []
    for g in sorted(range(len(glyphs)), key=lambda i: -(glyphs[i][2].height if glyphs[i][2] else 0)):
        im = glyphs[g][2]
        if im is None:
            spots.append((g, 0, 0)); continue
        if x + im.width + 1 > ATLAS_W:
            x, y, row = 0, y + row + 1, 0
        spots.append((g, x + 1, y + 1))
        x += im.width + 1; row = max(row, im.height)
    height = 64
    while height < y + row + 2:
        height *= 2
    atlas = Image.new('RGBA', (ATLAS_W, height), (255, 255, 255, 0))
    place = {}
    for g, sx, sy in spots:
        im = glyphs[g][2]
        place[g] = (sx, sy)
        if im is not None:
            atlas.paste(im if im.mode == 'RGBA' else Image.merge('RGBA', (Image.new('L', im.size, 255),) * 3 + (im,)),
                        (sx, sy))
    out_dir.mkdir(parents=True, exist_ok=True)
    (out_dir / 'font_atlas.bin').write_bytes(b'SM63FNT1' + struct.pack('>II', ATLAS_W, height) + bytes(16) +
                                             rgba8_tiles(atlas))
    lines = ['/* Generated by tools/export_fonts.py; do not edit. The original font 155 (bold',
             '   Arial) at the sign (17) and Toad (15) sizes x 1.6. Glyph boxes',
             '   are atlas px; (ox, oy) is the box corner from the pen on the baseline. */',
             '#ifndef SM63_FONT_METRICS_H', '#define SM63_FONT_METRICS_H',
             f'#define FONT_ATLAS_W {ATLAS_W}', f'#define FONT_ATLAS_H {height}', f'#define FONT_FIRST {FIRST}',
             f'#define FONT_LAST {LAST}', f'#define FONT_COUNT {len(infos)}',
             'typedef struct { unsigned short x, y, w, h; short ox, oy; float advance; } FontGlyph;',
             'typedef struct { float size, ascent, descent; const FontGlyph *glyphs; } FontInfo;']
    for fi, (name, px, asc, desc) in enumerate(infos):
        rows = []
        for g, (gfi, ch, im, ox, oy, adv) in enumerate(glyphs):
            if gfi != fi:
                continue
            sx, sy = place[g]
            w, h = (im.width, im.height) if im else (0, 0)
            rows.append(f'{{{sx}, {sy}, {w}, {h}, {ox}, {oy}, {adv:.3f}f}}')
        lines.append(f'static const FontGlyph {name.lower()}_glyphs[] = {{')
        for i in range(0, len(rows), 4):
            lines.append('    ' + ', '.join(rows[i:i + 4]) + ',')
        lines.append('};')
    lines.append('/* FONT_TALK_FX / FONT_TALK_FILL: coloured glyphs (draw with white) */')
    lines.append('enum { ' + ', '.join(n for n, *_ in infos) + ' };')
    lines.append('static const FontInfo font_info[FONT_COUNT] = {')
    for name, px, asc, desc in infos:
        lines.append(f'    {{{px}.0f, {asc:.3f}f, {desc:.3f}f, {name.lower()}_glyphs}},')
    lines += ['};', '#endif', '']
    (out_dir / 'font_metrics.h').write_text('\n'.join(lines))
    return atlas


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('source', type=Path); p.add_argument('ffdec_jar', type=Path); p.add_argument('out_dir', type=Path)
    a = p.parse_args()
    work = Path(__file__).resolve().parent.parent / 'private-art' / 'fonts'
    atlas = build(a.source, a.ffdec_jar, a.out_dir, work)
    atlas.save(work / 'atlas.png')
    print('Wrote font atlas', atlas.size)
