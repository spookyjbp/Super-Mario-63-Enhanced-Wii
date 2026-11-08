/* Draws the story cutscenes (cinema.h): the recorded frames of the data
   file's streamed timeline pack, the stage at 1.6 screen px a stage px (its
   300 rows on the 480 lines, the sides shown as far as 16:9 reaches, as the
   course does), and 627's message box with the text and the read button's
   prompt written by the port.

   The pack's pages (at most 256 x 256) are decoded into a cache in the
   course renderer's MEM2 area while no course is shown: every page drawn in
   the next LOOK frames is kept, the ones not yet there decoded a few a frame
   ahead of time (at once when the frame on screen needs them); the message
   box's pages stay. The draws are unpacked a chunk at a time. */
#include "cinema_render.h"
#include "audio.h"
#include "course_render.h"
#include "font_metrics.h"
#include "graphics.h"
#include "lz4_block.h"
#include "timeline.h"
#include <gccore.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define K 1.6f                    /* screen px a stage px */
#define LOOK 8u                   /* frames ahead whose pages are kept (tools/export_cinema.py checks they fit) */
#define AHEAD_PER_FRAME 4u        /* pages decoded ahead of time a rendered frame */
#define SLOT_RGBA8 (256u * 256u * 4u)
#define SLOT_RGB5A3 (256u * 256u * 2u)
#define MAX_SLOTS 192
#define CIN_MAX_FRAMES 16384

static TimelineStream pack;
static const CinemaData *data;
static int clip_cine = -1, clip_box = -1;
static unsigned char *chunk_buf;
static size_t chunk_cap;
static int chunk_loaded = -1;
typedef struct { unsigned char *mem; int page; } Slot;
static Slot slots[2][MAX_SLOTS];          /* [format]: RGBA8, RGB5A3 */
static unsigned slot_count[2];
static short page_slot[TLS_MAX_PAGES];    /* -1: not decoded */
static GXTexObj page_tex[TLS_MAX_PAGES];
static bool page_pinned[TLS_MAX_PAGES];
static bool ready;

typedef struct { float a, b, c, d, tx, ty; } Mat;
static Mat mul(Mat m, Mat n)
{
    return (Mat){m.a * n.a + m.c * n.b, m.b * n.a + m.d * n.b, m.a * n.c + m.c * n.d, m.b * n.c + m.d * n.d,
                 m.a * n.tx + m.c * n.ty + m.tx, m.b * n.tx + m.d * n.ty + m.ty};
}
static Mat mat(const float m[6]) { return (Mat){m[0], m[1], m[2], m[3], m[4], m[5]}; }

/* The most pages of each format the frames need at once (LOOK frames ahead
   and the pinned ones), from the pages' frame intervals. */
static void peaks(unsigned need[2])
{
    static short delta[2][CIN_MAX_FRAMES + LOOK + 2];
    const unsigned n = pack.frames < CIN_MAX_FRAMES ? pack.frames : CIN_MAX_FRAMES;
    memset(delta, 0, sizeof delta);
    unsigned pinned[2] = {0, 0};
    for (unsigned i = 0; i < pack.pages; ++i) {
        const TlsPage *pg = &pack.page[i];
        const unsigned f = pg->format ? 1 : 0;
        if (page_pinned[i]) { ++pinned[f]; continue; }
        unsigned last_end = 0;
        for (unsigned k = 0; k < pg->intervals; ++k) {
            const uint8_t *iv = pack.interval_data + (size_t)(pg->first_interval + k) * 8;
            const uint32_t a = (uint32_t)iv[0] << 24 | (uint32_t)iv[1] << 16 | (uint32_t)iv[2] << 8 | iv[3];
            const uint32_t b = (uint32_t)iv[4] << 24 | (uint32_t)iv[5] << 16 | (uint32_t)iv[6] << 8 | iv[7];
            unsigned from = a > LOOK ? a - LOOK : 0;
            if (from < last_end) from = last_end;       /* overlapping windows count once */
            const unsigned to = b + 1 < n ? b + 1 : n;
            if (from >= to) continue;
            ++delta[f][from]; --delta[f][to];
            last_end = to;
        }
    }
    for (unsigned f = 0; f < 2; ++f) {
        int run = 0, best = 0;
        for (unsigned i = 0; i < n; ++i) { run += delta[f][i]; if (run > best) best = run; }
        need[f] = (unsigned)best + pinned[f];
    }
}

bool cinema_render_begin(const CinemaData *d)
{
    ready = false;
    data = d;
    if (!d || !tls_parse(d->pack, d->pack_size, &pack)) return false;
    clip_cine = tls_find(&pack, "cine");
    clip_box = tls_find(&pack, "box");
    if (clip_cine < 0) return false;
    size_t size;
    unsigned char *mem = course_render_scratch(&size);
    size_t used = audio_cinema_begin(d, mem, size);
    used = (used + 31) & ~(size_t)31;
    chunk_buf = mem + used;
    chunk_cap = (pack.max_chunk + 31) & ~(size_t)31;
    used += chunk_cap;
    chunk_loaded = -1;
    for (unsigned i = 0; i < pack.pages; ++i) {
        page_slot[i] = -1;
        page_pinned[i] = clip_box >= 0 &&
                         tls_page_used(&pack.page[i], &pack, pack.clip[clip_box].first,
                                       pack.clip[clip_box].first + pack.clip[clip_box].count - 1);
    }
    unsigned need[2];
    peaks(need);
    const size_t sizes[2] = {SLOT_RGBA8, SLOT_RGB5A3};
    for (unsigned f = 0; f < 2; ++f) {
        slot_count[f] = need[f] + 2 < MAX_SLOTS ? need[f] + 2 : MAX_SLOTS;
        for (unsigned i = 0; i < slot_count[f]; ++i) {
            if (used + sizes[f] > size) return false;
            slots[f][i] = (Slot){mem + used, -1};
            used += sizes[f];
        }
    }
    ready = true;
    return true;
}
void cinema_render_end(void)
{
    ready = false;
    audio_cinema_end();
}

static bool decode_page(unsigned i)
{
    const TlsPage *pg = &pack.page[i];
    const unsigned f = pg->format ? 1 : 0;
    const size_t bytes = (size_t)pg->w * pg->h * (f ? 2 : 4);
    /* a free slot, or one whose page is not wanted in the next LOOK frames */
    int pick = -1;
    for (unsigned s = 0; s < slot_count[f] && pick < 0; ++s)
        if (slots[f][s].page < 0) pick = (int)s;
    if (pick < 0 || bytes > (f ? SLOT_RGB5A3 : SLOT_RGBA8)) return false;
    unsigned char *dst = slots[f][pick].mem;
    if (lz4_block_decode(pg->data, pg->packed, dst, bytes) != (long)bytes) return false;
    DCFlushRange(dst, bytes);
    GX_InitTexObj(&page_tex[i], dst, pg->w, pg->h, f ? GX_TF_RGB5A3 : GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GX_InitTexObjLOD(&page_tex[i], GX_LINEAR, GX_LINEAR, 0, 0, 0, GX_FALSE, GX_FALSE, GX_ANISO_1);
    slots[f][pick].page = (int)i;
    page_slot[i] = (short)pick;
    return true;
}
/* Keep the pages of frames [frame, frame + LOOK]; decode the ones the frame
   needs now, and a few of the ones after. */
static void cache(unsigned frame)
{
    const unsigned at = pack.clip[clip_cine].first + frame;
    bool changed = false;
    for (unsigned f = 0; f < 2; ++f)        /* free what is not wanted any more */
        for (unsigned s = 0; s < slot_count[f]; ++s) {
            const int p = slots[f][s].page;
            if (p < 0 || page_pinned[p] || tls_page_used(&pack.page[p], &pack, at, at + LOOK)) continue;
            slots[f][s].page = -1;
            page_slot[p] = -1;
        }
    unsigned ahead = 0;
    for (unsigned i = 0; i < pack.pages; ++i) {
        if (page_slot[i] >= 0) continue;
        const bool now = page_pinned[i] || tls_page_used(&pack.page[i], &pack, at, at);
        if (!now && (ahead >= AHEAD_PER_FRAME || !tls_page_used(&pack.page[i], &pack, at, at + LOOK))) continue;
        if (decode_page(i)) changed = true;
        if (!now) ++ahead;
    }
    if (changed) GX_InvalidateTexAll();
}
static void draw_frame(unsigned frame, Mat m, float alpha)
{
    const unsigned c = tls_chunk_of(&pack, frame);
    if ((int)c != chunk_loaded) {
        if (!tls_unpack_chunk(&pack, c, chunk_buf, chunk_cap)) return;
        chunk_loaded = (int)c;
    }
    unsigned n;
    const uint8_t *at = tls_frame(&pack, frame, chunk_buf, &n);
    for (unsigned i = 0; i < n && at; ++i) {
        TlDraw d;
        at = timeline_next(at, &d);
        const TlBake b = tls_bake(&pack, d.bake);
        if (b.page >= pack.pages || page_slot[b.page] < 0) continue;
        const TlsPage *pg = &pack.page[b.page];
        const Mat t = mul(m, mat(d.m));
        const float u0 = (float)b.x / pg->w, v0 = (float)b.y / pg->h, u1 = (float)(b.x + b.w) / pg->w,
                    v1 = (float)(b.y + b.h) / pg->h;
        const float mulc[4] = {d.mul[0], d.mul[1], d.mul[2], fmaxf(0, fminf(1, d.mul[3] * alpha * d.weight))};
        const float add[3] = {d.add[0], d.add[1], d.add[2]};
        if (mulc[3] <= 0.004f) continue;
        if (d.nverts) {   /* clipped by a mask: the polygon in clip px */
            float xs[16], ys[16], us[16], vs[16];
            const unsigned nv = d.nverts < 16 ? d.nverts : 16;
            for (unsigned k = 0; k < nv; ++k) {
                float x, y, u, v;
                timeline_vertex(&d, k, &x, &y, &u, &v);
                xs[k] = m.a * x + m.c * y + m.tx; ys[k] = m.b * x + m.d * y + m.ty;
                us[k] = u0 + u * (u1 - u0); vs[k] = v0 + v * (v1 - v0);
            }
            graphics_poly_cxform(&page_tex[b.page], nv, xs, ys, us, vs, mulc, add);
            continue;
        }
        const float x0 = b.ox, y0 = b.oy, x1 = b.ox + b.w * b.px, y1 = b.oy + b.h * b.px;
        const float xs[4] = {t.a * x0 + t.c * y0 + t.tx, t.a * x1 + t.c * y0 + t.tx, t.a * x1 + t.c * y1 + t.tx,
                             t.a * x0 + t.c * y1 + t.tx};
        const float ys[4] = {t.b * x0 + t.d * y0 + t.ty, t.b * x1 + t.d * y0 + t.ty, t.b * x1 + t.d * y1 + t.ty,
                             t.b * x0 + t.d * y1 + t.ty};
        graphics_quad_cxform(&page_tex[b.page], xs, ys, u0, v0, u1, v1, mulc, add);
    }
}
static const char *read_name(unsigned controller)
{
    return controller == CONTROLLER_CLASSIC ? "b" : controller == CONTROLLER_WIIMOTE ? "2" : "A";
}
void cinema_render(const Cinema *p, unsigned controller)
{
    const float width = graphics_width();
    graphics_set_zoom(1);
    graphics_rect(0, 0, width, 480, (GXColor){0, 0, 0, 255});
    if (!ready || !p->data) return;
    const unsigned frame = p->frame < pack.clip[clip_cine].count ? p->frame : pack.clip[clip_cine].count - 1;
    cache(frame);
    const Mat stage = {K, 0, 0, K, width / 2, 240};      /* the clip's (0, 0) at the stage centre */
    draw_frame(pack.clip[clip_cine].first + frame, stage, 1);
    /* 627: the box (its frame: the speaker's picture), the text, the prompt */
    const float a = p->alpha / 100;
    if (a <= 0.01f || clip_box < 0) return;
    const Mat box = mul(stage, mat(data->box));
    const unsigned pf = p->portrait >= 1 && p->portrait <= pack.clip[clip_box].count ? p->portrait - 1 : 0;
    draw_frame(pack.clip[clip_box].first + pf, box, a);
    char text[CIN_TEXT + 4];
    unsigned name_end;
    const bool short_ = p->length < 40;      /* txt2 += " \r ": a short message on the second line */
    if (short_) { text[0] = '\n'; }
    cinema_visible_text(p, text + (short_ ? 1 : 0), sizeof text - 1, &name_end);
    /* txt2 (623): 291 x 62 at (-145.45, -30.9) in the box */
    const float fx0 = box.a * -145.45f + box.tx, fx1 = box.a * 145.45f + box.tx, fy = box.d * -30.9f + box.ty;
    course_render_text(FONT_TALK, text, fx0, fx1, fy, (GXColor){255, 255, 255, (unsigned char)(255 * a)}, ALIGN_LEFT,
                       true);
    const float pa = a * p->prompt_alpha / 100;
    if (pa > 0.01f) {   /* bz at (143.45, 1): the read button */
        const float bx = box.a * 143.45f + box.tx, by = box.d * 1 + box.ty;
        course_render_text(FONT_SIGN, read_name(controller), bx - 40, bx + 40, by - 14,
                           (GXColor){255, 230, 90, (unsigned char)(255 * pa)}, ALIGN_CENTRE, true);
    }
}
