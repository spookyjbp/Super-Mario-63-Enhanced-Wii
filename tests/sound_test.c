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
int main(void)
{
    test_pack();
    test_requests();
    puts("2 sound pack/request test groups passed");
    return 0;
}
