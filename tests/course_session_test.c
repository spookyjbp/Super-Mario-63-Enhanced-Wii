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
    assert(s.state == SESSION_PLAY && s.runtime.platform_frame == 0);
    /* Reading the controls sign removes the barrier (completefunction). */
    const CourseObject *controls = named("Coded Sign", "signmessage", "controls");
    stand_at(controls->x, controls->y);
    ticks((GameInput){.pound_pressed = true}, 1);
    assert(s.state == SESSION_SIGN && course_string_equals(s.sign_text, "controls"));
    ticks((GameInput){0}, 10);
    assert(s.state == SESSION_SIGN); /* stays open until Z again */
    ticks((GameInput){.pound_pressed = true}, 1);
    assert(s.state == SESSION_PLAY && s.runtime.platform_frame == 1);
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
int main(void)
{
    FILE *f = fopen("../../assets/generated/course_8e1.bin", "rb"); assert(f);
    const size_t n = fread(blob, 1, sizeof(blob), f); fclose(f);
    assert(course_pack_parse(blob, n, &pack));
    test_opening_and_controls_sign(); test_hints(); test_area_chain_and_completion();
    puts("3 course session (tutorial flow) test groups passed"); return 0;
}
