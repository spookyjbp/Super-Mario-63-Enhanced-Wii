#include "game.h"
#include "world_query.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

static const Rect ground[]={{0,400,2400,80}};
static const World flat={.solids=ground,.count=1,.width=2400,.height=960,.spawn_x=100,.spawn_y=356};
static Player standing(void)
{Player p;player_init(&p,100,356);player_step(&p,&flat,(GameInput){0});assert(p.grounded);return p;}
static void ticks(Player *p,const World *w,GameInput in,unsigned n)
{for(unsigned i=0;i<n;++i)player_step(p,w,in);}
static void test_low_dive_and_backflip(void)
{
    Player p=standing();
    ticks(&p,&flat,(GameInput){.crouch_held=true},3); /* Dive.xml frame 0 is one source frame. */
    assert(p.action==ACTION_DIVE && player_height(&p)==26 && p.y+26==400);
    assert(p.animation==PLAYER_CROUCH);
    player_step(&p,&flat,(GameInput){.jump_pressed=true,.jump_held=true,.horizontal=-1,.crouch_held=true});
    assert(p.action==ACTION_BACKFLIP && p.vy<-500 && p.vx<0 && !p.grounded);
    assert(!p.facing_left && player_height(&p)==44);
    p=standing();p.vx=200;
    ticks(&p,&flat,(GameInput){.crouch_held=true},5);
    player_step(&p,&flat,(GameInput){.jump_pressed=true,.jump_held=true,.crouch_held=true});
    assert(p.action==ACTION_ROLL && p.vy<0);
    ticks(&p,&flat,(GameInput){.jump_held=true},120);
    assert(p.grounded && p.action==ACTION_NONE);
}
static void test_crouch_ceiling(void)
{
    const Rect shapes[]={{0,400,1000,40},{80,340,120,25}};
    const World w={.solids=shapes,.count=2,.width=1000,.height=600};
    Player p=standing();p.height=26;p.y=374;p.action=ACTION_DIVE;
    ticks(&p,&w,(GameInput){0},30);
    assert(player_height(&p)==26 && !player_stand_clear(&p,&w));
    p.x=250;
    ticks(&p,&w,(GameInput){0},16); /* Recovery frames 3..9 at the 32 Hz source clock. */
    assert(player_height(&p)==44 && p.y==356 && p.action==ACTION_NONE);
}
static void test_dive_and_pound_transition(void)
{
    Player p;player_init(&p,100,100);p.vy=-50;
    player_step(&p,&flat,(GameInput){.crouch_held=true});
    assert(p.action==ACTION_DIVE && p.vx>200);
    player_step(&p,&flat,(GameInput){.pound_pressed=true});
    assert(p.pounding && p.action==ACTION_NONE && p.pound_windup>0);
    ticks(&p,&flat,(GameInput){0},100);
    assert(p.grounded && !p.pounding && player_height(&p)==44);
}
static void test_dash_and_wall_rebound(void)
{
    Player p=standing();
    player_step(&p,&flat,(GameInput){.horizontal=1});
    player_step(&p,&flat,(GameInput){0});
    ticks(&p,&flat,(GameInput){.horizontal=1},40);
    assert(p.dashing && p.vx>280);
    const Rect wall[]={{200,0,32,400},{0,400,1000,40}};
    const World w={.solids=wall,.count=2,.width=1000,.height=600};
    player_init(&p,170,250);p.vx=800;
    player_step(&p,&w,(GameInput){.horizontal=1});
    assert(p.action==ACTION_BONK && p.vx==0 && p.bonk_direction==-1);
    ticks(&p,&w,(GameInput){0},19);
    /* hurt2 frame 11: knocked back from the wall, still in the hurt2 clip. */
    assert(p.action==ACTION_BONK && p.bonk_frame>=10 && p.vx<0 && p.vy<0 && !p.facing_left);
    ticks(&p,&w,(GameInput){0},300);
    assert(p.action==ACTION_BONK && p.bonk_frame==22 && p.grounded); /* lying: waits for a key */
    ticks(&p,&w,(GameInput){.jump_held=true},2);
    ticks(&p,&w,(GameInput){0},30);
    assert(p.action==ACTION_NONE);                                /* frame 32: control back */
    player_init(&p,170,356);p.grounded=true;p.vx=400;p.dashing=true;
    player_step(&p,&w,(GameInput){.horizontal=1});
    assert(p.action==ACTION_BONK && !p.dashing);
}
static void test_tiles_and_slopes(void)
{
    /* / rises from floor y=256 to plateau y=192, then \\ descends. */
    const uint8_t tiles[]={0,0,0,0,0,0,0,0,
                           0,0,0,0,0,0,0,0,
                           0,0,0,0,0,0,0,0,
                           0,0,3,1,4,0,0,0,
                           1,1,1,1,1,1,1,1};
    const Level l={1,8,5,64,tiles,NULL,0};
    const World w={.width=512,.height=320,.level=&l};
    Player p;player_init(&p,92,212);
    player_step(&p,&w,(GameInput){0});assert(p.grounded);
    float highest=p.y;
    for(int i=0;i<65;++i) {
        player_step(&p,&w,(GameInput){.horizontal=1});
        highest=fminf(highest,p.y);
        assert(p.grounded);
    }
    assert(highest<=148.1f && p.x>320 && p.y>highest);
    WorldQuery q;WorldShape s;unsigned count=0;
    world_query_begin(&q,&w,(Rect){0,0,64,64});
    while(world_query_next(&q,&s))++count;
    assert(count==0); /* Query culling, not scanning unrelated terrain. */
    assert(!world_water_at(&w,-5,20,NULL));
}
static void test_one_way_platform(void)
{
    const uint8_t tiles[]={0,0,0,0,0,2,0,0,1,1,1,1};
    const Level l={1,4,3,64,tiles,NULL,0};
    const World w={.width=256,.height=192,.level=&l};
    Player p;player_init(&p,70,85);p.vy=-400;p.jump_cut=true;
    player_step(&p,&w,(GameInput){.jump_held=true});assert(p.vy<0);
    for(int i=0;i<90&&!p.grounded;++i)player_step(&p,&w,(GameInput){.jump_held=true});
    assert(p.grounded && fabsf(p.y+player_height(&p)-64)<0.01f);
}
static void test_materials(void)
{
    const uint8_t tiles[]={0,0,0,0,0,0,0,0,1,6,7,8};
    const Level l={1,4,3,64,tiles,NULL,0};
    const World w={.width=256,.height=192,.spawn_x=0,.spawn_y=84,.level=&l};
    Player ice,normal;
    player_init(&ice,70,84);player_init(&normal,5,84);
    player_step(&ice,&w,(GameInput){0});player_step(&normal,&w,(GameInput){0});
    assert(ice.material==TILE_ICE);
    ice.vx=normal.vx=200;
    player_step(&ice,&w,(GameInput){0});player_step(&normal,&w,(GameInput){0});
    assert(ice.vx>normal.vx);
    Player puddle;player_init(&puddle,135,84);puddle.water=50;
    ticks(&puddle,&w,(GameInput){0},5);assert(puddle.water>50);
    assert(world_hazard_at(&w,220,130));
    Game g;game_init(&g,&w,640);g.player.x=210;g.player.y=84;
    game_step(&g,&w,(GameInput){0},640);assert(g.player.x==w.spawn_x);
}
static void test_water_transitions(void)
{
    const uint8_t tiles[]={0,0,0,0,0,5,5,0,0,5,5,0,1,1,1,1};
    const Level l={1,4,4,64,tiles,NULL,0};
    const World w={.width=256,.height=256,.level=&l};
    Player p;player_init(&p,80,80);p.vx=400;p.pounding=true;
    player_step(&p,&w,(GameInput){0});
    assert(p.submerged && !p.pounding && p.vx<400 && p.animation==PLAYER_SWIM);
    p.vx=0;
    ticks(&p,&w,(GameInput){.spin_held=true},2);assert(p.animation==PLAYER_SPIN);
    ticks(&p,&w,(GameInput){0},30);
    assert(p.spin_time==0);
    const float before_down=p.vy;
    ticks(&p,&w,(GameInput){.crouch_held=true},10);assert(p.vy>before_down);
    ticks(&p,&w,(GameInput){.jump_held=true},3);assert(p.vy<0);
    p.x=10;
    player_step(&p,&w,(GameInput){0});assert(!p.submerged);
}
static void test_nozzles(void)
{
    Player p;player_init(&p,100,100);p.vy=100;
    ticks(&p,&flat,(GameInput){.nozzle_held=true},15);
    assert(p.jet_active && p.vy<100 && p.power<100 && p.water<100);
    p.water=0;p.power=0;
    player_step(&p,&flat,(GameInput){.nozzle_held=true});assert(!p.jet_active);
    player_init(&p,100,200);p.nozzle=NOZZLE_ROCKET;
    ticks(&p,&flat,(GameInput){.nozzle_held=true},29);
    assert(p.rocket_boost>0 && p.vy<-900 && p.water<=95 && p.power==0);
    player_init(&p,100,100);p.nozzle=NOZZLE_ROCKET;
    ticks(&p,&flat,(GameInput){.nozzle_held=true},10);
    ticks(&p,&flat,(GameInput){0},15);assert(p.rocket_charge==0);
    player_init(&p,100,100);p.nozzle=NOZZLE_TURBO;
    ticks(&p,&flat,(GameInput){.nozzle_held=true},30);
    assert(p.vx>400 && p.water<100);
    player_step(&p,&flat,(GameInput){.nozzle_next_pressed=true});
    assert(p.nozzle==NOZZLE_HOVER && p.rocket_charge==0);
    uint8_t water_tiles[64];
    for(unsigned i=0;i<64;++i) water_tiles[i]=TILE_WATER;
    const Level water_level={1,16,4,64,water_tiles,NULL,0};
    const World water_world={.width=1024,.height=256,.level=&water_level};
    player_init(&p,100,80);p.nozzle=NOZZLE_TURBO;
    ticks(&p,&water_world,(GameInput){.nozzle_held=true},30);
    assert(p.submerged && p.vx>300);
    GameInput pending={0};
    game_input_latch(&pending,(GameInput){.nozzle_held=true,.crouch_held=true,.vertical=1,.nozzle_next_pressed=true});
    game_input_consume(&pending);
    assert(pending.nozzle_held && pending.crouch_held && pending.vertical==1 && !pending.nozzle_next_pressed);
}
static void test_pause_preserves_nozzle_state(void)
{
    for(unsigned nozzle=0;nozzle<NOZZLE_COUNT;++nozzle) {
        Game g;game_init(&g,&flat,640);g.player.nozzle=(Nozzle)nozzle;
        g.player.y=100;
        game_step(&g,&flat,(GameInput){.nozzle_held=true},640);
        game_step(&g,&flat,(GameInput){.pause_pressed=true},640);
        const float water=g.player.water,power=g.player.power,charge=g.player.rocket_charge;
        for(unsigned tick=0;tick<120;++tick)
            game_step(&g,&flat,(GameInput){.nozzle_held=true,.crouch_held=true},640);
        assert(g.paused && g.player.water==water && g.player.power==power && g.player.rocket_charge==charge);
        game_step(&g,&flat,(GameInput){.cancel_pressed=true},640);
        assert(!g.paused);
    }
}
int main(void)
{
    test_low_dive_and_backflip();test_crouch_ceiling();test_dive_and_pound_transition();
    test_dash_and_wall_rebound();test_tiles_and_slopes();test_one_way_platform();
    test_materials();test_water_transitions();test_nozzles();test_pause_preserves_nozzle_state();
    puts("10 v0.4 movement/environment test groups passed");return 0;
}
