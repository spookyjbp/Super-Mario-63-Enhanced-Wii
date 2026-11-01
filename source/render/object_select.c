#include "object_select.h"
#include "object_art_ids.h"
#include <math.h>

static ObjectDraw base(const LiveObject *l, int clip, unsigned frame)
{
    return (ObjectDraw){.clip = clip, .frame = frame, .alpha = 100, .scale = 1,
                        .flip_x = l->src->a < 0, .flip_y = l->src->d < 0};
}
unsigned object_select(const LiveObject *l, ObjectDraw o[OBJECT_DRAW_MAX])
{
    const unsigned f = l->frame;
    switch (l->kind) {
    case LIVE_COIN:
        o[0] = l->collected ? base(l, OBJ_COIN_COLLECT, f < 13 ? f : 13) : base(l, OBJ_COIN_SPIN, f % 16);
        o[0].alpha = l->alpha;
        return 1;
    case LIVE_BLUE_COIN:
        o[0] = l->collected ? base(l, OBJ_BCOIN_COLLECT, f < 13 ? f : 13) : base(l, OBJ_BCOIN_SPIN, f % 8);
        return 1;
    case LIVE_SIGN:
        o[0] = base(l, OBJ_SIGN, 0);
        if (l->alpha < 1) return 1;
        o[1] = base(l, OBJ_SIGN_Z, f % 49); o[1].alpha = l->alpha;
        return 2;
    case LIVE_TOAD:
        o[0] = base(l, OBJ_TOAD_IDLE, (f / 4) % 30);
        if (l->alpha < 1) return 1;
        o[1] = base(l, OBJ_TOAD_BZ, (f / 3) % 17); o[1].alpha = l->alpha;
        return 2;
    case LIVE_BUTTERFLY: /* clip._y = sin(wave)*5; _xscale = +/-scale */
        o[0] = base(l, OBJ_BUTTERFLY, f % 18);
        o[0].dy = sinf(l->wave) * 5 * l->scale; o[0].scale = l->scale; o[0].flip_x = l->vx < 0;
        return 1;
    case LIVE_GOONIE:
        if (l->carrying) o[0] = base(l, OBJ_GOONIE_RIDE, f % 9);
        else if (l->state == 2) o[0] = base(l, OBJ_GOONIE_DIVE, f >= 1000 ? (f - 1000 < 3 ? f - 1000 + 1 : 3) : 0);
        else o[0] = base(l, OBJ_GOONIE_FLY, f % 18);
        o[0].flip_x = l->facing_left;
        return 1;
    case LIVE_LOG: o[0] = base(l, OBJ_LOG, 0); o[0].dx = l->home_x - l->x; o[0].dy = l->home_y - l->y; return 1;
    case LIVE_ARROW: o[0] = base(l, OBJ_ARROW, f % 30); o[0].alpha = l->alpha; return 1;
    case LIVE_GOOMBA:
        switch (l->frame_label) {
        case GOOMBA_JUMP: o[0] = base(l, OBJ_GOOMBA_JUMP, f % 4); break;
        case GOOMBA_WALKR: o[0] = base(l, OBJ_GOOMBA_WALKR, f % 8); break;
        case GOOMBA_WALKL: o[0] = base(l, OBJ_GOOMBA_WALKL, f % 8); break;
        case GOOMBA_SQUISH: o[0] = base(l, OBJ_GOOMBA_SQUISH, l->count < 12 ? (unsigned)l->count : 12); break;
        case GOOMBA_TUMBLE: o[0] = base(l, OBJ_GOOMBA_TUMBLE, l->powerhit ? 0 : 1); break;
        default: o[0] = base(l, OBJ_GOOMBA_IDLE, 0); break;
        }
        o[0].flip_x = o[0].flip_y = false; /* GoombaCode resets _xscale/_yscale to scale * 100 */
        return 1;
    case LIVE_CHEEP: /* clip.gotoAndStop(frame); art faces left at _xscale > 0 */
        switch (l->frame_label) {
        case CHEEP_CHASE: o[0] = base(l, OBJ_CHEEP_CHASE, f % 11); break;
        case CHEEP_TURN: o[0] = base(l, OBJ_CHEEP_TURN, f < 3 ? f : 3); break;
        case CHEEP_JUMP: o[0] = base(l, OBJ_CHEEP_JUMP, f < 23 ? f : 22); break;
        case CHEEP_SPIN: o[0] = base(l, OBJ_CHEEP_SPIN, 0); o[0].rotation = l->tumblerot; break;
        case CHEEP_GONE: o[0] = base(l, OBJ_CHEEP_GONE, f < 7 ? f : 7); break;
        default: o[0] = base(l, OBJ_CHEEP_SWIM, f % 19); break;
        }
        o[0].flip_x = !l->facing_left;
        if (l->frame_label != CHEEP_SPIN) o[0].rotation = l->facing_left ? l->rotation : -l->rotation;
        return 1;
    case LIVE_BOMB: { /* clip 3310: Walkleft, Walkright (mirrored), tumble, explode, explode2 */
        const unsigned dead = l->deadcount > 0 ? (unsigned)l->deadcount - 1 : 0;
        switch (l->state) {
        case BOMB_TUMBLE: o[0] = base(l, OBJ_BOMB_TUMBLE, 0); o[0].pivot_y = -4; o[0].rotation = l->rotation; break;
        case BOMB_EXPLODE: o[0] = base(l, OBJ_BOMB_EXPLODE, dead < 12 ? dead : 12); o[0].pivot_y = -10; o[0].rotation = l->rotation; break;
        case BOMB_EXPLODE2: o[0] = base(l, OBJ_BOMB_EXPLODE2, dead < 14 ? dead : 14); break;
        default: o[0] = base(l, OBJ_BOMB_WALK, f % 30); break;
        }
        o[0].flip_y = false;
        o[0].flip_x = l->state == BOMB_WALKR;
        if (l->exploding && l->state != BOMB_EXPLODE && l->state != BOMB_EXPLODE2) {
            /* Fuse colour transform: rb = t^2/75 + blink, ga = ba = 100 - t^2/100 + blink (%). */
            const float t = (float)l->timer2;
            const float blink = sinf(t * t * t / 700 * 0.01745329252f) * 25 + 25 + t / 6;
            const float gb = (100 - t * t / 100 + blink) / 100;
            o[0].cxform = true;
            o[0].cx_mul[0] = 1; o[0].cx_mul[1] = o[0].cx_mul[2] = gb < 0 ? 0 : gb > 2 ? 2 : gb;
            o[0].cx_add[0] = t * t / 75 + blink; o[0].cx_add[1] = o[0].cx_add[2] = 0;
        }
        return 1;
    }
    case LIVE_KOOPA: o[0] = base(l, OBJ_KOOPA_WALK, f % 24); return 1;
    case LIVE_BOX: o[0] = base(l, OBJ_BOX, 0); return 1;
    case LIVE_FLUDD_BOX: o[0] = base(l, OBJ_FLUDD_BOX, 0); return 1;
    case LIVE_BOTTLE: o[0] = base(l, OBJ_BOTTLE, f % 3); return 1;
    case LIVE_GREEN_PLAT: o[0] = base(l, OBJ_GREEN_PLAT, 0); return 1;
    case LIVE_TIP_LOG: o[0] = base(l, OBJ_TIP_LOG, 0); return 1;
    case LIVE_ROT_BLOCK: o[0] = base(l, OBJ_ROT_BLOCK, 0); return 1;
    default: return 0;
    }
}
