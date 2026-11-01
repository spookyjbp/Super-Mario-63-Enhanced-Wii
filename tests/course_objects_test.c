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
static void test_cheep_cheep(void)
{
    play_area("8-E1-1");
    LiveObject *ch = NULL;
    for (unsigned i = 0; i < s.objects.count; ++i)
        if (s.objects.live[i].kind == LIVE_CHEEP && s.objects.live[i].home_x == 619.95f) ch = &s.objects.live[i];
    assert(ch);
    /* Idle: swims at idlespeed and stays in the water. */
    course_place_player(&s.runtime, &s.game.player, 1150, -60);
    ticks((GameInput){0}, 120);
    float surface;
    assert(course_area_water(s.runtime.area, ch->x, ch->y, &surface) && !ch->carrying);
    /* Mario in the water within 80 px: it chases and bites (MarioHit). */
    course_place_player(&s.runtime, &s.game.player, ch->x + 30, ch->y + 10);
    int t = 0;
    while (s.progress.hp == 8 && t++ < 240) {
        course_place_player(&s.runtime, &s.game.player, ch->x + 30 - t * 0.1f, ch->y + 10);
        ticks((GameInput){0}, 1);
    }
    assert(s.progress.hp == 7 && ch->timer > 0); /* stopchasing = 32 after a bite */
    /* Spin attack (the clip's 'water' area): it spins away, then is gone and
       drops one coin. (Diving is not possible in water.) */
    s.game.player.blink = 0;
    for (int i = 0; i < 20 && ch->frame_label != CHEEP_SPIN; ++i) {
        course_place_player(&s.runtime, &s.game.player, ch->x + 20, ch->y + 12);
        ticks((GameInput){.spin_pressed = true, .spin_held = true}, 1);
    }
    assert(ch->frame_label == CHEEP_SPIN);
    ticks((GameInput){0}, 60);
    assert(ch->frame_label == CHEEP_GONE && ch->spawned);
}
static LiveObject *bomb_at(float hx)
{
    for (unsigned i = 0; i < s.objects.count; ++i)
        if (s.objects.live[i].kind == LIVE_BOMB && s.objects.live[i].home_x == hx) return &s.objects.live[i];
    return NULL;
}
static void test_bob_omb(void)
{
    play_area("8-E1-1-2");
    LiveObject *b = bomb_at(282.5f);
    assert(b && b->state == BOMB_WALKL && !b->exploding);
    /* Out of sight it patrols (walks) without lighting its fuse. */
    course_place_player(&s.runtime, &s.game.player, b->x + 300, b->y - 200);
    ticks((GameInput){0}, 120);
    assert(!b->exploding && !b->air && (b->state == BOMB_WALKL || b->state == BOMB_WALKR));
    /* Seen within its eye (x 0.4 * 2381): the fuse burns 96 frames, it chases,
       then explodes beside Mario: MarioHit(2, 90, -7, +/-5, 10, ..., true). */
    s.game.player.blink = 0;
    course_place_player(&s.runtime, &s.game.player, b->x + 50, b->y);
    ticks((GameInput){0}, 2);
    assert(b->exploding && b->seeing);
    int t = 0;
    while (b->state != BOMB_EXPLODE && t++ < 400) {
        course_place_player(&s.runtime, &s.game.player, b->x + 14, b->y); /* stay beside it */
        s.game.player.vx = s.game.player.vy = 0; s.game.player.blink = 3; /* touch shoves only */
        ticks((GameInput){0}, 1);
    }
    assert(b->state == BOMB_EXPLODE && b->timer2 >= 96 && b->timer2 <= 97);
    /* EnemyHurtBox (X, Y-15) 30 x 30, time 6 (the other Bob-omb, which saw
       Mario on the same frame, may explode too). */
    bool box = false;
    for (unsigned k = 0; k < s.objects.hurt_count; ++k) {
        const EnemyHurtBox *h = &s.objects.hurt[k];
        box |= fabsf(h->x0 - (b->x - 15)) < 0.01f && fabsf(h->y1 - (b->y)) < 0.01f && h->time == 6;
    }
    assert(box);
    s.game.player.blink = 0;
    course_place_player(&s.runtime, &s.game.player, b->x + 8, b->y);
    ticks((GameInput){0}, 2);
    assert(s.progress.hp == 6 && s.game.player.stun > 0 && s.game.player.blink > 80);
    ticks((GameInput){0}, 20);
    assert(s.objects.hurt_count == 0); /* removed when count reaches time */
    LiveObject *drop = NULL;
    for (unsigned i = 0; i < s.objects.count; ++i) if (s.objects.live[i].kind == LIVE_COIN && s.objects.live[i].bounce) drop = &s.objects.live[i];
    assert(drop && b->spawned); /* coinappear: one bouncing coin */
    /* 30 frames later it stays exploded until its home is 600 px off camera. */
    ticks((GameInput){0}, 80);
    assert(b->state == BOMB_EXPLODE && b->deadcount >= 30);

    /* A spin knocks it tumbling with a power hit: it explodes on landing, and
       its blast (EnemyHurtBox 35 x 50) knocks a nearby Goomba. */
    play_area("8-E1-1-2");
    b = bomb_at(450.5f);
    course_place_player(&s.runtime, &s.game.player, b->x + 300, b->y - 200);
    ticks((GameInput){0}, 60);
    b->seeing = true; b->exploding = false;
    for (int i = 0; i < 10 && !b->tumble; ++i) {
        course_place_player(&s.runtime, &s.game.player, b->x + 20, b->y);
        ticks((GameInput){.spin_pressed = true, .spin_held = true}, 1);
    }
    assert(b->tumble && b->powerhit && b->vx < 0);
    course_place_player(&s.runtime, &s.game.player, b->x + 300, b->y - 200);
    for (t = 0; t < 200 && b->state != BOMB_EXPLODE && b->state != BOMB_EXPLODE2; ++t) ticks((GameInput){0}, 1);
    assert(b->state == BOMB_EXPLODE || b->state == BOMB_EXPLODE2);
    LiveObject *gm = first(LIVE_GOOMBA);
    assert(gm);
    course_place_player(&s.runtime, &s.game.player, b->x + 150, b->y - 150);
    ticks((GameInput){0}, 60); /* camera over the blast */
    gm->x = b->x + 12; gm->y = b->y; gm->vx = gm->vy = 0; gm->tumble = false; /* stand it in the blast */
    s.objects.hurt[0] = (EnemyHurtBox){b->x - 17.5f, b->y - 50, b->x + 17.5f, b->y, 0, 6};
    s.objects.hurt_count = 1;
    ticks((GameInput){0}, 2);
    assert(gm->tumble && gm->vx > 0); /* left point only: xspeed += max(12 - xspeed/1.5, 0) */
}
int main(void)
{
    FILE *f = fopen("../../assets/generated/course_8e1.bin", "rb"); assert(f);
    const size_t n = fread(blob, 1, sizeof(blob), f); fclose(f);
    assert(course_pack_parse(blob, n, &pack));
    test_coins_heal_and_persist(); test_toad_dialogue_pages(); test_falling_log(); test_birds_and_butterflies_move();
    test_goomba_hurts_and_stomps();
    test_cheep_cheep();
    test_bob_omb();
    puts("7 course object (coins/Toad/log/birds/Goomba/Cheep/Bob-omb) test groups passed"); return 0;
}
