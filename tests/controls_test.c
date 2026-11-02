/* Regression tests for GameCube Down, crouching and backflip activation.
   Source: Runouw/Super-Mario-63 83e81fb, Super_Mario_63_2010/LIBRARY/Char/
   Mario CLIPS/Mario (None) Dive.xml onEnterFrame and DOMDocument.xml:5847. */
#include "game.h"
#include "input_map.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

static const Rect floor_rect[]={{0,1000,4000,40}};
static const World flat={.solids=floor_rect,.count=1,.width=4000,.height=2000};
static Player standing(bool left)
{Player p;player_init(&p,1500,956);player_step(&p,&flat,(GameInput){0});assert(p.grounded);p.facing_left=left;return p;}
static void ticks(Player *p,GameInput in,int n){for(int i=0;i<n;++i)player_step(p,&flat,in);}
static float away(bool left){return left?1.0f:-1.0f;}

static void test_gamecube_down_reaches_gameplay(void)
{
    GameInput in=input_map_gamecube(GC_DOWN,GC_DOWN);
    assert(in.crouch_held && in.vertical==1 && in.menu_delta==1); /* D-pad Down. */
    in=input_map_gamecube(GC_DOWN|GC_LEFT|GC_A,GC_A);
    assert(in.crouch_held && in.horizontal==-1 && in.jump_pressed && in.jump_held);
    in=input_map_gamecube(GC_B,GC_B);
    assert(in.spin_pressed && in.spin_held && !in.crouch_held);   /* Spin mapping unchanged. */
    in=input_map_gamecube(GC_Z|GC_R,GC_Z|GC_L|GC_START);
    assert(in.pound_pressed && in.nozzle_held && in.nozzle_next_pressed && in.pause_pressed);
    /* The latch must carry held Down across catch-up ticks. */
    GameInput pending={0};game_input_latch(&pending,input_map_gamecube(GC_DOWN,GC_DOWN));
    game_input_consume(&pending);assert(pending.crouch_held);
}
/* New control scheme: D-pad movement only; Classic and sideways Wii Remote buttons. */
static void test_classic_mapping(void)
{
    GameInput in=input_map_classic(CC_RIGHT|CC_B,CC_B,0);
    assert(in.horizontal==1 && in.jump_pressed && in.jump_held && !in.spin_pressed);
    in=input_map_classic(CC_Y|CC_DOWN,CC_Y|CC_DOWN,0);
    assert(in.spin_pressed && in.crouch_held && in.vertical==1 && in.menu_delta==1);
    in=input_map_classic(CC_ZR,CC_L,0); assert(in.nozzle_held && in.pound_pressed);
    in=input_map_classic(0,CC_R,0); assert(in.pound_pressed);
    in=input_map_classic(0,CC_ZL|CC_PLUS,0); assert(in.nozzle_next_pressed && in.pause_pressed);
    in=input_map_classic(CC_A|CC_X,CC_A|CC_X,0);                     /* a and x do nothing */
    assert(!in.jump_pressed && !in.spin_pressed && !in.pound_pressed && !in.nozzle_held && !in.nozzle_next_pressed);
    assert(input_map_classic(0,0,1).zoom_axis==-1 && input_map_classic(0,0,-1).zoom_axis==1); /* right stick */
    assert(input_map_classic(0,0,0.3f).zoom_axis==0);
}
static GameInput wm(WiimoteState *w,unsigned *last,unsigned held)
{const unsigned down=held&~*last;*last=held;return input_map_wiimote(w,held,down);}
static void test_wiimote_mapping(void)
{
    WiimoteState w={0};unsigned last=0;
    /* Sideways: pad Up = left, Down = right, Right = up, Left = down (crouch). */
    assert(wm(&w,&last,WM_UP).horizontal==-1 && wm(&w,&last,WM_DOWN).horizontal==1);
    GameInput in=wm(&w,&last,WM_LEFT);assert(in.vertical==1 && in.crouch_held);
    in=wm(&w,&last,WM_RIGHT);assert(in.vertical==-1 && !in.crouch_held);
    in=wm(&w,&last,WM_2);assert(in.jump_pressed && in.jump_held);
    in=wm(&w,&last,WM_1);assert(in.spin_pressed && !in.jump_held);
    wm(&w,&last,0);
    /* B: FLUDD at once. */
    in=wm(&w,&last,WM_B);assert(in.nozzle_held && !in.pound_pressed);
    /* A joins within the window: stomp once, FLUDD stops, no nozzle switch. */
    in=wm(&w,&last,WM_B|WM_A);assert(in.pound_pressed && !in.nozzle_held && !in.nozzle_next_pressed);
    for(int i=0;i<20;++i){in=wm(&w,&last,WM_B|WM_A);assert(!in.pound_pressed && !in.nozzle_held && !in.nozzle_next_pressed);}
    wm(&w,&last,0);
    /* A then B within the window: stomp too. */
    in=wm(&w,&last,WM_A);assert(!in.nozzle_next_pressed);
    in=wm(&w,&last,WM_A|WM_B);assert(in.pound_pressed && !in.nozzle_held);
    wm(&w,&last,0);
    /* A alone: the nozzle switches once, after the window (or on release). */
    int switches=0;
    for(int i=0;i<30;++i) switches+=wm(&w,&last,WM_A).nozzle_next_pressed;
    assert(switches==1);
    wm(&w,&last,0);
    switches=wm(&w,&last,WM_A).nozzle_next_pressed;switches+=wm(&w,&last,0).nozzle_next_pressed;
    assert(switches==1);                                           /* quick tap */
    /* Spraying a while, then A: a nozzle switch, not a stomp. */
    for(int i=0;i<20;++i) in=wm(&w,&last,WM_B);
    assert(in.nozzle_held);
    bool pound=false;switches=0;
    for(int i=0;i<10;++i){in=wm(&w,&last,WM_B|WM_A);pound|=in.pound_pressed;switches+=in.nozzle_next_pressed;}
    assert(!pound && switches==1);
    wm(&w,&last,0);
    /* + / - zoom while held (after the window); together: pause once, no zoom. */
    for(int i=0;i<10;++i) in=wm(&w,&last,WM_PLUS);
    assert(in.zoom_axis==-1 && !in.pause_pressed);
    wm(&w,&last,0);
    for(int i=0;i<10;++i) in=wm(&w,&last,WM_MINUS);
    assert(in.zoom_axis==1);
    wm(&w,&last,0);
    in=wm(&w,&last,WM_PLUS);assert(in.zoom_axis==0);
    in=wm(&w,&last,WM_PLUS|WM_MINUS);assert(in.pause_pressed && in.zoom_axis==0);
    for(int i=0;i<10;++i){in=wm(&w,&last,WM_PLUS|WM_MINUS);assert(!in.pause_pressed && in.zoom_axis==0);}
    wm(&w,&last,0);
    assert(wm(&w,&last,WM_HOME).pause_pressed);
}
static void test_crouch_holds_with_slide_pose(void)
{
    for(int left=0;left<2;++left) {
        Player p=standing(left);
        ticks(&p,(GameInput){.crouch_held=true},3);
        assert(p.action==ACTION_DIVE && player_height(&p)==26 && p.animation==PLAYER_CROUCH);
        ticks(&p,(GameInput){.crouch_held=true},120);
        assert(p.action==ACTION_DIVE && p.animation==PLAYER_CROUCH && p.facing_left==(left!=0));
        /* Pushing away while crouched does not turn Mario around (attack==true). */
        ticks(&p,(GameInput){.crouch_held=true,.horizontal=away(left)},10);
        assert(p.action==ACTION_DIVE && p.facing_left==(left!=0) && p.grounded);
        /* Releasing Down plays recovery frames 3..9, then stands. */
        player_step(&p,&flat,(GameInput){0});
        assert(p.action==ACTION_DIVE);
        ticks(&p,(GameInput){0},16);
        assert(p.action==ACTION_NONE && player_height(&p)==44 && p.grounded);
    }
}
static int recovery_ticks(Nozzle nozzle)
{
    Player p=standing(false);p.nozzle=nozzle;
    ticks(&p,(GameInput){.crouch_held=true},3);
    int n=0;while(p.action==ACTION_DIVE && n<60) {player_step(&p,&flat,(GameInput){0});++n;}
    return n;
}
static void test_nozzle_dive_recovery(void)
{
    /* Slide (Hover).xml clears attack at frame 7; Turbo/Rocket slides at frame 9. */
    const int hover=recovery_ticks(NOZZLE_HOVER),turbo=recovery_ticks(NOZZLE_TURBO),rocket=recovery_ticks(NOZZLE_ROCKET);
    assert(turbo==rocket && turbo-hover>=3 && turbo-hover<=5);
}
static void test_backflip_activation_paths(void)
{
    for(int left=0;left<2;++left) {
        /* 1. Crouch, hold away, press A. */
        Player p=standing(left);ticks(&p,(GameInput){.crouch_held=true},3);
        GameInput flip={.crouch_held=true,.horizontal=away(left),.jump_pressed=true,.jump_held=true};
        for(int i=0;i<2 && p.action!=ACTION_BACKFLIP;++i) {player_step(&p,&flat,flip);flip.jump_pressed=false;}
        assert(p.action==ACTION_BACKFLIP && p.vy<-500 && !p.grounded && p.facing_left==(left!=0));
        assert(left ? p.vx>0 : p.vx<0); /* Travels opposite the facing. */
        /* 2. Crouch, hold A first, then push away (KeyUP is a held test). */
        p=standing(left);ticks(&p,(GameInput){.crouch_held=true},3);
        ticks(&p,(GameInput){.crouch_held=true,.jump_pressed=true,.jump_held=true},1);
        assert(p.action==ACTION_DIVE);
        for(int i=0;i<2 && p.action!=ACTION_BACKFLIP;++i)
            player_step(&p,&flat,(GameInput){.crouch_held=true,.jump_held=true,.horizontal=away(left)});
        assert(p.action==ACTION_BACKFLIP && p.facing_left==(left!=0));
        /* 3. Release Down, then away+A during the recovery frames. */
        p=standing(left);ticks(&p,(GameInput){.crouch_held=true},3);
        ticks(&p,(GameInput){0},3);
        assert(p.action==ACTION_DIVE && p.dive_recovering);
        for(int i=0;i<2 && p.action!=ACTION_BACKFLIP;++i)
            player_step(&p,&flat,(GameInput){.horizontal=away(left),.jump_pressed=i==0,.jump_held=true});
        assert(p.action==ACTION_BACKFLIP && p.facing_left==(left!=0));
        /* Facing and flip persist through the airborne 22-frame clip. */
        ticks(&p,(GameInput){.jump_held=true,.horizontal=-away(left)},15);
        assert(p.action==ACTION_BACKFLIP && p.facing_left==(left!=0));
        ticks(&p,(GameInput){0},240);
        assert(p.grounded && p.action==ACTION_NONE);
    }
}
static void test_not_a_backflip(void)
{
    for(int left=0;left<2;++left) {
        /* Down+A without the away direction: no flip, no jump. */
        Player p=standing(left);ticks(&p,(GameInput){.crouch_held=true},3);
        float top=p.y;
        for(int i=0;i<40;++i) {
            player_step(&p,&flat,(GameInput){.crouch_held=i<3,.jump_pressed=i==0,.jump_held=true});
            assert(p.action!=ACTION_BACKFLIP && p.action!=ACTION_ROLL);top=fminf(top,p.y);
        }
        assert(p.grounded && top>=956-0.01f);
        /* Standing (not crouched) away+A is an ordinary jump that turns Mario around. */
        p=standing(left);
        player_step(&p,&flat,(GameInput){.horizontal=away(left),.jump_pressed=true,.jump_held=true});
        assert(p.action==ACTION_NONE && !p.grounded && p.facing_left==(left==0));
        /* Fast slide + A is the source diveflip/roll, not a backflip. */
        p=standing(left);p.vx=(left?-1:1)*200;
        ticks(&p,(GameInput){.crouch_held=true},5);
        for(int i=0;i<2 && p.action==ACTION_DIVE;++i)
            player_step(&p,&flat,(GameInput){.crouch_held=true,.jump_pressed=i==0,.jump_held=true});
        assert(p.action==ACTION_ROLL);
    }
}
static void test_spin_and_jump_unchanged(void)
{
    Player p=standing(false);
    player_step(&p,&flat,(GameInput){.jump_pressed=true,.jump_held=true});
    assert(!p.grounded && p.vy<-400 && p.action==ACTION_NONE);
    const World sky={.width=10000,.height=100000};
    Player s;player_init(&s,1000,1000);unsigned starts=0;bool active=false;
    for(int i=0;i<360;++i) {
        player_step(&s,&sky,(GameInput){.spin_held=true,.spin_pressed=true});
        if(s.spin_time>0&&!active) ++starts;
        active=s.spin_time>0;
    }
    assert(starts>2); /* Repeated airborne spinning preserved. */
}
/* Phase 2 items 2 and 9: the active controller (for the picture signs) and the
   C-stick zoom axis. */
static void test_controller_detection_and_zoom_stick(void)
{
    assert(input_active_controller(CONTROLLER_GAMECUBE,false,false,CONTROLLER_CLASSIC)==CONTROLLER_GAMECUBE);
    assert(input_active_controller(CONTROLLER_GAMECUBE,false,true,CONTROLLER_CLASSIC)==CONTROLLER_CLASSIC);
    assert(input_active_controller(CONTROLLER_CLASSIC,true,false,CONTROLLER_WIIMOTE)==CONTROLLER_GAMECUBE);
    assert(input_active_controller(CONTROLLER_WIIMOTE,false,false,CONTROLLER_CLASSIC)==CONTROLLER_WIIMOTE); /* idle: keep */
    assert(input_map_zoom_stick(0)==0 && input_map_zoom_stick(20)==0 && input_map_zoom_stick(-24)==0); /* dead zone */
    assert(input_map_zoom_stick(90)==-1 && input_map_zoom_stick(-90)==1);   /* up: in (-1), down: out */
    assert(input_map_zoom_stick(50)<0 && input_map_zoom_stick(50)>-1);
    /* The zoom never reaches the game's movement/action inputs. */
    const GameInput a=input_map_gamecube(0,0);
    assert(a.horizontal==0 && a.vertical==0 && !a.jump_pressed && a.zoom_axis==0);
}
int main(void)
{
    test_gamecube_down_reaches_gameplay();test_crouch_holds_with_slide_pose();
    test_nozzle_dive_recovery();test_backflip_activation_paths();test_not_a_backflip();test_spin_and_jump_unchanged();
    test_controller_detection_and_zoom_stick();test_classic_mapping();test_wiimote_mapping();
    puts("9 Down/crouch/backflip/controller/Classic/Wii Remote control regression groups passed");return 0;
}
