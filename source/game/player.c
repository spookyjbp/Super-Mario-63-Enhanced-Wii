#include "game.h"
#include "source_movement.h"
#include <math.h>
#include <string.h>

#define POUND_WINDUP_FRAMES 12u /* Ground Pound.xml frames 0-11. */
#define POUND_HIT_FRAME 27u
#define POUND_END_FRAME 42u

static float approach(float value, float target, float change)
{
    if (value < target) return fminf(value + change, target);
    return fmaxf(value - change, target);
}
static float decay(float value) { return fmaxf(0, value - GAME_DT); }
void player_init(Player *p, float x, float y)
{
    memset(p, 0, sizeof(*p));
    p->x = p->previous_x = x;
    p->y = p->previous_y = y;
    p->animation = PLAYER_FALL;
    p->source_phase=28;
    p->height=PLAYER_HEIGHT; p->water=100; p->power=100; p->nozzle=NOZZLE_HOVER; p->has_fludd=true;
}

void player_step(Player *p, const World *world, GameInput input)
{
    p->previous_x = p->x;
    p->previous_y = p->y;
    p->animation_time += GAME_DT;
    p->jump_time+=GAME_DT;
    p->normal_land_time=decay(p->normal_land_time);
    p->input_horizontal=input.horizontal;
    player_source_begin(p,input);
    const bool was_grounded = p->grounded;
    p->coyote = was_grounded ? 0.1f : decay(p->coyote);
    p->chain_window = decay(p->chain_window);
    p->jump_buffer = input.jump_pressed ? 0.1f : decay(p->jump_buffer);

    p->landing_time = decay(p->landing_time);
    if (p->landing_time > 0 && p->source_step && p->pound_frame < POUND_END_FRAME) ++p->pound_frame;
    if (was_grounded && p->chain_window == 0) p->jump_stage = 0;

    const bool special = player_special_step(p,world,input,was_grounded);
    if(input.pound_pressed && !p->submerged && !was_grounded && p->action!=ACTION_BONK && !p->pounding) {
        p->action=ACTION_NONE;p->pounding=true;p->pound_windup=POUND_WINDUP_FRAMES/32.0f;p->pound_frame=0;
        p->spin_time=0;p->vx=p->vy=0;p->coyote=p->jump_buffer=0;
    }
    p->tap_window=decay(p->tap_window);
    const int tap=input.horizontal>0.5f ? 1 : input.horizontal<-0.5f ? -1 : 0;
    if(tap==0) p->tap_released=true;
    else if(p->tap_released || p->last_tap!=tap) {
        if(p->last_tap==tap && p->tap_window>0 && was_grounded) p->dashing=true;
        p->last_tap=tap;p->tap_window=9.0f/32;p->tap_released=false;
    }
    if(tap==0 || (tap<0)!=p->facing_left || input.jump_pressed || input.crouch_held || input.nozzle_held)
        p->dashing=false;
    const bool locked = p->pounding || p->landing_time > 0;
    if (!locked && !special) {
        const float axis = fmaxf(-1, fminf(1, input.horizontal));
        float target = axis * (p->dashing ? 450 : 280);
        if(input.nozzle_held && p->nozzle==NOZZLE_TURBO && p->water>0 && p->power>=100) target=p->vx;
        float acceleration = was_grounded ? 1700 : 950;
        if (axis == 0) acceleration = was_grounded ? 2000 : 300;
        else if (p->vx * axis < 0) acceleration = was_grounded ? 2400 : 1200;
        if(was_grounded && p->material==TILE_ICE) acceleration/=5;
        else if(was_grounded && p->material==TILE_PUDDLE) acceleration/=2.2f;
        p->vx = approach(p->vx, target, acceleration * GAME_DT);
        if (axis != 0 && p->action!=ACTION_BACKFLIP && p->action!=ACTION_ROLL) p->facing_left = axis < 0;

        if (p->jump_buffer > 0 && p->coyote > 0) {
            p->jump_stage = p->chain_window > 0 && p->jump_stage < 3 ? p->jump_stage + 1 : 1;
            if(p->jump_stage==3 && fabsf(p->vx)<=3*44) p->jump_stage=1;
            static const float jumps[] = {0, -430, -510, -610};
            p->vy = jumps[p->jump_stage];
            p->jump_time=0;
            p->grounded = false;
            p->coyote = p->jump_buffer = p->chain_window = 0;
            p->jump_cut = false;
        }
        if (input.pound_pressed && !p->grounded) {
            p->pounding = true;
            p->pound_windup = POUND_WINDUP_FRAMES / 32.0f;
            p->pound_frame = 0;
            p->spin_time = 0;
            p->vx = p->vy = 0;
            p->jump_buffer = p->coyote = 0;
        }
    } else if (p->landing_time > 0 && p->pound_frame < 35) {
        if (p->source_step) p->vx *= 0.8f; /* Ground Pound.xml "hit": xspeed *= 0.8. */
    } else if (locked && !(p->pounding && p->pound_windup == 0)) {
        p->vx = approach(p->vx, 0, 2400 * GAME_DT);
    }

    player_spin_prepare(p,input);
    if (p->pounding) {
        if (p->pound_windup > 0) {
            /* Frames 0-11: yspeed = -gravity cancels CharCode gravity; xspeed = 0. */
            p->vx = p->vy = 0;
            if (p->source_step && ++p->pound_frame >= POUND_WINDUP_FRAMES) {
                p->pound_windup = 0;
                p->vy = 8 * 44.0f; /* Frame 12: yspeed = 8. */
            } else p->pound_windup = (POUND_WINDUP_FRAMES - p->pound_frame) / 32.0f;
        } else if (p->source_step) {
            float vx = p->vx / 44, vy = p->vy / 44;
            vy += 1;                                   /* CharCode gravity in air. */
            if (vy > 0) vy = fmaxf(0, vy - 0.2f) / 1.05f; /* CharCode fall drag. */
            vy += 0.5f;                                /* Clip: gravity/2 ... */
            if (vy < 3) vy += 0.5f;                    /* ... and again while slow. */
            vx *= 0.95f;
            if (input.horizontal > 0) vx -= fminf((vx - 5) / 20, 0);
            if (input.horizontal < 0) vx -= fmaxf((vx + 5) / 20, 0);
            p->vx = vx * 44; p->vy = vy * 44;
            if (p->pound_frame < 26) ++p->pound_frame; /* stop() at frame 26 */
        }
    } else if (!p->submerged && p->action!=ACTION_BONK && p->spin_time==0) {
        if (!input.jump_held && p->vy < 0 && !p->jump_cut) {
            p->vy *= 0.45f;
            p->jump_cut = true;
        }
        const float gravity = input.jump_held && p->vy < 0 ? 1050 : 1600;
        p->vy = fminf(800, p->vy + gravity * GAME_DT);
    }
    if(p->grounded) p->power=100;
    if(p->material==TILE_PUDDLE) p->water=fminf(100,p->water+80*GAME_DT);
    player_spin_update(p,input);
    player_fludd_step(p,input);
    player_collision_step(p,world);
    if(p->wall_normal && (fabsf(p->wall_speed)>11*44 || (p->dashing && fabsf(p->wall_speed)>5*44)) && !p->submerged && p->action!=ACTION_BONK) {
        p->action=ACTION_BONK;p->action_time=0;p->bonk_direction=p->wall_normal;
        p->vx=p->vy=0;p->rocket_boost=0;p->dashing=false;
    }
    if (p->grounded) {

        if (!was_grounded) {
            p->normal_land_time=3.0f/32;
            p->chain_window = p->jump_stage > 0 ? 0.28f : 0;
            if (p->pounding) {
                /* "hit" (27) through frame 42: sixteen source frames of attack. */
                p->landing_time = (POUND_END_FRAME + 1 - POUND_HIT_FRAME) / 32.0f;
                p->pound_frame = POUND_HIT_FRAME;
                p->jump_stage = 0;
                p->chain_window = 0;
            }
        }
        p->pounding = false;
        p->pound_windup = 0;
    }
    if(p->submerged) p->animation=p->spin_time>0 ? PLAYER_SPIN : PLAYER_SWIM;
    else if(p->action==ACTION_BONK) p->animation=PLAYER_BONK;
    else if(p->action==ACTION_DIVE && p->grounded)
        /* Dive.xml bitmap layer: frames 1..5 Slide, frames 0 and 6..9 Air - 2. */
        p->animation=p->dive_frame>=1 && p->dive_frame<=5 ? PLAYER_CROUCH : PLAYER_DIVE_RECOVER;
    else if(p->action==ACTION_DIVE) p->animation=PLAYER_DIVE;
    else if(p->action==ACTION_BACKFLIP) p->animation=PLAYER_BACKFLIP;
    else if(p->action==ACTION_ROLL) p->animation=PLAYER_ROLL;
    else if(p->rocket_boost>0) p->animation=PLAYER_ROCKET;
    else if(p->jet_active) p->animation=p->nozzle==NOZZLE_HOVER ? PLAYER_HOVER : p->nozzle==NOZZLE_ROCKET ? PLAYER_ROCKET_CHARGE : PLAYER_TURBO;
    else if (p->landing_time > 0) p->animation = PLAYER_LAND;
    else if (p->pounding) p->animation = p->pound_windup > 0 ? PLAYER_POUND_WINDUP : PLAYER_POUND;
    else if (p->spin_time > 0) p->animation = PLAYER_SPIN;
    else if (p->normal_land_time>0 && p->grounded) p->animation=PLAYER_LAND;
    else if (!p->grounded) p->animation = p->vy<=2*44 ? PLAYER_RISE : PLAYER_FALL;
    else p->animation = fabsf(p->vx) > 8 ? PLAYER_RUN : PLAYER_IDLE;
    player_animation_step(p, input);
}

/* Attack geometry is exposed for future targets; this playground has no enemies. */
bool player_attack_hitbox(const Player *p, Rect *hitbox)
{
    if (p->spin_time > 0) {
        *hitbox = (Rect){p->x - 18, p->y - 4, PLAYER_WIDTH + 36, player_height(p) + 8};
        return true;
    }
    if (p->pounding && p->pound_windup == 0) {
        *hitbox = (Rect){p->x, p->y + player_height(p) - 4, PLAYER_WIDTH, 8};
        return true;
    }
    return false;
}
