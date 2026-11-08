/* The original tutorial's scripted flow, start to completion boundary. */
#include "course_session.h"
#include "sfx.h"
#include "course_objects.h"
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
    assert(s.area_title.length && s.area_text > 0);
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
    for (int i = 0; i < 120 && s.state == SESSION_WARP; ++i) ticks((GameInput){0}, 1);   /* the wipe in: 16 source frames */
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
/* The Castle Grounds (user request 2026-11-08): entered from Intro Movie 2
   and from the tutorial's last pipe once Star[39] is set; the Sling Star
   flings Mario up to the roof; the doors and pipes to courses the port does
   not have are closed; the left pipe goes back to 8-E1-2-2; the 1-Ups. */
static void test_castle_grounds(void)
{
    static Achievements a;
    ach_init(&a, NULL);
    ach_shine(&a, 39);
    /* the tutorial's last pipe with Star[39]: changecourse("righttransition2", "Castle", -1300, 100) */
    assert(course_session_start(&s, &pack, "8-E1-2-2", 0, 0, 640));
    s.ach = &a;
    const CourseObject *pipe = named("Invisible Door", "doorplace", "C-Opening");
    stand_at(pipe->x, pipe->y + 9);
    ticks((GameInput){.crouch_held = true, .vertical = 1}, 1);
    assert(s.state == SESSION_WARP);
    for (int i = 0; i < 200 && s.state == SESSION_WARP; ++i) course_session_step(&s, (GameInput){0});
    assert(s.state == SESSION_PLAY && course_string_equals(s.runtime.area->id, "Castle"));
    assert(fabsf(course_cx(&s.runtime, s.game.player.x + PLAYER_WIDTH / 2) + 1300) < 1);
    assert(s.area_text > 0 && s.area_text_kind == 1);
    /* Intro Movie 2's StartChar("Castle", 80, 60) */
    assert(course_session_start(&s, &pack, "Castle", 80, 60, 640));
    s.ach = &a;
    ticks((GameInput){0}, 60);
    assert(s.game.player.grounded && s.state == SESSION_PLAY);
    /* the door hint shows only before Star[39] */
    assert(!hint_is("(Hint"));
    /* the castle's doors: closed */
    const CourseObject *door = named("Main Door Clip", "doorplace", "inCastle");
    assert(door);
    stand_at(door->x, door->y + 15);
    ticks((GameInput){.vertical = -1}, 2);
    assert(s.state == SESSION_PLAY && hint_is("(Closed for now"));
    ticks((GameInput){0}, 200);
    assert(!hint_is("(Closed"));
    /* the green pipe to a course not in the port (8-E3-1): closed (it stands on
       CastlePlatforms frame 1, the one below 63 stars) */
    assert(s.runtime.platform_frame == 0);
    const CourseObject *p821 = named("Invisible Door", "doorplace", "8-E3-1");
    assert(p821);
    stand_at(p821->x, p821->y - 20);
    ticks((GameInput){.crouch_held = true, .vertical = 1}, 2);
    assert(s.state == SESSION_PLAY && hint_is("(Closed for now"));
    /* the Sling Star: Mario dropped into it is held, jump spins it, it flings him up */
    const CourseObject *star = NULL;
    for (unsigned i = 0; i < s.runtime.area->object_count; ++i)
        if (course_string_equals(s.runtime.area->objects[i].type, "Sling Star BackGFX") &&
            course_session_object_active(&s, &s.runtime.area->objects[i])) star = &s.runtime.area->objects[i];
    assert(star && fabsf(star->y + 577) < 1);
    ticks((GameInput){0}, 80);                         /* its 32 waiting frames */
    course_place_player(&s.runtime, &s.game.player, star->x, star->y + 10);
    s.game.player.vx = 0; s.game.player.vy = 0; s.game.player.grounded = false;
    for (int i = 0; i < 20 && s.sling.state == SLING_NONE; ++i) course_session_step(&s, (GameInput){0});
    assert(s.sling.state == SLING_HOLD);
    ticks((GameInput){0}, 20);
    assert(s.sling.state == SLING_HOLD);              /* held: no falling */
    assert(fabsf(course_cy(&s.runtime, s.game.player.y + player_height(&s.game.player)) - star->y) < 3);
    ticks((GameInput){.jump_held = true}, 4);
    for (int i = 0; i < 60 && s.sling.state != SLING_FLING; ++i) course_session_step(&s, (GameInput){0});
    assert(s.sling.state == SLING_FLING);
    float top = 1e9f;
    for (int i = 0; i < 200; ++i) {
        course_session_step(&s, (GameInput){0});
        top = fminf(top, course_cy(&s.runtime, s.game.player.y + player_height(&s.game.player)));
    }
    fflush(stdout); fprintf(stderr, "sling star: up to %.0f (the star at %.0f, the roof's 1-Ups at -964..-844)\n", top, star->y);
    assert(top < star->y - 250);
    /* without a press it lets Mario drop after its 32 frames */
    ticks((GameInput){0}, 200);
    course_place_player(&s.runtime, &s.game.player, star->x, star->y + 10);
    s.game.player.vx = 0; s.game.player.vy = 0; s.game.player.grounded = false;
    for (int i = 0; i < 20 && s.sling.state == SLING_NONE; ++i) course_session_step(&s, (GameInput){0});
    assert(s.sling.state == SLING_HOLD);
    for (int i = 0; i < 200 && s.sling.state == SLING_HOLD; ++i) course_session_step(&s, (GameInput){0});
    assert(s.sling.state == SLING_NONE);
    /* the 1-Ups are there */
    unsigned oneups = 0;
    for (unsigned i = 0; i < s.objects.count; ++i) oneups += s.objects.live[i].kind == LIVE_ONEUP;
    assert(oneups == 3);
    /* the left pipe (warp1, with Star[39]): back to 8-E1-2-2 at (964, -783) */
    const CourseObject *w1 = named("Blank Movieclip", NULL, "warp1");
    assert(w1);
    course_place_player(&s.runtime, &s.game.player, w1->x, w1->y);
    s.game.player.vx = s.game.player.vy = 0;
    for (int i = 0; i < 300 && !(s.state == SESSION_PLAY && course_string_equals(s.runtime.area->id, "8-E1-2-2")); ++i)
        course_session_step(&s, (GameInput){0});
    assert(course_string_equals(s.runtime.area->id, "8-E1-2-2"));
    s.ach = NULL;
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
    for (int i = 0; i < 161; ++i) hud_step(&h, 8, 8);
    assert(hud_meter_offset(&h) == 0);           /* 160 frames at full health... */
    for (int i = 0; i < 15; ++i) hud_step(&h, 8, 8);
    assert(hud_meter_offset(&h) == -50);         /* ...then it slides away (frame 15) */
    hud_step(&h, 7, 8);                             /* hurt: slides back (frames 16-20) */
    for (int i = 0; i < 6; ++i) hud_step(&h, 7, 8);
    assert(h.meter == 1 && hud_meter_offset(&h) == 0);
    for (int i = 0; i < 4; ++i) hud_step(&h, 7, 8);
    assert(hud_pie_frame(&h) == 7);              /* pie eases 0.3 a frame to CharHP */
    hud_step(&h, 8, 8);                             /* healed while shown: stays */
    assert(h.meter == 1 && h.count == 0);
    /* Under water (WaterHP 7) with full HP: the water pie joins (frames 56-65,
       "both"), the HP pie then leaves ("hpaway" to 45: water only); back at 8
       the water pie leaves (46-55) and the HP meter is hidden at 15. */
    float x, y;
    hud_init(&h, 8);
    for (int i = 0; i < 20; ++i) hud_step(&h, 8, 7);
    assert(h.meter == 45 && hud_water_place(&h, &x, &y) && x == 0 && y == 0 && !hud_hp_place(&h, &x, &y));
    for (int i = 0; i < 40; ++i) hud_step(&h, 8, 3);
    assert(hud_water_frame(&h) == 3);
    for (int i = 0; i < 12; ++i) hud_step(&h, 8, 8);
    assert(h.meter == 15 && !hud_water_place(&h, &x, &y) && hud_meter_offset(&h) == -50);
    /* hurt with the water meter shown: both, side by side */
    hud_init(&h, 8);
    for (int i = 0; i < 20; ++i) hud_step(&h, 8, 6);
    for (int i = 0; i < 12; ++i) hud_step(&h, 5, 6);
    assert(h.meter == 65 && hud_hp_place(&h, &x, &y) && x == 30 && hud_water_place(&h, &x, &y) && x == -30);
}
/* Phase 2 item 9: zoom by stick or Wii Remote +/- (smooth, limited). */
static void test_zoom_controls(void)
{
    /* The original's zoom: KeyPlus / KeyMinus change CameraZoom by 2 a frame,
       clamped to Maxzoomout 50 .. Maxzoomin 200; the view (CameraZoom2) follows
       a twentieth of the rounded difference a frame. */
    Player *p = tutorial_mario(300);
    assert(course_session_zoom(&s) == 1);
    float last = 1;
    for (int i = 0; i < 400; ++i) {   /* hold zoom in: 32 source frames a second */
        course_session_zoom_step(&s, -1, false, true, 1);
        const float z = course_session_zoom(&s);
        assert(z >= last && z - last < 0.06f);
        last = z;
    }
    assert(s.camera_zoom == 200 && fabsf(course_session_zoom(&s) - 2.0f) < 0.11f);
    course_session_zoom_step(&s, -1, false, true, 1);
    assert(s.camera_zoom == 200);                                  /* Maxzoomin */
    for (int i = 0; i < 400; ++i) course_session_zoom_step(&s, 1, false, true, 1);
    assert(s.camera_zoom == 50 && fabsf(course_session_zoom(&s) - 0.5f) < 0.11f);   /* Maxzoomout */
    /* 2 a frame: from 100, 25 frames in is 150 */
    s.camera_zoom = s.zoom2 = s.zoom2_from = 100;
    for (int i = 0; i < 25; ++i) course_session_zoom_step(&s, -1, false, true, 1);
    assert(s.camera_zoom == 150);
    const float rest = s.camera_zoom;
    for (int i = 0; i < 60; ++i) course_session_zoom_step(&s, 0, false, true, 1);
    assert(s.camera_zoom == rest);     /* neutral keeps the zoom */
    /* Between source frames the view moves smoothly from the last frame's value. */
    s.zoom2_from = 100; s.zoom2 = 110;
    course_session_zoom_step(&s, 0, false, false, 0.5f);
    assert(fabsf(course_session_zoom(&s) - 1.05f) < 1e-4f);
    /* Toad's talk: CameraZoom to 150 (capped), then back to 100 after the talk. */
    s.camera_zoom = s.zoom2 = 80;
    for (int i = 0; i < 40; ++i) course_session_zoom_step(&s, 0, true, true, 1);
    assert(s.camera_zoom == 150 && s.talk_zoomed);
    for (int i = 0; i < 40; ++i) course_session_zoom_step(&s, 0, false, true, 1);
    assert(s.camera_zoom == 100 && !s.talk_zoomed);
    s.camera_zoom = 190;                               /* already in past 150: capped, not restored */
    course_session_zoom_step(&s, 0, true, true, 1);
    assert(s.camera_zoom == 150 && !s.talk_zoomed);
    /* Zoomed out, the camera keeps the wider view inside CamEdge: the view's
       half height in course px doubles, so the camera centre moves away from the
       area's bottom edge. */
    s.camera_zoom = s.zoom2 = s.zoom2_from = 100; s.zoom = 1;
    course_camera_set_zoom(1);
    ticks((GameInput){0}, 120);
    const float cy1 = s.game.camera.cy;
    s.camera_zoom = s.zoom2 = s.zoom2_from = 50;
    ticks((GameInput){0}, 120);
    assert(fabsf(course_session_zoom(&s) - 0.5f) < 1e-4f);
    assert(s.game.camera.cy < cy1 - 10);
    (void)p;
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
    {   /* Port rule: rising through a one-way platform (BPlatforms) after a blast
           does not refill Power (the original's air = false did); landing does. */
        const CourseRuntime *c = &s.runtime;
        float bx = -1, by = -1;
        for (float cy = -1500; cy < 2500 && bx < 0; cy += 1)
            for (float cx = -1500; cx < 6000 && bx < 0; cx += 4)
                if (course_bsolid(c, cx, cy) && !course_bsolid(c, cx, cy - 1) && !course_solid(c, cx, cy)) {
                    float bottom = cy;   /* through its thickness */
                    while (bottom < cy + 80 && course_bsolid(c, cx, bottom + 1)) bottom += 1;
                    bool clear = bottom < cy + 80 && bottom >= cy + 16;   /* thick enough to touch; room below */
                    for (float k = bottom - cy + 1; k <= bottom - cy + 50 && clear; ++k)
                        for (float dx = -10; dx <= 10 && clear; dx += 5)
                            clear = !course_solid(c, cx + dx, cy + k) && !course_bsolid(c, cx + dx, cy + k) &&
                                    !course_solid(c, cx + dx, cy - k + bottom - cy - 80);
                    if (clear) bx = cx, by = bottom;
                }
        assert(bx >= 0);
        course_place_player(c, p, bx, by + 45);
        p->grounded = false; p->vx = 0; p->vy = -18 * 44; p->power = 0; p->rocket_spent = true; p->rocket_frame = 1;
        bool touched = false;
        for (int i = 0; i < 40 && p->vy < 0; ++i) {
            ticks((GameInput){0}, 1);
            if (p->grounded && p->vy < 0) touched = true;
            assert(p->power < 100);            /* still rising: no refill */
        }
        assert(touched);
        for (int i = 0; i < 400 && !(p->grounded && p->vy >= 0); ++i) ticks((GameInput){0}, 1);
        ticks((GameInput){0}, 2);
        assert(p->grounded && p->power == 100 && !p->rocket_spent);
    }
    /* Hover: no refill in the air (Fluddpow == "H"); full again on the ground. */
    p->nozzle = NOZZLE_HOVER;
    course_place_player(&s.runtime, p, 300, 60); p->vx = p->vy = 0; p->grounded = false; p->power = 30;
    ticks((GameInput){0}, 6);
    assert(!p->grounded && p->power == 30);
    for (int i = 0; i < 400 && !p->grounded; ++i) ticks((GameInput){0}, 1);
    ticks((GameInput){0}, 2);
    assert(p->grounded && p->power == 100);
}

static void test_idle_and_sleep(void)
{
    /* Mario (None) Walk.xml: standing still from the clip's start (count NaN)
       shows Idle (Idle Animation, 978 frames: "Lets go" at 385 ... "Yawn" at
       908), which ends in Sleep; Sleep stops on frame 11 snoring until a key,
       then plays to 31. At CharHP <= 3: Idle2 (Tired Animation, 37 frames, 12
       loops of 4..37). The original has no idle with FLUDD (its FLUDD walk
       clips); the port plays the same idle with FLUDD on (user request). */
    Player *p = tutorial_mario(300);
    p->has_fludd = false;
    ticks((GameInput){.horizontal = 1}, 20);
    ticks((GameInput){0}, 40);
    assert(p->grounded && p->idle_kind == 0);                  /* moving: count from 0 */
    ticks((GameInput){0}, 60 * 3);
    assert(p->idle_kind == 1);                                 /* 92 still frames */
    while (!p->sleeping) ticks((GameInput){0}, 1);
    assert(p->idle_kind == 0 && p->sleep_frame == 0);
    ticks((GameInput){0}, 60 * 4);
    assert(p->sleeping && p->sleep_frame == 10);               /* snoring */
    const float x = p->x;
    ticks((GameInput){.horizontal = 1}, 2);                    /* wakes; no walking while getting up */
    assert(p->sleeping && p->sleep_frame > 10 && fabsf(p->x - x) < 1);
    ticks((GameInput){0}, 60);
    assert(!p->sleeping);
    /* jump and land standing still: the new walk clip goes to Idle at once */
    ticks((GameInput){.jump_pressed = true, .jump_held = true}, 2);
    ticks((GameInput){0}, 90);
    assert(p->grounded && p->idle_kind == 1);
    /* low HP: Tired */
    s.progress.hp = 3;
    ticks((GameInput){0}, 4);
    assert(p->idle_kind == 2);
    /* with FLUDD: the same idle, and it still ends in Sleep */
    p = tutorial_mario(300);
    p->has_fludd = true;
    p->nozzle = NOZZLE_HOVER;
    ticks((GameInput){.horizontal = 1}, 20);
    ticks((GameInput){0}, 40);
    assert(p->grounded && p->idle_kind == 0);
    ticks((GameInput){0}, 60 * 3);
    assert(p->idle_kind == 1 && p->has_fludd);
    ticks((GameInput){.nozzle_held = true}, 2);                /* FLUDD held: like moving */
    assert(p->idle_kind == 0);
    ticks((GameInput){0}, 60 * 4);
    assert(p->grounded && p->idle_kind == 1);
    while (!p->sleeping) ticks((GameInput){0}, 1);
    assert(p->has_fludd && p->sleep_frame == 0);
    s.progress.hp = 8;
}
static void test_drowning(void)
{
    /* Head under water: every 81st frame a bubble, "sound - Drain", WaterHP - 1;
       at 0 attackFrame "Drown" (Mario held still), frame 76 Blackout, 88
       LifeLost, then the revive with WaterHP 8. Out of the water it heals 1
       every 9th frame; the HUD shows the water pie. */
    Player *p = tutorial_mario(300);
    course_place_player(&s.runtime, p, 560, 280);   /* 8-E1-1's middle pool (floor at ~316) */
    s.progress.water_hp = 2;
    bool bubble = false, drown = false, black = false;
    for (int i = 0; i < 60 * 20 && !s.lifelost; ++i) {
        ticks((GameInput){0}, 1);
        for (unsigned k = 0; k < s.fx_count; ++k) bubble |= s.fx[k].kind == COURSE_FX_BUBBLE;
        if (p->drowning) { drown = true; assert(s.progress.water_hp == 0 && p->vx == 0 && p->vy == 0); }
        if (s.blackout) { black = true; assert(p->drown_frame >= 75); }
    }
    assert(bubble && drown && black && s.lifelost);
    float hx, hy;
    assert(hud_water_place(&s.hud, &hx, &hy));
    for (int i = 0; i < 60 * 6 && !s.starin; ++i) ticks((GameInput){0}, 1);
    assert(s.starin && !p->drowning && s.progress.water_hp == 8);
    /* healing on land */
    p = tutorial_mario(300);
    s.progress.water_hp = 5;
    ticks((GameInput){0}, 60);   /* 32 frames: 3 heals */
    assert(s.progress.water_hp == 8);
}
static float gf(float v, float sub, float div) { return copysignf(fmaxf(0, fabsf(v) - sub) / div, v); }
static void test_fludd_exact(void)
{
    /* Hover in the air, frame by frame as CharCode: gravity (+1, the fall drag,
       /1.001), then yspeed *= 0.8; yspeed -= (9.2 - yspeed) / 10 * (Power / 175 +
       0.75); Power -= 1.5; WaterAmount -= 7 (of 10000); xspeed groundFriction(0.05, 1.03). */
    Player *p = tutorial_mario(300);
    p->has_fludd = true; p->nozzle = NOZZLE_HOVER; p->nozzles = 7; p->water = 100; p->power = 100;
    course_place_player(&s.runtime, p, 300, -250);
    p->vy = 4 * 44; p->vx = 6 * 44; p->grounded = false;
    float ys = 4, xs = 6, power = 100, water = 100;
    unsigned frames = 0;
    for (int i = 0; i < 60 && frames < 20; ++i) {
        ticks((GameInput){.nozzle_held = true}, 1);
        if (!p->source_step) continue;
        ++frames;
        ys += 1; if (ys > 0) ys = gf(ys, 0.2f, 1.05f);
        ys = gf(ys, 0, 1.001f); xs = gf(xs, 0, 1.001f);
        ys *= 0.8f; ys -= (9.2f - ys) / 10 * (power / 175 + 0.75f);
        power -= 1.5f; water -= 0.07f; xs = gf(xs, 0.05f, 1.03f);
        assert(fabsf(p->vy / 44 - ys) < 1e-3f && fabsf(p->vx / 44 - xs) < 1e-3f);
        assert(fabsf(p->power - power) < 1e-3f && fabsf(p->water - water) < 1e-3f);
    }
    assert(frames == 20 && p->vy < 0);   /* rising */
    /* Holding Right while hovering tilts Mario (+5, then groundFriction(3, 1.3): ~6.7 degrees) */
    for (int i = 0; i < 30; ++i) ticks((GameInput){.nozzle_held = true, .horizontal = 1}, 1);
    assert(p->char_rotation > 5 && p->char_rotation < 8);
    /* Rocket: frames 1-9 charge while held and run back when let go; from 10 it
       goes on by itself (yspeed *= 0.97); 16 blasts: yspeed = min(-25 + max(yspeed / 3, 0), yspeed) */
    p = tutorial_mario(300);
    p->has_fludd = true; p->nozzle = NOZZLE_ROCKET; p->power = 100; p->water = 100;
    unsigned held = 0;
    while (held < 5) { ticks((GameInput){.nozzle_held = true}, 1); held += p->source_step; }
    assert(p->rocket_frame == 6);
    unsigned back = 0;
    while (back < 2) { ticks((GameInput){0}, 1); back += p->source_step; }
    assert(p->rocket_frame == 4);
    while (p->rocket_frame < 10) ticks((GameInput){.nozzle_held = true}, 1);
    unsigned more = 0;
    while (p->rocket_frame != 16) { ticks((GameInput){0}, 1); more += p->source_step; }   /* let go: it still fires */
    assert(more == 6 && p->vy <= -24 * 44 && p->power == 0 && fabsf(p->water - 95) < 0.01f);
    /* one blast per jump: Power stays 0 in the air, refills on landing */
    while (p->rocket_frame != 1) ticks((GameInput){0}, 1);
    for (int i = 0; i < 60 && !p->grounded; ++i) {
        ticks((GameInput){.nozzle_held = (i / 4) % 2 == 0}, 1);
        assert(p->power < 100 && (p->rocket_frame < 10 || p->rocket_frame == 1) && p->rocket_boost == 0);
    }
    while (!p->grounded) ticks((GameInput){0}, 1);
    ticks((GameInput){0}, 2);
    assert(p->power == 100);
    /* Triple jump with FLUDD: the flip plays (Hfrontflip, an attack), and the
       Hover pushes along DiveRot = the flip's frame rotation + 90 (frame 4:
       -87.2 + 90) instead of hovering: yspeed *= 0.98; yspeed -= sin(DiveRot) * 0.92;
       xspeed *= 0.97; xspeed -= cos(DiveRot) * 0.46 (facing left). */
    Player q = {0};
    q.has_fludd = true; q.nozzle = NOZZLE_HOVER; q.power = 100; q.water = 100; q.facing_left = true;
    q.jump_stage = 3; q.jump_time = 3.5f / 32; q.vx = -10 * 44; q.vy = -12 * 44; q.source_step = true;
    assert(!player_hover_active(&q, (GameInput){.nozzle_held = true}));
    player_fludd_step(&q, (GameInput){.nozzle_held = true});
    const float r = 2.8f * 0.01745329252f;
    assert(fabsf(q.dive_rot - 2.8f) < 1e-3f);
    assert(fabsf(q.vy / 44 - (-12 * 0.98f - sinf(r) * 0.92f)) < 1e-3f);
    assert(fabsf(q.vx / 44 - (-10 * 0.97f - cosf(r) * 0.46f)) < 1e-3f);
}
static bool heard(SfxId id)
{
    SfxEvent ev[SFX_QUEUE];
    const unsigned n = sfx_drain(ev);
    bool hit = false;
    for (unsigned i = 0; i < n; ++i) hit |= ev[i].id == id;
    return hit;
}
static void test_fall_sound(void)
{
    /* "Mario-Fall" as CharCode: from rest, yspeed (gravity 1, fall drag 0.2 /
       1.05, then / 1.001) reaches the frames with fallingyspeed >= 15 and
       yspeed > 15; the fifth plays it, once. Reference: the original frames. */
    float ys = 0; unsigned count = 0, want = 0;
    for (unsigned f = 1; f < 200 && !want; ++f) {
        ys += 1;
        const float falling = gf(ys, 0.2f, 1.05f);
        if (ys > 7 && falling >= 15 && ys > 15 && ++count >= 5) want = f;
        ys = gf(gf(ys, 0.2f, 1.05f), 0, 1.001f);
    }
    assert(want > 20);
    Player *p = tutorial_mario(300);
    p->has_fludd = false;
    course_place_player(&s.runtime, p, 300, -2500);
    p->vy = 0; p->grounded = false;
    heard(SFX_FALL);
    unsigned frames = 0, at = 0, plays = 0;
    for (int i = 0; i < 60 * 4 && !p->grounded; ++i) {
        ticks((GameInput){0}, 1);
        frames += p->source_step;
        if (heard(SFX_FALL)) { ++plays; if (!at) at = frames; }
    }
    assert(at == want && plays == 1);
    /* a jump's fall is too short: no sound */
    p = tutorial_mario(300);
    heard(SFX_FALL);
    ticks((GameInput){.jump_pressed = true, .jump_held = true}, 1);
    bool any = false;
    for (int i = 0; i < 120; ++i) { ticks((GameInput){.jump_held = true}, 1); any |= heard(SFX_FALL); }
    assert(!any);
}
static void test_faint(void)
{
    /* hurt1 at 0 HP: frame 4 adds the faint speeds and goes to 'falling' (10),
       the ground sends it to 'land' (55); 95 Blackout, 107 LifeLost; the lives
       screen plays 59 frames (the course removed), then ReviveMario: the area's
       start, 8 HP, the StarIn wipe. */
    Player *p = tutorial_mario(300);
    s.progress.hp = 1;
    course_mario_hit(p, &s.progress, 1, 64, -6, 3, 0, false);
    assert(s.progress.hp == 0 && p->fainting && p->action == ACTION_HURT && p->hurt_kind == 1);
    bool fell = false, landed = false, blacked = false, lost = false;
    for (int i = 0; i < 60 * 12 && !s.starin; ++i) {
        ticks((GameInput){.jump_pressed = true, .horizontal = 1}, 1);   /* no control while fainting */
        if (p->action == ACTION_HURT && p->hurt_frame >= 9 && p->hurt_frame < 54) fell = true;
        if (p->action == ACTION_HURT && p->hurt_frame >= 54) landed = true;
        if (s.blackout) { blacked = true; assert(p->hurt_frame >= 94 || s.lifelost); }
        if (s.lifelost) lost = true;
    }
    assert(fell && landed && blacked && lost && s.starin);
    assert(s.progress.hp == 8 && !p->fainting && p->action == ACTION_NONE && !s.lifelost);
    /* hurt3 at 0 HP: frame 16 goes to 'end' (24), 70 LifeLost */
    p = tutorial_mario(300);
    s.progress.hp = 1;
    p->facing_left = true;
    course_mario_hit(p, &s.progress, 1, 64, -2, -6, 0, true);   /* knocked the way he faces: hurt3 */
    assert(p->fainting && p->action == ACTION_HURT && p->hurt_kind == 3);
    bool end = false;
    for (int i = 0; i < 60 * 12 && !s.lifelost; ++i) { ticks((GameInput){0}, 1); if (p->hurt_frame >= 23) end = true; }
    assert(end && s.lifelost);
    /* 0 HP without a hurt clip: "Faint", MarioDeathGFX for 60 frames, then the lives screen */
    p = tutorial_mario(300);
    s.progress.hp = 0;
    ticks((GameInput){0}, 2);
    assert(p->fainting && s.death);
    for (int i = 0; i < 60 * 4 && !s.lifelost; ++i) ticks((GameInput){0}, 1);
    assert(s.lifelost && !s.death);
}

static void test_wipes_and_background_water(void)
{
    /* righttransition: warpframe on its frame 16, then righttransition2 (17 frames) in the new area */
    assert(course_session_start(&s, &pack, "8-E1-1", 0, 0, 640));
    s.state = SESSION_PLAY; s.runtime.platform_frame = 1;
    const CourseObject *w = named("Blank Movieclip", NULL, "warp1");
    float b[4];
    course_object_bounds(w, COURSE_BLANK_U0, COURSE_BLANK_V0, COURSE_BLANK_U1, COURSE_BLANK_V1, b);
    course_place_player(&s.runtime, &s.game.player, (b[0] + b[2]) / 2, (b[1] + b[3]) / 2);
    ticks((GameInput){0}, 1);
    assert(s.state == SESSION_WARP && s.wipe == WIPE_RIGHT && s.wipe_in == 1);
    unsigned last = 0;
    while (s.state == SESSION_WARP) { last = s.wipe_in; ticks((GameInput){0}, 1); }
    assert(last == 16 && s.wipe_out >= 1 && course_string_equals(s.runtime.area->id, "8-E1-1-2"));
    unsigned shown = 0;
    while (s.wipe_out) { shown = s.wipe_out; ticks((GameInput){0}, 1); }
    assert(shown == 17);
    /* 8-E1-2-2 warp3 (falling off the clouds): downtransition back to 8-E1-2 above where Mario fell */
    assert(course_session_start(&s, &pack, "8-E1-2-2", 0, 0, 640));
    s.state = SESSION_PLAY; s.runtime.platform_frame = 1;
    w = named("Blank Movieclip", NULL, "warp3");
    assert(w);
    course_object_bounds(w, COURSE_BLANK_U0, COURSE_BLANK_V0, COURSE_BLANK_U1, COURSE_BLANK_V1, b);
    course_place_player(&s.runtime, &s.game.player, -400 > b[0] && -400 < b[2] ? -400 : (b[0] + b[2]) / 2, (b[1] + b[3]) / 2);
    const float fx = course_cx(&s.runtime, s.game.player.x + PLAYER_WIDTH / 2);
    ticks((GameInput){0}, 1);
    assert(s.state == SESSION_WARP && s.wipe == WIPE_DOWN);
    while (s.state == SESSION_WARP) ticks((GameInput){0}, 1);
    assert(course_string_equals(s.runtime.area->id, "8-E1-2"));
    assert(fabsf(s.warp_x - fminf(1800, fmaxf(400, 2000 + fx))) < 0.01f);
    /* 8-E1-2's CloseBackground holds a Dynamic Water (polygons 13, 10 at "MEDIUM"): simulated, its ends fixed */
    assert(s.bg_water_count == 1);
    const BgWave *wave = &s.bg_wave[0];
    const float x0 = wave->x[0], xn = wave->x[wave->n - 1];
    float moved = 0;
    for (int i = 0; i < 300; ++i) { ticks((GameInput){0}, 1); for (unsigned k = 0; k < wave->n; ++k) moved = fmaxf(moved, fabsf(wave->y[k])); }
    assert(wave->n == 10 && wave->x[0] == x0 && wave->x[wave->n - 1] == xn && moved > 0.01f && moved < 60);
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

static float wave_peak(const BgWave *w)
{
    float m = 0;
    for (unsigned i = 0; i < w->n; ++i) m = fmaxf(m, fabsf(w->y[i]));
    return m;
}
static void test_level_water_splash(void)
{
    /* 8-E1-1's middle pool (Water Code at 448): Mario dropping in faster than
       5 px a frame stirs the strip (yspeed / 2 at his point); without him it
       only ripples. */
    float peak[2];
    for (int k = 0; k < 2; ++k) {
        assert(course_session_start(&s, &pack, "8-E1-1", 0, 0, 640));
        s.state = SESSION_PLAY; s.runtime.platform_frame = 1;
        assert(s.water_count == 3 && s.water_wave[1].n == 30);
        for (int i = 0; i < 260; ++i) course_session_step(&s, (GameInput){0});
        if (k) {
            course_place_player(&s.runtime, &s.game.player, 640, 120);
            s.game.player.vy = 9 * 44; s.game.player.grounded = false;
        }
        peak[k] = 0;
        for (int i = 0; i < 60; ++i) { course_session_step(&s, (GameInput){0}); peak[k] = fmaxf(peak[k], wave_peak(&s.water_wave[1])); }
    }
    assert(peak[1] > peak[0] + 2);
}
static void test_camera_starts_inside_walls(void)
{
    /* 8-E1-2 entered from the left: Mario at x 0, the left wall (CamEdge
       -240..-40) closer than the screen's edge. The camera starts at the
       wall's inside, as the original settles once Mario moves, not showing
       the empty background past the area. */
    for (int w = 0; w < 2; ++w) {
        assert(course_session_start(&s, &pack, "8-E1-2", 0, 0, w ? 854 : 640));
        assert(s.game.camera.cx > 100);   /* was 0: the view reached 400 px past the wall */
        const float start = s.game.camera.cx;
        for (int i = 0; i < 12; ++i) course_session_step(&s, (GameInput){.horizontal = -1});   /* before the warp back */
        assert(s.state == SESSION_PLAY);
        assert(fabsf(s.game.camera.cx - start) <= 2);   /* already where the wall holds it */
    }
}

/* The original pause menu (795 / 794) and the achievement card */
static void test_pause_menu(void)
{
    Player *p = tutorial_mario(300);
    ticks((GameInput){0}, 30);
    const float x = p->x, y = p->y;
    GameInput start = {0}; start.pause_pressed = true;
    GameInput right = {0}; right.horizontal = 1; right.jump_held = true;
    ticks(start, 1);
    assert(pause_active(&s.pause) && s.game.paused);
    /* 795 frames 2..15 at 32 frames a second: open after about 25 ticks */
    ticks((GameInput){0}, 26);
    assert(s.pause.frame == PAUSE_OPEN_FRAMES);
    float dy, al; pause_clip_look(&s.pause, &dy, &al);
    assert(dy == 0 && al == 1);
    /* the game stands still underneath, even with Mario's buttons held */
    ticks(right, 60);
    assert(p->x == x && p->y == y && p->previous_x == p->x);
    ticks((GameInput){0}, 30);   /* (the held Right opened Star Map) */
    /* the column: Down from Star Map to Fludd, Options, Achievements; Up wraps */
    s.pause.focus = PI_STARMAP; s.pause.column = PI_STARMAP; s.pause.page = PAUSE_DEFAULT; s.pause.trans = 0;
    GameInput d = {0}; d.vertical = 1;
    GameInput u = {0}; u.vertical = -1;
    GameInput r = {0}; r.horizontal = 1;
    GameInput l = {0}; l.horizontal = -1;
    ticks(u, 1); ticks((GameInput){0}, 1);
    assert(s.pause.focus == PI_EXIT);
    ticks(d, 1); ticks((GameInput){0}, 1);
    assert(s.pause.focus == PI_STARMAP);
    for (int i = 0; i < 3; ++i) { ticks(d, 1); ticks((GameInput){0}, 1); }
    assert(s.pause.focus == PI_ACHIEVEMENTS && s.pause.page == PAUSE_DEFAULT);   /* moving does not change the page */
    /* jump opens its page through 661 (the page changes on its frame 8) and steps into the list */
    GameInput jump = {0}; jump.jump_pressed = jump.confirm_pressed = true;
    ticks(jump, 1);
    assert(s.pause.trans && s.pause.page == PAUSE_DEFAULT);
    ticks((GameInput){0}, 30);
    assert(!s.pause.trans && s.pause.page == PAUSE_ACHIEVEMENTS && s.pause.focus == PI_LIST);
    /* five rows: the list scrolls past its last row */
    for (int i = 0; i < 6; ++i) { ticks(d, 1); ticks((GameInput){0}, 1); }
    assert(s.pause.list_sel == 6 && s.pause.list_top == 2);
    /* Left: back to the column (the page stays); Up to Options, Right opens it at Mute Music */
    ticks(l, 1); ticks((GameInput){0}, 1);
    assert(s.pause.focus == PI_ACHIEVEMENTS && s.pause.page == PAUSE_ACHIEVEMENTS);
    ticks(u, 1); ticks((GameInput){0}, 1); ticks(r, 1); ticks((GameInput){0}, 30);
    assert(s.pause.page == PAUSE_OPTIONS && s.pause.focus == PI_MUTE && s.pause.column == PI_OPTIONS);
    /* jump on Mute Music: muted, the music stops (no song requested) */
    ticks(jump, 1);
    assert(s.pause.muted && sfx_music_muted());
    ticks(jump, 1);
    assert(!s.pause.muted && !sfx_music_muted());
    /* Star Map: its rows are a list; Down scrolls it, the list starts at its top */
    GameInput spin = {0}; spin.spin_pressed = spin.cancel_pressed = true;
    ticks(spin, 1);
    assert(s.pause.focus == PI_OPTIONS);
    s.pause.focus = PI_STARMAP; s.pause.column = PI_STARMAP;
    ticks(jump, 1); ticks((GameInput){0}, 30);
    assert(s.pause.page == PAUSE_STARMAP && s.pause.focus == PI_LIST && s.pause.list_sel == 0);
    for (int i = 0; i < 40; ++i) { ticks(d, 1); ticks((GameInput){0}, 1); }
    assert(s.pause.list_sel == PAUSE_STARMAP_ROWS - 1 && s.pause.list_top == PAUSE_STARMAP_ROWS - PAUSE_LIST_ROWS);
    ticks(l, 1); ticks((GameInput){0}, 1);
    assert(s.pause.focus == PI_STARMAP);
    /* Fludd (only Hover owned): the rows go None, Hover; the locked ones are skipped */
    s.game.player.nozzles = 1u << NOZZLE_HOVER;
    ticks(d, 1); ticks((GameInput){0}, 1); ticks(jump, 1); ticks((GameInput){0}, 30);
    assert(s.pause.page == PAUSE_FLUDD && s.pause.focus == PI_NOZ_NONE);
    ticks(d, 1); ticks((GameInput){0}, 1);
    assert(s.pause.focus == PI_NOZ_HOVER);
    ticks(d, 1); ticks((GameInput){0}, 1);
    assert(s.pause.focus == PI_NOZ_HOVER);
    ticks(u, 1); ticks((GameInput){0}, 1);
    assert(s.pause.focus == PI_NOZ_NONE);
    ticks(spin, 1);
    assert(s.pause.focus == PI_FLUDD);
    /* Exit: Exit Level cannot be chosen (no Castle Grounds in the port): Quit Game first */
    s.pause.focus = PI_EXIT; s.pause.column = PI_EXIT;
    ticks(r, 1); ticks((GameInput){0}, 30);
    assert(s.pause.page == PAUSE_EXIT && s.pause.focus == PI_QUIT);
    ticks(u, 1); ticks((GameInput){0}, 1);
    assert(s.pause.focus == PI_QUIT);
    ticks(spin, 1);
    assert(s.pause.focus == PI_EXIT);
    /* spin in the column: back to PAUSED; again: closes (795 backwards), then the game runs */
    ticks(spin, 1); ticks((GameInput){0}, 30);
    assert(s.pause.page == PAUSE_DEFAULT);
    ticks(spin, 1); ticks((GameInput){0}, 30);
    assert(!pause_active(&s.pause) && !s.game.paused);
    ticks(r, 20);
    assert(p->x > x);
    /* START during a death does not pause, and the old menu never sees it */
    s.death = 5; ticks(start, 1); assert(!pause_active(&s.pause) && !s.game.paused); s.death = 0;
}
static void test_achievement_card(void)
{
    static Achievements a;
    tutorial_mario(300);
    ach_init(&a, NULL);
    s.ach = &a;
    SfxEvent ev[SFX_QUEUE]; sfx_drain(ev);
    /* two at once (Star[39] and a forced one): one card after the other, the
       ping once per card, never pausing the game */
    ach_shine(&a, 39); ach_force(&a, 30);
    ticks((GameInput){0}, 1);
    assert(s.notice.id == 1 && !s.game.paused);
    unsigned pings = 0, n = sfx_drain(ev);
    for (unsigned i = 0; i < n; ++i) pings += ev[i].id == SFX_ACHIEVEMENT;
    assert(pings == 1);
    ticks((GameInput){0}, ACH_NOTICE_IN + ACH_NOTICE_HOLD / 2);
    assert(ach_notice_slide(&s.notice) == 1);
    ticks((GameInput){0}, ACH_NOTICE_TICKS);
    assert(s.notice.id == 30);
    n = sfx_drain(ev); pings = 0;
    for (unsigned i = 0; i < n; ++i) pings += ev[i].id == SFX_ACHIEVEMENT;
    assert(pings == 1);
    ticks((GameInput){0}, ACH_NOTICE_TICKS);
    assert(s.notice.id == 0 && ach_notice_slide(&s.notice) == 0);
    s.ach = NULL;
}

int main(void)
{
    FILE *f = fopen("../../assets/generated/course_8e1.bin", "rb"); assert(f);
    const size_t n = fread(blob, 1, sizeof(blob), f); fclose(f);
    assert(course_pack_parse(blob, n, &pack));
    test_opening_and_controls_sign(); test_hints(); test_area_chain_and_completion(); test_hud_power_meter();
    test_sign_camera_steady(); test_triple_jump_chain(); test_crouch_backflip_on_slope(); test_dive_flip_chain();
    test_zoom_controls(); test_sign_messages_per_controller(); test_none_nozzle(); test_camera_moves_every_tick();
    test_message_box_frames(); test_rocket_once_per_jump(); test_faint(); test_wipes_and_background_water();
    test_camera_starts_inside_walls(); test_level_water_splash(); test_idle_and_sleep(); test_drowning(); test_fludd_exact(); test_fall_sound();
    test_pause_menu(); test_achievement_card(); test_castle_grounds();
    puts("25 course session (Castle Grounds, pause menu, achievement card, fall sound, FLUDD frame by frame, idle/sleep, drowning, level water splash, camera inside walls on entry, wipes, background water, faint, Rocket once per jump, message box frames, tutorial flow, HUD, sign camera, movement, zoom, sign text, camera smoothing, None nozzle) test groups passed"); return 0;
}
