#include "game.h"
#include <string.h>

void game_init(Game *g, const World *w, float width)
{
    memset(g, 0, sizeof(*g));
    player_init(&g->player, w->spawn_x, w->spawn_y);
    camera_init(&g->camera, &g->player, w, width);
}
void game_step(Game *g, const World *w, GameInput input, float width)
{
    if (g->quit) return;
    if (input.pause_pressed) {
        g->paused = !g->paused;
        g->menu_selection = 0;
        /* Freeze interpolation too, and ignore gameplay presses on this tick. */
        g->player.previous_x = g->player.x;
        g->player.previous_y = g->player.y;
        g->camera.previous_x = g->camera.x;
        g->camera.previous_y = g->camera.y;
        return;
    }
    if (g->paused) {
        g->player.previous_x = g->player.x;
        g->player.previous_y = g->player.y;
        g->camera.previous_x = g->camera.x;
        g->camera.previous_y = g->camera.y;
        if (input.menu_delta) g->menu_selection = (g->menu_selection + (input.menu_delta > 0 ? 1 : 2)) % 3;
        if (input.cancel_pressed) g->paused = false;
        else if (input.confirm_pressed) {
            if (g->menu_selection == 0) g->paused = false;
            else if (g->menu_selection == 1) game_init(g, w, width);
            else g->quit = true;
        }
        return;
    }
    player_step(&g->player, w, input);
    if (g->player.y > w->height + 100) {
        player_init(&g->player, w->spawn_x, w->spawn_y);
        camera_init(&g->camera, &g->player, w, width);
    }
    camera_step(&g->camera, &g->player, w, width);
}
void game_input_latch(GameInput *p, GameInput current)
{
    p->horizontal = current.horizontal;
    p->jump_held = current.jump_held;
    p->jump_pressed |= current.jump_pressed;
    p->spin_pressed |= current.spin_pressed;
    p->pound_pressed |= current.pound_pressed;
    p->pause_pressed |= current.pause_pressed;
    p->confirm_pressed |= current.confirm_pressed;
    p->cancel_pressed |= current.cancel_pressed;
    if (current.menu_delta) p->menu_delta = current.menu_delta;
}
void game_input_consume(GameInput *p)
{
    const float horizontal = p->horizontal;
    const bool held = p->jump_held;
    *p = (GameInput){.horizontal = horizontal, .jump_held = held};
}
