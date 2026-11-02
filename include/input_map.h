#ifndef SM63_INPUT_MAP_H
#define SM63_INPUT_MAP_H
#include "game.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Pure controller mappings (host tested); source/platform/input.c reads the
   hardware and passes these neutral bit sets. Movement is D-pad only on every
   controller (no analog movement).

   GameCube: D-pad move, A jump, B spin, Z stomp/read, R FLUDD, L nozzle,
             C-stick zoom, START pause.
   Classic:  D-pad move, b jump, y spin, L or R stomp/read, ZR FLUDD, ZL nozzle,
             right stick zoom, + (or HOME) pause.
   Wii Remote (sideways, D-pad on the left): pad move (Up on the pad = left),
             2 jump, 1 spin, B FLUDD, A nozzle, A+B together stomp/read,
             + zoom in, - zoom out, + and - together pause (HOME also pauses).
   Two-button combos: a press waits CHORD_WINDOW polls for its partner (A for
   the nozzle, + and - for zoom); B (FLUDD) starts at once and stops when A
   joins it for a stomp. */

/* libogc PAD bit values. platform/input.c static-asserts these against gccore.h
   so the host-tested mapping cannot drift from the real Wii constants. */
enum {
    GC_LEFT=0x0001, GC_RIGHT=0x0002, GC_DOWN=0x0004, GC_UP=0x0008,
    GC_Z=0x0010, GC_R=0x0020, GC_L=0x0040,
    GC_A=0x0100, GC_B=0x0200, GC_X=0x0400, GC_Y=0x0800, GC_START=0x1000
};
/* Neutral Classic Controller buttons (platform/input.c converts). */
enum {
    CC_UP=1u<<0, CC_DOWN=1u<<1, CC_LEFT=1u<<2, CC_RIGHT=1u<<3, CC_A=1u<<4, CC_B=1u<<5, CC_X=1u<<6, CC_Y=1u<<7,
    CC_L=1u<<8, CC_R=1u<<9, CC_ZL=1u<<10, CC_ZR=1u<<11, CC_PLUS=1u<<12, CC_MINUS=1u<<13, CC_HOME=1u<<14
};
/* Neutral Wii Remote buttons, as printed on the remote (platform/input.c converts). */
enum {
    WM_UP=1u<<0, WM_DOWN=1u<<1, WM_LEFT=1u<<2, WM_RIGHT=1u<<3, WM_A=1u<<4, WM_B=1u<<5, WM_1=1u<<6, WM_2=1u<<7,
    WM_PLUS=1u<<8, WM_MINUS=1u<<9, WM_HOME=1u<<10
};
#define CHORD_WINDOW 6  /* polls (~0.1 s at 60 Hz) for the second button of a combo */
/* Two buttons X and Y that are also a combo when pressed within CHORD_WINDOW. */
typedef struct { unsigned char age_x, age_y; bool was_x, was_y, pend_x, pend_y, single_x, single_y, chord; } InputChord;
typedef struct {
    bool x_tap, y_tap;    /* a single press of X / Y, once it is known not to be a combo */
    bool x_hold, y_hold;  /* X / Y held as a single button (immediate buttons from the press) */
    bool both;            /* the combo, once */
} ChordOut;
ChordOut input_chord_step(InputChord *c, bool held_x, bool held_y, bool immediate_x, bool immediate_y);
typedef struct { InputChord ab, plus_minus; } WiimoteState;

GameInput input_map_gamecube(unsigned held, unsigned down);
GameInput input_map_classic(unsigned held, unsigned down, float right_stick_y);
GameInput input_map_wiimote(WiimoteState *state, unsigned held, unsigned down);
float input_stick_axis(float value);
/* C-stick (raw -128..127) vertical axis as GameInput.zoom_axis: up zooms in (-1). */
float input_map_zoom_stick(int stick_y);
/* Which controller the player is using: the last one with any button or stick
   activity; `previous` while none moves. */
unsigned input_active_controller(unsigned previous, bool gamecube_active, bool wpad_active, unsigned wpad_kind);
#ifdef __cplusplus
}
#endif
#endif
