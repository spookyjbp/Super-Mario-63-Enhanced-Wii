#include "art_pack.h"
#include "user_art.h"
#include "graphics.h"
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

const unsigned char *player_art_bin;unsigned int player_art_bin_size;
static unsigned calls;static float px,py,tu0,tu1;
void DCFlushRange(void *d,unsigned n) {(void)d;(void)n;}
void GX_InitTexObj(GXTexObj *t,void *d,unsigned w,unsigned h,unsigned f,unsigned s,unsigned v,unsigned m)
{(void)t;(void)d;(void)f;(void)s;(void)v;(void)m;assert(w==4&&h==4);}
void GX_InitTexObjLOD(GXTexObj *t,unsigned a,unsigned b,float c,float d,float e,unsigned f,unsigned g,unsigned h)
{(void)t;(void)a;(void)b;(void)c;(void)d;(void)e;(void)f;(void)g;(void)h;}
void GX_InvalidateTexAll(void) {}
void graphics_sprite(GXTexObj *t,float x,float y,float w,float h,float u0,float v0,float u1,float v1)
{(void)t;(void)w;(void)h;(void)v0;(void)v1;++calls;px=x;py=y;tu0=u0;tu1=u1;}
static void set16(unsigned char *p,unsigned n) {p[0]=(unsigned char)(n>>8);p[1]=(unsigned char)n;}
static void set32(unsigned char *p,unsigned n) {set16(p,n>>16);set16(p+2,n);}
static void fixture(unsigned char *d)
{
    memset(d,0,192);memcpy(d,"SM63ART1",8);
    set32(d+8,1);set32(d+12,4);set32(d+16,4);set32(d+20,3);set32(d+24,3);set32(d+28,64);
    const unsigned ids[]={0,48,51};
    for(unsigned i=0;i<3;++i) {
        set16(d+32+i*8,ids[i]);set16(d+34+i*8,i);set16(d+36+i*8,1);
        unsigned char *f=d+56+i*16;set16(f,i);set16(f+4,1);set16(f+6,1);
        set16(f+8,(unsigned)(i==0?-20:0)&65535);set16(f+12,1);
    }
}
static void directory(const char *p) {assert(mkdir(p,0700)==0 || errno==EEXIST);}
int main(void)
{
    unsigned char bytes[192],bad[192];fixture(bytes);ArtPack pack;
    assert(art_pack_parse(bytes,sizeof(bytes),&pack));
    assert(pack.frames[0].offset_x==-20 && pack.texture==bytes+128);
    assert(art_pack_frame(&pack,0,0)==&pack.frames[0]);
    assert(art_pack_frame(&pack,0,1000)==&pack.frames[0]);
    assert(art_pack_frame(&pack,1,0)==NULL && art_pack_frame(&pack,64,0)==NULL);
    memcpy(bad,bytes,192);set16(bad+36,3); /* Three one-tick frames at source 32 Hz. */
    assert(art_pack_parse(bad,192,&pack));
    for(unsigned tick=0;tick<3;++tick)
        assert(art_pack_frame(&pack,0,tick/32.0f)==&pack.frames[tick]);
    assert(art_pack_frame(&pack,0,3/32.0f)==&pack.frames[2]); /* Non-looping clip clamps. */
    set16(bad+38,1);assert(art_pack_parse(bad,192,&pack));
    assert(art_pack_frame(&pack,0,3/32.0f)==&pack.frames[0]);
    assert(!art_pack_parse(bytes,191,&pack));
    memcpy(bad,bytes,192);set16(bad+40,0);assert(!art_pack_parse(bad,192,&pack));
    memcpy(bad,bytes,192);set16(bad+60,5);assert(!art_pack_parse(bad,192,&pack));
    memcpy(bad,bytes,192);set16(bad+68,0);assert(!art_pack_parse(bad,192,&pack));
    memcpy(bad,bytes,192);set32(bad+12,1025);assert(!art_pack_parse(bad,192,&pack));
    /* Pack embedded in boot.dol is used when no SD/USB file exists. */
    directory("sd:");directory("sd:/apps");directory("sd:/apps/super-mario-63-enhanced-wii");
    remove("sd:/apps/super-mario-63-enhanced-wii/player.art");
    player_art_bin=bytes;player_art_bin_size=sizeof(bytes);
    user_art_init();assert(user_art_available());
    {Player q={.height=44,.animation=PLAYER_CROUCH,.facing_left=true};
     assert(user_art_draw_player(&q,100,100) && fabsf(px-112.5f)<0.001f);}
    user_art_shutdown();assert(!user_art_available());
    player_art_bin_size=0;user_art_init();assert(!user_art_available());
    /* An SD file overrides the embedded pack (embedded copy has offset 0 -> px 114). */
    static unsigned char embedded[192];memcpy(embedded,bytes,192);set16(embedded+64,0);
    player_art_bin=embedded;player_art_bin_size=sizeof(embedded);calls=0;
    FILE *f=fopen("sd:/apps/super-mario-63-enhanced-wii/player.art","wb");assert(f);
    assert(fwrite(bytes,1,sizeof(bytes),f)==sizeof(bytes));assert(fclose(f)==0);
    user_art_init();assert(user_art_available());
    Player p={.height=44,.animation=PLAYER_CROUCH,.facing_left=true};
    assert(user_art_draw_player(&p,100,100));assert(calls==1 && fabsf(px-112.5f)<0.001f && tu0<tu1);
    p.facing_left=false;assert(user_art_draw_player(&p,100,100));
    assert(fabsf(px-114)<0.001f && tu0>tu1);
    assert(user_art_fludd_available(&p));
    calls=0;user_art_draw_equipment(&p,100,100);assert(calls==1);
    p.jet_active=true;user_art_draw_equipment(&p,100,100);assert(calls==3);
    const float x=px,y=py;user_art_draw_equipment(&p,150,70);
    assert(px==x+50 && py==y-30);
    p.nozzle=NOZZLE_ROCKET;p.rocket_boost=0;calls=0;
    assert(!user_art_fludd_available(&p));
    user_art_draw_equipment(&p,100,100);assert(calls==0); /* No wrong-nozzle effect. */
    /* The committed embedded pack: original crouch, 30-frame triple, 22-frame backflip. */
    FILE *e=fopen("../../assets/generated/player_art.bin","rb");assert(e);
    static unsigned char real[4*1024*1024+8192];const size_t n=fread(real,1,sizeof(real),e);fclose(e);
    static ArtPack committed;assert(art_pack_parse(real,n,&committed));
    assert(committed.clips[ART_DOWN].count==1 && committed.clips[ART_TRIPLE].count==30 &&
           committed.clips[ART_BACKFLIP].count==22 && committed.clips[ART_RISE].count==1);
    puts("5 private-art parser/loader/render regression groups passed");return 0;
}
