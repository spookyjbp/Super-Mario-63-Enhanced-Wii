#include "course_session.h"
#include "sfx.h"
#include <math.h>
#include <string.h>

/* `_root.changecourse(transition, area, b, c, d, e)` targets from each area's
   BackGFX frame-3 script: StartChar puts Char at local (d, e). */
typedef struct { const char *from, *warp, *to; float x, y; } WarpRule;
static const WarpRule warps[] = {
    {"8-E1-1", "warp1", "8-E1-1-2", 0, 0},        /* 8-E1-1 Back GFX frame 3 */
    {"8-E1-1-2", "warp1", "8-E1-1", 1760, -20},   /* 8-E1-1-2 Back GFX frame 3 */
    {"8-E1-1-2", "warp2", "8-E1-2", 0, 0},
    {"8-E1-2", "warp1", "8-E1-1-2", 1730, -30},   /* 8-E1-2 Back GFX frame 3 */
    {"8-E1-2", "warp2", "8-E1-2-2", 0, 0},
    {"8-E1-2-2", "warp1", "8-E1-2", 1840, -170},  /* 8-E1-2-2 BackGFX frame 3 */
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
    /* Falling out respawns at the area's entry (StartX/StartY in StartChar). */
    s->world.spawn_x = course_wx(&s->runtime, cx) - PLAYER_WIDTH / 2;
    s->world.spawn_y = course_wy(&s->runtime, cy) - PLAYER_HEIGHT;
    s->back_frame = 2;
    const bool keep_fludd = s->game.player.has_fludd;
    game_init(&s->game, &s->world, s->view_width);
    s->game.player.has_fludd = keep_fludd;
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
    s->game.player.has_fludd = false; /* StartChar(..., Fluddpow "") */
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
    s->sign = o; s->state = SESSION_SIGN;
    s->sign_picture = param(o, "picture", "true");
    course_object_param(o, "signmessage", &s->sign_text);
    Player *p = &s->game.player;
    p->vx = p->vy = 0; p->action = ACTION_NONE; p->dashing = false;
    /* othercamcenter: the Start sign looks at the controls sign (sign2), the
       goomba/bomb/stomp signs at their targetN clip. */
    CourseString target;
    s->camera_override = false;
    if (course_object_param(o, "othercamcenter", &target) || param(o, "signmessage", "Start")) {
        const CourseArea *a = s->runtime.area;
        const char *want = param(o, "signmessage", "Start") ? "sign2" : NULL;
        for (unsigned i = 0; i < a->object_count && !s->camera_override; ++i) {
            const CourseObject *t = &a->objects[i];
            const bool match = want ? course_string_equals(t->name, want)
                                    : (t->name.length && target.length > t->name.length &&
                                       !memcmp(target.text + target.length - t->name.length, t->name.text, t->name.length));
            if (match) {
                s->camera_override = true;
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
                    s->state = SESSION_WARP; s->warp_time = 0; s->warp_target = (int)w;
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
    return true;
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
    s->state = SESSION_TALK;
    s->camera_override = true; s->camera_target_x = t->x; s->camera_target_y = t->y; /* Camcenter = this */
    Player *pm = &s->game.player;
    pm->vx = pm->vy = 0;
    pm->facing_left = course_cx(&s->runtime, pm->x + PLAYER_WIDTH / 2) > t->x; /* face Toad */
}
void course_session_step(CourseSession *s, GameInput in)
{
    /* KeyZ(): Z (read/talk/stomp). The port maps it to the pound button. */
    const bool z_pressed = in.pound_pressed; /* already a button-down event */
    if (s->area_title_time > 0) s->area_title_time = fmaxf(0, s->area_title_time - GAME_DT);
    Player *p = &s->game.player;
    const Camera camera_before = s->game.camera;
    switch (s->state) {
    case SESSION_COMPLETE:
        return;
    case SESSION_WARP:
        s->warp_time += GAME_DT;
        p->vx = p->vy = 0;
        if (s->warp_time >= COURSE_WARP_SECONDS) {
            const WarpRule *w = &warps[s->warp_target];
            const CourseArea *next = course_pack_area(s->pack, w->to);
            if (next) enter_area(s, next, w->x, w->y);
            s->warp_target = -1;
        }
        return;
    case SESSION_SIGN:
        p->vx = p->vy = 0;
        game_step(&s->game, &s->world, (GameInput){0}, s->view_width);
        if (z_pressed) close_sign(s);
        break;
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
        if (p->source_step && s->toad_chars < s->toad_text.length) /* tm: two characters a frame */
            s->toad_chars = s->toad_chars + 2 < s->toad_text.length ? s->toad_chars + 2 : s->toad_text.length;
        if (z_pressed && !toad_page(s, ++s->toad_page)) {
            s->state = SESSION_PLAY; s->toad = NULL; s->camera_override = false;
        }
        break;
    case SESSION_PLAY:
        if (p->stun > 0 || p->ejump) in = (GameInput){.jump_held = in.jump_held}; /* StunTime / EJump: no control */
        game_step(&s->game, &s->world, in, s->view_width);
        if (s->progress.hp <= 0) {
            /* Faint: the original plays the faint clip and restarts the course
               (lives); the port restarts this area's entry with full health. */
            s->progress.hp = 8; s->progress.hpcoins = 0;
            enter_area(s, s->runtime.area, course_cx(&s->runtime, s->world.spawn_x + PLAYER_WIDTH / 2),
                       course_cy(&s->runtime, s->world.spawn_y + PLAYER_HEIGHT));
            break;
        }
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
        course_camera_step_to(&s->game.camera, &s->runtime, s->camera_target_x, s->camera_target_y, p->source_step,
                              s->view_width);
    }
    update_hint(s);
}
