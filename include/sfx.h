#ifndef SM63_SFX_H
#define SM63_SFX_H
/* Sound requests from the game simulation (pure, host-tested). The platform
   layer drains them every frame and plays the original sounds (see
   source/platform/audio.c and tools/export_audio.py). */
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Order matches SOUNDS in tools/export_audio.py (the pack's entry order). */
typedef enum {
    SFX_COIN, SFX_JUMP1, SFX_JUMP1B, SFX_JUMP2, SFX_TRIPLE, SFX_DIVE, SFX_STEP, SFX_STEP_WATER,
    SFX_STEP_WETWOOD, SFX_SPLASH, SFX_SWIM, SFX_FALL, SFX_DOH, SFX_UH, SFX_SQUISH, SFX_OHHH,
    SFX_GOOMBA_ATTENTION, SFX_GOOMBA_STOMPED, SFX_EXPLOSION, SFX_FUSE_LITE, SFX_FUSE_LOOP, SFX_FLIPPERS,
    SFX_BOXBREAK, SFX_NOZZLE_BOX, SFX_FLUDD_NOZZLE, SFX_FLUDD_LOOP, SFX_FLUDD_SWITCH,
    MUS_CASTLE_INTRO, MUS_CASTLE_REPEAT,
    SFX_COUNT
} SfxId;
typedef struct { SfxId id; unsigned char volume; signed char pan; } SfxEvent; /* volume 0..100, pan -100..100 */
#define SFX_QUEUE 32
void sfx_play(SfxId id, float volume, float pan);
/* Copy and clear pending requests; returns how many. */
unsigned sfx_drain(SfxEvent out[SFX_QUEUE]);
/* The FLUDD water loop (Char frame 1 `watersoundclip`, "fluddrepeat") while spraying. */
void sfx_set_fludd_loop(bool on);
bool sfx_fludd_loop(void);
/* Course music: _root[course + "MusicIntro"] then "Music" repeating; -1 = none. */
void sfx_music(int intro, int repeat);
bool sfx_music_take(int *intro, int *repeat);
/* Original object volume/pan from the camera distance (BombombCode `distancevolume`:
   100 - min(100, distance / 100 * falloff); pan = clamp((x - camX) / 2, +/-100)). */
void sfx_at(SfxId id, float x, float y, float cam_x, float cam_y, float falloff);
/* Effect voice allocation (audio.c asks; pure so it is host-tested). Voices
   FIRST..LAST are effects. A request for a sound already started on the last
   SFX_RETRIGGER_TICKS ticks is merged into it (two coins at once play one
   jingle); at most SFX_SAME_MAX copies of one sound play together (the oldest
   copy is replaced); otherwise a free voice, else the oldest effect. */
#define SFX_FIRST_VOICE 2
#define SFX_LAST_VOICE 15
#define SFX_RETRIGGER_TICKS 2
#define SFX_SAME_MAX 2 /* two CoinJingles peak at 2 x 13383 < 32767: no clipping */
typedef struct { bool busy; int id; unsigned started; } SfxVoice;
/* Returns the voice to (re)start, or -1 to drop the request. */
int sfx_pick_voice(const SfxVoice voices[16], SfxId id, unsigned now);
/* _root.StepSound: "Mario-Step", or another sound for `frames` source frames. */
void sfx_set_step(SfxId id, int frames);
void sfx_step_tick(bool puddle);
SfxId sfx_step_sound(void);
bool sfx_coin_flip(void);
#ifdef __cplusplus
}
#endif
#endif
