#ifndef SM63_TIMELINE_H
#define SM63_TIMELINE_H
/* Flash timelines replayed (tools/timeline_art.py, "SM63TLP2"): each clip
   frame is a list of draws of baked pieces (a matrix, a colour transform,
   sometimes a clipped polygon). Pure parsing and iteration (host-tested);
   source/platform/front_render.c uploads the pages and draws. */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#define TL_MAX_PAGES 96
#define TL_MAX_CLIPS 48

typedef struct { unsigned short w, h; unsigned char format, clip; size_t packed; const uint8_t *data; } TlPage;
typedef struct { unsigned short page, x, y, w, h; float ox, oy, px; } TlBake;   /* px: stage px per texel */
typedef struct { char name[17]; unsigned first, count; } TlClip;
typedef struct {
    unsigned pages, bakes, clips, frames;
    size_t draw_bytes, draw_packed;
    TlPage page[TL_MAX_PAGES];
    TlClip clip[TL_MAX_CLIPS];
    const uint8_t *bake_data, *frame_data, *draw_data_packed;
    const uint8_t *draws;      /* the unpacked draw data (timeline_unpack_draws) */
} TimelinePack;

typedef struct {
    unsigned bake, group, nverts;
    float weight;              /* 0..1: a cross-faded key over the one before */
    float m[6];                /* a b c d tx ty: piece px -> clip px (stage px) */
    float mul[4], add[4];      /* Flash colour transform: rgb * mul + add, alpha too (add 0..255) */
    const uint8_t *verts;      /* nverts x (x, y, u, v) big-endian f32 when clipped */
} TlDraw;

bool timeline_parse(const uint8_t *data, size_t size, TimelinePack *p);
/* Unpack the draw data into dst (at least p->draw_bytes); sets p->draws. */
bool timeline_unpack_draws(TimelinePack *p, uint8_t *dst, size_t cap);
int timeline_find(const TimelinePack *p, const char *name);   /* clip index or -1 */
TlBake timeline_bake(const TimelinePack *p, unsigned i);
/* Frame `frame` of clip `clip` (clamped): the first draw and how many. */
const uint8_t *timeline_frame(const TimelinePack *p, unsigned clip, unsigned frame, unsigned *count);
/* Reads one draw, returns the next. */
const uint8_t *timeline_next(const uint8_t *at, TlDraw *d);
void timeline_vertex(const TlDraw *d, unsigned i, float *x, float *y, float *u, float *v);

#ifdef __cplusplus
}
#endif
#endif
