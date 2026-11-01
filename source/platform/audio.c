/* Plays the original tutorial sounds (assets/generated/sounds_8e1.bin, IMA
   ADPCM from tools/export_audio.py), decoded once into MEM2 as 16-bit PCM.
   Voice 0 is the course music (intro once, then the repeat looping, as
   StartChar's bgsong.onSoundComplete does), voice 1 the FLUDD water loop,
   voices 2-15 sound effects. */
#include "audio.h"
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
