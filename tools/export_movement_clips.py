#!/usr/bin/env python3
"""Export every original Mario movement clip, in all four FLUDD states, from the
pinned Super Mario 63 SWF.

* Clip ids come from the compiled `Mario Main` frame labels (tools/swf_states.py),
  e.g. "H1", "Rbackflip". No hand-picked character ids.
* Static FFDec export does not run ActionScript. The five scripted FLUDD sprites
  reachable from movement clips only choose a frame from `_root.Fluddpow`
  (Fludd Side.xml, Fludd Side Front.xml, Dynamic Fludd Nozzle Clip.xml,
  Fludd Front Code.xml, Fludd Backwards.xml). For each FLUDD state a private
  temporary SWF freezes those sprites on the frame their script selects. The Wing
  Code sprite is blanked (WingCap=false), as the original script does.
* Offsets are recorded in twips from the Char origin (Mario's feet): Char places
  Mario Main at (0,-60); Mario Main places each clip with its own matrix.

Runtime frame selection (walk phase, spin speed, blast level...) is done by the
Wii code; see docs/movement-animation.md.
"""
import argparse, hashlib, json, re, struct, subprocess, zlib
from pathlib import Path
import xml.etree.ElementTree as ET
from PIL import Image
import swf_states as S
from build_player_art import build

SOURCE_SHA = '6f8dc22d74963cb75928fcac6ad67769384c3088bb419f9995e24b81d8411e1c'
WING_CODE = 1003
CHAR_TO_MAIN = (0, -60)  # DefineSprite 2061 places 1707 at ty=-60 twips.
# Art variant order: none, then the port's Nozzle enum (HOVER, ROCKET, TURBO).
VARIANTS = ['', 'H', 'R', 'T']
# Body states. Value: label suffix in Mario Main ("" variant uses frame numbers 1-4).
STATES = ['walk', 'rise', 'fall', 'Dive', 'diveflip', 'backflip', 'frontflip',
          'Spin', 'swim1', 'swim2', 'land', 'Run', 'Ground']
SHARED = {'land', 'Run', 'Ground'}       # same label for every FLUDD state
NUMBERED = {'walk': '1', 'rise': '3', 'fall': '4'}
BLAST_IDS = {1157: 'blast', 1340: 'rocket_blast'}
# Nested water clips inside body clips stay on frame 1 unless FLUDD is spraying
# (Wait WaterBlast / Diving / Frontflip Rocket Fludd Water: stop(), prevFrame()).
NESTED_WATER = (1157, 1349, 1370)
# Nested animations inside one-frame body clips: (nested sprite, frames) expanded
# into a timeline. 1440 Mario Land Animation; 1119/1124 Floating/Swimming Up,
# whose frame the Floating script picks from yspeed.
EXPAND = {1441: (1440, 8), 1234: (1119, 2), 1391: (1119, 2), 1305: (1119, 2),
          1237: (1124, 2), 1393: (1124, 2), 1307: (1124, 2)}
BACK_EFFECT, FRONT_EFFECT = 64, 128


def clip_id(state, variant):
    return STATES.index(state) * 4 + VARIANTS.index(variant)


def label_frame(main, state, variant):
    if state in SHARED:
        return main['labels'][state]
    if state in NUMBERED:
        return main['labels'][variant + NUMBERED[state]] if variant else int(NUMBERED[state]) - 1
    return main['labels'][variant + state]


def collapse(raw, pos, length, ident, keep):
    """DefineSprite body showing only frame `keep` as a one-frame clip."""
    out, shown = bytearray(), 0
    for code, p, n in S.tags(raw, pos + 4, pos + length):
        if code == 1:
            if shown == keep:
                break
            shown += 1
            continue
        if code in (0, 12, 43):  # End, DoAction, FrameLabel
            continue
        out += struct.pack('<H', (code << 6) | min(n, 63))
        if n >= 63:
            out += struct.pack('<I', n)
        out += raw[p:p + n]
    out += struct.pack('<H', 1 << 6) + struct.pack('<H', 0)
    return struct.pack('<HH', ident, 1) + bytes(out)


def fludd_frames(all_sprites, raw, start, roots):
    """Map every scripted FLUDD sprite reachable from movement clips to the frame
    index chosen per variant, by the structure of its known script."""
    scripted = set()
    for code, pos, length in S.tags(raw, start, len(raw)):
        if code == 39 and b'Fluddpow' in raw[pos:pos + length]:
            scripted.add(struct.unpack_from('<H', raw, pos)[0])
    seen, stack = set(), list(roots)
    while stack:
        x = stack.pop()
        if x in seen or x not in all_sprites:
            continue
        seen.add(x)
        stack += [e['id'] for f in all_sprites[x]['stage'] for e in f.values() if e.get('id')]
    choice = {}
    for ident in sorted(seen & scripted):
        sp = all_sprites[ident]
        if sp['frames'] == 2 and not sp['labels'] and not sp['stage'][1]:
            # Fludd Side / Side Front: Fluddpow=="" -> nextFrame (empty), else stop.
            choice[ident] = {v: (1 if v == '' else 0) for v in VARIANTS}
        elif sp['frames'] == 4 and set(sp['labels']) == {'H', 'R', 'T'}:
            # Dynamic Fludd Nozzle Clip: gotoAndStop(_root.Fluddpow).
            choice[ident] = {v: sp['labels'].get(v, 0) for v in VARIANTS}
        elif sp['frames'] == 4 and not sp['labels'] and not sp['stage'][3]:
            # Fludd Front Code / Backwards: ""->4, T->3, R->2, H->1 (1-based).
            choice[ident] = {'': 3, 'T': 2, 'R': 1, 'H': 0}
        else:
            raise ValueError(f'Unrecognised scripted FLUDD sprite {ident}; refusing to guess')
    return choice


def patched_swf(swf, choice, variant, freeze=None):
    freeze = freeze or {}
    raw, start = S.body(swf)
    out, found = bytearray(raw[:start]), False
    for code, pos, length in S.tags(raw, start, len(raw)):
        data = raw[pos:pos + length]
        if code == 39:
            ident = struct.unpack_from('<H', data)[0]
            if ident == WING_CODE:
                if b'WingCap' not in data:
                    raise ValueError('Unexpected Wing Code symbol')
                data, found = struct.pack('<HHHHH', WING_CODE, 2, 64, 64, 0), True
            elif ident in freeze:
                data = collapse(raw, pos, length, ident, freeze[ident])
            elif ident in choice:
                data = collapse(raw, pos, length, ident, choice[ident][variant])
        out += struct.pack('<H', (code << 6) | min(len(data), 63))
        if len(data) >= 63:
            out += struct.pack('<I', len(data))
        out += data
    if not found:
        raise ValueError('Expected Wing Code was not found')
    return b'FWS' + swf[3:4] + struct.pack('<I', len(out) + 8) + bytes(out)


def ffdec(jar, directory, swf_path, ids, log):
    cmd = ['java', '-Xmx2g', f'-Duser.home={directory.resolve()}/ffdec-home', '-Djava.awt.headless=true',
           '-jar', str(jar.resolve()), '-onerror', 'abort', '-ignorebackground']
    sel = ','.join(map(str, ids))
    subprocess.run(cmd + ['-selectid', sel, '-export', 'sprite', str(directory / 'png'), str(swf_path)],
                   check=True, stdout=log, stderr=log)
    subprocess.run(cmd + ['-selectid', sel, '-select', ','.join(f'{i}:1' for i in ids), '-format', 'sprite:svg',
                          '-export', 'sprite', str(directory / 'svg'), str(swf_path)], check=True, stdout=log, stderr=log)


def canvas_origin(directory, ident):
    ns = {'s': 'http://www.w3.org/2000/svg'}
    svg = ET.parse(directory / 'svg' / f'DefineSprite_{ident}' / '1.svg').getroot()
    values = [float(v) for v in re.findall(r'[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?', svg.find('s:g', ns).get('transform'))]
    if len(values) != 6 or values[:4] != [1, 0, 0, 1]:
        raise ValueError('Unexpected SVG registration transform')
    return values[4] * 20, values[5] * 20  # canvas pixel (0,0) is at -translate in clip space


def place(image, matrix, origin):
    """Apply an axis-aligned Mario Main placement. Returns (image, top-left twips)."""
    a, b, c, d = (round(matrix['sx'], 2), round(matrix['r0'], 2), round(matrix['r1'], 2), round(matrix['sy'], 2))
    if {abs(a), abs(d)} - {0, 1, 1.0} or {abs(b), abs(c)} - {0, 1, 1.0} or (a != 0) == (b != 0):
        raise ValueError(f'Unsupported placement matrix {matrix}')
    a, b, c, d = (1 if v > 0 else -1 if v < 0 else 0 for v in (a, b, c, d))
    ox, oy = -origin[0], -origin[1]
    w, h = image.width * 20, image.height * 20
    corners = [(ox, oy), (ox + w, oy), (ox, oy + h), (ox + w, oy + h)]
    # Flash: x' = a*x + c*y + tx ; y' = b*x + d*y + ty
    mapped = [(a * x + c * y, b * x + d * y) for x, y in corners]
    left, top = min(p[0] for p in mapped), min(p[1] for p in mapped)
    if b:  # axes swap: x' follows y, y' follows x
        image = image.transpose(Image.TRANSPOSE)
        if c < 0: image = image.transpose(Image.FLIP_LEFT_RIGHT)
        if b < 0: image = image.transpose(Image.FLIP_TOP_BOTTOM)
    else:
        if a < 0: image = image.transpose(Image.FLIP_LEFT_RIGHT)
        if d < 0: image = image.transpose(Image.FLIP_TOP_BOTTOM)
    return image, (round(left + matrix['tx'] + CHAR_TO_MAIN[0]), round(top + matrix['ty'] + CHAR_TO_MAIN[1]))


def y_flipped(matrix):
    """Wait WaterBlast.xml: `_yscale *= -1` every frame, i.e. M * diag(1,-1)."""
    m = dict(matrix)
    m['r1'], m['sy'] = -matrix['r1'], -matrix['sy']
    return m


def export(source, jar, output):
    swf = (source / 'Publish/Super_Mario_63_2010.swf').read_bytes()
    if hashlib.sha256(swf).hexdigest() != SOURCE_SHA:
        raise ValueError('Unsupported SWF revision; do not guess character ids')
    all_sprites = S.sprites(swf)
    main_id = S.mario_main(all_sprites)
    main = all_sprites[main_id]
    raw, start = S.body(swf)
    plan = {}  # variant -> list of (state, frame index in Main)
    for v in VARIANTS:
        plan[v] = [(st, label_frame(main, st, v)) for st in STATES]
    roots = {e['id'] for v in VARIANTS for _, i in plan[v] for e in main['stage'][i].values() if e.get('id')}
    choice = fludd_frames(all_sprites, raw, start, roots)
    work = output.parent / 'rendered-movement'
    work.mkdir(parents=True, exist_ok=True)
    clips, report = [], {'mario_main': main_id, 'scripted_fludd': {str(k): v for k, v in choice.items()}, 'clips': {}}
    with (work / 'export.log').open('w') as log:
        for v in VARIANTS:
            directory = work / (v or 'none')
            directory.mkdir(exist_ok=True)
            stages = {st: [(d, e) for d, e in main['stage'][i].items() if e.get('id') != 896] for st, i in plan[v]}
            bodies = sorted({e['id'] for st in stages for _, e in stages[st] if e['id'] not in BLAST_IDS})
            blasts = sorted({e['id'] for st in stages for _, e in stages[st] if e['id'] in BLAST_IDS})
            water = {w: 0 for w in NESTED_WATER}
            passes = {'body': (dict(water), [b for b in bodies if b not in EXPAND])}
            if blasts:
                passes['fx'] = ({}, blasts)
            for b in bodies:
                if b in EXPAND:
                    nested, count = EXPAND[b]
                    for k in range(count):
                        passes[f"x{b}_{k}"] = ({**water, nested: k}, [b])
            rendered = {}
            for name, (freeze, ids) in passes.items():
                pdir = directory / name
                pdir.mkdir(exist_ok=True)
                swf_path = pdir / 'variant.swf'
                swf_path.write_bytes(patched_swf(swf, choice, v, freeze))
                ffdec(jar, pdir, swf_path, ids, log)
                for ident in ids:
                    rendered.setdefault(ident, []).append(pdir)
            def sources(ident):
                """[(png path, canvas origin)] in timeline order."""
                out = []
                if ident in EXPAND:
                    for k in range(EXPAND[ident][1]):
                        pdir = directory / f'x{ident}_{k}'
                        out.append((pdir / 'png' / f'DefineSprite_{ident}' / '1.png', canvas_origin(pdir, ident)))
                    return out
                pdir = rendered[ident][0]
                origin = canvas_origin(pdir, ident)
                return [(pdir / 'png' / f'DefineSprite_{ident}' / f'{n}.png', origin)
                        for n in range(1, all_sprites[ident]['frames'] + 1)]
            for state, index in plan[v]:
                stage = stages[state]
                body_depth = max((d for d, e in stage if e['id'] not in BLAST_IDS), default=None)
                for depth, entry in stage:
                    ident = entry['id']
                    frames = []
                    # Spray: frames are [level0 even, level0 odd, level1 even, ...].
                    phases = [entry['matrix'], y_flipped(entry['matrix'])] if ident == 1157 else [entry['matrix']]
                    for n, (png, origin) in enumerate(sources(ident), 1):
                        source = Image.open(png).convert('RGBA')
                        for k, m in enumerate(phases):
                            image, offset = place(source, m, origin)
                            name = directory / 'placed' / f'{state}_{depth}_{n}_{k}.png'
                            name.parent.mkdir(exist_ok=True)
                            image.save(name)
                            frames.append({'file': str(name.resolve()), 'offset_twips': list(offset), 'duration': 1})
                    if ident in BLAST_IDS:
                        base = BACK_EFFECT if depth < body_depth else FRONT_EFFECT
                        cid = base + clip_id(state, v)
                    else:
                        cid = clip_id(state, v)
                    clips.append({'id': cid, 'loop': False, 'frames': frames})
                    report['clips'][str(cid)] = {'state': state, 'variant': v or 'none', 'sprite': ident,
                                                 'frames': len(frames), 'depth': depth}
    (work / 'report.json').write_text(json.dumps(report, indent=1))
    data = {'canonical_facing': 'left', 'source_swf_sha256': SOURCE_SHA, 'origin': 'feet', 'clips': clips}
    (work / 'manifest.json').write_text(json.dumps(data, indent=1))
    output.write_bytes(build(data, Path('/')))
    return report


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('source', type=Path); p.add_argument('ffdec_jar', type=Path); p.add_argument('output', type=Path)
    a = p.parse_args()
    r = export(a.source, a.ffdec_jar, a.output)
    print(f"Exported {len(r['clips'])} original movement clips in 4 FLUDD states")
