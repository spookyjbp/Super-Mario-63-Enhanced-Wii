#include "game.h"
#include <math.h>
#include <string.h>

static float approach(float value, float target, float change)
{
    if (value < target) return fminf(value + change, target);
    return fmaxf(value - change, target);
}
static float decay(float value) { return fmaxf(0, value - GAME_DT); }
static bool overlap(float a, float length, float b, float other)
{
    return a < b + other && a + length > b;
}

void player_init(Player *p, float x, float y)
{
    memset(p, 0, sizeof(*p));
    p->x = p->previous_x = x;
    p->y = p->previous_y = y;
    p->animation = PLAYER_FALL;
}

/* Sweep each axis against every solid and choose the closest contact, independent
   of platform order. Sweeps prevent tunnelling even at ground-pound speed. */
static void move_horizontal(Player *p, const World *world)
{
    float dx = p->vx * GAME_DT;
    for (size_t i = 0; i < world->count; ++i) {
        const Rect r = world->solids[i];
        if (!overlap(p->y, PLAYER_HEIGHT, r.y, r.h)) continue;
        if (dx > 0 && p->x + PLAYER_WIDTH <= r.x)
            dx = fminf(dx, r.x - (p->x + PLAYER_WIDTH));
        else if (dx < 0 && p->x >= r.x + r.w)
            dx = fmaxf(dx, r.x + r.w - p->x);
    }
    float target = p->x + dx;
    target = fmaxf(0, fminf(target, world->width - PLAYER_WIDTH));
    if (fabsf(target - p->x - p->vx * GAME_DT) > 0.001f) p->vx = 0;
    p->x = target;
}

static bool move_vertical(Player *p, const World *world)
{
    float dy = p->vy * GAME_DT;
    bool landed = false;
    for (size_t i = 0; i < world->count; ++i) {
        const Rect r = world->solids[i];
        if (!overlap(p->x, PLAYER_WIDTH, r.x, r.w)) continue;
        if (dy >= 0 && p->y + PLAYER_HEIGHT <= r.y && p->y + PLAYER_HEIGHT + dy >= r.y) {
            dy = r.y - (p->y + PLAYER_HEIGHT);
            landed = true;
        } else if (dy < 0 && p->y >= r.y + r.h && p->y + dy <= r.y + r.h) {
            dy = r.y + r.h - p->y;
        }
    }
    if (p->y + dy < 0) dy = -p->y;
    if (landed || fabsf(dy - p->vy * GAME_DT) > 0.001f) p->vy = 0;
    p->y += dy;
    return landed;
}

void player_step(Player *p, const World *world, GameInput input)
{
    p->previous_x = p->x;
    p->previous_y = p->y;
    p->animation_time += GAME_DT;
    const bool was_grounded = p->grounded;
    p->coyote = was_grounded ? 0.1f : decay(p->coyote);
    p->chain_window = decay(p->chain_window);
    p->jump_buffer = input.jump_pressed ? 0.1f : decay(p->jump_buffer);
    p->spin_time = decay(p->spin_time);
    p->spin_cooldown = decay(p->spin_cooldown);
    p->landing_time = decay(p->landing_time);
    if (was_grounded && p->chain_window == 0) p->jump_stage = 0;

    const bool locked = p->pounding || p->landing_time > 0;
    if (!locked) {
        const float axis = fmaxf(-1, fminf(1, input.horizontal));
        const float target = axis * 280;
        float acceleration = was_grounded ? 1700 : 950;
        if (axis == 0) acceleration = was_grounded ? 2000 : 300;
        else if (p->vx * axis < 0) acceleration = was_grounded ? 2400 : 1200;
        p->vx = approach(p->vx, target, acceleration * GAME_DT);
        if (axis != 0) p->facing_left = axis < 0;

        if (p->jump_buffer > 0 && p->coyote > 0) {
            p->jump_stage = p->chain_window > 0 && p->jump_stage < 3 ? p->jump_stage + 1 : 1;
            static const float jumps[] = {0, -430, -510, -610};
            p->vy = jumps[p->jump_stage];
            p->grounded = false;
            p->coyote = p->jump_buffer = p->chain_window = 0;
            p->jump_cut = false;
        }
        if (input.spin_pressed && p->spin_cooldown == 0 &&
            (p->grounded || !p->air_spin_used)) {
            p->spin_time = 0.32f;
            p->spin_cooldown = 0.5f;
            if (!p->grounded) {
                p->air_spin_used = true;
                p->vy = fminf(p->vy, -100);
            }
        }
        if (input.pound_pressed && !p->grounded) {
            p->pounding = true;
            p->pound_windup = 0.12f;
            p->spin_time = 0;
            p->vx = p->vy = 0;
            p->jump_buffer = p->coyote = 0;
        }
    } else {
        p->vx = approach(p->vx, 0, 2400 * GAME_DT);
    }

    if (p->pounding) {
        if (p->pound_windup > 0) {
            p->pound_windup = decay(p->pound_windup);
            p->vy = 0;
        } else p->vy = fminf(1200, p->vy + 3600 * GAME_DT);
    } else {
        if (!input.jump_held && p->vy < 0 && !p->jump_cut) {
            p->vy *= 0.45f;
            p->jump_cut = true;
        }
        const float gravity = input.jump_held && p->vy < 0 ? 1050 : 1600;
        p->vy = fminf(800, p->vy + gravity * GAME_DT);
    }
    move_horizontal(p, world);
    p->grounded = move_vertical(p, world);
    if (p->grounded) {
        p->air_spin_used = false;
        if (!was_grounded) {
            p->chain_window = p->jump_stage > 0 ? 0.28f : 0;
            if (p->pounding) {
                p->landing_time = 0.18f;
                p->jump_stage = 0;
                p->chain_window = 0;
            }
        }
        p->pounding = false;
        p->pound_windup = 0;
    }
    if (p->landing_time > 0) p->animation = PLAYER_LAND;
    else if (p->pounding) p->animation = p->pound_windup > 0 ? PLAYER_POUND_WINDUP : PLAYER_POUND;
    else if (p->spin_time > 0) p->animation = PLAYER_SPIN;
    else if (!p->grounded) p->animation = p->vy < 0 ? PLAYER_RISE : PLAYER_FALL;
    else p->animation = fabsf(p->vx) > 8 ? PLAYER_RUN : PLAYER_IDLE;
}

/* Attack geometry is exposed for future targets; this playground has no enemies. */
bool player_attack_hitbox(const Player *p, Rect *hitbox)
{
    if (p->spin_time > 0) {
        *hitbox = (Rect){p->x - 18, p->y - 4, PLAYER_WIDTH + 36, PLAYER_HEIGHT + 8};
        return true;
    }
    if (p->pounding && p->pound_windup == 0) {
        *hitbox = (Rect){p->x, p->y + PLAYER_HEIGHT - 4, PLAYER_WIDTH, 8};
        return true;
    }
    return false;
}
