/* Original tutorial objects. Each behaviour cites the decompiled clip script
   (FFDec export of the pinned SWF) it follows; see docs/course-objects.md.
   Units: course pixels and pixels per 32 Hz frame. */
#include "course_objects.h"
#include <math.h>
#include <string.h>

static unsigned rnd(CourseObjects *o, unsigned n) /* AS2 random(n): 0..n-1 */
{
    o->seed = o->seed * 1103515245u + 12345u;
    return n ? (o->seed >> 16) % n : 0;
}
static float frand(CourseObjects *o) { return rnd(o, 65536) / 65536.0f; } /* Math.random() */
static float drag(float v, float s, float d) { return copysignf(fmaxf(0, fabsf(v) - s) / d, v); }
static bool param_f(const CourseObject *src, const char *k, float *v)
{
    CourseString s;
    if (!course_object_param(src, k, &s) || !s.length || course_string_equals(s, "undefined")) return false;
    char buf[32]; unsigned n = s.length < 31 ? s.length : 31;
    memcpy(buf, s.text, n); buf[n] = 0;
    float sign = 1, val = 0, frac = 0; unsigned i = 0;
    if (buf[0] == '-') { sign = -1; i = 1; }
    for (; buf[i] && buf[i] != '.'; ++i) val = val * 10 + (float)(buf[i] - '0');
    if (buf[i] == '.') for (float f = 0.1f; buf[++i]; f /= 10) frac += (float)(buf[i] - '0') * f;
    *v = sign * (val + frac);
    return true;
}
static LiveKind kind_of(CourseString t)
{
    static const struct { const char *name; LiveKind kind; } table[] = {
        {"Gold Coin Code", LIVE_COIN}, {"Blue Coin Code", LIVE_BLUE_COIN}, {"Coded Sign", LIVE_SIGN},
        {"Butterfly Code", LIVE_BUTTERFLY}, {"Goonie Code", LIVE_GOONIE}, {"Toad Main Clip", LIVE_TOAD},
        {"Falling Log Code (graphics)", LIVE_LOG}, {"YI Arrow Animation", LIVE_ARROW}, {"Goomba", LIVE_GOOMBA},
        {"Cheep Cheep Code", LIVE_CHEEP}, {"Bomb omb Code", LIVE_BOMB}, {"Koopa Code", LIVE_KOOPA},
        {"Breakable Orange Box", LIVE_BOX}, {"Squirt Nozzle Box Clip", LIVE_FLUDD_BOX},
        {"Water Bottle Clip", LIVE_BOTTLE}, {"Green Circle Platform", LIVE_GREEN_PLAT},
        {"Tipping Log", LIVE_TIP_LOG}, {"Rotating Block Plat", LIVE_ROT_BLOCK},
    };
    for (unsigned i = 0; i < sizeof(table) / sizeof(table[0]); ++i)
        if (course_string_equals(t, table[i].name)) return table[i].kind;
    return LIVE_NONE;
}
static bool active_on(const CourseObject *src, unsigned back_frame)
{
    const unsigned frame = course_string_equals(src->layer, "BackGFX") ? back_frame : 0;
    return frame >= src->frame_start && frame < src->frame_end;
}

void course_objects_init(CourseObjects *o, const CourseArea *a, const CoursePickups *taken, unsigned back_frame,
                         unsigned seed)
{
    memset(o, 0, sizeof(*o));
    o->seed = seed ? seed : 1;
    for (unsigned i = 0; i < a->object_count && o->count < COURSE_MAX_LIVE; ++i) {
        const CourseObject *src = &a->objects[i];
        const LiveKind kind = kind_of(src->type);
        if (kind == LIVE_NONE || !active_on(src, back_frame)) continue;
        if ((kind == LIVE_COIN || kind == LIVE_BLUE_COIN) && taken && taken->area == a && i < COURSE_MAX_PICKUPS &&
            taken->taken[i]) continue; /* coindata[coinnum] == 1 */
        LiveObject *l = &o->live[o->count++];
        *l = (LiveObject){.kind = kind, .src = src, .index = i, .x = src->x, .y = src->y, .home_x = src->x,
                          .home_y = src->y, .alive = true, .alpha = 100, .scale = 1, .facing_left = src->a < 0};
        switch (kind) {
        case LIVE_BUTTERFLY: { /* ButterflyCode frame 1 */
            const unsigned diff = rnd(o, 5);
            l->scale = (float)(rnd(o, 40) + 80) / 100;
            l->p0 = diff / 5.0f + 0.4f;           /* maxxspeed */
            l->vx = l->p0;
            l->timer = (int)rnd(o, 25) + 50;      /* waittime */
            l->dir = rnd(o, 2) + 1 == 1 ? 1 : -1; /* movedir */
            break;
        }
        case LIVE_GOONIE: { /* GoonieCode frame 1 */
            float v;
            l->vy = param_f(src, "speedY", &v) ? v : -0.7f - frand(o) * 0.25f;
            l->p0 = param_f(src, "moveup", &v) ? v : 250;
            l->p1 = param_f(src, "movedown", &v) ? v : 45;
            /* _xscale from the load event (instances set +/-100); else the matrix. */
            CourseString xs;
            if (course_object_param(src, "_xscale", &xs)) l->facing_left = xs.length && xs.text[0] == '-';
            const bool right_scale = !l->facing_left; /* _xscale > 0 */
            l->vx = param_f(src, "speedX", &v) ? v : right_scale ? -2 - frand(o) * 0.5f : 2 + frand(o) * 0.5f;
            l->count = (int)rnd(o, (unsigned)l->p0); /* upcount */
            l->frame = rnd(o, 18);                    /* clip.gotoAndPlay(random(18)) */
            l->state = 0;                             /* 0 fly, 1 ride, 2 dive */
            break;
        }
        case LIVE_LOG: { /* Falling Log Code (graphics) */
            float v;
            l->p0 = param_f(src, "fallamount", &v) ? v : 24;
            l->p1 = param_f(src, "fallwait", &v) ? v : 10;
            l->p2 = param_f(src, "quakeamount", &v) ? v : 3;
            l->alpha = 0; l->count = 0;
            break;
        }
        default: break;
        }
    }
}

/* Mario's charClip.body box, approximated from the CharCode collider. */
static void body_box(const Player *p, const CourseRuntime *c, float b[4])
{
    const float fx = course_cx(c, p->x + PLAYER_WIDTH / 2), fy = course_cy(c, p->y + player_height(p));
    const float tall = p->action == ACTION_DIVE ? 25.0f : p->has_fludd ? 40.0f : 34.0f;
    b[0] = fx - 10; b[1] = fy - tall; b[2] = fx + 10; b[3] = fy;
}
static bool overlap(const float a[4], const float b[4]) { return a[0] < b[2] && a[2] > b[0] && a[1] < b[3] && a[3] > b[1]; }
static bool inside(float x, float y, const float b[4]) { return x >= b[0] && x <= b[2] && y >= b[1] && y <= b[3]; }

static void goonie_box(const LiveObject *l, float b[4]) /* GooniePL shape (SWF 53) */
{
    const float x0 = l->facing_left ? -10.5f : -14.5f, x1 = l->facing_left ? 14.5f : 10.5f;
    b[0] = l->x + x0; b[1] = l->y + 3; b[2] = l->x + x1; b[3] = l->y + 15;
}
static void log_box(const LiveObject *l, float b[4]) /* FallinglogCode (SWF 57) */
{
    b[0] = l->x - 8.5f; b[1] = l->y - 6.5f; b[2] = l->x + 8.5f; b[3] = l->y + 10.5f;
}
bool course_objects_platform(const CourseObjects *o, float x, float y)
{
    for (unsigned i = 0; i < o->count; ++i) {
        const LiveObject *l = &o->live[i];
        float b[4];
        if (!l->alive) continue;
        if (l->kind == LIVE_GOONIE) goonie_box(l, b);
        else if (l->kind == LIVE_LOG) log_box(l, b);
        else continue;
        if (inside(x, y, b)) return true;
    }
    return false;
}
const LiveObject *course_objects_near(const CourseObjects *o, LiveKind kind, float fx, float fy)
{
    for (unsigned i = 0; i < o->count; ++i) {
        const LiveObject *l = &o->live[i];
        if (!l->alive || l->kind != kind) continue;
        if (kind == LIVE_SIGN) { /* Coded Sign 'box': (-7.5,-7.7)-(7.5,7.3) */
            const float b[4] = {l->x - 7.5f, l->y - 7.7f, l->x + 7.5f, l->y + 7.3f};
            if (inside(fx, fy, b)) return l;
        } else if (kind == LIVE_TOAD) { /* Toad 'box' 2507 at scale 1.83 x 0.58 */
            const float b[4] = {l->x - 32.0f, l->y - 9.9f, l->x + 32.0f, l->y + 9.9f};
            if (inside(fx, fy - 15, b)) return l;
        }
    }
    return NULL;
}

static void heal(CourseProgress *g) /* DOMDocument.xml:5151 */
{
    while (g->hpcoins >= 5 && g->hp < 8) { g->hpcoins -= 5; ++g->hp; }
    if (g->hpcoins > 5) g->hpcoins = 5;
    while (g->coincount >= 100) g->coincount -= 100; /* CharLives++ (lives not shown yet) */
}

static void step_one(CourseObjects *o, LiveObject *l, CourseRuntime *c, Player *p, CourseProgress *g,
                     CoursePickups *taken, const float body[4], float fx, float fy)
{
    ++l->frame;
    switch (l->kind) {
    case LIVE_COIN: case LIVE_BLUE_COIN: {
        if (l->collected) {
            if (l->frame >= 14) l->alive = false; /* frames 4..17, then removed */
            break;
        }
        const float box[4] = {l->x - 7, l->y - (l->kind == LIVE_COIN ? 9 : 10), l->x + 7, l->y + (l->kind == LIVE_COIN ? 9 : 8)};
        if (overlap(box, body) || inside(fx, fy - 15, box)) {
            l->collected = true; l->frame = 0;
            if (l->kind == LIVE_COIN) { g->coins += 1; g->coincount += 1; g->hpcoins += 1; }
            else { g->coins += 5; g->coincount += 5; g->hpcoins += 15; }
            heal(g);
            if (taken && l->index < COURSE_MAX_PICKUPS) { taken->area = c->area; taken->taken[l->index] = 1; }
        }
        break;
    }
    case LIVE_SIGN: { /* Coded Sign frame 3: aa.aa = 100 while readable, alpha eases 1/3 */
        const float b[4] = {l->x - 7.5f, l->y - 7.7f, l->x + 7.5f, l->y + 7.3f};
        const bool ready = inside(fx, fy, b) && p->action == ACTION_NONE && p->grounded && p->input_horizontal == 0;
        l->alpha += ((ready ? 100.0f : 0.0f) - l->alpha) / 3;
        break;
    }
    case LIVE_TOAD: {
        const bool ready = course_objects_near(o, LIVE_TOAD, fx, fy) == l && p->grounded && p->action == ACTION_NONE;
        l->alpha += ((ready ? 100.0f : 0.0f) - l->alpha) / 3; /* bz prompt */
        break;
    }
    case LIVE_BUTTERFLY: { /* ButterflyCode onEnterFrame */
        l->wave += l->p0 / 5;
        if (++l->count > l->timer) { l->dir = -l->dir; l->count = 0; }
        l->vx += l->dir * 0.1f;
        l->vx = fmaxf(-l->p0, fminf(l->p0, l->vx));
        l->x += l->vx;
        l->facing_left = l->vx < 0;
        break;
    }
    case LIVE_GOONIE: { /* GoonieCode onEnterFrame */
        const unsigned fly_frame = l->frame % 18;
        if (l->state == 0) {
            if (++l->count > (int)l->p0 && (fly_frame == 6 || fly_frame == 14)) { /* clip.wingdive */
                l->state = 2; l->count = 0; l->timer = (int)rnd(o, (unsigned)(l->p1 / 2)); l->frame = 0;
            }
        } else if (l->state == 2) {
            if (++l->timer > (int)l->p1 && l->frame < 1000) l->frame = 1000; /* clip.play() */
            if (l->frame >= 1003) { l->state = 0; l->count = (int)rnd(o, (unsigned)(l->p0 / 2)); l->timer = 0; }
        }
        float sx = l->vx, sy = l->vy;
        if (l->carrying) sy = 1.0f / 1.5f;                   /* ride: gravity/1.5 */
        else if (l->state == 2) { sy = 1.5f; sx = l->vx * 1.5f; } /* dive */
        float b[4]; goonie_box(l, b);
        const bool on = inside(fx, fy, b) && p->vy >= 0;
        if (on) {
            p->x += sx * COURSE_SCALE; p->y += sy * COURSE_SCALE;
            p->vx = drag(p->vx / 44, 0.3f, 1.1f) * 44;
            if (!l->carrying) p->vx -= sx * 44;
            l->carrying = true;
        } else {
            if (l->carrying) { p->vx += sx * 44; p->vy += sy / 2 * 44; }
            l->carrying = false;
        }
        l->x += sx; l->y += sy;
        const CourseMask *edge = course_area_mask(c->area, COURSE_CAMEDGE, 0);
        if (edge) { /* wrap around the CamEdge bounds */
            const float x0 = (float)edge->x0, y0 = (float)edge->y0, w = (float)edge->width, h = (float)edge->height;
            if (l->x > x0 + w + 5) l->x -= w;
            if (l->x < x0 - 5) l->x += w;
            if (l->y > y0 + h + 5) l->y -= h;
            if (l->y < y0 - 5) l->y += h;
        }
        break;
    }
    case LIVE_LOG: { /* Falling Log Code: invisible one-way platform in BPlatforms */
        float b[4]; log_box(l, b);
        if (l->state == 0) {
            l->alpha = fminf(100, l->alpha + 10);
            if (l->alpha >= 100 && inside(fx, fy, b)) { l->state = 1; l->timer = 0; l->count = 0; l->vy = 0; }
        } else {
            ++l->timer;
            if (l->timer > (int)l->p1) {
                ++l->count;
                l->vy = drag(l->vy + 1, 0.1f, 1.05f); /* yspeed += gravity; groundFriction */
            } else {
                const float q = l->p2;
                l->x = l->home_x + (frand(o) * q - frand(o) * q);
                l->y = l->home_y + (frand(o) * q - frand(o) * q);
                if (l->timer == (int)l->p1) { l->x = l->home_x; l->y = l->home_y; }
            }
            if (inside(fx, fy, b)) { p->y += l->vy * COURSE_SCALE; l->carrying = true; }
            else { if (l->carrying) p->vy += l->vy * 44; l->carrying = false; }
            l->y += l->vy;
            if (l->count > (int)l->p0 - 10) l->alpha = fmaxf(0, l->alpha - 10);
            if (l->count > (int)l->p0) { l->state = 0; l->count = 0; l->vy = 0; l->x = l->home_x; l->y = l->home_y; }
        }
        break;
    }
    case LIVE_ARROW: /* YI Arrow: fades and is removed once Platforms leaves frame 1 */
        if (c->platform_frame != 0) { l->alpha -= 10; if (l->alpha <= 0) l->alive = false; }
        break;
    default:
        break;
    }
}

void course_objects_step(CourseObjects *o, CourseRuntime *c, Player *p, CourseProgress *g, CoursePickups *taken,
                         bool frozen)
{
    if (!p->source_step || frozen) return;
    float body[4];
    body_box(p, c, body);
    const float fx = course_cx(c, p->x + PLAYER_WIDTH / 2), fy = course_cy(c, p->y + player_height(p));
    for (unsigned i = 0; i < o->count; ++i)
        if (o->live[i].alive) step_one(o, &o->live[i], c, p, g, taken, body, fx, fy);
}
