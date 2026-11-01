#ifndef SM63_INPUT_MAP_H
#define SM63_INPUT_MAP_H
#include "game.h"
#ifdef __cplusplus
extern "C" {
#endif
/* libogc PAD bit values. platform/input.c static-asserts these against gccore.h
   so the host-tested mapping cannot drift from the real Wii constants. */
enum {
    GC_LEFT=0x0001, GC_RIGHT=0x0002, GC_DOWN=0x0004, GC_UP=0x0008,
    GC_Z=0x0010, GC_R=0x0020, GC_L=0x0040,
    GC_A=0x0100, GC_B=0x0200, GC_X=0x0400, GC_Y=0x0800, GC_START=0x1000
};
float input_stick_axis(float value);
/* Pure GameCube/WaveBird mapping: buttons held/pressed this poll, raw stick (-128..127). */
GameInput input_map_gamecube(unsigned held, unsigned down, int stick_x, int stick_y);
#ifdef __cplusplus
}
#endif
#endif
