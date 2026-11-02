/* Original course collision and camera (Runouw/Super-Mario-63 83e81fb,
   DOMDocument.xml CharCode 6262-6420 and camera 5200-5290), applied to the
   port's existing velocities. Collision probes run in course pixels against the
   exported hitTest masks; see docs/course-collision.md. */
#include "course_world.h"
#include "course_objects.h"
#include <math.h>

#define V 44.0f /* port px/s per original px/frame */

void course_runtime_init(CourseRuntime *c, const CourseArea *a)
{
    const CourseMask *edge = course_area_mask(a, COURSE_CAMEDGE, 0);
    *c = (CourseRuntime){.area = a, .objects = NULL, .ox = edge ? (float)edge->x0 : 0, .oy = edge ? (float)edge->y0 : 0};
}
World course_world(CourseRuntime *c)
{
    const CourseMask *edge = course_area_mask(c->area, COURSE_CAMEDGE, 0);
    const float w = edge ? edge->width * COURSE_SCALE : 4000, h = edge ? edge->height * COURSE_SCALE : 3000;
    return (World){.width = w, .height = h, .spawn_x = course_wx(c, 0) - PLAYER_WIDTH / 2,
                   .spawn_y = course_wy(c, 0) - PLAYER_HEIGHT, .course = c};
}
bool course_solid(const CourseRuntime *c, float x, float y)
{
    return course_area_hit(c->area, COURSE_PLATFORMS, c->platform_frame, x, y) ||
           (c->objects && course_objects_solid(c->objects, x, y));
}
bool course_bsolid(const CourseRuntime *c, float x, float y)
{
    return course_area_hit(c->area, COURSE_BPLATFORMS, 0, x, y) ||
           (c->objects && course_objects_platform(c->objects, x, y));
}
void course_place_player(const CourseRuntime *c, Player *p, float cx, float cy)
{
    p->x = p->previous_x = course_wx(c, cx) - PLAYER_WIDTH / 2;
    p->y = p->previous_y = course_wy(c, cy) - player_height(p);
}
static float tall_of(const Player *p)
{ return p->action == ACTION_DIVE ? 25.0f : p->has_fludd ? 40.0f : 34.0f; } /* CharCode tall */
static float wide_of(const Player *p) { return p->action == ACTION_DIVE ? 8.0f : 10.0f; }

bool course_stand_clear(const Player *p, const World *w)
{
    const CourseRuntime *c = w->course;
    const float x = course_cx(c, p->x + PLAYER_WIDTH / 2), y = course_cy(c, p->y + player_height(p));
    const float tall = p->has_fludd ? 40.0f : 34.0f;
    /* Only the part above the diving/crouching Mario (tall 25) can block standing
       up; checking lower made every slope look like a ceiling, so crouch
       backflips and diveflips failed on the tutorial's hills. */
    for (float k = 26; k <= tall; k += 2)
        if (course_solid(c, x, y - k) || course_solid(c, x - 8, y - k) || course_solid(c, x + 8, y - k)) return false;
    return true;
}
static float surface_below(const CourseRuntime *c, float x, float y)
{
    for (int k = -8; k <= 8; ++k)
        if (course_solid(c, x, y + k) || course_bsolid(c, x, y + k)) return y + k;
    return NAN;
}
void course_collision_step(Player *p, const World *w)
{
    const CourseRuntime *c = w->course;
    const bool dive = p->action == ACTION_DIVE;
    const float wide = wide_of(p), tall = tall_of(p), friction = dive ? 0.997f : 0.98f;
    float x = course_cx(c, p->x + PLAYER_WIDTH / 2), y = course_cy(c, p->y + player_height(p));
    const float travel = (fabsf(p->vx) + fabsf(p->vy)) * GAME_DT / COURSE_SCALE;
    /* CharCode: repeats = ceil((|xspeed|+|yspeed|)/5) per 32 Hz frame, 1..50. */
    const int repeats = (int)fmaxf(1, fminf(50, ceilf(travel / (5.0f * 60 / 32))));
    const bool lockon = p->grounded && p->vy >= 0 && !p->submerged; /* lockon=false on jumps/water */
    bool grounded = false;
    p->wall_normal = 0; p->wall_speed = 0;
    for (int i = 0; i < repeats; ++i) {
        if (course_solid(c, x, y) || course_bsolid(c, x, y)) { grounded = true; p->vy = fminf(0, p->vy); }
        for (int n = 0; n < 64 && (course_solid(c, x, y - 3) ||
                                   (course_bsolid(c, x, y - 3) && p->vy / V >= -3)); ++n) {
            y -= 1; p->vx *= friction; p->vy = fminf(0, p->vy); grounded = true;
        }
        if (lockon) {
            for (int n = 0; n < 16 && course_solid(c, x, y + 5) && !course_solid(c, x, y - 1); ++n) { y += 1; p->vx *= dive ? 0.998f : 0.98f; }
            for (int n = 0; n < 16 && course_bsolid(c, x, y + 5) && !course_bsolid(c, x, y - 1) && p->vy / V >= -3; ++n)
                { y += 1; p->vx *= dive ? 0.998f : 0.98f; }
        }
        const float mid = y - tall / 2;
        for (int n = 0; n < 32 && course_solid(c, x + wide - 1, mid); ++n) x -= 1;
        for (int n = 0; n < 32 && course_solid(c, x - wide + 1, mid); ++n) x += 1;
        /* Wall contact: bonk handled by player_step from wall_speed; else bounce. */
        if (course_solid(c, x + wide, mid) && p->vx > 0) {
            p->wall_normal = -1; p->wall_speed = p->vx;
            float s = p->vx / V;
            if (p->input_horizontal > 0) s = fminf(s, -s * 0.5f), s = copysignf(fmaxf(0, fabsf(s) - 3), s);
            else s = fminf(s - 1, -s * 0.5f);
            p->vx = s * V;
        }
        if (course_solid(c, x - wide, mid) && p->vx < 0) {
            p->wall_normal = 1; p->wall_speed = p->vx;
            float s = p->vx / V;
            if (p->input_horizontal < 0) s = fminf(s, -s * 0.5f), s = copysignf(fmaxf(0, fabsf(s) - 3), s);
            else s = fmaxf(s + 1, -s * 0.5f);
            p->vx = s * V;
        }
        if (course_solid(c, x, y - tall + 1)) p->vy = fmaxf(p->vy, -3 * V); /* ceiling */
        for (int n = 0; n < 32 && course_solid(c, x, y - tall); ++n) y += 1;
        x += p->vx * GAME_DT / COURSE_SCALE / repeats;
        y += p->vy * GAME_DT / COURSE_SCALE / repeats;
    }
    if (!grounded && p->vy >= 0) grounded = course_solid(c, x, y + 1) || course_bsolid(c, x, y + 1);
    p->grounded = grounded;
    if (grounded && p->vy > 0) p->vy = 0;
    p->material = TILE_EMPTY; p->ground_slope = 0; p->ground_angle = 0;
    if (grounded) {
        p->material = course_area_hit(c->area, COURSE_ICE, 0, x, y + 1) ? TILE_ICE :
                      course_area_hit(c->area, COURSE_PUDDLE, 0, x, y + 1) ? TILE_PUDDLE : TILE_SOLID;
        const float left = surface_below(c, x - 5, y), right = surface_below(c, x + 5, y);
        if (isfinite(left) && isfinite(right) && fabsf(right - left) >= 3)
            p->ground_slope = right < left ? -1 : 1; /* port slide: -1 rises to the right */
        if (isfinite(left) && isfinite(right)) p->ground_angle = atan2f(right - left, 10) * 57.29578f;
    }
    p->x = course_wx(c, x) - PLAYER_WIDTH / 2;
    p->y = course_wy(c, y) - player_height(p);
}

/* Camera: Mario is the camera centre's anchor. Each 32 Hz frame the original
   moves the course by (-Char + CamChange)/Camspeed - speed/4 (Camspeed 10), then
   pushes the screen-edge midpoints out of CamEdge by up to 50 px. */
static float half_width(float view_width) { return view_width / course_view_zoom() / COURSE_SCALE / 2; }
static void edge_push(const CourseRuntime *c, Camera *cam, float hw)
{
    const float hh = COURSE_VIEW_HEIGHT / 2;
    const CourseArea *a = c->area;
    int n = 0;
    while (n < 50 && course_area_hit(a, COURSE_CAMEDGE, 0, cam->cx, cam->cy + hh - n)) ++n;
    cam->cy -= n; n = 0;
    while (n < 50 && course_area_hit(a, COURSE_CAMEDGE, 0, cam->cx, cam->cy - hh + n)) ++n;
    cam->cy += n; n = 0;
    while (n < 50 && course_area_hit(a, COURSE_CAMEDGE, 0, cam->cx + hw - n, cam->cy)) ++n;
    cam->cx -= n; n = 0;
    while (n < 50 && course_area_hit(a, COURSE_CAMEDGE, 0, cam->cx - hw + n, cam->cy)) ++n;
    cam->cx += n;
}
static void publish(Camera *cam, const CourseRuntime *c, float view_width, bool moved, float frac)
{
    const float zoom = course_view_zoom();
    const float x = course_wx(c, cam->cx) - view_width / zoom / 2, y = course_wy(c, cam->cy) - 480.0f / zoom / 2;
    if (moved) { cam->from_x = cam->to_x; cam->from_y = cam->to_y; cam->to_x = x; cam->to_y = y; }
    /* A jump (area change, warp) is not eased. */
    if (fabsf(cam->to_x - cam->from_x) > 200 || fabsf(cam->to_y - cam->from_y) > 200) { cam->from_x = x; cam->from_y = y; }
    cam->x = cam->from_x + (cam->to_x - cam->from_x) * frac;
    cam->y = cam->from_y + (cam->to_y - cam->from_y) * frac;
}
/* Progress through the current source frame: p->source_phase (0..59) counts 32
   per tick and a source frame ran when it wrapped. */
static float source_frac(const Player *p) { return fminf(1, p->source_phase / 60.0f); }
void course_camera_init(Camera *cam, const Player *p, const World *w, float view_width)
{
    const CourseRuntime *c = w->course;
    cam->cx = course_cx(c, p->x + PLAYER_WIDTH / 2);
    cam->cy = course_cy(c, p->y + player_height(p)) - 50;
    cam->ychange = 50;
    for (int i = 0; i < 4; ++i) edge_push(c, cam, half_width(view_width));
    publish(cam, c, view_width, true, 1);
    cam->from_x = cam->to_x; cam->from_y = cam->to_y;
    cam->x = cam->to_x; cam->y = cam->to_y;
    cam->previous_x = cam->x; cam->previous_y = cam->y;
}
/* One 32 Hz camera move toward `dx`, `dy` (course px), limited by CamEdge as in
   the original: moving toward an edge the view already touches goes at most
   1 px, then the edge midpoints are pushed out of CamEdge. */
static void camera_move(Camera *cam, const CourseRuntime *c, float dx, float dy, float view_width)
{
    const float hw = half_width(view_width), hh = COURSE_VIEW_HEIGHT / 2;
    const CourseArea *a = c->area;
    if (course_area_hit(a, COURSE_CAMEDGE, 0, cam->cx, cam->cy + hh + fmaxf(dy, 0) + 1)) dy = fminf(1, dy);
    if (course_area_hit(a, COURSE_CAMEDGE, 0, cam->cx, cam->cy - hh + fminf(dy, 0) - 1)) dy = fmaxf(-1, dy);
    if (course_area_hit(a, COURSE_CAMEDGE, 0, cam->cx + hw + fmaxf(dx, 0) + 1, cam->cy)) dx = fminf(1, dx);
    if (course_area_hit(a, COURSE_CAMEDGE, 0, cam->cx - hw + fminf(dx, 0) - 1, cam->cy)) dx = fmaxf(-1, dx);
    if (fabsf(dx) >= 1) cam->cx += dx;
    if (fabsf(dy) >= 1) cam->cy += dy;
    edge_push(c, cam, hw);
}
void course_camera_step(Camera *cam, const Player *p, const World *w, float view_width)
{
    const CourseRuntime *c = w->course;
    cam->previous_x = cam->x; cam->previous_y = cam->y;
    if (p->source_step) {
        const float mx = course_cx(c, p->x + PLAYER_WIDTH / 2), my = course_cy(c, p->y + player_height(p));
        /* `CamYchange = (80 - CamYchange)/10` (50 on ground) exactly as written;
           where Camera Control Boxes exist they set CamYchange = CamYchangeVar + 50
           and CamXchange = CamXchangeVar every frame instead. */
        float xchange = 0;
        if (c->objects && c->objects->camboxes) {
            cam->ychange = c->objects->cam_y_var + 50;
            xchange = c->objects->cam_x_var;
        } else cam->ychange = ((p->grounded ? 50.0f : 80.0f) - cam->ychange) / 10;
        camera_move(cam, c, (mx - cam->cx - xchange) / 10 + p->vx / V / 4,
                    (my - cam->cy - cam->ychange) / 10 + p->vy / V / 4, view_width);
    }
    publish(cam, c, view_width, p->source_step, source_frac(p));
}
void course_camera_step_to(Camera *cam, const CourseRuntime *c, float tx, float ty, const Player *p, float view_width)
{
    /* _root.Camcenter is another clip: camXspeed = -(Camcenter)/Camspeed, no
       speed terms, the same CamEdge limits. */
    cam->previous_x = cam->x; cam->previous_y = cam->y;
    if (p->source_step) camera_move(cam, c, (tx - cam->cx) / 10, (ty - cam->cy) / 10, view_width);
    publish(cam, c, view_width, p->source_step, source_frac(p));
}
