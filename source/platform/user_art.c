#include "user_art.h"
#include "art_pack.h"
#include "graphics.h"
#include <fat.h>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static ArtPack art;
static GXTexObj texture;
static unsigned char *storage;
void user_art_init(void)
{
    if(storage || !fatInitDefault()) return;
    FILE *file=fopen("sd:/apps/super-mario-63-enhanced-wii/player.art","rb");
    if(!file) file=fopen("usb:/apps/super-mario-63-enhanced-wii/player.art","rb");
    if(!file) return;
    if(fseek(file,0,SEEK_END)!=0) {fclose(file);return;}
    long size=ftell(file);
    if(size<32 || size>4*1024*1024+8192 || fseek(file,0,SEEK_SET)!=0) {fclose(file);return;}
    storage=memalign(32,(size_t)size);
    if(!storage) {fclose(file);return;}
    const bool read=fread(storage,1,(size_t)size,file)==(size_t)size;fclose(file);
    if(!read || !art_pack_parse(storage,(size_t)size,&art)) {free(storage);storage=NULL;return;}
    DCFlushRange((void *)art.texture,art.width*art.height*4);
    GX_InitTexObj(&texture,(void *)art.texture,art.width,art.height,GX_TF_RGBA8,GX_CLAMP,GX_CLAMP,GX_FALSE);
    GX_InitTexObjLOD(&texture,GX_NEAR,GX_NEAR,0,0,0,GX_FALSE,GX_FALSE,GX_ANISO_1);
    GX_InvalidateTexAll();
}
bool user_art_available(void) {return storage!=NULL;}
static unsigned body_clip(const Player *p)
{
    if(p->animation==PLAYER_CROUCH) return ART_DOWN;
    if(p->animation==PLAYER_DIVE_RECOVER) return ART_RISE;
    if(p->animation==PLAYER_BACKFLIP) return ART_BACKFLIP;
    if(p->animation==PLAYER_SWIM) return p->swim_stroke ? ART_SWIM_STROKE : ART_SWIM_FLOAT;
    if(p->animation==PLAYER_LAND) return ART_LAND;
    if(p->animation==PLAYER_RISE || p->animation==PLAYER_FALL) {
        if(p->jump_stage==3 && p->jump_time<30.0f/32) return ART_TRIPLE;
        if(p->jump_time<1.0f/32) return ART_TAKEOFF;
        if(p->jump_stage==2 && p->animation==PLAYER_RISE) return ART_DOUBLE;
        return p->animation==PLAYER_RISE ? ART_RISE : ART_FALL;
    }
    return ART_CLIP_COUNT;
}
static float clip_time(const Player *p,unsigned clip)
{
    if(clip==ART_DOWN) return 0;
    if(clip==ART_SWIM_FLOAT) return p->vy>0 ? 0 : 1.0f/32;
    if(clip==ART_SWIM_STROKE) return p->vy<0.5f*44 ? 0 : 1.0f/32;
    if(clip==ART_BACKFLIP) return p->action_time;
    if(clip==ART_TRIPLE || clip==ART_TAKEOFF) return p->jump_time;
    return p->animation_time;
}
static void draw(const Player *p,const ArtFrame *f,float x,float y)
{
    const float scale=1.5f,w=f->width*scale,h=f->height*scale;
    float ox=f->offset_x/20.0f*scale;
    /* Frames are authored canonical-left. Mirror both registration and UV once. */
    const bool mirror=!p->facing_left;
    if(mirror) ox=-ox-w;
    float u0=(float)f->x/art.width,u1=(float)(f->x+f->width)/art.width;
    if(mirror) {float swap=u0;u0=u1;u1=swap;}
    graphics_sprite(&texture,x+PLAYER_WIDTH/2+ox,
                    y+player_height(p)-PLAYER_HEIGHT/2+f->offset_y/20.0f*scale,w,h,
                    u0,(float)f->y/art.height,u1,(float)(f->y+f->height)/art.height);
}
bool user_art_draw_player(const Player *p,float x,float y)
{
    unsigned clip=body_clip(p);
    const unsigned equipped=16+clip*3+p->nozzle;
    const ArtFrame *f=art_pack_frame(&art,equipped,clip_time(p,clip));
    if(!f) f=art_pack_frame(&art,clip,clip_time(p,clip));
    if(!f && clip==ART_TAKEOFF) f=art_pack_frame(&art,ART_RISE,0);
    if(!f) return false;
    draw(p,f,x,y);return true;
}
bool user_art_fludd_available(const Player *p)
{
    const unsigned equipped=16+body_clip(p)*3+p->nozzle;
    const bool body_has=equipped<ART_CLIP_COUNT && (art.clips[equipped].flags&2);
    return (body_has || art.clips[48+p->nozzle].count>0) &&
           (!p->jet_active || art.clips[51+p->nozzle].count>0);
}
void user_art_draw_equipment(const Player *p,float x,float y)
{
    unsigned clip=body_clip(p),equipped=16+clip*3+p->nozzle;
    bool contains=equipped<ART_CLIP_COUNT && (art.clips[equipped].flags&2);
    if(!contains) {
        const ArtFrame *f=art_pack_frame(&art,48+p->nozzle,p->animation_time);
        if(f) draw(p,f,x,y);
    }
    if(p->jet_active) {
        const float elapsed=p->nozzle==NOZZLE_ROCKET ? fmaxf(0,8.0f/32-p->rocket_boost) : p->animation_time;
        const ArtFrame *f=art_pack_frame(&art,51+p->nozzle,elapsed);
        if(f && (p->nozzle!=NOZZLE_ROCKET || p->rocket_boost>0)) draw(p,f,x,y);
    }
}
