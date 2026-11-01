#include "graphics.h"
#include "render.h"
#include "player_sprite.h"
#include "mario_atlas.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

const unsigned char mario_rgba8_bin[1] = {0};
const unsigned int mario_rgba8_bin_size = 1;
static struct {
    unsigned rects, sprites, yellow_squares;
    float x, y, w, h, u0, v0, u1, v1;
} draws;

void DCFlushRange(void *data, unsigned size) { (void)data; (void)size; }
void GX_InitTexObj(GXTexObj *t, void *data, unsigned w, unsigned h,
                   unsigned format, unsigned s, unsigned v, unsigned mip)
{
    (void)t; (void)data; (void)w; (void)h; (void)format; (void)s; (void)v; (void)mip;
}
void GX_InitTexObjLOD(GXTexObj *t, unsigned min, unsigned mag, float lo,
                      float hi, float bias, unsigned clamp, unsigned edge, unsigned aniso)
{
    (void)t; (void)min; (void)mag; (void)lo; (void)hi; (void)bias;
    (void)clamp; (void)edge; (void)aniso;
}
void GX_InvalidateTexAll(void) {}
float graphics_width(void) { return 640; }
void graphics_rect(float x, float y, float w, float h, GXColor color)
{
    (void)x; (void)y;
    ++draws.rects;
    if (w == 7 && h == 7 && color.r == 255 && color.g == 250 && color.b == 120)
        ++draws.yellow_squares;
}
void graphics_text(float x, float y, const char *text, float scale, GXColor color)
{
    (void)x; (void)y; (void)text; (void)scale; (void)color;
}
void graphics_sprite(GXTexObj *t, float x, float y, float w, float h,
                     float u0, float v0, float u1, float v1)
{
    (void)t;
    ++draws.sprites;
    draws.x = x; draws.y = y; draws.w = w; draws.h = h;
    draws.u0 = u0; draws.v0 = v0; draws.u1 = u1; draws.v1 = v1;
}
static void near(float a, float b) { assert(fabsf(a - b) < 0.001f); }
static void record(const Game *game, const World *world, float alpha)
{
    Game before;
    memcpy(&before, game, sizeof(before));
    memset(&draws, 0, sizeof(draws));
    render_game(game, world, alpha);
    assert(memcmp(&before, game, sizeof(before)) == 0);
    assert(draws.sprites == 1);
    assert(draws.yellow_squares == 0);
}
static void test_no_spin_placeholders(void)
{
    const World world = {NULL, 0, 2400, 960, 100, 100};
    Game game = {0};
    game.player.animation = PLAYER_IDLE;
    record(&game, &world, 0);
    const unsigned baseline = draws.rects;
    for (unsigned air = 0; air < 2; ++air)
        for (unsigned phase = 0; phase < 8; ++phase) {
            game.player.grounded = air == 0;
            game.player.animation = PLAYER_SPIN;
            game.player.spin_time = 0.32f - (phase + 0.5f) / 25;
            record(&game, &world, 0);
            assert(draws.rects == baseline); /* Old loop added six draws. */
        }
}
static void test_spin_tracks_player_and_camera(void)
{
    const World world = {NULL, 0, 2400, 960, 100, 100};
    Game game = {0};
    game.player.animation = PLAYER_SPIN;
    game.player.spin_time = 0.30f;
    game.player.previous_x = 100; game.player.x = 200;
    game.player.previous_y = 200; game.player.y = 300;
    game.camera.previous_x = 10; game.camera.x = 30;
    game.camera.previous_y = 20; game.camera.y = 40;
    record(&game, &world, 0.5f);
    /* Frame 13 is 16x32: 24x48 at 1.5x, centered in a 28x44 collider.
       Interpolated player top-left (150,250), camera (20,30). */
    near(draws.x, 132); near(draws.y, 216);
    near(draws.w, 24); near(draws.h, 48);
    game.player.previous_x += 50; game.player.x += 50;
    game.player.previous_y -= 80; game.player.y -= 80;
    record(&game, &world, 0.5f);
    near(draws.x, 182); near(draws.y, 136);
    game.camera.previous_x += 25; game.camera.x += 25;
    game.camera.previous_y += 10; game.camera.y += 10;
    record(&game, &world, 0.5f);
    near(draws.x, 157); near(draws.y, 126);
}
static void test_direction_reaches_uvs(void)
{
    const World world = {NULL, 0, 2400, 960, 100, 100};
    Game game = {0};
    for (unsigned left = 0; left < 2; ++left) {
        game.player.facing_left = left != 0;
        for (int state = PLAYER_RISE; state <= PLAYER_SPIN; ++state) {
            game.player.animation = (PlayerAnimation)state;
            for (unsigned phase = 0; phase < 8; ++phase) {
                game.player.spin_time = 0.32f - (phase + 0.5f) / 25;
                const PlayerSprite pose = player_sprite_select(&game.player);
                const MarioFrame frame = mario_frames[pose.frame];
                record(&game, &world, 1);
                const float lo = (float)frame.x / MARIO_ATLAS_SIZE;
                const float hi = (float)(frame.x + frame.width) / MARIO_ATLAS_SIZE;
                near(draws.u0, pose.flip_x ? hi : lo);
                near(draws.u1, pose.flip_x ? lo : hi);
                assert(draws.v0 < draws.v1); /* Never invert vertically. */
            }
        }
    }
}
int main(void)
{
    render_init();
    test_no_spin_placeholders();
    test_spin_tracks_player_and_camera();
    test_direction_reaches_uvs();
    puts("3 render command regression groups passed (not a visual/GX test)");
    return 0;
}
