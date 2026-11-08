#!/usr/bin/env python3
"""Export the tutorial's original sounds into one IMA ADPCM sound pack.

Each sound is the SWF's DefineSound (by its linkage name), decoded with
ffmpeg at its own sample rate (mono), trimmed to the DefineSound sample count,
and IMA ADPCM encoded (4 bits a sample). The Wii decodes the pack once into
16-bit PCM (source/platform/audio.c).

Pack (big-endian): "SM63SND1", u32 version 1, u32 count, then per sound
u32 rate, u32 samples, u32 offset, u32 size; each sound's data is s16 initial
predictor, u8 step index, u8 0, then nibbles (high nibble first).

    python export_audio.py <checkout> <out_dir>
"""
import argparse, hashlib, struct, subprocess, tempfile
from pathlib import Path
import numpy as np
import swf_states as S
import export_course as C

# Order matches SfxId in include/sfx.h.
SOUNDS = [
    'CoinJingle', 'Mario Jump-1', 'Mario Jump-1b', 'Mario Jump-2', 'Mario Tripple Jump', 'Mario-Dive',
    'Mario-Step', 'step - Water', 'step-wetwood', 'sound - Splash', 'sound - Water', 'Mario-Fall',
    'Mario - Doh', 'Mario - Uh', 'Mario-Squish', 'Mario - Ohhhhh', 'Goomba-Attention', 'Goomba-Stomped',
    'Explosion', 'fuse-lite', 'fuse-loop', 'flippers', 'boxbreak', 'Nozzle Box Break', 'Fludd Nozzle',
    'fluddrepeat', 'FluddSwitch', '*rocket-charge', '*rocket-blast', 'Mario - Mamamiamoan',
    'mario-drown', 'sound - Drain', 'Goomba-Die', 'Mario - Letsgo', 'Mario - Mmmm', 'Mario - Hmm',
    'Mario - Imtired', 'Mario - Yawn', 'Mario - Snore1', 'Mario - Snore2',
    'PrincessPeachesCastle-Intro', 'PrincessPeachesCastle-Repeat',
    '+achievement_ping',
    # the front end (tools/export_front_end.py): the opening's sound stream
    # (sprite 9280, MP3 blocks on its frames 158-341), the title theme as an
    # intro (0 - 10.576 s) and a loop (2.58 - 10.576 s: the original repeats
    # from 2.58 s; 10.576 s is where the music lines up with it again, an
    # 8 s phrase, and the title's frames 82-337), and the file select song
    '>9280', 'TitleScreenTheme@0:10.576', 'TitleScreenTheme@2.58:10.576',
    'FileSelectSong-Intro', 'FileSelectSong-Repeat',
    # the Castle Grounds: the 1-Up mushrooms, the waterfall (Sound FX Clip)
    '1up', 'waterfall',
]
# '+' sounds are the project's own WAV files in assets/audio/ (the achievement
# notification sound, chosen by the project owner): mono, resampled to
# CUSTOM_RATE, peak set to -9 dBFS like the synthesised ones.
AUDIO_DIR = Path(__file__).resolve().parent.parent / 'assets' / 'audio'


def wav_sound(name):
    import wave
    with wave.open(str(AUDIO_DIR / f'{name}.wav')) as w:
        ch, width, rate, n = w.getnchannels(), w.getsampwidth(), w.getframerate(), w.getnframes()
        raw = w.readframes(n)
    if width != 2:
        raise ValueError(f'{name}.wav: 16-bit PCM expected')
    a = np.frombuffer(raw, '<i2').astype(np.float64).reshape(-1, ch).mean(1)
    if rate != CUSTOM_RATE:   # linear resampling after a 2-tap average (rate 44100 -> 22050)
        if rate == 2 * CUSTOM_RATE:
            a = np.convolve(a, [0.25, 0.5, 0.25], 'same')[::2]
        else:
            t = np.arange(int(len(a) * CUSTOM_RATE / rate)) * rate / CUSTOM_RATE
            a = np.interp(t, np.arange(len(a)), a)
    peak = np.max(np.abs(a)) or 1
    return (a / peak * (32767 * 10 ** (-9 / 20))).astype(np.int32)
# '*' sounds are not in the original: the 2010 SWF's Rocket nozzle is silent
# (Rocket Fludd Water 1340/1349/1370 attach no sound; Char's "fluddrepeat" loop
# is Hover/Turbo only). They are synthesised here, deterministically, from
# filtered noise like the original water sounds, peaking at -9 dBFS.
CUSTOM_RATE = 22050


def synth(name):
    rng = np.random.default_rng(63)
    if name == 'rocket-charge':   # pressure building for the 15-frame charge (0.47 s)
        n = int(CUSTOM_RATE * 0.47)
        t = np.arange(n) / CUSTOM_RATE
        noise = rng.standard_normal(n)
        out, low = np.zeros(n), 0.0
        for i in range(n):        # one-pole low-pass whose cut-off rises with the charge
            a = 0.04 + 0.35 * (t[i] / t[-1]) ** 1.5
            low += a * (noise[i] - low)
            out[i] = low
        env = np.minimum(1, t / 0.05) * (0.35 + 0.65 * t / t[-1]) * np.minimum(1, (t[-1] - t) / 0.02 + 0.2)
        out *= env
    elif name == 'rocket-blast':  # the release: a short burst of water pressure (0.4 s)
        n = int(CUSTOM_RATE * 0.4)
        t = np.arange(n) / CUSTOM_RATE
        noise = rng.standard_normal(n)
        out, low, band = np.zeros(n), 0.0, 0.0
        for i in range(n):
            low += 0.45 * (noise[i] - low)
            band += 0.08 * (low - band)
            out[i] = low - 0.6 * band
        thump = np.sin(2 * np.pi * (90 - 50 * t / t[-1]) * t) * np.exp(-t / 0.06)
        out = out * np.exp(-t / 0.12) * np.minimum(1, t / 0.004) + 0.8 * thump
    else:
        raise ValueError(name)
    out = out / np.max(np.abs(out)) * (32767 * 10 ** (-9 / 20))
    return out.astype(np.int32)
RATES = [5512, 11025, 22050, 44100]
STEPS = [7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 50, 55, 60, 66, 73, 80,
         88, 97, 107, 118, 130, 143, 157, 173, 190, 209, 230, 253, 279, 307, 337, 371, 408, 449, 494, 544,
         598, 658, 724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749,
         3024, 3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487,
         12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767]
INDEX = [-1, -1, -1, -1, 2, 4, 6, 8]


def linkage(raw, start):
    names = {}
    for code, pos, length in S.tags(raw, start, len(raw)):
        if code == 56:
            count = struct.unpack_from('<H', raw, pos)[0]
            q = pos + 2
            for _ in range(count):
                ident = struct.unpack_from('<H', raw, q)[0]
                end = raw.index(b'\0', q + 2)
                names[raw[q + 2:end].decode('latin1')] = ident
                q = end + 1
    return names


def sounds(raw, start):
    out = {}
    for code, pos, length in S.tags(raw, start, len(raw)):
        if code == 14:
            ident, flags, samples = struct.unpack_from('<HBI', raw, pos)
            out[ident] = (flags >> 4, RATES[(flags >> 2) & 3], flags & 1, samples, raw[pos + 7:pos + length])
    return out


SWF_ADPCM_INDEX = {2: [-1, 2], 3: [-1, -1, 2, 4], 4: [-1, -1, -1, -1, 2, 4, 6, 8],
                   5: [-1, -1, -1, -1, -1, -1, -1, -1, 1, 2, 4, 6, 8, 10, 13, 16]}


def swf_adpcm(data, samples):
    """Flash's own ADPCM (DefineSound format 1, mono): packets of 4096
    samples, each a 16-bit first sample and 6-bit index, then codes."""
    b = S.Bits(data, 0)
    total = len(data) * 8
    def left():
        return total - (b.byte * 8 + b.bit)
    nbits = b.ub(2) + 2
    sign, k0, table = 1 << (nbits - 1), 1 << (nbits - 2), SWF_ADPCM_INDEX[nbits]
    out = []
    while len(out) < samples and left() >= 22:
        pred, index = b.sb(16), b.ub(6)
        out.append(pred)
        for _ in range(4095):
            if len(out) >= samples or left() < nbits:
                break
            code = b.ub(nbits)
            step, vp, k = STEPS[index], 0, k0
            while k:
                if code & k:
                    vp += step
                step >>= 1; k >>= 1
            vp += step
            pred = max(-32768, min(32767, pred - vp if code & sign else pred + vp))
            index = max(0, min(88, index + table[code & (sign - 1)]))
            out.append(pred)
    a = np.array(out, dtype=np.int32)
    return a[:samples] if len(a) >= samples else np.pad(a, (0, samples - len(a)))


def decode(fmt, rate, stereo, samples, data):
    if fmt == 1 and not stereo:
        return swf_adpcm(data, samples)
    if fmt != 2:
        raise ValueError('only MP3 and mono ADPCM sounds are supported')
    seek = struct.unpack_from('<h', data)[0]
    with tempfile.TemporaryDirectory() as tmp:
        mp3 = Path(tmp) / 's.mp3'
        mp3.write_bytes(data[2:])
        pcm = subprocess.run(['ffmpeg', '-v', 'error', '-i', str(mp3), '-f', 's16le', '-ac', '1', '-ar', str(rate), '-'],
                             check=True, capture_output=True).stdout
    a = np.frombuffer(pcm, dtype='<i2').astype(np.int32)
    a = a[max(seek, 0):]
    return a[:samples] if len(a) >= samples else np.pad(a, (0, samples - len(a)))


def adpcm(samples):
    pred, index = int(samples[0]) if len(samples) else 0, 0
    out = bytearray(struct.pack('>hBB', pred, 0, 0))
    nibbles = []
    for s in samples:
        step = STEPS[index]
        diff = int(s) - pred
        code = 0
        if diff < 0:
            code = 8; diff = -diff
        delta = step >> 3
        if diff >= step: code |= 4; diff -= step; delta += step
        step >>= 1
        if diff >= step: code |= 2; diff -= step; delta += step
        step >>= 1
        if diff >= step: code |= 1; delta += step
        pred = max(-32768, min(32767, pred - delta if code & 8 else pred + delta))
        index = max(0, min(88, index + INDEX[code & 7]))
        nibbles.append(code)
    if len(nibbles) & 1:
        nibbles.append(0)
    out.extend(bytes((nibbles[i] << 4) | nibbles[i + 1] for i in range(0, len(nibbles), 2)))
    return bytes(out)


def stream_sound(raw, start, sprite):
    """A sprite's MP3 sound stream (SoundStreamHead + SoundStreamBlocks), mono PCM at its rate."""
    head, mp3 = None, bytearray()
    for code, pos, length in S.tags(raw, start, len(raw)):
        if code == 39 and struct.unpack_from('<H', raw, pos)[0] == sprite:
            for c, p, n in S.tags(raw, pos + 4, pos + length):
                if c in (18, 45):
                    head = raw[p:p + n]
                elif c == 19 and n > 4:
                    mp3 += raw[p + 4:p + n]          # sample count, seek, then MP3 frames
    if head is None or (head[1] >> 4) != 2:
        raise ValueError(f'sprite {sprite}: no MP3 stream')
    rate = [5512, 11025, 22050, 44100][(head[1] >> 2) & 3]
    with tempfile.TemporaryDirectory() as tmp:
        f = Path(tmp) / 's.mp3'
        f.write_bytes(bytes(mp3))
        pcm = subprocess.run(['ffmpeg', '-v', 'error', '-i', str(f), '-f', 's16le', '-ac', '1', '-ar', str(rate), '-'],
                             check=True, capture_output=True).stdout
    return np.frombuffer(pcm, dtype='<i2').astype(np.int32), rate


def export(source, out_dir):
    swf = (source / 'Publish/Super_Mario_63_2010.swf').read_bytes()
    if hashlib.sha256(swf).hexdigest() != C.SOURCE_SHA:
        raise ValueError('Unsupported SWF revision')
    raw, start = S.body(swf)
    names, defined = linkage(raw, start), sounds(raw, start)
    entries, blobs = [], []
    for name in SOUNDS:
        if name[0] in '+*':
            pcm, rate = (wav_sound if name[0] == '+' else synth)(name[1:]), CUSTOM_RATE
            blob = adpcm(pcm)
            entries.append((rate, len(pcm), blob))
            continue
        if name[0] == '>':
            pcm, rate = stream_sound(raw, start, int(name[1:]))
            entries.append((rate, len(pcm), adpcm(pcm)))
            continue
        base, _, span = name.partition('@')
        fmt, rate, stereo, count, data = defined[names[base]]
        pcm = decode(fmt, rate, stereo, count, data)
        if span:   # 'name@from:to' seconds
            t0, t1 = (float(v) for v in span.split(':'))
            pcm = pcm[round(t0 * rate):round(t1 * rate)]
        blob = adpcm(pcm)
        entries.append((rate, len(pcm), blob))
    header = bytearray(b'SM63SND1' + struct.pack('>II', 1, len(entries)))
    offset = len(header) + 16 * len(entries)
    offset += (-offset) % 32
    body = bytearray()
    for rate, count, blob in entries:
        header += struct.pack('>4I', rate, count, offset + len(body), len(blob))
        body += blob + bytes((-len(blob)) % 32)
    header += bytes((-len(header)) % 32)
    out_dir.mkdir(parents=True, exist_ok=True)
    (out_dir / 'sounds_8e1.bin').write_bytes(bytes(header + body))
    return len(header) + len(body)


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('source', type=Path)
    p.add_argument('out_dir', type=Path)
    a = p.parse_args()
    print('Wrote sound pack,', export(a.source, a.out_dir), 'bytes')
