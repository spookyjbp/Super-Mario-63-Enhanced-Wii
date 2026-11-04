#include "course_session.h"
#include "bg_water.h"
#include "bg_water_places.h"
#include "sfx.h"
#include <math.h>
#include <string.h>

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
    if (course_string_equals(a->id, "8-E1-1")) {
        s->area_title = (CourseString){"Mushroom Kingdom\n(Tutorial)", 27}; s->area_title_time = 45.0f / 32 * 2;
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
    s->pack = pack; s->view_width = view_width; s->warp_target = -1;
    s->zoom = 1; s->camera_zoom = s->zoom2 = s->zoom2_from = 100; s->talk_zoomed = false;
    course_camera_set_zoom(1);
    s->last_hurt_frame = ~0u;
    s->game.player.has_fludd = false; s->game.player.nozzles = 0; /* StartChar(..., Fluddpow "") */
    s->progress.hp = 8;                 /* _root.CharHP = 8 */
    hud_init(&s->hud, s->progress.hp);
    /* StartChar(..., resetmusic = true): _root["8-E1-1MusicIntro"], then "8-E1-1Music". */
    if (course_string_equals(course_pack_area(pack, id)->id, "8-E1-1") || course_string_equals(course_pack_area(pack, id)->id, "8-E1-2-2"))
        sfx_music(MUS_CASTLE_INTRO, MUS_CASTLE_REPEAT);
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
        float b[4];
        course_object_bounds(o, COURSE_BLANK_U0, COURSE_BLANK_V0, COURSE_BLANK_U1, COURSE_BLANK_V1, b);
        if (inside(b, bx, by)) s->hint = mess;
    }
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
                if (inside(b, fx, fy)) {
                    s->state = SESSION_WARP; s->warp_target = (int)w;
                    s->wipe = warps[w].wipe; s->wipe_in = 1; s->wipe_out = 0;
                    s->warp_x = isnan(warps[w].x) ? fminf(1800, fmaxf(400, 2000 + fx)) : warps[w].x;
                    s->game.player.vx = s->game.player.vy = 0;
                }
            }
        }
        if (course_string_equals(o->type, "Invisible Door") && param(o, "doorplace", "C-Opening")) {
            const float sx = o->a;
            const float box[4] = {o->x + door_box[0] * sx, o->y + door_box[1], o->x + door_box[2] * sx, o->y + door_box[3]};
            if (inside(box, fx, fy) && in.crouch_held && s->game.player.grounded) {
                /* doorplace "C-Opening" with Star[39] false: Intro Movie 2, then the
                   Castle. Neither exists in this port yet: stop at a boundary. */
                s->state = SESSION_COMPLETE;
                s->game.player.vx = s->game.player.vy = 0;
            }
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
    s->progress.hp = 8; s->progress.hpcoins = 0;
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
void course_session_step(CourseSession *s, GameInput in)
{
    s->controller = in.controller;
    course_session_zoom_step(s, in.zoom_axis, s->state == SESSION_TALK,
                             s->game.player.source_step, s->game.player.source_phase / 60.0f);
    course_camera_set_zoom(course_session_zoom(s));
    if (s->death || s->blackout || s->lifelost || s->starin || s->wipe_in || s->wipe_out) {
        s->fx_phase += 32;
        if (s->fx_phase >= 60) { s->fx_phase -= 60; faint_fx_frame(s); }
    }
    if (s->lifelost) return;   /* LifeLost frame 2: _root.RemoveCourse() */
    if (s->game.player.source_step)   /* simWave, each source frame */
        for (unsigned i = 0; i < s->bg_water_count; ++i)
            bg_wave_step(&s->bg_wave[i], bg_waters[s->bg_water_index[i]].stiffness, bg_waters[s->bg_water_index[i]].randomwave);
    /* KeyZ(): Z (read/talk/stomp). The port maps it to the pound button. */
    const bool z_pressed = in.pound_pressed; /* already a button-down event */
    if (s->area_title_time > 0) s->area_title_time = fmaxf(0, s->area_title_time - GAME_DT);
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
            if (next) enter_area(s, next, s->warp_x, w->y);
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
        if (p->stun > 0 || p->ejump) in = (GameInput){.jump_held = in.jump_held}; /* StunTime / EJump: no control */
        if (p->fainting || s->death) in = (GameInput){0};
        if (s->death) { p->vx = p->vy = 0; break; }       /* Faint: Mario is hidden while MarioDeathGFX plays */
        game_step(&s->game, &s->world, in, s->view_width);
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
    if (s->game.player.source_step) hud_step(&s->hud, s->progress.hp);
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
