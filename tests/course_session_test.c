/* The original tutorial's scripted flow, start to completion boundary. */
#include "course_session.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static unsigned char blob[1 << 20];
static CoursePack pack;
static CourseSession s;

static const CourseObject *named(const char *type, const char *key, const char *value)
{
    const CourseArea *a = s.runtime.area;
    for (unsigned i = 0; i < a->object_count; ++i) {
        CourseString v;
        if (!course_string_equals(a->objects[i].type, type)) continue;
        const bool match = key ? course_object_param(&a->objects[i], key, &v) && course_string_equals(v, value)
                               : course_string_equals(a->objects[i].name, value);
        if (match && course_session_object_active(&s, &a->objects[i])) return &a->objects[i];
    }
    return NULL;
}
static void ticks(GameInput in, int n) { for (int i = 0; i < n; ++i) course_session_step(&s, in); }
static void stand_at(float x, float y)
{
    course_place_player(&s.runtime, &s.game.player, x, y);
    s.game.player.vx = s.game.player.vy = 0;
    ticks((GameInput){0}, 40);
    assert(s.game.player.grounded);
}
/* Coded Sign frames 4-9 before the message, 11-16 after Z. */
static void sign_shown(void)
{
    for (int i = 0; i < 30 && s.sign_phase != 1; ++i) course_session_step(&s, (GameInput){0});
    assert(s.state == SESSION_SIGN && s.sign_phase == 1 && s.game.player.reading_sign);
}
static void sign_close(void)
{
    ticks((GameInput){.pound_pressed = true}, 1);
    assert(s.state == SESSION_SIGN && s.sign_phase == 2 && s.game.player.reading_sign);
    for (int i = 0; i < 30 && s.state == SESSION_SIGN; ++i) course_session_step(&s, (GameInput){0});
    assert(s.state == SESSION_PLAY && !s.game.player.reading_sign);
}
static bool hint_is(const char *prefix)
{ return s.hint.text && s.hint.length >= strlen(prefix) && !memcmp(s.hint.text, prefix, strlen(prefix)); }

static void test_opening_and_controls_sign(void)
{
    assert(course_session_start(&s, &pack, "8-E1-1", 0, 0, 640));
    assert(s.state == SESSION_OPENING && !s.game.player.has_fludd && s.runtime.platform_frame == 0);
    assert(s.area_title.length && s.area_title_time > 0);
    for (int i = 0; i < 60 * 8 && s.state == SESSION_OPENING; ++i) course_session_step(&s, (GameInput){.horizontal = -1});
    /* Walked in by script (input ignored), then the Start picture opens. */
    assert(s.state == SESSION_SIGN && s.sign_picture && course_string_equals(s.sign_text, "Start"));
    assert(s.camera_override && s.camera_target_x == 146); /* looks at the controls sign */
    ticks((GameInput){.pound_pressed = true}, 1);
    assert(s.state == SESSION_SIGN && s.sign_phase == 0); /* Z before frame 10 does nothing */
    sign_shown();
    sign_close();
    assert(s.runtime.platform_frame == 0);
    /* Reading the controls sign removes the barrier (completefunction). */
    const CourseObject *controls = named("Coded Sign", "signmessage", "controls");
    stand_at(controls->x, controls->y);
    ticks((GameInput){.pound_pressed = true}, 1);
    assert(s.state == SESSION_SIGN && course_string_equals(s.sign_text, "controls"));
    /* Bug 10: the reading pose; the camera looks at the sign (Camcenter = this). */
    assert(s.game.player.reading_sign && s.camera_override && s.camera_target_x == controls->x);
    sign_shown();
    ticks((GameInput){0}, 10);
    assert(s.state == SESSION_SIGN && s.sign_phase == 1); /* stays open until Z again */
    sign_close();
    assert(s.runtime.platform_frame == 1);
}
static void test_hints(void)
{
    assert(course_session_start(&s, &pack, "8-E1-1", 0, 0, 640));
    s.state = SESSION_PLAY; s.runtime.platform_frame = 0;
    stand_at(287, 60);
    assert(hint_is("<b>No Mario!"));
    s.runtime.platform_frame = 1; ticks((GameInput){0}, 1);
    assert(!hint_is("<b>No Mario!")); /* only while the barrier stands */
    stand_at(750, 60);
    assert(hint_is("(Hint:  Jumping twice"));
    stand_at(150, 90); ticks((GameInput){0}, 1);
    assert(!s.hint.text); /* cleared when leaving the zone */
}
static void warp_through(const char *warp, const char *next)
{
    const CourseObject *w = named("Blank Movieclip", NULL, warp);
    assert(w);
    float b[4];
    course_object_bounds(w, COURSE_BLANK_U0, COURSE_BLANK_V0, COURSE_BLANK_U1, COURSE_BLANK_V1, b);
    course_place_player(&s.runtime, &s.game.player, (b[0] + b[2]) / 2, (b[1] + b[3]) / 2);
    ticks((GameInput){0}, 1);
    assert(s.state == SESSION_WARP);
    ticks((GameInput){0}, (int)(COURSE_WARP_SECONDS * 60) + 2);
    if (!(s.state == SESSION_PLAY && course_string_equals(s.runtime.area->id, next)))
        fprintf(stderr, "warp %s -> %s: state %d area %.*s\n", warp, next, s.state, s.runtime.area->id.length, s.runtime.area->id.text);
    assert(s.state == SESSION_PLAY && course_string_equals(s.runtime.area->id, next));
}
static void test_area_chain_and_completion(void)
{
    assert(course_session_start(&s, &pack, "8-E1-1", 0, 0, 640));
    s.state = SESSION_PLAY; s.runtime.platform_frame = 1;
    warp_through("warp1", "8-E1-1-2");
    assert(fabsf(course_cx(&s.runtime, s.game.player.x + PLAYER_WIDTH / 2)) < 1); /* (0, 0) */
    warp_through("warp1", "8-E1-1");  /* back: arrives at (1760, -20) */
    assert(fabsf(course_cx(&s.runtime, s.game.player.x + PLAYER_WIDTH / 2) - 1760) < 1);
    assert(s.state == SESSION_PLAY && s.runtime.platform_frame == 1); /* no second opening */
    warp_through("warp1", "8-E1-1-2");
    warp_through("warp2", "8-E1-2");
    warp_through("warp2", "8-E1-2-2");
    assert(!s.game.player.has_fludd); /* FLUDD box not ported yet: still none */
    const CourseObject *pipe = named("Invisible Door", "doorplace", "C-Opening");
    stand_at(pipe->x, pipe->y + 9);
    ticks((GameInput){0}, 10);
    assert(s.state == SESSION_PLAY);
    ticks((GameInput){.crouch_held = true, .vertical = 1}, 1);
    assert(s.state == SESSION_COMPLETE);
}
/* Bug 1: reading a sign (Start looks at sign2 near the CamEdge) must not shake. */
static void test_sign_camera_steady(void)
{
    assert(course_session_start(&s, &pack, "8-E1-1", 0, 0, 854));
    for (int t = 0; t < 2000 && s.state != SESSION_SIGN; ++t) course_session_step(&s, (GameInput){0});
    assert(s.state == SESSION_SIGN && s.camera_override);
    float lx = s.game.camera.x, ly = s.game.camera.y, ldx = 0, ldy = 0;
    for (int i = 0; i < 400; ++i) {
        course_session_step(&s, (GameInput){0});
        const float dx = s.game.camera.x - lx, dy = s.game.camera.y - ly;
        assert(!(dx * ldx < 0) && !(dy * ldy < 0) && fabsf(dx) < 8 && fabsf(dy) < 8); /* no reversals or jumps */
        if (dx != 0) ldx = dx;
        if (dy != 0) ldy = dy;
        lx = s.game.camera.x; ly = s.game.camera.y;
    }
    /* Closing eases back to Mario (1/10 a frame, as the original) without reversing. */
    sign_close();
    assert(!s.camera_override);
    ldx = 0;
    for (int i = 0; i < 90; ++i) {
        course_session_step(&s, (GameInput){0});
        const float dx = s.game.camera.x - s.game.camera.previous_x;
        assert(!(dx * ldx < -0.01f));
        if (fabsf(dx) > 0.01f) ldx = dx;
    }
}
/* Bug 3 on the tutorial's real (sloped) ground. */
static Player *tutorial_mario(float x)
{
    assert(course_session_start(&s, &pack, "8-E1-1", 0, 0, 854));
    s.state = SESSION_PLAY; s.runtime.platform_frame = 1;
    course_place_player(&s.runtime, &s.game.player, x, 60);
    for (int i = 0; i < 40; ++i) course_session_step(&s, (GameInput){0});
    return &s.game.player;
}
static void test_triple_jump_chain(void)
{
    Player *p = tutorial_mario(300);
    bool was = p->grounded;
    unsigned stages[4] = {0}; int n = 0;
    float flip_vx = 0;
    for (int i = 0; i < 400 && n < 3; ++i) {
        GameInput in = {.horizontal = 1};
        const bool landed = p->grounded && !was; was = p->grounded;
        if (i == 60 || landed) in.jump_pressed = in.jump_held = true;
        course_session_step(&s, in);
        if (in.jump_pressed) { stages[n++] = p->jump_stage; if (p->jump_stage == 3) flip_vx = p->vx; }
    }
    for (int i = 0; i < 6; ++i) course_session_step(&s, (GameInput){.horizontal = 1});
    assert(p->jump_stage == 3 && !p->grounded); /* still the frontflip (its art) in the air */
    assert(stages[0] == 1 && stages[1] == 2 && stages[2] == 3); /* the takeoff tick no longer resets it */
    assert(flip_vx > 300);                                       /* xspeed -= (xspeed - 15) / 5 */
    /* After the frontflip the chain starts over; a late jump also starts over. */
    for (int i = 0; i < 200 && !p->grounded; ++i) course_session_step(&s, (GameInput){0});
    for (int i = 0; i < 30; ++i) course_session_step(&s, (GameInput){0}); /* > 9 frames on the ground */
    course_session_step(&s, (GameInput){.jump_pressed = true, .jump_held = true, .horizontal = 1});
    assert(p->jump_stage == 1);
}
static void test_crouch_backflip_on_slope(void)
{
    Player *p = tutorial_mario(300); /* a hill: the old stand-up check failed here */
    p->facing_left = false;
    for (int i = 0; i < 10; ++i) course_session_step(&s, (GameInput){.crouch_held = true, .vertical = 1});
    assert(p->action == ACTION_DIVE && p->grounded);
    for (int i = 0; i < 4 && p->action != ACTION_BACKFLIP; ++i)
        course_session_step(&s, (GameInput){.crouch_held = true, .vertical = 1, .horizontal = -1, .jump_pressed = i == 0,
                                            .jump_held = true});
    assert(p->action == ACTION_BACKFLIP);
    for (int i = 0; i < 6; ++i) course_session_step(&s, (GameInput){.jump_held = true});
    assert(p->action == ACTION_BACKFLIP && !p->grounded && p->vy < 0 && !p->facing_left); /* not cut off at takeoff */
    for (int i = 0; i < 40; ++i) course_session_step(&s, (GameInput){0});
    assert(p->action == ACTION_NONE); /* frame 21 or landing */
}
static void test_dive_flip_chain(void)
{
    Player *p = tutorial_mario(200);
    for (int i = 0; i < 60; ++i) course_session_step(&s, (GameInput){.horizontal = 1});
    int flips = 0, air_dives = 0;
    unsigned last = ACTION_NONE;
    for (int i = 0; i < 600 && flips < 3; ++i) {
        GameInput in = {.horizontal = 0};
        if (p->action == ACTION_NONE && p->grounded) in = (GameInput){.crouch_held = true, .vertical = 1, .horizontal = 1};
        else if (p->action == ACTION_DIVE && p->grounded && p->dive_frame >= 2) in = (GameInput){.jump_pressed = true, .jump_held = true};
        else if (!p->grounded && p->action == ACTION_NONE) in = (GameInput){.crouch_held = true, .vertical = 1};
        course_session_step(&s, in);
        if (p->action == ACTION_ROLL && last != ACTION_ROLL) ++flips;
        if (p->action == ACTION_DIVE && last == ACTION_NONE && !p->grounded) ++air_dives;
        last = p->action;
    }
    assert(flips >= 3 && air_dives >= 2); /* dive, flip, dive, flip... */
}
static void test_hud_power_meter(void)
{
    HudState h;
    hud_init(&h, 8);
    assert(hud_meter_offset(&h) == 0 && hud_pie_frame(&h) == 8);
    for (int i = 0; i < 161; ++i) hud_step(&h, 8);
    assert(hud_meter_offset(&h) == 0);           /* 160 frames at full health... */
    for (int i = 0; i < 15; ++i) hud_step(&h, 8);
    assert(hud_meter_offset(&h) == -50);         /* ...then it slides away (frame 15) */
    hud_step(&h, 7);                             /* hurt: slides back (frames 16-20) */
    for (int i = 0; i < 6; ++i) hud_step(&h, 7);
    assert(h.meter == 1 && hud_meter_offset(&h) == 0);
    for (int i = 0; i < 4; ++i) hud_step(&h, 7);
    assert(hud_pie_frame(&h) == 7);              /* pie eases 0.3 a frame to CharHP */
    hud_step(&h, 8);                             /* healed while shown: stays */
    assert(h.meter == 1 && h.count == 0);
}
/* Phase 2 item 9: zoom by stick or Wii Remote +/- (smooth, limited). */
static void test_zoom_controls(void)
{
    Player *p = tutorial_mario(300);
    const float x = p->x, y = p->y;
    assert(course_session_zoom(&s) == 1);
    float last = 1;
    for (int i = 0; i < 600; ++i) {   /* full stick up: zoom in, eased, to the limit */
        course_session_zoom_step(&s, -1, false, false);
        const float z = course_session_zoom(&s);
        assert(z >= last && z - last < 0.05f);
        last = z;
    }
    assert(fabsf(course_session_zoom(&s) - COURSE_ZOOM_MAX) < 0.01f);
    for (int i = 0; i < 900; ++i) course_session_zoom_step(&s, 1, false, false);
    assert(fabsf(course_session_zoom(&s) - COURSE_ZOOM_MIN) < 0.01f);
    course_session_zoom_step(&s, 0, false, false);
    const float rest = s.zoom_target;
    for (int i = 0; i < 60; ++i) course_session_zoom_step(&s, 0, false, false);
    assert(s.zoom_target == rest);     /* neutral keeps the zoom */
    /* Visual only: Mario and the camera's world framing are untouched. */
    assert(p->x == x && p->y == y);
}
/* Phase 2 item 8: None is in the nozzle cycle, shown by the HUD (tank hidden,
   switch banner), and the spray button then does nothing. */
static void test_none_nozzle(void)
{
    Player *p = tutorial_mario(300);
    assert(!p->has_fludd && !p->nozzles);
    ticks((GameInput){.nozzle_next_pressed = true}, 1);
    assert(!p->has_fludd && s.nozzle_banner == 0);          /* nothing to switch before FLUDD */
    p->has_fludd = true; p->nozzle = NOZZLE_HOVER; p->nozzles = (1u << NOZZLE_COUNT) - 1;
    ticks((GameInput){0}, 100);
    const unsigned char order[] = {1 + NOZZLE_ROCKET, 1 + NOZZLE_TURBO, 0, 1 + NOZZLE_HOVER};
    for (unsigned i = 0; i < 4; ++i) {
        ticks((GameInput){.nozzle_next_pressed = true}, 1);
        assert(s.nozzle_banner > 80 && s.banner_fludd == order[i]);
        ticks((GameInput){0}, 3);
    }
    ticks((GameInput){.nozzle_next_pressed = true}, 1);
    ticks((GameInput){.nozzle_next_pressed = true}, 1);
    ticks((GameInput){.nozzle_next_pressed = true}, 1);
    assert(!p->has_fludd);
    const float water = p->water, vy = p->vy;
    ticks((GameInput){.nozzle_held = true}, 60);
    assert(p->water == water && !p->jet_active && p->blast_level == 0 && p->vy >= vy - 1);
    ticks((GameInput){0}, 100);
    assert(s.nozzle_banner == 0);
}
/* Phase 2 item 2: keyboard keys in text signs become the controller's buttons. */
static void test_sign_messages_per_controller(void)
{
    assert(course_session_start(&s, &pack, "8-E1-1", 0, 0, 854));
    const CourseArea *a = s.runtime.area;
    CourseString run = {0};
    for (unsigned i = 0; i < a->object_count; ++i) {
        CourseString v;
        if (course_object_param(&a->objects[i], "signmessage", &v) && v.length > 29 &&
            !memcmp(v.text, "You can run by double tapping", 29)) run = v;
    }
    assert(run.length);
    for (unsigned k = 0; k < CONTROLLER_KINDS; ++k) {
        const CourseString m = course_sign_message(run, k);
        /* only "pressing Up and back" changes (Up is the keyboard jump) */
        const char *jump = k == CONTROLLER_CLASSIC ? "pressing B and back." : k == CONTROLLER_WIIMOTE ?
                           "pressing 2 and back." : "pressing A and back.";
        assert(m.length == run.length - 1 && !memcmp(m.text + m.length - strlen(jump), jump, strlen(jump)));
        assert(!memcmp(m.text, run.text, run.length - strlen("pressing Up and back.")));
    }
    assert(!memcmp(run.text + run.length - 21, "pressing Up and back.", 21));
    assert(course_session_start(&s, &pack, "8-E1-2-2", 0, 0, 854));
    a = s.runtime.area;
    CourseString pause = {0};
    for (unsigned i = 0; i < a->object_count; ++i) {
        CourseString v;
        if (course_object_param(&a->objects[i], "signmessage", &v) && v.length > 28 &&
            !memcmp(v.text, "Oh!  Mario!  You can pause t", 28)) pause = v;
    }
    assert(pause.length);
    CourseString m = course_sign_message(pause, CONTROLLER_GAMECUBE);
    assert(strstr(m.text, "<b>START</b>") && !strstr(m.text, "<b>P</b>"));
    m = course_sign_message(pause, CONTROLLER_CLASSIC);
    assert(strstr(m.text, "<b>PLUS</b>.  There"));
    m = course_sign_message(pause, CONTROLLER_WIIMOTE);
    assert(strstr(m.text, "<b>PLUS and MINUS</b> together.  There"));
    /* the rest of the message is the original's */
    const char *tail = ".  There, you can change settings";
    assert(strstr(m.text, tail) && strstr(pause.text, tail));
    /* other signs are untouched */
    const CourseString other = {"You can take this Warp-Pipe", 27};
    assert(course_sign_message(other, CONTROLLER_CLASSIC).text == other.text);
}
/* Phase 2 item 3: running right, the view and Mario both advance on every 60 Hz
   tick (the camera's 32 Hz moves are spread over the ticks), so Mario's place on
   screen changes smoothly instead of jumping back and forth. */
static void test_camera_moves_every_tick(void)
{
    Player *p = tutorial_mario(250);
    ticks((GameInput){.horizontal = 1}, 120);  /* up to speed, camera caught up */
    float lc = s.game.camera.x, lp = p->x, lrel = p->x - s.game.camera.x;
    for (int i = 0; i < 40; ++i) {  /* (a wall stops him a little later) */
        course_session_step(&s, (GameInput){.horizontal = 1});
        const float dc = s.game.camera.x - lc, dp = p->x - lp, rel = p->x - s.game.camera.x;
        assert(dp > 0 && dc > 0);                     /* both move every tick */
        assert(fabsf(rel - lrel) < 0.75f * dp);       /* no back-and-forth on screen */
        lc = s.game.camera.x; lp = p->x; lrel = rel;
    }
}
static void test_rocket_once_per_jump(void)
{
    /* Rocket water 1340 frames 16-23 hold Power at 0, and the Rocket needs Power 100:
       one blast per jump (the course ground kept Mario "grounded" for a tick after
       the blast, which refilled Power and allowed a second blast). */
    Player *p = tutorial_mario(300);
    p->has_fludd = true; p->nozzles = (1u << NOZZLE_COUNT) - 1; p->nozzle = NOZZLE_ROCKET;
    int blasts = 0, left = 0; float last = 0;
    for (int t = 0; t < 60 * 10; ++t) {
        ticks((GameInput){.nozzle_held = (t % 10) < 8}, 1);
        if (p->rocket_boost > last) ++blasts;
        last = p->rocket_boost;
        if (!p->grounded) left = 1;
        if (left && p->grounded) break;
    }
    assert(left && blasts == 1);
    /* Hover: no refill in the air (Fluddpow == "H"); full again on the ground. */
    p->nozzle = NOZZLE_HOVER;
    course_place_player(&s.runtime, p, 300, 60); p->vx = p->vy = 0; p->grounded = false; p->power = 30;
    ticks((GameInput){0}, 6);
    assert(!p->grounded && p->power == 30);
    for (int i = 0; i < 400 && !p->grounded; ++i) ticks((GameInput){0}, 1);
    ticks((GameInput){0}, 2);
    assert(p->grounded && p->power == 100);
}

static void test_message_box_frames(void)
{
    /* Stats.Message 253: frames 2-6 open, 6 holds with the text, 7-11 close; the prompt starts on frame 6. */
    CourseSession m = {0};
    m.state = SESSION_SIGN; m.sign_phase = 1;
    const unsigned open[] = {1, 2, 3, 4, 5, 6, 6, 6};
    for (unsigned f = 0; f < 8; ++f) { m.sign_frames = f; assert(course_message_frame(&m) == open[f]); }
    m.sign_frames = 4; assert(course_prompt_frame(&m) == -1);
    m.sign_frames = 5; assert(course_prompt_frame(&m) == 0);
    m.sign_frames = 40; assert(course_prompt_frame(&m) == 35);
    m.sign_phase = 2;
    for (unsigned f = 0; f < 6; ++f) { m.sign_frames = f; assert(course_message_frame(&m) == (f + 7 < 11 ? f + 7 : 11)); }
    assert(course_prompt_frame(&m) == -1);
    m.sign_phase = 1; m.sign_picture = true; m.sign_frames = 3;
    assert(course_message_frame(&m) == 0 && course_prompt_frame(&m) == 3);  /* picture pages have no box */
    m.state = SESSION_PLAY; assert(course_message_frame(&m) == 0 && course_prompt_frame(&m) == -1);
}

int main(void)
{
    FILE *f = fopen("../../assets/generated/course_8e1.bin", "rb"); assert(f);
    const size_t n = fread(blob, 1, sizeof(blob), f); fclose(f);
    assert(course_pack_parse(blob, n, &pack));
    test_opening_and_controls_sign(); test_hints(); test_area_chain_and_completion(); test_hud_power_meter();
    test_sign_camera_steady(); test_triple_jump_chain(); test_crouch_backflip_on_slope(); test_dive_flip_chain();
    test_zoom_controls(); test_sign_messages_per_controller(); test_none_nozzle(); test_camera_moves_every_tick();
    test_message_box_frames(); test_rocket_once_per_jump();
    puts("14 course session (Rocket once per jump, message box frames, tutorial flow, HUD, sign camera, movement, zoom, sign text, camera smoothing, None nozzle) test groups passed"); return 0;
}
