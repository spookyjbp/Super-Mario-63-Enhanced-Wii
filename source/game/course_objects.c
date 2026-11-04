/* Original tutorial objects. Each behaviour cites the decompiled clip script
   (FFDec export of the pinned SWF) it follows; see docs/course-objects.md.
   Units: course pixels and pixels per 32 Hz frame. */
#include "course_objects.h"
#include "sfx.h"
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
        {"Camera Control Box", LIVE_CAMBOX},
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
            l->state = (int)rnd(o, 6);            /* frame = random(6) + 1: colour */
            const unsigned diff = rnd(o, 5);
            l->scale = (float)(rnd(o, 40) + 80) / 100;
            l->p0 = diff / 5.0f + 0.4f;           /* maxxspeed */
            l->vx = l->p0;
            l->timer = (int)rnd(o, 25) + 50;      /* waittime */
            l->dir = rnd(o, 2) + 1 == 1 ? 1 : -1; /* movedir */
            break;
        }
        case LIVE_TOAD: { /* Toad Main Clip frame 1: frame ("idle"/"walk"), Mood, _xscale */
            CourseString v;
            l->state = course_object_param(src, "Mood", &v) ? course_toad_mood(v) : TOAD_SMILE; /* StartMood */
            l->eye = course_toad_mood_label(l->state);
            l->p0 = course_object_param(src, "frame", &v) && course_string_equals(v, "walk") ? 1 : 0; /* Startframe */
            if (course_object_param(src, "_xscale", &v)) l->facing_left = v.length && v.text[0] == '-';
            l->timer = (int)rnd(o, 40); /* body 2501 frame 1: gotoAndPlay(random(40) + _currentframe) */
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
        case LIVE_BOMB: { /* BombombCode frame 1 */
            float v;
            CourseString md;
            if (course_object_param(src, "movedirection", &md)) { l->dir = course_string_equals(md, "right") ? 1 : -1; l->count = 0; }
            else { l->count = (int)rnd(o, 2); l->dir = l->count == 0 ? 1 : -1; } /* randomnum = random(2) */
            l->p0 = param_f(src, "WalkDistance", &v) ? v : 100;
            l->p1 = param_f(src, "walkspeed", &v) ? v : 0.5f;
            l->frame_label = BOMB_IDLE; l->state = BOMB_WALKL; /* clip 3310 starts on Walkleft */
            break;
        }
        case LIVE_KOOPA: { /* KoopaCode frame 1 */
            float v;
            CourseString st, md, uw;
            l->timer2 = KOOPA_WALK_STATE;                       /* savestate */
            if (course_object_param(src, "State", &st)) {
                if (course_string_equals(st, "Flying")) l->timer2 = KOOPA_FLYING_STATE;
                else if (course_string_equals(st, "Shell")) l->timer2 = KOOPA_SHELL_STATE;
            }
            l->state = l->timer2;
            l->p1 = param_f(src, "walkspeed", &v) ? v : 3;
            if (course_object_param(src, "movedirection", &md)) { l->dir = course_string_equals(md, "right") ? 1 : -1; l->count = 0; }
            else { l->count = (int)rnd(o, 2); l->dir = l->count == 0 ? 1 : -1; }
            l->p0 = param_f(src, "WalkDistance", &v) ? v : 100;
            l->p3 = course_object_param(src, "underwater", &uw) && course_string_equals(uw, "true") ? 1 : 0;
            l->frame_label = l->state == KOOPA_FLYING_STATE ? KOOPA_FLYING : l->state == KOOPA_SHELL_STATE ? KOOPA_SHELL : KOOPA_WALK;
            l->p2 = 1; /* shell clip 3401 frame */
            break;
        }
        case LIVE_BOX: { /* Breakable Orange Box frame 1 */
            CourseString st;
            static const struct { const char *name; int total, per; } stuff[] = {
                {"1coin", 1, 1}, {"3coins", 3, 1}, {"5coins", 5, 2}, {"10coins", 10, 3},
                {"20coins", 20, 5}, {"30coins", 30, 6}, {"100coins", 100, 10}};
            l->p0 = 3; l->p1 = 1;                           /* stuffinside = "3coins" */
            if (course_object_param(src, "stuffinside", &st)) {
                l->p0 = 0; l->p1 = 0;                       /* other contents: not in the tutorial */
                for (unsigned k = 0; k < sizeof(stuff) / sizeof(stuff[0]); ++k)
                    if (course_string_equals(st, stuff[k].name)) { l->p0 = (float)stuff[k].total; l->p1 = (float)stuff[k].per; }
            }
            CourseString ra;
            l->p2 = course_object_param(src, "reappear", &ra) && course_string_equals(ra, "false") ? 0 : 1;
            l->state = 0;
            break;
        }
        case LIVE_GREEN_PLAT: { /* Green Circle Platform frame 2 */
            float v;
            l->p3 = param_f(src, "scale", &v) ? v : 2;
            l->p0 = param_f(src, "angle", &v) ? v : 0;
            l->p1 = param_f(src, "distance", &v) ? v : 50;
            l->p2 = param_f(src, "speed", &v) ? v : 5;
            l->timer = param_f(src, "dotdistance", &v) ? (int)v : 20;
            const float r = l->p0 * 0.01745329252f;
            l->q0 = -sinf(r) * l->p1; l->q1 = cosf(r) * l->p1; /* pl._x, pl._y */
            l->rotation = l->q0; l->spinrot = l->q1;            /* oldx, oldy */
            l->q2 = l->x; l->q3 = l->y;                         /* GreenPL starts at the centre */
            l->q3 = l->y;
            l->wave = l->q1;                                    /* pl._y as drawn */
            break;
        }
        case LIVE_TIP_LOG: { float v; l->p0 = param_f(src, "scale", &v) ? v : 1; break; }
        case LIVE_ROT_BLOCK: { /* Rotating Block Plat frame 1 */
            float v;
            CourseString rd;
            l->p0 = param_f(src, "rotwait", &v) ? v : 72;
            l->p1 = param_f(src, "rotspeed", &v) ? v : 3;
            l->p2 = param_f(src, "scale", &v) ? v : 80;
            l->p3 = param_f(src, "blocktype", &v) ? v : 1;
            l->dir = course_object_param(src, "rotdirection", &rd) && course_string_equals(rd, "Right") ? 1 : -1;
            break;
        }
        case LIVE_CAMBOX: { /* Camera Control Box */
            float v;
            CourseString t;
            l->dir = course_object_param(src, "t", &t) && course_string_equals(t, "X") ? 1 : 0;
            l->p0 = param_f(src, "amount", &v) ? v : 100;
            if (l->dir == 1) l->p0 = -l->p0;
            l->p1 = param_f(src, "speed", &v) && v >= 1 ? v : 15;
            o->camboxes = true; o->cam_x_var = o->cam_y_var = 0;
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
void course_mario_hit(Player *p, CourseProgress *g, int hp, int blink, float vy, float vx, int stun, bool add)
{
    if (p->blink > 0 || g->hp < 1) return; /* _root.MarioHit guard */
    p->blink = blink; g->hp -= hp; g->hpcoins = 0;
    p->action = ACTION_NONE; p->spin_time = 0; p->pounding = false; p->ejump = false; /* attack = false */
    if (hp > 0) {
        /* a > 0: speed halves and the attack becomes a hurt clip (attack = true). */
        p->vx *= 0.5f; p->vy *= 0.5f;
        p->jump_cut = true; p->dashing = false; p->rocket_boost = 0;
        if (add) {
            /* g: (d, c) added now; hurt3 when the new xspeed points the way Mario
               faces (xspeed * _xscale < 0; _xscale > 0 is facing left), else hurt2. */
            p->vx += vx * 44; p->vy += vy * 44;
            if (p->vx * (p->facing_left ? 1 : -1) < 0) {
                p->action = ACTION_HURT; p->hurt_kind = 3; p->hurt_frame = 0;
            } else {
                p->action = ACTION_BONK; p->bonk_frame = 11; /* hitwall false: gotoAndStop("afterwall") */
            }
        } else {
            /* hurt1: setYspeed = c, setXspeed = d, added by the clip on its frame 4. */
            p->action = ACTION_HURT; p->hurt_kind = 1; p->hurt_frame = 0;
            p->set_vx = vx * 44; p->set_vy = vy * 44;
        }
        p->action_time = 0;
    } else {
        p->vx += vx * 44; p->vy += vy * 44;               /* a shove */
    }
    if (vy < 0) p->grounded = false;
    if (stun > 0) p->stun = stun;
    /* Sound: none for a shove ("none"), "Mario - Ohhhhh" at 0 HP, the Bob-omb's
       "Mario-Squish", otherwise "Mario - Doh" / "Mario - Uh" (random(2)). */
    if (hp > 0 && g->hp <= 0) { sfx_play(SFX_OHHH, 100, 0); p->fainting = true; }
    else if (hp > 0) sfx_play(add ? SFX_SQUISH : sfx_coin_flip() ? SFX_DOH : SFX_UH, 100, 0);
}
static bool overlap(const float a[4], const float b[4]) { return a[0] < b[2] && a[2] > b[0] && a[1] < b[3] && a[3] > b[1]; }
static bool inside(float x, float y, const float b[4]) { return x >= b[0] && x <= b[2] && y >= b[1] && y <= b[3]; }

/* Mario's charClip boxes for this source frame, read by the enemy scripts. */
static struct {
    float body[4], feet[4], spin[4], spray[4];
    bool spin_on, spray_on, dive, facing_left, pound;
    float mx, my, mxs;
    float last_vx, last_vy;  /* Mario's speed (px/frame) on the previous source frame */
} M;
/* Spin Attack clip 1114 'water' (Blank 896 at 1.31 x 0.637, (1, 5)). */
static const float MARIO_SPIN_WATER[4] = {-35.65f, -34.58f, 38.35f, 1.41f};
/* Wait WaterBlast 1157 shapes 1151/1153/1155/1156 reach 12.7/20.7/34.7/43.7 px;
   Mario Main H1-H4 place it rotated 90 degrees at (7.5, -23.05): it points down. */
static const float SPRAY_LENGTH[5] = {0, 12.7f, 20.7f, 34.7f, 43.7f};
static void mario_attacks(const Player *p, const CourseRuntime *c)
{
    mario_box(p, c, p->action == ACTION_DIVE ? MARIO_DIVE_BODY : MARIO_BODY, M.body);
    mario_box(p, c, MARIO_FEET, M.feet);
    M.spin_on = p->spin_time > 0;
    if (M.spin_on) mario_box(p, c, MARIO_SPIN_WATER, M.spin);
    /* charClip.water exists in the plain H frames (wait/walk/jump/fall/swim). */
    M.spray_on = p->has_fludd && p->nozzle == NOZZLE_HOVER && p->blast_level > 0 && p->blast_level <= 4 &&
                 !M.spin_on && p->action == ACTION_NONE && !p->pounding && p->landing_time <= 0 && p->stun <= 0;
    if (M.spray_on) { /* _yscale *= -1 each frame mirrors the stream's width */
        const float spray[4] = {p->blast_parity ? -7.65f : -12.35f, -30.35f, p->blast_parity ? 27.35f : 22.65f,
                                -26.05f + SPRAY_LENGTH[p->blast_level]};
        mario_box(p, c, spray, M.spray);
    }
    M.dive = p->action == ACTION_DIVE;
    M.facing_left = p->facing_left;
    M.mx = course_cx(c, p->x + PLAYER_WIDTH / 2);
    M.my = course_cy(c, p->y + player_height(p));
    M.mxs = p->vx / 44;
    M.pound = p->pounding || p->landing_time > 0; /* attackFrame "Ground" */
}
static bool enemy_hurt_at(const CourseObjects *o, float x, float y)
{
    for (unsigned i = 0; i < o->hurt_count; ++i) {
        const EnemyHurtBox *h = &o->hurt[i];
        if (x >= h->x0 && x <= h->x1 && y >= h->y0 && y <= h->y1) return true;
    }
    return false;
}
/* attachMovie("EnemyHurtBox", {_x, _y, _xscale, _yscale, time}): shape 62 is 100 x 100. */
static void enemy_hurt_box(CourseObjects *o, float x, float y, float xscale, float yscale, int time)
{
    if (o->hurt_count >= COURSE_MAX_HURT) return;
    o->hurt[o->hurt_count++] = (EnemyHurtBox){x - 50 * xscale, y - 50 * yscale, x + 50 * xscale, y + 50 * yscale, 0, time};
}
static void enemy_hurt_age(CourseObjects *o) /* EnemyHurtBox onEnterFrame */
{
    unsigned n = 0;
    for (unsigned i = 0; i < o->hurt_count; ++i) {
        EnemyHurtBox h = o->hurt[i];
        if (h.count >= h.time) continue;
        ++h.count; o->hurt[n++] = h;
    }
    o->hurt_count = n;
}
/* GoombaCode / BombombCode: dive, Enemyhurt and water (Spin or FLUDD) hits.
   The Turbo ram is not reachable in the tutorial (no Turbo nozzle there). */
static void enemy_attacks(CourseObjects *o, LiveObject *l, const float body[4], const float *head, float scale,
                          bool bomb)
{
    const float wide = 10 * scale, tall = 25 * scale;
    if (overlap(body, M.body) && M.dive && fabsf(M.mxs) > 1) {
        l->tumble = true; l->vy -= fabsf((M.mxs + 1) / 5) / scale; l->vx += (M.mxs - l->vx / 1.5f) / scale; l->y -= 3;
        if (fabsf(l->vx / (scale / 2)) > 17) l->powerhit = true;
    }
    const bool hl = enemy_hurt_at(o, l->x - wide, l->y - tall / 2), hr = enemy_hurt_at(o, l->x + wide, l->y - tall / 2);
    if (hl && hr) {
        l->tumble = true; l->vy -= 1.4f / scale; l->vx += (frand(o) * 4 - 2 - l->vx / 1.5f) / scale; l->y -= 3;
        if (fabsf(l->vy / (scale / 2)) > 5) l->powerhit = true;
    } else {
        if (hl) {
            l->tumble = true; l->vy -= 1.4f / scale; l->vx += fmaxf((12 - l->vx / 1.5f) / scale, 0); l->y -= 3;
            if (fabsf(l->vx / (scale / 2)) > 10) l->powerhit = true;
        }
        if (hr) {
            l->tumble = true; l->vy -= 1.4f / scale; l->vx += fminf((-12 - l->vx / 1.5f) / scale, 0); l->y -= 3;
            if (fabsf(l->vx / (scale / 2)) > 10) l->powerhit = true;
        }
    }
    const bool spin = M.spin_on && (overlap(body, M.spin) || (head && overlap(head, M.spin)));
    const bool spray = M.spray_on && (overlap(body, M.spray) || (head && overlap(head, M.spray)));
    if (spin || spray) {
        l->tumble = true;
        if (bomb) l->vy *= 0.9f;
        l->vy -= 1.5f / scale;
        if (spin) { l->vx *= 0.7f; l->vx += (M.mx > l->x ? -20 : 20) / scale; }
        else if (M.facing_left) { l->vx *= 0.9f; l->vx -= 3 / scale; } /* _xscale > 0, Hover */
        else { l->vx *= 0.9f; l->vx += 3 / scale; }
        if (fabsf(l->vx / (scale / 2)) > 9) l->powerhit = true;
        l->y -= 3;
    }
}
/* Eye clip 2385: shape 2381 on frames 1-9, 2382 on 10-12, 2383 on 13-19 (it
   also tilts up to 10 degrees there; not modelled), 2384 while seeing (20). */
static void eye_box(const LiveObject *l, float sx, float dy, float b[4])
{
    static const float E[4][4] = {{-208.95f, -62.25f, 208.95f, 33.05f}, {-287.95f, -74.9f, 287.95f, 34.95f},
                                  {-208.95f, -62.25f, 208.95f, 34.05f}, {-287.95f, -84.5f, 286.0f, 84.5f}};
    const unsigned k = l->eye >= 19 ? 3 : l->eye >= 12 ? 2 : l->eye >= 9 ? 1 : 0;
    b[0] = l->x + 0.1f + E[k][0] * sx; b[2] = l->x + 0.1f + E[k][2] * sx;
    b[1] = l->y + dy + E[k][1]; b[3] = l->y + dy + E[k][3];
}
static void eye_step(LiveObject *l)
{
    if (l->seeing) l->eye = 19;           /* gotoAndStop(20) */
    else if (l->eye >= 19) l->eye = 0;    /* frame 20: gotoAndPlay(1) once unseen */
    else l->eye = (l->eye + 1) % 19;      /* frame 19: gotoAndPlay(1) */
}

static void goonie_box(const LiveObject *l, float b[4]) /* GooniePL shape (SWF 53) */
{
    const float x0 = l->facing_left ? -10.5f : -14.5f, x1 = l->facing_left ? 14.5f : 10.5f;
    b[0] = l->x + x0; b[1] = l->y + 3; b[2] = l->x + x1; b[3] = l->y + 15;
}
static void log_box(const LiveObject *l, float b[4]) /* FallinglogCode (SWF 57) */
{
    b[0] = l->x - 8.5f; b[1] = l->y - 6.5f; b[2] = l->x + 8.5f; b[3] = l->y + 10.5f;
}
/* Point in a scaled, rotated rectangle placed at (ox, oy) (Flash matrix order). */
static bool in_rot_rect(float px, float py, float ox, float oy, float degrees, float sx, float sy, const float r[4])
{
    const float a = degrees * 0.01745329252f, c = cosf(a), sn = sinf(a), dx = px - ox, dy = py - oy;
    const float lx = (dx * c + dy * sn) / sx, ly = (-dx * sn + dy * c) / sy;
    return lx >= r[0] && lx <= r[2] && ly >= r[1] && ly <= r[3];
}
/* GreenPLcode (2103): shape 52 at 1.4 x, (2.8, -9), _xscale 100/150/200 by scale. */
static void green_pl_box(const LiveObject *l, float b[4])
{
    const float k = l->p3 >= 3 ? 2.0f : l->p3 >= 2 ? 1.5f : 1.0f;
    b[0] = l->q2 - 17.5f * k; b[1] = l->q3 - 6; b[2] = l->q2 + 17.5f * k; b[3] = l->q3 + 6;
}
static const float LOG_PL[4] = {-41.0f, -13.5f, 41.0f, -6.5f};       /* LogPL (55), shape 54 */
static const float ORANGE_BLOCK_PL[4] = {-16.0f, -31.0f, 16.0f, -5.0f}; /* OrangeBlockPL (2807) */
static const float ROT_BLOCK_PL[4] = {-50.0f, -50.0f, 50.0f, 50.0f};   /* Rotating Block Plat Code (7527) */
bool course_objects_platform(const CourseObjects *o, float x, float y)
{
    for (unsigned i = 0; i < o->count; ++i) {
        const LiveObject *l = &o->live[i];
        float b[4];
        if (!l->alive) continue;
        if (l->kind == LIVE_GOONIE) goonie_box(l, b);
        else if (l->kind == LIVE_LOG) log_box(l, b);
        else if (l->kind == LIVE_GREEN_PLAT) green_pl_box(l, b);
        else if (l->kind == LIVE_TIP_LOG) {
            if (in_rot_rect(x, y, l->x, l->y, l->rotation, l->p0, 1, LOG_PL)) return true;
            continue;
        } else continue;
        if (inside(x, y, b)) return true;
    }
    return false;
}
bool course_objects_solid(const CourseObjects *o, float x, float y)
{
    for (unsigned i = 0; i < o->count; ++i) {
        const LiveObject *l = &o->live[i];
        if (!l->alive) continue;
        if (l->kind == LIVE_BOX && l->state == 0) {
            if (x >= l->x + ORANGE_BLOCK_PL[0] && x <= l->x + ORANGE_BLOCK_PL[2] &&
                y >= l->y + ORANGE_BLOCK_PL[1] && y <= l->y + ORANGE_BLOCK_PL[3]) return true;
        } else if (l->kind == LIVE_ROT_BLOCK) {
            const float k = l->p2 / 100;
            if (in_rot_rect(x, y, l->x, l->y, l->rotation, k, k, ROT_BLOCK_PL)) return true;
        }
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

static unsigned toad_rand(void *ctx, unsigned n) { return rnd((CourseObjects *)ctx, n); }
int course_toad_mood(CourseString name)
{
    static const char *names[TOAD_MOODS] = {"Smile", "Talk", "Sad", "Mad", "Shocked", "Happy", "Confused", "Crazy"};
    for (int i = 0; i < TOAD_MOODS; ++i)
        if (course_string_equals(name, names[i])) return i;
    return TOAD_SMILE;
}
unsigned course_toad_head_step(unsigned frame, int mood, unsigned (*rand)(void *ctx, unsigned n), void *ctx)
{
    /* Head 2498 scripts: Skip(n) = gotoAndPlay(random(n) + _currentframe) on
       frames 2 (35), 100 (20), 186 (15), 254 (15), 300 (20) and 361 (8); each
       label frame calls ChangeFrame() = gotoAndPlay(Mood). */
    static const struct { unsigned frame, n; } skips[] = {{1, 35}, {99, 20}, {185, 15}, {253, 15}, {299, 20}, {360, 8}};
    const unsigned start = course_toad_mood_label(mood);
    const unsigned end = mood + 1 < TOAD_MOODS ? course_toad_mood_label(mood + 1) : 397;
    if (frame < start || frame >= end) return start;
    ++frame;
    for (unsigned i = 0; i < sizeof(skips) / sizeof(skips[0]); ++i)
        if (frame == skips[i].frame) frame += rand(ctx, skips[i].n);
    return frame >= end ? start : frame;
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
/* attachMovie("GoldCoin", {bounce: true}): GoldCoin frame 1 defaults yspeed
   to -7 and xspeed to Math.random() * 4 - 2 when not given. */
static LiveObject *spawn_coin(CourseObjects *o, const CourseObject *src, float x, float y)
{
    LiveObject *coin = spawn(o, LIVE_COIN, src, x, y);
    if (coin) { coin->bounce = true; coin->vy = -7; coin->vx = frand(o) * 4 - 2; }
    return coin;
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
                spawn_coin(o, l->src, l->x, l->y - 12);
            }
        }
        return;
    }
    const float body[4] = {l->x - 7.5f, l->y - 7.0f, l->x + 7.5f, l->y + 0.9f};
    const float head[4] = {l->x - 7.5f, l->y - 21.0f, l->x + 7.5f, l->y - 13.0f};
    float eye[4];
    eye_box(l, 1, -2.95f, eye);
    const float *mbody = M.body, *mfeet = M.feet;
    const float mx = M.mx, mxs = M.mxs;
    if (overlap(eye, mbody) && !l->tumble) {
        if (!l->seeing) { l->see = true; sfx_at(SFX_GOOMBA_ATTENTION, l->x, l->y, cam_cx, cam_cy, 15); }
        l->seeing = true;
    } else { l->seeing = false; l->see = false; }
    eye_step(l);
    enemy_attacks(o, l, body, head, 1, false);
    if (overlap(body, mbody) && !(M.dive && fabsf(mxs) > 0.3f) && !l->tumble)
        course_mario_hit(p, g, 1, 90, -5, mx > l->x ? 7 : -7, 7, false);
    if (overlap(head, mfeet) && !p->grounded && p->vy > 0) {
        p->vy = 0; l->hurt = true; l->tumblerot = 0; l->bounce = true;
        sfx_at(SFX_GOOMBA_STOMPED, l->x, l->y, cam_cx, cam_cy, 15);
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
/* BombombCode Moveright/Moveleft: probe up to 100 px down for ground; water
   ends one column (lava is not in the tutorial). The step grows by 2 to the
   right but by 0.5 to the left, as in the script. */
static bool bomb_ground_ahead(const CourseRuntime *c, const LiveObject *l, int dir)
{
    for (float xs2 = 15; xs2 < 30; xs2 += 10) {
        const float xs = dir * xs2 + l->vx * 7;
        float adding = 3, b;
        for (float ys = -5; ys < 100;) {
            adding = fminf(adding, 5);
            if (solid_any(c, l->x + xs, l->y + ys)) return true;
            if (course_area_water(c->area, l->x + xs, l->y + ys, &b)) break;
            adding += dir > 0 ? 2 : 0.5f;
            ys += adding;
        }
    }
    return false;
}
static void bomb_move(const CourseRuntime *c, LiveObject *l, int dir)
{
    if (bomb_ground_ahead(c, l, dir)) {     /* AiGround is never set, so only ground counts */
        l->vx += dir * l->p1;
        if (l->exploding) l->vx += dir * l->p1 / 3;
    } else {
        l->count = 0; l->dir = -dir;
        if (dir > 0 && !l->air) l->frame_label = BOMB_IDLE; /* Moveleft's test is always false */
    }
}
static void bomb_reset(LiveObject *l) /* restartvars (coinappear is not reset) */
{
    l->x = l->home_x; l->y = l->home_y;
    l->deadcount = 0; l->powerhit = l->seeing = l->see = l->hurt = false;
    l->frame_label = BOMB_IDLE; l->vx = l->vy = 0; l->air = l->tumble = false;
    l->tumblerot = 0; l->timer2 = 0; l->exploding = false;
}
static bool bomb_boom(int label) { return label == BOMB_EXPLODE || label == BOMB_EXPLODE2; }
/* Explode clip 3294 'explode' (Blank 896): 0.567 x 0.354 at (0, 7.85) on
   frames 1-3, 0.567 x 0.797 at (0, 11.15) on frames 4-6, inside clip.frame at
   (0, -10). explode2 (3309) has no 'explode' child, so it never hurts Mario. */
static bool bomb_explode_box(const LiveObject *l, float b[4])
{
    if (l->state != BOMB_EXPLODE || l->deadcount > 5) return false;
    const bool big = l->deadcount >= 3;
    const float y0 = big ? -33.86f : -12.14f, y1 = big ? 11.17f : 7.86f, x = 16.02f;
    /* clip.frame._rotation = tumblerot (0 or +/-90 from a wall) */
    const float r = l->rotation * 0.01745329252f, cr = cosf(r), sr = sinf(r);
    const float cx[4] = {-x, x, x, -x}, cy[4] = {y0, y0, y1, y1};
    b[0] = b[1] = 1e9f; b[2] = b[3] = -1e9f;
    for (unsigned i = 0; i < 4; ++i) {
        const float px = l->x + cx[i] * cr - cy[i] * sr, py = l->y - 10 + cx[i] * sr + cy[i] * cr;
        b[0] = fminf(b[0], px); b[1] = fminf(b[1], py); b[2] = fmaxf(b[2], px); b[3] = fmaxf(b[3], py);
    }
    return true;
}
/* BombombCode frame 3 onEnterFrame (DefineSprite_3311, decompiled). l->state is
   the label clip 3310 shows, l->rotation its clip.frame._rotation. */
static void step_bomb(CourseObjects *o, LiveObject *l, CourseRuntime *c, Player *p, CourseProgress *g,
                      CoursePickups *taken)
{
    const float scale = 1, wide = 10 * scale, tall = 25 * scale;
    const bool on_screen = fabsf(cam_cx - l->x) < 600 && fabsf(cam_cy - l->y) < 600;
    if (on_screen) {
        if (l->hurt) { l->vx = l->vy = 0; l->tumble = false; l->rotation = l->tumblerot; }
        if (bomb_boom(l->frame_label) && !l->spawned) { /* coinappear: one bouncing coin */
            l->spawned = true;
            if (!(taken && l->index < COURSE_MAX_PICKUPS && taken->taken[l->index])) {
                if (taken && l->index < COURSE_MAX_PICKUPS) { taken->area = c->area; taken->taken[l->index] = 1; }
                spawn_coin(o, l->src, l->x, l->y - 12);
            }
        }
        if (!bomb_boom(l->frame_label)) {
            const float body[4] = {l->x - 7.5f, l->y - 7.03f, l->x + 7.5f, l->y + 0.95f};
            float eye[4];
            eye_box(l, 0.4f / scale, -3.0f, eye); /* eye._xscale = 1 / scale * 40 */
            if (overlap(eye, M.body)) { if (!l->tumble) { if (!l->seeing) l->see = true; l->seeing = true; } }
            else { l->seeing = false; l->see = false; }
            eye_step(l);
            enemy_attacks(o, l, body, NULL, scale, true);
            if (overlap(body, M.body) && !(M.dive && fabsf(M.mxs) > 0.3f) && !l->tumble) { /* a shove */
                if (M.mx > l->x) { course_mario_hit(p, g, 0, 0, 0, 3, 0, false); l->vx -= fmaxf(M.mxs / 3, 0); }
                else { course_mario_hit(p, g, 0, 0, 0, -3, 0, false); l->vx -= fminf(M.mxs / 3, 0); }
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
            for (int n = 0; n < 64 && solid_any(c, l->x, l->y - 3); ++n) {
                l->y -= 1;
                if (l->powerhit && fabsf(l->vx) > 3) { l->hurt = true; l->tumblerot = 0; }
            }
            for (int n = 0; n < 64 && course_solid(c, l->x + wide, l->y - tall / 2); ++n) {
                l->x -= 1; l->dir = -1; l->count = 0;
                if (l->tumble) { if (l->vx < 7) l->vx = fmaxf(l->vx * 0.7f, -l->vx * 0.7f); else if (l->vx > 0 && l->powerhit) { l->hurt = true; l->tumblerot = -90; } }
            }
            for (int n = 0; n < 64 && course_solid(c, l->x - wide, l->y - tall / 2); ++n) {
                l->x += 1; l->dir = 1; l->count = 0;
                if (l->tumble) { if (l->vx > 7) l->vx = fminf(l->vx * 0.7f, -l->vx * 0.7f); else if (l->vx < 0 && l->powerhit) { l->hurt = true; l->tumblerot = 90; } }
            }
            for (int n = 0; n < 64 && solid_any(c, l->x, l->y + 3) && !(course_solid(c, l->x, l->y - 1) || course_bsolid(c, l->x, l->y - 3)); ++n) l->y += 1;
            for (int n = 0; n < 64 && (course_solid(c, l->x, l->y - 1) || course_bsolid(c, l->x, l->y - 3)); ++n) l->y -= 1;
            if (l->hurt) {
                if (!bomb_boom(l->frame_label)) {
                    enemy_hurt_box(o, l->x, l->y - 25, 0.35f, 0.5f, 6);
                    sfx_at(SFX_EXPLOSION, l->x, l->y, cam_cx, cam_cy, 20);
                }
                l->frame_label = l->air ? BOMB_EXPLODE2 : BOMB_EXPLODE;
            }
            const bool boom = bomb_boom(l->frame_label);
            if (l->seeing && !l->tumble && !boom) {
                if (M.mx > l->x && fabsf(M.mx - l->x) > 20) bomb_move(c, l, 1);
                else if (M.mx < l->x && fabsf(M.mx - l->x) > 20) bomb_move(c, l, -1);
            }
            if (!l->seeing && !l->tumble && !bomb_boom(l->frame_label)) {
                ++l->count;
                bomb_move(c, l, l->dir);
                if (l->count > (int)l->p0) { l->dir = -l->dir; l->count = 0; }
            }
            if (l->see) { /* fuse lit: "fuse-lite", then "fuse-loop" on each completion */
                l->see = false;
                if (!l->exploding) sfx_at(SFX_FUSE_LITE, l->x, l->y, cam_cx, cam_cy, 15);
                l->exploding = true;
            }
            const bool ground3 = solid_any(c, l->x, l->y + 3);
            if (!l->air && !bomb_boom(l->frame_label) && !l->tumble && fabsf(l->vx) > 0.3f && ground3)
                l->frame_label = l->vx > 0 ? BOMB_WALKR : BOMB_WALKL;
            /* `air == true && Platforms... || BPlatforms...`; inner test only excludes "explode" */
            if ((l->air && course_solid(c, l->x, l->y + 3)) || course_bsolid(c, l->x, l->y + 3))
                if (l->frame_label != BOMB_EXPLODE && !l->tumble && fabsf(l->vx) > 0.3f && ground3)
                    l->frame_label = l->vx > 0 ? BOMB_WALKR : BOMB_WALKL;
            if (l->tumble && !l->hurt) { l->tumblerot += l->vx * 3; l->frame_label = BOMB_TUMBLE; }
            if (l->vy > 7) l->vy *= 0.9f;
            const bool has_frame = l->state == BOMB_TUMBLE || l->state == BOMB_EXPLODE; /* clip.frame exists */
            if (has_frame && fabsf(l->rotation) > 0 && l->frame_label != BOMB_TUMBLE) {
                l->rotation *= 0.8f; l->tumblerot *= 0.8f;
                if (fabsf(l->rotation) < 10) { l->rotation = 0; l->tumblerot = 0; }
            }
            l->vy = fmaxf(-12, fminf(9, l->vy));
            l->vx = fmaxf(-10, fminf(10, l->vx));
            l->x += l->vx;
            l->vx = l->tumble ? drag(l->vx, 0.05f, 1.01f) : drag(l->vx, 0.3f, 1.1f);
            if (!l->tumble && fabsf(l->tumblerot) > 0) { l->tumblerot = drag(l->tumblerot, 5, 1.3f); l->frame_label = BOMB_TUMBLE; }
            if (l->frame_label != BOMB_TUMBLE) l->tumblerot = 0;
            if (l->exploding) {
                ++l->timer2; /* explodetimer; the red flash is drawn from it */
                /* fuse-lite lasts 13 frames and fuse-loop 37 (at 32 Hz). */
                if (l->timer2 == 13 || l->timer2 == 50 || l->timer2 == 87) sfx_at(SFX_FUSE_LOOP, l->x, l->y, cam_cx, cam_cy, 15);
                if (l->timer2 >= 96) {
                    if (!bomb_boom(l->frame_label)) {
                        enemy_hurt_box(o, l->x, l->y - 15, 0.3f, 0.3f, 6);
                        sfx_at(SFX_EXPLOSION, l->x, l->y, cam_cx, cam_cy, 15);
                    }
                    l->frame_label = l->air ? BOMB_EXPLODE2 : BOMB_EXPLODE;
                }
            }
            if (l->frame_label != BOMB_IDLE) l->state = l->frame_label; /* clip.gotoAndStop(frame) */
            l->rotation = l->tumblerot;                                  /* clip.frame._rotation */
        }
        float eb[4];
        if (bomb_explode_box(l, eb) && overlap(eb, M.body))
            course_mario_hit(p, g, 2, 90, -7, M.mx > l->x ? 5 : -5, 10, true); /* "Mario-Squish" */
    }
    if (l->frame_label != BOMB_IDLE) l->state = l->frame_label;
    if (bomb_boom(l->frame_label)) ++l->deadcount;
    const bool here_far = fabsf(cam_cx - l->x) > 600 || fabsf(cam_cy - l->y) > 600;
    const bool home_far = fabsf(cam_cx - l->home_x) > 600 || fabsf(cam_cy - l->home_y) > 600;
    if ((l->deadcount >= 30 || here_far) && home_far) bomb_reset(l);
}
/* KoopaCode Moveright/Moveleft: three columns 15/25/35 px ahead, 30 px deep;
   each column's first hit decides (water or lava means no ground). */
static void koopa_move(const CourseRuntime *c, LiveObject *l, int dir)
{
    bool offground = true;
    for (float xs2 = 15; xs2 < 40; xs2 += 10) {
        const float xs = dir * xs2 + l->vx * 7;
        float adding = 3, b;
        for (float ys = -5; ys < 30;) {
            adding = fminf(adding, 5);
            if (solid_any(c, l->x + xs, l->y + ys)) { offground = false; break; }
            if (l->p3 == 0 && course_area_water(c->area, l->x + xs, l->y + ys, &b)) { offground = true; break; }
            adding += 0.5f; ys += adding;
        }
    }
    if (!offground) { if (l->frame_label != KOOPA_TURN) l->vx += (dir * l->p1 - l->vx) / 5; }
    else { l->count = 0; if (!l->air && l->frame_label != KOOPA_SHELL) l->frame_label = KOOPA_TURN; }
}
static void koopa_reset(LiveObject *l) /* restartvars */
{
    l->x = l->home_x; l->y = l->home_y; l->state = l->timer2;
    l->vx = l->vy = 0; l->air = false;
    l->frame_label = l->state == KOOPA_FLYING_STATE ? KOOPA_FLYING : l->state == KOOPA_SHELL_STATE ? KOOPA_SHELL : KOOPA_WALK;
}
/* Koopa boxes (Blank 896): body 0.265 at (1, 6.4), head 0.265 x 0.142 at
   (-1, -17); the clip is mirrored while moving right (_xscale = -scale). */
static void koopa_box(const LiveObject *l, bool head, float b[4])
{
    const float x0 = head ? -8.49f : -6.49f, x1 = head ? 6.49f : 8.49f;
    const bool m = l->dir > 0;
    b[0] = l->x + (m ? -x1 : x0); b[2] = l->x + (m ? -x0 : x1);
    b[1] = l->y + (head ? -25.02f : -8.57f); b[3] = l->y + (head ? -17.0f : 6.41f);
}
/* Mario Main 'head' (Blank 896 at 0.354 x 0.212, (1.4, -24.1)). */
static const float MARIO_HEAD[4] = {-8.6f, -39.1f, 11.4f, -27.1f};
/* KoopaCode frame 3 onEnterFrame (DefineSprite_3403, decompiled). */
static void step_koopa(CourseObjects *o, LiveObject *l, CourseRuntime *c, Player *p, CourseProgress *g)
{
    const float scale = 1, wide = 7 * scale, tall = 25 * scale;
    const bool on_screen = fabsf(cam_cx - l->x) < 600 && fabsf(cam_cy - l->y) < 600;
    if (on_screen) {
        if (l->state == KOOPA_WALK_STATE && solid_any(c, l->x, l->y + 3)) {
            koopa_move(c, l, l->dir);
            if (l->frame_label == KOOPA_TURN) l->vx = l->dir > 0 ? -0.8571428571f : 0.8571428571f;
            else ++l->count;
            if (l->count > (int)l->p0) { l->frame_label = KOOPA_TURN; l->count = 0; }
            l->vx *= 0.95f;
        }
        if (l->state == KOOPA_SHELL_STATE) l->vx *= 0.99f;
        float body[4], head[4], mhead[4];
        koopa_box(l, false, body); koopa_box(l, true, head);
        mario_box(p, c, MARIO_HEAD, mhead);
        const bool mariowater = (M.spin_on && overlap(body, M.spin)) ||
                                (M.spray_on && (overlap(body, M.spray) || overlap(head, M.spray)));
        const bool inv = M.dive && fabsf(M.mxs) > 0.3f;
        const bool mair = !p->grounded;
        float mys = p->vy / 44;
        if (l->state == KOOPA_FLYING_STATE && overlap(body, M.body) && inv) {
            l->vx += M.mxs / 1.5f; l->vy += mys / 1.5f; l->state = KOOPA_SHELL_STATE;
        }
        if (l->state == KOOPA_WALK_STATE && overlap(body, M.body) && inv) {
            l->vx += M.mxs / 1.5f; l->vy -= 5; l->y -= 5; l->state = KOOPA_SHELL_STATE;
        }
        if (l->state == KOOPA_SHELL_STATE && overlap(body, M.body) && inv) {
            l->vx += M.mx > l->x ? mys / 2 : -mys / 2;
            l->vx += M.mxs / 1.5f;
        }
        if (mariowater) {
            l->state = KOOPA_SHELL_STATE; l->vy -= 1.5f / scale;
            if (M.spin_on) { l->vx *= 0.7f; l->vx += (M.mx > l->x ? -20 : 20) / scale; }
            else if (M.facing_left) { l->vx *= 0.9f; l->vx -= 3 / scale; }
            else { l->vx *= 0.9f; l->vx += 3 / scale; }
        }
        /* Stomps (+ "flippers" sound). The boost is not cut by releasing Jump. */
        if (l->state == KOOPA_FLYING_STATE && overlap(head, M.feet) && mair && mys > 0) {
            mys = mys / 2 - 15; p->vy = mys * 44; p->jump_cut = true; l->state = KOOPA_WALK_STATE;
            sfx_play(SFX_FLIPPERS, 100, 0);
        }
        if (l->state == KOOPA_WALK_STATE && overlap(head, M.feet) && mair && mys > 0) {
            mys = mys / 2 - 12; p->vy = mys * 44; p->jump_cut = true; l->vx = 0; l->state = KOOPA_SHELL_STATE;
            sfx_play(SFX_FLIPPERS, 100, 0);
        }
        if (l->state == KOOPA_SHELL_STATE && overlap(body, M.feet) && mair && mys > 0) {
            if (l->air) l->vy += (mys - l->vy) * 0.5f;
            mys = mys / 4 - 12; p->vy = mys * 44; p->jump_cut = true;
            sfx_play(SFX_FLIPPERS, 100, 0);
            l->vx += M.mx > l->x ? mys / 1.5f : -mys / 1.5f;
            l->vx += M.mxs / 2;
        }
        if ((l->state == KOOPA_FLYING_STATE || l->state == KOOPA_WALK_STATE) && overlap(body, M.body) && !inv)
            course_mario_hit(p, g, 1, 90, -5, M.mx > l->x ? 7 : -7, 7, false);
        if (l->state == KOOPA_SHELL_STATE && ((overlap(body, M.body) && !mair) || (overlap(body, mhead) && mair)) && !inv) {
            l->vx += (M.mxs - l->vx) / 2;   /* kick */
            l->vx += M.mx > l->x ? -3 : 3;
        }
        if (l->state == KOOPA_FLYING_STATE || l->state == KOOPA_WALK_STATE) {
            const bool hl = enemy_hurt_at(o, l->x - wide, l->y - tall / 2), hr = enemy_hurt_at(o, l->x + wide, l->y - tall / 2);
            if (hl && hr) { l->vy -= 1.4f; l->vx += frand(o) * 4 - 2 - l->vx / 1.5f; l->y -= 3; l->state = KOOPA_SHELL_STATE; }
            else {
                if (hl) { l->vy -= 1.4f; l->vx += fmaxf(5 - l->vx / 1.5f, 0); l->y -= 3; l->state = KOOPA_SHELL_STATE; }
                if (hr) { l->vy -= 1.4f; l->vx += fminf(-5 - l->vx / 1.5f, 0); l->y -= 3; l->state = KOOPA_SHELL_STATE; }
            }
        }
        float b;
        if (l->state == KOOPA_WALK_STATE && course_area_water(c->area, l->x, l->y, &b)) {
            if (l->p3 == 0) l->state = KOOPA_SHELL_STATE; else l->vx *= 0.9f;
        }
        if (l->state == KOOPA_SHELL_STATE) { /* the shell floats */
            int n = 0;
            while (course_area_water(c->area, l->x, l->y - n + 5, &b)) { if (++n > 7) break; }
            if (course_area_water(c->area, l->x, l->y - n + 5, &b)) {
                if (!course_solid(c, l->x, l->y - 8)) { l->vy *= 0.9f; l->vy -= n / 7.0f; }
                if (course_solid(c, l->x, l->y + 3)) l->y -= 5;
                l->vx *= 0.85f;
            }
        }
        if (l->state == KOOPA_WALK_STATE || l->state == KOOPA_SHELL_STATE) {
            if (solid_any(c, l->x, l->y)) l->vy = 0;
            else { l->vy += 1; if (l->vy > 7) l->vy *= 0.9f; }
        }
        for (int n = 0; n < 64 && course_solid(c, l->x + wide, l->y - tall / 2); ++n) {
            l->x -= 1;
            if (l->state == KOOPA_WALK_STATE && l->dir > 0) l->frame_label = KOOPA_TURN;
            if (l->state == KOOPA_SHELL_STATE) l->vx = fminf(-l->vx * 0.7f, l->vx);
        }
        for (int n = 0; n < 64 && course_solid(c, l->x - wide, l->y - tall / 2); ++n) {
            l->x += 1;
            if (l->state == KOOPA_WALK_STATE && l->dir < 0) l->frame_label = KOOPA_TURN;
            if (l->state == KOOPA_SHELL_STATE) l->vx = fmaxf(-l->vx * 0.7f, l->vx);
        }
        if (l->state != KOOPA_FLYING_STATE)
            for (int n = 0; n < 64 && solid_any(c, l->x, l->y + 3) && !solid_any(c, l->x, l->y - 1); ++n) l->y += 1;
        for (int n = 0; n < 64 && solid_any(c, l->x, l->y - 1); ++n) l->y -= 1;
        const float ceiling = l->state != KOOPA_SHELL_STATE ? 30.0f : 20.0f;
        for (int n = 0; n < 64 && course_solid(c, l->x, l->y - ceiling); ++n) l->y += 1;
        if (l->state == KOOPA_FLYING_STATE) l->frame_label = KOOPA_FLYING;
        if (l->state == KOOPA_WALK_STATE && l->frame_label != KOOPA_TURN) l->frame_label = KOOPA_WALK;
        if (l->state == KOOPA_SHELL_STATE) {
            l->frame_label = KOOPA_SHELL;
            if (fabsf(l->vx) > 1) enemy_hurt_box(o, l->x, l->y - 7, 0.15f, 0.15f, 1);
        }
        l->vx = fmaxf(-12, fminf(12, l->vx));
        l->y += l->vy; l->x += l->vx;
        /* clip.gotoAndStop(frame): the Turn clip (3377) restarts on entry and its
           frame 15 flips movedirection and returns to Walk. */
        if (l->frame_label == KOOPA_TURN) {
            if (!l->carrying) { l->carrying = true; l->timer = 0; }
            else if (++l->timer >= 14) { l->frame_label = KOOPA_WALK; l->dir = -l->dir; l->carrying = false; }
        } else l->carrying = false;
        if (l->frame_label == KOOPA_SHELL) { /* shell clip 3401: frame += |xspeed|, wraps at 20 */
            l->p2 += fabsf(l->vx);
            while (l->p2 > 20) l->p2 -= 20;
            l->p2 = fmaxf(l->p2, 1);
        }
    }
    const bool here_far = fabsf(cam_cx - l->x) > 600 || fabsf(cam_cy - l->y) > 600;
    const bool home_far = fabsf(cam_cx - l->home_x) > 600 || fabsf(cam_cy - l->home_y) > 600;
    if (here_far && home_far) koopa_reset(l);
}
/* Breakable Orange Box (2808): frame 3 waits with OrangeBlockPL in Platforms;
   a ground pound (box3), a hard run/dive (box4, HitWall), Spin water or an
   Enemyhurt blast breaks it (frames 4-25); frame 5 drops its coins once
   (coindata); it reforms when the camera and Mario are both 600 px away.
   (Its hurt2 trigger, `box2` while Mario is knocked into it, is not modelled.) */
static void step_box(CourseObjects *o, LiveObject *l, CourseRuntime *c, Player *p, CoursePickups *taken)
{
    if (l->state == 0) {
        if (!(fabsf(cam_cx - l->x) < 600 && fabsf(cam_cy - l->y) < 600)) return;
        const float box[4] = {l->x - 24, l->y - 18.02f, l->x + 24, l->y - 0.98f};
        const float box3[4] = {l->x - 21, l->y - 39.52f, l->x + 21, l->y - 19.48f};
        const float box4[4] = {l->x - 36, l->y - 22.52f, l->x + 36, l->y - 2.48f};
        /* The port lands Mario before objects run, so the pound's fall speed is
           the one from the previous source frame. */
        const float fall = fmaxf(p->vy / 44, M.last_vy);
        bool brk = false;
        if (overlap(box3, M.feet) && fall > 3 && M.pound && !p->submerged) {
            p->vy = (fall * 0.75f - 3) * 44; p->grounded = false; brk = true;
        }
        if (overlap(box4, M.body) && fabsf(M.last_vx) > 9) { /* _root.HitWall() */
            p->action = ACTION_BONK; p->action_time = 0; p->bonk_direction = M.last_vx > 0 ? -1 : 1; p->bonk_frame = 0;
            p->vx = p->vy = 0; p->spin_time = 0; p->pounding = false;
            p->facing_left = !(M.mx < l->x);
            brk = true;
        }
        if ((M.spin_on && overlap(box, M.spin)) || enemy_hurt_at(o, l->x, l->y - 5) || enemy_hurt_at(o, l->x + 10, l->y - 5) ||
            enemy_hurt_at(o, l->x - 10, l->y - 5) || enemy_hurt_at(o, l->x, l->y - 30)) brk = true;
        if (brk) { l->state = 1; l->timer = 0; l->count = 0; sfx_play(SFX_BOXBREAK, 100, 0); } /* frame 4 */
        return;
    }
    if (l->state == 1) { /* frames 4..25 (index 3..24) */
        ++l->timer;
        if (l->timer == 1) { /* frame 5: coindata, then createCoins(total, per frame) */
            const bool had = taken && l->index < COURSE_MAX_PICKUPS && taken->taken[l->index];
            if (!had && taken && l->index < COURSE_MAX_PICKUPS) { taken->area = c->area; taken->taken[l->index] = 1; }
            l->count = had ? (int)l->p0 : 0;
            if (!had && l->p0 == 1) { spawn_coin(o, l->src, l->x - 5, l->y - 12); l->count = 1; }
        } else if (l->count < (int)l->p0) {
            for (int i = 0; i < (int)l->p1 && l->count < (int)l->p0; ++i, ++l->count) {
                LiveObject *coin = spawn_coin(o, l->src, l->x - 5, l->y - 12);
                if (!coin) break;
                if (l->p0 == 5) { coin->vy = frand(o) * 5 - 7; coin->vx = frand(o) * 10 - 5; }
                else if (l->p0 == 10) { coin->vy = frand(o) * 6 - 10; coin->vx = frand(o) * 12 - 6; }
                else if (l->p0 == 20) { coin->vy = frand(o) * 7 - 15; coin->vx = frand(o) * 14 - 7; }
                else if (l->p0 == 30) { coin->vy = frand(o) * 10 - 15; coin->vx = frand(o) * 16 - 8; }
                else if (l->p0 == 100) { coin->vy = frand(o) * 15 - 18; coin->vx = frand(o) * 18 - 9; }
            }
        }
        if (3 + l->timer >= 24) l->state = 2;
        return;
    }
    const bool cam_far = fabsf(cam_cx - l->x) > 600 || fabsf(cam_cy - l->y) > 600;
    const bool mario_far = fabsf(M.mx - l->x) > 600 || fabsf(M.my - l->y) > 600;
    if (cam_far && mario_far && l->p2 != 0) { l->state = 0; l->timer = 0; } /* gotoAndStop(3) */
}
/* Squirt Nozzle Box (HoverFluddBox 972): landing on its top (Blank 839 at
   0.604 x, (0, -14)) bounces Mario (yspeed -6) and breaks it (frames 2-12),
   releasing the Hover nozzle; it reforms 300 frames later. */
static void step_fludd_box(CourseObjects *o, LiveObject *l, Player *p, float fx, float fy)
{
    if (l->state == 0) {
        const float b[4] = {l->x - 16.0f, l->y - 22.0f, l->x + 16.0f, l->y - 6.0f};
        if (inside(fx, fy, b) && p->vy > 0) {
            p->vy = -6 * 44; p->grounded = false; p->jump_cut = true;
            l->state = 1; l->timer = 1;
            LiveObject *n = spawn(o, LIVE_NOZZLE, l->src, l->x, l->y + 10); /* attachMovie("Hnozzle") */
            if (n) n->vy = -3;
            sfx_play(SFX_NOZZLE_BOX, 100, 0);
        }
    } else if (l->state == 1) {
        if (++l->timer >= 11) { l->state = 2; l->count = 0; }
    } else if (++l->count > 300) { l->state = 0; l->timer = 0; }
}
/* Hnozzle (42): falls to the ground; touching it gives FLUDD Hover with a full tank. */
static void step_nozzle(LiveObject *l, const CourseRuntime *c, Player *p, float fx, float fy)
{
    const float b[4] = {l->x - 16.0f, l->y - 21.95f, l->x + 16.0f, l->y + 21.05f};
    if (inside(fx, fy, b)) { /* Fluddpow = "H"; WaterAmount = TotalWater */
        p->has_fludd = true; p->nozzle = NOZZLE_HOVER; p->water = 100; l->alive = false;
        /* The original tutorial only has Hover; the port has always let every
           nozzle be switched to once FLUDD is worn, and keeps that rule. */
        p->nozzles = (1u << NOZZLE_COUNT) - 1;
        sfx_play(SFX_FLUDD_NOZZLE, 100, 0);
        return;
    }
    if (!solid_any(c, l->x, l->y + 7)) { l->y += l->vy; l->vy += 0.5f; l->vy *= 0.95f; }
    for (int n = 0; n < 64 && solid_any(c, l->x, l->y + 5); ++n) { l->y -= 1; l->vy = fminf(0, l->vy); }
    ++l->count;
    if (l->count > 900) l->alpha = (float)((l->count * 37) % 100); /* _alpha = random(100) */
    if (l->count > 1000) l->alive = false;
}
/* Water Bottle (3995): frames 1-3 appear; on frame 3 touching it adds 5000 of
   TotalWater 10000 (half a tank); frames 4-18 play; it comes back when off
   camera or after 960 frames. */
static void step_bottle(LiveObject *l, Player *p, float fx, float fy)
{
    const bool on_screen = fabsf(cam_cx - l->x) < 600 && fabsf(cam_cy - l->y) < 600;
    if (l->state == 0) { if (++l->timer >= 2) l->state = 1; return; }
    if (l->state == 1) {
        l->alpha = on_screen ? 100 : 0;  /* _visible */
        const float b[4] = {l->x - 7.9f, l->y - 4.9f, l->x + 7.85f, l->y + 23.65f};
        if (on_screen && (overlap(b, M.body) || inside(fx, fy - 15, b))) {
            p->water = fminf(100, p->water + 50); l->state = 2; l->timer = 3; l->alpha = 100;
            sfx_play(SFX_FLUDD_NOZZLE, 100, 0); /* frame 4 */
        }
        return;
    }
    if (l->state == 2) { if (++l->timer >= 17) { l->state = 3; l->count = 0; } return; }
    if (!on_screen || l->count++ > 960) { l->state = 0; l->timer = 0; }
}
/* Green Circle Platform (3315 frame 2): `pl` circles the centre; GreenPLcode
   (2103) in BPlatforms follows it and carries Mario by the step (diffx, diffy). */
static void step_green_plat(LiveObject *l, Player *p, float fx, float fy)
{
    l->p0 += l->p2;                                     /* angle += speed */
    const float r = l->p0 * 0.01745329252f;
    const float nx = -sinf(r) * l->p1, ny = cosf(r) * l->p1;
    l->vx = nx - l->rotation; l->vy = ny - l->spinrot;  /* diffx, diffy */
    l->wave = l->spinrot;                               /* pl._y drawn one step behind */
    l->rotation = nx; l->spinrot = ny;
    l->q0 = nx; l->q1 = ny;
    if (fabsf(cam_cx - l->x) < 600 + l->p1 * 2 && fabsf(cam_cy - l->y) < 600 + l->p1 * 2) {
        float b[4];
        green_pl_box(l, b);
        if (inside(fx, fy + l->vy + 4, b) && p->vy / 44 > -3) {
            p->x += l->vx * COURSE_SCALE; p->y += l->vy * COURSE_SCALE;
            p->vx = drag(p->vx / 44, 0.3f, 1.1f) * 44;
            if (!l->carrying) p->vx -= l->vx / 2 * 44;
            l->carrying = true;
        } else {
            if (l->carrying) { p->vx += l->vx * 44; p->vy += l->vy * 44; }
            l->carrying = false;
        }
        l->q2 = l->x + nx; l->q3 = l->y + ny;
    }
}
/* Tipping Log (3476) with LogPL (55) in BPlatforms: Mario's weight tips it. */
static void step_tip_log(LiveObject *l, Player *p, float fx, float fy)
{
    if (!(fabsf(cam_cx - l->x) < 600 && fabsf(cam_cy - l->y) < 600)) return;
    const float k = l->p0, w = (k - 1) / 2 + 1;
    l->vx = drag(l->vx, 0.15f, 1.05f);                  /* rotspeed */
    const float distance = (fx - l->x) / k;
    if (in_rot_rect(fx, fy, l->x, l->y, l->rotation, k, 1, LOG_PL)) {
        l->carrying = true;                             /* land */
        sfx_set_step(SFX_STEP_WETWOOD, 3);              /* changestep = 3; StepSound = "step-wetwood" */
        p->x += l->rotation / 7 * COURSE_SCALE;
        l->vx += distance / 5 / w;
        l->vx -= l->rotation - drag(l->rotation, 0.3f / k, 0.07f / k + 1);
    } else {
        if (l->carrying) {
            p->vx += (l->rotation / 5 - p->vx / 44) / 3 * 44;
            if (p->vy / 44 < -3) l->vx += distance * 2 / w;
        }
        l->carrying = false;
        l->vx -= l->rotation - drag(l->rotation, 1 / k, 0.12f / k + 1);
    }
    l->rotation += l->vx / 20;
    if (l->rotation >= 80 || l->rotation <= -80) l->vx *= -0.7f;
    l->rotation = fmaxf(-80, fminf(80, l->rotation));
}
/* Rotating Block Plat (3063) with its Platforms code clip (7527): turns 90
   degrees every rotwait frames, quaking for the last 20. (The tutorial's
   blocks have no speedX/speedY, so the travel code is not modelled.) */
static void step_rot_block(CourseObjects *o, LiveObject *l, Player *p, float fx, float fy)
{
    const float k = l->p2 / 100;
    ++l->count;
    if (l->count > (int)l->p0) {
        ++l->timer;                                     /* spincount */
        if (l->timer * l->p1 <= 90) {
            l->rotation += l->dir * l->p1;
            if (in_rot_rect(fx, fy, l->x, l->y, l->rotation, k, k, ROT_BLOCK_PL)) p->x += l->dir * l->p1 * COURSE_SCALE;
        } else { l->count = 0; l->timer = 0; l->rotation = roundf(l->rotation / 90) * 90; }
    }
    if (l->count + 20 > (int)l->p0 && l->count < (int)l->p0)
        l->rotation = roundf(l->rotation / 90) * 90 + (frand(o) * 5 - 2.5f);
    if (fabsf(cam_cx - l->x) < 600 && fabsf(cam_cy - l->y) < 600 &&
        in_rot_rect(fx, fy + 1, l->x, l->y, l->rotation, k, k, ROT_BLOCK_PL) && p->grounded)
        p->vx = drag(p->vx / 44, 0.15f, 1.05f) * 44;   /* standing on it */
}
/* Camera Control Box: eases its count toward |amount| while Mario's body
   touches it and adds the change to _root.CamXchangeVar / CamYchangeVar. */
static void step_cambox(CourseObjects *o, LiveObject *l)
{
    float b[4];
    course_object_bounds(l->src, -50.025f, -50.0f, 49.975f, 50.0f, b); /* shape: XFL edges in twips */
    const float last = l->q0;
    if (overlap(b, M.body)) l->q0 += (fabsf(l->p0) - l->q0) / l->p1;
    else l->q0 -= l->q0 / l->p1;
    float *var = l->dir == 1 ? &o->cam_x_var : &o->cam_y_var;
    if (l->p0 > 0) *var += l->q0 - last;
    else if (l->p0 < 0) *var -= l->q0 - last;
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
    /* The turn clip (2220) frame 4 sets _parent._parent.frame = "swim". */
    if (l->frame_label == CHEEP_TURN && l->frame >= 3) l->frame_label = CHEEP_SWIM;
    const int label_before = l->frame_label;
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
        const float *mbody = M.body;
        const float body[4] = {l->x - 7.5f, l->y - 7.6f, l->x + 7.5f, l->y + 7.4f};
        const bool invincible = M.dive && fabsf(M.mxs) > 0.3f;
        const bool mariowater = (M.spin_on && overlap(body, M.spin)) || (M.spray_on && overlap(body, M.spray));
        const float spinspeed = fabsf(M.mxs);
        if (overlap(body, mbody) && invincible) {
            l->vx += M.mxs; l->vy += p->vy / 44 - 5;
            l->frame_label = CHEEP_SPIN; l->spinrot = spinspeed; l->count = 0;
        }
        const bool hl = enemy_hurt_at(o, l->x - wide, l->y - tall / 2), hr = enemy_hurt_at(o, l->x + wide, l->y - tall / 2);
        if (hl && hr) {
            l->vy -= 1.4f; l->vx += frand(o) * 4 - 2 - l->vx / 1.5f;
            l->frame_label = CHEEP_SPIN; l->spinrot = spinspeed; l->count = 0;
        } else {
            if (hl) { l->vy -= 1.4f; l->vx += fmaxf(5 - l->vx / 1.5f, 0); l->frame_label = CHEEP_SPIN; l->spinrot = spinspeed; l->count = 0; }
            if (hr) { l->vy -= 1.4f; l->vx += fminf(-5 - l->vx / 1.5f, 0); l->frame_label = CHEEP_SPIN; l->spinrot = spinspeed; l->count = 0; }
            if (mariowater) {
                l->frame_label = CHEEP_SPIN; l->spinrot = spinspeed; l->count = 0;
                l->vy -= 1.5f;
                if (M.spin_on) { l->vx *= 0.7f; l->vx += mx > l->x ? -20 : 20; }
                else if (M.facing_left) { l->vx *= 0.9f; l->vx -= 3; }
                else { l->vx *= 0.9f; l->vx += 3; }
            }
        }
        if (overlap(body, mbody) && !invincible && l->frame_label != CHEEP_SPIN && l->frame_label != CHEEP_GONE) {
            l->timer = 32;
            course_mario_hit(p, g, 1, 90, -5, mx > l->x ? 7 : -7, 7, false);
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
            spawn_coin(o, l->src, l->x, l->y - 4);
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
    /* clip.gotoAndStop(frame): a new label starts its clip from frame 1. */
    if (l->frame_label != label_before) l->frame = 0;
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
            sfx_play(SFX_COIN, 100, 0); /* GoldCoin frame 4: "CoinJingle" */
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
        /* body: idle 2501 (120 frames; frames 1 and 60 gotoAndPlay(random(40) +
           _currentframe)) or walk 2516 (20 frames); 'timer' is its frame. */
        if (l->p0 > 0) l->timer = (l->timer + 1) % 20;
        else {
            if (++l->timer >= 120) l->timer = 0;
            if (l->timer == 0 || l->timer == 59) l->timer += (int)rnd(o, 40);
            if (l->timer >= 120) l->timer -= 120;
        }
        l->eye = course_toad_head_step(l->eye, l->state, toad_rand, o);
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
    case LIVE_BOMB: step_bomb(o, l, c, p, g, taken); break;
    case LIVE_KOOPA: step_koopa(o, l, c, p, g); break;
    case LIVE_BOX: step_box(o, l, c, p, taken); break;
    case LIVE_FLUDD_BOX: step_fludd_box(o, l, p, fx, fy); break;
    case LIVE_NOZZLE: step_nozzle(l, c, p, fx, fy); break;
    case LIVE_BOTTLE: step_bottle(l, p, fx, fy); break;
    case LIVE_GREEN_PLAT: step_green_plat(l, p, fx, fy); break;
    case LIVE_TIP_LOG: step_tip_log(l, p, fx, fy); break;
    case LIVE_ROT_BLOCK: step_rot_block(o, l, p, fx, fy); break;
    case LIVE_CAMBOX: step_cambox(o, l); break;
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
    mario_attacks(p, c);
    const float fx = course_cx(c, p->x + PLAYER_WIDTH / 2), fy = course_cy(c, p->y + player_height(p));
    const unsigned n = o->count; /* objects spawned this frame start next frame */
    for (unsigned i = 0; i < n; ++i)
        if (o->live[i].alive) step_one(o, &o->live[i], c, p, g, taken, body, fx, fy);
    enemy_hurt_age(o);
    M.last_vx = p->vx / 44; M.last_vy = p->vy / 44;
}
