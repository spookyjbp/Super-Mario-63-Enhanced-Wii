#include "render.h"
#include "graphics.h"
#include "player_sprite.h"
#include "world_query.h"
#include "user_art.h"
#include <math.h>
#include "mario_atlas.h"
#include "mario_rgba8_bin.h"
#include <stdio.h>

static GXTexObj mario_texture;

void render_init(void)
{
    DCFlushRange((void *)mario_rgba8_bin, mario_rgba8_bin_size);
    GX_InitTexObj(&mario_texture, (void *)mario_rgba8_bin, MARIO_ATLAS_SIZE, MARIO_ATLAS_SIZE,
                  GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GX_InitTexObjLOD(&mario_texture, GX_NEAR, GX_NEAR, 0, 0, 0,
                     GX_FALSE, GX_FALSE, GX_ANISO_1);
    GX_InvalidateTexAll();
    user_art_init();
}

static void draw_player(const Player *p, float x, float y)
{
    const PlayerSprite selected = player_sprite_select(p);
    const MarioFrame sprite = mario_frames[selected.frame];
    const float scale = 1.5f;
    const float width = sprite.width * scale, height = sprite.height * scale * selected.height_scale;
    float u0 = (float)sprite.x / MARIO_ATLAS_SIZE;
    float u1 = (float)(sprite.x + sprite.width) / MARIO_ATLAS_SIZE;
    if (selected.flip_x) { const float swap = u0; u0 = u1; u1 = swap; }
    const float px=x+(PLAYER_WIDTH-width)/2, py=y+player_height(p)-height;
    const float v0=(float)sprite.y/MARIO_ATLAS_SIZE;
    const float v1=(float)(sprite.y+sprite.height)/MARIO_ATLAS_SIZE;
    if(selected.rotation!=0)
        graphics_sprite_rotated(&mario_texture,px,py,width,height,u0,v0,u1,v1,selected.rotation);
    else graphics_sprite(&mario_texture,px,py,width,height,u0,v0,u1,v1);

}

static float lerp(float a, float b, float t) { return a + (b - a) * t; }

void render_game(const Game *game, const World *world, float interpolation)
{
    const Player *p = &game->player;
    const float camera_x = lerp(game->camera.previous_x, game->camera.x, interpolation);
    const float camera_y = lerp(game->camera.previous_y, game->camera.y, interpolation);
    const float width = graphics_width();
    const GXColor white = {245, 248, 255, 255};
    graphics_rect(0, 0, width, 480, (GXColor){100, 177, 235, 255});
    graphics_rect(width - 110, 60, 48, 48, (GXColor){255, 226, 120, 255});
    if(world->level) {
        const Level *l=world->level;
        for(unsigned layer=0;layer<l->background_count;++layer) {
            const BackgroundLayer bg=l->backgrounds[layer];
            const float offset=fmodf(camera_x*bg.parallax,bg.spacing);
            for(float bx=-offset-bg.width;bx<width;bx+=bg.spacing)
                graphics_rect(bx,bg.y-camera_y*bg.parallax,bg.width,bg.height,(GXColor){bg.r,bg.g,bg.b,255});
        }
    } else {
        for (int i=0;i<8;++i) {
            const float bx=i*420-camera_x*0.3f;
            graphics_rect(bx,290-camera_y*0.12f,240,200,(GXColor){66,150,100,255});
            graphics_rect(bx+60,90,95,18,white);
        }
    }
    WorldQuery query;WorldShape shape;
    world_query_begin(&query,world,(Rect){camera_x,camera_y,width,480});
    while(world_query_next(&query,&shape)) {
        const Rect r=shape.bounds;const float tx=r.x-camera_x,ty=r.y-camera_y;
        GXColor color={138,91,55,255};
        if(shape.type==TILE_WATER) color=(GXColor){45,120,200,255};
        else if(shape.type==TILE_ICE) color=(GXColor){170,225,245,255};
        else if(shape.type==TILE_PUDDLE) color=(GXColor){70,145,160,255};
        else if(shape.type==TILE_HAZARD) color=(GXColor){220,70,45,255};
        if(shape.type==TILE_SLOPE_UP)
            graphics_triangle(tx,ty+r.h,tx+r.w,ty,tx+r.w,ty+r.h,color);
        else if(shape.type==TILE_SLOPE_DOWN)
            graphics_triangle(tx,ty,tx+r.w,ty+r.h,tx,ty+r.h,color);
        else {
            graphics_rect(tx,ty,r.w,shape.type==TILE_PLATFORM ? 12 : r.h,color);
            if(shape.type!=TILE_WATER) graphics_rect(tx,ty,r.w,4,(GXColor){74,188,79,255});
        }
    }
    const float x = lerp(p->previous_x, p->x, interpolation) - camera_x;
    const float y = lerp(p->previous_y, p->y, interpolation) - camera_y;
    if(!user_art_draw_player(p,x,y)) draw_player(p, x, y);
    if (p->landing_time > 0) {
        const float spread = (0.18f - p->landing_time) * 220;
        graphics_rect(x - spread, y + PLAYER_HEIGHT - 5, 10, 5, white);
        graphics_rect(x + PLAYER_WIDTH + spread, y + PLAYER_HEIGHT - 5, 10, 5, white);
    }
    static const char *states[] = {"IDLE", "RUN", "RISE", "FALL", "SPIN", "WINDUP", "POUND", "LAND", "LOW DIVE", "DIVE", "BACKFLIP", "ROLL", "SWIM",
        "HOVER", "CHARGING", "ROCKET", "TURBO", "BONK", "LOW DIVE"};
    /* Every PlayerAnimation needs a label; a missing one is an out-of-bounds read. */
    _Static_assert(sizeof(states) / sizeof(states[0]) == PLAYER_ANIMATION_COUNT,
                   "HUD state labels must match PlayerAnimation");
    char hud[100];
    static const char *nozzles[]={"H","R","T"};
    snprintf(hud, sizeof(hud), "V0.4.1 %s %s W%u P%u", states[p->animation], nozzles[p->nozzle], (unsigned)p->water, (unsigned)p->power);
    graphics_rect(14, 14, width - 28, 28, (GXColor){28, 40, 65, 255});
    graphics_text(22, 21, hud, 2, white);
    if(!user_art_available()) graphics_text(22,46,"PRIVATE ART PACK MISSING",1,white);
    else if(!user_art_fludd_available(p)) graphics_text(22,46,"FLUDD ART NOT LOADED",1,white);
    if (game->paused) {
        const float left = width / 2 - 155;
        graphics_rect(left, 135, 310, 210, (GXColor){25, 30, 55, 255});
        graphics_text(left + 80, 153, "PAUSED", 3, white);
        static const char *options[] = {"RESUME", "RESTART", "EXIT TO LOADER"};
        for (unsigned i = 0; i < 3; ++i) {
            const float top = 205 + i * 42;
            if (i == game->menu_selection)
                graphics_rect(left + 15, top - 7, 280, 30, (GXColor){65, 95, 155, 255});
            graphics_text(left + 30, top, options[i], 2, white);
        }
    }
}
