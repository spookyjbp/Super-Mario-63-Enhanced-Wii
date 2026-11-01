#include "input.h"
#include <gccore.h>
#include <math.h>
#include <wiiuse/wpad.h>

#include "input_map.h"

_Static_assert(GC_LEFT==PAD_BUTTON_LEFT && GC_RIGHT==PAD_BUTTON_RIGHT && GC_DOWN==PAD_BUTTON_DOWN &&
               GC_UP==PAD_BUTTON_UP && GC_Z==PAD_TRIGGER_Z && GC_R==PAD_TRIGGER_R && GC_L==PAD_TRIGGER_L &&
               GC_A==PAD_BUTTON_A && GC_B==PAD_BUTTON_B && GC_START==PAD_BUTTON_START,
               "GameCube mapping constants must match libogc");
void input_init(void) { PAD_Init(); WPAD_Init(); }
GameInput input_poll(void)
{
    WPAD_ScanPads();
    PAD_ScanPads();
    GameInput state = input_map_gamecube(PAD_ButtonsHeld(0), PAD_ButtonsDown(0), PAD_StickX(0), PAD_StickY(0));

    u32 type;
    if (WPAD_Probe(0, &type) == WPAD_ERR_NONE) {
        const u32 held = WPAD_ButtonsHeld(0), down = WPAD_ButtonsDown(0);
        const bool classic = type == WPAD_EXP_CLASSIC;
        const u32 left = classic ? WPAD_CLASSIC_BUTTON_LEFT : WPAD_BUTTON_LEFT;
        const u32 right = classic ? WPAD_CLASSIC_BUTTON_RIGHT : WPAD_BUTTON_RIGHT;
        const u32 up = classic ? WPAD_CLASSIC_BUTTON_UP : WPAD_BUTTON_UP;
        const u32 below = classic ? WPAD_CLASSIC_BUTTON_DOWN : WPAD_BUTTON_DOWN;
        const u32 jump = classic ? WPAD_CLASSIC_BUTTON_A : WPAD_BUTTON_A;
        const u32 spin = classic ? WPAD_CLASSIC_BUTTON_X :
            WPAD_BUTTON_B | (type == WPAD_EXP_NUNCHUK ? WPAD_NUNCHUK_BUTTON_C : 0);
        const u32 pound = classic ? WPAD_CLASSIC_BUTTON_ZR :
            (type == WPAD_EXP_NUNCHUK ? WPAD_NUNCHUK_BUTTON_Z : WPAD_BUTTON_DOWN);
        const u32 pause = classic ? WPAD_CLASSIC_BUTTON_PLUS | WPAD_CLASSIC_BUTTON_HOME :
            WPAD_BUTTON_PLUS | WPAD_BUTTON_HOME;
        const u32 nozzle = classic ? WPAD_CLASSIC_BUTTON_Y : WPAD_BUTTON_1;
        const u32 next_nozzle = classic ? WPAD_CLASSIC_BUTTON_ZL : WPAD_BUTTON_2;
        const u32 cancel = classic ? WPAD_CLASSIC_BUTTON_B : WPAD_BUTTON_B;
        expansion_t expansion;
        WPAD_Expansion(0, &expansion);
        const joystick_t *stick = classic ? &expansion.classic.ljs :
            type == WPAD_EXP_NUNCHUK ? &expansion.nunchuk.js : NULL;
        if (stick) {
            const float axis = input_stick_axis(stick->mag * sinf(stick->ang * (3.14159265f / 180)));
            if (fabsf(axis) > fabsf(state.horizontal)) state.horizontal = axis;
            const float vertical=-input_stick_axis(stick->mag*cosf(stick->ang*(3.14159265f/180)));
            if(fabsf(vertical)>fabsf(state.vertical)) state.vertical=vertical;
        }
        if(held & up) state.vertical=-1;
        if(held & below) state.vertical=1;
        state.crouch_held |= state.vertical>=0.5f; /* Same threshold as input_map.c. */
        state.nozzle_held |= (held & nozzle)!=0;
        state.nozzle_next_pressed |= (down & next_nozzle)!=0;
        if (held & left) state.horizontal = -1;
        if (held & right) state.horizontal = 1;
        state.jump_pressed |= (down & jump) != 0;
        state.jump_held |= (held & jump) != 0;
        state.spin_pressed |= (down & spin) != 0;
        state.spin_held |= (held & spin) != 0;
        state.pound_pressed |= (down & pound) != 0;
        state.pause_pressed |= (down & pause) != 0;
        state.confirm_pressed |= (down & jump) != 0;
        state.cancel_pressed |= (down & cancel) != 0;
        if (down & below) state.menu_delta = 1;
        if (down & up) state.menu_delta = -1;
    }
    return state;
}
void input_shutdown(void) { WPAD_Shutdown(); }
