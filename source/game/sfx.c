#include "sfx.h"
#include <math.h>
#include <string.h>

static SfxEvent queue[SFX_QUEUE];
static unsigned queued;
static bool fludd_loop;
static int music_intro = -1, music_repeat = -1;
static bool music_pending;

void sfx_play(SfxId id, float volume, float pan)
{
    if (queued >= SFX_QUEUE || id >= SFX_COUNT || volume < 1) return;
    volume = fminf(100, volume); pan = fmaxf(-100, fminf(100, pan));
    queue[queued++] = (SfxEvent){id, (unsigned char)volume, (signed char)pan};
}
unsigned sfx_drain(SfxEvent out[SFX_QUEUE])
{
    const unsigned n = queued;
    memcpy(out, queue, n * sizeof(SfxEvent));
    queued = 0;
    return n;
}
void sfx_set_fludd_loop(bool on) { fludd_loop = on; }
bool sfx_fludd_loop(void) { return fludd_loop; }
void sfx_music(int intro, int repeat) { music_intro = intro; music_repeat = repeat; music_pending = true; }
/* Pause menu Options "Mute Music": _root.MuteBGMusic = true calls StopBGsong();
   false calls PlayMusicAndIntro(), the course song again from its intro. */
static bool music_muted;
void sfx_set_music_muted(bool muted)
{
    if (muted == music_muted) return;
    music_muted = muted;
    music_pending = true;   /* stop it, or start the current song over */
}
bool sfx_music_muted(void) { return music_muted; }
bool sfx_music_take(int *intro, int *repeat)
{
    if (!music_pending) return false;
    *intro = music_muted ? -1 : music_intro; *repeat = music_muted ? -1 : music_repeat; music_pending = false;
    return true;
}
void sfx_at(SfxId id, float x, float y, float cam_x, float cam_y, float falloff)
{
    const float d = sqrtf((cam_x - x) * (cam_x - x) + (cam_y - y) * (cam_y - y));
    const float volume = 100 - fminf(100, d / 100 * falloff);
    sfx_play(id, volume, (x - cam_x) / 2);
}

static int step_override = -1, step_frames;
void sfx_set_step(SfxId id, int frames) { step_override = (int)id; step_frames = frames; } /* _root.changestep */
void sfx_step_tick(bool puddle)
{
    /* CharCode: `if(!Metal && Puddle == false && changestep-- < 0) StepSound = defaultstep`;
       a puddle sets "step - Water". */
    if (puddle) { step_override = SFX_STEP_WATER; step_frames = 0; return; }
    if (step_frames-- < 0) step_override = -1;
}
SfxId sfx_step_sound(void) { return step_override >= 0 ? (SfxId)step_override : SFX_STEP; }
static unsigned flip = 0x6d2b79f5u;
bool sfx_coin_flip(void) { flip ^= flip << 13; flip ^= flip >> 17; flip ^= flip << 5; return flip & 1; } /* random(2) */

int sfx_pick_voice(const SfxVoice v[16], SfxId id, unsigned now)
{
    int same = 0, oldest_same = -1, free_voice = -1, oldest = -1;
    for (int i = SFX_FIRST_VOICE; i <= SFX_LAST_VOICE; ++i) {
        if (!v[i].busy) { if (free_voice < 0) free_voice = i; continue; }
        if (oldest < 0 || v[i].started < v[oldest].started) oldest = i;
        if (v[i].id != (int)id) continue;
        if (now - v[i].started < SFX_RETRIGGER_TICKS) return -1;         /* merge a simultaneous retrigger */
        ++same;
        if (oldest_same < 0 || v[i].started < v[oldest_same].started) oldest_same = i;
    }
    if (same >= SFX_SAME_MAX) return oldest_same;                     /* cap stacked copies */
    return free_voice >= 0 ? free_voice : oldest;
}

static SfxId ambient_id = SFX_COUNT;
static float ambient_volume, ambient_pan;
void sfx_set_ambient(SfxId id, float volume, float pan) { ambient_id = id; ambient_volume = volume; ambient_pan = pan; }
void sfx_ambient(SfxId *id, float *volume, float *pan) { *id = ambient_id; *volume = ambient_volume; *pan = ambient_pan; }
