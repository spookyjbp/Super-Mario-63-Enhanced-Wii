#include "game.h"
#include "source_movement.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
static void test_backflip_and_down(void)
{
    const Rect shapes[]={{0,1000,4000,40}};
    const World w={.solids=shapes,.count=1,.width=4000,.height=2000};
    for(unsigned left=0;left<2;++left) {
        Player p;player_init(&p,1500,956);player_step(&p,&w,(GameInput){0});
        p.facing_left=left!=0;
        for(int i=0;i<3;++i) player_step(&p,&w,(GameInput){.crouch_held=true});
        assert(p.action==ACTION_DIVE && p.vx==0 && p.vy==0);
        for(int i=0;i<20;++i) player_step(&p,&w,(GameInput){.crouch_held=true,.jump_pressed=i==0,.jump_held=true});
        /* Down+Up alone plays the original recovery; no invented stationary roll or jump. */
        assert(p.grounded && p.vx==0 && p.vy==0 && p.action!=ACTION_BACKFLIP && p.action!=ACTION_ROLL);
        for(int i=0;i<3;++i) player_step(&p,&w,(GameInput){.crouch_held=true});
        for(int i=0;i<2 && p.action!=ACTION_BACKFLIP;++i)
            player_step(&p,&w,(GameInput){.crouch_held=true,.jump_pressed=i==0,.jump_held=true,.horizontal=left?1:-1});
        assert(p.action==ACTION_BACKFLIP && p.facing_left==(left!=0));
        assert(fabsf(p.vx-(left?264:-264))<0.01f);
        for(int i=0;i<15;++i) player_step(&p,&w,(GameInput){.jump_held=true,.horizontal=left?-1:1});
        assert(p.action==ACTION_BACKFLIP && p.facing_left==(left!=0));
    }
}
static void test_source_clock(void)
{
    Player p={0};unsigned count=0;
    for(int i=0;i<60;++i) {player_source_begin(&p,(GameInput){0});if(p.source_step)++count;}
    assert(count==32 && p.source_phase==0);
}
static void test_water_recurrence_and_strokes(void)
{
    uint8_t tiles[64];for(unsigned i=0;i<64;++i)tiles[i]=TILE_WATER;
    const Level l={1,8,8,64,tiles,NULL,0};
    const World w={.width=512,.height=512,.level=&l};
    Player p;player_init(&p,100,100);p.submerged=true;p.source_step=true;
    assert(player_swim_step(&p,&w,(GameInput){0}));
    const float expected=(((1.0f/3-0.05f)/1.01f)/1.001f-0.05f)/1.1f*44;
    assert(fabsf(p.vy-expected)<0.001f && p.vy>0); /* Original slowly sinks at rest. */
    {   /* the jump button does nothing in water (port control choice) */
        Player q=p;player_swim_step(&q,&w,(GameInput){.jump_held=true,.jump_pressed=true});
        Player r=p;player_swim_step(&r,&w,(GameInput){0});
        assert(q.vy==r.vy && !q.swim_stroke);
    }
    player_swim_step(&p,&w,(GameInput){.vertical=-1});
    const float first=p.vy;assert(first<-200 && p.swim_stroke);
    player_swim_step(&p,&w,(GameInput){.vertical=-1});
    assert(p.vy>first); /* Held Up is sustained stroke, not another -6 impulse. */
    for(int i=0;i<6;++i)player_swim_step(&p,&w,(GameInput){0});
    assert(!p.swim_stroke);
    const float before=p.vy;
    player_swim_step(&p,&w,(GameInput){.vertical=-1});assert(p.vy<before-150);
    p.vy=8*44;p.submerged=false;
    player_swim_step(&p,&w,(GameInput){0});assert(p.vy<8*44*0.7f*0.9f);
}
static void test_repeated_spins_are_bounded(void)
{
    const World w={.width=10000,.height=100000};
    Player plain,spin;player_init(&plain,1000,1000);spin=plain;
    unsigned starts=0;bool active=false;
    for(int i=0;i<360;++i) {
        player_step(&plain,&w,(GameInput){0});
        player_step(&spin,&w,(GameInput){.spin_held=true,.spin_pressed=true});
        if(spin.spin_time>0 && !active) ++starts;
        active=spin.spin_time>0;
        assert(isfinite(spin.vy) && spin.y>=1000 && spin.spin_speed<100);
    }
    assert(starts>2 && spin.y<plain.y && spin.vy>0);
    Player boosted;player_init(&boosted,1000,1000);int dir=1;
    for(int i=0;i<360;++i) {
        if(boosted.source_phase+32>=60) dir=-dir;
        player_step(&boosted,&w,(GameInput){.spin_held=true,.spin_pressed=true,.horizontal=(float)dir});
        assert(isfinite(boosted.vy) && boosted.y>=1000 && boosted.spin_speed<100);
    }
    assert(boosted.vy>0);
    Player p;player_init(&p,1000,1000);p.source_step=true;p.spin_requested=true;p.vy=-200;
    player_spin_prepare(&p,(GameInput){0});assert(p.spin_time==0);
    p.spin_requested=true;p.vy=0;player_spin_prepare(&p,(GameInput){0});assert(p.spin_speed==15);
    player_spin_update(&p,(GameInput){0});const float speed=p.spin_speed;
    p.spin_requested=true;player_spin_prepare(&p,(GameInput){.spin_held=true});
    assert(p.spin_speed==speed); /* Button spam cannot refresh an active attack. */
    p.nozzle=NOZZLE_HOVER;p.jet_active=false;const float water=p.water;
    player_fludd_step(&p,(GameInput){.nozzle_held=true});assert(!p.jet_active && p.water==water);
}
/* Mario Ground Pound.xml: frames 0-11 hang, frame 12 yspeed=8, fall recurrence,
   "hit" 27-34 (xspeed*=0.8), "end" 35-41, attack cleared at 42. */
static void test_ground_pound_source_timing(void)
{
    const Rect shapes[]={{0,1000,4000,40}};
    const World w={.solids=shapes,.count=1,.width=4000,.height=2000};
    Player p;player_init(&p,1500,200);p.vx=300;
    player_step(&p,&w,(GameInput){0});
    player_step(&p,&w,(GameInput){.pound_pressed=true});
    assert(p.pounding && p.pound_frame==0 && p.vx==0 && p.vy==0);
    const float y=p.y;int ticks=1;Rect hit;
    while(p.pound_windup>0) {player_step(&p,&w,(GameInput){.horizontal=1});++ticks;assert(p.vx==0 && (p.y==y || p.pound_windup==0));}
    assert(ticks>=22 && ticks<=24 && fabsf(p.vy-8*44)<0.01f && player_attack_hitbox(&p,&hit));
    float last=p.vy;
    for(int i=0;i<300 && !p.grounded;++i) {
        player_step(&p,&w,(GameInput){0});
        if(!p.grounded) assert(p.vy>=last-0.01f && p.vy<27*44); /* Accelerates toward ~26.5 units, never past. */
        if(!p.grounded) last=p.vy;
    }
    assert(p.grounded && !p.pounding && p.pound_frame==27 && last>20*44);
    p.vx=200;int locked=0;float settled=1e9f;
    while(p.landing_time>0) {
        player_step(&p,&w,(GameInput){.horizontal=1,.jump_pressed=true,.jump_held=true});++locked;
        if(p.landing_time>0) {assert(p.grounded);settled=fabsf(p.vx);} /* No jump or walk during the lock. */
    }
    assert(locked>=29 && locked<=31 && p.pound_frame==42 && settled<1);
}
int main(void)
{
    test_backflip_and_down();test_source_clock();test_water_recurrence_and_strokes();test_repeated_spins_are_bounded();test_ground_pound_source_timing();
    puts("5 v0.4.1 source-physics correction groups passed");return 0;
}
