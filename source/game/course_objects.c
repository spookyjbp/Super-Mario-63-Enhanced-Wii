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
static void cheep_reset(CourseObjects *o, LiveObject *l);
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
        case LIVE_GOOMBA: { /* GoombaCode frame 1 */
            float v;
            CourseString md;
            if (course_object_param(src, "movedirection", &md)) l->dir = course_string_equals(md, "right") ? 1 : -1;
            else l->dir = rnd(o, 2) == 0 ? 1 : -1;
            l->p0 = param_f(src, "WalkDistance", &v) ? v : 100;
            l->frame_label = GOOMBA_IDLE;
            break;
        }
        case LIVE_CHEEP: { /* CheepCheepCode frame 1 */
            float v;
            l->p0 = param_f(src, "idlespeed", &v) ? v : 2;
            l->p1 = param_f(src, "chasespeed", &v) ? v : 0.5f;
            l->p2 = param_f(src, "turncount", &v) ? v : 96;
            CourseString hg;
            l->p3 = course_object_param(src, "hungry", &hg) && course_string_equals(hg, "false") ? 1 : 0;
            cheep_reset(o, l);
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
/* charClip boxes from Mario Main (canonical facing left; mirrored facing right),
   Char places Main at y -3. body (-10.3,-31)-(17,3.8), feet (-12.3,-14.4)-(15,0.4). */
static void mario_box(const Player *p, const CourseRuntime *c, const float local[4], float b[4])
{
    const float fx = course_cx(c, p->x + PLAYER_WIDTH / 2), fy = course_cy(c, p->y + player_height(p));
    if (p->facing_left) { b[0] = fx + local[0]; b[2] = fx + local[2]; }
    else { b[0] = fx - local[2]; b[2] = fx - local[0]; }
    b[1] = fy + local[1]; b[3] = fy + local[3];
}
static const float MARIO_BODY[4] = {-10.3f, -31.0f, 17.0f, 3.8f};
static const float MARIO_DIVE_BODY[4] = {-10.3f, -19.2f, 17.1f, -4.2f};
static const float MARIO_FEET[4] = {-12.3f, -14.4f, 15.0f, 0.4f};
static bool solid_any(const CourseRuntime *c, float x, float y) { return course_solid(c, x, y) || course_bsolid(c, x, y); }
void course_mario_hit(Player *p, CourseProgress *g, int hp, int blink, float vy, float vx, int stun)
{
    if (p->blink > 0 || g->hp < 1) return; /* _root.MarioHit guard */
    p->blink = blink; g->hp -= hp; g->hpcoins = 0;
    p->action = ACTION_NONE; p->spin_time = 0; p->pounding = false; p->ejump = false;
    /* hurt1: setYspeed/setXspeed replace the velocity */
    p->vx = vx * 44; p->vy = vy * 44; p->grounded = false;
    if (stun > 0) p->stun = stun;
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

static float cam_cx, cam_cy;
static bool jump_key;
static LiveObject *spawn(CourseObjects *o, LiveKind kind, const CourseObject *src, float x, float y)
{
    if (o->count >= COURSE_MAX_LIVE) return NULL;
    LiveObject *l = &o->live[o->count++];
    *l = (LiveObject){.kind = kind, .src = src, .index = COURSE_MAX_PICKUPS, .x = x, .y = y, .home_x = x, .home_y = y,
                      .alive = true, .alpha = 100, .scale = 1};
    return l;
}
/* GoldCoin with bounce = true (DoAction_2): falls, bounces, collectable after
   12 frames, flickers after 288 and vanishes at 384. */
static void step_bounce_coin(LiveObject *l, const CourseRuntime *c)
{
    float b;
    if (course_area_water(c->area, l->x, l->y, &b)) { l->vy += 1.0f / 3; l->vy *= 0.95f; l->vx *= 0.95f; }
    else { l->vy += 1; if (l->vy > 0) l->vy *= 0.97f; }
    l->y += fmaxf(-8, fminf(8, l->vy)); l->x += fmaxf(-7, fminf(7, l->vx));
    if ((l->vy > 0 && course_solid(c, l->x, l->y + 8)) || course_bsolid(c, l->x, l->y + 8))
        { l->vy = drag(-l->vy, 0.5f, 1.2f); l->vx = drag(l->vx, 0.3f, 1.1f); }
    if (l->vx < 0 && course_solid(c, l->x - 7, l->y)) { l->vx = -l->vx * 0.6f; l->vy = drag(l->vy, 0.5f, 1.1f); }
    if (l->vx > 0 && course_solid(c, l->x + 7, l->y)) { l->vx = -l->vx * 0.6f; l->vy = drag(l->vy, 0.5f, 1.1f); }
    if (l->vy < 0 && course_solid(c, l->x, l->y - 8)) { l->vy = drag(-l->vy, 0.5f, 1.5f); l->vx *= 0.95f; }
    for (int n = 0; n < 64 && solid_any(c, l->x, l->y + 8); ++n) l->y -= 1;
    for (int n = 0; n < 64 && course_solid(c, l->x - 7, l->y); ++n) l->x += 1;
    for (int n = 0; n < 64 && course_solid(c, l->x + 7, l->y); ++n) l->x -= 1;
    for (int n = 0; n < 64 && course_solid(c, l->x, l->y - 8); ++n) l->y += 1;
    ++l->count;
    if (l->count > 288) l->alpha = (float)((l->count * 37) % 100); /* _alpha = random(100) */
    if (l->count > 384) l->alive = false;
}
/* Goomba Moveright/Moveleft: probe ahead for ground (water first means no). */
static bool ground_ahead(const CourseRuntime *c, const LiveObject *l, int dir)
{
    for (float xs2 = 15; xs2 < 30; xs2 += 10) {
        const float xs = dir * xs2 + l->vx * 7;
        float adding = 3, b;
        for (float ys = -5; ys < 30;) {
            adding = fminf(adding, 5);
            if (course_solid(c, l->x + xs, l->y + ys) || course_bsolid(c, l->x + xs, l->y + ys)) return true;
            if (course_area_water(c->area, l->x + xs, l->y + ys, &b)) return false;
            adding += 0.5f; ys += adding;
        }
    }
    return false;
}
static void goomba_move(const CourseRuntime *c, LiveObject *l, int dir)
{
    if (ground_ahead(c, l, dir)) l->vx += dir * 0.5f; /* walkspeed */
    else {
        l->count = 0; l->dir = -dir;
        if (!l->air && (dir > 0 || (l->frame_label != GOOMBA_TUMBLE && l->frame_label != GOOMBA_SQUISH)))
            l->frame_label = GOOMBA_IDLE;
    }
}
static void goomba_reset(LiveObject *l)
{
    l->x = l->home_x; l->y = l->home_y; l->vx = l->vy = 0;
    l->timer = 0; l->powerhit = l->seeing = l->see = l->hurt = l->air = l->tumble = l->bounce = false;
    l->tumblerot = 0; l->frame_label = GOOMBA_IDLE; l->frame = 0;
}
/* GoombaCode frame 3 onEnterFrame (DefineSprite_2386, decompiled). */
static void step_goomba(CourseObjects *o, LiveObject *l, CourseRuntime *c, Player *p, CourseProgress *g,
                        CoursePickups *taken)
{
    const bool on_screen = fabsf(cam_cx - l->x) < 600 && fabsf(cam_cy - l->y) < 600;
    if (!on_screen) {
        if (l->frame_label == GOOMBA_SQUISH) ++l->timer;
        if (l->timer > 30 || fabsf(cam_cx - l->home_x) > 600 || fabsf(cam_cy - l->home_y) > 600) goomba_reset(l);
        return;
    }
    if (l->frame_label == GOOMBA_SQUISH) {
        /* Squish clip 2377: hold Mario on top (frames 1-8), bounce at 9, coin at 12. */
        const unsigned f = ++l->count;
        if (l->bounce && f <= 8 && p->ejump) {
            p->vy = 0;
            const float top = l->y - (f < 3 ? 7.0f - 1.5f : 5.5f), fx = course_cx(c, p->x + PLAYER_WIDTH / 2);
            const float fy = course_cy(c, p->y + player_height(p));
            if (fabsf(top - fy) < 15) p->y += (top - fy) / 5 * COURSE_SCALE;
            if (fabsf(l->x - fx) < 15) p->x += (l->x - fx) / (fabsf(p->vx / 44 / 2) + 5) * COURSE_SCALE;
            if (f == 4 || f == 6 || f == 8) if (jump_key && l->p3 != 1) l->p3 = 2; /* landtime 2 */
            if (f == 8) p->y -= 5 * COURSE_SCALE;
        }
        if (l->bounce && f == 9) {
            const float boost = !p->ejump ? 9 : l->p3 == 1 ? 10 : l->p3 == 2 ? 12 : 8;
            p->vy -= boost * 44; p->ejump = false; p->grounded = false;
            p->jump_cut = true; /* the boost is not cut by releasing Jump */
        }
        if (f == 12 && !l->spawned) { /* frame 12: one bouncing coin per Goomba (coindata) */
            l->spawned = true;
            if (!(taken && l->index < COURSE_MAX_PICKUPS && taken->taken[l->index])) {
                if (taken && l->index < COURSE_MAX_PICKUPS) { taken->area = c->area; taken->taken[l->index] = 1; }
                LiveObject *coin = spawn(o, LIVE_COIN, l->src, l->x, l->y - 12);
                if (coin) coin->bounce = true;
            }
        }
        return;
    }
    float body[4] = {l->x - 7.5f, l->y - 7.0f, l->x + 7.5f, l->y + 0.9f};
    const float head[4] = {l->x - 7.5f, l->y - 21.0f, l->x + 7.5f, l->y - 13.0f};
    const float eye[4] = {l->x - 208.8f, l->y - 65.2f, l->x + 209.0f, l->y + 30.1f};
    float mbody[4], mfeet[4];
    mario_box(p, c, p->action == ACTION_DIVE ? MARIO_DIVE_BODY : MARIO_BODY, mbody);
    mario_box(p, c, MARIO_FEET, mfeet);
    const float mx = course_cx(c, p->x + PLAYER_WIDTH / 2), mxs = p->vx / 44;
    if (overlap(eye, mbody) && !l->tumble) { if (!l->seeing) l->see = true; l->seeing = true; }
    else { l->seeing = false; l->see = false; }
    if (overlap(body, mbody) && p->action == ACTION_DIVE && fabsf(mxs) > 1) {
        l->tumble = true; l->vy -= fabsf((mxs + 1) / 5); l->vx += mxs - l->vx / 1.5f; l->y -= 3;
        if (fabsf(l->vx / 0.5f) > 17) l->powerhit = true;
    }
    float spin[4] = {mbody[0] - 18, mbody[1] - 4, mbody[2] + 18, mbody[3] + 4};
    if (p->spin_time > 0 && (overlap(body, spin) || overlap(head, spin))) { /* charClip 'water' during Spin */
        l->tumble = true; l->vy -= 1.5f;
        l->vx *= 0.7f; l->vx += mx > l->x ? -20 : 20;
        if (fabsf(l->vx / 0.5f) > 9) l->powerhit = true;
        l->y -= 3;
    }
    if (overlap(body, mbody) && !(p->action == ACTION_DIVE && fabsf(mxs) > 0.3f) && !l->tumble && p->spin_time == 0)
        course_mario_hit(p, g, 1, 90, -5, mx > l->x ? 7 : -7, 7);
    if (overlap(head, mfeet) && !p->grounded && p->vy > 0) {
        p->vy = 0; l->hurt = true; l->tumblerot = 0; l->bounce = true;
        p->ejump = true; p->action = ACTION_NONE; p->spin_time = 0;
        l->p3 = jump_key ? 1 : 0; /* landtime 1 when Up is held on contact */
    }
    float b;
    if (course_area_water(c->area, l->x, l->y, &b)) { l->hurt = true; l->tumblerot = 0; }
    if (!solid_any(c, l->x, l->y)) { l->y += l->vy; l->vy += 1; l->vy *= 0.98f; l->air = true; }
    if (solid_any(c, l->x, l->y)) {
        if (l->air) l->jump = false;
        l->air = false;
        if (!l->tumble) l->vy = fminf(0, l->vy);
        else {
            if (fabsf(l->vy) + fabsf(l->vx / 2) > 11) { l->tumble = false; if (l->powerhit) { l->tumblerot = 0; l->hurt = true; } }
            if (l->vy > 3) { l->vy = fminf(0, -l->vy + 2); l->vx = drag(l->vx, 2, 1.2f); }
            else { l->vx = drag(l->vx, 0.5f, 1.1f); l->vy = 0; }
            if (fabsf(l->vx) < 2) { l->tumble = false; if (l->powerhit) { l->tumblerot = 0; l->hurt = true; } }
        }
    }
    for (int n = 0; n < 64 && solid_any(c, l->x, l->y - 3); ++n) l->y -= 1;
    for (int n = 0; n < 64 && course_solid(c, l->x + 10, l->y - 12.5f); ++n) {
        l->x -= 1; l->dir = -1; l->count = 0;
        if (l->tumble) { if (l->vx < 7) l->vx = fmaxf(l->vx * 0.7f, -l->vx * 0.7f); else if (l->vx > 0 && l->powerhit) { l->hurt = true; l->tumblerot = -90; } }
    }
    for (int n = 0; n < 64 && course_solid(c, l->x - 10, l->y - 12.5f); ++n) {
        l->x += 1; l->dir = 1; l->count = 0;
        if (l->tumble) { if (l->vx > 7) l->vx = fminf(l->vx * 0.7f, -l->vx * 0.7f); else if (l->vx < 0 && l->powerhit) { l->hurt = true; l->tumblerot = 90; } }
    }
    for (int n = 0; n < 64 && solid_any(c, l->x, l->y + 3) && !(course_solid(c, l->x, l->y - 1) || course_bsolid(c, l->x, l->y - 3)); ++n) l->y += 1;
    for (int n = 0; n < 64 && (course_solid(c, l->x, l->y - 1) || course_bsolid(c, l->x, l->y - 3)); ++n) l->y -= 1;
    if (l->hurt) { l->frame_label = GOOMBA_SQUISH; l->count = 0; l->frame = 0; l->vx = l->vy = 0; return; }
    if (l->air) l->frame_label = GOOMBA_JUMP;
    if (l->seeing && !l->tumble) {
        if (mx > l->x && fabsf(mx - l->x) > 20) goomba_move(c, l, 1);
        else if (mx < l->x && fabsf(mx - l->x) > 20) goomba_move(c, l, -1);
        else if (!l->air && !l->tumble) l->frame_label = GOOMBA_IDLE;
    }
    if (!l->seeing && !l->tumble) {
        ++l->count;
        goomba_move(c, l, l->dir);
        if (l->count > (int)l->p0) { l->dir = -l->dir; l->count = 0; }
    }
    if (l->see) { l->jump = true; l->see = false; }
    if (l->jump && !l->air) { l->vy = -5; l->y -= 3; l->jump = false; l->frame_label = GOOMBA_JUMP; }
    if (!l->air && !l->tumble && fabsf(l->vx) > 0.3f && solid_any(c, l->x, l->y + 3))
        l->frame_label = l->vx > 0 ? GOOMBA_WALKR : GOOMBA_WALKL;
    else if (!l->air && !l->tumble) l->frame_label = GOOMBA_IDLE;
    if (l->tumble) { l->tumblerot += l->vx * 3; l->frame_label = GOOMBA_TUMBLE; }
    if (l->vy > 7) l->vy *= 0.9f;
    l->vy = fmaxf(-10, fminf(10, l->vy));
    l->vx = fmaxf(-15, fminf(15, l->vx));
    l->x += l->vx;
    l->vx = l->tumble ? drag(l->vx, 0.05f, 1.01f) : drag(l->vx, 0.3f, 1.1f);
    if (!l->tumble && fabsf(l->tumblerot) > 0) { l->tumblerot = drag(l->tumblerot, 5, 1.3f); l->frame_label = GOOMBA_TUMBLE; }
}
/* CheepCheepCode HitTest(): the layer at (X, Y), or BPlatforms at the fish
   centre (the original tests _X/_Y there regardless of the arguments). */
static bool cheep_hit(const CourseRuntime *c, const LiveObject *l, bool water, float x, float y)
{
    float b;
    const bool layer = water ? course_area_water(c->area, x, y, &b) : course_solid(c, x, y);
    return layer || course_bsolid(c, l->x, l->y);
}
static void cheep_reset(CourseObjects *o, LiveObject *l)
{
    l->x = l->home_x; l->y = l->home_y; l->vx = l->vy = 0; l->rotation = 0;
    l->timer = 0; l->carrying = false; l->facing_left = true; /* Direction 1, _xscale > 0 */
    l->count = (int)rnd(o, 96) + 1; l->frame_label = CHEEP_SWIM; l->frame = rnd(o, 19);
}
/* CheepCheepCode frame 3 onEnterFrame (DefineSprite_2234, decompiled). */
static void step_cheep(CourseObjects *o, LiveObject *l, CourseRuntime *c, Player *p, CourseProgress *g,
                       CoursePickups *taken)
{
    const float idle = l->p0, chase = l->p1, turncount = l->p2, wide = 9, tall = 8;
    const bool hungry = l->p3 == 0;
    const bool visible = fabsf(cam_cx - l->x) < 600 && fabsf(cam_cy - l->y) < 600;
    const bool far = fabsf(cam_cx - l->home_x) > 600 || fabsf(cam_cy - l->home_y) > 600;
    if (!visible) {
        if (l->frame_label == CHEEP_GONE && far) cheep_reset(o, l);
        return;
    }
    const float mx = course_cx(c, p->x + PLAYER_WIDTH / 2), my = course_cy(c, p->y + player_height(p));
    if (l->frame_label != CHEEP_SPIN && l->frame_label != CHEEP_GONE) {
        const float distance = sqrtf((my - l->y) * (my - l->y) + (mx - l->x) * (mx - l->x));
        if (l->timer > 0) --l->timer; /* stopchasing */
        if (cheep_hit(c, l, true, l->x, l->y)) {
            if (!l->lastwater) l->frame_label = CHEEP_SWIM;
            l->lastwater = true;
            if (!l->carrying) { /* chase == false */
                l->rotation *= 0.7f;
                if (++l->count > turncount) { l->count = 0; l->frame_label = CHEEP_TURN; l->frame = 0; l->facing_left = !l->facing_left; }
                l->vx += ((l->facing_left ? -idle : idle) - l->vx) / 7;
                if (distance < 80 && hungry && l->timer <= 0) l->carrying = true;
            } else {
                l->frame_label = CHEEP_CHASE;
                float rot = atan2f(my - l->y, mx - l->x) * 57.29578f - 180;
                if (mx - l->x > 0) { if (l->facing_left) { l->rotation += 180; l->facing_left = false; } rot += 180; }
                else if (!l->facing_left) { l->rotation += 180; l->facing_left = true; }
                while (rot > 180) rot -= 360;
                while (rot < -180) rot += 360;
                while (l->rotation > 180) l->rotation -= 360; /* Flash wraps _rotation */
                while (l->rotation < -180) l->rotation += 360;
                l->rotation += -(l->rotation - rot) / 3;
                const float r = l->rotation / 57.29578f, k = l->facing_left ? -1.0f : 1.0f;
                l->vx += k * cosf(r) * chase; l->vy += k * sinf(r) * chase;
                if (distance > 100 || l->timer > 0) { l->carrying = false; l->frame_label = CHEEP_SWIM; }
            }
            l->vx *= 0.7f; l->vy *= 0.7f;
        } else {
            if (l->carrying && l->lastwater) { l->vx *= 2; l->rotation = 0; l->vy = -11; l->frame_label = CHEEP_JUMP; l->frame = 0; }
            l->lastwater = false;
            l->vy += 1;
        }
        float mbody[4]; mario_box(p, c, p->action == ACTION_DIVE ? MARIO_DIVE_BODY : MARIO_BODY, mbody);
        const float body[4] = {l->x - 7.5f, l->y - 7.6f, l->x + 7.5f, l->y + 7.4f};
        const bool invincible = p->action == ACTION_DIVE && fabsf(p->vx / 44) > 0.3f;
        const float spin[4] = {mbody[0] - 18, mbody[1] - 4, mbody[2] + 18, mbody[3] + 4};
        if (overlap(body, mbody) && invincible) {
            l->vx += p->vx / 44; l->vy += p->vy / 44 - 5;
            l->frame_label = CHEEP_SPIN; l->p2 = turncount; l->spinrot = fabsf(p->vx / 44); l->count = 0;
        } else if (p->spin_time > 0 && overlap(body, spin)) { /* charClip 'water' during Spin */
            l->frame_label = CHEEP_SPIN; l->spinrot = fabsf(p->vx / 44); l->count = 0;
            l->vy -= 1.5f; l->vx *= 0.7f; l->vx += mx > l->x ? -20 : 20;
        } else if (overlap(body, mbody) && !invincible) {
            l->timer = 32;
            course_mario_hit(p, g, 1, 90, -5, mx > l->x ? 7 : -7, 7);
        }
    } else if (l->frame_label == CHEEP_SPIN) {
        l->vx *= 0.85f; l->vy *= 0.85f;
        l->tumblerot += l->spinrot * 6;
        if (++l->count > 16) { l->frame_label = CHEEP_GONE; l->frame = 0; }
    } else if ((fabsf(cam_cx - l->x) > 600 || fabsf(cam_cy - l->y) > 600) && far) {
        cheep_reset(o, l);
    }
    if (l->frame_label == CHEEP_GONE && l->frame == 4 && !l->spawned) { /* gone clip frame 5: coin */
        l->spawned = true;
        if (!(taken && l->index < COURSE_MAX_PICKUPS && taken->taken[l->index])) {
            if (taken && l->index < COURSE_MAX_PICKUPS) { taken->area = c->area; taken->taken[l->index] = 1; }
            LiveObject *coin = spawn(o, LIVE_COIN, l->src, l->x, l->y - 4);
            if (coin) coin->bounce = true;
        }
    }
    if (cheep_hit(c, l, false, l->x, l->y + tall)) {
        for (int n = 0; n < 64 && cheep_hit(c, l, false, l->x, l->y + tall - 1); ++n) l->y -= 1;
        if (!l->lastwater) {
            l->vy = -4; l->vx *= -0.5f; l->vx += frand(o) * 4 - 2;
            if (l->frame_label != CHEEP_GONE && l->frame_label != CHEEP_SPIN) l->frame_label = CHEEP_SWIM;
            if (l->vx < 0 && !l->facing_left) l->facing_left = true;
            if (l->vx > 0 && l->facing_left) l->facing_left = false;
        } else l->vy = -l->vy * 0.7f;
    }
    if (cheep_hit(c, l, false, l->x, l->y - tall)) {
        for (int n = 0; n < 64 && cheep_hit(c, l, false, l->x, l->y - tall); ++n) l->y += 1;
        l->vy = -l->vy * 0.7f;
    }
    if (cheep_hit(c, l, false, l->x + wide, l->y)) {
        for (int n = 0; n < 64 && cheep_hit(c, l, false, l->x + wide, l->y); ++n) l->x -= 1;
        l->vx = -l->vx * 0.7f;
    }
    if (cheep_hit(c, l, false, l->x - wide, l->y)) {
        for (int n = 0; n < 64 && cheep_hit(c, l, false, l->x - wide, l->y); ++n) l->x += 1;
        l->vx = -l->vx * 0.7f;
    }
    l->x += l->vx; l->y += l->vy;
}
static void step_one(CourseObjects *o, LiveObject *l, CourseRuntime *c, Player *p, CourseProgress *g,
                     CoursePickups *taken, const float body[4], float fx, float fy)
{
    ++l->frame;
    switch (l->kind) {
    case LIVE_COIN: case LIVE_BLUE_COIN: {
        if (l->bounce && !l->collected) {
            step_bounce_coin(l, c);
            if (!l->alive || l->count <= 12) break; /* hitcoin false for 12 frames */
        }
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
            if (!l->bounce && taken && l->index < COURSE_MAX_PICKUPS) { taken->area = c->area; taken->taken[l->index] = 1; }
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
    case LIVE_GOOMBA: step_goomba(o, l, c, p, g, taken); break;
    case LIVE_CHEEP: step_cheep(o, l, c, p, g, taken); break;
    case LIVE_ARROW: /* YI Arrow: fades and is removed once Platforms leaves frame 1 */
        if (c->platform_frame != 0) { l->alpha -= 10; if (l->alpha <= 0) l->alive = false; }
        break;
    default:
        break;
    }
}

void course_objects_step(CourseObjects *o, CourseRuntime *c, Player *p, CourseProgress *g, CoursePickups *taken,
                         bool frozen, float cam_x, float cam_y, bool jump_held)
{
    if (!p->source_step || frozen) return;
    cam_cx = cam_x; cam_cy = cam_y; jump_key = jump_held;
    if (p->blink > 0) --p->blink;
    if (p->stun > 0) --p->stun;
    float body[4];
    body_box(p, c, body);
    const float fx = course_cx(c, p->x + PLAYER_WIDTH / 2), fy = course_cy(c, p->y + player_height(p));
    const unsigned n = o->count; /* objects spawned this frame start next frame */
    for (unsigned i = 0; i < n; ++i)
        if (o->live[i].alive) step_one(o, &o->live[i], c, p, g, taken, body, fx, fy);
}
