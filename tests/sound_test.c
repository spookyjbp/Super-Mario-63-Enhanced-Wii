/* Sound pack (original tutorial sounds) and the simulation's sound requests. */
#include "course_session.h"
#include "sfx.h"
#include "sound_pack.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char course[1 << 20];
static unsigned char sounds[4 << 20];
static CoursePack pack;
static CourseSession s;

static bool heard(SfxId id)
{
    SfxEvent e[SFX_QUEUE];
    const unsigned n = sfx_drain(e);
    bool found = false;
    for (unsigned i = 0; i < n; ++i) found |= e[i].id == id;
    return found;
}
static void test_pack(void)
{
    FILE *f = fopen("../../assets/generated/sounds_8e1.bin", "rb"); assert(f);
    const size_t n = fread(sounds, 1, sizeof(sounds), f); fclose(f);
    static SoundPack p;
    assert(sound_pack_parse(sounds, n, &p) && p.count == SFX_COUNT);
    /* DefineSound sample counts: CoinJingle 0.75 s, the castle intro 12.96 s and repeat 66.6 s at 22050 Hz. */
    assert(p.sound[SFX_COIN].rate == 22050 && fabsf(p.sound[SFX_COIN].samples / 22050.0f - 0.750f) < 0.01f);
    assert(fabsf(p.sound[MUS_CASTLE_INTRO].samples / 22050.0f - 12.957f) < 0.01f);
    assert(fabsf(p.sound[MUS_CASTLE_REPEAT].samples / 22050.0f - 66.586f) < 0.01f);
    assert(p.sound[SFX_EXPLOSION].rate == 11025);
    int16_t *pcm = malloc(p.sound[SFX_COIN].samples * sizeof(int16_t));
    assert(pcm && sound_pack_decode(&p.sound[SFX_COIN], pcm));
    double energy = 0;
    for (unsigned i = 0; i < p.sound[SFX_COIN].samples; ++i) energy += (double)pcm[i] * pcm[i];
    assert(sqrt(energy / p.sound[SFX_COIN].samples) > 500); /* not silent */
    free(pcm);
    sounds[8 + 3] = 9; /* wrong version */
    assert(!sound_pack_parse(sounds, n, &p));
}
static void test_requests(void)
{
    FILE *f = fopen("../../assets/generated/course_8e1.bin", "rb"); assert(f);
    const size_t n = fread(course, 1, sizeof(course), f); fclose(f);
    assert(course_pack_parse(course, n, &pack));
    assert(course_session_start(&s, &pack, "8-E1-1", 0, 0, 640));
    int intro, repeat;
    assert(sfx_music_take(&intro, &repeat) && intro == MUS_CASTLE_INTRO && repeat == MUS_CASTLE_REPEAT);
    s.state = SESSION_PLAY; s.runtime.platform_frame = 1;
    course_place_player(&s.runtime, &s.game.player, 300, 60);
    for (int i = 0; i < 60; ++i) course_session_step(&s, (GameInput){0});
    sfx_drain((SfxEvent[SFX_QUEUE]){{0}});
    /* Walking plays StepSound as the walk frame passes 11-20 and 51-60. */
    bool step = false;
    for (int i = 0; i < 120; ++i) { course_session_step(&s, (GameInput){.horizontal = 1}); step |= heard(SFX_STEP); }
    assert(step);
    /* A jump says "Mario Jump-1" or "-1b". */
    bool jump = false;
    for (int i = 0; i < 4 && !jump; ++i) {
        course_session_step(&s, (GameInput){.jump_pressed = true, .jump_held = true});
        SfxEvent e[SFX_QUEUE];
        const unsigned k = sfx_drain(e);
        for (unsigned j = 0; j < k; ++j) jump |= e[j].id == SFX_JUMP1 || e[j].id == SFX_JUMP1B;
    }
    assert(jump);
    /* Warps keep the music (changecourse passes no resetmusic). */
    assert(course_session_start(&s, &pack, "8-E1-1-2", 0, 0, 640) == true);
    assert(!sfx_music_take(&intro, &repeat));
}
/* Bug 11: coin jingles collected together do not stack past two voices, a
   simultaneous pair merges, other sounds keep their voices, and every coin
   still counts. */
static void test_coin_voices(void)
{
    SfxVoice v[16] = {{0}};
    v[0] = (SfxVoice){true, MUS_CASTLE_REPEAT, 0}; /* music and FLUDD voices are not effects */
    unsigned now = 10;
    int a = sfx_pick_voice(v, SFX_COIN, now); assert(a == SFX_FIRST_VOICE); v[a] = (SfxVoice){true, SFX_COIN, now};
    assert(sfx_pick_voice(v, SFX_COIN, now) == -1);           /* same tick: merged */
    now += 2;
    int b = sfx_pick_voice(v, SFX_COIN, now); assert(b > a); v[b] = (SfxVoice){true, SFX_COIN, now};
    now += 2;
    int c = sfx_pick_voice(v, SFX_COIN, now); assert(c == a);  /* third copy replaces the oldest */
    v[c] = (SfxVoice){true, SFX_COIN, now};
    int d = sfx_pick_voice(v, SFX_JUMP1, now); assert(d != a && d != b && d >= SFX_FIRST_VOICE);
    /* Ten coins in a row through the session: counter exact, at most two jingles. */
    assert(course_session_start(&s, &pack, "8-E1-1", 0, 0, 640));
    s.state = SESSION_PLAY; s.runtime.platform_frame = 1;
    SfxVoice live[16] = {{0}};
    unsigned coins = 0, t = 0, max_same = 0;
    for (unsigned i = 0; i < s.objects.count && coins < 10; ++i) {
        LiveObject *l = &s.objects.live[i];
        if (l->kind != LIVE_COIN || !l->alive || l->collected) continue;
        course_place_player(&s.runtime, &s.game.player, l->x, l->y + 9);
        for (int k = 0; k < 2; ++k) {
            course_session_step(&s, (GameInput){0});
            SfxEvent e[SFX_QUEUE];
            const unsigned n = sfx_drain(e);
            ++t;
            for (unsigned j = 0; j < n; ++j) {
                const int voice = sfx_pick_voice(live, e[j].id, t);
                if (voice >= 0) live[voice] = (SfxVoice){true, (int)e[j].id, t};
            }
            unsigned same = 0;
            for (int j = 0; j < 16; ++j) same += live[j].busy && live[j].id == SFX_COIN;
            if (same > max_same) max_same = same;
        }
        ++coins;
    }
    assert(s.progress.coins == 10 && max_same <= 2);
}
int main(void)
{
    test_pack();
    test_requests();
    test_coin_voices();
    puts("3 sound pack/request/voice test groups passed");
    return 0;
}
