#include "game.h"
#include "course_world.h"
#include <math.h>

static float clamp(float value, float maximum)
{
    return fmaxf(0, fminf(value, fmaxf(0, maximum)));
}
void camera_init(Camera *c, const Player *p, const World *w, float width)
{
    if (w->course) { course_camera_init(c, p, w, width); return; }
    c->x = c->previous_x = clamp(p->x + PLAYER_WIDTH / 2 - width / 2, w->width - width);
    c->y = c->previous_y = clamp(p->y + PLAYER_HEIGHT / 2 - 280, w->height - 480);
}
void camera_step(Camera *c, const Player *p, const World *w, float width)
{
    if (w->course) { course_camera_step(c, p, w, width); return; }
    c->previous_x = c->x;
    c->previous_y = c->y;
    const float x = clamp(p->x + PLAYER_WIDTH / 2 - width / 2, w->width - width);
    const float y = clamp(p->y + PLAYER_HEIGHT / 2 - 280, w->height - 480);
    c->x = clamp(c->x + (x - c->x) * 0.12f, w->width - width);
    c->y = clamp(c->y + (y - c->y) * 0.12f, w->height - 480);
}
