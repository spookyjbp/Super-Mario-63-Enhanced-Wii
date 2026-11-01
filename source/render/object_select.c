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
        { /* ButterflyCode frames 1-6: the clip's colour transform (mult / 256, add) */
            static const float mul[6][3] = {{0.5f, 0.898f, 1}, {0.898f, 1, 0.5f}, {1, 0.699f, 0.699f},
                                            {1, 0.699f, 1}, {0.5f, 1, 0.5f}, {1, 1, 1}};
            static const float add[6][3] = {{30, 30, 0}, {30, 30, 0}, {50, 0, 0}, {50, 0, 50}, {0, 20, 0}, {15, 15, 0}};
            const unsigned k = l->state >= 0 && l->state < 6 ? (unsigned)l->state : 5;
            o[0].cxform = true;
            for (unsigned i = 0; i < 3; ++i) { o[0].cx_mul[i] = mul[k][i]; o[0].cx_add[i] = add[k][i]; }
        }
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
    case LIVE_KOOPA: /* clip 3402 by label; KoopaCode sets _xscale = -scale while moving right */
        switch (l->frame_label) {
        case KOOPA_TURN: o[0] = base(l, OBJ_KOOPA_TURN, l->timer < 14 ? (unsigned)l->timer : 14);
            o[0].dx = -0.8571428571f * (float)l->timer * (l->dir > 0 ? -1.0f : 1.0f); break; /* 3377 _X -= 6/7 */
        case KOOPA_FLYING: o[0] = base(l, OBJ_KOOPA_FLY, f % 18); break;
        case KOOPA_SHELL: { const unsigned k = (unsigned)lroundf(l->p2); o[0] = base(l, OBJ_KOOPA_SHELL, k > 0 ? k - 1 : 0); break; }
        default: o[0] = base(l, OBJ_KOOPA_WALK, f % 24); break;
        }
        o[0].flip_x = l->dir > 0; o[0].flip_y = false;
        return 1;
    case LIVE_BOX: /* frame 3 intact, 4-25 breaking (the 'stuff' icon is "none") */
        o[0] = base(l, OBJ_BOX, l->state == 0 ? 2u : l->state == 1 ? (unsigned)(3 + l->timer < 24 ? 3 + l->timer : 24) : 24u);
        return 1;
    case LIVE_FLUDD_BOX:
        o[0] = base(l, OBJ_FLUDD_BOX, l->state == 0 ? 0u : l->state == 1 ? (unsigned)(l->timer < 11 ? l->timer : 11) : 11u);
        return 1;
    case LIVE_NOZZLE: o[0] = base(l, OBJ_HNOZZLE, 0); o[0].flip_x = o[0].flip_y = false; o[0].alpha = l->alpha; return 1;
    case LIVE_BOTTLE: {
        const unsigned k = l->state == 0 ? (unsigned)l->timer : l->state == 1 ? 2u : l->state == 2 ? (unsigned)l->timer : 17u;
        o[0] = base(l, OBJ_BOTTLE, k < 17 ? k : 17); o[0].alpha = l->alpha;
        return 1;
    }
    case LIVE_GREEN_PLAT: { /* Path dots every dotdistance degrees, then pl (frame = scale) */
        unsigned n = 0;
        const int step = l->timer > 0 ? l->timer : 20;
        if (step < 360)
            for (int i = 0; i < 360 && n < OBJECT_DRAW_MAX - 1; i += step) {
                const float r = (float)i * 0.01745329252f;
                o[n] = base(l, OBJ_PATH_DOT, 0); o[n].flip_x = o[n].flip_y = false;
                o[n].dx = -sinf(r) * l->p1; o[n].dy = cosf(r) * l->p1; ++n;
            }
        const unsigned k = l->p3 >= 3 ? 2u : l->p3 >= 2 ? 1u : 0u;
        o[n] = base(l, OBJ_GREEN_PL, k); o[n].flip_x = o[n].flip_y = false;
        o[n].dx = l->q0; o[n].dy = l->wave; ++n;
        return n;
    }
    case LIVE_TIP_LOG: { /* gotoAndStop(round(scale)); the log turns with LogPL */
        const int k = (int)lroundf(l->p0);
        o[0] = base(l, OBJ_TIP_LOG, k >= 1 && k <= 4 ? (unsigned)k - 1 : 0); o[0].rotation = l->rotation;
        return 1;
    }
    case LIVE_ROT_BLOCK: {
        o[0] = base(l, OBJ_ROT_BLOCK, l->p3 >= 2 ? 1u : 0u); o[0].flip_x = o[0].flip_y = false;
        o[0].scale = l->p2 / 100; o[0].rotation = l->rotation;
        return 1;
    }
    default: return 0;
    }
}
