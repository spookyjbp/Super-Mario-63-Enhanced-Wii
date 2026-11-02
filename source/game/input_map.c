#include "input_map.h"
#include <math.h>

float input_stick_axis(float value)
{
    const float magnitude = fabsf(value);
    if (magnitude <= 0.18f) return 0;
    const float scaled = fminf(1, (magnitude - 0.18f) / 0.82f);
    return value < 0 ? -scaled : scaled;
}
float input_map_zoom_stick(int stick_y)
{
    /* Wider dead zone than movement: a resting C-stick must never drift the zoom. */
    const float v = stick_y / 70.0f, m = fabsf(v);
    if (m <= 0.35f) return 0;
    const float scaled = fminf(1, (m - 0.35f) / 0.65f);
    return v > 0 ? -scaled : scaled;
}
unsigned input_active_controller(unsigned previous, bool gamecube_active, bool wpad_active, unsigned wpad_kind)
{
    if (wpad_active) return wpad_kind;
    if (gamecube_active) return CONTROLLER_GAMECUBE;
    return previous;
}
static void dpad(GameInput *s, bool up, bool down, bool left, bool right, bool up_pressed, bool down_pressed)
{
    s->horizontal = left && !right ? -1.0f : right && !left ? 1.0f : 0.0f;
    s->vertical = up && !down ? -1.0f : down && !up ? 1.0f : 0.0f; /* Positive is Down. */
    s->crouch_held = down;
    s->menu_delta = down_pressed ? 1 : up_pressed ? -1 : 0;
}
GameInput input_map_gamecube(unsigned held, unsigned down)
{
    GameInput state = {0};
    dpad(&state, held & GC_UP, held & GC_DOWN, held & GC_LEFT, held & GC_RIGHT, down & GC_UP, down & GC_DOWN);
    state.nozzle_held = (held & GC_R) != 0;
    state.nozzle_next_pressed = (down & GC_L) != 0;
    state.jump_pressed = (down & GC_A) != 0;
    state.jump_held = (held & GC_A) != 0;
    state.spin_pressed = (down & GC_B) != 0;
    state.spin_held = (held & GC_B) != 0;
    state.pound_pressed = (down & GC_Z) != 0;
    state.pound_held = (held & GC_Z) != 0;
    state.pause_pressed = (down & GC_START) != 0;
    state.confirm_pressed = state.jump_pressed;
    state.cancel_pressed = state.spin_pressed;
    return state;
}
GameInput input_map_classic(unsigned held, unsigned down, float right_stick_y)
{
    GameInput state = {0};
    dpad(&state, held & CC_UP, held & CC_DOWN, held & CC_LEFT, held & CC_RIGHT, down & CC_UP, down & CC_DOWN);
    state.jump_pressed = (down & CC_B) != 0;
    state.jump_held = (held & CC_B) != 0;
    state.spin_pressed = (down & CC_Y) != 0;
    state.spin_held = (held & CC_Y) != 0;
    state.pound_pressed = (down & (CC_L | CC_R)) != 0;
    state.pound_held = (held & (CC_L | CC_R)) != 0;
    state.nozzle_held = (held & CC_ZR) != 0;
    state.nozzle_next_pressed = (down & CC_ZL) != 0;
    state.pause_pressed = (down & (CC_PLUS | CC_HOME)) != 0;
    /* Right stick: up zooms in; the same wide dead zone as the C-stick. */
    const float m = fabsf(right_stick_y);
    if (m > 0.35f) state.zoom_axis = (right_stick_y > 0 ? -1.0f : 1.0f) * fminf(1, (m - 0.35f) / 0.65f);
    state.confirm_pressed = state.jump_pressed;
    state.cancel_pressed = state.spin_pressed;
    return state;
}
ChordOut input_chord_step(InputChord *c, bool hx, bool hy, bool imm_x, bool imm_y)
{
    ChordOut o = {0};
    const bool dx = hx && !c->was_x, dy = hy && !c->was_y;
    if (dx) c->age_x = 0; else if (hx && c->age_x < 255) ++c->age_x;
    if (dy) c->age_y = 0; else if (hy && c->age_y < 255) ++c->age_y;
    /* The second press within the window of the first (still held): the combo. */
    if (!c->chord && hx && hy && (dx || dy) && (dx ? c->age_y : c->age_x) < CHORD_WINDOW) {
        c->chord = true; o.both = true;
        c->pend_x = c->pend_y = false;
    }
    if (dx && !c->chord) { if (imm_x) c->single_x = true; else c->pend_x = true; }
    if (dy && !c->chord) { if (imm_y) c->single_y = true; else c->pend_y = true; }
    /* A waiting press becomes a single press when released or after the window. */
    if (c->pend_x && (!hx || c->age_x >= CHORD_WINDOW)) { c->pend_x = false; o.x_tap = true; c->single_x = hx; }
    if (c->pend_y && (!hy || c->age_y >= CHORD_WINDOW)) { c->pend_y = false; o.y_tap = true; c->single_y = hy; }
    if (!hx) c->single_x = false;
    if (!hy) c->single_y = false;
    if (!hx && !hy) c->chord = false;
    o.both_held = c->chord && hx && hy;
    o.x_hold = hx && c->single_x && !c->chord;
    o.y_hold = hy && c->single_y && !c->chord;
    c->was_x = hx; c->was_y = hy;
    return o;
}
GameInput input_map_wiimote(WiimoteState *w, unsigned held, unsigned down)
{
    GameInput state = {0};
    /* Sideways, D-pad on the left: the pad's Up points left, Right points up. */
    dpad(&state, held & WM_RIGHT, held & WM_LEFT, held & WM_UP, held & WM_DOWN, down & WM_RIGHT, down & WM_LEFT);
    state.jump_pressed = (down & WM_2) != 0;
    state.jump_held = (held & WM_2) != 0;
    state.spin_pressed = (down & WM_1) != 0;
    state.spin_held = (held & WM_1) != 0;
    /* A (nozzle, waits) + B (FLUDD, immediate): A+B stomp/read. */
    const ChordOut ab = input_chord_step(&w->ab, held & WM_A, held & WM_B, false, true);
    state.nozzle_next_pressed = ab.x_tap;
    state.nozzle_held = ab.y_hold;
    state.pound_pressed = ab.both;
    state.pound_held = ab.both_held;
    /* + (zoom in) and - (zoom out), both waiting: together pause. */
    const ChordOut pm = input_chord_step(&w->plus_minus, held & WM_PLUS, held & WM_MINUS, false, false);
    state.zoom_axis = pm.x_hold ? -1.0f : pm.y_hold ? 1.0f : 0.0f;
    state.pause_pressed = pm.both || (down & WM_HOME) != 0;
    state.confirm_pressed = state.jump_pressed;
    state.cancel_pressed = state.spin_pressed;
    return state;
}
