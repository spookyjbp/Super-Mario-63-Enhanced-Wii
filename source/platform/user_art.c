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
#include "player_spray.h"

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
/* charClip.frame._alpha (CharCode blink): 255 normally. */
static unsigned char draw_alpha=255;
void user_art_set_alpha(unsigned char alpha) {draw_alpha=alpha;}
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
    const float r=degrees*0.01745329252f,c=cosf(r),s=sinf(r);
    const float cx=left+w/2-px,cy=top+h/2-py;
    if(draw_alpha<255) { /* blinking: the same quad, translucent */
        const float mx=feet_x+px+cx*c-cy*s,my=feet_y+py+cx*s+cy*c;
        const float hx[4]={-w/2,w/2,w/2,-w/2},hy[4]={-h/2,-h/2,h/2,h/2};
        float xs[4],ys[4];
        for(unsigned i=0;i<4;++i) {xs[i]=mx+hx[i]*c-hy[i]*s;ys[i]=my+hx[i]*s+hy[i]*c;}
        graphics_quad_alpha(&texture,xs,ys,u0,v0,u1,v1,draw_alpha);
        return;
    }
    if(degrees==0) {graphics_sprite(&texture,feet_x+left,feet_y+top,w,h,u0,v0,u1,v1);return;}
    graphics_sprite_rotated(&texture,feet_x+px+cx*c-cy*s-w/2,feet_y+py+cx*s+cy*c-h/2,w,h,u0,v0,u1,v1,degrees);
}
/* FLUDD spray nested inside the Dive / Frontflip bodies (Wait WaterBlast 1157
   in HDive 1200, Hfrontflip 1216 > 1214, ...). The body art is rendered with
   it empty, so it is drawn here with its placement for the body frame
   (player_spray.h), the clip's `_yscale *= -1` parity, the dive rotation and
   the facing mirror. Frame = spray level (1157 frames 2-5). */
static void draw_spray(const Player *p,const ArtPick *a,float x,float y,bool behind)
{
    SprayPick s;
    if(!player_spray_pick(p,a,PLAYER_SPRAY_CLIP,PLAYER_ROCKET_SPRAY_CLIP,&s)) return;
    const ArtFrame *f=art_pack_frame_index(&art,s.clip,s.frame);
    if(!f) return;
    for(unsigned t=0;t<PLAYER_SPRAY_TABLES;++t) {
        const SprayTable *T=&player_spray_tables[t];
        if(T->state!=a->state || T->variant!=a->variant) continue;
        unsigned n=a->timed ? (unsigned)(fmaxf(0,a->seconds)*32) : a->index;
        if(n>=T->frames) n=T->frames-1u;
        const float scale=art_scale,k=scale/20,deg=a->rotation*0.01745329252f,c=cosf(deg),sn=sinf(deg);
        const float lx[4]={(float)f->offset_x,(float)f->offset_x+f->width*20.0f,(float)f->offset_x+f->width*20.0f,(float)f->offset_x};
        const float ly[4]={(float)f->offset_y,(float)f->offset_y,(float)f->offset_y+f->height*20.0f,(float)f->offset_y+f->height*20.0f};
        const float feet_x=x+PLAYER_WIDTH/2,feet_y=y+player_height(p);
        for(unsigned i=0;i<T->per_frame;++i) {
            const SprayPlacement *m=&T->data[n*T->per_frame+i];
            if(m->behind!=(behind ? 1 : 0)) continue;
            float xs[4],ys[4];
            for(unsigned v=0;v<4;++v) {
                const float qy=s.flip ? -ly[v] : ly[v];
                float X=m->a*lx[v]+m->c*qy+m->tx,Y=m->b*lx[v]+m->d*qy+m->ty;
                const float dx=X-a->pivot_x,dy=Y-a->pivot_y;   /* Dive.xml _rotation */
                X=a->pivot_x+dx*c-dy*sn;Y=a->pivot_y+dx*sn+dy*c;
                if(!p->facing_left) X=-X;
                xs[v]=feet_x+X*k;ys[v]=feet_y+Y*k;
            }
            graphics_quad_alpha(&texture,xs,ys,(float)f->x/art.width,(float)f->y/art.height,
                                (float)(f->x+f->width)/art.width,(float)(f->y+f->height)/art.height,255);
        }
    }
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
    /* Char's own _rotation (the FLUDD tilt) turns the whole clip about the feet;
       draw() mirrors its angle for the canonical-left art, so pass it mirrored. */
    const float tilt=a.rotation==0 ? (p->facing_left ? p->tilt_draw : -p->tilt_draw) : 0;
    if(back) draw(p,back,x,y,tilt,0,0);
    draw_spray(p,&a,x,y,true);
    draw(p,f,x,y,a.rotation!=0 ? a.rotation : tilt,a.rotation!=0 ? (float)a.pivot_x : 0,a.rotation!=0 ? (float)a.pivot_y : 0);
    draw_spray(p,&a,x,y,false);
    if(front) draw(p,front,x,y,tilt,0,0);
    return true;
}
bool user_art_fludd_available(const Player *p)
{
    return loaded && art_pack_clip_length(&art,art_clip(ART_WALK,1u+(unsigned)p->nozzle))>0;
}
