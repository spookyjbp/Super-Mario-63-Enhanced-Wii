/* Plays the original tutorial sounds (assets/generated/sounds_8e1.bin, IMA
   ADPCM from tools/export_audio.py), decoded once into MEM2 as 16-bit PCM.
   Voice 0 is the course music (intro once, then the repeat looping, as
   StartChar's bgsong.onSoundComplete does), voice 1 the FLUDD water loop,
   voices 2-15 sound effects. */
#include "audio.h"
#include "cinema.h"
#include "sfx.h"
#include "sound_pack.h"
#include "sounds_8e1_bin.h"
#include <asndlib.h>
#include <gccore.h>
#include <stdint.h>
#include <string.h>

#define MUSIC_VOICE 0
#define FLUDD_VOICE 1

static SoundPack pack;
static int16_t *pcm[SOUND_PACK_MAX];
static unsigned bytes[SOUND_PACK_MAX];
static bool ready, fludd_on, intro_playing;
static int repeat_id = -1;
static SfxVoice voices[16];
static unsigned tick;

bool audio_init(void)
{
    if (!sound_pack_parse(sounds_8e1_bin, sounds_8e1_bin_size, &pack) || pack.count < SFX_COUNT) return false;
    size_t need = 0;
    for (unsigned i = 0; i < pack.count; ++i) need += ((size_t)pack.sound[i].samples * 2 + 31) & ~(size_t)31;
    unsigned char *lo = (unsigned char *)(((uintptr_t)SYS_GetArena2Lo() + 31) & ~(uintptr_t)31);
    if ((unsigned char *)SYS_GetArena2Hi() - lo < (ptrdiff_t)need) return false;
    SYS_SetArena2Lo(lo + need);
    for (unsigned i = 0; i < pack.count; ++i) {
        const unsigned size = ((unsigned)pack.sound[i].samples * 2 + 31) & ~31u;
        pcm[i] = (int16_t *)lo; bytes[i] = size;
        memset(lo, 0, size);
        sound_pack_decode(&pack.sound[i], pcm[i]);
        DCFlushRange(lo, size);
        lo += size;
    }
    ASND_Init();
    ASND_Pause(0);
    ready = true;
    return true;
}
static void volumes(unsigned volume, int pan, int *l, int *r)
{
    const int v = (int)(volume * 255 / 100);
    *l = pan > 0 ? v * (100 - pan) / 100 : v;
    *r = pan < 0 ? v * (100 + pan) / 100 : v;
}
static void start(int voice, unsigned id, unsigned volume, int pan, bool loop)
{
    int l, r;
    volumes(volume, pan, &l, &r);
    ASND_StopVoice(voice);
    if (loop) ASND_SetInfiniteVoice(voice, VOICE_MONO_16BIT, (int)pack.sound[id].rate, 0, pcm[id], (int)bytes[id], l, r);
    else ASND_SetVoice(voice, VOICE_MONO_16BIT, (int)pack.sound[id].rate, 0, pcm[id], (int)bytes[id], l, r, NULL);
}
void audio_update(bool paused)
{
    if (!ready) return;
    int intro, repeat;
    if (sfx_music_take(&intro, &repeat)) {
        ASND_StopVoice(MUSIC_VOICE);
        repeat_id = repeat;
        intro_playing = intro >= 0;
        if (intro >= 0) start(MUSIC_VOICE, (unsigned)intro, 100, 0, false);
        else if (repeat >= 0) start(MUSIC_VOICE, (unsigned)repeat, 100, 0, true);
    }
    if (intro_playing && ASND_StatusVoice(MUSIC_VOICE) == SND_UNUSED) { /* onSoundComplete */
        intro_playing = false;
        if (repeat_id >= 0) start(MUSIC_VOICE, (unsigned)repeat_id, 100, 0, true);
    }
    const bool fludd = sfx_fludd_loop() && !paused;
    if (fludd != fludd_on) {
        fludd_on = fludd;
        if (fludd) start(FLUDD_VOICE, SFX_FLUDD_LOOP, 67, 0, true); /* MarioVolume / 1.5 */
        else ASND_StopVoice(FLUDD_VOICE);
    }
    SfxEvent events[SFX_QUEUE];
    const unsigned n = sfx_drain(events);
    ++tick;
    for (int i = SFX_FIRST_VOICE; i <= SFX_LAST_VOICE; ++i)
        if (voices[i].busy && ASND_StatusVoice(i) == SND_UNUSED) voices[i].busy = false;
    for (unsigned i = 0; i < n; ++i) {
        /* Bug 11: ASND_GetFirstUnusedVoice() returned the idle music/FLUDD voices,
           so every effect went round-robin over busy voices, and identical coin
           jingles stacked at full volume until the mix clipped. */
        const int voice = sfx_pick_voice(voices, events[i].id, tick);
        if (voice < 0) continue;
        start(voice, events[i].id, events[i].volume, events[i].pan, false);
        voices[voice] = (SfxVoice){true, (int)events[i].id, tick};
    }
}
void audio_shutdown(void)
{
    if (!ready) return;
    ASND_End();
    ready = false;
}

/* ---- the story cutscenes (cinema.h) ---- */
#define STREAM_SAMPLES 4096u
#define STREAM_BUFFERS 3u
static const CinemaData *cin;
static int16_t *cin_pcm[CIN_MAX_SOUNDS];
static unsigned cin_bytes[CIN_MAX_SOUNDS];
static int16_t *stream_buf[STREAM_BUFFERS];
static unsigned stream_next_buf, music_serial, cin_tick;
static CinemaStream stream;
static bool stream_on;
static float stream_volume = -1;

size_t audio_cinema_begin(const CinemaData *d, unsigned char *mem, size_t size)
{
    if (!ready || !d) return 0;
    ASND_StopVoice(MUSIC_VOICE);
    ASND_StopVoice(FLUDD_VOICE);
    fludd_on = intro_playing = false;
    /* which sounds are effects (decoded now) and which music (streamed) */
    bool effect[CIN_MAX_SOUNDS] = {false};
    size_t at = 0;
    CinemaEvent e;
    for (unsigned i = 0; i < d->events; ++i) {
        at = cinema_event(d, at, &e);
        if (e.kind == CIN_SFX && e.size && e.payload[0] < d->sounds) effect[e.payload[0]] = true;
    }
    size_t used = 0;
    for (unsigned i = 0; i < STREAM_BUFFERS; ++i) {
        stream_buf[i] = (int16_t *)(mem + used);
        used += STREAM_SAMPLES * 2;
    }
    for (unsigned i = 0; i < d->sounds; ++i) {
        cin_pcm[i] = NULL;
        if (!effect[i]) continue;
        const unsigned bytes_ = ((unsigned)d->sound[i].samples * 2 + 31) & ~31u;
        if (used + bytes_ > size) return 0;
        cin_pcm[i] = (int16_t *)(mem + used); cin_bytes[i] = bytes_;
        memset(cin_pcm[i], 0, bytes_);
        const PackedSound ps = {d->sound[i].rate, d->sound[i].samples, d->sound[i].data, d->sound[i].size};
        sound_pack_decode(&ps, cin_pcm[i]);
        DCFlushRange(cin_pcm[i], bytes_);
        used += bytes_;
    }
    cin = d;
    stream_on = false;
    music_serial = 0;
    stream_volume = -1;
    return used;
}
static void stream_fill(int16_t *buf)
{
    cinema_stream_read(&stream, buf, STREAM_SAMPLES);
    DCFlushRange(buf, STREAM_SAMPLES * 2);
}
void audio_cinema_update(Cinema *p)
{
    if (!ready || !cin) return;
    const CinemaMusic *m = &p->music;
    int l, r;
    if (m->serial != music_serial) {
        music_serial = m->serial;
        ASND_StopVoice(MUSIC_VOICE);
        stream_on = false;
        if (m->sound >= 0) {
            cinema_stream_start(&stream, cin, m);
            stream_fill(stream_buf[0]);
            volumes((unsigned)(m->volume < 0 ? 0 : m->volume > 100 ? 100 : m->volume), 0, &l, &r);
            ASND_SetVoice(MUSIC_VOICE, VOICE_MONO_16BIT, (int)cinema_stream_rate(&stream), 0, stream_buf[0],
                          (int)(STREAM_SAMPLES * 2), l, r, NULL);
            stream_next_buf = 1;
            stream_on = true;
            stream_volume = m->volume;
        }
    }
    if (stream_on) {
        while (ASND_TestVoiceBufferReady(MUSIC_VOICE) == 1) {
            int16_t *b = stream_buf[stream_next_buf];
            stream_next_buf = (stream_next_buf + 1) % STREAM_BUFFERS;
            stream_fill(b);
            if (ASND_AddVoice(MUSIC_VOICE, b, (int)(STREAM_SAMPLES * 2)) != SND_OK) break;
        }
        if (m->volume != stream_volume) {
            stream_volume = m->volume;
            volumes((unsigned)(m->volume < 0 ? 0 : m->volume > 100 ? 100 : m->volume), 0, &l, &r);
            ASND_ChangeVolumeVoice(MUSIC_VOICE, l, r);
        }
    }
    unsigned ids[CIN_SFX_QUEUE];
    const unsigned n = cinema_take_sfx(p, ids, CIN_SFX_QUEUE);
    ++cin_tick;
    for (unsigned i = 0; i < n; ++i) {
        if (ids[i] >= cin->sounds || !cin_pcm[ids[i]]) continue;
        int voice = -1;
        for (int v = SFX_FIRST_VOICE; v <= SFX_LAST_VOICE; ++v)
            if (ASND_StatusVoice(v) == SND_UNUSED) { voice = v; break; }
        if (voice < 0) voice = SFX_FIRST_VOICE + (int)(cin_tick % (SFX_LAST_VOICE - SFX_FIRST_VOICE + 1));
        volumes(100, 0, &l, &r);
        ASND_StopVoice(voice);
        ASND_SetVoice(voice, VOICE_MONO_16BIT, (int)cin->sound[ids[i]].rate, 0, cin_pcm[ids[i]], (int)cin_bytes[ids[i]],
                      l, r, NULL);
    }
}
void audio_cinema_end(void)
{
    if (!ready) return;
    for (int v = 0; v <= SFX_LAST_VOICE; ++v) ASND_StopVoice(v);
    cin = NULL;
    stream_on = false;
}
