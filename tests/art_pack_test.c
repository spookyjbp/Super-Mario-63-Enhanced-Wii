/* Player art pack (format v2): parser, loader precedence, clip selection and
   feet-anchored drawing. */
#include "art_pack.h"
#include "user_art.h"
#include "player_art_select.h"
#include "graphics.h"
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

const unsigned char *player_art_bin;unsigned int player_art_bin_size;
typedef struct {float x,y,w,h,u0,u1,deg;} Draw;
static Draw draws[8];static unsigned calls;
void DCFlushRange(void *d,unsigned n) {(void)d;(void)n;}
void GX_InitTexObj(GXTexObj *t,void *d,unsigned w,unsigned h,unsigned f,unsigned s,unsigned v,unsigned m)
{(void)t;(void)d;(void)w;(void)h;(void)f;(void)s;(void)v;(void)m;}
void GX_InitTexObjLOD(GXTexObj *t,unsigned a,unsigned b,float c,float d,float e,unsigned f,unsigned g,unsigned h)
{(void)t;(void)a;(void)b;(void)c;(void)d;(void)e;(void)f;(void)g;(void)h;}
void GX_InvalidateTexAll(void) {}
static void record(float x,float y,float w,float h,float u0,float u1,float deg)
{ if(calls<8) draws[calls]=(Draw){x,y,w,h,u0,u1,deg}; ++calls; }
void graphics_sprite(GXTexObj *t,float x,float y,float w,float h,float u0,float v0,float u1,float v1)
{(void)t;(void)v0;(void)v1;record(x,y,w,h,u0,u1,0);}
void graphics_sprite_rotated(GXTexObj *t,float x,float y,float w,float h,float u0,float v0,float u1,float v1,float deg)
{(void)t;(void)v0;(void)v1;record(x,y,w,h,u0,u1,deg);}

static void set16(unsigned char *p,unsigned n) {p[0]=(unsigned char)(n>>8);p[1]=(unsigned char)n;}
static void set32(unsigned char *p,unsigned n) {set16(p,n>>16);set16(p+2,n);}
/* Fixture: 4x4 texture; clips (id, frames): walk/Hover 1 frame, back blast for it,
   Ground/Hover 3 frames, and no-FLUDD rise only. */
enum { FIX_CLIPS=4, FIX_FRAMES=7 };
static const unsigned fix_ids[FIX_CLIPS]={1,ART_BACK_EFFECT+1,ART_GROUND*4+1,ART_RISE*4};
static const unsigned fix_counts[FIX_CLIPS]={1,2,3,1};
static size_t fixture(unsigned char *d,unsigned version)
{
    const size_t meta=32+FIX_CLIPS*8+FIX_FRAMES*16,offset=(meta+31)&~(size_t)31,size=offset+64;
    memset(d,0,size);memcpy(d,"SM63ART1",8);
    set32(d+8,version);set32(d+12,4);set32(d+16,4);set32(d+20,FIX_CLIPS);set32(d+24,FIX_FRAMES);set32(d+28,64);
    unsigned first=0;
    for(unsigned i=0;i<FIX_CLIPS;++i) {
        set16(d+32+i*8,fix_ids[i]);set16(d+34+i*8,first);set16(d+36+i*8,fix_counts[i]);first+=fix_counts[i];
    }
    for(unsigned i=0;i<FIX_FRAMES;++i) {
        unsigned char *f=d+32+FIX_CLIPS*8+i*16;
        set16(f+4,2);set16(f+6,2);set16(f+12,1);
        set16(f+8,(unsigned)(i==0 ? -40 : 20*(int)i)&65535); /* x offset twips */
        set16(f+10,(unsigned)(-80)&65535);                     /* y offset twips */
    }
    return size;
}
static void directory(const char *p) {assert(mkdir(p,0700)==0 || errno==EEXIST);}
static void near(float a,float b) {assert(fabsf(a-b)<0.001f);}

static void test_parser(void)
{
    static unsigned char d[512];ArtPack pack;const size_t n=fixture(d,2);
    assert(art_pack_parse(d,n,&pack));
    assert(art_pack_clip_length(&pack,ART_GROUND*4+1)==3 && art_pack_clip_length(&pack,0)==0);
    assert(art_pack_frame_index(&pack,ART_GROUND*4+1,1)==&pack.frames[4]);
    assert(art_pack_frame_index(&pack,ART_GROUND*4+1,99)==&pack.frames[5]); /* clamps */
    assert(art_pack_frame_index(&pack,0,0)==NULL && art_pack_frame_index(&pack,ART_CLIP_COUNT,0)==NULL);
    assert(art_pack_frame(&pack,ART_GROUND*4+1,2/32.0f)==&pack.frames[5]);
    fixture(d,1);assert(!art_pack_parse(d,n,&pack)); /* v1 ids mean other clips: reject */
    fixture(d,2);assert(!art_pack_parse(d,n-1,&pack));
}
static void test_selection_rules(void)
{
    Player p={.nozzle=NOZZLE_HOVER,.grounded=true,.walk_frame=40.4f};
    ArtPick a=player_art_select(&p);
    assert(a.state==ART_WALK && a.variant==1 && a.index==39 && !a.timed);
    p.nozzle=NOZZLE_TURBO;p.blast_level=3;p.blast_parity=true;a=player_art_select(&p);
    assert(a.variant==3 && a.effect_index==7);
    p.nozzle=NOZZLE_ROCKET;p.rocket_charge=5.0f/32;a=player_art_select(&p);assert(a.effect_index==5);
    p.rocket_boost=8.0f/32-3.0f/32;a=player_art_select(&p);assert(a.effect_index==18);
    p=(Player){.grounded=true,.dashing=true,.run_count=3};assert(player_art_select(&p).index==3);
    p.run_count=6;p.run_frame=10;a=player_art_select(&p);assert(a.state==ART_RUN && a.index==16);
    p=(Player){.vy=3*44.0f};assert(player_art_select(&p).state==ART_FALL);
    p.vy=2*44.0f;assert(player_art_select(&p).state==ART_RISE);
    p.jump_stage=3;p.jump_time=0.5f;a=player_art_select(&p);assert(a.state==ART_FRONTFLIP && a.timed);
    p=(Player){.spin_time=1,.spin_frame=12,.spin_speed=11};assert(player_art_select(&p).index==12);
    p.spin_speed=9;assert(player_art_select(&p).index==52);
    p=(Player){.pounding=true,.pound_frame=20};a=player_art_select(&p);assert(a.state==ART_GROUND && a.index==20);
    p=(Player){.landing_time=0.2f,.pound_frame=30,.grounded=true};assert(player_art_select(&p).index==30);
    p=(Player){.action=ACTION_DIVE,.dive_frame=2,.dive_rotation=-30,.nozzle=NOZZLE_HOVER};
    a=player_art_select(&p);assert(a.state==ART_DIVE && a.index==2 && a.rotation==-30 && a.pivot_y==-393);
    p=(Player){.action=ACTION_BACKFLIP,.action_time=0.25f};a=player_art_select(&p);
    assert(a.state==ART_BACKFLIP && a.timed && a.seconds==0.25f);
    p=(Player){.submerged=true,.swim_stroke=true,.vy=-44};a=player_art_select(&p);
    assert(a.state==ART_SWIM_STROKE && a.index==0);
    p=(Player){.grounded=true,.normal_land_time=1.0f/32};assert(player_art_select(&p).state==ART_LAND);
}
static void test_loader_and_drawing(void)
{
    static unsigned char bytes[512],embedded[512];const size_t n=fixture(bytes,2);
    directory("sd:");directory("sd:/apps");directory("sd:/apps/super-mario-63-enhanced-wii");
    remove("sd:/apps/super-mario-63-enhanced-wii/player.art");
    /* Pack embedded in boot.dol is used when no SD/USB file exists. */
    player_art_bin=bytes;player_art_bin_size=(unsigned)n;
    user_art_init();assert(user_art_available());
    Player p={.height=44,.nozzle=NOZZLE_HOVER,.grounded=true,.walk_frame=1,.facing_left=true};
    assert(user_art_fludd_available(&p));
    calls=0;assert(user_art_draw_player(&p,100,100));
    /* Back blast first (frame 1 of its clip: +20 twips), then body (frame 0: -40). */
    assert(calls==2);
    near(draws[1].x,100+14-3);near(draws[1].y,100+44-6);near(draws[1].w,3);assert(draws[1].u0<draws[1].u1);
    near(draws[0].x,100+14+1.5f);
    p.facing_left=false;calls=0;user_art_draw_player(&p,100,100);
    near(draws[1].x,100+14+3-3);assert(draws[1].u0>draws[1].u1); /* mirrored registration + UVs */
    /* Crouched collider: still anchored at the feet. */
    p.height=26;calls=0;user_art_draw_player(&p,100,100);near(draws[1].y,100+26-6);
    /* Missing FLUDD variant falls back to the no-FLUDD clip; no effects drawn. */
    p=(Player){.height=44,.nozzle=NOZZLE_TURBO,.vy=0,.facing_left=true};
    calls=0;assert(user_art_draw_player(&p,0,0) && calls==1);
    assert(!user_art_fludd_available(&p));
    /* Ground pound frames come from the Hover Ground clip, unrotated. */
    p=(Player){.height=44,.nozzle=NOZZLE_HOVER,.pounding=true,.pound_frame=0,.facing_left=true};
    calls=0;assert(user_art_draw_player(&p,0,0) && calls==1 && draws[0].deg==0);
    user_art_shutdown();assert(!user_art_available());
    player_art_bin_size=0;user_art_init();assert(!user_art_available());
    assert(!user_art_draw_player(&p,0,0));
    /* An SD file overrides the embedded pack. */
    memcpy(embedded,bytes,n);set32(embedded+8,1); /* invalid embedded copy */
    player_art_bin=embedded;player_art_bin_size=(unsigned)n;
    FILE *f=fopen("sd:/apps/super-mario-63-enhanced-wii/player.art","wb");assert(f);
    assert(fwrite(bytes,1,n,f)==n);assert(fclose(f)==0);
    user_art_init();assert(user_art_available());
    remove("sd:/apps/super-mario-63-enhanced-wii/player.art");user_art_shutdown();
}
static void test_committed_pack(void)
{
    /* The embedded pack: every movement state in all four FLUDD variants. */
    FILE *e=fopen("../../assets/generated/player_art.bin","rb");assert(e);
    static unsigned char real[4*1024*1024+128*1024];const size_t n=fread(real,1,sizeof(real),e);fclose(e);
    static ArtPack pack;assert(art_pack_parse(real,n,&pack));
    for(unsigned s=0;s<ART_STATE_COUNT;++s)
        for(unsigned v=0;v<ART_VARIANTS;++v) assert(art_pack_clip_length(&pack,art_clip((ArtState)s,v))>0);
    assert(art_pack_clip_length(&pack,art_clip(ART_WALK,0))==82);
    assert(art_pack_clip_length(&pack,art_clip(ART_BACKFLIP,0))==22);
    assert(art_pack_clip_length(&pack,art_clip(ART_FRONTFLIP,0))==30);
    assert(art_pack_clip_length(&pack,art_clip(ART_SPIN,1))==79);
    assert(art_pack_clip_length(&pack,art_clip(ART_GROUND,2))==43);
    assert(art_pack_clip_length(&pack,art_clip(ART_DIVE,1))==8);   /* Slide (Hover) */
    assert(art_pack_clip_length(&pack,art_clip(ART_LAND,3))==8);
    for(unsigned v=1;v<ART_VARIANTS;++v) {
        const unsigned walk=art_clip(ART_WALK,v),blast=v==2 ? 24 : 10; /* Rocket / spray */
        assert(art_pack_clip_length(&pack,ART_BACK_EFFECT+walk)+art_pack_clip_length(&pack,ART_FRONT_EFFECT+walk)>=blast);
    }
}
int main(void)
{
    test_parser();test_selection_rules();test_loader_and_drawing();test_committed_pack();
    puts("4 player-art parser/selection/loader/drawing regression groups passed");return 0;
}
