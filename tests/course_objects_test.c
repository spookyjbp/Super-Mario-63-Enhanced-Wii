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
static void settle_goomba(LiveObject *gm) { for (int i = 0; i < 90; ++i) course_session_step(&s, (GameInput){0}); (void)gm; }
static void test_goomba_hurts_and_stomps(void)
{
    play_area("8-E1-1");
    LiveObject *gm = first(LIVE_GOOMBA);
    assert(gm && gm->home_x == 1337.95f);
    /* Stand far away, let the Goomba settle on the ground. */
    course_place_player(&s.runtime, &s.game.player, 1150, -60);
    settle_goomba(gm);
    assert(!gm->air);
    /* Walk into it: MarioHit(1, 90, -5, +/-7, 7). */
    course_place_player(&s.runtime, &s.game.player, gm->x - 4, gm->y);
    s.game.player.vx = s.game.player.vy = 0; s.game.player.grounded = true;
    ticks((GameInput){0}, 2);
    assert(s.progress.hp == 7 && s.game.player.blink > 80 && s.game.player.vy < 0);
    ticks((GameInput){0}, 2);
    assert(s.progress.hp == 7); /* invulnerable while blinking */
    /* Stomp: fall onto the head from above. */
    play_area("8-E1-1");
    gm = first(LIVE_GOOMBA);
    course_place_player(&s.runtime, &s.game.player, 1150, -60);
    settle_goomba(gm);
    gm->seeing = true; /* already noticed Mario: no surprise hop */
    const float gx = gm->x, gy = gm->y;
    const unsigned coins = s.progress.coins;
    course_place_player(&s.runtime, &s.game.player, gx, gy - 40);
    s.game.player.vy = 200; s.game.player.grounded = false;
    int t = 0;
    while (gm->frame_label != GOOMBA_SQUISH && t++ < 40) ticks((GameInput){0}, 1);
    assert(gm->frame_label == GOOMBA_SQUISH && s.progress.hp == 8 && s.game.player.ejump);
    float lowest = 0;
    for (int i = 0; i < 40; ++i) { ticks((GameInput){0}, 1); lowest = fminf(lowest, s.game.player.vy); }
    /* bounced: yspeed -= 8 (read one port tick after the 32 Hz frame) */
    assert(lowest < -6.5f * 44 && !s.game.player.ejump);
    ticks((GameInput){0}, 10);
    LiveObject *drop = NULL;
    for (unsigned i = 0; i < s.objects.count; ++i) if (s.objects.live[i].kind == LIVE_COIN && s.objects.live[i].bounce) drop = &s.objects.live[i];
    assert(drop); /* frame 12: one bouncing coin */
    for (int i = 0; i < 90 && !drop->collected; ++i) {
        course_place_player(&s.runtime, &s.game.player, drop->x, drop->y + 9);
        ticks((GameInput){0}, 1);
    }
    assert(s.progress.coins == coins + 1);
}
int main(void)
{
    FILE *f = fopen("../../assets/generated/course_8e1.bin", "rb"); assert(f);
    const size_t n = fread(blob, 1, sizeof(blob), f); fclose(f);
    assert(course_pack_parse(blob, n, &pack));
    test_coins_heal_and_persist(); test_toad_dialogue_pages(); test_falling_log(); test_birds_and_butterflies_move();
    test_goomba_hurts_and_stomps();
    puts("5 course object (coins/Toad/log/birds/Goomba) test groups passed"); return 0;
}
