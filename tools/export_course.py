#!/usr/bin/env python3
"""Export original course areas (collision masks, camera edge, water, placed
objects) from a Runouw/Super-Mario-63 checkout into the course pack format.

Geometry comes from the compiled SWF: each layer sprite named by `StartChar`'s
linkage ids (`<area>Platforms`, `BPlatforms`, `CamEdge`, `Ice`, `Puddle`,
`Lava`) is rendered by FFDec at 1 px per source pixel; Flash `hitTest(x, y,
true)` is approximated as alpha >= 128. Objects come from the XFL `BackGFX`,
`FrontGFX` and `Water` symbols with their instance names, positions, keyframe
ranges and `onClipEvent(load)` parameters. Coordinates are course pixels with
Char at (0,0) for `StartChar(area, 0, 0, 0, 0)`.

    python export_course.py <checkout> <ffdec.jar> <out.bin> 8-E1-1 8-E1-1-2 ...
"""
import argparse, hashlib, json, re, struct, subprocess
from pathlib import Path
import xml.etree.ElementTree as ET
from PIL import Image
import swf_states as S
import xfl

SOURCE_SHA = '6f8dc22d74963cb75928fcac6ad67769384c3088bb419f9995e24b81d8411e1c'
VERSION = 2
MASK_LAYERS = ['Platforms', 'BPlatforms', 'CamEdge', 'Ice', 'Puddle', 'Lava']
OBJECT_LAYERS = ['BackGFX', 'FrontGFX', 'Water']
# Decoration-only clips whose graphics belong to the layer art, not to objects.
DECOR = re.compile(r'(Color (Back|Front) GFX|Color Changing|Background Hills|Bitmap Fix|BG Quality)')


def exports(raw, start):
    out = {}
    for code, pos, length in S.tags(raw, start, len(raw)):
        if code == 56:
            count = struct.unpack_from('<H', raw, pos)[0]; q = pos + 2
            for _ in range(count):
                ident = struct.unpack_from('<H', raw, q)[0]; q += 2
                end = raw.index(0, q); out[raw[q:end].decode('latin-1')] = ident; q = end + 1
    return out


def evaluate(expr):
    """AS2 string expression of literals, `newline` and `+` (sign/Toad text)."""
    parts, out = re.findall(r'"((?:[^"\\]|\\.)*)"|(newline)|(_root\.CurrentPlayer)', expr), []
    for literal, nl, player in parts:
        out.append('\n' if nl else 'Mario' if player else bytes(literal, 'utf-8').decode('unicode_escape'))
    return ''.join(out) if parts else expr.strip()


def load_params(actions):
    """Top-level assignments in onClipEvent(load), comments removed."""
    params = {}
    for action in actions:
        m = re.search(r'onClipEvent \(load\) \{(.*?)\n\}', action, re.S)
        if not m:
            continue
        body = re.sub(r'/\*.*?\*/', '', m.group(1), flags=re.S)
        depth = 0
        for line in body.split('\n'):
            stripped = re.sub(r'//.*', '', line).strip()
            if depth == 0:
                a = re.match(r'([A-Za-z_][\w]*)\s*=\s*(.+?);?$', stripped)
                if a and 'function' not in a.group(2):
                    value = a.group(2).rstrip(';').strip()
                    params[a.group(1)] = evaluate(value) if '"' in value or value == 'newline' else value
                f = re.match(r'([A-Za-z_][\w]*)\s*=\s*function', stripped)
                if f:
                    params[f.group(1)] = 'function'
            depth += stripped.count('{') - stripped.count('}')
        if 'completefunction' in body:
            c = re.search(r'completefunction = function \(\) \{\s*(.*?)\s*\};', body, re.S)
            params['completefunction'] = c.group(1).strip() if c else 'function'
    if any('onClipEvent (enterFrame)' in a for a in actions):
        params['_enterFrame'] = 'true'
    return params


def symbol_for(linkage):
    for p in xfl.LIB.rglob('*.xml'):
        if f'linkageIdentifier="{linkage}"' in p.read_text(encoding='utf-8', errors='ignore')[:4000]:
            return p
    return None


def objects(area, layer):
    path = symbol_for(area + layer)
    if not path:
        return []
    sym = xfl.symbol(path)
    out = []
    for L in sym['layers']:
        if L['guide']:
            continue
        for f in L['frames']:
            for inst in f['symbols']:
                item = inst['item'].split('/')[-1].strip()
                if DECOR.search(item):
                    continue
                m = inst['m']
                out.append({'type': item, 'name': inst['name'] or '', 'layer': layer, 'x': round(m['tx'], 2),
                            'y': round(m['ty'], 2), 'sx': round(m['a'], 4), 'sy': round(m['d'], 4),
                            'b': round(m['b'], 4), 'c': round(m['c'], 4),
                            'frames': [f['index'], f['index'] + f['duration']],
                            'params': load_params(inst['actions'])})
    return out


def frame_scripts(area):
    """BackGFX timeline scripts (warps, area text, star checks) for reference."""
    path = symbol_for(area + 'BackGFX')
    sym = xfl.symbol(path)
    return [{'layer': L['name'], 'frame': f['index'], 'script': sc} for L in sym['layers'] for f in L['frames']
            for sc in f['scripts'] if not sc.lstrip().startswith('onClipEvent')]


def render(jar, swf_path, work, ids):
    cmd = ['java', '-Xmx2g', '-Djava.awt.headless=true', f'-Duser.home={work.resolve()}/ffdec-home', '-jar',
           str(jar.resolve()), '-onerror', 'abort', '-selectid', ','.join(map(str, ids))]
    with (work / 'export.log').open('a') as log:
        subprocess.run(cmd + ['-export', 'sprite', str(work / 'png'), str(swf_path)], check=True, stdout=log, stderr=log)
        subprocess.run(cmd + ['-select', ','.join(f'{i}:1' for i in ids), '-format', 'sprite:svg', '-export',
                              'sprite', str(work / 'svg'), str(swf_path)], check=True, stdout=log, stderr=log)


def sprite_dir(root, ident):
    hits = sorted(h for h in root.glob(f'DefineSprite_{ident}*')
                  if h.name == f'DefineSprite_{ident}' or h.name.startswith(f'DefineSprite_{ident}_'))
    if len(hits) != 1:
        raise ValueError(f'Expected one export directory for sprite {ident}')
    return hits[0]


def mask_rows(image):
    """Row runs of hitTest-solid pixels: [[(start, length), ...] per row]."""
    alpha = image.getchannel('A').point(lambda v: 255 if v >= 128 else 0).tobytes()
    w, rows = image.width, []
    for y in range(image.height):
        row, runs, x = alpha[y * w:(y + 1) * w], [], 0
        while x < w:
            if row[x]:
                s = x
                while x < w and row[x]:
                    x += 1
                runs.append((s, x - s))
            else:
                x += 1
        rows.append(runs)
    return rows


def pack_string(s):
    b = s.encode('utf-8')
    if len(b) > 65535:
        raise ValueError('String too long')
    return struct.pack('>H', len(b)) + b


def build(areas):
    out = bytearray(struct.pack('>8sII', b'SM63CRS1', VERSION, len(areas)))
    for a in areas:
        chunk = bytearray(pack_string(a['id']))
        chunk += struct.pack('>I', len(a['masks']))
        for m in a['masks']:
            chunk += struct.pack('>BBhiiII', MASK_LAYERS.index(m['layer']), m['frame'], 0, m['x0'], m['y0'],
                                 m['width'], m['height'])
            # Row offset table (bytes from the start of the run block), then per
            # row: u16 run count, runs of (u16 start, u16 length), sorted.
            block, offsets = bytearray(), []
            for runs in m['rows']:
                offsets.append(len(block))
                block += struct.pack('>H', len(runs))
                for s, n in runs:
                    block += struct.pack('>HH', s, n)
            chunk += struct.pack(f'>{len(offsets)}I', *offsets) + struct.pack('>I', len(block)) + block
        chunk += struct.pack('>I', len(a['water']))
        for w in a['water']:
            chunk += struct.pack('>4f', *w)
        chunk += struct.pack('>I', len(a['objects']))
        for o in a['objects']:
            chunk += pack_string(o['type']) + pack_string(o['name']) + pack_string(o['layer'])
            # Full placement matrix: x' = a*u + c*v + x, y' = b*u + d*v + y
            chunk += struct.pack('>6f2H', o['x'], o['y'], o['sx'], o['b'], o['c'], o['sy'], *o['frames'])
            chunk += struct.pack('>H', len(o['params']))
            for k, v in sorted(o['params'].items()):
                chunk += pack_string(k) + pack_string(str(v))
        out += struct.pack('>I', len(chunk)) + chunk
    out += bytes((-len(out)) % 32)
    return bytes(out)


def export(source, jar, output, area_ids):
    swf = (source / 'Publish/Super_Mario_63_2010.swf').read_bytes()
    if hashlib.sha256(swf).hexdigest() != SOURCE_SHA:
        raise ValueError('Unsupported SWF revision')
    xfl.LIB = source / 'Super_Mario_63_2010/LIBRARY'
    raw, start = S.body(swf)
    table, sprites = exports(raw, start), S.sprites(swf)
    work = output.parent / 'rendered-course'
    work.mkdir(parents=True, exist_ok=True)
    ids = {f'{a}{l}': table[f'{a}{l}'] for a in area_ids for l in MASK_LAYERS if f'{a}{l}' in table}
    render(jar, source / 'Publish/Super_Mario_63_2010.swf', work, sorted(set(ids.values())))
    areas, report = [], {}
    for a in area_ids:
        area = {'id': a, 'masks': [], 'water': [], 'objects': []}
        for layer in MASK_LAYERS:
            ident = ids.get(a + layer)
            if ident is None:
                continue
            svg = ET.parse(sprite_dir(work / 'svg', ident) / '1.svg').getroot()
            t = [float(v) for v in re.findall(r'[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?',
                                              svg.find('{http://www.w3.org/2000/svg}g').get('transform'))]
            if t[:4] != [1, 0, 0, 1]:
                raise ValueError('Unexpected registration')
            for frame in range(sprites[ident]['frames']):
                im = Image.open(sprite_dir(work / 'png', ident) / f'{frame + 1}.png').convert('RGBA')
                area['masks'].append({'layer': layer, 'frame': frame, 'x0': round(-t[4]), 'y0': round(-t[5]),
                                      'width': im.width, 'height': im.height, 'rows': mask_rows(im)})
        for layer in OBJECT_LAYERS:
            for o in objects(a, layer):
                if o['type'] == 'Water Code':
                    p = o['params']
                    area['water'].append((o['x'], o['y'], float(p.get('WaterLength', 500)),
                                          float(p.get('WaterDeepness', 400))))
                else:
                    area['objects'].append(o)
        areas.append(area)
        report[a] = {'masks': [{k: v for k, v in m.items() if k != 'rows'} for m in area['masks']],
                     'water': area['water'], 'objects': area['objects'], 'scripts': frame_scripts(a)}
    output.write_bytes(build(areas))
    (work / 'course.json').write_text(json.dumps(report, indent=1))
    return report


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('source', type=Path); p.add_argument('ffdec_jar', type=Path); p.add_argument('output', type=Path)
    p.add_argument('areas', nargs='+')
    a = p.parse_args()
    r = export(a.source, a.ffdec_jar, a.output, a.areas)
    for k, v in r.items():
        print(k, len(v['masks']), 'masks', len(v['water']), 'water', len(v['objects']), 'objects')
