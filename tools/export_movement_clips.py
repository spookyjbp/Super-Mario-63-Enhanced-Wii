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
          'Spin', 'swim1', 'swim2', 'land', 'Run', 'Ground', 'hurt2', 'Sign']
SHARED = {'land', 'Run', 'Ground', 'hurt2', 'Sign'}  # same label for every FLUDD state
# Frames kept per state: hurt2 (Mario Main "hurt2", the HitWall bonk) runs 1-32;
# its later frames are the faint sequence.
FRAME_CAP = {'hurt2': 32}
# FLUDD spray (Wait WaterBlast 1157) frames 2-5 = levels 1-4, unplaced, for the
# sprays nested inside the Dive and Frontflip bodies (drawn at runtime with the
# matrices in assets/generated/player_spray.h).
SPRAY_CLIP = 60
SPRAY_STATES = ('Dive', 'frontflip')
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


def mat(m):
    """swf_states matrix -> (a, b, c, d, tx, ty): x' = a x + c y + tx, y' = b x + d y + ty."""
    m = m or {}
    return (m.get('sx', 1.0), m.get('r0', 0.0), m.get('r1', 0.0), m.get('sy', 1.0), m.get('tx', 0), m.get('ty', 0))


def mul(p, q):
    """p after q (q applied first)."""
    a1, b1, c1, d1, x1, y1 = p
    a2, b2, c2, d2, x2, y2 = q
    return (a1 * a2 + c1 * b2, b1 * a2 + d1 * b2, a1 * c2 + c1 * d2, b1 * c2 + d1 * d2,
            a1 * x2 + c1 * y2 + x1, b1 * x2 + d1 * y2 + y1)


def spray_matrices(all_sprites, main, variant):
    """Per Dive/Frontflip body frame, every nested 1157 as (matrix from the Char
    origin in twips, behind-body flag). Frontflip clips set
    `water._rotation = frame._rotation` every frame (Mario (Hover) Frontflip.xml)."""
    import math
    out = {}
    for state in SPRAY_STATES:
        if not variant:
            continue
        entry = [e for e in main['stage'][label_frame(main, state, variant)].values() if e.get('id') != 896][0]
        body = all_sprites[entry['id']]
        base = mul((1, 0, 0, 1, CHAR_TO_MAIN[0], CHAR_TO_MAIN[1]), mat(entry.get('matrix')))
        frames, last = [], {}
        for n, stage in enumerate(body['stage']):
            for d, e in stage.items():  # SWF frames only list changes after a move
                if e.get('matrix') or d not in last:
                    last[d] = e
            items = sorted(last.items())
            body_depth = max((d for d, e in items if e['id'] not in (1157, WING_CODE) and e.get('name') != 'water'), default=0)
            frame_rot = next((mat(e.get('matrix')) for d, e in items if e.get('name') == 'frame'), None)
            sprays = []
            for d, e in items:
                if e['id'] == 1157:
                    sprays.append((mul(base, mat(e.get('matrix'))), d < body_depth))
                elif e['id'] in all_sprites and e.get('name') == 'water':
                    inner = [x for x in all_sprites[e['id']]['stage'][0].values() if x.get('id') == 1157]
                    if not inner:
                        continue
                    m = mat(e.get('matrix'))
                    if frame_rot is not None:  # water._rotation = frame._rotation
                        r = math.atan2(frame_rot[1], frame_rot[0])
                        m = (math.cos(r), math.sin(r), -math.sin(r), math.cos(r), m[4], m[5])
                    for x in inner:
                        sprays.append((mul(base, mul(m, mat(x.get('matrix')))), d < body_depth))
            frames.append(sprays)
        if any(frames):
            out[state] = frames
    return out


def write_spray_header(path, tables):
    order = {'Dive': 'ART_DIVE', 'frontflip': 'ART_FRONTFLIP'}
    lines = ['/* Generated by tools/export_movement_clips.py; do not edit. FLUDD sprays',
             '   nested in the Dive and Frontflip bodies: per body frame, matrix from the',
             '   Char origin in twips (x\' = a x + c y + tx), canonical-left. */',
             '#ifndef SM63_PLAYER_SPRAY_H', '#define SM63_PLAYER_SPRAY_H',
             f'#define PLAYER_SPRAY_CLIP {SPRAY_CLIP}',
             'typedef struct { float a, b, c, d, tx, ty; unsigned char behind; } SprayPlacement;',
             'typedef struct { unsigned char state, variant, frames, per_frame; const SprayPlacement *data; } SprayTable;']
    tables_out = []
    for v, states in tables.items():
        for state, frames in states.items():
            per = max(len(f) for f in frames)
            name = f'spray_{state.lower()}_{v.lower()}'
            rows = []
            for f in frames:
                for k in range(per):
                    if k < len(f):
                        (a, b, c, d, tx, ty), behind = f[k]
                        rows.append(f'{{{a:.5f}f, {b:.5f}f, {c:.5f}f, {d:.5f}f, {tx:.1f}f, {ty:.1f}f, {int(behind)}}}')
                    else:
                        rows.append('{0, 0, 0, 0, 0, 0, 2}')  # unused slot
            lines.append(f'static const SprayPlacement {name}[] = {{\n    ' + ',\n    '.join(rows) + '\n};')
            tables_out.append(f'{{{order[state]}, {VARIANTS.index(v)}, {len(frames)}, {per}, {name}}}')
    lines.append('static const SprayTable player_spray_tables[] = {\n    ' + ',\n    '.join(tables_out) + '\n};')
    lines.append(f'#define PLAYER_SPRAY_TABLES {len(tables_out)}')
    lines += ['#endif', '']
    path.write_text('\n'.join(lines))


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
    spray_tables = {}
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
                        if n > FRAME_CAP.get(state, 1 << 30):
                            break
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
            if blasts and 1157 in rendered and not any(c['id'] == SPRAY_CLIP for c in clips):
                pdir = rendered[1157][0]
                ox, oy = canvas_origin(pdir, 1157)
                frames = [{'file': str((pdir / 'png' / 'DefineSprite_1157' / f'{n}.png').resolve()),
                           'offset_twips': [round(-ox), round(-oy)], 'duration': 1} for n in range(1, 6)]
                clips.append({'id': SPRAY_CLIP, 'loop': False, 'frames': frames})
            spray_tables[v] = spray_matrices(all_sprites, main, v)
    write_spray_header(output.parent / 'player_spray.h', spray_tables)
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
