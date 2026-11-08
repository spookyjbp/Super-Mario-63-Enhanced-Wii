#ifndef SM63_PAUSE_MENU_H
#define SM63_PAUSE_MENU_H
/* The original pause menu (795 / 794; tools/pause_art.py) run by a controller.
   Pure and host-tested; course_render.c draws it.

   795: the pause key plays it (794 rises 20 px and fades in, frames 2-15) and
   the game stops at once; the key again plays it backwards and the game runs
   again when it is back on frame 1. 794's buttons: a selected one eases to
   120 % scale (half the way each frame), a chosen one jumps to 140 % and the
   page-change clip (661, 13 frames) swaps the page on its frame 8, taking no
   input until its frame 13. The original runs at 32 frames a second; this
   counts its own 32 Hz frames inside the 60 Hz tick.

   The port lays it out for the TV and the D-pad (the original places four
   buttons for the mouse under each page): the panel spreads over the 16:9
   screen, the original buttons stand in a column on the left (Star Map,
   Fludd, Options, the port's Achievements, Exit) and the chosen page shows on
   the right. Up / Down move in the column; jump or Right opens the page and
   steps into its items; spin or Left goes back to the column; spin in the
   column goes back to PAUSED, then closes. */
#include "achievements.h"
#include "game.h"
#include <stdbool.h>

typedef enum {
    PAUSE_DEFAULT, PAUSE_STARMAP, PAUSE_FLUDD, PAUSE_OPTIONS, PAUSE_EXIT, PAUSE_REFRESH, PAUSE_ACHIEVEMENTS,
    PAUSE_PAGES
} PausePage;

/* Items: the column's buttons, then the pages' own items. */
typedef enum {
    PI_STARMAP, PI_FLUDD, PI_OPTIONS, PI_ACHIEVEMENTS, PI_EXIT,   /* the column, top to bottom */
    PI_NOZ_NONE, PI_NOZ_HOVER, PI_NOZ_TURBO, PI_NOZ_ROCKET,
    PI_MUTE, PI_QUALITY, PI_SPEED,            /* quality / speed: Flash-only settings, shown, not selectable */
    PI_EXIT_LEVEL, PI_QUIT,
    PI_LIST,                                  /* the achievements list */
    PI_COUNT
} PauseItem;
#define PI_COLUMN 5
typedef enum { PAUSE_ACT_NONE, PAUSE_ACT_QUIT, PAUSE_ACT_MUTE, PAUSE_ACT_NOZZLE } PauseAction;

#define PAUSE_OPEN_FRAMES 15     /* 795 frames 1..15 */
#define PAUSE_TRANS_FRAMES 13    /* 661 */
#define PAUSE_TRANS_SWAP 8
#define PAUSE_LIST_ROWS 5        /* achievements shown at once */

typedef struct {
    unsigned char frame;         /* 795's frame, 1 = closed */
    bool rewind;                 /* closing */
    unsigned char page, next;    /* PausePage */
    unsigned char trans;         /* 661's frame, 0 = not playing */
    signed char focus;           /* PauseItem, -1 none */
    signed char column;          /* the column button last used (its page is shown) */
    signed char next_focus;      /* focus once the page changes */
    float scale[PI_COUNT];       /* % */
    float alpha[PI_COUNT];       /* % (Options items) */
    unsigned char list_top, list_sel;   /* achievements list: first row shown, selected (0..63) */
    unsigned phase;              /* 32 Hz frame accumulator */
    unsigned clock;              /* 32 Hz frames while open (the spinning coin) */
    signed char held_dx, held_dy;
    unsigned char repeat;
    bool muted;                  /* _root.MuteBGMusic */
    unsigned char action, action_arg;   /* PauseAction for the caller, taken with pause_take_action */
} PauseMenu;

typedef struct {
    bool can_pause;              /* the game allows pausing now */
    bool has_fludd;              /* FLUDD worn ("None" otherwise) */
    unsigned nozzles;            /* bit (1 << Nozzle): owned */
    unsigned total_stars;        /* _root.TotalStars: Exit Level is off at 0 */
} PauseContext;

void pause_init(PauseMenu *m, bool muted);
bool pause_active(const PauseMenu *m);            /* the game is stopped */
/* One 60 Hz tick. */
void pause_step(PauseMenu *m, const GameInput *in, const PauseContext *ctx);
PauseAction pause_take_action(PauseMenu *m, unsigned *arg);
/* Which items the page has (for drawing and navigation). */
bool pause_page_has(unsigned page, PauseItem item);
/* 794 px position of a page item (the column is placed by the renderer). */
void pause_item_pos(unsigned page, PauseItem item, float *x, float *y);
/* The page a column button opens. */
unsigned pause_column_page(PauseItem item);
/* 795's look at its frame: 794's y offset (px) and alpha (0..1). */
void pause_clip_look(const PauseMenu *m, float *dy, float *alpha);
/* 661 over the panel at its frame: grey level 0..255 and alpha 0..1 (0: none). */
float pause_trans_look(const PauseMenu *m, unsigned char *grey);
#endif
