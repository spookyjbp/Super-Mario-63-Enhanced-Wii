#ifndef SM63_CINEMA_H
#define SM63_CINEMA_H
/* The original's story cutscenes (user request 2026-11-08), from the data
   files tools/export_cinema.py writes ("SM63CIN1"):
     cinema_intro.bin   the Intro (9814): a new file, before the tutorial
     cinema_intro2.bin  Intro Movie 2 (10108): after the tutorial's last pipe
   Each was played with the game's own scripts and recorded frame by frame
   (32 a second), with what happens on the frames: the cinema messages, where
   the read button resumes the cutscene, the music and sounds, where START
   skips to (the original's Skip button) and the course it ends in.

   The player (pure, host tested) replays that on the 60 Hz tick:
   - the message box is 627's script: it fades in (_alpha += (100 - _alpha)
     / 4), types 2 characters a frame once it is all in, the read button
     shows the whole text, the Z prompt (bz) fades in when it is out;
   - a PRESS event is where the recording pressed the read button (10 s
     after the text was out, the scene moving on meanwhile as in the
     original, where only the timeline waits): pressing it with the text out
     goes on from there at once; waiting longer replays the last 5 s of the
     wait (CIN_HOLD_LOOP) until the press;
   - START goes to the original's Skip target, with the music it would have
     there.
   The read button: the port's read button (the course signs' one) or jump. */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "game.h"
#include "timeline.h"
#ifdef __cplusplus
extern "C" {
#endif

enum { CIN_MESSAGE = 1, CIN_PRESS, CIN_MUSIC, CIN_STOP, CIN_VOLUME, CIN_SFX, CIN_SKIP, CIN_END };
#define CIN_MAX_SOUNDS 64
#define CIN_TEXT 400
#define CIN_SEEK_SAMPLES 32768u    /* a decoder state every so many samples */
/* A held message: the recording goes on CIN_HOLD frames (tools/cinema_sim.py
   HOLD) after the text is out before it presses; a player who waits longer
   sees the last CIN_HOLD_LOOP of them again, and again, until the press. */
#define CIN_HOLD_LOOP 160u

typedef struct {
    unsigned rate, samples;
    const uint8_t *data; size_t size;       /* IMA ADPCM as sound_pack.h */
    const uint8_t *seek; unsigned seeks;    /* (s16 predictor, u8 index, u8 0) per CIN_SEEK_SAMPLES */
} CinemaSound;
typedef struct CinemaData {
    unsigned frames, events, sounds;
    const uint8_t *event_data;
    CinemaSound sound[CIN_MAX_SOUNDS];
    const uint8_t *pack; size_t pack_size;
    float box[6];           /* where 627 (the message box) is on the stage: clip px -> stage px from the centre */
} CinemaData;
typedef struct {
    unsigned frame, kind, size;
    const uint8_t *payload;
} CinemaEvent;

bool cinema_parse(const uint8_t *data, size_t size, CinemaData *c);
/* Event i; returns the offset of the next one (0 at the end). */
size_t cinema_event(const CinemaData *c, size_t at, CinemaEvent *e);

/* What the audio side should do (audio.c plays it). */
typedef struct {
    int sound, repeat;          /* -1: none */
    float offset;               /* seconds into `sound` */
    float volume;               /* 0..100 */
    bool loop;                  /* the repeat loops */
    unsigned serial;            /* bumped on a (re)start */
} CinemaMusic;
#define CIN_SFX_QUEUE 8

typedef struct Cinema {
    const CinemaData *data;
    unsigned frame;             /* the recorded frame shown */
    unsigned phase;             /* 32 Hz frames inside the 60 Hz tick */
    size_t next;                /* the next event's offset */
    unsigned hold_at;           /* the next PRESS frame (0xFFFFFFFF: none) */
    unsigned skip_to;           /* where START goes (0xFFFFFFFF: nowhere) */
    /* 627 */
    char message[CIN_TEXT];     /* _root.CinemaMessage */
    unsigned length;            /* its length (the original counts the tags too) */
    unsigned portrait;          /* CinemaMessageFrame 1..4 */
    float alpha, prompt_alpha;  /* _alpha, bz._alpha */
    float prompt_target;        /* bz.aa */
    bool latched;               /* the read button, until the next 32 Hz frame */
    unsigned typed;             /* num */
    bool restart;               /* (unused) */
    bool on;                    /* CinemaMessage is not "" */
    bool finished;              /* CinemaMessageFinished */
    /* audio */
    CinemaMusic music;
    unsigned sfx[CIN_SFX_QUEUE]; unsigned sfx_count;
    /* the end */
    bool done;
    char course[32];
    float end_x, end_y;
} Cinema;

void cinema_start(Cinema *p, const CinemaData *d);
/* One 60 Hz tick. read: the read button went down; skip: START went down. */
void cinema_step(Cinema *p, bool read, bool skip);
/* The message as shown: the tags taken out, `typed` characters of the original
   (tags count, as in 627); *name_end: where the speaker's <b><i> name ends. */
unsigned cinema_visible_text(const Cinema *p, char *out, size_t cap, unsigned *name_end);
/* Sound effects started since the last call. */
unsigned cinema_take_sfx(Cinema *p, unsigned *out, unsigned cap);

/* The music decoded as it plays (the songs are too long to hold as PCM):
   `sound` from `offset` s, then `repeat` (looping when `loop`), as the
   original's bgsong.onSoundComplete does. */
typedef struct {
    const CinemaData *data;
    int sound, repeat;
    bool loop, ended;
    unsigned pos;              /* the next sample of `sound` */
    int pred, index;           /* the IMA decoder */
} CinemaStream;
void cinema_stream_start(CinemaStream *s, const CinemaData *d, const CinemaMusic *m);
/* n samples (silence once it has ended). */
void cinema_stream_read(CinemaStream *s, int16_t *out, unsigned n);
unsigned cinema_stream_rate(const CinemaStream *s);

#ifdef __cplusplus
}
#endif
#endif
