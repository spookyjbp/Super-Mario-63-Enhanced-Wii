"""Minimal read-only XFL symbol parser (layers, frames, instances, scripts, text)."""
import re, sys, html
from pathlib import Path
LIB = None  # set by caller: <checkout>/Super_Mario_63_2010/LIBRARY
def find(name):
    hits = [p for p in LIB.rglob('*.xml') if p.stem == name]
    return hits
def parse_matrix(s):
    m = re.search(r'<Matrix([^>]*)/>', s or '')
    d = {'a':1.0,'b':0.0,'c':0.0,'d':1.0,'tx':0.0,'ty':0.0}
    if m:
        for k, v in re.findall(r'(\w+)="([-\d.e]+)"', m.group(1)): d[k] = float(v)
    return d
def symbol(path):
    s = path.read_text(encoding='utf-8')
    linkage = re.search(r'linkageIdentifier="([^"]+)"', s)
    out = {'path': str(path.relative_to(LIB)), 'linkage': html.unescape(linkage.group(1)) if linkage else None, 'layers': []}
    for lm in re.finditer(r'<DOMLayer name="([^"]*)"([^>]*)>(.*?)</DOMLayer>', s, re.S):
        layer = {'name': lm.group(1), 'guide': 'layerType="guide"' in lm.group(2), 'frames': []}
        for f in re.finditer(r'<DOMFrame index="(\d+)"(?: duration="(\d+)")?([^>]*)>(.*?)</DOMFrame>', lm.group(3), re.S):
            body = f.group(4)
            fr = {'index': int(f.group(1)), 'duration': int(f.group(2) or 1),
                  'label': (re.search(r' name="([^"]+)"', f.group(3)) or [None, None])[1],
                  'scripts': [html.unescape(x) for x in re.findall(r'<script><!\[CDATA\[(.*?)\]\]></script>', body, re.S)],
                  'symbols': [], 'bitmaps': [], 'shapes': len(re.findall(r'<DOMShape', body)), 'texts': []}
            for inst in re.finditer(r'<DOMSymbolInstance libraryItemName="([^"]+)"([^>]*)>(.*?)</DOMSymbolInstance>', body, re.S):
                nm = re.search(r' name="([^"]+)"', inst.group(2))
                acts = re.findall(r'<script><!\[CDATA\[(.*?)\]\]></script>', inst.group(3), re.S)
                fr['symbols'].append({'item': html.unescape(inst.group(1)).strip(), 'name': nm.group(1) if nm else None,
                                      'm': parse_matrix(inst.group(3)), 'actions': [html.unescape(a) for a in acts]})
            for b in re.finditer(r'<DOMBitmapInstance[^>]*libraryItemName="([^"]+)"[^>]*>(.*?)</DOMBitmapInstance>', body, re.S):
                fr['bitmaps'].append({'item': html.unescape(b.group(1)), 'm': parse_matrix(b.group(2))})
            for t in re.findall(r'<characters>(.*?)</characters>', body, re.S):
                fr['texts'].append(html.unescape(t))
            layer['frames'].append(fr)
        out['layers'].append(layer)
    return out
def summary(name, scripts=400):
    for p in find(name):
        sym = symbol(p)
        print(f"### {sym['path']}  linkage={sym['linkage']}")
        for L in sym['layers']:
            print(f"  layer '{L['name']}'{' (guide)' if L['guide'] else ''}: {len(L['frames'])} keyframes")
            for f in L['frames']:
                bits = []
                if f['label']: bits.append(f"label={f['label']}")
                if f['shapes']: bits.append(f"shapes={f['shapes']}")
                if f['bitmaps']: bits.append(f"bitmaps={len(f['bitmaps'])}")
                if f['symbols']: bits.append(f"symbols={len(f['symbols'])}")
                print(f"    frame {f['index']} dur {f['duration']} " + ' '.join(bits))
                for sy in f['symbols']:
                    m = sy['m']
                    print(f"      - {sy['item'].split('/')[-1].strip()} name={sy['name']} at ({m['tx']:.1f},{m['ty']:.1f}) scale=({m['a']:.2f},{m['d']:.2f})"
                          + (f" ACT:{sy['actions'][0][:scripts]!r}" if sy['actions'] else ''))
                for t in f['texts']: print(f"      TEXT: {t[:scripts]!r}")
                for sc in f['scripts']: print(f"      SCRIPT: {sc[:scripts]!r}")
if __name__ == '__main__':
    for n in sys.argv[1:]: summary(n)
