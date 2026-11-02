#include "game.h"
#include "sfx.h"
#include "source_movement.h"
#include <math.h>
/* Original game is 32 Hz. Its velocity unit at the chosen logical collider scale is 32*1.375=44
   screen px/s. These conversions are adapters, not a full Flash simulation. */
#define ORIGINAL_VELOCITY 44.0f
static float friction(float v,float subtract,float divisor)
{
    float original=fabsf(v)/ORIGINAL_VELOCITY;
    original=fmaxf(0,original-subtract*(32*GAME_DT));
    original/=powf(divisor,32*GAME_DT);
    return copysignf(original*ORIGINAL_VELOCITY,v);
}
static void resize(Player *p,float height)
{
    float delta=player_height(p)-height;
    p->y+=delta;p->previous_y+=delta;p->height=height;
}

/* _root.ChangeFludd: "" -> H -> R -> T -> "" through the collected nozzles
   ("None" is always part of the cycle); "FluddSwitch" when it changed. */
bool player_change_fludd(Player *p)
{
    if(!p->nozzles) return false;
    int next=p->has_fludd ? (int)p->nozzle+1 : 0;
    while(next<NOZZLE_COUNT && !(p->nozzles&(1u<<next))) ++next;
    if(next>=NOZZLE_COUNT) p->has_fludd=false;
    else {p->has_fludd=true;p->nozzle=(Nozzle)next;}
    p->rocket_charge=p->rocket_boost=0;p->jet_active=false;p->blast_level=0;
    sfx_set_fludd_loop(false);
    sfx_play(SFX_FLUDD_SWITCH,100,0);
    return true;
}
bool player_special_step(Player *p,const World *w,GameInput in,bool grounded)
{
    p->action_time+=GAME_DT;
    p->jet_active=false;
    if(in.nozzle_next_pressed && p->action==ACTION_NONE && !p->pounding && p->spin_time==0) player_change_fludd(p);
    if(player_swim_step(p,w,in)) {
        if(player_height(p)<PLAYER_HEIGHT && player_stand_clear(p,w)) resize(p,PLAYER_HEIGHT);
        return true;
    }
    if(p->action==ACTION_BONK) {
        /* _root.HitWall(): attackFrame "hurt2", xspeed = yspeed = 0 (Mario Main
           "hurt2", clip 1422). Frames 1-10: against the wall (gravity still runs);
           frame 11: yspeed = -4, xspeed = +/-5 away from it; frame 15 waits for
           the ground; frame 23 waits for any key (CharHP > 0); frame 32 ends it. */
        if(p->bonk_frame<10) p->vx=0;
        if(p->source_step) {
            const bool any_key=in.horizontal!=0 || in.vertical!=0 || in.jump_held || in.crouch_held || in.nozzle_held;
            const bool hold=(p->bonk_frame==14 && !(grounded || p->submerged)) || (p->bonk_frame==22 && !any_key);
            if(!hold) ++p->bonk_frame;
            if(p->bonk_frame==10) {
                p->vy=-4*ORIGINAL_VELOCITY;
                p->vx=(p->facing_left ? 5 : -5)*ORIGINAL_VELOCITY;
                p->grounded=false;p->jump_cut=true;
            }
            if(p->bonk_frame>10 && p->bonk_frame<14 && grounded && p->vy>=0) p->vx=0;
            if(p->bonk_frame>=31) {p->action=ACTION_NONE;p->bonk_frame=0;return false;} /* frame 32: attack = false */
        }
        if(p->bonk_frame>=14 && grounded) p->vx=0;
        return true;
    }
    if(p->action==ACTION_HURT) {
        /* Mario Main "hurt1" (1411): frame 4 adds setYspeed/setXspeed, frame 9
           clears the attack. "hurt3" (1430): frame 4 waits for the ground (or
           water), frame 16 for any key, frame 23 clears the attack. */
        if(p->source_step) {
            const bool any_key=in.horizontal!=0 || in.vertical!=0 || in.jump_held || in.crouch_held || in.nozzle_held;
            bool hold=false;
            if(p->hurt_kind==3) hold=(p->hurt_frame==3 && !(grounded || p->submerged)) || (p->hurt_frame==15 && !any_key);
            if(!hold) ++p->hurt_frame;
            if(p->hurt_kind==1 && p->hurt_frame==3) {
                p->vx+=p->set_vx;p->vy+=p->set_vy;
                if(p->set_vy<0) p->grounded=false;
            }
            const unsigned end=p->hurt_kind==1 ? 8u : 22u;
            if(p->hurt_frame>=end) {p->action=ACTION_NONE;p->hurt_frame=0;return false;}
        }
        if(grounded && p->vy>=0) { /* CharCode ground friction while the attack runs */
            const float v=fabsf(p->vx)-2000*GAME_DT;
            p->vx=v>0 ? copysignf(v,p->vx) : 0;
        }
        return true;
    }
    if(p->action==ACTION_BACKFLIP || p->action==ACTION_ROLL) {
        /* Mario Backflip.xml: attack = false at frame 21, or from frame 14 on
           landing (air == false && yspeed > -3); from frame 14 KeyDOWN dives.
           Mario Diveflip.xml: attack = false at frame 8, after which the air
           dive is available again (dive, flip, dive... chains). The takeoff
           tick still touches the ground, hence the yspeed test. */
        const bool backflip=p->action==ACTION_BACKFLIP;
        const float end=backflip ? 21.0f/32 : 8.0f/32;
        const bool late=backflip && p->action_time>=14.0f/32;
        if(p->action_time>=end || (late && grounded && p->vy>-3*ORIGINAL_VELOCITY)) p->action=ACTION_NONE;
        else if(late && in.crouch_held && !p->submerged) p->action=ACTION_NONE; /* dive below */
        else {
            if(p->source_step) {
                float vx=p->vx/44;
                if(in.horizontal>0) vx-=fminf((vx-5)/(20.0f/3),0)/1.5f;
                if(in.horizontal<0) vx-=fmaxf((vx+5)/(20.0f/3),0)/1.5f;
                p->vx=vx*44;
            }
            p->facing_left=p->backflip_facing_left;
            return true;
        }
    }
    if(in.crouch_held && p->spin_time==0 && !p->pounding && p->landing_time==0 && p->action==ACTION_NONE) {
        p->action=ACTION_DIVE;p->action_time=0;p->spin_time=0;p->dashing=false;
        sfx_play(SFX_DIVE,100,0); /* "Mario-Dive" */
        p->dive_frame=0;p->dive_recovering=false;
        if(!grounded) {
            float target=(p->facing_left ? -1 : 1)*35*ORIGINAL_VELOCITY;
            p->vx+=(target-p->vx)/5;p->vy+=3*ORIGINAL_VELOCITY;
        }
    }
    if(p->action==ACTION_DIVE) {
        if(player_height(p)!=26) resize(p,26);
        const float speed=fabsf(p->vx);
        const bool backwards=(p->facing_left && in.horizontal>0)||(!p->facing_left && in.horizontal<0);
        /* Diveflip needs |rotation| < 25 (the dive's ground angle); tile slopes are 45. */
        const bool level=w->course ? fabsf(p->ground_angle)<25 : p->ground_slope==0;
        /* Mario (None) Dive.xml onEnterFrame, lines 136-160. Both checks run on every
           source frame of the clip, including the recovery frames, and test KeyUP() as
           a held state. A fresh press is not required: Down, hold Jump, then push away
           from the facing direction also backflips, as in the original. */
        if(p->source_step && grounded && in.jump_held && player_stand_clear(p,w)) {
            if(backwards && speed<5*ORIGINAL_VELOCITY) {
                resize(p,PLAYER_HEIGHT);
                p->action=ACTION_BACKFLIP;p->action_time=0;p->backflip_facing_left=p->facing_left;
                p->vx*=0.75f;
                const float target=(p->facing_left ? 1 : -1)*30*ORIGINAL_VELOCITY;
                p->vx+=(target-p->vx)/5;p->vy=-12*ORIGINAL_VELOCITY;
            } else if(p->dive_frame>=2 && speed>3*ORIGINAL_VELOCITY && level && !backwards) {
                resize(p,PLAYER_HEIGHT);
                p->action=ACTION_ROLL;p->action_time=0;p->backflip_facing_left=p->facing_left;
                const float target=(p->facing_left ? -1 : 1)*15*ORIGINAL_VELOCITY;
                p->vx+=(target-p->vx)/5;p->vy=-10*ORIGINAL_VELOCITY/1.5f;
            }
            if(p->action!=ACTION_DIVE) {
                p->grounded=false;p->jump_cut=true;p->jump_buffer=p->coyote=0;
                return true;
            }
        }
        if(grounded) {
            float a=p->material==TILE_ICE ? 0.04f : p->material==TILE_PUDDLE ? 0.1f : 0.2f;
            float b=p->material==TILE_ICE ? 1.007f : p->material==TILE_PUDDLE ? 1.015f : 1.05f;
            p->vx=friction(p->vx,a,b);
            p->vx+=p->ground_slope*995.6f*GAME_DT; /* sin(45 degrees) * original acceleration. */
        } else p->vx+=in.horizontal*120*GAME_DT;
        if(p->source_step) {
            /* Timeline: frames 0-1 play, stop() holds frame 2 (the Slide pose).
               play() runs recovery frames 3..9; frame 9 sets attack=false. */
            if(p->dive_recovering || p->dive_frame<2) ++p->dive_frame;
            /* Slide (Hover).xml clears attack at frame 7; None/Turbo/Rocket at 9. */
            const unsigned end=p->has_fludd && p->nozzle==NOZZLE_HOVER ? 7 : 9;
            if(p->dive_frame>=end) {
                if(player_stand_clear(p,w)) {
                    resize(p,PLAYER_HEIGHT);p->action=ACTION_NONE;
                    p->dive_frame=0;p->dive_recovering=false;
                    p->jump_buffer=p->coyote=0; /* JumpPress has expired by frame 9. */
                    return false;
                }
                p->dive_frame=2;p->dive_recovering=false; /* Ceiling adaptation: stay down. */
            }
            const float vx=fabsf(p->vx);
            if(!p->dive_recovering && p->dive_frame>=2 && grounded && level &&
               ((in.jump_held && vx<3*ORIGINAL_VELOCITY) || (vx<ORIGINAL_VELOCITY && !in.crouch_held)))
                p->dive_recovering=true; /* Dive.xml lines 161-168 call play(). */
        }
        return true;
    }
    if(player_height(p)<PLAYER_HEIGHT && player_stand_clear(p,w)) resize(p,PLAYER_HEIGHT);
    return false;
}
void player_fludd_step(Player *p,GameInput in)
{
    if(!p->has_fludd) {p->jet_active=false;p->rocket_charge=p->rocket_boost=0;return;}
    if(!in.nozzle_held || p->water<=0 || p->action==ACTION_BONK || p->action==ACTION_HURT || p->pounding || p->spin_time>0) {
        p->rocket_charge=fmaxf(0,p->rocket_charge-GAME_DT);
        /* CharCode: if(!KeySPACE() && Fluddpow !== "H") Power += 1 a frame. The
           Hover nozzle only refills on the ground (Power = TotalPower) or in water. */
        if(!in.nozzle_held && p->rocket_boost<=0 && p->nozzle!=NOZZLE_HOVER) p->power=fminf(100,p->power+32*GAME_DT);
    } else if(p->nozzle==NOZZLE_HOVER && p->power>0) {
        float v=p->vy/44;
        v*=powf(0.8f,32*GAME_DT);
        v-=((9.2f-v)/10)*(p->power/175+0.75f)*(32*GAME_DT);
        p->vy=v*44;p->vx*=powf(1.03f,-32*GAME_DT);
        p->water=fmaxf(0,p->water-2.24f*GAME_DT);
        p->power=fmaxf(0,p->power-48*GAME_DT);p->jet_active=true;
    } else if(p->nozzle==NOZZLE_ROCKET && p->power>=100 && p->water>=5 && p->rocket_boost<=0) {
        if(p->rocket_charge==0) sfx_play(SFX_ROCKET_CHARGE,70,0); /* port-made: the original Rocket is silent */
        p->rocket_charge+=GAME_DT;p->jet_active=true;
        if(p->rocket_charge>=15.0f/32 && p->water>=5) {
            sfx_play(SFX_ROCKET_BLAST,85,0);
            p->water-=5;p->power=0;p->rocket_charge=0;p->rocket_boost=8.0f/32;
            p->vy=fminf(-25*44+fmaxf(p->vy/3,0),p->vy);
            p->grounded=false;p->jump_cut=true;
        }
    } else if(p->nozzle==NOZZLE_TURBO && p->power>=100) {
        const float target=(p->facing_left ? -1 : 1)*(p->grounded && !p->submerged ? 75 : 37.5f)*44;
        p->vx+=(target-p->vx)*(1-powf(29.0f/30,32*GAME_DT));
        p->water=fmaxf(0,p->water-1.6f*GAME_DT);p->jet_active=true;
        if(p->submerged) p->vy+=in.vertical*938.67f*GAME_DT;
    }
    if(p->rocket_boost>0) {
        p->rocket_boost=fmaxf(0,p->rocket_boost-GAME_DT);
        p->vy-=(p->submerged ? 1486 : 704)*GAME_DT;p->jet_active=true;
        /* Rocket water frames 16-23 (1340): onEnterFrame keeps Power = 0, so the ground
           refill cannot re-arm the Rocket while Mario is still leaving the ground. */
        p->power=0;
    }
    p->vx=fmaxf(-1400,fminf(1400,p->vx));
}
