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
            const bool hold=(p->bonk_frame==14 && !(grounded || p->submerged)) || (p->bonk_frame==22 && !any_key && !p->fainting);
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
        if(p->source_step && p->fainting) {
            /* CharHP <= 0. hurt1: frame 4 adds setYspeed - 3 and setXspeed * 0.8 and
               goes to 'falling' (10); frames 10-54 go to 'land' (55) on the ground or
               in water (54 waits there); then on to 107. hurt3: frame 4 waits for the
               ground, frame 16 goes to 'end' (24), then on to 70. Nothing ends them:
               LifeLost() takes over (course_session.c). */
            const unsigned f=p->hurt_frame;
            const bool down=grounded || p->submerged;
            if(p->hurt_kind==1) {
                if(f<3) ++p->hurt_frame;
                if(p->hurt_frame==3) {
                    p->vx+=p->set_vx*0.8f;p->vy+=p->set_vy-3*ORIGINAL_VELOCITY;
                    p->grounded=false;p->hurt_frame=9;
                } else if(f>=9 && f<=53) {
                    if(down && f>9) p->hurt_frame=54;
                    else if(f<53) ++p->hurt_frame;
                } else if(f>=54 && f<106) ++p->hurt_frame;
            } else {
                if(!(f==3 && !down) && f<69) ++p->hurt_frame;
                if(p->hurt_frame==15) p->hurt_frame=23;
            }
        } else if(p->source_step) {
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
/* _root.groundFriction(v, subtract, divide) in original units. */
static float gf(float v,float subtract,float divide) { return copysignf(fmaxf(0,fabsf(v)-subtract)/divide,v); }
/* The FLUDD frontflip (Hfrontflip 1216 / R / T, 27 frames, frame 27 ends the
   attack): its "frame" child's rotation per frame; DiveRot = that + 90. */
static const float flip_rotation[27] = {0,-30.1f,-59.4f,-87.2f,-113.8f,-139.4f,-163.7f,173.2f,151.2f,130.3f,110.6f,92,75,
                                        58.7f,43.8f,30,21.9f,19.3f,17,14.5f,12.1f,9.6f,7.3f,4.8f,2.3f,0,0};
static bool flipping(const Player *p) { return p->jump_stage==3 && p->jump_time<27.0f/32 && !p->grounded; }
/* CharCode's "attack == false" for the FLUDD code (and Controll). */
static bool fludd_free(const Player *p)
{
    return !flipping(p) && p->action==ACTION_NONE && !p->pounding && p->spin_time==0 && p->landing_time<=0 && !p->ejump &&
           !p->sleeping && !p->drowning && !p->reading_sign && p->stun<=0;
}
bool player_hover_active(const Player *p,GameInput in)
{
    return p->has_fludd && p->nozzle==NOZZLE_HOVER && in.nozzle_held && p->power>0 && p->water>0 && fludd_free(p);
}
/* The FLUDD, one 32 Hz frame at a time as CharCode and its clips run it
   (frame_4 CharCode; Rocket WaterBlast; HDive 1200 / Hfrontflip 1216 DiveRot):
   WaterAmount is p->water (TotalWater 10000 = 100), Power 0..100.
   * Hover (KeySPACE, Power > 0, WaterAmount > 0, no attack): after the frame's
     gravity (applied here exactly while hovering in the air), yspeed *= 0.8;
     yspeed -= (Hoverspeed * 10 - yspeed) / 10 * (Power / 175 + 0.75);
     WaterAmount -= 7; Power -= 1.5; xspeed = groundFriction(xspeed, 0.05, 1.03).
     In the air Right / Left also add -min((xspeed - 5) / 20, 0) and tilt Mario
     5 degrees a frame. During a dive: yspeed *= 0.98, xspeed *= 0.97 and a
     push of Hoverspeed along DiveRot (half sideways in the air).
   * Rocket (Power >= 100): the charge clip's frames 1-9 run forward while held
     and back when not; from 10 it runs on (yspeed *= 0.97), 15 sets Power 1,
     16 blasts (WaterAmount -= 500, yspeed = min(-25 + max(yspeed / 3, 0),
     yspeed)) and 16-23 push yspeed -= 25 / 50 (and 25 / 45 in water), Right /
     Left xspeed += / -= 0.7 with the tilt.
   * Turbo (Power >= 100): xspeed -= (xspeed -/+ 75) / 30 on the ground (not
     ice), 37.5 otherwise; WaterAmount -= 5; in water yspeed -= gravity / 3 and
     Up / Down steer by gravity / 1.5 (10 degrees a frame); at the surface it
     runs on the water. In a dive: WaterAmount -= 10 and a half push along
     DiveRot - 90.
   * Not spraying and not Hover: Power += 1 a frame (CharCode's end). */
void player_fludd_step(Player *p,GameInput in)
{
    const bool right=in.horizontal>0.5f,left=in.horizontal<-0.5f;
    const bool up=in.vertical<-0.5f || in.jump_held,down=in.vertical>0.5f || in.crouch_held;
    if(!p->has_fludd) {p->jet_active=false;p->rocket_charge=p->rocket_boost=0;p->rocket_frame=1;p->char_rotation=p->tilt_draw=0;return;}
    const bool space=in.nozzle_held;
    const bool free=fludd_free(p);
    /* attackFrame "Dive" or "frontflip": the FLUDD pushes along DiveRot */
    const bool diving=p->action==ACTION_DIVE || flipping(p);
    if(flipping(p)) {
        unsigned f=(unsigned)(p->jump_time*32);
        p->dive_rot=flip_rotation[f<27 ? f : 26]+90;
    }
    bool jet=false;
    if(p->nozzle==NOZZLE_HOVER && space && p->power>0 && p->water>0 && (free || diving)) jet=true;
    if(p->nozzle==NOZZLE_TURBO && space && p->power>=100 && p->water>0 && (free || diving)) jet=true;
    if(p->nozzle==NOZZLE_ROCKET && p->rocket_frame>1) jet=true;
    if(p->nozzle==NOZZLE_ROCKET && space && p->power>=100 && p->water>0) jet=true;
    if(p->action==ACTION_BONK || p->action==ACTION_HURT) jet=false;
    p->jet_active=jet;
    /* the tilt as drawn: eased toward Char's _rotation every 60 Hz tick (it
       changes in 32 Hz steps of 5 degrees) */
    p->tilt_draw+=(p->char_rotation-p->tilt_draw)*0.5f;
    if(!p->source_step) {p->vx=fmaxf(-1400,fminf(1400,p->vx));return;}
    float xs=p->vx/ORIGINAL_VELOCITY,ys=p->vy/ORIGINAL_VELOCITY;
    p->water_running=false;
    const float facing=p->facing_left ? 1 : -1;   /* _xscale / 100 */
    const float deg=0.01745329252f;
    if(p->nozzle==NOZZLE_HOVER && space && p->power>0 && p->water>0) {
        if(free) {
            if(!p->grounded && !p->submerged) {   /* the frame's gravity, exactly (player_step left it out) */
                ys+=1;
                if(ys>0) ys=gf(ys,0.2f,1.05f);
                ys=gf(ys,0,1.001f); xs=gf(xs,0,1.001f);
                if(in.jump_held) ys-=0.1f;   /* KeyUP: yspeed -= risespeed */
                if(down) ys+=0.1f;           /* KeyDOWN: yspeed += risespeed */
                /* KeyRIGHT / KeyLEFT: xspeed -= min((xspeed - airwalkspeed) / (airmaxWalkspeed / 3), 0),
                   and with the Hover spraying again over airmaxWalkspeed, tilting 5 degrees */
                if(right) {xs-=fminf((xs-5)/(20.0f/3),0);xs-=fminf((xs-5)/20,0);p->char_rotation+=5;}
                if(left) {xs-=fmaxf((xs+5)/(20.0f/3),0);xs-=fmaxf((xs+5)/20,0);p->char_rotation-=5;}
            }
            ys*=0.8f;
            ys-=(0.92f*10-ys)/10*(p->power/175+0.75f);
            p->water-=0.07f;p->power-=1.5f;
            xs=gf(xs,0.05f,1.03f);
            p->jump_cut=true;   /* the hover is not a jump to cut */
            p->grounded=false;
        } else if(diving) {
            const float r=p->dive_rot*deg;
            p->water-=0.07f;p->power-=1.5f;
            ys*=0.98f;xs*=0.97f;
            if(!p->grounded) {ys-=sinf(r)*0.92f;xs-=facing*cosf(r)*0.92f/2;}
            else xs-=facing*cosf(r)*0.92f;
        }
    }
    if(p->nozzle==NOZZLE_TURBO && space && p->power>=100 && p->water>0) {
        if(free) {
            p->water-=0.05f;
            const float speed=p->grounded && p->material!=TILE_ICE ? 75 : 37.5f;
            xs-=(xs+facing*speed)/30;
            if(p->submerged) {
                ys-=1.0f/3;
                if(up) {p->char_rotation-=facing*10;ys-=1/1.5f;}
                if(down) {p->char_rotation+=facing*10;ys+=1/1.5f;}
            } else if(p->water_below) {   /* running on the water */
                if(p->water_at_feet) {ys-=1.0f/5;p->y-=1.375f;}
                p->water=fminf(100,p->water+0.5f);
                ys=fminf(0,ys);
                if(!right && !left) xs=gf(xs,0.1f,1.05f);
                xs-=(xs+facing*37.5f)/30;
                p->water_running=true;
                if(up) ys=fminf(ys,-10/1.5f);
            }
        } else if(diving) {
            const float r=(p->dive_rot-90)*deg;
            p->water-=0.1f;
            ys-=sinf(r)/2;
            xs-=facing*cosf(r)/2;
        }
    }
    /* CharCode adds 1 Power a frame in the air when not spraying (not Hover);
       the Rocket gets one blast per jump (user decision): its Power only
       refills on the ground or in water. */
    if(!space && p->nozzle!=NOZZLE_HOVER && p->nozzle!=NOZZLE_ROCKET) p->power+=1;
    /* Rocket WaterBlast (24 frames; chargeframe kept between uses). The clip is
       only in Mario Main's Rocket frames: an attack pauses it; a hit or a wall
       bonk (rocket_boost cleared) ends the blast. */
    if(p->nozzle==NOZZLE_ROCKET && (free || p->submerged)) {
        const bool blast=space && p->power>=100 && p->water>0;
        unsigned f=p->rocket_frame ? p->rocket_frame : 1;
        if(f>=16 && p->rocket_boost==0) f=1;
        if(f<10) {
            if(blast) {if(f==1) sfx_play(SFX_ROCKET_CHARGE,70,0);++f;}   /* port-made sound: the original is silent */
            else if(f>1) --f;
            if(f==10) ys*=0.97f;
        } else if(f<16) {
            ++f; ys*=0.97f;
            if(f==15) p->power=1;
            if(f==16) {
                sfx_play(SFX_ROCKET_BLAST,85,0);
                p->water-=5;p->power=0;p->rocket_spent=true;
                ys=fminf(-25+fmaxf(ys/3,0),ys);
                p->grounded=false;p->jump_cut=true;
            }
        } else {
            p->power=0;
            ys-=25.0f/50;
            if(p->submerged) ys-=25.0f/45;
            if(right) {xs+=0.7f;p->char_rotation+=5;}
            if(left) {xs-=0.7f;p->char_rotation-=5;}
            if(++f>=24) f=1;
        }
        p->rocket_frame=f;
        p->rocket_charge=f>1 && f<16 ? (f-1)/32.0f : 0;
        p->rocket_boost=f>=16 ? (24-f)/32.0f : 0;
    } else if(p->nozzle!=NOZZLE_ROCKET) {p->rocket_frame=1;p->rocket_charge=p->rocket_boost=0;}
    /* CharCode end: in the air _rotation = groundFriction(_rotation, 3, 1.3),
       kept within +/-30; on the ground CharRotationCode (the port draws no slope tilt). */
    p->char_rotation=p->grounded ? 0 : fmaxf(-30,fminf(30,gf(p->char_rotation,3,1.3f)));
    p->water=fmaxf(0,fminf(100,p->water));
    p->power=fmaxf(0,fminf(100,p->power));
    p->vx=fmaxf(-1400,fminf(1400,xs*ORIGINAL_VELOCITY));
    p->vy=ys*ORIGINAL_VELOCITY;
}
