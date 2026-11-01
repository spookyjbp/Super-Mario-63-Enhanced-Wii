#include "user_art.h"
#include "art_pack.h"
#include "graphics.h"
#include <fat.h>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "player_art_bin.h"
#include "player_art_select.h"

static ArtPack art;
static GXTexObj texture;
static unsigned char *storage;          /* SD/USB override, if one was loaded. */
static const unsigned char *loaded;     /* Pack actually in use. */
static float art_scale=1.5f;
void user_art_set_scale(float scale) {art_scale=scale;}
static bool load_file(const char *path)
{
    FILE *file=fopen(path,"rb");
    if(!file) return false;
    if(fseek(file,0,SEEK_END)!=0) {fclose(file);return false;}
    long size=ftell(file);
    if(size<32 || size>4*1024*1024+128*1024 || fseek(file,0,SEEK_SET)!=0) {fclose(file);return false;}
    storage=memalign(32,(size_t)size);
    if(!storage) {fclose(file);return false;}
    const bool read=fread(storage,1,(size_t)size,file)==(size_t)size;fclose(file);
    if(!read || !art_pack_parse(storage,(size_t)size,&art)) {free(storage);storage=NULL;return false;}
    loaded=storage;return true;
}
/* An SD/USB player.art overrides the pack embedded in boot.dol
   (assets/generated/player_art.bin, built by tools/export_source_clips.py). */
void user_art_init(void)
{
    if(loaded) return;
    if(fatInitDefault() && !load_file("sd:/apps/super-mario-63-enhanced-wii/player.art"))
        load_file("usb:/apps/super-mario-63-enhanced-wii/player.art");
    if(!loaded && player_art_bin_size>0 && art_pack_parse(player_art_bin,player_art_bin_size,&art))
        loaded=player_art_bin;
    if(!loaded) return;
    DCFlushRange((void *)art.texture,art.width*art.height*4);
    GX_InitTexObj(&texture,(void *)art.texture,art.width,art.height,GX_TF_RGBA8,GX_CLAMP,GX_CLAMP,GX_FALSE);
    GX_InitTexObjLOD(&texture,GX_NEAR,GX_NEAR,0,0,0,GX_FALSE,GX_FALSE,GX_ANISO_1);
    GX_InvalidateTexAll();
}
void user_art_shutdown(void)
{
    free(storage);storage=NULL;loaded=NULL;
}
bool user_art_available(void) {return loaded!=NULL;}
/* Draws a frame whose offset is in twips from Mario's feet, canonical-left. */
static void draw(const Player *p,const ArtFrame *f,float x,float y,float degrees,float pivot_x,float pivot_y)
{
    const float scale=art_scale,twips=scale/20,w=f->width*scale,h=f->height*scale;
    float left=f->offset_x*twips,top=f->offset_y*twips,px=pivot_x*twips,py=pivot_y*twips;
    float u0=(float)f->x/art.width,u1=(float)(f->x+f->width)/art.width;
    if(!p->facing_left) { /* Mirror registration, pivot, rotation and UVs once. */
        left=-left-w;px=-px;degrees=-degrees;
        const float swap=u0;u0=u1;u1=swap;
    }
    const float feet_x=x+PLAYER_WIDTH/2,feet_y=y+player_height(p);
    const float v0=(float)f->y/art.height,v1=(float)(f->y+f->height)/art.height;
    if(degrees==0) {graphics_sprite(&texture,feet_x+left,feet_y+top,w,h,u0,v0,u1,v1);return;}
    const float r=degrees*0.01745329252f,c=cosf(r),s=sinf(r);
    const float cx=left+w/2-px,cy=top+h/2-py;
    graphics_sprite_rotated(&texture,feet_x+px+cx*c-cy*s-w/2,feet_y+py+cx*s+cy*c-h/2,w,h,u0,v0,u1,v1,degrees);
}
static const ArtFrame *body_frame(const ArtPick *a,unsigned clip)
{
    return a->timed ? art_pack_frame(&art,clip,a->seconds) : art_pack_frame_index(&art,clip,a->index);
}
bool user_art_draw_player(const Player *p,float x,float y)
{
    if(!loaded) return false;
    ArtPick a=player_art_select(p);
    unsigned clip=art_clip(a.state,a.variant);
    const ArtFrame *f=body_frame(&a,clip);
    if(!f) {a.variant=0;clip=art_clip(a.state,0);f=body_frame(&a,clip);} /* no-FLUDD clip */
    if(!f) return false;
    const ArtFrame *back=art_pack_frame_index(&art,ART_BACK_EFFECT+clip,a.effect_index);
    const ArtFrame *front=art_pack_frame_index(&art,ART_FRONT_EFFECT+clip,a.effect_index);
    if(back) draw(p,back,x,y,0,0,0);
    draw(p,f,x,y,a.rotation,(float)a.pivot_x,(float)a.pivot_y);
    if(front) draw(p,front,x,y,0,0,0);
    return true;
}
bool user_art_fludd_available(const Player *p)
{
    return loaded && art_pack_clip_length(&art,art_clip(ART_WALK,1u+(unsigned)p->nozzle))>0;
}
