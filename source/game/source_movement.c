#include "source_movement.h"
#include <math.h>
/* These recurrences execute once per original 32 Hz source frame. Collision and
   position integration remain at 60 Hz; no fractional damping approximation. */
#define V 44.0f
static float drag(float v,float subtraction,float division)
{ return copysignf(fmaxf(0,fabsf(v)-subtraction)/division,v); }
void player_source_begin(Player *p,GameInput in)
{
    p->source_phase+=32;
    p->source_step=p->source_phase>=60;
    if(p->source_step) p->source_phase-=60;
    p->spin_requested |= in.spin_pressed;
}
bool player_swim_step(Player *p,const World *w,GameInput in)
{
    const float h=player_height(p),feet=p->y+h;
    float surface=0;
    const bool wet=world_water_at(w,p->x+PLAYER_WIDTH/2,feet-h/4,&surface);
    const bool entered=wet&&!p->submerged;
    p->submerged=wet;
    const bool up=in.jump_held || in.vertical<-0.5f;
    const bool down=in.crouch_held || in.vertical>0.5f;
    if(entered) {
        float v=p->vy/V;
        if(v>7) v*=0.7f;
        p->vy=v*0.9f*V;
        p->pounding=false;p->action=ACTION_NONE;p->jump_stage=0;
    }
    bool surface_jump=false;
    if(p->source_step && up && !p->grounded && p->vy>-5*V && p->vy<V &&
       !world_water_at(w,p->x+PLAYER_WIDTH/2,feet-12*1.375f,NULL) &&
       world_water_at(w,p->x+PLAYER_WIDTH/2,feet+5*1.375f,&surface)) {
        p->vy=(p->vy/V*0.7f-10.0f/1.5f)*V;
        p->jump_cut=true;p->swim_stroke=false;surface_jump=true;
    }
    if(!wet) {p->swim_stroke=false;p->swim_frames=0;return surface_jump;}
    p->action=ACTION_NONE;p->pounding=false;p->jump_stage=0;
    p->jump_buffer=p->coyote=0;
    if(!p->source_step) return true;
    float vx=p->vx/V,vy=p->vy/V;
    if(!p->grounded) {
        if(world_water_at(w,p->x+PLAYER_WIDTH/2,feet-h/2,NULL)) vy+=1.0f/3;
        vy=drag(vy,0.05f,1.01f);
        vy/=1.001f;vx/=1.001f;
    }
    if(p->spin_time==0) {
        if(in.horizontal>0) vx-=fminf((vx-5)/(20.0f/3),0);
        if(in.horizontal<0) vx-=fmaxf((vx+5)/(20.0f/3),0);
        if(p->swim_stroke) {
            ++p->swim_frames;
            if(!up && p->swim_frames>5) p->swim_stroke=false;
        }
        if(up) {
            if(p->swim_stroke) vy-=0.2f;
            else {
                p->swim_frames=0;
                if(!surface_jump) vy-=down ? 4 : 6;
                /* Original uses xpseed (a typo) on the left; preserve the
                   actual script behavior rather than silently symmetrizing. */
                if(in.horizontal>0) vx+=2;
            }
            p->swim_stroke=true;
        }
        if(down) vy+=0.3f;
    }
    vy=drag(vy,0.05f,1.1f);vx=drag(vx,0.05f,1.05f);
    p->vx=vx*V;p->vy=vy*V;
    p->water=fminf(100,p->water+5);
    p->power=fminf(100,p->power+3);
    if(in.horizontal!=0) p->facing_left=in.horizontal<0;
    return true;
}
void player_spin_prepare(Player *p,GameInput in)
{
    if(!p->source_step) return;
    const bool requested=p->spin_requested || in.spin_held;
    p->spin_requested=false;
    if(p->action!=ACTION_NONE || p->pounding || p->landing_time>0) return;
    if(p->spin_time==0 && requested && p->vy>-3*V) {
        p->spin_speed=15;p->spin_frame=0;p->spin_time=1;
        p->spin_combo_count=0;p->spin_combo_direction=0;p->spin_combo_window=0;
    }
}
void player_spin_update(Player *p,GameInput in)
{
    if(p->spin_time==0 || !p->source_step) return;
    float vx=p->vx/V,vy=p->vy/V;
    if(!p->submerged && !p->grounded) vy+=1; /* Original CharCode gravity. */
    const int direction=in.horizontal>0.5f ? 1 : in.horizontal<-0.5f ? -1 : 0;
    p->spin_combo_window=fmaxf(0,p->spin_combo_window-1);
    if(p->spin_combo_window==0) p->spin_combo_count=0;
    if(direction && direction!=p->spin_combo_direction) {
        ++p->spin_combo_count;p->spin_combo_direction=direction;
        p->spin_combo_window=direction>0 ? 15-p->spin_combo_count*3 : 11-p->spin_combo_count;
        if(p->spin_combo_count>=3) {
            p->spin_speed=p->spin_speed*0.8f+15;
            p->spin_combo_count=0;p->spin_combo_direction=0;p->spin_combo_window=0;
        }
    }
    p->spin_frame=fmodf(p->spin_frame+fminf(roundf(fmaxf(p->spin_speed,5)),10),40);
    const bool down=in.crouch_held || in.vertical>0.5f;
    if(down && !p->submerged && !p->grounded) {
        p->spin_speed=fmaxf(10,p->spin_speed+1);
        vy+=sqrtf((p->spin_speed+1)/50)/5;
    }
    p->spin_speed=p->spin_speed>7 ? drag(p->spin_speed,0.5f,1.1f) : drag(p->spin_speed,0.1f,1.03f);
    if(p->submerged) p->spin_speed=drag(p->spin_speed,0.3f,1.1f);
    p->spin_speed=fmaxf(1,p->spin_speed);
    if(!p->grounded) {
        if(!down) {
            vy=p->spin_speed>7 ? drag(vy,0.3f,1.05f) : drag(vy,0.1f,1.03f);
            vy-=sqrtf((p->spin_speed+1)/30)/3;
        }
        if(p->submerged) vy-=sqrtf((p->spin_speed+1)/20)/2;
    }
    p->vx=vx*V;p->vy=vy*V;
    if(p->spin_speed<=(p->grounded ? 5 : 3)) {p->spin_time=0;p->spin_speed=0;}
}
