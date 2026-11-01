#include "game.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

static const Rect floor_rect[] = {{0, 400, 2400, 80}};
static const World floor_world = {.solids=floor_rect, .count=1, .width=2400, .height=960, .spawn_x=100, .spawn_y=356};
static Player standing(void)
{
    Player p;
    player_init(&p, 100, 356);
    player_step(&p, &floor_world, (GameInput){0});
    assert(p.grounded);
    return p;
}
static void ticks(Player *p, const World *w, GameInput input, int count)
{
    for (int i = 0; i < count; ++i) player_step(p, w, input);
}
static void test_acceleration(void)
{
    Player p = standing();
    player_step(&p, &floor_world, (GameInput){.horizontal = 1});
    assert(p.vx > 0 && p.vx < 280);
    ticks(&p, &floor_world, (GameInput){.horizontal = 1}, 30);
    assert(fabsf(p.vx - 280) < 0.01f);
    player_step(&p, &floor_world, (GameInput){.horizontal = -1});
    assert(p.vx > 0 && p.vx < 280); /* Reversal is responsive but not instantaneous. */
    ticks(&p, &floor_world, (GameInput){.horizontal = -1}, 30);
    assert(p.vx == -280);
    ticks(&p, &floor_world, (GameInput){0}, 30);
    assert(p.vx == 0);
}
static float jump_height(bool held)
{
    Player p = standing();
    float top = p.y;
    player_step(&p, &floor_world, (GameInput){.jump_pressed = true, .jump_held = true});
    for (int i = 0; i < 120; ++i) {
        player_step(&p, &floor_world, (GameInput){.jump_held = held});
        top = fminf(top, p.y);
    }
    assert(p.grounded && p.y == 356);
    return 356 - top;
}
static void test_variable_jump(void)
{
    const float tap = jump_height(false), hold = jump_height(true);
    assert(hold > tap * 1.8f && hold < 120 && tap > 10);
    Player p = standing();
    ticks(&p, &floor_world, (GameInput){.jump_pressed = false, .jump_held = true}, 90);
    assert(p.grounded); /* Holding without a new press never auto-jumps. */
}
static void land(Player *p)
{
    for (int i = 0; i < 180 && !p->grounded; ++i)
        player_step(p, &floor_world, (GameInput){.jump_held = true});
    assert(p->grounded);
}
static void test_jump_chain(void)
{
    Player p = standing();
    float previous_speed = 0;
    for (unsigned stage = 1; stage <= 3; ++stage) {
        if(stage==3) p.vx=200; /* Original triple jump requires horizontal speed >3. */
        player_step(&p, &floor_world, (GameInput){.jump_pressed = true, .jump_held = true});
        assert(p.jump_stage == stage && -p.vy > previous_speed);
        previous_speed = -p.vy;
        land(&p);
        ticks(&p, &floor_world, (GameInput){0}, 3);
    }
    player_step(&p, &floor_world, (GameInput){.jump_pressed = true, .jump_held = true});
    assert(p.jump_stage == 1);
    land(&p);
    ticks(&p, &floor_world, (GameInput){0}, 20);
    player_step(&p, &floor_world, (GameInput){.jump_pressed = true, .jump_held = true});
    assert(p.jump_stage == 1); /* Timing window expired. */
}
static void test_buffer_and_coyote(void)
{
    Player p;
    player_init(&p, 100, 350);
    p.vy = 150;
    player_step(&p, &floor_world, (GameInput){.jump_pressed = true, .jump_held = true});
    for (int i = 0; i < 5 && p.vy >= 0; ++i)
        player_step(&p, &floor_world, (GameInput){.jump_held = true});
    assert(p.vy < 0 && p.jump_stage == 1);
    const Rect ledge[] = {{0, 400, 150, 50}};
    const World w = {.solids=ledge, .count=1, .width=2400, .height=960, .spawn_x=0, .spawn_y=0};
    p = standing(); p.x = 149; p.vx = 280;
    player_step(&p, &w, (GameInput){.horizontal = 1});
    assert(!p.grounded);
    player_step(&p, &w, (GameInput){.jump_pressed = true, .jump_held = true});
    assert(p.vy < 0); /* Late jump just off an edge. */
    p = standing(); p.x = 149; p.vx = 280;
    ticks(&p, &w, (GameInput){.horizontal = 1}, 10);
    player_step(&p, &w, (GameInput){.jump_pressed = true, .jump_held = true});
    assert(p.vy > 0); /* No jump after the coyote window. */
}
static void test_collisions(void)
{
    const Rect solids[] = {{0, 400, 800, 40}, {200, 200, 20, 200}, {50, 250, 90, 20}};
    const World w = {.solids=solids, .count=3, .width=800, .height=960, .spawn_x=0, .spawn_y=0};
    Player p = standing();
    ticks(&p, &w, (GameInput){.horizontal = 1}, 100);
    assert(p.x == 200 - PLAYER_WIDTH && p.vx == 0 && p.grounded);
    p = standing();
    player_step(&p, &w, (GameInput){.jump_pressed = true, .jump_held = true});
    float minimum = p.y;
    for (int i = 0; i < 90; ++i) {
        player_step(&p, &w, (GameInput){.jump_held = true});
        minimum = fminf(minimum, p.y);
    }
    assert(minimum >= 270 && p.grounded); /* Underside of low ceiling. */
    const Rect thin[] = {{0, 400, 800, 2}};
    const World thin_world = {.solids=thin, .count=1, .width=800, .height=960, .spawn_x=0, .spawn_y=0};
    player_init(&p, 100, 340); p.vy = 1200; p.pounding = true;
    player_step(&p, &thin_world, (GameInput){0});
    assert(p.y == 356 && p.grounded && p.vy == 0); /* No tunnelling. */
    player_init(&p, 170, 300); p.vx = 1000;
    player_step(&p, &w, (GameInput){.horizontal = 1});
    assert(p.x <= 172); /* Horizontal sweep at excessive speed. */
    const Rect left_wall[] = {{50, 200, 20, 200}, {0, 400, 800, 40}};
    const World left = {.solids=left_wall, .count=2, .width=800, .height=960, .spawn_x=0, .spawn_y=0};
    p = standing();
    ticks(&p, &left, (GameInput){.horizontal = -1}, 40);
    assert(p.x == 70 && p.vx == 0);
    const Rect edge[] = {{0, 400, 150, 30}};
    const World edges = {.solids=edge, .count=1, .width=800, .height=960, .spawn_x=0, .spawn_y=0};
    p = standing(); p.x = 150;
    player_step(&p, &edges, (GameInput){0});
    assert(!p.grounded && p.y > 356); /* Exact edge has no support. */
}
static void test_spin_and_pound(void)
{
    Player p = standing();
    ticks(&p, &floor_world, (GameInput){.spin_held=true},2);
    assert(p.animation == PLAYER_SPIN);
    Rect attack;
    assert(player_attack_hitbox(&p, &attack) && attack.w > PLAYER_WIDTH);
    ticks(&p, &floor_world, (GameInput){0}, 40);
    assert(p.spin_time == 0 && !player_attack_hitbox(&p, &attack));
    player_step(&p, &floor_world, (GameInput){.jump_pressed = true, .jump_held = true});
    p.vy=0;
    ticks(&p, &floor_world, (GameInput){.spin_held=true, .jump_held = true},2);
    assert(p.spin_time > 0);
    p.spin_speed = p.spin_time = 0;
    player_step(&p, &floor_world, (GameInput){.spin_pressed = true});
    ticks(&p, &floor_world, (GameInput){.spin_held=true},2);
    assert(p.spin_time > 0); /* Original permits another spin after an attack ends. */
    player_step(&p, &floor_world, (GameInput){.pound_pressed = true});
    assert(p.pounding && p.animation == PLAYER_POUND_WINDUP && p.vy == 0);
    assert(!player_attack_hitbox(&p, &attack));
    ticks(&p, &floor_world, (GameInput){0}, 8);
    assert(p.animation == PLAYER_POUND && p.vy > 0);
    assert(player_attack_hitbox(&p, &attack) && attack.y > p.y);
    for (int i = 0; i < 120 && !p.grounded; ++i) player_step(&p, &floor_world, (GameInput){0});
    assert(p.grounded && !p.pounding && p.animation == PLAYER_LAND && p.vy == 0);
    player_step(&p, &floor_world, (GameInput){.horizontal = 1, .jump_pressed = true, .jump_held = true});
    assert(p.grounded && p.vx == 0); /* Landing recovery. */
    ticks(&p, &floor_world, (GameInput){0}, 20);
    assert(p.animation == PLAYER_IDLE);
}
static void test_camera(void)
{
    Player p = standing();
    Camera c;
    camera_init(&c, &p, &floor_world, 854);
    assert(c.x == 0 && c.y >= 0);
    p.x = 2000;
    camera_step(&c, &p, &floor_world, 854);
    assert(c.x > 0 && c.x < 1546);
    p.x = 3000; p.y = 2000;
    for (int i = 0; i < 500; ++i) camera_step(&c, &p, &floor_world, 854);
    assert(c.x <= 1546 && c.y <= 480);
    const World tiny = {.solids=NULL, .count=0, .width=320, .height=240, .spawn_x=10, .spawn_y=10};
    camera_init(&c, &p, &tiny, 854);
    camera_step(&c, &p, &tiny, 854);
    assert(c.x == 0 && c.y == 0);
}
static void test_pause_and_respawn(void)
{
    Game g;
    game_init(&g, &floor_world, 640);
    game_step(&g, &floor_world, (GameInput){0}, 640);
    const Player before = g.player;
    game_step(&g, &floor_world, (GameInput){.pause_pressed = true}, 640);
    for (int i = 0; i < 90; ++i)
        game_step(&g, &floor_world, (GameInput){.horizontal = 1, .jump_pressed = true}, 640);
    assert(g.paused && g.player.x == before.x && g.player.y == before.y);
    assert(g.player.animation_time == before.animation_time);
    game_step(&g, &floor_world, (GameInput){.cancel_pressed = true}, 640);
    assert(!g.paused);
    g.player.y = 2000;
    game_step(&g, &floor_world, (GameInput){0}, 640);
    assert(g.player.x == floor_world.spawn_x && g.player.y == floor_world.spawn_y);
    game_step(&g, &floor_world, (GameInput){.pause_pressed = true}, 640);
    game_step(&g, &floor_world, (GameInput){.menu_delta = 1}, 640);
    game_step(&g, &floor_world, (GameInput){.confirm_pressed = true}, 640);
    assert(!g.paused && g.player.x == floor_world.spawn_x);
    game_step(&g, &floor_world, (GameInput){.pause_pressed = true}, 640);
    game_step(&g, &floor_world, (GameInput){.menu_delta = -1}, 640);
    assert(g.menu_selection == 2);
    game_step(&g, &floor_world, (GameInput){.confirm_pressed = true}, 640);
    assert(g.quit);
}
static void test_input_latching(void)
{
    GameInput p = {0};
    game_input_latch(&p, (GameInput){.jump_pressed = true, .jump_held = true, .horizontal = 1});
    game_input_latch(&p, (GameInput){.jump_held = true, .horizontal = 1});
    assert(p.jump_pressed && p.jump_held && p.horizontal == 1);
    game_input_consume(&p);
    assert(!p.jump_pressed && p.jump_held && p.horizontal == 1);
    game_input_latch(&p, (GameInput){0});
    assert(!p.jump_held && p.horizontal == 0);
}
int main(void)
{
    test_acceleration(); test_variable_jump(); test_jump_chain();
    test_buffer_and_coyote(); test_collisions(); test_spin_and_pound();
    test_camera(); test_pause_and_respawn(); test_input_latching();
    puts("9 physics/game test groups passed");
    return 0;
}
