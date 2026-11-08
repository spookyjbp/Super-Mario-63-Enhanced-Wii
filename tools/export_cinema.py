#!/usr/bin/env python3
"""The original's two story cutscenes as data files (assets/data):

  cinema_intro.bin   the Intro (9814, main timeline frame "Intro"): a new
                     file's story before the tutorial, with its MP3 sound
                     stream; it ends with StartChar("8-E1-1", ...)
  cinema_intro2.bin  Intro Movie 2 (10108, frame "Intro2"): after the
                     tutorial's last pipe; the airship attack, Kamek and
                     Bowser, Eddie; it ends with StartChar("Castle", 80, 60...)

Each is played with the game's own scripts (tools/cinema_sim.py) and stored
as a timeline pack of its frames (tools/timeline_art.py) and what happens on
them. Big-endian:

  "SM63CIN1", u32 version 1, u32 frames, u32 events, u32 sounds,
  u32 events offset, u32 sounds offset, u32 pack offset, u32 pack size,
  f32 a b c d tx ty: where 627, the message box, is (its px -> stage px from
  the stage centre)
  events: u32 frame, u8 kind, u8 payload bytes, u16 0, payload (4-aligned)
    1 MESSAGE   u8 portrait (627's frame: 1 Toad, 2 Kamek, 3 Bowser, 4 Peach),
                then the text (Latin-1, the original's <b>/<i> tags kept);
                empty: the box fades out
    2 PRESS     the read button resumes the cutscene here (see cinema.h)
    3 MUSIC     u8 sound, u8 repeat sound (255: none), u8 loops (the repeat
                plays until stopped), u8 0, f32 offset s, f32 volume
    4 STOP      the music stops
    5 VOLUME    f32 volume (0..100)
    6 SFX       u8 sound, u8 0, u16 0, f32 volume
    7 SKIP      u32 frame START goes to from here on (0xFFFFFFFF: none)
    8 END       f32 x, f32 y, then the course id (StartChar)
  sounds: per sound u32 rate, u32 samples, u32 offset, u32 size (IMA ADPCM
          as tools/export_audio.py), u32 seek offset, u32 seeks: the decoder's
          state (s16 predictor, u8 index, u8 0) every SEEK samples, for music
          started part way (offsets from the file's start)
  then the streamed timeline pack ("SM63TLP3", timeline_art.write_stream):
  clips "cine" (the cutscene's frames) and "box" (627's frames 1-4, the
  message box with Toad's, Kamek's, Bowser's, Peach's picture, without its
  text and Z prompt: the port writes those)

    python export_cinema.py <checkout> <ffdec.jar> <scripts> [--data DIR] [--work DIR]

<scripts> is FFDec's script export of the SWF (ffdec -export script); it is
made there if missing.
"""
import argparse
import struct
import subprocess
from pathlib import Path

import numpy as np

import as2
import cinema_sim as C
import export_audio as A
import swf_states as S
import timeline_art as T

CINEMAS = {   # file: (sprite, the root's state when it starts)
    'cinema_intro.bin': (9814, {'TotalStars': 0.0}),
    'cinema_intro2.bin': (10108, {'TotalStars': 0.0, 'CheatCastleRestore': True}),
}
MESSAGE, PRESS, MUSIC, STOP, VOLUME, SFX, SKIP, END = range(1, 9)
NONE = 0xFFFFFFFF
SEEK = 32768
BOX, BOX_SKIP = 627, {184, 623}    # the box; its Z prompt (bz) and text field


def adpcm_seek(blob, samples):
    """The IMA decoder's (predictor, index) before every SEEK-th sample."""
    pred = struct.unpack_from('>h', blob, 0)[0]
    index, out = 0, bytearray()
    for i in range(samples):
        if i % SEEK == 0:
            out += struct.pack('>hBx', pred, index)
        byte = blob[4 + (i >> 1)]
        code = byte & 15 if i & 1 else byte >> 4
        step = A.STEPS[index]
        delta = step >> 3
        if code & 4: delta += step
        if code & 2: delta += step >> 1
        if code & 1: delta += step >> 2
        pred = max(-32768, min(32767, pred - delta if code & 8 else pred + delta))
        index = max(0, min(88, index + A.INDEX[code & 7]))
    return bytes(out)


def repeat_name(fn):
    """The song an onSoundComplete function attaches (the intro's repeat)."""
    if not isinstance(fn, as2.Function):
        return None
    found = []
    def walk(n):
        if isinstance(n, (tuple, list)):
            if isinstance(n, tuple) and n and n[0] == 'call' and n[1][0] == 'member' and n[1][2] == 'attachSound' \
                    and n[2] and n[2][0][0] == 'lit':
                found.append(n[2][0][1])
            for x in n:
                walk(x)
    walk(fn.body)
    return found[0] if found else None


LOOK = 8                        # cinema_render.c
SLOT = (256 * 256 * 4, 256 * 256 * 2)
SCRATCH = 21 * 1024 * 1024      # COURSE_MEM2_RESERVE (course_art.h)


def cache_bytes(pack):
    """The memory cinema_render.c's page cache takes for this pack: per
    format, slots for the most pages the frames [f, f + LOOK] use at once
    (its peaks()), plus the message box's pages, plus 2."""
    npages, nbakes, nclips, nframes, cf, nch, niv, mx = struct.unpack_from('>8I', pack, 8)
    q, pages = 40, []
    for _ in range(npages):
        w, h, n, f, _z, ivn, iv0 = struct.unpack_from('>HHIBBHI', pack, q)
        q += 16 + n; q += -q % 4
        pages.append((f, ivn, iv0))
    ivs = [struct.unpack_from('>II', pack, q + 8 * i) for i in range(niv)]
    q += 8 * niv + 24 * nbakes
    clips = {}
    for _ in range(nclips):
        name = pack[q:q + 16].rstrip(b'\0').decode(); clips[name] = struct.unpack_from('>II', pack, q + 16); q += 24
    b0, bn = clips.get('box', (nframes, 0))
    delta = [[0] * (nframes + LOOK + 2) for _ in range(2)]
    pinned = [0, 0]
    for f, ivn, iv0 in pages:
        f = 1 if f else 0
        if any(a <= b0 + bn - 1 and b >= b0 for a, b in ivs[iv0:iv0 + ivn]) and bn:
            pinned[f] += 1
            continue
        last = 0
        for a, b in ivs[iv0:iv0 + ivn]:
            fr, to = max(a - LOOK, 0, last), min(b + 1, nframes)
            if fr >= to:
                continue
            delta[f][fr] += 1; delta[f][to] -= 1; last = to
    total = 0
    for f in (0, 1):
        run = best = 0
        for x in delta[f][:nframes]:
            run += x; best = max(best, run)
        total += (best + pinned[f] + 2) * SLOT[f]
    return total, mx


def stream_pcm(raw, start, sprite):
    pcm, rate = A.stream_sound(raw, start, sprite)
    return pcm, rate


def build(tl, swf, scripts, jar, work, sprite, init):
    frames, events, labels, sim = C.record(tl, scripts, swf, sprite, init=init)
    if not sim.ended:
        raise SystemExit(f'{sprite}: the recording did not reach its StartChar in {len(frames)} frames')
    raw, start = S.body(swf)
    names, defined = A.linkage(raw, start), A.sounds(raw, start)
    sounds, index = [], {}

    def sound(key):
        if key not in index:
            if key == ('stream', sprite):
                pcm, rate = stream_pcm(raw, start, sprite)
            else:
                ident = names[key] if isinstance(key, str) else key
                fmt, rate, stereo, count, data = defined[ident]
                pcm = A.decode(fmt, rate, stereo, count, data)
            index[key] = len(sounds)
            sounds.append((rate, len(pcm), A.adpcm(pcm)))
        return index[key]

    out = []   # (frame, kind, payload)
    stream_started = False
    for ev in events:
        f, kind, *args = ev
        f = min(f, len(frames) - 1)
        if kind == 'message':
            text = as2.string(args[0]) if args[0] not in (as2.UNDEF, None) else ''
            portrait = as2.toint(sim.root.props.get('CinemaMessageFrame', 1.0))
            # the portrait in force when the message was set
            for g in reversed([e for e in events if e[1] == 'portrait' and e[0] <= ev[0]]):
                portrait = as2.toint(g[2]); break
            out.append((f, MESSAGE, bytes([portrait]) + text.encode('latin-1', 'replace')))
        elif kind == 'press':
            out.append((f, PRESS, b''))
        elif kind == 'sound_start':
            n, name, offset, loops = args
            obj = sim.sound_objects[n]
            rep = repeat_name(obj.props.get('onSoundComplete'))
            if name is None:
                continue
            if name not in names:
                raise ValueError(f'unknown sound {name}')
            if obj.target is sim.root.props.get('Mariosound') or name.startswith('cannon'):
                out.append((f, SFX, struct.pack('>B3xf', sound(name), obj.volume)))
                continue
            r = sound(rep) if rep else sound(name) if loops > 1 else 255
            out.append((f, MUSIC, struct.pack('>BBBxff', sound(name), r, 1 if (loops > 1 or rep) else 0,
                                               offset, obj.volume)))
        elif kind == 'sound_stop':
            out.append((f, STOP, b''))
        elif kind == 'sound_volume':
            out.append((f, VOLUME, struct.pack('>f', args[1])))
        elif kind == 'sound' and args[2] == 'event':
            out.append((f, SFX, struct.pack('>B3xf', sound(args[3]), 100.0)))
        elif kind == 'sound' and args[2] == 'stream' and not stream_started:
            stream_started = True
            out.append((f, MUSIC, struct.pack('>BBBxff', sound(('stream', sprite)), 255, 0, 0.0, 100.0)))
        elif kind == 'skip':
            target = labels.get(args[0], NONE) if args[0] else NONE
            out.append((f, SKIP, struct.pack('>I', target)))
        elif kind == 'start_char':
            course, x, y = args[0], as2.num(args[1]), as2.num(args[2])
            out.append((len(frames) - 1, END, struct.pack('>ff', x, y) + as2.string(course).encode()))
    out.sort(key=lambda e: e[0])   # stable: same-frame events keep their order
    box = [('box', BOX, range(4), None, None, BOX_SKIP)]
    pack, info = T.build_pack(tl, jar, work, swf, [('cine', T.Recorded(frames), None, None, None)] + box,
                              stream=T.StreamOptions())
    ev_bytes = bytearray()
    for f, kind, payload in out:
        ev_bytes += struct.pack('>IBBH', f, kind, len(payload), 0) + payload + b'\0' * (-len(payload) % 4)
    head = 64
    ev_off = head
    snd_off = ev_off + len(ev_bytes)
    body = bytearray()
    table = bytearray()
    data_off = snd_off + 24 * len(sounds)
    data_off += -data_off % 32
    for rate, count, blob in sounds:
        seek = adpcm_seek(blob, count)
        at = data_off + len(body)
        body += blob + b'\0' * (-len(blob) % 32)
        table += struct.pack('>6I', rate, count, at, len(blob), data_off + len(body), len(seek) // 4)
        body += seek + b'\0' * (-len(seek) % 32)
    pack_off = data_off + len(body)
    blob = bytearray(b'SM63CIN1')
    blob += struct.pack('>8I', 1, len(frames), len(out), len(sounds), ev_off, snd_off, pack_off, len(pack))
    blob += struct.pack('>6f', *(sim.box_at or (1, 0, 0, 1, 0, 100)))
    blob += ev_bytes + table
    blob += b'\0' * (data_off - len(blob))
    blob += body + pack
    cache, chunk = cache_bytes(pack)
    effects = {e[2][0] for e in out if e[1] == SFX}
    pcm = sum((sounds[i][1] * 2 + 31) // 32 * 32 for i in effects)
    need = 3 * 4096 * 2 + pcm + (chunk + 31) // 32 * 32 + cache
    if need > SCRATCH:
        raise SystemExit(f'{sprite}: the cutscene needs {need} bytes of MEM2, more than {SCRATCH}')
    info['mem2'] = need
    info.update({'events': len(out), 'sounds': len(sounds), 'sound bytes': len(body), 'labels': labels,
                 'missing calls': sim.interp.missing})
    return bytes(blob), info


def main():
    p = argparse.ArgumentParser()
    p.add_argument('source', type=Path); p.add_argument('ffdec_jar', type=Path); p.add_argument('scripts', type=Path)
    p.add_argument('--data', type=Path, default=Path(__file__).resolve().parent.parent / 'assets' / 'data')
    p.add_argument('--work', type=Path, default=Path(__file__).resolve().parent.parent / 'private-art' / 'cinema')
    p.add_argument('--only', help='one file name of CINEMAS')
    a = p.parse_args()
    swf_path = a.source / 'Publish/Super_Mario_63_2010.swf'
    swf = swf_path.read_bytes()
    if (a.scripts / 'scripts').is_dir():
        a.scripts = a.scripts / 'scripts'
    if not a.scripts.exists():
        subprocess.run(['java', '-Xmx3g', '-jar', str(a.ffdec_jar), '-export', 'script', str(a.scripts), str(swf_path)],
                       check=True)
    tl = T.parse(swf)
    scripts = C.Scripts(a.scripts)
    a.data.mkdir(parents=True, exist_ok=True)
    for name, (sprite, init) in CINEMAS.items():
        if a.only and a.only != name:
            continue
        data, info = build(tl, swf, scripts, a.ffdec_jar, a.work / name.split('.')[0], sprite, init)
        (a.data / name).write_bytes(data)
        print(name, len(data), info)


if __name__ == '__main__':
    main()
