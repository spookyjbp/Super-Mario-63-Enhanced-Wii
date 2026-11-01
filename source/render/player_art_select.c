/* Mapping from the port's player state to original Mario Main clips.
   Frame rules follow the clip scripts in Runouw/Super-Mario-63 83e81fb
   (the Mario CLIPS XML files); see docs/movement-animation.md. */
#include "player_art_select.h"
#include <math.h>

/* Mario Main places each Dive clip at this matrix translate, plus Char's
   (0,-60): the point Dive.xml's `_rotation` turns around. */
static const int dive_pivot[ART_VARIANTS][2]={{60,-240},{-13,-393},{-40,0},{7,-190}};

static unsigned clampu(float v,unsigned hi)
{ return v<=0 ? 0u : v>=(float)hi ? hi : (unsigned)v; }

ArtPick player_art_select(const Player *p)
{
    ArtPick a={.variant=p->has_fludd ? 1u+(unsigned)p->nozzle : 0u};
    const float vy=p->vy/44.0f;
    /* FLUDD water layers (Mario Main H1-H4/T1-T4/R1-R4 and swim frames). */
    if(p->nozzle==NOZZLE_ROCKET)
        a.effect_index=p->rocket_boost>0 ? 15u+clampu((8.0f/32-p->rocket_boost)*32,7)
                                         : clampu(p->rocket_charge*32,14);
    else a.effect_index=p->blast_level*2u+(p->blast_parity ? 1u : 0u);

    if(p->submerged && p->spin_time==0) {
        /* Floating.xml / Swimming Up.xml: frame 1 while yspeed>0 (stroke: >0.5). */
        a.state=p->swim_stroke ? ART_SWIM_STROKE : ART_SWIM_FLOAT;
        a.index=(p->swim_stroke ? vy<0.5f : vy>0) ? 0u : 1u;
    } else if(p->spin_time>0) {
        /* Spin Attack.xml: frame 1..40 fast, +40 while speed <= 10. */
        a.state=ART_SPIN;
        a.index=(unsigned)p->spin_frame+(p->spin_speed>10 ? 0u : 40u);
    } else if(p->action==ACTION_BONK) {
        a.state=ART_HURT2;a.index=p->bonk_frame;    /* hurt2 (HitWall) timeline */
    } else if(p->reading_sign) {
        a.state=ART_SIGN;a.index=0;                 /* Main "Sign": Mario faces the sign */
    } else if(p->pounding || p->landing_time>0) {
        a.state=ART_GROUND;a.index=p->pound_frame;  /* Ground Pound.xml timeline */
    } else if(p->action==ACTION_DIVE) {
        a.state=ART_DIVE;a.index=p->dive_frame;       /* Dive.xml timeline */
        a.rotation=p->dive_rotation;
        a.pivot_x=dive_pivot[a.variant][0];a.pivot_y=dive_pivot[a.variant][1];
    } else if(p->action==ACTION_BACKFLIP) {
        a.state=ART_BACKFLIP;a.timed=true;a.seconds=p->action_time;
    } else if(p->action==ACTION_ROLL) {
        a.state=ART_DIVEFLIP;a.timed=true;a.seconds=p->action_time;
    } else if(!p->grounded) {
        if(p->jump_stage==3 && p->jump_time<30.0f/32) {
            a.state=ART_FRONTFLIP;a.timed=true;a.seconds=p->jump_time;
        } else a.state=vy>2 ? ART_FALL : ART_RISE; /* CharCode frame 3/4 */
    } else if(p->normal_land_time>0) {
        a.state=ART_LAND;a.timed=true;a.seconds=3.0f/32-p->normal_land_time;
    } else if(p->dashing) {
        /* Running.xml: frames 0-5 are the dash start, then round(frame)+7. */
        a.state=ART_RUN;
        a.index=p->run_count<6 ? p->run_count : (unsigned)roundf(p->run_frame)+6u;
    } else {
        a.state=ART_WALK;a.index=clampu(roundf(p->walk_frame)-1,79); /* Walk.xml */
    }
    return a;
}
