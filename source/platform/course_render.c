/* Draws an original course area as StartChar layers it (DOMDocument.xml:4889):
   FarBackground (fixed), Background (1/10 parallax), CloseBackground (1/2),
   BackGFX, Char, Water, FrontGFX; then the session's text and overlays. */
#include "course_render.h"
#include "course_art.h"
#include "graphics.h"
#include "lz4_block.h"
#include "user_art.h"
#include "course_8e1_art_bin.h"
#include "art_pack.h"
#include "object_art_ids.h"
#include "object_select.h"
#include "objects_8e1_0_bin.h"
#if OBJECT_ART_PACKS > 1
#include "objects_8e1_1_bin.h"
#endif
#if OBJECT_ART_PACKS > 2
#include "objects_8e1_2_bin.h"
#endif
#include <gccore.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define MEM2_RESERVE (26u * 1024 * 1024)
#define PICTURE_BYTES (1024u * 1024)
#define OBJECT_BYTES (5u * 1024 * 1024)
static CourseArtPack art;
static unsigned char *mem2, *picture_mem;
static GXTexObj layer_tex[ART_LAYER_PICTURE][COURSE_ART_MAX_TILES];
static const CourseArtLayer *layers[ART_LAYER_PICTURE];
static const CourseArea *loaded_area;
static GXTexObj picture_tex;
static ArtPack object_packs[OBJECT_ART_PACKS];
static GXTexObj object_tex[OBJECT_ART_PACKS];
static bool objects_ready;

/* Decode the three compressed object packs (art pack v3) into MEM2 once. */
static bool load_objects(unsigned char *dst, size_t cap)
{
    const unsigned char *blobs[OBJECT_ART_PACKS] = {objects_8e1_0_bin,
#if OBJECT_ART_PACKS > 1
        objects_8e1_1_bin,
#endif
#if OBJECT_ART_PACKS > 2
        objects_8e1_2_bin,
#endif
    };
    const unsigned sizes[OBJECT_ART_PACKS] = {objects_8e1_0_bin_size,
#if OBJECT_ART_PACKS > 1
        objects_8e1_1_bin_size,
#endif
#if OBJECT_ART_PACKS > 2
        objects_8e1_2_bin_size,
#endif
    };
#if OBJECT_ART_PACKS > 3
#error "add the extra object art packs here"
#endif
    size_t used = 0;
    for (unsigned i = 0; i < OBJECT_ART_PACKS; ++i) {
        ArtPack *p = &object_packs[i];
        if (!art_pack_parse(blobs[i], sizes[i], p) || !p->rgb5a3 || p->raw_size > cap - used) return false;
        if (lz4_block_decode(p->packed, p->packed_size, dst + used, p->raw_size) != (long)p->raw_size) return false;
        p->texture = dst + used;
        DCFlushRange(dst + used, p->raw_size);
        GX_InitTexObj(&object_tex[i], dst + used, p->width, p->height, GX_TF_RGB5A3, GX_CLAMP, GX_CLAMP, GX_FALSE);
        GX_InitTexObjLOD(&object_tex[i], GX_LINEAR, GX_LINEAR, 0, 0, 0, GX_FALSE, GX_FALSE, GX_ANISO_1);
        used = (used + p->raw_size + 31) & ~(size_t)31;
    }
    GX_InvalidateTexAll();
    return true;
}
/* Draw one object-art frame; (wx, wy) is the object's registration point in
   screen-space world px (before the course zoom). */
static void draw_object_frame(const ObjectDraw *d, float wx, float wy)
{
    if (!objects_ready || d->clip < 0 || d->clip >= OBJ_CLIP_COUNT || d->alpha <= 0) return;
    const unsigned pack = object_art_pack_of[d->clip];
    const ArtPack *p = &object_packs[pack];
    const ArtFrame *f = art_pack_frame_index(p, (unsigned)d->clip, d->frame);
    if (!f) return;
    const float k = COURSE_SCALE / OBJECT_ART_ZOOM * d->scale;  /* world px per image px */
    const float t = COURSE_SCALE / 20.0f * d->scale;            /* world px per twip */
    const float w = f->width * k, h = f->height * k;
    float x = f->offset_x * t, y = f->offset_y * t;
    float u0 = (float)f->x / p->width, u1 = (float)(f->x + f->width) / p->width;
    float v0 = (float)f->y / p->height, v1 = (float)(f->y + f->height) / p->height;
    if (d->flip_x) { x = -x - w; const float s = u0; u0 = u1; u1 = s; }
    if (d->flip_y) { y = -y - h; const float s = v0; v0 = v1; v1 = s; }
    const float a = fminf(100, d->alpha) * 2.55f;
    const float ox = wx + d->dx * COURSE_SCALE, oy = wy + d->dy * COURSE_SCALE; /* registration point */
    const float py = d->pivot_y * COURSE_SCALE * d->scale;
    const float lx[4] = {x, x + w, x + w, x}, ly[4] = {y - py, y - py, y + h - py, y + h - py};
    const float r = d->rotation * 0.01745329252f, cr = cosf(r), sr = sinf(r);
    float xs[4], ys[4];
    for (unsigned i = 0; i < 4; ++i) { xs[i] = ox + lx[i] * cr - ly[i] * sr; ys[i] = oy + py + lx[i] * sr + ly[i] * cr; }
    if (d->cxform) {
        const float mul[4] = {d->cx_mul[0], d->cx_mul[1], d->cx_mul[2], a / 255.0f};
        graphics_quad_cxform(&object_tex[pack], xs, ys, u0, v0, u1, v1, mul, d->cx_add);
    } else graphics_quad_alpha(&object_tex[pack], xs, ys, u0, v0, u1, v1, (unsigned char)a);
}
static void draw_objects(const CourseSession *s, float ox, float oy, bool front)
{
    for (unsigned i = 0; i < s->objects.count; ++i) {
        const LiveObject *l = &s->objects.live[i];
        if (!l->alive || course_string_equals(l->src->layer, "FrontGFX") != front) continue;
        ObjectDraw d[OBJECT_DRAW_MAX];
        const unsigned n = object_select(l, d);
        for (unsigned k = 0; k < n; ++k) draw_object_frame(&d[k], ox + l->x * COURSE_SCALE, oy + l->y * COURSE_SCALE);
    }
}
static const CourseArtLayer *loaded_picture;

bool course_render_init(void)
{
    if (!course_art_parse(course_8e1_art_bin, course_8e1_art_bin_size, &art)) return false;
    /* Reserve MEM2 for one area's textures plus a picture page, once. */
    unsigned char *lo = (unsigned char *)(((uintptr_t)SYS_GetArena2Lo() + 31) & ~(uintptr_t)31);
    if ((unsigned char *)SYS_GetArena2Hi() - lo < (ptrdiff_t)(MEM2_RESERVE + PICTURE_BYTES)) return false;
    if ((unsigned char *)SYS_GetArena2Hi() - lo < (ptrdiff_t)(MEM2_RESERVE + PICTURE_BYTES + OBJECT_BYTES)) return false;
    mem2 = lo; picture_mem = lo + MEM2_RESERVE;
    SYS_SetArena2Lo(lo + MEM2_RESERVE + PICTURE_BYTES + OBJECT_BYTES);
    objects_ready = load_objects(lo + MEM2_RESERVE + PICTURE_BYTES, OBJECT_BYTES);
    user_art_set_scale(COURSE_SCALE);
    return true;
}
static bool upload(const CourseArtTile *t, unsigned char *dst, size_t cap, GXTexObj *tex)
{
    if (t->size > cap || lz4_block_decode(t->data, t->packed, dst, t->size) != (long)t->size) return false;
    DCFlushRange(dst, t->size);
    GX_InitTexObj(tex, dst, t->width, t->height, GX_TF_RGB5A3, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GX_InitTexObjLOD(tex, GX_LINEAR, GX_LINEAR, 0, 0, 0, GX_FALSE, GX_FALSE, GX_ANISO_1);
    return true;
}
static void load_area(const CourseArea *a)
{
    char id[32] = {0};
    memcpy(id, a->id.text, a->id.length < 31 ? a->id.length : 31);
    size_t used = 0;
    for (unsigned k = 0; k < ART_LAYER_PICTURE; ++k) {
        layers[k] = course_art_layer(&art, id, (CourseArtKind)k);
        if (!layers[k]) continue;
        for (unsigned t = 0; t < layers[k]->tile_count; ++t) {
            const CourseArtTile *tile = &layers[k]->tiles[t];
            if (!upload(tile, mem2 + used, MEM2_RESERVE - used, &layer_tex[k][t])) {layers[k] = NULL; break;}
            used = (used + tile->size + 31) & ~(size_t)31;
        }
    }
    GX_InvalidateTexAll();
    loaded_area = a;
}
/* Draw one layer. `anchor_*`: world px of the layer's attach point (Course
   origin for BackGFX/FrontGFX; the view centre minus C*parallax for backgrounds). */
static void draw_layer(CourseArtKind k, float anchor_x, float anchor_y, float scale)
{
    const CourseArtLayer *L = layers[k];
    if (!L) return;
    const float px = COURSE_SCALE / L->zoom * scale; /* world px per image px */
    for (unsigned t = 0; t < L->tile_count; ++t) {
        const CourseArtTile *tile = &L->tiles[t];
        graphics_sprite(&layer_tex[k][t], anchor_x + (L->ox * L->zoom + tile->x) * px,
                        anchor_y + (L->oy * L->zoom + tile->y) * px, tile->width * px, tile->height * px, 0, 0, 1, 1);
    }
}
static void text_block(float x, float y, CourseString s, float scale, GXColor color, unsigned wrap)
{
    char line[96]; unsigned n = 0; bool tag = false;
    for (unsigned i = 0; i <= s.length; ++i) {
        const char c = i < s.length ? s.text[i] : '\n';
        if (c == '<') {tag = true; continue;}
        if (c == '>') {tag = false; continue;}
        if (tag) continue;
        if (c == '\n' || n >= wrap || n >= sizeof(line) - 1) {
            /* break at the last space when wrapping mid-word */
            unsigned cut = n;
            if (c != '\n' && n >= wrap) for (unsigned k = n; k > 0; --k) if (line[k - 1] == ' ') {cut = k - 1; break;}
            line[cut] = 0;
            graphics_text(x, y, line, scale, color);
            y += 10 * scale;
            unsigned rest = cut < n ? n - cut - 1 : 0;
            memmove(line, line + n - rest, rest); n = rest;
            if (c == '\n') continue;
        }
        line[n++] = (c >= 'a' && c <= 'z') ? (char)(c - 32) : c;
    }
}
void course_render(const CourseSession *s, float t)
{
    const CourseRuntime *c = &s->runtime;
    if (c->area != loaded_area) load_area(c->area);
    const Game *g = &s->game;
    const float width = graphics_width(), zoom = course_view_zoom();
    const float vw = width / zoom, vh = 480.0f / zoom;
    const float camx = g->camera.previous_x + (g->camera.x - g->camera.previous_x) * t;
    const float camy = g->camera.previous_y + (g->camera.y - g->camera.previous_y) * t;
    const float ccx = course_cx(c, camx + vw / 2), ccy = course_cy(c, camy + vh / 2); /* camera centre */
    graphics_rect(0, 0, width, 480, (GXColor){0, 0, 0, 255});
    graphics_set_zoom(zoom);
    /* Far background: fixed at the view centre; widened to fill 16:9. */
    const float far_scale = fmaxf(1, vw / COURSE_SCALE / 450.0f);
    draw_layer(ART_LAYER_FAR, vw / 2, vh / 2, far_scale);
    const float sx = vw / 2, sy = vh / 2;
    draw_layer(ART_LAYER_BG, sx - ccx * 0.1f * COURSE_SCALE, sy - ccy * 0.1f * COURSE_SCALE, 1);
    draw_layer(ART_LAYER_MID, sx - ccx * 0.2f * COURSE_SCALE, sy - ccy * 0.2f * COURSE_SCALE, 1);
    draw_layer(ART_LAYER_CLOSE, sx - ccx * 0.5f * COURSE_SCALE, sy - ccy * 0.5f * COURSE_SCALE, 1);
    const float ox = course_wx(c, 0) - camx, oy = course_wy(c, 0) - camy; /* course origin on screen */
    draw_layer(ART_LAYER_BACK, ox, oy, 1);
    draw_objects(s, ox, oy, false);
    const Player *p = &g->player;
    const float px = p->previous_x + (p->x - p->previous_x) * t - camx;
    const float py = p->previous_y + (p->y - p->previous_y) * t - camy;
    if (!(p->blink > 0 && (p->blink / 2) % 2)) user_art_draw_player(p, px, py); /* MarioHit blink */
    /* Water Code volumes (surface waves still to be ported): flat translucent fill. */
    for (unsigned i = 0; i < c->area->water_count; ++i) {
        const CourseWater *w = &c->area->water[i];
        graphics_rect(ox + w->x * COURSE_SCALE, oy + w->y * COURSE_SCALE, w->w * COURSE_SCALE, w->h * COURSE_SCALE,
                      (GXColor){40, 110, 220, 110});
    }
    draw_layer(ART_LAYER_FRONT, ox, oy, 1);
    draw_objects(s, ox, oy, true);
    graphics_set_zoom(1);
    const GXColor white = {255, 255, 255, 255};
    if (s->hint.length) {
        graphics_rect(20, 420, width - 40, 40, (GXColor){0, 0, 0, 150});
        text_block(30, 428, s->hint, 1.5f, white, (unsigned)((width - 60) / 9));
    }
    { /* HUD (placeholder layout; original Stats clip art still to be ported) */
        char hud[48];
        snprintf(hud, sizeof(hud), "COINS %u   HP %u", s->progress.coins, s->progress.hp);
        graphics_rect(14, 14, 220, 22, (GXColor){0, 0, 0, 140});
        graphics_text(22, 19, hud, 2, white);
    }
    if (s->state == SESSION_TALK && s->toad_text.length) {
        graphics_rect(40, 330, width - 80, 120, (GXColor){20, 20, 40, 220});
        text_block(56, 346, s->toad_text, 2, white, (unsigned)((width - 112) / 12));
    }
    if (s->area_title_time > 0)
        text_block(width / 2 - 100, 60, s->area_title, 3, white, 30);
    if (s->state == SESSION_SIGN) {
        if (s->sign_picture) {
            const CourseArtLayer *pic = course_art_picture(&art, s->sign_text.text, s->sign_text.length);
            if (pic && pic != loaded_picture && pic->tile_count == 1 &&
                upload(&pic->tiles[0], picture_mem, PICTURE_BYTES, &picture_tex)) {
                GX_InvalidateTexAll(); loaded_picture = pic;
            }
            if (pic && pic == loaded_picture) {
                const float w = fminf(width - 40, 600), h = w * pic->height / pic->width;
                graphics_sprite(&picture_tex, (width - w) / 2, (480 - h) / 2, w, h, 0, 0, 1, 1);
            }
        } else {
            graphics_rect(40, 120, width - 80, 200, (GXColor){20, 20, 40, 220});
            text_block(56, 136, s->sign_text, 2, white, (unsigned)((width - 112) / 12));
        }
    }
    if (s->state == SESSION_WARP) { /* righttransition: wipe to black and back */
        const float k = fminf(1, s->warp_time / COURSE_WARP_SECONDS);
        graphics_rect(0, 0, width * k, 480, (GXColor){0, 0, 0, 255});
    }
    if (s->state == SESSION_COMPLETE) {
        graphics_rect(0, 0, width, 480, (GXColor){0, 0, 0, 230});
        graphics_text(width / 2 - 228, 200, "TUTORIAL COMPLETE", 4, white);
        graphics_text(width / 2 - 270, 260, "INTRO MOVIE 2 AND THE CASTLE ARE NOT IN THIS PORT YET", 1.5f, white);
    }
}
