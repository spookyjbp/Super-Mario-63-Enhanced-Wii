/* Presentation counters reproduced from the original clip scripts.
   Runouw/Super-Mario-63 83e81fb, Super_Mario_63_2010/LIBRARY/Char/Mario CLIPS.
   They never feed back into physics. */
#include "game.h"
#include "sfx.h"
#include <math.h>

#define V 44.0f
enum { CLIP_OTHER, CLIP_WALK, CLIP_RUN };

static float drag(float v,float subtract,float divide)
{ return copysignf(fmaxf(0,fabsf(v)-subtract)/divide,v); } /* _root.groundFriction */

static unsigned clip_family(const Player *p)
{
    if(p->submerged || p->action!=ACTION_NONE || p->pounding || p->landing_time>0 || p->sleeping || p->ejump || p->drowning ||
       p->spin_time>0 || !p->grounded || p->normal_land_time>0) return CLIP_OTHER;
    return p->dashing ? CLIP_RUN : CLIP_WALK;
}

/* Walk.xml / Running.xml play _root.StepSound when frame enters 11-20 or 51-60. */
/* Idle Animation 1050's frame sounds (1-based frames) and its end: Sleep. */
static void idle_frame_script(Player *p)
{
    static const struct { unsigned frame; SfxId sfx; } say[] = {
        {385,SFX_LETSGO},{528,SFX_MMMM},{635,SFX_HMM},{833,SFX_IMTIRED},{908,SFX_YAWN}};
    for(unsigned i=0;i<sizeof say/sizeof say[0];++i) if(p->idle_frame+1==say[i].frame) sfx_play(say[i].sfx,100,0);
    if(p->idle_frame+1>=978) {p->sleeping=true;p->sleep_frame=0;p->snore=0;p->idle_kind=0;}
}
/* Tired Animation 1067: frame 3 count = 0; frame 37 count++, < 12 back to
   "repeat" (frame 4), else Sleep. */
static void tired_frame_script(Player *p)
{
    if(p->idle_frame+1==3) p->tired_count=0;
    if(p->idle_frame+1==37) {
        if(++p->tired_count<12) p->idle_frame=3;
        else {p->sleeping=true;p->sleep_frame=0;p->snore=0;p->idle_kind=0;}
    }
}
/* Mario Sleep.xml (Main "Sleep"): plays to frame 11, stops there snoring
   ("Mario - Snore1" at count 32, "Snore2" at 32 * 1.7, then again) until Up,
   Right, Left, Down or Space; then plays on and frame 31 ends the attack. */
static void sleep_step(Player *p)
{
    if(p->sleep_frame==10) {
        ++p->snore;
        if(p->snore==32) sfx_play(SFX_SNORE1,100,0);
        if(p->snore>=32*1.7f) {sfx_play(SFX_SNORE2,100,0);p->snore=0;}
        if(p->wake_keys) p->sleep_frame=11;
        return;
    }
    if(++p->sleep_frame>=30) {p->sleeping=false;p->sleep_frame=0;}
}
/* Enemy Jump Animation 1445 (Mario Main "EJump", held on a stomped enemy):
   frame 1 jumps to "jump" (9) when landtime is 1, else checks every frame for
   landtime 2 and jumps to "midjump" (12) once; frame 11 goes to "aftermid"
   (14); frames 8 and 21 end the attack (the clip stays on its last frame). */
static void ejump_step(Player *p)
{
    if(p->ejump_frame==0) {
        p->ejump_frame=p->ejump_landtime==1 ? 9 : 1;
        p->ejump_checking=p->ejump_landtime!=1;
        return;
    }
    if(p->ejump_checking && p->ejump_landtime==2) {p->ejump_frame=12;p->ejump_checking=false;return;}
    if(p->ejump_frame==8 || p->ejump_frame>=21) return;
    if(++p->ejump_frame==11) p->ejump_frame=14;
}

static bool step_zone(float f) { return (f>10&&f<=20)||(f>50&&f<=60); }
static void step_sound(float before,float after)
{ if(step_zone(after)&&!step_zone(before)) sfx_play(sfx_step_sound(),100,0); }

void player_animation_step(Player *p,GameInput in)
{
    const unsigned family=clip_family(p);
    /* A new clip instance starts from its first frame (frame = 1). */
    if(family!=p->art_clip) {
        if(family==CLIP_WALK) {p->walk_frame=1;p->walk_count=0;p->walk_count_nan=true;p->idle_kind=0;}
        if(family==CLIP_RUN) {p->run_frame=1;p->run_count=0;}
        p->art_clip=family;
        p->idle_kind=0;
    }
    /* Char frame 1: the "fluddrepeat" loop while spraying (no attack, or a dive). */
    sfx_set_fludd_loop(p->has_fludd && (p->nozzle==NOZZLE_HOVER || p->nozzle==NOZZLE_TURBO) && in.nozzle_held &&
                       p->power>0 && p->water>0 && p->spin_time==0 && !p->pounding &&
                       (p->action==ACTION_NONE || p->action==ACTION_DIVE));
    if(!p->source_step) return;
    sfx_step_tick(p->material==TILE_PUDDLE);
    /* CharCode: falling with fallingyspeed >= 15 builds hurtfallcount (Hover drains
       it); at 5 "Mario-Fall" plays once; landing clears fallingsfx. */
    if(p->grounded||p->submerged) {p->fall_sfx=false;if(p->hurtfall>0) --p->hurtfall;}
    else if(p->vy>0 && p->action==ACTION_NONE && !p->pounding && p->vy/V>7) {
        if(in.nozzle_held && p->has_fludd && p->nozzle==NOZZLE_HOVER && p->power>0 && p->water>0)
            p->hurtfall=p->hurtfall>0 ? p->hurtfall-1 : 0;
        if(drag(p->vy/V,0.2f,1.05f)>=15 && p->vy/V>15 && !p->fall_sfx) {
            if(++p->hurtfall>7) p->hurtfall=7;
            if(p->hurtfall>=5) {p->fall_sfx=true;sfx_play(SFX_FALL,100,0);}
        }
    }
    const float xspeed=p->vx/V;
    if(p->sleeping) sleep_step(p);
    if(p->ejump) ejump_step(p); else p->ejump_frame=0;
    if(family==CLIP_WALK) {
        /* count++ while |xspeed| < 1 (NaN stays NaN), else 0; from 92 (or NaN)
           the clip shows Idle / Idle2 instead of the walk (without FLUDD only:
           the FLUDD walk clips have no idle). */
        if(fabsf(xspeed)<1) {if(!p->walk_count_nan) ++p->walk_count;}
        else {p->walk_count=0;p->walk_count_nan=false;}
        const bool idle=!p->has_fludd && (p->walk_count_nan || p->walk_count>=92);
        const unsigned kind=idle ? (p->low_hp ? 2u : 1u) : 0u;
        if(kind!=p->idle_kind) {p->idle_kind=kind;p->idle_frame=0;p->tired_count=0;}
        else if(kind==1) {++p->idle_frame;idle_frame_script(p);}
        else if(kind==2) {++p->idle_frame;tired_frame_script(p);}
        /* Mario (None) Walk.xml: frame += xspeed; Ice adjusts; wrap 1..80. */
        const float before=p->walk_frame;
        p->walk_frame+=xspeed;
        step_sound(before,p->walk_frame);
        if(p->material==TILE_ICE) {
            p->walk_frame-=xspeed/2;
            if(in.horizontal>0) p->walk_frame+=3;
            if(in.horizontal<0) p->walk_frame-=3;
        }
        if(p->walk_frame>=80) p->walk_frame=1;
        if(p->walk_frame<=0) p->walk_frame=80;
    } else if(family==CLIP_RUN) {
        /* Mario Running.xml: frames 0-5 play, then gotoAndStop(round(frame)+7)
           with frame += round(xspeed*0.7) - 1.5*_xscale/100 (+1 facing left). */
        if(p->run_count<6) ++p->run_count;
        else {
            const float before=p->run_frame;
            p->run_frame+=roundf(xspeed*0.7f)-1.5f*(p->facing_left ? 1 : -1);
            step_sound(before,p->run_frame);
            if(p->run_frame>=80) p->run_frame=1;
            if(p->run_frame<=0) p->run_frame=80;
        }
    }
    /* Mario Wait WaterBlast.xml: nextFrame() while KeySPACE && Power>0 &&
       WaterAmount>0, else prevFrame(); `_yscale *= -1` every frame. */
    const bool spraying=p->has_fludd && in.nozzle_held && p->power>0 && p->water>0;
    if(spraying && p->blast_level<4) ++p->blast_level;
    else if(!spraying && p->blast_level>0) --p->blast_level;
    p->blast_parity=!p->blast_parity;
    /* Mario (None) Dive.xml: airborne rotation eases toward the travel angle. */
    if(p->action==ACTION_DIVE && !p->grounded) {
        const float yspeed=p->vy/V,deg=57.29577951f;
        const float target=p->facing_left ? -atan2f(yspeed,-xspeed)*deg+10 : -atan2f(yspeed,xspeed)*deg-10;
        float rotation=p->dive_rotation;
        /* `rot` is the previous target: crossing +/-90 wraps the eased angle. */
        if(p->dive_target>0 ? target<-90 : target>90) rotation+=p->dive_target>0 ? -360 : 360;
        p->dive_target=target;
        if(rotation>520) rotation-=360;
        if(rotation<-520) rotation+=360;
        const float speed=fmaxf(-15,fminf(15,drag((target-rotation)/10,0.1f,1.15f)));
        p->dive_rotation=rotation+speed;
    } else {p->dive_rotation=0;p->dive_target=0;}
}
