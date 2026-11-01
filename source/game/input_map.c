#include "input_map.h"
#include <math.h>

float input_stick_axis(float value)
{
    const float magnitude = fabsf(value);
    if (magnitude <= 0.18f) return 0;
    const float scaled = fminf(1, (magnitude - 0.18f) / 0.82f);
    return value < 0 ? -scaled : scaled;
}
GameInput input_map_gamecube(unsigned held, unsigned down, int stick_x, int stick_y)
{
    GameInput state = {0};
    state.horizontal = input_stick_axis(stick_x / 80.0f);
    state.vertical = -input_stick_axis(stick_y / 80.0f); /* Positive is Down. */
    /* 0.5 (not 0.65) so a full down-diagonal on the GameCube octagon gate (~57,57)
       still counts as Down while holding away for a backflip. */
    state.crouch_held = state.vertical >= 0.5f || (held & GC_DOWN) != 0;
    if (held & GC_UP) state.vertical = -1;
    if (held & GC_DOWN) state.vertical = 1;
    state.nozzle_held = (held & GC_R) != 0;
    state.nozzle_next_pressed = (down & GC_L) != 0;
    state.jump_pressed = (down & GC_A) != 0;
    state.jump_held = (held & GC_A) != 0;
    state.spin_pressed = (down & GC_B) != 0;
    state.spin_held = (held & GC_B) != 0;
    state.pound_pressed = (down & GC_Z) != 0;
    state.pause_pressed = (down & GC_START) != 0;
    state.confirm_pressed = state.jump_pressed;
    state.cancel_pressed = state.spin_pressed;
    state.menu_delta = (down & GC_DOWN) ? 1 : (down & GC_UP) ? -1 : 0;
    if (held & GC_LEFT) state.horizontal = -1;
    if (held & GC_RIGHT) state.horizontal = 1;
    return state;
}
