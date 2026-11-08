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

/* ---- the streamed form ("SM63TLP3", timeline_art.write_stream) ----
   For cutscenes longer than memory holds: pages of at most 512 x 512 texels
   with the frames each is used on, and the draws in LZ4 chunks of
   `chunk_frames` frames. The player keeps the pages of the next frames
   (source/platform/cinema_render.c) and one chunk unpacked. */
#define TLS_MAX_PAGES 4096
#define TLS_MAX_CHUNKS 1024
typedef struct {
    unsigned short w, h;
    unsigned char format;
    unsigned intervals, first_interval;
    size_t packed;
    const uint8_t *data;
} TlsPage;
typedef struct {
    unsigned pages, bakes, clips, frames, chunk_frames, chunks, intervals;
    size_t max_chunk;          /* bytes of the biggest chunk unpacked */
    TlsPage page[TLS_MAX_PAGES];
    TlClip clip[TL_MAX_CLIPS];
    const uint8_t *interval_data, *bake_data, *frame_data;
    const uint8_t *chunk[TLS_MAX_CHUNKS];
    uint32_t chunk_bytes[TLS_MAX_CHUNKS], chunk_packed[TLS_MAX_CHUNKS];
} TimelineStream;

bool tls_parse(const uint8_t *data, size_t size, TimelineStream *s);
int tls_find(const TimelineStream *s, const char *name);
TlBake tls_bake(const TimelineStream *s, unsigned i);
/* Whether page `page` is drawn on any frame in [f0, f1] (pack frames). */
bool tls_page_used(const TlsPage *pg, const TimelineStream *s, unsigned f0, unsigned f1);
/* The chunk holding pack frame `frame`. */
unsigned tls_chunk_of(const TimelineStream *s, unsigned frame);
bool tls_unpack_chunk(const TimelineStream *s, unsigned chunk, uint8_t *dst, size_t cap);
/* Pack frame `frame`'s first draw in its chunk's unpacked data `chunk_data`. */
const uint8_t *tls_frame(const TimelineStream *s, unsigned frame, const uint8_t *chunk_data, unsigned *count);

#ifdef __cplusplus
}
#endif
#endif
