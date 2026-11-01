/* Playing the original tutorial area 8-E1-1 on the exported collision. */
#include "course_world.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

static unsigned char blob[1 << 20];
static CoursePack pack;
static CourseRuntime course;
static World world;

static float feet_x(const Player *p) {return course_cx(&course,p->x+PLAYER_WIDTH/2);}
static float feet_y(const Player *p) {return course_cy(&course,p->y+player_height(p));}
static Player spawn(void)
{
    Player p;player_init(&p,0,0);p.has_fludd=false;
    course_place_player(&course,&p,0,0);
    return p;
}
static void test_spawn_lands_on_original_ground(void)
{
    Player p=spawn();
    for(int i=0;i<120;++i) player_step(&p,&world,(GameInput){0});
    assert(p.grounded && fabsf(feet_x(&p))<0.5f);
    /* Surface at y = 93; the original climb rule (while (x, y-3) is solid: y--)
       settles the feet inside the ground, just under y = 96. */
    assert(feet_y(&p)>95 && feet_y(&p)<96);
    assert(!p.jet_active && p.has_fludd==false);
    player_step(&p,&world,(GameInput){.nozzle_held=true});assert(!p.jet_active); /* no FLUDD yet */
}
static float walk_right(Player *p,int ticks,bool hop)
{
    float best=feet_x(p);
    for(int i=0;i<ticks;++i) {
        const bool stuck=hop && p->grounded && fabsf(p->vx)<20 && i%20==0;
        player_step(p,&world,(GameInput){.horizontal=1,.jump_pressed=stuck,.jump_held=hop && !p->grounded});
        best=fmaxf(best,feet_x(p));
    }
    return best;
}
static void test_barrier_and_controls_sign(void)
{
    course.platform_frame=0; /* TotalStars==0, Mario left of the start sign */
    Player p=spawn();
    const float reached=walk_right(&p,600,true);
    assert(reached<316 && reached>250); /* stopped by the barrier at x = 316 */
    course.platform_frame=1; /* "controls" sign completefunction: gotoAndStop(2) */
    assert(walk_right(&p,300,false)>400);
}
/* A simple player: hold right; when blocked, chain jumps (the area's own hint:
   "Jumping twice in a row makes you jump higher"), holding Jump through each. */
typedef struct { int hold, since_land; } Bot;
static GameInput bot_input(Bot *b,const Player *p,int tick)
{
    GameInput in={.horizontal=1};
    if(p->grounded) ++b->since_land; else b->since_land=0;
    const bool blocked=p->grounded && (p->wall_normal || fabsf(p->vx)<60);
    if(b->hold>0) {--b->hold;in.jump_held=true;}
    else if((blocked || (p->jump_stage>0 && b->since_land>0 && b->since_land<6)) && p->grounded) {
        in.jump_pressed=in.jump_held=true;b->hold=24;
    }
    if(p->submerged) {in.jump_held=tick%12<6;in.jump_pressed=tick%12==0;}
    return in;
}
static float run(Player *p,int seconds,float goal)
{
    Bot bot={0};float best=feet_x(p);
    for(int i=0;i<60*seconds && best<goal;++i) {
        player_step(p,&world,bot_input(&bot,p,i));
        best=fmaxf(best,feet_x(p));
        assert(isfinite(p->x) && isfinite(p->y) && feet_y(p)<400);
    }
    return best;
}
static void test_route_segments(void)
{
    course.platform_frame=1;
    /* Spawn to the far side of the central rock: hills, pool edge and the
       ~72 px ledge at x = 792-808 need the chained jump the hint describes. */
    Player p=spawn();
    assert(run(&p,40,1000)>=1000);
    /* Far bank to warp1 (1855.8, -126): slopes, steps and the ice patch. The
       gap between needs the Falling Log object (not yet ported). */
    p=spawn();course_place_player(&course,&p,1150,-60);
    for(int i=0;i<90;++i) player_step(&p,&world,(GameInput){0});
    assert(p.grounded);
    assert(run(&p,40,1855)>=1840);
}
static void test_camera_stays_inside_camedge(void)
{
    course.platform_frame=1;
    Player p=spawn();Camera cam;camera_init(&cam,&p,&world,640);
    Bot bot={0};
    for(int i=0;i<60*40;++i) {
        player_step(&p,&world,bot_input(&bot,&p,i));
        camera_step(&cam,&p,&world,640);
        const float hw=640/course_view_zoom()/COURSE_SCALE/2,hh=COURSE_VIEW_HEIGHT/2;
        assert(!course_area_hit(course.area,COURSE_CAMEDGE,0,cam.cx+hw-2,cam.cy));
        assert(!course_area_hit(course.area,COURSE_CAMEDGE,0,cam.cx-hw+2,cam.cy));
        assert(!course_area_hit(course.area,COURSE_CAMEDGE,0,cam.cx,cam.cy-hh+2));
        assert(!course_area_hit(course.area,COURSE_CAMEDGE,0,cam.cx,cam.cy+hh-2));
        assert(fabsf(cam.cx-feet_x(&p))<300); /* trails Mario, never loses him */
    }
}
int main(void)
{
    FILE *f=fopen("../../assets/generated/course_8e1.bin","rb");assert(f);
    const size_t n=fread(blob,1,sizeof(blob),f);fclose(f);
    assert(course_pack_parse(blob,n,&pack));
    course_runtime_init(&course,course_pack_area(&pack,"8-E1-1"));
    world=course_world(&course);
    test_spawn_lands_on_original_ground();test_barrier_and_controls_sign();
    test_route_segments();test_camera_stays_inside_camedge();
    puts("4 course play (8-E1-1 collision/camera) test groups passed");return 0;
}
