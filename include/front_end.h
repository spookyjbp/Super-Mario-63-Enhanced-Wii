#ifndef SM63_FRONT_END_H
#define SM63_FRONT_END_H
/* The game's opening, title screen and main menu (the original's main
   timeline frames 6-9), run on the 60 Hz tick with the original's 32 Hz
   frames counted inside it. Pure and host-tested; front_render.c draws it
   from the replayed timelines (front_end.bin, tools/export_front_end.py).

   Opening (9280): 341 frames; its frame 64 holds 65 more (stop(), then
   play() once a counter passes 64); its sound stream starts on frame 158;
   the last frame moves on to the title.
   Title (9339): synced to the title theme as the original does it (frame =
   music position / length x 1106); the port loops frames 82-337 with the
   theme's 2.58-10.58 s. START (9298) runs its script here: it grows in
   (scale 25 -> 100, alpha 0 -> 100 over 20 frames), then pulses (alpha
   75 + 30 sin, 0.03 a frame); pressed, it grows 5 % a frame while
   TitleScreenTransition (2137, 20 frames) plays over everything, and the
   menu follows. */
#include "game.h"
#include <stdbool.h>

typedef enum { FE_OPENING, FE_TITLE, FE_MENU, FE_PLAY } FrontState;

/* The main menu (9535): pages, and the items the port moves as their scripts do. */
typedef enum { MP_START, MP_STORY, MP_EXTRAS, MP_OPTIONS, MP_CREDITS, MP_ACHIEVEMENTS, MP_PAGES } MenuPage;
typedef enum {
    MI_STORY, MI_OPTIONS, MI_EXTRAS,                       /* Start */
    MI_FILE_A, MI_FILE_B, MI_FILE_C, MI_FILE_D, MI_TOPBACK, /* Story (BACK: the file menu's 'bach') */
    MI_PLAY, MI_COPY, MI_ERASE, MI_FILEBACK,               /* the file's side buttons */
    MI_ACHIEVEMENTS, MI_CREDITS, MI_BACK,                  /* Extras (Achievements in Level Designer's place) */
    MI_MUTE,                                               /* Options */
    MI_LIST,                                               /* Achievements page */
    MI_COUNT
} MenuItem;
enum { FM_NONE, FM_COPYING, FM_DELETING };
#define FE_MESSAGES 3
#define FE_FILES 4

/* A floating button (DefineSprite_9376's script, shared by the menu's
   buttons and file boxes): bob, sway, grow when chosen, spin when clicked. */
typedef struct { float scale, r, ywave, rwave, rspeed, scalespeed; bool cl; unsigned box_phase; } FloatItem;
typedef struct { bool on; float y, speed; int count; bool green; char text[48]; } MenuMessage;

typedef struct {
    unsigned char page;
    int trans_y; bool trans_on, trans_swapped; unsigned char trans_to;   /* MenuTransition (2108) */
    signed char focus;
    FloatItem item[MI_COUNT];
    int hl;                         /* filem.HL: the file chosen, -1 none */
    unsigned char filem;            /* the file menu's frame, 1..23 */
    unsigned char mode;             /* FM_* */
    /* MenuRedTrans (2127): Delete File? */
    bool red_on; unsigned char red_frame; float red_alpha; signed char red_choice; int red_file; bool red_yes;
    unsigned char erase[FE_FILES];  /* 9403 frame + 1 while a file's erase plays */
    /* MenuGreenTrans (2129): copying */
    bool green_on, green_back; float green_alpha; int copy_from, copy_to;
    /* MenuBlackTrans (2128): Play */
    bool black_on; float black_alpha;
    MenuMessage msg[FE_MESSAGES];
    float credits_scroll, credits_max;
    unsigned char ach_file, ach_top, ach_sel;
    /* the backdrop's scrolling layers (9373) */
    float far_x, mid_x, tree_x[3]; unsigned char tree_frame[3], tree_last[2];
    unsigned rnd;
    signed char held_dx, held_dy; unsigned char repeat;
} MenuState;

/* What the menu needs from a save file, and changes it makes (the caller
   saves when `changed` is set). */
typedef struct {
    unsigned shines[FE_FILES];      /* TotalStars of each file */
    bool muted;
    bool changed;
    int copy_from, copy_to;         /* a copy to make (-1 none) */
    int erase;                      /* a file to erase (-1 none) */
} MenuFiles;

#define FE_OPENING_FRAMES 341
#define FE_OPENING_HOLD_FRAME 63      /* 0-based: frame 64 */
#define FE_OPENING_HOLD 65
#define FE_OPENING_SOUND_FRAME 157    /* frame 158 */
#define FE_TITLE_FRAMES 340
#define FE_TITLE_START_FRAME 64       /* 9298 is placed on frame 65 */
#define FE_TITLE_OUT_FRAMES 20

typedef struct {
    unsigned char state;
    unsigned phase;                   /* 32 Hz frames inside the 60 Hz tick */
    unsigned frame, hold;             /* opening frame (0-based), frames held on 64 */
    unsigned title_ticks;             /* 60 Hz ticks since the title theme started */
    /* START (9298): its script's variables */
    float start_scale, start_alpha, start_a2, start_wave;
    unsigned start_anum;
    bool start_placed, start_clicked;
    unsigned out_frame;               /* TitleScreenTransition frame + 1, 0 = not playing */
    int play_slot;                    /* FE_PLAY: the save file chosen (0..3) */
    MenuState menu;
    MenuFiles files;
} FrontEnd;

void front_init(FrontEnd *f);
/* Straight to the main menu's Start page (Quit Game in the pause menu goes back to the title instead). */
void front_to_title(FrontEnd *f);
/* Item positions (9535 px) on the current page, for drawing and navigation. */
bool front_item_pos(const FrontEnd *f, MenuItem item, float *x, float *y);
bool front_item_shown(const FrontEnd *f, MenuItem item);
void front_step(FrontEnd *f, const GameInput *in);
/* The title's frame (0-based) at its music position. */
unsigned front_title_frame(const FrontEnd *f);
#endif
