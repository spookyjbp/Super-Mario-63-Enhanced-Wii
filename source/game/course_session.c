#include "course_session.h"
#include "bg_water.h"
#include "bg_water_places.h"
#include "water_places.h"
#include "sfx.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

/* `_root.changecourse(transition, area, b, c, d, e)` targets from each area's
   BackGFX frame-3 script: StartChar puts Char at local (d, e). */
/* wipe: the "<dir>transition" attached on touching the warp and the
   "<dir>transition2" changecourse plays in the new area (WIPE_*). */
typedef struct { const char *from, *warp, *to; float x, y; unsigned char wipe; } WarpRule;
static const WarpRule warps[] = {
    {"8-E1-1", "warp1", "8-E1-1-2", 0, 0, WIPE_RIGHT},        /* 8-E1-1 Back GFX frame 3 */
    {"8-E1-1-2", "warp1", "8-E1-1", 1760, -20, WIPE_LEFT},    /* 8-E1-1-2 Back GFX frame 3 */
    {"8-E1-1-2", "warp2", "8-E1-2", 0, 0, WIPE_RIGHT},
    {"8-E1-2", "warp1", "8-E1-1-2", 1730, -30, WIPE_LEFT},    /* 8-E1-2 Back GFX frame 3 */
    {"8-E1-2", "warp2", "8-E1-2-2", 0, 0, WIPE_RIGHT},
    {"8-E1-2-2", "warp1", "8-E1-2", 1840, -170, WIPE_LEFT},   /* 8-E1-2-2 BackGFX frame 3 */
    /* falling off the cloud: downtransition, back to 8-E1-2 above where Mario fell,
       X = 2000 + (Char._x - CamEdge._x) within 400..1800, Y = -450 */
    {"8-E1-2-2", "warp3", "8-E1-2", NAN, -450, WIPE_DOWN},
    /* doorplace "C-Opening" once Star[39] is set: changecourse("righttransition2", "Castle", -1300, 100) */
    {"8-E1-2-2", "C-Opening", "Castle", -1300, 100, WIPE_RIGHT},
    /* CastleBackGFX frame 3: warp1 (with Star[39]) lefttransition to 8-E1-2-2 (964, -783);
       warp2 (to K-1) is not in the port: closed */
    {"Castle", "warp1", "8-E1-2-2", 964, -783, WIPE_LEFT},
};
/* Coded Sign "box" (SWF 986 first shape) and Invisible Door (SWF 2256). */
static const float sign_box[4] = {-7.5f, -7.7f, 7.5f, 7.3f};
static const float door_box[4] = {-9.0f, 8.0f, 9.0f, 18.2f};

static float feet_x(const CourseSession *s) {return course_cx(&s->runtime, s->game.player.x + PLAYER_WIDTH / 2);}
static float feet_y(const CourseSession *s) {return course_cy(&s->runtime, s->game.player.y + player_height(&s->game.player));}
static bool inside(const float b[4], float x, float y) {return x >= b[0] && x <= b[2] && y >= b[1] && y <= b[3];}
static bool param(const CourseObject *o, const char *k, const char *v)
{ CourseString t; return course_object_param(o, k, &t) && course_string_equals(t, v); }

bool course_session_object_active(const CourseSession *s, const CourseObject *o)
{
    const unsigned frame = course_string_equals(o->layer, "BackGFX") ? s->back_frame : 0;
    return frame >= o->frame_start && frame < o->frame_end;
}
/* The level water's simWave, one source frame: only within 1000 px (vertically)
   of Mario; a strip without `graphics = true` is stirred when Mario crosses its
   surface (within 20 px) faster than 5 px a frame up or down. The water
   blocks' clips play on. */
static void level_water_step(CourseSession *s)
{
    s->water_frame = (s->water_frame + 1) % WATER_BLOCK_FRAMES;
    const Player *p = &s->game.player;
    const float mx = course_cx(&s->runtime, p->x + PLAYER_WIDTH / 2), my = course_cy(&s->runtime, p->y + player_height(p));
    const float xs = p->vx / 44, ys = p->vy / 44;
    for (unsigned i = 0; i < s->water_count; ++i) {
        const WaterPlace *w = &water_places[s->water_index[i]];
        if (!(fabsf(my - w->y) < 1000)) continue;
        const BgWaveSplash m = {mx - w->x, xs, ys, w->length};
        const bool stir = w->splash && fabsf(ys) > 5 && fabsf(my - w->y) < 20;
        bg_wave_step_splash(&s->water_wave[i], w->stiffness, w->randomwave, stir ? &m : 0);
    }
}
static void enter_area(CourseSession *s, const CourseArea *a, float cx, float cy)
{
    course_runtime_init(&s->runtime, a);
    s->world = course_world(&s->runtime);
    /* Dynamic Water in the backgrounds (bg_water.h): makeWave on attach */
    s->bg_water_count = 0;
    for (unsigned i = 0; i < BG_WATERS && s->bg_water_count < 4; ++i)
        if (course_string_equals(a->id, bg_waters[i].area)) {
            bg_wave_init(&s->bg_wave[s->bg_water_count], bg_waters[i].points, bg_waters[i].length, 63u + i);
            s->bg_water_index[s->bg_water_count++] = i;
        }
    s->fx_count = 0;   /* FrontGFX's effects go with the area */
    /* The level water: makeWave when the Water clip is attached */
    s->water_count = 0; s->water_frame = 0;
    for (unsigned i = 0; i < WATER_PLACES && s->water_count < 6; ++i)
        if (course_string_equals(a->id, water_places[i].area)) {
            bg_wave_init(&s->water_wave[s->water_count], water_places[i].points, water_places[i].length, 163u + i);
            s->water_index[s->water_count++] = i;
        }
    /* Falling out respawns at the area's entry (StartX/StartY in StartChar). */
    s->world.spawn_x = course_wx(&s->runtime, cx) - PLAYER_WIDTH / 2;
    s->world.spawn_y = course_wy(&s->runtime, cy) - PLAYER_HEIGHT;
    s->back_frame = 2;
    const bool keep_fludd = s->game.player.has_fludd;
    const unsigned keep_nozzles = s->game.player.nozzles;
    const Nozzle keep_nozzle = s->game.player.nozzle;
    game_init(&s->game, &s->world, s->view_width);
    s->game.player.has_fludd = keep_fludd; s->game.player.nozzles = keep_nozzles; s->game.player.nozzle = keep_nozzle;
    course_place_player(&s->runtime, &s->game.player, cx, cy);
    camera_init(&s->game.camera, &s->game.player, &s->world, s->view_width);
    s->state = SESSION_PLAY;
    s->hint = (CourseString){0};
    CoursePickups *taken = NULL;
    for (unsigned i = 0; i < s->pack->area_count; ++i) if (&s->pack->areas[i] == a) taken = &s->pickups[i];
    course_objects_init(&s->objects, a, taken, s->back_frame, 0x5363u + (unsigned)a->object_count);
    s->runtime.objects = &s->objects;
    s->camera_override = false;
    /* 8-E1-1 Ground: barrier frame while TotalStars == 0 and Char is left of the start sign. */
    s->runtime.platform_frame = 1;
    /* CastlePlatforms: frame 1 (stop()), frame 3 once TotalStars >= 63 */
    if (course_string_equals(a->id, "Castle")) s->runtime.platform_frame = s->total_stars >= 63 ? 2 : 0;
    memset(&s->sling, 0, sizeof s->sling);
    s->closed_note = 0;
    if (course_string_equals(a->id, "Castle")) {
        /* CastleBackGFX frame 1: AreaTextClipF("Castle Grounds:\r<i>Outside the Castle</i>", 35) */
        s->area_title = (CourseString){"Castle Grounds:\nOutside the Castle", 34}; s->area_text = 1;
        s->area_text_kind = 1;
    }
    if (course_string_equals(a->id, "8-E1-1")) {
        /* 8-E1-1BackGFX frame 3: AreaTextClipF("Mushroom Kingdom\r<i>(Tutorial)</i>", 45) */
        s->area_title = (CourseString){"Mushroom Kingdom\n(Tutorial)", 27}; s->area_text = 1;
        s->area_text_kind = 0;
        for (unsigned i = 0; i < a->object_count; ++i)
            if (param(&a->objects[i], "signmessage", "Start") && s->total_stars == 0 && cx < a->objects[i].x + 5) {
                s->runtime.platform_frame = 0;
                /* Start sign onClipEvent: `_root.Course.Char._y = 60` then the walk-in. */
                course_place_player(&s->runtime, &s->game.player, cx, 60);
                s->state = SESSION_OPENING; s->opening_wait = s->opening_count = 0;
            }
    }
}
bool course_session_start(CourseSession *s, const CoursePack *pack, const char *id, float cx, float cy, float view_width)
{
    const CourseArea *a = course_pack_area(pack, id);
    if (!a) return false;
    memset(s, 0, sizeof(*s));
    pause_init(&s->pause, sfx_music_muted());
    s->pack = pack; s->view_width = view_width; s->warp_target = -1;
    s->zoom = 1; s->camera_zoom = s->zoom2 = s->zoom2_from = 100; s->talk_zoomed = false;
    course_camera_set_zoom(1);
    s->last_hurt_frame = ~0u;
    s->game.player.has_fludd = false; s->game.player.nozzles = 0; /* StartChar(..., Fluddpow "") */
    s->progress.hp = 8;                 /* _root.CharHP = 8 */
    s->progress.water_hp = 8;           /* _root.WaterHP = 8 */
    hud_init(&s->hud, s->progress.hp);
    /* StartChar(..., resetmusic = true): _root["8-E1-1MusicIntro"], then "8-E1-1Music". */
    if (course_string_equals(course_pack_area(pack, id)->id, "8-E1-1") || course_string_equals(course_pack_area(pack, id)->id, "8-E1-2-2"))
        sfx_music(MUS_CASTLE_INTRO, MUS_CASTLE_REPEAT);
    /* the Castle has no CastleMusic, and its BackGFX frame 1 calls StopBGsong() */
    if (course_string_equals(course_pack_area(pack, id)->id, "Castle")) sfx_music(-1, -1);
    enter_area(s, a, cx, cy);
    return true;
}
static void open_sign(CourseSession *s, const CourseObject *o)
{
    s->sign = o; s->state = SESSION_SIGN; s->sign_phase = 0; s->sign_frames = 0;
    s->game.player.reading_sign = true; /* attack = true; attackFrame = "Sign" */
    s->sign_picture = param(o, "picture", "true");
    course_object_param(o, "signmessage", &s->sign_text);
    Player *p = &s->game.player;
    p->vx = p->vy = 0; p->action = ACTION_NONE; p->dashing = false;
    /* othercamcenter: the Start sign looks at the controls sign (sign2), the
       goomba/bomb/stomp signs at their targetN clip. */
    CourseString target;
    /* Coded Sign frame 3: _root.Camcenter = this (the sign), unless othercamcenter
       (frame 10) or the Start sign's sign2. */
    s->camera_override = true; s->camera_target_x = o->x; s->camera_target_y = o->y;
    if (course_object_param(o, "othercamcenter", &target) || param(o, "signmessage", "Start")) {
        const CourseArea *a = s->runtime.area;
        const char *want = param(o, "signmessage", "Start") ? "sign2" : NULL;
        bool found = false;
        for (unsigned i = 0; i < a->object_count && !found; ++i) {
            const CourseObject *t = &a->objects[i];
            const bool match = want ? course_string_equals(t->name, want)
                                    : (t->name.length && target.length > t->name.length &&
                                       !memcmp(target.text + target.length - t->name.length, t->name.text, t->name.length));
            if (match) {
                found = true;
                s->camera_target_x = t->x; s->camera_target_y = t->y - (want ? 60 : 0);
            }
        }
    }
}
static void close_sign(CourseSession *s)
{
    CourseString done;
    if (course_object_param(s->sign, "completefunction", &done) &&
        course_string_equals(done, "_root.Course.Platforms.gotoAndStop(2);"))
        s->runtime.platform_frame = 1;
    s->sign = NULL; s->state = SESSION_PLAY; s->camera_override = false;
    s->game.player.reading_sign = false; /* frame 16: attack = false */
}
static bool star39(const CourseSession *s);
static void update_hint(CourseSession *s)
{
    const CourseArea *a = s->runtime.area;
    /* charClip.body: roughly the middle of Mario. */
    const float bx = feet_x(s), by = feet_y(s) - 17;
    s->hint = (CourseString){0};
    for (unsigned i = 0; i < a->object_count; ++i) {
        const CourseObject *o = &a->objects[i];
        CourseString mess;
        if (!course_string_equals(o->type, "Blank Movieclip") || !course_object_param(o, "mess", &mess) ||
            !course_session_object_active(s, o)) continue;
        /* "No Mario! Go Left and read that Sign!" only while the barrier stands. */
        if (mess.length > 9 && !memcmp(mess.text, "<b>No Mario", 11) && s->runtime.platform_frame != 0) continue;
        /* the Castle's door hint: only while TotalStars <= 0 and Star[39] is false */
        if (course_string_equals(a->id, "Castle") && (s->total_stars > 0 || star39(s))) continue;
        float b[4];
        course_object_bounds(o, COURSE_BLANK_U0, COURSE_BLANK_V0, COURSE_BLANK_U1, COURSE_BLANK_V1, b);
        if (inside(b, bx, by)) s->hint = mess;
    }
    if (s->closed_note) s->hint = (CourseString){"(Closed for now: not in this port yet)", 38};
}
static bool star39(const CourseSession *s) { return s->ach && (s->ach->p.shines >> 38 & 1); }
static int warp_named(const char *from, const char *name)
{
    for (unsigned w = 0; w < sizeof(warps) / sizeof(warps[0]); ++w)
        if (!strcmp(warps[w].from, from) && !strcmp(warps[w].warp, name)) return (int)w;
    return -1;
}
static void start_warp(CourseSession *s, int w, float fx)
{
    if (w < 0) return;
    s->state = SESSION_WARP; s->warp_target = w;
    s->wipe = warps[w].wipe; s->wipe_in = 1; s->wipe_out = 0;
    s->warp_x = isnan(warps[w].x) ? fminf(1800, fmaxf(400, 2000 + fx)) : warps[w].x;
    s->game.player.vx = s->game.player.vy = 0;
}
/* ---- The Castle's Sling Star (2162; Castle BackGFX frame 1: otherstar
   "ss1", xblast 0, yblast -27) and Mario's SlingStar animation (1476) ----
   The star waits 32 frames (frame 3), then takes Mario when his body is in
   its box in the air, not rising faster than 7 px a frame, with no attack or
   a spin or dive: CameraZoom 50, Mario eased to its middle
   (`Char._x += (X - Char._x) / 5`), held (1476 frames 1-4, then frame 5
   waiting). Jump or spin there (KeySPACE / KeySPIN: speed += 15, speed *=
   0.8) with speed over 10 within 25 frames spins the star (its frames 4-15:
   CameraZoom 75, 50 on 10) and flings Mario on its frame 16 (yspeed =
   yblast, xspeed = xblast; 1476 frames 21-28 take half the gravity away;
   attack until its frame 46). Otherwise after 32 frames (34 with Down
   held twice as fast) 1476 drops him (frame 11: attack = false). The port
   shows Mario's spin pose while he is in the star. */
static float string_float(CourseString v)
{
    char buf[32];
    const size_t n = v.length < sizeof buf - 1 ? v.length : sizeof buf - 1;
    memcpy(buf, v.text, n); buf[n] = 0;
    return strtof(buf, NULL);
}
#define SLING_BOX_U0 -14.44f   /* 'box' (839 at 0.56 x 1.25, (0.4, 0.1)) */
#define SLING_BOX_V0 -9.9f
#define SLING_BOX_U1 15.24f
#define SLING_BOX_V1 10.1f
static void sling_input(CourseSession *s, GameInput *in)
{
    Player *p = &s->game.player;
    if (s->sling.state == SLING_HOLD || s->sling.state == SLING_SPIN) {
        const GameInput keep = *in;
        *in = (GameInput){0};
        in->jump_held = keep.jump_held; in->spin_held = keep.spin_held; in->crouch_held = keep.crouch_held;
        in->jump_pressed = in->spin_pressed = false;
        p->spin_time = 0.05f;          /* the spin pose, no gravity */
        in->jump_held = false;
        s->sling.idle = keep.jump_held || keep.spin_held ? 1u : 0u;   /* KeySPACE() || KeySPIN() this frame */
        if (keep.crouch_held) s->sling.idle |= 2u;                    /* KeyDOWN() */
    } else if (s->sling.state == SLING_FLING) {
        in->horizontal = 0; in->jump_pressed = in->spin_pressed = in->pound_pressed = false;   /* attack */
        if (s->sling.frame >= 21 && s->sling.frame <= 28) p->vy -= 800.0f * GAME_DT;           /* yspeed -= gravity / 2 */
    }
}
static void sling_frame(CourseSession *s, GameInput in)
{
    (void)in;
    Player *p = &s->game.player;
    const CourseArea *a = s->runtime.area;
    if (!course_string_equals(a->id, "Castle")) { s->sling.state = SLING_NONE; return; }
    if (s->sling.state == SLING_NONE) {
        if (s->sling.count < 33) { ++s->sling.count; return; }   /* the star's frame 3 count */
        const float bx = feet_x(s), by = feet_y(s) - 17;
        const bool attack_ok = p->action == ACTION_NONE || p->action == ACTION_DIVE || p->spin_time > 0;
        if (p->grounded || !(p->vy / 44 > -7) || !attack_ok) return;
        for (unsigned i = 0; i < a->object_count; ++i) {
            const CourseObject *o = &a->objects[i];
            if (!course_string_equals(o->type, "Sling Star BackGFX") || !course_session_object_active(s, o)) continue;
            float b[4];
            course_object_bounds(o, SLING_BOX_U0, SLING_BOX_V0, SLING_BOX_U1, SLING_BOX_V1, b);
            if (!inside(b, bx, by)) continue;
            s->sling = (__typeof__(s->sling)){o, SLING_HOLD, 1, 0, 0, true, 0};
            s->camera_zoom = 50;                               /* savecamzoom; CameraZoom = 50 */
            p->action = ACTION_NONE; p->dashing = false;
            return;
        }
        return;
    }
    const CourseObject *o = s->sling.star;
    const float fx = feet_x(s), fy = feet_y(s);
    if (s->sling.state == SLING_HOLD || s->sling.state == SLING_SPIN) {   /* Char eased to the star */
        p->x += (o->x - fx) / 5 * COURSE_SCALE;
        p->y += (o->y - fy) / 5 * COURSE_SCALE;
    }
    if (s->sling.state == SLING_HOLD) {
        /* 1476: frames 1-4 play, frame 5 waits counting */
        if (s->sling.frame < 5) { ++s->sling.frame; return; }
        if (s->sling.go) {
            if (s->sling.idle & 1u) { s->sling.speed += 15; s->sling.speed *= 0.8f; }
            if (s->sling.idle & 2u) s->sling.count += 2;
            if (++s->sling.count > 32) s->sling.go = false;
        }
        if (fabsf(s->sling.speed) > 10 && s->sling.count <= 25) {   /* the star plays on: frame 4 */
            s->sling.state = SLING_SPIN; s->sling.frame = 4;
            s->camera_zoom = 75;
            return;
        }
        if (!s->sling.go) {   /* 1476 frames 6-11: frame 11, attack = false */
            if (++s->sling.frame >= 11) { s->sling.state = SLING_NONE; s->sling.count = 0; s->camera_zoom = 100; }
        }
        return;
    }
    if (s->sling.state == SLING_SPIN) {
        if (++s->sling.frame == 10) s->camera_zoom = 50;
        if (s->sling.frame >= 16) {   /* fling */
            CourseString v;
            float xb = 0, yb = -27;
            if (course_object_param(o, "xblast", &v)) xb = string_float(v);
            if (course_object_param(o, "yblast", &v)) yb = string_float(v);
            s->sling.state = SLING_FLING; s->sling.frame = 20;     /* 1476 "fling" */
            p->vy = yb * 44; p->vx = xb * 44; p->spin_time = 0; p->grounded = false;
            p->jump_cut = true;      /* yspeed is set: no short-hop cut of it */
        }
        return;
    }
    /* SLING_FLING: the star plays to its frame 25 (CameraZoom back) and 30, 1476 to 46 */
    ++s->sling.frame;
    if (s->sling.frame == 29) s->camera_zoom = 100;
    if (s->sling.frame >= 46 || p->grounded) { s->sling.state = SLING_NONE; s->sling.count = 0; }
}
/* Sound FX Clip (the Castle's waterfall): sound "waterfall", range 15/100,
   maxvolume 100: distancevolume = 100 - max(100 - maxvolume, min(100,
   distance from the camera * range)); panX = clamp((camX - x) / -2, +/-100);
   it loops while distancevolume >= 1. */
static void ambient_step(CourseSession *s)
{
    const CourseArea *a = s->runtime.area;
    SfxId id = SFX_COUNT;
    float best = 0, pan = 0;
    for (unsigned i = 0; i < a->object_count; ++i) {
        const CourseObject *o = &a->objects[i];
        if (!course_string_equals(o->type, "Sound FX Clip") || !course_session_object_active(s, o)) continue;
        float d = hypotf(s->game.camera.cx - o->x, s->game.camera.cy - o->y) * 0.15f;
        d = fmaxf(0, fminf(100, d));
        const float vol = 100 - d;
        if (vol >= 1 && vol > best) {
            best = vol; id = SFX_WATERFALL;
            pan = fmaxf(-100, fminf(100, (s->game.camera.cx - o->x) / -2));
        }
    }
    sfx_set_ambient(id, best, pan);
}
static void check_exits(CourseSession *s, GameInput in)
{
    const CourseArea *a = s->runtime.area;
    const float fx = feet_x(s), fy = feet_y(s);
    for (unsigned i = 0; i < a->object_count; ++i) {
        const CourseObject *o = &a->objects[i];
        if (!course_session_object_active(s, o)) continue;
        float b[4];
        if (course_string_equals(o->type, "Blank Movieclip") && o->name.length) {
            for (unsigned w = 0; w < sizeof(warps) / sizeof(warps[0]); ++w) {
                if (!course_string_equals(a->id, warps[w].from) || !course_string_equals(o->name, warps[w].warp)) continue;
                course_object_bounds(o, COURSE_BLANK_U0, COURSE_BLANK_V0, COURSE_BLANK_U1, COURSE_BLANK_V1, b);
                if (inside(b, fx, fy) && (!course_string_equals(a->id, "Castle") || star39(s))) start_warp(s, (int)w, fx);
            }
        }
        if (course_string_equals(o->type, "Invisible Door") && param(o, "doorplace", "C-Opening")) {
            const float sx = o->a;
            const float box[4] = {o->x + door_box[0] * sx, o->y + door_box[1], o->x + door_box[2] * sx, o->y + door_box[3]};
            if (inside(box, fx, fy) && in.crouch_held && s->game.player.grounded && star39(s)) {
                start_warp(s, warp_named("8-E1-2-2", "C-Opening"), fx);
            } else if (inside(box, fx, fy) && in.crouch_held && s->game.player.grounded) {
                /* doorplace "C-Opening" with Star[39] false: bgsong.stop() and
                   gotoAndStop("Intro2"): main.c plays Intro Movie 2 (cinema.h) */
                s->state = SESSION_COMPLETE;
                s->game.player.vx = s->game.player.vy = 0;
                /* Intro Movie 2 (1503) ends with _root.Star[39] = true; SaveFile() */
                if (s->ach) ach_shine(s->ach, 39);
                s->save_request = true;
            }
        }
    }
}
/* The Castle Grounds' doors (Up, which there opens the door instead of
   jumping) and warp pipes (Down) lead to courses the port does not have yet:
   closed (user decision 2026-11-08), with a note. Before the move. */
static void closed_doors(CourseSession *s, GameInput *in)
{
    const CourseArea *a = s->runtime.area;
    if (!course_string_equals(a->id, "Castle") || !s->game.player.grounded) return;
    const float fx = feet_x(s), fy = feet_y(s);
    for (unsigned i = 0; i < a->object_count; ++i) {
        const CourseObject *o = &a->objects[i];
        if (!course_session_object_active(s, o)) continue;
        const bool pipe = course_string_equals(o->type, "Invisible Door") && param(o, "walk", "warppipe");
        const bool door = course_string_equals(o->type, "Main Door Clip");
        if (!pipe && !door) continue;
        float b[4];
        if (door) course_object_bounds(o, -8, -15.5f, 8, 15.5f, b);
        else { b[0] = o->x + door_box[0] * o->a; b[1] = o->y + door_box[1];
               b[2] = o->x + door_box[2] * o->a; b[3] = o->y + door_box[3];
               if (b[0] > b[2]) { const float t = b[0]; b[0] = b[2]; b[2] = t; } }
        if (!inside(b, fx, door ? fy - 10 : fy)) continue;
        if (door && in->vertical < -0.5f) {
            s->closed_note = 64;
            in->vertical = 0; in->jump_pressed = in->jump_held = false;
        } else if (pipe && in->crouch_held) {
            s->closed_note = 64;
        }
    }
}
static void try_read_sign(CourseSession *s, GameInput in, bool z_pressed)
{
    const Player *p = &s->game.player;
    if (!z_pressed || !p->grounded || p->action != ACTION_NONE || in.horizontal != 0) return;
    const CourseArea *a = s->runtime.area;
    const float fx = feet_x(s), fy = feet_y(s);
    for (unsigned i = 0; i < a->object_count; ++i) {
        const CourseObject *o = &a->objects[i];
        if (!course_string_equals(o->type, "Coded Sign") || !course_session_object_active(s, o)) continue;
        const float b[4] = {o->x + sign_box[0], o->y + sign_box[1], o->x + sign_box[2], o->y + sign_box[3]};
        if (inside(b, fx, fy)) {open_sign(s, o); return;}
    }
}
static CoursePickups *pickups_for(CourseSession *s)
{
    for (unsigned i = 0; i < s->pack->area_count; ++i) if (&s->pack->areas[i] == s->runtime.area) return &s->pickups[i];
    return NULL;
}
/* Toad Main Clip: pages M1..M6; a page whose MnTxt is undefined ends the talk. */
static bool toad_page(CourseSession *s, unsigned page)
{
    char key[8] = {'M', (char)('0' + page), 'T', 'x', 't', 0};
    CourseString t;
    if (page > 9 || !course_object_param(s->toad->src, key, &t) || course_string_equals(t, "undefined")) return false;
    s->toad_text = t; s->toad_chars = 0;
    key[2] = 'M'; key[3] = 'o'; key[4] = 'o';
    if (!course_object_param(s->toad->src, key, &s->toad_mood)) s->toad_mood = (CourseString){0};
    /* Mood = M<n>Moo: the head plays that mood from its label (ChangeFrame). */
    LiveObject *toad = (LiveObject *)s->toad; /* points into s->objects */
    if (s->toad_mood.length && !course_string_equals(s->toad_mood, "undefined")) {
        const int mood = course_toad_mood(s->toad_mood);
        if (mood != toad->state) { toad->state = mood; toad->eye = course_toad_mood_label(mood); }
    }
    return true;
}
/* Stats.tm's onEnterFrame, once a source frame (z_held: KeyZ()). The cloud
   fades in; only once it is above 98 does the page type, two characters a
   frame, all at once while Z is held; then its Z prompt fades in. */
static void tm_step(CourseSession *s, bool z_held)
{
    const float aa = s->tm_message ? 100 : 0;
    const float bz_aa = s->tm_message && s->toad_chars >= s->toad_text.length ? 100 : 0;
    s->tm_alpha += (aa - s->tm_alpha) / 4;
    s->tm_bz += (bz_aa - s->tm_bz) / 2;
    if (s->tm_alpha > 98 && s->tm_message) {
        s->toad_chars = s->toad_chars + 2 < s->toad_text.length ? s->toad_chars + 2 : s->toad_text.length;
        if (z_held) s->toad_chars = s->toad_text.length;
    }
}
/* Toad's MnEnd (BP(): Z pressed) only counts once tm shows the whole page and its prompt is at 98. */
static bool toad_page_done(const CourseSession *s)
{
    return s->tm_message && s->toad_chars >= s->toad_text.length && s->tm_bz >= 98;
}
/* The conversation ended: Mood = StartMood, frame = Startframe. */
static void toad_rest(CourseSession *s)
{
    LiveObject *toad = (LiveObject *)s->toad;
    CourseString v;
    const int mood = course_object_param(toad->src, "Mood", &v) ? course_toad_mood(v) : TOAD_SMILE;
    if (mood != toad->state) { toad->state = mood; toad->eye = course_toad_mood_label(mood); }
}
static void try_talk(CourseSession *s, bool z_pressed)
{
    const Player *p = &s->game.player;
    if (!z_pressed || !p->grounded || p->action != ACTION_NONE) return;
    const LiveObject *t = course_objects_near(&s->objects, LIVE_TOAD, course_cx(&s->runtime, p->x + PLAYER_WIDTH / 2),
                                              course_cy(&s->runtime, p->y + player_height(p)));
    if (!t) return;
    s->toad = t; s->toad_page = 1;
    if (!toad_page(s, 1)) return;
    s->tm_message = true;
    s->state = SESSION_TALK;
    s->camera_override = true; s->camera_target_x = t->x; s->camera_target_y = t->y; /* Camcenter = this */
    Player *pm = &s->game.player;
    pm->vx = pm->vy = 0;
    /* Phase 1: Toad and Mario face each other (_xscale = +/-100 for both). */
    const bool mario_right = course_cx(&s->runtime, pm->x + PLAYER_WIDTH / 2) > t->x;
    pm->facing_left = mario_right;
    ((LiveObject *)t)->facing_left = !mario_right;
}
void course_session_zoom_step(CourseSession *s, float axis, bool talking, bool source_step, float frac)
{
    if (s->camera_zoom <= 0) { s->camera_zoom = s->zoom2 = s->zoom2_from = 100; s->zoom = 1; }
    if (source_step) {
        s->zoom2_from = s->zoom2;
        /* KeyPlus / KeyMinus: CameraZoom += / -= 2 a frame (a stick scales it). */
        s->camera_zoom -= 2 * axis;
        /* Toad Main Clip: CameraZoom += 3 while talking (to 150), -= 3 to 100 after. */
        if (talking) {
            if (s->camera_zoom < 150) { s->camera_zoom += 3; s->talk_zoomed = true; }
            else s->camera_zoom = fminf(s->camera_zoom + 3, 150);
        } else if (s->talk_zoomed) {
            if (s->camera_zoom > 100) s->camera_zoom -= 3;
            else { s->camera_zoom = 100; s->talk_zoomed = false; }
        }
        s->camera_zoom = fminf(COURSE_ZOOM_MAX * 100, fmaxf(COURSE_ZOOM_MIN * 100, s->camera_zoom));
        s->zoom2 += roundf(s->camera_zoom - s->zoom2) / 20;    /* Math.round(CameraZoom - CameraZoom2) / 20 */
    }
    s->zoom = (s->zoom2_from + (s->zoom2 - s->zoom2_from) * fminf(1, fmaxf(0, frac))) / 100;
}
static void add_fx(CourseSession *s, int kind, float x, float y, float dy, unsigned frames)
{
    if (s->fx_count >= sizeof s->fx / sizeof s->fx[0]) return;
    s->fx[s->fx_count].kind = kind; s->fx[s->fx_count].x = x; s->fx[s->fx_count].y = y;
    s->fx[s->fx_count].dy = dy; s->fx[s->fx_count].frame = 0; s->fx[s->fx_count].frames = frames;
    ++s->fx_count;
}
/* The FrontGFX effects, one source frame: WaterBubble 2589 rises 3 px a frame
   and removes itself on frame 40, Med-Splash 2609 on frame 10. */
static void fx_step(CourseSession *s)
{
    unsigned n = 0;
    for (unsigned i = 0; i < s->fx_count; ++i) {
        s->fx[i].y += s->fx[i].dy;
        if (++s->fx[i].frame < s->fx[i].frames) s->fx[n++] = s->fx[i];
    }
    s->fx_count = n;
}
/* CharCode's air (WaterHP), one source frame: with the feet in the water and
   the head (tall above) too, every 81st frame a WaterBubble rises from the
   chest, "sound - Drain" plays and WaterHP drops (at 0: attackFrame "Drown");
   feet in but head out heals 1 every 9th frame, and so does being out of the
   water (Char.Water false, which also restarts the count). */
static void water_hp_step(CourseSession *s)
{
    Player *p = &s->game.player;
    CourseProgress *g = &s->progress;
    const CourseArea *a = s->runtime.area;
    const float x = course_cx(&s->runtime, p->x + PLAYER_WIDTH / 2), feet = course_cy(&s->runtime, p->y + player_height(p));
    const float tall = player_height(p) / COURSE_SCALE;
    if (course_area_water(a, x, feet, NULL)) {
        if (course_area_water(a, x, feet - tall, NULL)) {
            if (++s->under_count > 80) {   /* _root.WaterDrownSpeed */
                add_fx(s, COURSE_FX_BUBBLE, x, feet - tall * 0.75f, -3, 39);
                sfx_play(SFX_DRAIN, 100, 0);
                if (--g->water_hp <= 0 && !p->drowning) {
                    p->drowning = true; p->drown_frame = 0;
                    p->action = ACTION_NONE; p->sleeping = false; p->ejump = false; p->pounding = false; p->spin_time = 0;
                    sfx_play(SFX_DROWN, 100, 0);   /* Mario Drown frame 1: "mario-drown" */
                }
                if (g->water_hp < 0) g->water_hp = 0;
                s->under_count = 0;
            }
        } else if (++s->above_count > 8) {   /* _root.WaterHealSpeed */
            s->above_count = 0;
            if (g->water_hp < 8) ++g->water_hp;
        }
    }
    if (!p->submerged) {
        s->under_count = 0;
        if (++s->above_count > 8) { s->above_count = 0; if (g->water_hp < 8) ++g->water_hp; }
    }
}
/* Mario Drown (1692), one source frame on: 34 bgsong.stop(), 48 "Mario -
   Mamamiamoan", 76 Blackout(), 88 LifeLost() (1-based clip frames). */
static void drown_events(CourseSession *s)
{
    Player *p = &s->game.player;
    if (p->drown_frame < 87) ++p->drown_frame;
    const unsigned f = p->drown_frame + 1;
    if (f == 34) sfx_music(-1, -1);
    if (f == 48) sfx_play(SFX_MAMAMIA, 100, 0);
    if (f == 76 && !s->blackout) s->blackout = 1;
    if (f == 88 && !s->lifelost) s->lifelost = 1;
}
/* The faint's timeline, from the hurt clip Mario is in (0-based frames):
   hurt1 1411 60 bgsong.stop(), 67 "Mario - Mamamiamoan", 95 Blackout(),
   107 LifeLost(); hurt3 1430 24, 30, 58, 70. Otherwise (hurt2 ended, or no
   hurt clip) attackFrame "Faint": 1703 'nes' plays "Mario-Fall", stops the
   music and attaches MarioDeathGFX at Mario. */
static void faint_events(CourseSession *s)
{
    Player *p = &s->game.player;
    if (p->action == ACTION_HURT) {
        const unsigned f = p->hurt_frame;
        if (f == s->last_hurt_frame) return;
        s->last_hurt_frame = f;
        static const unsigned ev[2][4] = {{59, 66, 94, 106}, {23, 29, 57, 69}};
        const unsigned *e = ev[p->hurt_kind == 1 ? 0 : 1];
        if (f == e[0]) sfx_music(-1, -1);
        if (f == e[1]) sfx_play(SFX_MAMAMIA, 100, 0);
        if (f == e[2] && !s->blackout) s->blackout = 1;
        if (f == e[3] && !s->lifelost) s->lifelost = 1;
    } else if (p->action != ACTION_BONK && !s->death) {
        s->death = 1;
        s->death_x = p->x + PLAYER_WIDTH / 2; s->death_y = p->y + player_height(p);
        sfx_music(-1, -1); sfx_play(SFX_FALL, 100, 0);
    }
}
/* _root.ReviveMario(playingcourse, StartClip x, y): the area again from its
   start, full health, its music, the StarIn wipe. */
static void revive(CourseSession *s)
{
    Player *p = &s->game.player;
    s->progress.hp = 8; s->progress.hpcoins = 0; s->progress.water_hp = 8;
    p->drowning = false; p->drown_frame = 0; p->sleeping = false; p->idle_kind = 0;
    s->under_count = s->above_count = 0; s->fx_count = 0;
    p->fainting = false; p->action = ACTION_NONE; p->hurt_frame = 0; p->blink = 0;
    s->death = s->blackout = 0; s->last_hurt_frame = ~0u;
    enter_area(s, s->runtime.area, course_cx(&s->runtime, s->world.spawn_x + PLAYER_WIDTH / 2),
               course_cy(&s->runtime, s->world.spawn_y + PLAYER_HEIGHT));
    /* PlayMusicAndIntro(): _root.SongIntro, the song playing before (the castle theme in every tutorial area) */
    sfx_music(MUS_CASTLE_INTRO, MUS_CASTLE_REPEAT);
    s->starin = 1;
}
/* One source frame of the faint's screen clips. MarioDeathGFX: 48 Blackout(),
   60 LifeLost(). Blackout: removed on frame 13. LifeLost: 14 "Explosion",
   59 ReviveMario(). StarIn: 12 frames. */
static void faint_fx_frame(CourseSession *s)
{
    if (s->death) {
        ++s->death;
        if (s->death == 48 && !s->blackout) s->blackout = 1;
        if (s->death >= 60) { s->death = 0; if (!s->lifelost) s->lifelost = 1; }
    }
    if (s->blackout && ++s->blackout > 12) s->blackout = 0;   /* frame 13: removeMovieClip */
    if (s->starin && ++s->starin > 12) s->starin = 0;
    if (s->wipe_in) ++s->wipe_in;
    if (s->wipe_out && ++s->wipe_out > wipe_out_frames(s->wipe)) s->wipe_out = 0;
    if (s->lifelost) {
        ++s->lifelost;
        if (s->lifelost == 14) sfx_play(SFX_EXPLOSION, 100, 0);
        if (s->lifelost >= 59) { s->lifelost = 0; revive(s); }
    }
}
/* The pause menu (pause_menu.c) and the achievement card run first; while
   the menu is up the game stands still (_root.PauseGame). */
static bool pause_and_notice(CourseSession *s, GameInput *in)
{
    Player *p = &s->game.player;
    const bool busy = s->death || s->blackout || s->lifelost || s->starin || s->wipe_in || s->wipe_out ||
                      s->state == SESSION_WARP || s->state == SESSION_COMPLETE;
    const PauseContext pc = {!busy, p->has_fludd, p->nozzles, s->ach ? ach_popcount(s->ach->p.shines) : 0};
#ifdef SM63_DEV
    /* Developer build (make DEV=1; saves go to a separate file): on the
       Achievements page, jump unlocks the selected one, the nozzle button
       collects all 64 Shine Sprites, 64 Star Coins and the keys through the
       game's own events, stomp clears the dev save's progress. */
    if (s->ach && pause_active(&s->pause) && s->pause.page == PAUSE_ACHIEVEMENTS && !s->pause.trans) {
        if (in->confirm_pressed && s->pause.focus == PI_LIST) ach_force(s->ach, s->pause.list_sel + 1u);
        if (in->nozzle_next_pressed) {
            for (unsigned i = 1; i <= ACH_SHINES; ++i) ach_shine(s->ach, i);
            for (unsigned i = 1; i <= ACH_STAR_COINS; ++i) ach_star_coin(s->ach, i);
            for (unsigned k = 1; k <= 3; ++k) ach_key(s->ach, k);
            ach_milestone(s->ach, ACH_MS_ENTER_BOWSER_TRAP2); ach_milestone(s->ach, ACH_MS_ENDING);
        }
        if (in->pound_pressed) { ach_init(s->ach, NULL); s->ach->dirty = true; s->notice.id = 0; }
        if (in->confirm_pressed || in->nozzle_next_pressed || in->pound_pressed) s->save_request = true;
    }
#endif
    pause_step(&s->pause, in, &pc);
    unsigned arg;
    switch (pause_take_action(&s->pause, &arg)) {
    case PAUSE_ACT_QUIT: s->game.quit = true; break;
    case PAUSE_ACT_MUTE: sfx_set_music_muted(arg != 0); s->save_request = true; break;
    case PAUSE_ACT_NOZZLE:   /* _root.Fluddpow = "" / "H" / "R" / "T" */
        if (arg == 0xFF) p->has_fludd = false;
        else if (p->nozzles >> arg & 1) { p->has_fludd = true; p->nozzle = (Nozzle)arg; }
        break;
    default: break;
    }
    ach_notice_step(&s->notice, s->ach);
    in->pause_pressed = false;   /* the menu owns the key (game_step's old toggle must not see it) */
    s->game.paused = pause_active(&s->pause);
    if (s->game.paused) {        /* stand still: no interpolation between ticks */
        p->previous_x = p->x; p->previous_y = p->y;
        s->game.camera.previous_x = s->game.camera.x; s->game.camera.previous_y = s->game.camera.y;
    }
    return s->game.paused;
}
void course_session_step(CourseSession *s, GameInput in)
{
    s->controller = in.controller;
    if (pause_and_notice(s, &in)) return;
    course_session_zoom_step(s, in.zoom_axis, s->state == SESSION_TALK,
                             s->game.player.source_step, s->game.player.source_phase / 60.0f);
    course_camera_set_zoom(course_session_zoom(s));
    if (s->death || s->blackout || s->lifelost || s->starin || s->wipe_in || s->wipe_out) {
        s->fx_phase += 32;
        if (s->fx_phase >= 60) { s->fx_phase -= 60; faint_fx_frame(s); }
    }
    if (s->lifelost) return;   /* LifeLost frame 2: _root.RemoveCourse() */
    if (s->game.player.source_step) {   /* simWave, each source frame */
        for (unsigned i = 0; i < s->bg_water_count; ++i)
            bg_wave_step(&s->bg_wave[i], bg_waters[s->bg_water_index[i]].stiffness, bg_waters[s->bg_water_index[i]].randomwave);
        level_water_step(s);
    }
    /* KeyZ(): Z (read/talk/stomp). The port maps it to the pound button. */
    const bool z_pressed = in.pound_pressed; /* already a button-down event */
    if (s->area_text && s->game.player.source_step && ++s->area_text > COURSE_AREA_TEXT_LAST) s->area_text = 0;
    Player *p = &s->game.player;
    const Camera camera_before = s->game.camera;
    switch (s->state) {
    case SESSION_COMPLETE:
        return;
    case SESSION_WARP:
        /* the wipe in sets _root.warpframe on its frame 16 (downtransition 15):
           changecourse, and the new area starts under the wipe out */
        p->vx = p->vy = 0;
        if (s->wipe_in > wipe_warp_frame(s->wipe)) {
            const WarpRule *w = &warps[s->warp_target];
            const CourseArea *next = course_pack_area(s->pack, w->to);
            const bool from_castle = course_string_equals(s->runtime.area->id, "Castle");
            if (next) enter_area(s, next, s->warp_x, w->y);
            /* into the Castle: StopBGsong(); out of it: changecourse(..., true) starts the area's music */
            if (next && course_string_equals(next->id, "Castle")) sfx_music(-1, -1);
            else if (next && from_castle) sfx_music(MUS_CASTLE_INTRO, MUS_CASTLE_REPEAT);
            s->warp_target = -1; s->wipe_in = 0; s->wipe_out = 1;
            s->state = SESSION_PLAY;
        }
        return;
    case SESSION_SIGN: {
        /* Coded Sign: Mario stands in the "Sign" pose and slides to the sign
           (`Char._x += (X - Char._x) / 5`); the message shows on frame 10 and Z
           (pressed again) plays frames 11-16, which end the reading. */
        p->vx = 0; if (p->vy < 0) p->vy = 0;
        game_step(&s->game, &s->world, (GameInput){0}, s->view_width);
        if (p->source_step) {
            const float fx = course_cx(&s->runtime, p->x + PLAYER_WIDTH / 2);
            p->x += (s->sign->x - fx) / 5 * COURSE_SCALE;
            ++s->sign_frames;
            if (s->sign_phase == 0 && s->sign_frames >= 6) { s->sign_phase = 1; s->sign_frames = 0; }
            else if (s->sign_phase == 2 && s->sign_frames >= 6) { close_sign(s); break; }
        }
        if (s->sign_phase == 1 && z_pressed) { s->sign_phase = 2; s->sign_frames = 0; }
        break;
    }
    case SESSION_OPENING: {
        /* Start sign enterFrame: after landing wait 60 frames, walk right for
           12 more (`xspeed += 3; xspeed *= 0.6`, facing right), then read. */
        GameInput none = {0};
        game_step(&s->game, &s->world, none, s->view_width);
        if (p->grounded && p->source_step) {
            if (s->opening_wait < 60) ++s->opening_wait;
            else if (s->opening_count++ <= 12) {
                p->facing_left = false;
                p->vx = (p->vx / 44 + 3) * 0.6f * 44;
            } else {
                for (unsigned i = 0; i < s->runtime.area->object_count; ++i)
                    if (param(&s->runtime.area->objects[i], "signmessage", "Start")) open_sign(s, &s->runtime.area->objects[i]);
            }
        }
        break;
    }
    case SESSION_TALK:
        p->vx = p->vy = 0;
        game_step(&s->game, &s->world, (GameInput){0}, s->view_width);
        if (p->source_step) {
            tm_step(s, in.pound_held);
            /* Between pages ToadMessage is "" for one frame (Toad's next MnSta), so the cloud dips. */
            if (!s->tm_message && s->toad_page && toad_page(s, s->toad_page)) s->tm_message = true;
        }
        if (p->source_step) {
            /* talking: Mario is pushed 2 px a frame out of Toad's body. */
            const float mx = course_cx(&s->runtime, p->x + PLAYER_WIDTH / 2);
            if (fabsf(mx - s->toad->x) < 12) p->x += (mx > s->toad->x ? 2 : -2) * COURSE_SCALE;
        }
        if (z_pressed && toad_page_done(s)) {
            s->tm_message = false;   /* ToadMessage = ""; Phase += 1 */
            CourseString keep = s->toad_text;
            const unsigned keep_chars = s->toad_chars;
            if (!toad_page(s, ++s->toad_page)) {
                toad_rest(s);
                s->state = SESSION_PLAY; s->toad = NULL; s->camera_override = false;
            }
            /* the shown page stays in tm until the next one starts typing */
            s->toad_text = keep; s->toad_chars = keep_chars;
        }
        break;
    case SESSION_PLAY:
        p->low_hp = s->progress.hp <= 3;
        /* Sleep: attack = true, no control; Up, Right, Left, Down or Space wake him. */
        p->wake_keys = in.horizontal != 0 || in.jump_held || in.vertical != 0 || in.crouch_held || in.nozzle_held;
        if (p->stun > 0 || p->ejump || p->sleeping) in = (GameInput){.jump_held = in.jump_held}; /* StunTime / EJump: no control */
        if (p->fainting || s->death || p->drowning) in = (GameInput){0};
        if (s->death) { p->vx = p->vy = 0; break; }       /* Faint: Mario is hidden while MarioDeathGFX plays */
        if (p->drowning) {   /* attackFrame "Drown": xspeed = 0, yspeed 0 in water (-gravity out of it) */
            const float hx = p->x, hy = p->y;
            game_step(&s->game, &s->world, in, s->view_width);
            p->x = hx; p->y = hy; p->vx = p->vy = 0;
            if (p->source_step) { fx_step(s); drown_events(s); }
            break;
        }
        sling_input(s, &in);
        closed_doors(s, &in);
        game_step(&s->game, &s->world, in, s->view_width);
        if (p->source_step) { fx_step(s); water_hp_step(s); sling_frame(s, in); }
        if (s->sling.state == SLING_HOLD || s->sling.state == SLING_SPIN) p->vx = p->vy = 0;
        if (p->source_step && s->closed_note) --s->closed_note;
        if (p->source_step) ambient_step(s);
        if (p->splashed) {   /* Med-Splash at Mario's feet */
            p->splashed = false;
            add_fx(s, COURSE_FX_SPLASH, course_cx(&s->runtime, p->x + PLAYER_WIDTH / 2),
                   course_cy(&s->runtime, p->y + player_height(p)), 0, 9);
        }
        if (p->source_step && s->tm_alpha > 0.01f) tm_step(s, false); /* the cloud fades out after the talk */
        if (s->progress.hp <= 0 && !p->fainting && p->action != ACTION_HURT && p->action != ACTION_BONK) p->fainting = true;
        if (p->fainting) { faint_events(s); break; }
        try_read_sign(s, in, z_pressed);
        if (s->state == SESSION_PLAY) try_talk(s, z_pressed);
        if (s->state == SESSION_PLAY) check_exits(s, in);
        break;
    }
    if (s->state != SESSION_WARP)
        course_objects_step(&s->objects, &s->runtime, &s->game.player, &s->progress, pickups_for(s), false,
                            s->game.camera.cx, s->game.camera.cy, in.jump_held);
    if (s->game.player.source_step) hud_step(&s->hud, s->progress.hp, s->progress.water_hp);
    if (s->camera_override && s->state != SESSION_WARP) {
        /* Camcenter = another clip: the only camera move this frame (following
           Mario as well made the two moves and the CamEdge push fight: shake). */
        s->game.camera = camera_before;
        course_camera_step_to(&s->game.camera, &s->runtime, s->camera_target_x, s->camera_target_y, p,
                              s->view_width);
    }
    {   /* FLUDD state shown by the switch banner */
        const Player *pl = &s->game.player;
        const unsigned char now = pl->has_fludd ? (unsigned char)(1 + pl->nozzle) : 0;
        if (now != s->last_fludd && (pl->nozzles || pl->has_fludd)) { s->nozzle_banner = 90; s->banner_fludd = now; }
        s->last_fludd = now;
        if (s->nozzle_banner) --s->nozzle_banner;
    }
    update_hint(s);
}

/* Text signs that name keyboard keys: the same message with the button of the
   controller in use (mapping: source/platform/input.c). Up is the original's
   jump key; P its pause key. */
CourseString course_sign_message(CourseString text, unsigned controller)
{
#define RUN_SIGN "You can run by double tapping Right or Left.\nIn the air you can press Down to Lunge or Dive forwards\n" \
    "Also, while on ducking/crouching on the ground, you can backflip off the ground by quickly pressing "
#define PAUSE_SIGN "Oh!  Mario!  You can pause the game by pressing "
#define PAUSE_END ".  There, you can change settings, view stuff you've collected, and switch FLUDD power-ups."
    /* Jump: GameCube A, Classic b, Wii Remote 2. Pause: START, PLUS, PLUS and MINUS together.
       (The sign font has letters and digits only, so + and - are spelled out.) */
    static const char *const backflip[CONTROLLER_KINDS] = {RUN_SIGN "A and back.", RUN_SIGN "B and back.",
                                                           RUN_SIGN "2 and back."};
    static const char *const pause[CONTROLLER_KINDS] = {PAUSE_SIGN "<b>START</b>" PAUSE_END,
                                                        PAUSE_SIGN "<b>PLUS</b>" PAUSE_END,
                                                        PAUSE_SIGN "<b>PLUS and MINUS</b> together" PAUSE_END};
    if (controller >= CONTROLLER_KINDS) return text;
    const char *pick = NULL;
    if (text.length >= 29 && !memcmp(text.text, "You can run by double tapping", 29)) pick = backflip[controller];
    else if (text.length >= 28 && !memcmp(text.text, "Oh!  Mario!  You can pause t", 28)) pick = pause[controller];
    return pick ? (CourseString){pick, (uint16_t)strlen(pick)} : text;
}
