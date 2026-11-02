#include "input.h"
#include <gccore.h>
#include <math.h>
#include <stdlib.h>
#include <wiiuse/wpad.h>

#include "input_map.h"

_Static_assert(GC_LEFT==PAD_BUTTON_LEFT && GC_RIGHT==PAD_BUTTON_RIGHT && GC_DOWN==PAD_BUTTON_DOWN &&
               GC_UP==PAD_BUTTON_UP && GC_Z==PAD_TRIGGER_Z && GC_R==PAD_TRIGGER_R && GC_L==PAD_TRIGGER_L &&
               GC_A==PAD_BUTTON_A && GC_B==PAD_BUTTON_B && GC_START==PAD_BUTTON_START,
               "GameCube mapping constants must match libogc");
void input_init(void) { PAD_Init(); WPAD_Init(); }

static unsigned classic_bits(u32 b)
{
    static const struct { u32 wpad; unsigned cc; } map[] = {
        {WPAD_CLASSIC_BUTTON_UP, CC_UP}, {WPAD_CLASSIC_BUTTON_DOWN, CC_DOWN}, {WPAD_CLASSIC_BUTTON_LEFT, CC_LEFT},
        {WPAD_CLASSIC_BUTTON_RIGHT, CC_RIGHT}, {WPAD_CLASSIC_BUTTON_A, CC_A}, {WPAD_CLASSIC_BUTTON_B, CC_B},
        {WPAD_CLASSIC_BUTTON_X, CC_X}, {WPAD_CLASSIC_BUTTON_Y, CC_Y}, {WPAD_CLASSIC_BUTTON_FULL_L, CC_L},
        {WPAD_CLASSIC_BUTTON_FULL_R, CC_R}, {WPAD_CLASSIC_BUTTON_ZL, CC_ZL}, {WPAD_CLASSIC_BUTTON_ZR, CC_ZR},
        {WPAD_CLASSIC_BUTTON_PLUS, CC_PLUS}, {WPAD_CLASSIC_BUTTON_MINUS, CC_MINUS}, {WPAD_CLASSIC_BUTTON_HOME, CC_HOME}};
    unsigned out = 0;
    for (unsigned i = 0; i < sizeof(map) / sizeof(map[0]); ++i) if (b & map[i].wpad) out |= map[i].cc;
    return out;
}
static unsigned wiimote_bits(u32 b)
{
    static const struct { u32 wpad; unsigned wm; } map[] = {
        {WPAD_BUTTON_UP, WM_UP}, {WPAD_BUTTON_DOWN, WM_DOWN}, {WPAD_BUTTON_LEFT, WM_LEFT}, {WPAD_BUTTON_RIGHT, WM_RIGHT},
        {WPAD_BUTTON_A, WM_A}, {WPAD_BUTTON_B, WM_B}, {WPAD_BUTTON_1, WM_1}, {WPAD_BUTTON_2, WM_2},
        {WPAD_BUTTON_PLUS, WM_PLUS}, {WPAD_BUTTON_MINUS, WM_MINUS}, {WPAD_BUTTON_HOME, WM_HOME}};
    unsigned out = 0;
    for (unsigned i = 0; i < sizeof(map) / sizeof(map[0]); ++i) if (b & map[i].wpad) out |= map[i].wm;
    return out;
}
static void merge(GameInput *s, GameInput o)
{
    if (o.horizontal != 0) s->horizontal = o.horizontal;
    if (o.vertical != 0) s->vertical = o.vertical;
    if (o.menu_delta) s->menu_delta = o.menu_delta;
    if (fabsf(o.zoom_axis) > fabsf(s->zoom_axis)) s->zoom_axis = o.zoom_axis;
    s->crouch_held |= o.crouch_held; s->nozzle_held |= o.nozzle_held; s->nozzle_next_pressed |= o.nozzle_next_pressed;
    s->jump_pressed |= o.jump_pressed; s->jump_held |= o.jump_held; s->spin_pressed |= o.spin_pressed;
    s->spin_held |= o.spin_held; s->pound_pressed |= o.pound_pressed; s->pound_held |= o.pound_held; s->pause_pressed |= o.pause_pressed;
    s->confirm_pressed |= o.confirm_pressed; s->cancel_pressed |= o.cancel_pressed;
}
GameInput input_poll(void)
{
    WPAD_ScanPads();
    PAD_ScanPads();
    /* GameCube: D-pad movement only; C-stick zoom. */
    GameInput state = input_map_gamecube(PAD_ButtonsHeld(0), PAD_ButtonsDown(0));
    state.zoom_axis = input_map_zoom_stick(PAD_SubStickY(0));
    const bool gc_active = PAD_ButtonsHeld(0) != 0 || state.zoom_axis != 0;
    bool wpad_active = false;
    unsigned wpad_kind = CONTROLLER_WIIMOTE;
    static WiimoteState wiimote;

    u32 type;
    if (WPAD_Probe(0, &type) == WPAD_ERR_NONE) {
        const u32 held = WPAD_ButtonsHeld(0), down = WPAD_ButtonsDown(0);
        GameInput w;
        if (type == WPAD_EXP_CLASSIC) {
            expansion_t expansion;
            WPAD_Expansion(0, &expansion);
            const joystick_t *r = &expansion.classic.rjs;
            const float ry = r->mag * cosf(r->ang * (3.14159265f / 180)); /* up is positive */
            w = input_map_classic(classic_bits(held), classic_bits(down), ry);
            wpad_kind = CONTROLLER_CLASSIC;
            wpad_active = held != 0 || fabsf(ry) > 0.35f;
        } else {
            /* Wii Remote held sideways; an attached Nunchuk is ignored. */
            w = input_map_wiimote(&wiimote, wiimote_bits(held), wiimote_bits(down));
            wpad_active = (wiimote_bits(held)) != 0;
        }
        merge(&state, w);
    }
    static unsigned active = CONTROLLER_GAMECUBE;
    active = input_active_controller(active, gc_active, wpad_active, wpad_kind);
    state.controller = (unsigned char)active;
    return state;
}
void input_shutdown(void) { WPAD_Shutdown(); }
