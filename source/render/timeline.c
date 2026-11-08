#include "timeline.h"
#include "lz4_block.h"
#include <string.h>

static unsigned be16(const uint8_t *p) { return (unsigned)p[0] << 8 | p[1]; }
static uint32_t be32(const uint8_t *p) { return (uint32_t)be16(p) << 16 | be16(p + 2); }
static float bef(const uint8_t *p) { uint32_t u = be32(p); float f; memcpy(&f, &u, 4); return f; }
static short bes16(const uint8_t *p) { return (short)be16(p); }

bool timeline_parse(const uint8_t *data, size_t size, TimelinePack *p)
{
    memset(p, 0, sizeof *p);
    if (size < 28 || memcmp(data, "SM63TLP2", 8)) return false;
    p->pages = be32(data + 8); p->bakes = be32(data + 12); p->clips = be32(data + 16);
    p->frames = be32(data + 20); p->draw_bytes = be32(data + 24);
    if (p->pages > TL_MAX_PAGES || p->clips > TL_MAX_CLIPS) return false;
    size_t q = 28;
    for (unsigned i = 0; i < p->pages; ++i) {
        if (q + 12 > size) return false;
        TlPage *pg = &p->page[i];
        pg->w = (unsigned short)be16(data + q); pg->h = (unsigned short)be16(data + q + 2);
        pg->packed = be32(data + q + 4); pg->format = data[q + 8]; pg->clip = data[q + 9];
        pg->data = data + q + 12;
        q += 12 + pg->packed;
        q += (4 - q % 4) % 4;
        if (q > size) return false;
    }
    p->bake_data = data + q; q += (size_t)p->bakes * 24;
    for (unsigned i = 0; i < p->clips; ++i) {
        if (q + 24 > size) return false;
        memcpy(p->clip[i].name, data + q, 16); p->clip[i].name[16] = 0;
        p->clip[i].first = be32(data + q + 16); p->clip[i].count = be32(data + q + 20);
        q += 24;
    }
    p->frame_data = data + q; q += (size_t)p->frames * 8;
    if (q + 4 > size) return false;
    p->draw_packed = be32(data + q); p->draw_data_packed = data + q + 4;
    return q + 4 + p->draw_packed <= size;
}
bool timeline_unpack_draws(TimelinePack *p, uint8_t *dst, size_t cap)
{
    if (cap < p->draw_bytes) return false;
    if (lz4_block_decode(p->draw_data_packed, p->draw_packed, dst, p->draw_bytes) != (long)p->draw_bytes) return false;
    p->draws = dst;
    return true;
}
int timeline_find(const TimelinePack *p, const char *name)
{
    for (unsigned i = 0; i < p->clips; ++i)
        if (!strncmp(p->clip[i].name, name, 16)) return (int)i;
    return -1;
}
TlBake timeline_bake(const TimelinePack *p, unsigned i)
{
    const uint8_t *b = p->bake_data + (size_t)i * 24;
    return (TlBake){(unsigned short)be16(b), (unsigned short)be16(b + 2), (unsigned short)be16(b + 4),
                    (unsigned short)be16(b + 6), (unsigned short)be16(b + 8), bef(b + 10), bef(b + 14), bef(b + 18)};
}
const uint8_t *timeline_frame(const TimelinePack *p, unsigned clip, unsigned frame, unsigned *count)
{
    *count = 0;
    if (clip >= p->clips || !p->draws || !p->clip[clip].count) return NULL;
    if (frame >= p->clip[clip].count) frame = p->clip[clip].count - 1;
    const uint8_t *f = p->frame_data + (size_t)(p->clip[clip].first + frame) * 8;
    *count = be32(f + 4);
    return p->draws + be32(f);
}
const uint8_t *timeline_next(const uint8_t *at, TlDraw *d)
{
    d->bake = be16(at); d->weight = at[2] / 255.0f; d->group = at[3]; d->nverts = at[4];
    for (unsigned i = 0; i < 6; ++i) d->m[i] = bef(at + 8 + 4 * i);
    for (unsigned i = 0; i < 4; ++i) d->mul[i] = bes16(at + 32 + 2 * i) / 256.0f;
    for (unsigned i = 0; i < 4; ++i) d->add[i] = bes16(at + 40 + 2 * i);
    d->verts = at + 48;
    return at + 48 + 16 * d->nverts;
}
void timeline_vertex(const TlDraw *d, unsigned i, float *x, float *y, float *u, float *v)
{
    const uint8_t *p = d->verts + 16 * i;
    *x = bef(p); *y = bef(p + 4); *u = bef(p + 8); *v = bef(p + 12);
}
