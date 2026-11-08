#!/usr/bin/env python3
"""Plays the original's scripted cutscenes (the Intro, 9814, and Intro
Movie 2, 10108) the way Flash Player does, with the game's own ActionScript
(tools/as2.py runs the scripts FFDec decompiles), and records every frame's
display list for the timeline pack (tools/export_cinema.py).

The scripts move the camera (9862 camControl), throw debris (10047), start
the music, set the cinema messages and wait for the read button; nothing of
that is in the timelines alone. The recording presses the read button the
way a player does (once the text is out and has been up for HOLD frames), and
notes each wait so the port can hold there until the player presses.

Flash Player's frame (AVM1), as modelled here:
  1. enterFrame: onClipEvent(enterFrame) and onEnterFrame of every clip
  2. every playing clip goes to its next frame (parents before children):
     its display list changes, new clips are made on their first frame
  3. the queued frame scripts run (a clip's first frame before its
     onClipEvent(load)); gotoAndPlay / gotoAndStop change the display list
     at once and queue the target frame's scripts
Scripts that set _x, _y, _xscale... take that property from the timeline, as
in Flash. Random numbers come from a fixed seed."""
import math
import random as _random
import re
import struct
from pathlib import Path

import as2
from as2 import UNDEF, Obj, Native, Function
import swf_states as S
import timeline_art as T

FPS = 32
STAGE_W, STAGE_H = 450, 300
TIMELINE_DEPTH = 16384          # SWF depth of script depth 0
HIDDEN = {627, 9791}            # drawn by the port itself: the message box, the Skip button
HOLD = 40                       # frames a finished message stays before the recording presses


# ---- the original's scripts ------------------------------------------------
class Scripts:
    """Frame scripts {(sprite, frame0): [program]} and clip events
    {(sprite, frame0, depth): {event: program}} from FFDec's script export."""

    def __init__(self, folder):
        self.frames, self.events = {}, {}
        self.cache = {}
        folder = Path(folder)
        for d in folder.iterdir():
            m = re.match(r'DefineSprite_(\d+)(?:_.*)?$', d.name)
            if not m:
                continue
            sprite = int(m.group(1))
            for fd in d.iterdir():
                fm = re.match(r'frame_(\d+)$', fd.name)
                if not fm:
                    continue
                f0 = int(fm.group(1)) - 1
                acts = sorted(fd.glob('DoAction*.as'), key=lambda p: (len(p.stem), p.stem))
                if acts:
                    self.frames[(sprite, f0)] = [p for p in acts]
                for po in fd.glob('PlaceObject*'):
                    depth = int(po.name.rsplit('_', 1)[1])
                    evs = {}
                    for ev in po.glob('CLIPACTIONRECORD*.as'):
                        src = ev.read_text(encoding='utf-8', errors='replace')
                        for em in re.finditer(r'onClipEvent\((\w+)\)\s*\{', src):
                            body = _braced(src, em.end() - 1)
                            evs[em.group(1)] = body
                    if evs:
                        self.events[(sprite, f0, depth)] = evs

    def program(self, key):
        """A parsed program for a script file path or event body."""
        if key not in self.cache:
            src = key.read_text(encoding='utf-8', errors='replace') if isinstance(key, Path) else key
            self.cache[key] = as2.parse(src)
        return self.cache[key]

    def frame(self, sprite, f0):
        return [self.program(p) for p in self.frames.get((sprite, f0), [])]

    def clip_events(self, sprite, f0, depth):
        return {k: self.program(v) for k, v in self.events.get((sprite, f0, depth), {}).items()}


def _braced(src, i):
    """The text inside the braces opening at src[i]."""
    n = 0
    for j in range(i, len(src)):
        if src[j] == '{':
            n += 1
        elif src[j] == '}':
            n -= 1
            if n == 0:
                return src[i + 1:j]
    raise as2.AS2Error('unbalanced clip event')


def sound_tags(swf):
    """{sprite: {frame0: [('event', sound id) | ('stream', ...)]}} and
    {export name: id} from the SWF."""
    raw, start = S.body(swf)
    sounds, exports = {}, {}
    for code, pos, length in S.tags(raw, start, len(raw)):
        if code == 56:      # ExportAssets
            n = struct.unpack_from('<H', raw, pos)[0]
            q = pos + 2
            for _ in range(n):
                ident = struct.unpack_from('<H', raw, q)[0]
                z = raw.index(0, q + 2)
                exports[raw[q + 2:z].decode('latin-1')] = ident
                q = z + 1
        if code != 39:
            continue
        ident = struct.unpack_from('<H', raw, pos)[0]
        frame, per = 0, {}
        for c, p, n in S.tags(raw, pos + 4, pos + length):
            if c == 15:
                per.setdefault(frame, []).append(('event', struct.unpack_from('<H', raw, p)[0], raw[p + 2]))
            elif c in (18, 45):
                per.setdefault(frame, []).append(('stream_head',))
            elif c == 19:
                per.setdefault(frame, []).append(('stream',))
            elif c == 1:
                frame += 1
        if per:
            sounds[ident] = per
    return sounds, exports


# ---- display objects ---------------------------------------------------------
def decompose(m):
    a, b, c, d, tx, ty = m
    return [tx, ty, math.hypot(a, b), math.hypot(c, d), math.atan2(b, a), math.atan2(-c, d)]


def compose(t):
    x, y, sx, sy, rx, ry = t
    return (sx * math.cos(rx), sx * math.sin(rx), -sy * math.sin(ry), sy * math.cos(ry), x, y)


class Leaf:
    """A shape, text or morph shape on a timeline."""
    def __init__(self, sim, ident, place):
        self.ident, self.place = ident, place
        self.born = place['born']
        self.kind = sim.tl.get(ident, {}).get('kind', 'other')
        self.dynamic = False
        self.removed = False

    def update(self, e):
        self.place = e

    def bounds(self, sim):
        return sim.tl.get(self.ident, {}).get('bounds')


class Clip(Obj):
    def __init__(self, sim, sprite, parent, depth, place=None, name=None):
        super().__init__()
        self.sim, self.sprite, self.parent, self.depth = sim, sprite, parent, depth
        self.place = place
        self.dynamic = place is None
        self.born = place['born'] if place else None
        self.matrix = place['matrix'] if place else T.IDENT
        self.cx = place['cx'] if place else T.NO_CX
        self.filters = place['filters'] if place else []
        self.mask = place['clip'] if place else 0
        self.iname = name or (place.get('name') if place else None) or f'instance{sim.next_instance()}'
        self.frame = 0
        self.playing = True
        self.children = {}
        self.visible = True
        self.tf = None             # [x, y, sx, sy, rx, ry] once a script moved it
        self.lock_cx = False
        self.handlers = {}
        self.removed = False
        self.created = sim.tick
        self.drawing = []
        sim.made(self)

    # -- the timeline
    def total(self):
        return len(self.sim.tl[self.sprite]['frames']) if self.sprite is not None else 1

    def set_frame(self, f):
        """Go to frame f (0-based): update the display list, queue its scripts."""
        self.frame = f
        if self.sprite is None:
            return
        sim = self.sim
        new = sim.tl[self.sprite]['frames'][f]
        for depth in list(self.children):
            ch = self.children[depth]
            if ch.dynamic:
                continue
            e = new.get(depth)
            if e is None or e['id'] != (ch.sprite if isinstance(ch, Clip) else ch.ident) or e['born'] != ch.born:
                sim.remove(ch)
                del self.children[depth]
        for depth in sorted(new):
            e = new[depth]
            ch = self.children.get(depth)
            if ch is not None and ch.dynamic:     # a script's clip there: the timeline cannot place
                continue
            if ch is not None:
                ch.update(e)
                continue
            self.children[depth] = sim.place(self, depth, e)
        for prog in sim.scripts.frame(self.sprite, f):
            sim.queue.append((prog, self))
        for kind, *args in sim.sounds.get(self.sprite, {}).get(f, []):
            sim.event('sound', self.sprite, f, kind, *args)

    def update(self, e):
        self.place = e
        if self.tf is None:
            self.matrix = e['matrix']
        if not self.lock_cx:
            self.cx = e['cx']
        self.filters = e['filters']
        if e.get('name'):
            self.iname = e['name']

    def advance(self):
        n = self.total()
        if n <= 1:
            return
        self.set_frame(self.frame + 1 if self.frame + 1 < n else 0)

    def goto(self, target, play):
        f = self.resolve(target)
        self.playing = play
        if f is None or f == self.frame:
            return
        self.set_frame(f)

    def resolve(self, target):
        if isinstance(target, str):
            labels = self.sim.tl[self.sprite]['labels'] if self.sprite is not None else {}
            if target in labels:
                return labels[target]
            try:
                target = float(target)
            except ValueError:
                return None
        f = as2.toint(target) - 1
        return max(0, min(self.total() - 1, f))

    # -- properties
    def transform(self):
        if self.tf is None:
            self.tf = decompose(self.matrix)
        return self.tf

    def set_tf(self, i, v):
        t = self.transform()
        t[i] = v
        self.matrix = compose(t)

    def bounds(self, sim):
        """This clip's bounds in its own coordinates (x0, y0, x1, y1) or None."""
        box = None
        for ch in self.children.values():
            b = ch.bounds(sim)
            if b is None:
                continue
            m = ch.matrix if isinstance(ch, Clip) else ch.place['matrix']
            pts = [T._apply(m, x, y) for x, y in ((b[0], b[1]), (b[2], b[1]), (b[2], b[3]), (b[0], b[3]))]
            xs, ys = [p[0] for p in pts], [p[1] for p in pts]
            nb = (min(xs), min(ys), max(xs), max(ys))
            box = nb if box is None else (min(box[0], nb[0]), min(box[1], nb[1]), max(box[2], nb[2]), max(box[3], nb[3]))
        return box

    def size(self, axis):
        b = self.bounds(self.sim)
        if b is None:
            return 0.0
        pts = [T._apply(self.matrix, x, y) for x, y in ((b[0], b[1]), (b[2], b[1]), (b[2], b[3]), (b[0], b[3]))]
        v = [p[axis] for p in pts]
        return max(v) - min(v)

    def get(self, name):
        low = name.lower()
        if low.startswith('_'):
            t = self.transform() if low in ('_xscale', '_yscale', '_rotation') else None
            if low == '_x': return self.matrix[4]
            if low == '_y': return self.matrix[5]
            if low == '_xscale': return t[2] * 100
            if low == '_yscale': return t[3] * 100
            if low == '_rotation':
                r = math.degrees(t[4])
                return r - 360 if r > 180 else r
            if low == '_alpha': return self.cx[3] * 100
            if low == '_visible': return self.visible
            if low == '_currentframe': return float(self.frame + 1)
            if low == '_totalframes': return float(self.total())
            if low == '_framesloaded': return float(self.total())
            if low == '_name': return self.iname
            if low == '_parent': return self.parent if self.parent is not None else UNDEF
            if low == '_width': return self.size(0)
            if low == '_height': return self.size(1)
            if low == '_target': return self.target()
            if low in ('_xmouse', '_ymouse'): return -10000.0
            if low == '_root': return self.sim.root
        m = self.sim.methods(self, name)
        if m is not None:
            return m
        for ch in self.children.values():
            if isinstance(ch, Clip) and ch.iname == name:
                return ch
        if name in self.props:
            return self.props[name]
        return self.sim.prototype.get(name)

    def set(self, name, value):
        low = name.lower()
        if low.startswith('_'):
            if low == '_x': self.set_tf(0, as2.num(value)); return
            if low == '_y': self.set_tf(1, as2.num(value)); return
            if low == '_xscale': self.set_tf(2, as2.num(value) / 100); return
            if low == '_yscale': self.set_tf(3, as2.num(value) / 100); return
            if low == '_rotation':
                t = self.transform()
                r = math.radians(as2.num(value))
                if r != r:
                    return
                d = r - t[4]
                t[4] += d; t[5] += d
                self.matrix = compose(t)
                return
            if low == '_alpha':
                v = as2.num(value)
                if v == v:
                    c = list(self.cx); c[3] = v / 100; self.cx = tuple(c); self.lock_cx = True
                return
            if low == '_visible': self.visible = as2.truth(value); return
            if low == '_width' or low == '_height':
                cur = self.size(0 if low == '_width' else 1)
                if cur > 0:
                    i = 2 if low == '_width' else 3
                    self.set_tf(i, self.transform()[i] * as2.num(value) / cur)
                return
            if low == '_name': self.iname = as2.string(value); return
        self.props[name] = value

    def target(self):
        if self.parent is None or not isinstance(self.parent, Clip):
            return '/' + self.iname
        return self.parent.target() + '/' + self.iname


# ---- host objects ------------------------------------------------------------
class SoundObj(Obj):
    def __init__(self, sim, target):
        super().__init__()
        self.sim, self.target, self.linked = sim, target, None
        self.volume = 100.0
        sim.sound_objects.append(self)
        self.n = len(sim.sound_objects) - 1

    def get(self, name):
        sim = self
        if name == 'attachSound':
            return Native(lambda this, n: setattr(self, 'linked', as2.string(n)))
        if name == 'start':
            return Native(lambda this, off=0.0, loops=1.0: self.sim.event('sound_start', self.n, self.linked,
                                                                         as2.num(off), as2.num(loops)))
        if name == 'stop':
            return Native(lambda this, *a: self.sim.event('sound_stop', self.n))
        if name == 'setVolume':
            def setv(this, v):
                self.volume = as2.num(v)
                self.sim.event('sound_volume', self.n, self.volume)
            return Native(setv)
        if name == 'getVolume':
            return Native(lambda this: self.volume)
        return super().get(name)


class ColorObj(Obj):
    def __init__(self, target):
        super().__init__()
        self.target = target

    def get(self, name):
        t = self.target
        if name == 'setTransform':
            def st(this, o):
                if not isinstance(t, Clip) or not isinstance(o, Obj):
                    return
                c = list(t.cx)
                for i, (m, a) in enumerate((('ra', 'rb'), ('ga', 'gb'), ('ba', 'bb'), ('aa', 'ab'))):
                    vm, va = o.get(m), o.get(a)
                    if vm is not UNDEF:
                        c[i] = as2.num(vm) / 100
                    if va is not UNDEF:
                        c[i + 4] = as2.num(va)
                t.cx = tuple(c); t.lock_cx = True
            return Native(st)
        if name == 'getTransform':
            def gt(this):
                c = t.cx if isinstance(t, Clip) else T.NO_CX
                return Obj({'ra': c[0] * 100, 'ga': c[1] * 100, 'ba': c[2] * 100, 'aa': c[3] * 100,
                            'rb': float(c[4]), 'gb': float(c[5]), 'bb': float(c[6]), 'ab': float(c[7])})
            return Native(gt)
        if name == 'setRGB':
            def rgb(this, v):
                if not isinstance(t, Clip):
                    return
                n = as2.toint(v)
                t.cx = (0, 0, 0, t.cx[3], (n >> 16) & 255, (n >> 8) & 255, n & 255, t.cx[7]); t.lock_cx = True
            return Native(rgb)
        return super().get(name)


class Root(Obj):
    """_root: the game's globals the cutscenes read and the calls they make."""
    def __init__(self, sim):
        super().__init__({'TotalStars': 0.0, 'PauseGame': False, 'BGquality': 'HIGH', 'MarioVolume': 100.0,
                          'CurrentPlayer': 'Mario', 'CheatCastleRestore': True, 'Fluddpow': ''})
        self.sim = sim

    def get(self, name):
        sim = self.sim
        if name == 'KeyZ':
            return Native(lambda this: sim.key_z())
        if name == 'StartChar':
            return Native(lambda this, *a: sim.event('start_char', *a))
        if name == 'gotoAndStop':
            return Native(lambda this, f: sim.event('root_goto', f))
        if name in ('CharCode', 'Restartcoins'):
            return Native(lambda this, *a: UNDEF)
        if name in ('_xmouse', '_ymouse'):
            return -10000.0
        if name == '_parent':
            return UNDEF
        return super().get(name)

    def set(self, name, value):
        if name in ('CinemaMessage', 'CinemaMessageFrame') and self.props.get(name) != value:
            self.sim.event('message' if name == 'CinemaMessage' else 'portrait', value)
            if name == 'CinemaMessage':
                self.sim.message_at = self.sim.tick
        super().set(name, value)


# ---- the player ----------------------------------------------------------------
class Sim:
    def __init__(self, tl, scripts, swf, seed=63):
        self.tl, self.scripts = tl, scripts
        self.sounds, self.exports = sound_tags(swf)
        self.rng = _random.Random(seed)
        self.tick = 0
        self.queue = []
        self.events = []
        self.sound_objects = []
        self.instances = 0
        self.all = []
        self.prototype = Obj()
        self.root = Root(self)
        self.root.set('bgsong', SoundObj(self, self.root))
        self.root.set('Mariosound', SoundObj(self, self.root))
        self.message_at = None
        self.box_at = None
        self.finished_for = 0
        self.press_now = False
        self.ended = False
        rnd = self.rng

        def math_round(this, v):
            x = as2.num(v)
            return math.floor(x + 0.5) if x == x else x
        M = Obj({
            'random': Native(lambda this: rnd.random()),
            'abs': Native(lambda this, v: abs(as2.num(v))),
            'round': Native(math_round),
            'floor': Native(lambda this, v: float(math.floor(as2.num(v))) if as2.num(v) == as2.num(v) else as2.num(v)),
            'ceil': Native(lambda this, v: float(math.ceil(as2.num(v))) if as2.num(v) == as2.num(v) else as2.num(v)),
            'sqrt': Native(lambda this, v: math.sqrt(as2.num(v)) if as2.num(v) >= 0 else float('nan')),
            'min': Native(lambda this, a, b: min(as2.num(a), as2.num(b))),
            'max': Native(lambda this, a, b: max(as2.num(a), as2.num(b))),
            'sin': Native(lambda this, v: math.sin(as2.num(v))),
            'cos': Native(lambda this, v: math.cos(as2.num(v))),
            'atan2': Native(lambda this, y, x: math.atan2(as2.num(y), as2.num(x))),
            'PI': math.pi,
        })
        self.interp = as2.Interp({
            '_root': self.root, 'Math': M,
            'random': Native(lambda this, n: float(math.floor(rnd.random() * as2.toint(n))) if as2.toint(n) > 0 else 0.0),
            'Stage': Obj({'width': float(STAGE_W), 'height': float(STAGE_H), 'scaleMode': 'showAll'}),
            'Sound': Native(lambda this, target=UNDEF: SoundObj(self, target)),
            'Color': Native(lambda this, target=UNDEF: ColorObj(target)),
            'Object': Native(lambda this, *a: Obj()),
            'MovieClip': Obj({'prototype': self.prototype}),
            'trace': Native(lambda this, *a: UNDEF),
            'getTimer': Native(lambda this: self.tick * 1000.0 / FPS),
        })

    # -- bookkeeping
    def next_instance(self):
        self.instances += 1
        return self.instances

    def made(self, clip):
        self.all.append(clip)

    def event(self, kind, *args):
        self.events.append((self.tick, kind) + args)
        if kind in ('start_char', 'root_goto'):
            self.ended = True
        return UNDEF

    def key_z(self):
        return self.press_now

    # -- making and removing
    def place(self, parent, depth, e):
        ident = e['id']
        if self.tl.get(ident, {}).get('kind') == 'sprite':
            return self.new_clip(ident, parent, depth, e)
        return Leaf(self, ident, e)

    def new_clip(self, sprite, parent, depth, e=None, name=None, init=None):
        c = Clip(self, sprite, parent, depth, e, name)
        if init is not None:
            for k, v in init.props.items():
                c.set(k, v)
        if sprite is not None and self.tl[sprite]['frames']:
            c.set_frame(0)
        if e is not None and isinstance(parent, Clip):
            evs = self.scripts.clip_events(parent.sprite, e['born'], depth)
            c.handlers = evs
            if 'load' in evs:
                self.queue.append((evs['load'], c))
        return c

    def remove(self, ch):
        ch.removed = True
        if isinstance(ch, Clip):
            for g in list(ch.children.values()):
                self.remove(g)

    # -- MovieClip methods
    def methods(self, c, name):
        sim = self
        if name == 'play':
            return Native(lambda this: setattr(c, 'playing', True))
        if name == 'stop':
            return Native(lambda this: setattr(c, 'playing', False))
        if name == 'gotoAndPlay':
            return Native(lambda this, f: c.goto(f, True))
        if name == 'gotoAndStop':
            return Native(lambda this, f: c.goto(f, False))
        if name == 'nextFrame':
            return Native(lambda this: c.goto(c.frame + 2, False))
        if name == 'prevFrame':
            return Native(lambda this: c.goto(c.frame, False))
        if name == 'getNextHighestDepth':
            def nhd(this):
                ds = [d - TIMELINE_DEPTH for d in c.children]
                return float(max(ds) + 1 if ds and max(ds) >= 0 else 0)
            return Native(nhd)
        if name == 'attachMovie':
            def attach(this, link, iname, depth, init=UNDEF):
                ident = sim.exports.get(as2.string(link))
                if ident is None:
                    raise as2.AS2Error(f'attachMovie: no export {link}')
                d = as2.toint(depth) + TIMELINE_DEPTH
                old = c.children.get(d)
                if old is not None:
                    sim.remove(old)
                ch = sim.new_clip(ident, c, d, None, as2.string(iname), init if isinstance(init, Obj) else None)
                c.children[d] = ch
                return ch
            return Native(attach)
        if name == 'createEmptyMovieClip':
            def empty(this, iname, depth):
                d = as2.toint(depth) + TIMELINE_DEPTH
                ch = sim.new_clip(None, c, d, None, as2.string(iname))
                c.children[d] = ch
                return ch
            return Native(empty)
        if name == 'removeMovieClip':
            def rm(this):
                p = c.parent
                if isinstance(p, Clip) and c.depth >= TIMELINE_DEPTH and p.children.get(c.depth) is c:
                    del p.children[c.depth]
                    sim.remove(c)
            return Native(rm)
        if name == 'swapDepths':
            def swap(this, other):
                p = c.parent
                if not isinstance(p, Clip):
                    return
                d = other.depth if isinstance(other, Clip) else as2.toint(other) + TIMELINE_DEPTH
                o = p.children.get(d)
                del p.children[c.depth]
                if o is not None:
                    p.children[c.depth] = o
                    o.depth = c.depth
                    o.dynamic = True
                p.children[d] = c
                c.depth = d
                c.dynamic = True      # out of the timeline's reach, as in Flash
            return Native(swap)
        if name == 'hitTest':
            return Native(lambda this, *a: False)
        if name in ('clear', 'beginFill', 'lineStyle', 'moveTo', 'lineTo', 'curveTo', 'endFill'):
            def draw(this, *a):
                if name == 'clear':
                    c.drawing = []
                else:
                    c.drawing.append((name,) + tuple(as2.num(v) for v in a))
            return Native(draw)
        return None

    # -- running
    def preorder(self, c, out):
        out.append(c)
        for d in sorted(c.children):
            ch = c.children[d]
            if isinstance(ch, Clip):
                self.preorder(ch, out)
        return out

    def run_queue(self):
        while self.queue:
            prog, c = self.queue.pop(0)
            if c.removed:
                continue
            self.interp.run(prog, c)

    def step(self, top):
        """One frame of Flash Player."""
        self.tick += 1
        for c in self.preorder(top, []):
            if c.removed:
                continue
            if 'enterFrame' in c.handlers:
                self.interp.run(c.handlers['enterFrame'], c)
            h = c.props.get('onEnterFrame', UNDEF)
            if isinstance(h, (Function, Native)):
                self.interp.call(h, c, [])
            self.run_queue()
        for c in self.preorder(top, []):
            if c.removed or c.created >= self.tick or not c.playing:
                continue
            c.advance()
        self.run_queue()

    # -- the recording's read button
    def update_press(self):
        """Press the read button once the message has been out HOLD frames."""
        self.press_now = False
        r = self.root
        if r.props.get('CinemaMessage', '') in ('', UNDEF):
            self.finished_for = 0
            return
        if r.props.get('CinemaMessageFinished') is True:
            self.finished_for += 1
        if self.finished_for >= HOLD:
            self.press_now = True
            self.finished_for = 0
            self.event('press')

    # -- the display list -> draws (as timeline_art.flatten)
    def flatten(self, c, matrix=T.IDENT, cx=T.NO_CX, path=(), out=None, filters=(), clips=()):
        if out is None:
            out = []
        if c.drawing:
            raise as2.AS2Error(f'drawing API on {c.target()} is not supported')
        masks = []
        for depth in sorted(c.children):
            ch = c.children[depth]
            p = path + (depth,)
            if isinstance(ch, Leaf):
                e = ch.place
                m, cc = T.mul(matrix, e['matrix']), T.cx_mul(cx, e['cx'])
                if e['clip']:
                    bx = self.tl.get(ch.ident, {}).get('bounds')
                    if bx is None:
                        raise ValueError(f'mask {ch.ident} is not a shape')
                    x0, y0, x1, y1 = bx
                    masks.append((e['clip'], [T._apply(m, x, y) for x, y in ((x0, y0), (x1, y0), (x1, y1), (x0, y1))]))
                    continue
                here = clips + tuple(poly for last, poly in masks if depth <= last)
                fl = tuple(filters) + tuple(e['filters'])
                ratio = round(e.get('ratio', 0) / 65535 * T.RATIO_STEPS) * 65535 // T.RATIO_STEPS \
                    if ch.kind == 'morph' else 0
                out.append((p, (ch.ident, ratio), m, cc, fl, here))
                continue
            if not ch.visible:
                continue
            if ch.sprite in HIDDEN:
                if ch.sprite == 627:     # where the port draws the message box
                    self.box_at = T.mul(matrix, ch.matrix)
                continue
            if ch.mask:
                raise ValueError(f'clip {ch.sprite} used as a mask')
            m, cc = T.mul(matrix, ch.matrix), T.cx_mul(cx, ch.cx)
            if cc[3] <= 0 and cc[7] <= 0:
                continue
            here = clips + tuple(poly for last, poly in masks if depth <= last)
            fl = tuple(filters) + tuple(ch.filters)
            blur_only = all(t == 1 for t, _ in fl)
            if fl and not blur_only and ch.sprite is not None and not T._single_child(self.tl, ch.sprite):
                out.append((p, (ch.sprite, ch.frame), m, cc, fl, here))
                self.whole.add((ch.sprite, ch.frame))
            else:
                self.flatten(ch, m, cc, p, out, fl, here)
        return out


def record(tl, scripts, swf, sprite, max_frames=20000, seed=63, init=None):
    """Plays `sprite` (a cutscene on the main timeline) until it starts a
    course. Returns (frames, events, labels_at, sim): frames = the flatten
    list per frame; events = [(frame, kind, ...)]; labels_at = {label: first
    frame the cutscene's timeline is at that label}."""
    sim = Sim(tl, scripts, swf, seed)
    sim.whole = set()
    if init:
        for k, v in init.items():
            sim.root.set(k, v)
    top = Clip(sim, None, None, 0, None, 'root')
    top.sprite = None
    movie = sim.new_clip(sprite, top, 1, {'id': sprite, 'born': 0, 'matrix': T.IDENT, 'cx': T.NO_CX,
                                          'filters': [], 'clip': 0})
    top.children[1] = movie
    sim.run_queue()
    frames, labels_at, last_skip = [], {}, None
    lab = {f: n for n, f in tl[sprite]['labels'].items()}
    while not sim.ended and len(frames) < max_frames:
        if movie.frame in lab and lab[movie.frame] not in labels_at:
            labels_at[lab[movie.frame]] = len(frames)
        frames.append(sim.flatten(movie))
        sim.update_press()
        sim.step(top)
        sk = skip_label(movie.props.get('Skip'))
        if sk != last_skip:
            sim.event('skip', sk)
            last_skip = sk
    if movie.frame in lab:      # the label it ended on (StartChar ran on that very frame)
        labels_at.setdefault(lab[movie.frame], len(frames) - 1)
    events = [(t, *rest) for t, *rest in sim.events]
    return frames, events, labels_at, sim


def skip_label(fn):
    """The label a Skip function goes to (they are all `gotoAndStop("x")`)."""
    if not isinstance(fn, Function):
        return None
    def walk(n):
        if isinstance(n, tuple):
            if n and n[0] == 'call' and n[1] == ('name', 'gotoAndStop') and n[2] and n[2][0][0] == 'lit':
                return n[2][0][1]
            for x in n:
                r = walk(x)
                if r is not None:
                    return r
        elif isinstance(n, list):
            for x in n:
                r = walk(x)
                if r is not None:
                    return r
        return None
    return walk(fn.body)
