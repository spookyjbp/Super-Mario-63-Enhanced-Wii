/* The story cutscenes' data files and player (cinema.h, timeline.h's
   streamed packs): the files are whole, the player holds for the read button
   and goes on with it, START skips where the original's Skip button goes,
   each cutscene ends in its course, every page a frame draws says so (the
   page cache relies on it), and the music seeks to the same samples it would
   reach playing from the start. */
#include "cinema.h"
#include "timeline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int failures;
#define CHECK(c) do { if (!(c)) { printf("FAIL %s:%d %s\n", __FILE__, __LINE__, #c); ++failures; } } while (0)

static uint8_t *load(const char *path, size_t *size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    *size = (size_t)ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *b = malloc(*size);
    if (fread(b, 1, *size, f) != *size) { free(b); b = NULL; }
    fclose(f);
    return b;
}

/* Every draw names a bake, every bake a page, and the page's intervals hold
   the frame it is drawn on. */
static void check_pack(const CinemaData *d)
{
    static TimelineStream s;
    CHECK(tls_parse(d->pack, d->pack_size, &s));
    const int cine = tls_find(&s, "cine"), box = tls_find(&s, "box");
    CHECK(cine >= 0 && box >= 0);
    if (cine < 0 || box < 0) return;
    CHECK(s.clip[cine].count == d->frames);
    CHECK(s.clip[box].count == 4);   /* 627's four pictures */
    uint8_t *chunk = malloc(s.max_chunk + 16);
    unsigned bad = 0, draws = 0;
    for (unsigned f = 0; f < s.frames; ++f) {
        if (f % s.chunk_frames == 0) CHECK(tls_unpack_chunk(&s, tls_chunk_of(&s, f), chunk, s.max_chunk));
        unsigned n;
        const uint8_t *at = tls_frame(&s, f, chunk, &n);
        for (unsigned i = 0; i < n; ++i) {
            TlDraw dr;
            at = timeline_next(at, &dr);
            ++draws;
            if (dr.bake >= s.bakes) { ++bad; continue; }
            const TlBake b = tls_bake(&s, dr.bake);
            if (b.page >= s.pages || !tls_page_used(&s.page[b.page], &s, f, f)) ++bad;
            else if (b.x + b.w > s.page[b.page].w || b.y + b.h > s.page[b.page].h) ++bad;
        }
    }
    CHECK(bad == 0);
    CHECK(draws > 1000);
    free(chunk);
}

static unsigned first_event(const CinemaData *d, unsigned kind, CinemaEvent *out)
{
    size_t at = 0;
    CinemaEvent e;
    for (unsigned i = 0; i < d->events; ++i) {
        at = cinema_event(d, at, &e);
        if (e.kind == kind) { if (out) *out = e; return e.frame; }
    }
    return 0xFFFFFFFFu;
}

/* Plays to the end, pressing the read button whenever the cutscene holds. */
static unsigned play_through(Cinema *p, const CinemaData *d, unsigned *holds)
{
    cinema_start(p, d);
    unsigned ticks = 0, last = 0, still = 0;
    *holds = 0;
    while (!p->done && ticks < 2000000) {
        const bool held = p->frame == p->hold_at && p->finished;
        if (held) ++*holds;
        cinema_step(p, held && (ticks & 1), false);
        if (p->frame == last) ++still; else still = 0;
        last = p->frame;
        if (still > 2000) break;     /* stuck */
        ++ticks;
    }
    return ticks;
}

static void check_cinema(const char *path, const char *course, float x, float y)
{
    size_t size;
    uint8_t *data = load(path, &size);
    CHECK(data != NULL);
    if (!data) return;
    static CinemaData d;
    CHECK(cinema_parse(data, size, &d));
    CHECK(!cinema_parse(data, 40, &d) && (cinema_parse(data, size, &d), true));
    printf("%s: %u frames, %u events, %u sounds\n", path, d.frames, d.events, d.sounds);
    check_pack(&d);
    static Cinema p;
    /* with no button the cutscene stops at the first PRESS, the text out */
    CinemaEvent e;
    const unsigned press = first_event(&d, CIN_PRESS, &e);
    if (press != 0xFFFFFFFFu) {
        cinema_start(&p, &d);
        for (unsigned t = 0; t < (press + 400) * 2; ++t) cinema_step(&p, false, false);
        CHECK(p.frame == press);
        CHECK(p.length > 0 && p.typed == p.length && p.finished);
        char text[CIN_TEXT];
        unsigned name_end;
        cinema_visible_text(&p, text, sizeof text, &name_end);
        CHECK(strchr(text, '<') == NULL && name_end > 0 && name_end < strlen(text));
        /* the read button goes on */
        cinema_step(&p, true, false);
        for (unsigned t = 0; t < 8; ++t) cinema_step(&p, false, false);
        CHECK(p.frame > press);
    }
    /* START: where the original's Skip button goes, the music as it would be */
    cinema_start(&p, &d);
    for (unsigned t = 0; t < 20; ++t) cinema_step(&p, false, false);
    const unsigned to = p.skip_to, serial = p.music.serial;
    CHECK(to != 0xFFFFFFFFu && to > p.frame);
    cinema_step(&p, false, true);
    CHECK(p.frame >= to && p.frame <= to + 1);
    CHECK(p.music.serial != serial);
    /* the whole cutscene, then its course */
    unsigned holds;
    play_through(&p, &d, &holds);
    CHECK(p.done);
    CHECK(!strcmp(p.course, course));
    CHECK(p.end_x == x && p.end_y == y);
    /* START again and again ends it too */
    cinema_start(&p, &d);
    for (unsigned t = 0; t < 200000 && !p.done; ++t)
        cinema_step(&p, p.frame == p.hold_at && p.finished, (t % 90) == 0);
    CHECK(p.done && !strcmp(p.course, course));
    /* the music: seeking lands on the samples playing from the start reaches */
    for (unsigned i = 0; i < d.sounds; ++i) {
        if (d.sound[i].samples < 3 * CIN_SEEK_SAMPLES) continue;
        CinemaMusic m = {(int)i, -1, 0, 100, false, 1};
        static CinemaStream a, b;
        static int16_t x0[CIN_SEEK_SAMPLES * 3], x1[256];
        cinema_stream_start(&a, &d, &m);
        cinema_stream_read(&a, x0, CIN_SEEK_SAMPLES * 3);
        const unsigned at = CIN_SEEK_SAMPLES * 2 + 1234;
        m.offset = (float)at / (float)d.sound[i].rate;
        cinema_stream_start(&b, &d, &m);
        const unsigned got = b.pos;
        cinema_stream_read(&b, x1, 256);
        CHECK(got + 256 <= CIN_SEEK_SAMPLES * 3);
        CHECK(!memcmp(x1, x0 + got, sizeof x1));
        /* past its end: silence, no repeat */
        m.offset = (float)d.sound[i].samples / (float)d.sound[i].rate + 1;
        cinema_stream_start(&b, &d, &m);
        cinema_stream_read(&b, x1, 16);
        CHECK(b.ended && x1[0] == 0 && x1[15] == 0);
        break;
    }
    free(data);
}

int main(void)
{
    check_cinema("../../assets/data/cinema_intro.bin", "8-E1-1", 0, 0);
    check_cinema("../../assets/data/cinema_intro2.bin", "Castle", 80, 60);
    if (failures) { printf("%d failures\n", failures); return 1; }
    puts("cinema tests passed");
    return 0;
}
