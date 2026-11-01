#include "input.h"

#include <gccore.h>
#include <math.h>
#include <wiiuse/wpad.h>

static float stick_axis(float value)
{
    if (fabsf(value) < 0.18f) return 0;
    return value < -1 ? -1 : value > 1 ? 1 : value;
}

void input_init(void)
{
    PAD_Init();
    WPAD_Init();
}

InputState input_poll(void)
{
    InputState state = {0};
    WPAD_ScanPads();
    PAD_ScanPads();

    /* Channel/port 1 is the player controller for this milestone. */
    const u32 held = WPAD_ButtonsHeld(0);
    const u32 down = WPAD_ButtonsDown(0);
    const u16 gc_held = PAD_ButtonsHeld(0);
    const u16 gc_down = PAD_ButtonsDown(0);
    state.horizontal = stick_axis(PAD_StickX(0) / 80.0f);

    u32 type;
    if (WPAD_Probe(0, &type) == WPAD_ERR_NONE) {
        expansion_t expansion;
        WPAD_Expansion(0, &expansion);
        float axis = 0;
        if (type == WPAD_EXP_NUNCHUK) {
            const joystick_t *stick = &expansion.nunchuk.js;
            axis = stick->mag * sinf(stick->ang * (3.14159265f / 180.0f));
        } else if (type == WPAD_EXP_CLASSIC) {
            const joystick_t *stick = &expansion.classic.ljs;
            axis = stick->mag * sinf(stick->ang * (3.14159265f / 180.0f));
        }
        if (fabsf(axis) > fabsf(state.horizontal)) state.horizontal = stick_axis(axis);
    }
    if ((held & (WPAD_BUTTON_LEFT | WPAD_CLASSIC_BUTTON_LEFT)) || (gc_held & PAD_BUTTON_LEFT))
        state.horizontal = -1;
    if ((held & (WPAD_BUTTON_RIGHT | WPAD_CLASSIC_BUTTON_RIGHT)) || (gc_held & PAD_BUTTON_RIGHT))
        state.horizontal = 1;
    state.jump_pressed = (down & (WPAD_BUTTON_A | WPAD_NUNCHUK_BUTTON_Z | WPAD_CLASSIC_BUTTON_A)) ||
                         (gc_down & PAD_BUTTON_A);
    state.exit_pressed = (down & (WPAD_BUTTON_HOME | WPAD_CLASSIC_BUTTON_HOME)) ||
                         (gc_down & PAD_BUTTON_START);
    return state;
}

void input_shutdown(void) { WPAD_Shutdown(); }
