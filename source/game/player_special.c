#include "game.h"
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

bool player_special_step(Player *p,const World *w,GameInput in,bool grounded)
{
    p->action_time+=GAME_DT;
    p->jet_active=false;
    if(in.nozzle_next_pressed && p->action==ACTION_NONE && !p->pounding && p->spin_time==0) {
        p->nozzle=(Nozzle)((p->nozzle+1)%NOZZLE_COUNT);
        p->rocket_charge=p->rocket_boost=0;
    }
    float surface=0;
    p->submerged=world_water_at(w,p->x+PLAYER_WIDTH/2,p->y+player_height(p)*0.55f,&surface);
    if(p->submerged) {
        p->action=ACTION_NONE;p->pounding=false;
        if(in.spin_pressed && p->spin_cooldown==0) {p->spin_time=0.32f;p->spin_cooldown=0.5f;}
        p->jump_stage=0;p->jump_buffer=p->coyote=0;
        if(player_height(p)<PLAYER_HEIGHT && player_stand_clear(p,w)) resize(p,PLAYER_HEIGHT);
        p->water=fminf(100,p->water+160*GAME_DT);
        p->power=fminf(100,p->power+96*GAME_DT);
        p->vx+=in.horizontal*500*GAME_DT;
        p->vx=friction(p->vx,0.05f,1.05f);
        const float swim_limit=in.nozzle_held && p->nozzle==NOZZLE_TURBO && p->power>=100 ? 1400 : 220;
        p->vx=fmaxf(-swim_limit,fminf(swim_limit,p->vx));
        const float vertical=(in.crouch_held || in.vertical>0) ? 1 : in.vertical<0 || in.jump_held ? -1 : 0;
        p->vy+=vertical*422.4f*GAME_DT;
        p->vy=friction(p->vy,0.05f,1.1f);
        p->vy=fmaxf(-180,fminf(200,p->vy));
        if(in.horizontal!=0) p->facing_left=in.horizontal<0;
        /* Original surface jump is weaker than a land jump. */
        if(in.jump_pressed && p->y<surface+12) {
            p->vy=-10*ORIGINAL_VELOCITY/1.5f;p->submerged=false;p->jump_cut=true;
        }
        return true;
    }
    if(p->action==ACTION_BONK) {
        p->vx=0;p->vy=0;
        if(p->action_time>=10.0f/32) {
            p->vx=p->bonk_direction*5*ORIGINAL_VELOCITY;p->vy=-4*ORIGINAL_VELOCITY;
            p->action=ACTION_NONE;p->jump_cut=true;
            p->grounded=false;
        }
        return true;
    }
    if(p->action==ACTION_BACKFLIP || p->action==ACTION_ROLL) {
        if(grounded) p->action=ACTION_NONE;
        else if(p->action_time<0.5f) return false;
        else p->action=ACTION_NONE;
    }
    if(in.crouch_held && !p->pounding && p->landing_time==0 && p->action==ACTION_NONE) {
        p->action=ACTION_DIVE;p->action_time=0;p->spin_time=0;p->dashing=false;
        if(!grounded) {
            float target=(p->facing_left ? -1 : 1)*35*ORIGINAL_VELOCITY;
            p->vx+=(target-p->vx)/5;p->vy+=3*ORIGINAL_VELOCITY;
        }
    }
    if(p->action==ACTION_DIVE) {
        if(player_height(p)!=26) resize(p,26);
        const float speed=fabsf(p->vx);
        const bool backwards=(p->facing_left && in.horizontal>0)||(!p->facing_left && in.horizontal<0);
        if(grounded && in.jump_pressed && player_stand_clear(p,w)) {
            resize(p,PLAYER_HEIGHT);
            if(backwards && speed<5*ORIGINAL_VELOCITY) {
                p->action=ACTION_BACKFLIP;p->action_time=0;
                p->vx*=0.75f;
                const float target=(p->facing_left ? 1 : -1)*30*ORIGINAL_VELOCITY;
                p->vx+=(target-p->vx)/5;p->vy=-12*ORIGINAL_VELOCITY;
            } else {
                p->action=ACTION_ROLL;p->action_time=0;
                const float target=(p->facing_left ? -1 : 1)*15*ORIGINAL_VELOCITY;
                p->vx+=(target-p->vx)/5;p->vy=-10*ORIGINAL_VELOCITY/1.5f;
            }
            p->grounded=false;p->jump_cut=true;p->jump_buffer=p->coyote=0;
            return true;
        }
        if(grounded) {
            float a=p->material==TILE_ICE ? 0.04f : p->material==TILE_PUDDLE ? 0.1f : 0.2f;
            float b=p->material==TILE_ICE ? 1.007f : p->material==TILE_PUDDLE ? 1.015f : 1.05f;
            p->vx=friction(p->vx,a,b);
            p->vx+=p->ground_slope*995.6f*GAME_DT; /* sin(45 degrees) * original acceleration. */
        } else p->vx+=in.horizontal*120*GAME_DT;
        if(grounded && !in.crouch_held && fabsf(p->vx)<44 && player_stand_clear(p,w)) {
            resize(p,PLAYER_HEIGHT);p->action=ACTION_NONE;
        } else return true;
    }
    if(player_height(p)<PLAYER_HEIGHT && player_stand_clear(p,w)) resize(p,PLAYER_HEIGHT);
    return false;
}
void player_fludd_step(Player *p,GameInput in)
{
    if(!in.nozzle_held || p->water<=0 || p->action==ACTION_BONK || p->pounding) {
        p->rocket_charge=fmaxf(0,p->rocket_charge-GAME_DT);
        if(!in.nozzle_held && p->rocket_boost<=0) p->power=fminf(100,p->power+32*GAME_DT);
    } else if(p->nozzle==NOZZLE_HOVER && p->power>0) {
        float v=p->vy/44;
        v*=powf(0.8f,32*GAME_DT);
        v-=((9.2f-v)/10)*(p->power/175+0.75f)*(32*GAME_DT);
        p->vy=v*44;p->vx*=powf(1.03f,-32*GAME_DT);
        p->water=fmaxf(0,p->water-2.24f*GAME_DT);
        p->power=fmaxf(0,p->power-48*GAME_DT);p->jet_active=true;
    } else if(p->nozzle==NOZZLE_ROCKET && p->power>=100 && p->water>=5 && p->rocket_boost<=0) {
        p->rocket_charge+=GAME_DT;p->jet_active=true;
        if(p->rocket_charge>=15.0f/32 && p->water>=5) {
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
    }
    p->vx=fmaxf(-1400,fminf(1400,p->vx));
}
