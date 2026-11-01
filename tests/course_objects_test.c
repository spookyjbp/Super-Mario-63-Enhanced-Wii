/* Original tutorial objects: coins, Toad dialogue, falling log, birds. */
#include "course_session.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static unsigned char blob[1 << 20];
static CoursePack pack;
static CourseSession s;

static void ticks(GameInput in, int n) { for (int i = 0; i < n; ++i) course_session_step(&s, in); }
static LiveObject *first(LiveKind k)
{
    for (unsigned i = 0; i < s.objects.count; ++i) if (s.objects.live[i].kind == k && s.objects.live[i].alive) return &s.objects.live[i];
    return NULL;
}
static unsigned alive(LiveKind k)
{
    unsigned n = 0;
    for (unsigned i = 0; i < s.objects.count; ++i) n += s.objects.live[i].kind == k && s.objects.live[i].alive;
    return n;
}
static void play_area(const char *id)
{
    assert(course_session_start(&s, &pack, "8-E1-1", 0, 0, 640));
    s.state = SESSION_PLAY; s.runtime.platform_frame = 1;
    if (strcmp(id, "8-E1-1")) assert(course_session_start(&s, &pack, id, 0, 0, 640));
}
static void test_coins_heal_and_persist(void)
{
    play_area("8-E1-1");
    assert(alive(LIVE_COIN) == 18 && alive(LIVE_BLUE_COIN) == 1 && s.progress.hp == 8);
    s.progress.hp = 6; /* hurt by 2 */
    LiveObject *c = first(LIVE_COIN);
    const float cx = c->x, cy = c->y;
    course_place_player(&s.runtime, &s.game.player, cx, cy + 9);
    ticks((GameInput){0}, 3);
    assert(s.progress.coins == 1 && c->collected);
    ticks((GameInput){0}, 40);
    assert(!c->alive && alive(LIVE_COIN) == 17);
    LiveObject *b = first(LIVE_BLUE_COIN);
    course_place_player(&s.runtime, &s.game.player, b->x, b->y + 8);
    ticks((GameInput){0}, 3);
    /* +5 coins, hpcoincount 1+15 = 16 -> heals to 8 (two HP), then capped at 5. */
    assert(s.progress.coins == 6 && s.progress.hp == 8 && s.progress.hpcoins == 5);
    /* Leaving and returning keeps collected coins gone (coindata). */
    s.state = SESSION_PLAY;
    const CourseArea *a = s.runtime.area;
    unsigned collected = 0;
    for (unsigned i = 0; i < COURSE_MAX_PICKUPS; ++i) collected += s.pickups[a - pack.areas].taken[i];
    assert(collected == 2);
}
static void test_toad_dialogue_pages(void)
{
    play_area("8-E1-1");
    LiveObject *t = first(LIVE_TOAD);
    course_place_player(&s.runtime, &s.game.player, t->x - 12, t->y);
    ticks((GameInput){0}, 30);
    assert(t->alpha > 90); /* bz prompt */
    ticks((GameInput){.pound_pressed = true}, 1);
    assert(s.state == SESSION_TALK && s.toad_page == 1);
    assert(!memcmp(s.toad_text.text, ".....Did you know", 17) && course_string_equals(s.toad_mood, "Talk"));
    /* M2Txt = undefined: the original ends the talk after the first page. */
    ticks((GameInput){.pound_pressed = true}, 1);
    assert(s.state == SESSION_PLAY);
    play_area("8-E1-2-2");
    t = first(LIVE_TOAD);
    course_place_player(&s.runtime, &s.game.player, t->x + 12, t->y);
    ticks((GameInput){0}, 30);
    ticks((GameInput){.pound_pressed = true}, 1);
    assert(s.state == SESSION_TALK && !memcmp(s.toad_text.text, "...Hello Mario, How are you?", 28));
    unsigned pages = 1;
    while (s.state == SESSION_TALK) { ticks((GameInput){.pound_pressed = true}, 1); ticks((GameInput){0}, 2); ++pages; }
    assert(pages == 5); /* M1..M4, M5Txt undefined */
}
static void test_falling_log(void)
{
    play_area("8-E1-1");
    LiveObject *l = first(LIVE_LOG);
    assert(l && l->x == 939.45f);
    ticks((GameInput){0}, 25); /* fades in to alpha 100 */
    assert(l->alpha >= 100);
    course_place_player(&s.runtime, &s.game.player, l->x, l->y - 6);
    s.game.player.vy = 0;
    ticks((GameInput){0}, 2);
    assert(l->state == 1 && s.game.player.grounded); /* standing on it starts the fall */
    const float start = l->home_y;
    ticks((GameInput){0}, 30);
    assert(l->y > start + 3); /* falling after the 10-frame quake */
    ticks((GameInput){0}, 60);
    assert(l->state == 0 && l->y == start); /* returns home after fallamount */
}
static void test_birds_and_butterflies_move(void)
{
    play_area("8-E1-1");
    LiveObject *g = first(LIVE_GOONIE), *b = first(LIVE_BUTTERFLY);
    const float gx = g->x, bx = b->x;
    ticks((GameInput){0}, 120);
    assert(g->x != gx && b->x != bx && fabsf(b->x - bx) < 200);
}
int main(void)
{
    FILE *f = fopen("../../assets/generated/course_8e1.bin", "rb"); assert(f);
    const size_t n = fread(blob, 1, sizeof(blob), f); fclose(f);
    assert(course_pack_parse(blob, n, &pack));
    test_coins_heal_and_persist(); test_toad_dialogue_pages(); test_falling_log(); test_birds_and_butterflies_move();
    puts("4 course object (coins/Toad/log/birds) test groups passed"); return 0;
}
