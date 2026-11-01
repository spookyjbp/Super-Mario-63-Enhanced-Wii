#include "render.h"
#include "graphics.h"
#include "mario_atlas.h"
#include "mario_rgba8_bin.h"
#include <math.h>
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
}

static void draw_player(const Player *p, float x, float y)
{
    unsigned frame = 0;
    switch (p->animation) {
    case PLAYER_RUN: frame = 1 + (unsigned)(p->animation_time * 12) % 8; break;
    case PLAYER_RISE: frame = 9; break;
    case PLAYER_FALL: frame = 10; break;
    case PLAYER_SPIN: {
        /* Original front/side/back turn order, fitted to the existing spin window. */
        static const unsigned cycle[] = {13, 12, 11, 12, 13, 14, 15, 14};
        const unsigned tick = (unsigned)(fmaxf(0, 0.32f - p->spin_time) * 25);
        frame = cycle[tick % 8];
        break;
    }
    case PLAYER_POUND_WINDUP: frame = p->pound_windup > 0.06f ? 16 : 17; break;
    case PLAYER_POUND: frame = 18; break;
    case PLAYER_LAND: frame = 19; break;
    case PLAYER_IDLE: break;
    }
    const MarioFrame sprite = mario_frames[frame];
    const float scale = 1.5f;
    const float width = sprite.width * scale, height = sprite.height * scale;
    float u0 = (float)sprite.x / MARIO_ATLAS_SIZE;
    float u1 = (float)(sprite.x + sprite.width) / MARIO_ATLAS_SIZE;
    if (p->facing_left != (frame >= 16 && frame <= 18)) { const float swap = u0; u0 = u1; u1 = swap; }
    graphics_sprite(&mario_texture, x + (PLAYER_WIDTH - width) / 2,
                    y + PLAYER_HEIGHT - height, width, height,
                    u0, (float)sprite.y / MARIO_ATLAS_SIZE,
                    u1, (float)(sprite.y + sprite.height) / MARIO_ATLAS_SIZE);
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
    for (int i = 0; i < 8; ++i) {
        const float x = i * 420 - camera_x * 0.3f;
        graphics_rect(x, 290 - camera_y * 0.12f, 240, 200, (GXColor){66, 150, 100, 255});
        graphics_rect(x + 60, 90, 95, 18, white);
    }
    for (size_t i = 0; i < world->count; ++i) {
        const Rect r = world->solids[i];
        if (r.x + r.w < camera_x || r.x > camera_x + width ||
            r.y + r.h < camera_y || r.y > camera_y + 480) continue;
        graphics_rect(r.x - camera_x, r.y - camera_y, r.w, r.h, (GXColor){138, 91, 55, 255});
        graphics_rect(r.x - camera_x, r.y - camera_y, r.w, 6, (GXColor){74, 188, 79, 255});
    }
    const float x = lerp(p->previous_x, p->x, interpolation) - camera_x;
    const float y = lerp(p->previous_y, p->y, interpolation) - camera_y;
    draw_player(p, x, y);
    if (p->spin_time > 0) {
        for (int i = 0; i < 6; ++i) {
            const float phase = p->animation_time * 24 + i * 1.0472f;
            graphics_rect(x + 10 + cosf(phase) * 30, y + 20 + sinf(phase) * 15, 7, 7,
                          (GXColor){255, 250, 120, 255});
        }
    }
    if (p->landing_time > 0) {
        const float spread = (0.18f - p->landing_time) * 220;
        graphics_rect(x - spread, y + PLAYER_HEIGHT - 5, 10, 5, white);
        graphics_rect(x + PLAYER_WIDTH + spread, y + PLAYER_HEIGHT - 5, 10, 5, white);
    }
    static const char *states[] = {"IDLE", "RUN", "RISE", "FALL", "SPIN", "WINDUP", "POUND", "LAND"};
    char hud[80];
    snprintf(hud, sizeof(hud), "V0.3  %s  JUMP %u", states[p->animation], p->jump_stage);
    graphics_rect(14, 14, width - 28, 28, (GXColor){28, 40, 65, 255});
    graphics_text(22, 21, hud, 2, white);
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
