"""Linkage -> sprite ids and world-space bounds of course layers (read-only)."""
import os, sys, struct, json
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
import swf_states as S
swf = (Path(os.environ.get('SM63_SOURCE', '.')) / 'Publish/Super_Mario_63_2010.swf').read_bytes()
raw, start = S.body(swf)
exports, shapes = {}, {}
def rect(data, pos):
    b = S.Bits(data, pos); n = b.ub(5)
    x0, x1, y0, y1 = b.sb(n), b.sb(n), b.sb(n), b.sb(n)
    return (x0/20, y0/20, x1/20, y1/20)
for code, pos, length in S.tags(raw, start, len(raw)):
    if code == 56:
        count = struct.unpack_from('<H', raw, pos)[0]; q = pos + 2
        for _ in range(count):
            ident = struct.unpack_from('<H', raw, q)[0]; q += 2
            end = raw.index(0, q); exports[raw[q:end].decode('latin-1')] = ident; q = end + 1
    elif code in (2, 22, 32, 83):
        shapes[struct.unpack_from('<H', raw, pos)[0]] = rect(raw, pos + 2)
sprites = S.sprites(swf)
def bounds(ident, frame=0, depth=0):
    """Union of shape bounds placed in `ident` at `frame` (identity-ish matrices)."""
    if ident in shapes: return [shapes[ident]]
    if ident not in sprites or depth > 6: return []
    st = sprites[ident]['stage']
    out = []
    for d, e in (st[min(frame, len(st)-1)] if st else {}).items():
        m = e.get('matrix') or {'sx':1,'sy':1,'r0':0,'r1':0,'tx':0,'ty':0}
        for x0, y0, x1, y1 in bounds(e.get('id'), 0, depth+1):
            xs = [m['sx']*x + m['r1']*y + m['tx']/20 for x in (x0, x1) for y in (y0, y1)]
            ys = [m['r0']*x + m['sy']*y + m['ty']/20 for x in (x0, x1) for y in (y0, y1)]
            out.append((min(xs), min(ys), max(xs), max(ys)))
    return out
def union(rs):
    return [round(min(r[0] for r in rs),1), round(min(r[1] for r in rs),1), round(max(r[2] for r in rs),1), round(max(r[3] for r in rs),1)] if rs else None
if __name__ == '__main__':
    rep = {}
    for area in ['8-E1-1', '8-E1-1-2', '8-E1-2', '8-E1-2-2']:
        rep[area] = {}
        for layer in ['Platforms', 'BPlatforms', 'BackGFX', 'FrontGFX', 'Water', 'CamEdge', 'Edge', 'Ice', 'Puddle', 'Lava']:
            ident = exports.get(area + layer)
            if ident is None: rep[area][layer] = None; continue
            sp = sprites.get(ident, {})
            rep[area][layer] = {'id': ident, 'frames': sp.get('frames'), 'bounds': union(bounds(ident)),
                                'parts': len(bounds(ident))}
            if layer == 'Platforms' and sp.get('frames', 1) > 1:
                rep[area][layer]['bounds_frame2'] = union(bounds(ident, 1))
    for n in ['CastleColoredBG', 'CastleColoredFarBG', '8-E1-1CloseBG', '8-E1-2CloseBG', '8-E1-2-2CloseBG']:
        rep[n] = {'id': exports.get(n)}
    print(json.dumps(rep, indent=1))
