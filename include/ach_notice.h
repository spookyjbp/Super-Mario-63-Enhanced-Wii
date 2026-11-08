#ifndef SM63_ACH_NOTICE_H
#define SM63_ACH_NOTICE_H
/* The "Achievement Unlocked!" card: slides in from the right edge, stays about
   five seconds, slides back out; queued unlocks follow one after another. The
   ping plays once as each card starts. Runs on the 60 Hz game tick and never
   pauses the game. */
#include "achievements.h"
#include <stdbool.h>

#define ACH_NOTICE_IN 21      /* ticks to slide in (0.35 s) */
#define ACH_NOTICE_HOLD 300   /* five seconds on screen */
#define ACH_NOTICE_OUT 21
#define ACH_NOTICE_GAP 12     /* between two cards */
#define ACH_NOTICE_TICKS (ACH_NOTICE_IN + ACH_NOTICE_HOLD + ACH_NOTICE_OUT + ACH_NOTICE_GAP)

typedef struct { unsigned id, t; } AchNotice;

/* Advances one tick; takes the next queued unlock when idle. Returns true on
   the tick a card starts (play the sound then). */
bool ach_notice_step(AchNotice *n, Achievements *a);
/* How far the card is out, 0 (hidden past the right edge) .. 1 (fully in), eased. */
float ach_notice_slide(const AchNotice *n);
#endif
