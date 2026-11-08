/* The story cutscenes' player (cinema.h). */
#include "cinema.h"
#include <string.h>

#define NONE 0xFFFFFFFFu

static unsigned be16(const uint8_t *p) { return (unsigned)p[0] << 8 | p[1]; }
static uint32_t be32(const uint8_t *p) { return (uint32_t)be16(p) << 16 | be16(p + 2); }
static float bef(const uint8_t *p) { uint32_t u = be32(p); float f; memcpy(&f, &u, 4); return f; }

bool cinema_parse(const uint8_t *data, size_t size, CinemaData *c)
{
    memset(c, 0, sizeof *c);
    if (!data || size < 64 || memcmp(data, "SM63CIN1", 8) || be32(data + 8) != 1) return false;
    c->frames = be32(data + 12); c->events = be32(data + 16); c->sounds = be32(data + 20);
    const uint32_t ev = be32(data + 24), snd = be32(data + 28), pack = be32(data + 32), pack_size = be32(data + 36);
    for (unsigned i = 0; i < 6; ++i) c->box[i] = bef(data + 40 + 4 * i);
    if (c->sounds > CIN_MAX_SOUNDS || ev > size || snd > size || pack > size || pack_size > size - pack) return false;
    if ((size_t)snd + (size_t)c->sounds * 24 > size) return false;
    c->event_data = data + ev;
    for (unsigned i = 0; i < c->sounds; ++i) {
        const uint8_t *e = data + snd + 24 * i;
        const uint32_t off = be32(e + 8), len = be32(e + 12), seek = be32(e + 16), seeks = be32(e + 20);
        if (off > size || len > size - off || len < 4 || seek > size || (size_t)seeks * 4 > size - seek) return false;
        c->sound[i] = (CinemaSound){be32(e), be32(e + 4), data + off, len, data + seek, seeks};
        if (!c->sound[i].rate || (len - 4) * 2 < c->sound[i].samples) return false;
    }
    /* the events must be whole and in frame order */
    size_t at = 0;
    unsigned last = 0;
    for (unsigned i = 0; i < c->events; ++i) {
        if (ev + at + 8 > snd) return false;
        const uint8_t *p = c->event_data + at;
        const unsigned f = be32(p), n = p[5];
        if (f < last || f >= c->frames || p[4] < CIN_MESSAGE || p[4] > CIN_END) return false;
        last = f;
        at += 8 + ((n + 3u) & ~3u);
    }
    if (ev + at > snd) return false;
    c->pack = data + pack; c->pack_size = pack_size;
    return true;
}
size_t cinema_event(const CinemaData *c, size_t at, CinemaEvent *e)
{
    const uint8_t *p = c->event_data + at;
    *e = (CinemaEvent){be32(p), p[4], p[5], p + 8};
    return at + 8 + ((e->size + 3u) & ~3u);
}

/* ---- the player ---- */
static size_t end_of_events(const CinemaData *c)
{
    size_t at = 0;
    CinemaEvent e;
    for (unsigned i = 0; i < c->events; ++i) at = cinema_event(c, at, &e);
    return at;
}
static void next_hold(Cinema *p)
{
    const CinemaData *c = p->data;
    const size_t end = end_of_events(c);
    p->hold_at = NONE;
    CinemaEvent e;
    for (size_t at = p->next; at < end;) {
        at = cinema_event(c, at, &e);
        if (e.kind == CIN_PRESS) { p->hold_at = e.frame; return; }
    }
}
/* Apply one event. `seek_to`: when set (not NONE), the events are replayed to
   reach that frame: no sound effects, the music where it would be by then. */
static void apply(Cinema *p, const CinemaEvent *e, unsigned seek_to)
{
    switch (e->kind) {
    case CIN_MESSAGE: {
        const unsigned n = e->size ? e->size - 1 : 0;
        const unsigned k = n < CIN_TEXT - 1 ? n : CIN_TEXT - 1;
        if (e->size) p->portrait = e->payload[0];
        if (k != p->length || memcmp(p->message, e->payload + 1, k)) p->restart = true;
        memcpy(p->message, e->payload + 1, k);
        p->message[k] = 0;
        p->length = k;
        break;
    }
    case CIN_MUSIC:
        if (e->size < 12) break;
        p->music.sound = e->payload[0];
        p->music.repeat = e->payload[1] == 255 ? -1 : e->payload[1];
        p->music.loop = e->payload[2] != 0;
        p->music.offset = bef(e->payload + 4);
        p->music.volume = bef(e->payload + 8);
        if (seek_to != NONE && seek_to > e->frame) p->music.offset += (float)(seek_to - e->frame) / 32;
        ++p->music.serial;
        break;
    case CIN_STOP:
        p->music.sound = p->music.repeat = -1;
        ++p->music.serial;
        break;
    case CIN_VOLUME:
        if (e->size >= 4) p->music.volume = bef(e->payload);
        break;
    case CIN_SFX:
        if (seek_to == NONE && e->size >= 1 && p->sfx_count < CIN_SFX_QUEUE) p->sfx[p->sfx_count++] = e->payload[0];
        break;
    case CIN_SKIP:
        if (e->size >= 4) p->skip_to = be32(e->payload);
        break;
    case CIN_END:
        if (e->size >= 8) {
            p->end_x = bef(e->payload); p->end_y = bef(e->payload + 4);
            const unsigned n = e->size - 8 < sizeof p->course - 1 ? e->size - 8 : (unsigned)sizeof p->course - 1;
            memcpy(p->course, e->payload + 8, n);
            p->course[n] = 0;
        }
        p->done = true;
        break;
    default:
        break;
    }
}
/* Apply the events up to and including `frame`. */
static void events_to(Cinema *p, unsigned frame, unsigned seek_to)
{
    const CinemaData *c = p->data;
    const size_t end = end_of_events(c);
    CinemaEvent e;
    while (p->next < end) {
        const size_t after = cinema_event(c, p->next, &e);
        if (e.frame > frame) break;
        p->next = after;
        apply(p, &e, seek_to);
    }
}
static void reset(Cinema *p, const CinemaData *d)
{
    const unsigned serial = p->music.serial;
    memset(p, 0, sizeof *p);
    p->data = d;
    p->portrait = 1;
    p->skip_to = NONE;
    p->music.sound = p->music.repeat = -1;
    p->music.volume = 100;
    p->music.serial = serial;
}
void cinema_start(Cinema *p, const CinemaData *d)
{
    p->music.serial = 0;
    reset(p, d);
    events_to(p, 0, NONE);
    next_hold(p);
}
/* To frame `to` as the recording got there (START's skip): the message box
   and the music as they would be by then. */
static void seek(Cinema *p, unsigned to)
{
    reset(p, p->data);
    events_to(p, to, to);
    p->frame = to;
    p->sfx_count = 0;
    p->alpha = p->length ? 100 : 0;
    ++p->music.serial;
    next_hold(p);
}
/* 627's onEnterFrame */
static void box_frame(Cinema *p, bool read)
{
    p->finished = false;
    p->alpha += ((p->length ? 100.0f : 0.0f) - p->alpha) / 4;
    p->prompt_alpha += (p->prompt_target - p->prompt_alpha) / 2;
    if (p->alpha > 98) {
        if (p->restart) { p->typed = 0; p->restart = false; }
        for (unsigned i = 0; i < 2; ++i) {
            if (p->typed < p->length) ++p->typed;
            else p->finished = true;
        }
        if (read) p->typed = p->length;
        p->prompt_target = p->typed >= p->length ? 100.0f : 0.0f;
    }
}
static void source_frame(Cinema *p, bool read)
{
    const bool was_finished = p->finished && p->length;
    box_frame(p, read);
    if (p->done) return;
    if (p->frame == p->hold_at) {
        if (!(read && was_finished)) return;      /* held until the read button */
        p->hold_at = NONE;
    } else if (read && was_finished && p->hold_at != NONE && p->hold_at > p->frame) {
        /* the read button with the text out: on from where the recording pressed */
        events_to(p, p->hold_at, NONE);
        p->frame = p->hold_at;
        p->sfx_count = 0;
        p->hold_at = NONE;
        return;
    }
    if (p->frame + 1 >= p->data->frames) { p->done = true; return; }
    ++p->frame;
    events_to(p, p->frame, NONE);
    if (p->hold_at == NONE || p->hold_at < p->frame) next_hold(p);
}
void cinema_step(Cinema *p, bool read, bool skip)
{
    if (!p->data || p->done) return;
    if (read) p->latched = true;
    if (skip && p->skip_to != NONE && p->skip_to > p->frame) {
        seek(p, p->skip_to);
        if (p->done) return;
    }
    p->phase += 32;
    while (p->phase >= 60 && !p->done) {
        p->phase -= 60;
        source_frame(p, p->latched);
        p->latched = false;
    }
}
unsigned cinema_visible_text(const Cinema *p, char *out, size_t cap, unsigned *name_end)
{
    unsigned n = 0, i = 0;
    bool in_tag = false;
    if (name_end) *name_end = 0;
    const unsigned upto = p->typed < p->length ? p->typed : p->length;
    while (i < upto && n + 1 < cap) {
        const char ch = p->message[i];
        if (ch == '<') {
            in_tag = true;
            if (name_end && !strncmp(p->message + i, "</b>", 4)) *name_end = n;
        } else if (ch == '>' && in_tag) {
            in_tag = false;
        } else if (!in_tag) {
            out[n++] = ch;
        }
        ++i;
    }
    if (cap) out[n] = 0;
    return n;
}
unsigned cinema_take_sfx(Cinema *p, unsigned *out, unsigned cap)
{
    unsigned n = p->sfx_count < cap ? p->sfx_count : cap;
    memcpy(out, p->sfx, n * sizeof *out);
    p->sfx_count = 0;
    return n;
}

/* ---- the music stream ---- */
static const int16_t steps[89] = {
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 50, 55, 60, 66, 73, 80,
    88, 97, 107, 118, 130, 143, 157, 173, 190, 209, 230, 253, 279, 307, 337, 371, 408, 449, 494, 544,
    598, 658, 724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749,
    3024, 3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487,
    12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767};
static const int8_t index_step[8] = {-1, -1, -1, -1, 2, 4, 6, 8};

static int16_t decode_one(CinemaStream *s, const CinemaSound *snd)
{
    const unsigned i = s->pos++;
    const uint8_t b = snd->data[4 + (i >> 1)];
    const int code = i & 1 ? b & 15 : b >> 4;
    const int step = steps[s->index];
    int delta = step >> 3;
    if (code & 4) delta += step;
    if (code & 2) delta += step >> 1;
    if (code & 1) delta += step >> 2;
    s->pred = code & 8 ? s->pred - delta : s->pred + delta;
    if (s->pred > 32767) s->pred = 32767;
    if (s->pred < -32768) s->pred = -32768;
    s->index += index_step[code & 7];
    if (s->index < 0) s->index = 0;
    if (s->index > 88) s->index = 88;
    return (int16_t)s->pred;
}
/* to sample `at` of the current sound: from the seek point before it */
static void stream_seek(CinemaStream *s, unsigned at)
{
    const CinemaSound *snd = &s->data->sound[s->sound];
    if (at >= snd->samples) at = snd->samples;
    unsigned k = at / CIN_SEEK_SAMPLES;
    if (k >= snd->seeks) k = snd->seeks ? snd->seeks - 1 : 0;
    if (snd->seeks) {
        s->pred = (int16_t)be16(snd->seek + 4 * k);
        s->index = snd->seek[4 * k + 2] > 88 ? 88 : snd->seek[4 * k + 2];
        s->pos = k * CIN_SEEK_SAMPLES;
    } else {
        s->pred = (int16_t)be16(snd->data); s->index = 0; s->pos = 0;
    }
    while (s->pos < at) decode_one(s, snd);
}
/* the current sound has ended: on to the repeat, or stop */
static bool stream_next(CinemaStream *s)
{
    if (s->repeat >= 0 && (unsigned)s->repeat < s->data->sounds) {
        s->sound = s->repeat;
        if (!s->loop) s->repeat = -1;
        stream_seek(s, 0);
        return true;
    }
    s->ended = true;
    return false;
}
void cinema_stream_start(CinemaStream *s, const CinemaData *d, const CinemaMusic *m)
{
    memset(s, 0, sizeof *s);
    s->data = d;
    s->sound = m->sound; s->repeat = m->repeat; s->loop = m->loop;
    if (m->sound < 0 || (unsigned)m->sound >= d->sounds) { s->ended = true; return; }
    float off = m->offset > 0 ? m->offset : 0;
    for (unsigned guard = 0; guard < 64; ++guard) {   /* past the intro: into the repeat */
        const CinemaSound *snd = &d->sound[s->sound];
        const float len = (float)snd->samples / (float)snd->rate;
        if (off < len) { stream_seek(s, (unsigned)(off * (float)snd->rate)); return; }
        off -= len;
        if (!stream_next(s)) return;
    }
    s->ended = true;
}
void cinema_stream_read(CinemaStream *s, int16_t *out, unsigned n)
{
    for (unsigned i = 0; i < n; ++i) {
        if (s->ended) { out[i] = 0; continue; }
        const CinemaSound *snd = &s->data->sound[s->sound];
        if (s->pos >= snd->samples && !stream_next(s)) { out[i] = 0; continue; }
        out[i] = decode_one(s, &s->data->sound[s->sound]);
    }
}
unsigned cinema_stream_rate(const CinemaStream *s)
{
    return s->data && s->sound >= 0 && (unsigned)s->sound < s->data->sounds ? s->data->sound[s->sound].rate : 22050;
}
