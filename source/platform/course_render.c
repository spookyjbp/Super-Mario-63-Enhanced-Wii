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
#include "water_places.h"
#include "font_atlas_bin.h"
#include "lz4w.h"
#include "trophies_bin.h"
#include "pause_layout.h"
#include "text_layout.h"
#include "sign_layout.h"
#include "faint_layout.h"
#include "area_text_layout.h"
#include "bg_water_places.h"
#include "ui_layout.h"
#include "objects_8e1_0_bin.h"
#if OBJECT_ART_PACKS > 1
#include "objects_8e1_1_bin.h"
#endif
#if OBJECT_ART_PACKS > 2
#include "objects_8e1_2_bin.h"
#endif
#if OBJECT_ART_PACKS > 3
#include "objects_8e1_3_bin.h"
#endif
#if OBJECT_ART_PACKS > 4
#include "objects_8e1_4_bin.h"
#endif
#include <gccore.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define MEM2_RESERVE COURSE_MEM2_RESERVE
#define PICTURE_BYTES (1024u * 1024)
#define OBJECT_BYTES (17u * 512 * 1024)   /* the packs' decoded textures (tests/test_object_art.py checks they fit) */
#define PROMPT_BYTES (128u * 1024)
#define ANIM_BYTES (320u * 1024)
#define PAUSE_PARTS_BYTES (768u * 1024)   /* "PauseParts": up to 1024 x 384 RGB5A3 (tests/test_pause_art.py) */
static CourseArtPack art;
static unsigned char *mem2, *picture_mem, *prompt_mem, *anim_mem;
static GXTexObj anim_tex;
static const CourseArtTile *loaded_anim;
static GXTexObj layer_tex[ART_LAYER_PICTURE][COURSE_ART_MAX_TILES];
static const CourseArtLayer *layers[ART_LAYER_PICTURE];
static const CourseArea *loaded_area;
static GXTexObj picture_tex;
static ArtPack object_packs[OBJECT_ART_PACKS];
static GXTexObj object_tex[OBJECT_ART_PACKS];
static bool objects_ready;
static GXTexObj water_tex, font_tex, prompt_tex, trophy_tex, parts_tex;
static unsigned char *parts_mem;
static bool parts_ready;
static const CourseArtLayer *loaded_prompt;

/* Decode the compressed object packs (art pack v3) into MEM2 once. */
static bool load_objects(unsigned char *dst, size_t cap)
{
    const unsigned char *blobs[OBJECT_ART_PACKS] = {objects_8e1_0_bin,
#if OBJECT_ART_PACKS > 1
        objects_8e1_1_bin,
#endif
#if OBJECT_ART_PACKS > 2
        objects_8e1_2_bin,
#endif
#if OBJECT_ART_PACKS > 3
        objects_8e1_3_bin,
#endif
#if OBJECT_ART_PACKS > 4
        objects_8e1_4_bin,
#endif
    };
    const unsigned sizes[OBJECT_ART_PACKS] = {objects_8e1_0_bin_size,
#if OBJECT_ART_PACKS > 1
        objects_8e1_1_bin_size,
#endif
#if OBJECT_ART_PACKS > 2
        objects_8e1_2_bin_size,
#endif
#if OBJECT_ART_PACKS > 3
        objects_8e1_3_bin_size,
#endif
#if OBJECT_ART_PACKS > 4
        objects_8e1_4_bin_size,
#endif
    };
#if OBJECT_ART_PACKS > 5
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
        const unsigned n = object_select(l, s->controller, d);
        for (unsigned k = 0; k < n; ++k) draw_object_frame(&d[k], ox + l->x * COURSE_SCALE, oy + l->y * COURSE_SCALE);
    }
}
/* Stats Clip (SWF 800): stage px (450 x 300) at 1.6 screen px each. Elements
   left of x 150 keep to the left edge, right of 300 to the right edge, the
   POWER meter to the centre, so 16:9 keeps the original corners. */
#define HUD_K 1.6f
static float hud_x(float x, float width)
{
    return x < 150 ? x * HUD_K : x > 300 ? width - (450 - x) * HUD_K : width / 2 + (x - 225) * HUD_K;
}
static void hud_clip(int clip, unsigned frame, float x, float y, float width, float scale)
{
    const ObjectDraw d = {.clip = clip, .frame = frame, .alpha = 100, .scale = HUD_K / COURSE_SCALE * scale};
    draw_object_frame(&d, hud_x(x, width), y * HUD_K);
}
/* A DefineEditText counter (font 187, 16 px): digits from the field's left edge. */
static void hud_number(unsigned value, float x, float y, float width, float scale, bool centre)
{
    char buf[12];
    const int n = snprintf(buf, sizeof(buf), "%u", value);
    const float advance = 8.9f * scale; /* Arial digit advance, 0.556 em */
    float left = centre ? x - n * advance / 2 : x;
    for (int i = 0; i < n; ++i) hud_clip(OBJ_HUD_DIGITS, (unsigned)(buf[i] - '0'), left + i * advance, y, width, scale);
}
/* Lines the text wraps to in [x0, x1] (screen px). */
static unsigned text_lines(unsigned font, const char *src, size_t len, float wrap_width)
{
    static char plain[2048];
    TextLine lines[32];
    const unsigned n = text_plain(src, len, plain, sizeof plain);
    return text_wrap(&font_info[font], plain, n, wrap_width, lines, 32);
}
static void draw_cstring(unsigned font, const char *t, float x0, float x1, float y, GXColor c, int align);
static void draw_hud(const CourseSession *s, float width)
{
    /* Shine and star coin icons with "=" (left), TotalStars / TotalStarCoins. */
    hud_clip(OBJ_HUD_STATIC, 0, 0, 0, width, 1);
    hud_number(s->total_stars, 45.3f, 12.7f, width, 1, false);
    hud_number(0, 136.7f, 11.2f, width, 1, false);                  /* TotalStarCoins */
    /* Coins: spinning coin (SWF 17), "=", _root.Coins. */
    hud_clip(OBJ_COIN_SPIN, s->hud.ticks % 16, 342.6f, 10.3f, width, 1);
    hud_clip(OBJ_HUD_DIGITS, 10, 349.5f, 2.9f, width, 1);
    hud_number(s->progress.coins, 364.1f, 2.9f, width, 1, false);
    /* POWER meter (114 at (225, 25)): the WaterHP pie (113, depth 1) under the
       CharHP pie (111, depth 5), each where 114's frame places it. */
    {
        float mx, my;
        if (hud_water_place(&s->hud, &mx, &my)) hud_clip(OBJ_HUD_WATER_PIE, hud_water_frame(&s->hud), 225 + mx, 25 + my, width, 1);
        if (hud_hp_place(&s->hud, &mx, &my)) hud_clip(OBJ_HUD_PIE, hud_pie_frame(&s->hud), 225 + mx, 25 + my, width, 1);
    }
    /* FLUDD tank (160 at (431.2, 266.9)): level 'cl', power gauge, percentage. */
    const Player *p = &s->game.player;
    if (p->has_fludd) {
        int level = (int)lroundf(p->water * 5);
        level = level < 2 ? 2 : level > 500 ? 500 : level;
        const int power = (int)lroundf(p->power);
        hud_clip(OBJ_HUD_TANK, 0, 431.2f, 266.9f, width, 1);
        hud_clip(OBJ_HUD_TANK_LEVEL, (unsigned)(level - 1) / 10, 431.2f, 266.9f, width, 1);
        hud_clip(OBJ_HUD_POWER_BAR, (unsigned)(power < 0 ? 0 : power > 100 ? 100 : power) / 5, 431.2f, 266.9f, width, 1);
        hud_number((unsigned)lroundf(p->water), 431.2f, 266.9f - 20.7f + 1, width, 15.0f / 16, true);
    }
    if (s->nozzle_banner) { /* port HUD: which nozzle was just selected (1.5 s) */
        static const char *const names[] = {"No Nozzle", "Hover Nozzle", "Rocket Nozzle", "Turbo Nozzle"};
        const char *name = names[s->banner_fludd < 4 ? s->banner_fludd : 0];
        const unsigned char a = (unsigned char)(s->nozzle_banner > 20 ? 255 : s->nozzle_banner * 12);
        const float tw = text_width(&font_info[FONT_SIGN], name, (unsigned)strlen(name)), x = width - 24 - tw, y = 480 - 190;
        graphics_rect(x - 10, y - 6, tw + 20, 36, (GXColor){0, 0, 0, (unsigned char)(a / 2)});
        draw_cstring(FONT_SIGN, name, x, x + tw + 1, y, (GXColor){120, 200, 255, a}, ALIGN_LEFT);
    }
}
static const CourseArtLayer *loaded_picture;

bool course_render_init(void)
{
    if (!course_art_parse(course_8e1_art_bin, course_8e1_art_bin_size, &art)) return false;
    /* Reserve MEM2 for one area's textures plus a picture page, once. */
    unsigned char *lo = (unsigned char *)(((uintptr_t)SYS_GetArena2Lo() + 31) & ~(uintptr_t)31);
    if ((unsigned char *)SYS_GetArena2Hi() - lo < (ptrdiff_t)(MEM2_RESERVE + PICTURE_BYTES)) return false;
    if ((unsigned char *)SYS_GetArena2Hi() - lo <
        (ptrdiff_t)(MEM2_RESERVE + PICTURE_BYTES + OBJECT_BYTES + PROMPT_BYTES + ANIM_BYTES + PAUSE_PARTS_BYTES))
        return false;
    mem2 = lo; picture_mem = lo + MEM2_RESERVE; prompt_mem = lo + MEM2_RESERVE + PICTURE_BYTES + OBJECT_BYTES;
    anim_mem = prompt_mem + PROMPT_BYTES;
    parts_mem = anim_mem + ANIM_BYTES;
    SYS_SetArena2Lo(lo + MEM2_RESERVE + PICTURE_BYTES + OBJECT_BYTES + PROMPT_BYTES + ANIM_BYTES + PAUSE_PARTS_BYTES);
    objects_ready = load_objects(lo + MEM2_RESERVE + PICTURE_BYTES, OBJECT_BYTES);
    /* The original font 155 (tools/export_fonts.py): RGBA8 after a 32-byte header. */
    size_t font_size;
    const unsigned char *font_data = lz4w_open(font_atlas_bin, font_atlas_bin_size, &font_size);   /* MEM2 */
    if (!font_data || font_size < 32 + FONT_ATLAS_W * FONT_ATLAS_H * 4) return false;
    DCFlushRange((void *)font_data, font_size);
    GX_InitTexObj(&font_tex, (void *)(font_data + 32), FONT_ATLAS_W, FONT_ATLAS_H, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP,
                  GX_FALSE);
    GX_InitTexObjLOD(&font_tex, GX_LINEAR, GX_LINEAR, 0, 0, 0, GX_FALSE, GX_FALSE, GX_ANISO_1);
    /* The four trophies (tools/export_trophies.py): one RGBA8 strip, cells side by side. */
    {
        const unsigned cell = (unsigned)trophies_bin[8] << 24 | (unsigned)trophies_bin[9] << 16 |
                              (unsigned)trophies_bin[10] << 8 | trophies_bin[11];
        GX_InitTexObj(&trophy_tex, (void *)(trophies_bin + 32), (u16)(cell * ACH_TIERS), (u16)cell, GX_TF_RGBA8,
                      GX_CLAMP, GX_CLAMP, GX_FALSE);
        GX_InitTexObjLOD(&trophy_tex, GX_LINEAR, GX_LINEAR, 0, 0, 0, GX_FALSE, GX_FALSE, GX_ANISO_1);
    }
    /* Water textures repeat across a volume (GX_REPEAT); data lives in the DOL. */
    /* bitmap 2062 of the water blocks: non-smoothed, repeated */
    DCFlushRange((void *)water_tile, sizeof(water_tile));
    GX_InitTexObj(&water_tex, (void *)water_tile, WATER_TILE_W, WATER_TILE_H, GX_TF_RGB5A3, GX_REPEAT, GX_REPEAT, GX_FALSE);
    GX_InitTexObjLOD(&water_tex, GX_NEAR, GX_NEAR, 0, 0, 0, GX_FALSE, GX_FALSE, GX_ANISO_1);
    user_art_set_scale(COURSE_SCALE);
    return true;
}
static bool upload(const CourseArtTile *t, unsigned char *dst, size_t cap, GXTexObj *tex)
{
    if (t->size > cap || lz4_block_decode(t->data, t->packed, dst, t->size) != (long)t->size) return false;
    DCFlushRange(dst, t->size);
    GX_InitTexObj(tex, dst, t->width, t->height, t->size == t->width * t->height * 4 ? GX_TF_RGBA8 : GX_TF_RGB5A3,
                  GX_CLAMP, GX_CLAMP, GX_FALSE);
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
        /* Neighbouring tiles share edge coordinates exactly (same expression for
           one tile's right edge and the next one's left edge), so no hairline
           opens between them at any camera position. */
        float xs[4], ys[4];
        course_layer_tile_quad(anchor_x + L->ox * L->zoom * px, anchor_y + L->oy * L->zoom * px, px,
                               tile->x, tile->y, tile->width, tile->height, xs, ys);
        graphics_quad_alpha(&layer_tex[k][t], xs, ys, 0, 0, 1, 1, 255);
    }
}
/* Stage x (0..450) to screen x, centred on wide screens (the message box,
   picture pages and their prompts are centred on the original stage). */
static float stage_x(float x, float width) { return width / 2 + (x - 225) * HUD_K; }
/* Text in the original font 155 (text_layout.c): HTML dropped, wrapped to
   [x0, x1] (screen px), first line's top at y; at most max_chars plain characters. */
static void draw_text(unsigned font, const char *src, size_t len, float x0, float x1, float y, GXColor colour, int align,
                      unsigned max_chars)
{
    static char plain[2048];
    TextLine lines[32];
    unsigned n = text_plain(src, len, plain, sizeof plain);
    if (max_chars < n) n = max_chars;
    const FontInfo *f = &font_info[font];
    const unsigned count = text_wrap(f, plain, n, x1 - x0, lines, 32);
    const float lh = text_line_height(f, 2 * HUD_K);              /* Flash leading 2 */
    const float mul[4] = {colour.r / 255.0f, colour.g / 255.0f, colour.b / 255.0f, colour.a / 255.0f}, add[3] = {0, 0, 0};
    for (unsigned l = 0; l < count; ++l) {
        float x = align == ALIGN_CENTRE ? (x0 + x1 - lines[l].width) / 2 : align == ALIGN_RIGHT ? x1 - lines[l].width : x0;
        const float base = y + f->ascent + (float)l * lh;
        for (unsigned k = lines[l].start; k < lines[l].end; ++k) {
            const unsigned char c = (unsigned char)plain[k];
            if (c < FONT_FIRST || c > FONT_LAST) continue;
            const FontGlyph *g = &f->glyphs[c - FONT_FIRST];
            if (g->w) {
                const float gx = roundf(x) + g->ox, gy = roundf(base) + g->oy;
                const float xs[4] = {gx, gx + g->w, gx + g->w, gx}, ys[4] = {gy, gy, gy + g->h, gy + g->h};
                graphics_quad_cxform(&font_tex, xs, ys, (float)g->x / FONT_ATLAS_W, (float)g->y / FONT_ATLAS_H,
                                     (float)(g->x + g->w) / FONT_ATLAS_W, (float)(g->y + g->h) / FONT_ATLAS_H, mul, add);
            }
            x += g->advance;
        }
    }
}
/* A text field with the black Glow filter the original puts on its HUD text
   (blur 2, strength 10: a solid dark outline about 2 stage px wide), then the
   text. */
static void draw_text_glow(unsigned font, const char *src, size_t len, float x0, float x1, float y, GXColor colour,
                           int align, unsigned max_chars)
{
    static const float ring[8][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {0.7f, 0.7f}, {-0.7f, 0.7f}, {0.7f, -0.7f}, {-0.7f, -0.7f}};
    const float r = 1.25f * HUD_K;
    for (unsigned i = 0; i < 8; ++i)
        draw_text(font, src, len, x0 + ring[i][0] * r, x1 + ring[i][0] * r, y + ring[i][1] * r,
                  (GXColor){0, 0, 0, colour.a}, align, max_chars);
    draw_text(font, src, len, x0, x1, y, colour, align, max_chars);
}
static void draw_cstring(unsigned font, const char *t, float x0, float x1, float y, GXColor c, int align)
{ draw_text(font, t, strlen(t), x0, x1, y, c, align, ~0u); }
/* A picture-slot layer (picture page or message box), one texture at a time. */
static bool load_picture(const CourseArtLayer *pic)
{
    if (!pic || pic->tile_count != 1) return false;
    if (pic != loaded_picture) {
        if (!upload(&pic->tiles[0], picture_mem, PICTURE_BYTES, &picture_tex)) return false;
        GX_InvalidateTexAll(); loaded_picture = pic;
    }
    return true;
}
/* The prompt slot: the "Press .. to Continue" prompt or Toad's talk button, one at a time. */
static bool load_prompt_layer(const CourseArtLayer *l)
{
    if (!l || l->tile_count != 1) return false;
    if (l != loaded_prompt) {
        if (!upload(&l->tiles[0], prompt_mem, PROMPT_BYTES, &prompt_tex)) return false;
        GX_InvalidateTexAll(); loaded_prompt = l;
    }
    return true;
}
/* The "Press .. to Continue" prompt (250) for the controller in use, placed at
   its page's position, faded as the original's frames. */
static void draw_prompt(const CourseSession *s, const char *page, size_t page_len, float width,
                        const CourseArtLayer *picture)
{
    int f = course_prompt_frame(s);
    if (f < 0) return;
    if (f >= PROMPT_FRAMES) f = PROMPT_LOOP + (f - PROMPT_LOOP) % (PROMPT_FRAMES - PROMPT_LOOP);
    const unsigned char a = prompt_alpha[f];
    if (!a) return;
    const PromptPlace *place = NULL;
    for (unsigned i = 0; i < PROMPT_PLACES; ++i)
        if (strlen(prompt_places[i].page) == page_len && !memcmp(prompt_places[i].page, page, page_len)) place = &prompt_places[i];
    /* Every picture sign shows it (user request), also the pages whose original
       has none (stomp, hover, zoom, refill): under the page. */
    if (!place && !picture) return;
    char key[12];
    snprintf(key, sizeof key, "Prompt@%u", (unsigned)s->controller);
    const CourseArtLayer *l = course_art_picture(&art, key, strlen(key));
    if (!load_prompt_layer(l)) return;
    float x = 0, y = 0;
    if (picture) {   /* on picture signs: under the page (docs/sign-design) */
        float rx, ry;
        sign_prompt_fallback(picture->ox, picture->width / HUD_K, l->width / HUD_K, l->height / HUD_K, l->ox, l->oy, &rx, &ry);
        if (place) { rx = place->x; ry = place->y; }
        sign_prompt_picture(picture->ox, picture->oy, picture->width / HUD_K, picture->height / HUD_K,
                            l->width / HUD_K, l->height / HUD_K, rx, ry, l->ox, l->oy, &x, &y);
    } else { x = place->x + l->ox; y = place->y + l->oy; }
    graphics_sprite_alpha(&prompt_tex, stage_x(x, width), y * HUD_K, (float)l->width, (float)l->height, 0, 0, 1, 1, a);
}
/* One frame of a full-screen clip ("<name>~" layer, tools/faint_art.py) at its stage place. */
static void draw_fx(const char *name, unsigned frame, float width)
{
    const CourseArtLayer *L = course_art_picture(&art, name, strlen(name));
    if (!L || frame >= L->tile_count) return;
    const CourseArtTile *T = &L->tiles[frame];
    if (T != loaded_anim) {
        if (!upload(T, anim_mem, ANIM_BYTES, &anim_tex)) return;
        GX_InvalidateTexAll(); loaded_anim = T;
    }
    const float k = HUD_K / L->zoom;   /* screen px per image px */
    graphics_sprite(&anim_tex, stage_x(L->ox, width) + T->x * k, L->oy * HUD_K + T->y * k, T->width * k, T->height * k,
                    0, 0, 1, 1);
}
/* The level water over Mario (Water is attached above Char, under FrontGFX):
   each Water Code strip as a polygon from its wave points down to
   WaterDeepness, filled with its colour, or, where it masks a water block,
   showing the block: two layers of the repeating bitmap at 30 % that slide
   diagonally (water_places.h). ox, oy: the course origin on screen. */
static void draw_level_water(const CourseSession *s, float ox, float oy)
{
    for (unsigned k = 0; k < s->water_count; ++k) {
        const WaterPlace *wp = &water_places[s->water_index[k]];
        const BgWave *w = &s->water_wave[k];
        const WaterBlock *blk = wp->block >= 0 ? &water_blocks[(unsigned)wp->block] : 0;
        for (unsigned j = 0; j + 1 < w->n; ++j) {
            const float cx[4] = {wp->x + w->x[j], wp->x + w->x[j + 1], wp->x + w->x[j + 1], wp->x + w->x[j]};
            const float cy[4] = {wp->y + w->y[j], wp->y + w->y[j + 1], wp->y + wp->deep, wp->y + wp->deep};
            float xs[4], ys[4];
            for (unsigned i = 0; i < 4; ++i) { xs[i] = ox + cx[i] * COURSE_SCALE; ys[i] = oy + cy[i] * COURSE_SCALE; }
            if (!blk) {
                graphics_quad_color(xs, ys, (GXColor){wp->rgba[0], wp->rgba[1], wp->rgba[2], wp->rgba[3]});
                continue;
            }
            for (unsigned l = 0; l < 2; ++l) {
                const WaterBlockLayer *L = &blk->layers[l];
                const float *off = L->offs[s->water_frame % WATER_BLOCK_FRAMES];
                float us[4], vs[4];
                for (unsigned i = 0; i < 4; ++i)
                    course_water_block_uv(blk->x, blk->y, off[0], off[1], L->sx, L->origin_x, L->origin_y, L->tile,
                                          cx[i], cy[i], &us[i], &vs[i]);
                graphics_quad_uv(&water_tex, xs, ys, us, vs, (unsigned char)(L->alpha * 255 + 0.5f));
            }
        }
    }
}
/* ---- The pause menu (795 / 794; pause_menu.c, tools/pause_art.py) ---- */
static const CourseArtLayer *parts_layer;
static bool load_parts(void)
{
    if (parts_ready) return true;
    parts_layer = course_art_picture(&art, "PauseParts", 10);
    if (!parts_layer || parts_layer->tile_count != 1 ||
        !upload(&parts_layer->tiles[0], parts_mem, PAUSE_PARTS_BYTES, &parts_tex)) return false;
    GX_InvalidateTexAll();
    return parts_ready = true;
}
/* A part with its registration point at screen (rx, ry), k screen px per 794 px. */
static void part_screen(unsigned id, float rx, float ry, float k, unsigned char alpha)
{
    if (id >= PAUSE_PARTS || !alpha) return;
    const PausePart *pp = &pause_parts[id];
    const float aw = (float)parts_layer->width, ah = (float)parts_layer->height;
    graphics_sprite_alpha(&parts_tex, rx + pp->ox * k, ry + pp->oy * k, pp->w / PAUSE_ZOOM * k, pp->h / PAUSE_ZOOM * k,
                          pp->x / aw, pp->y / ah, (pp->x + pp->w) / aw, (pp->y + pp->h) / ah, alpha);
}
/* The port's pause layout over the 16:9 screen (pause_menu.h): the column of
   buttons on the left, the page's content (794 px) scaled by PAGE_K to the
   right of it. */
#define PAGE_K 0.86f
#define COLUMN_K 0.82f
typedef struct { float margin, col_x, page_x, dy; } PauseView;
static PauseView pause_view(float width, float dy)
{
    PauseView v;
    v.margin = 0.05f * width;
    v.col_x = v.margin + 121.25f * HUD_K * COLUMN_K / 2 + 4;     /* the buttons are 121 px wide */
    const float left = v.col_x + 121.25f * HUD_K * COLUMN_K / 2 + 12;
    v.page_x = (left + width - v.margin) / 2;
    v.dy = dy * HUD_K;
    return v;
}
static float page_sx(const PauseView *v, float x) { return v->page_x + x * HUD_K * PAGE_K; }
static float page_sy(const PauseView *v, float y) { return (y + 150) * HUD_K * PAGE_K + 8 + v->dy; }
static void draw_part(const PauseView *v, unsigned id, float x, float y, float scale, unsigned char alpha)
{
    part_screen(id, page_sx(v, x), page_sy(v, y), HUD_K * PAGE_K * scale / 100, alpha);
}
/* One trophy (tier) in a size x size screen box; locked ones dimmed. */
static void draw_trophy(unsigned tier, float x, float y, float size, bool lit, float a)
{
    const float u0 = (float)tier / ACH_TIERS, u1 = (float)(tier + 1) / ACH_TIERS;
    const float xs[4] = {x, x + size, x + size, x}, ys[4] = {y, y, y + size, y + size};
    const float on[4] = {1, 1, 1, a}, off[4] = {0.38f, 0.38f, 0.42f, a * 0.7f}, add[3] = {0, 0, 0};
    graphics_quad_cxform(&trophy_tex, xs, ys, u0, 0, u1, 1, lit ? on : off, add);
}
static const unsigned char tier_rgb[ACH_TIERS][3] = {{222, 150, 95}, {205, 212, 222}, {255, 214, 70}, {150, 205, 255}};
/* The controller's buttons in the menu: jump chooses, spin goes back. */
static void menu_buttons(unsigned controller, const char **choose, const char **back)
{
    switch (controller) {
    case CONTROLLER_CLASSIC: *choose = "b"; *back = "y"; return;
    case CONTROLLER_WIIMOTE: *choose = "2"; *back = "1"; return;
    default: *choose = "A"; *back = "B"; return;
    }
}
/* ---- the pages: rows in the Achievements page's style (pause_menu.h) ---- */
typedef struct { float x0, x1, top, rows_y; float a; unsigned char al; } PageBox;
#define ROW_H 58.0f
/* The page's title (its original piece), a summary line and the box under it. */
static PageBox page_box(const PauseView *v, unsigned title, const char *left, const char *right, float a, float width)
{
    PageBox b;
    b.a = a; b.al = (unsigned char)(255 * a);
    draw_part(v, title, 0, -124, 100, b.al);
    b.x1 = width - v->margin; b.x0 = 2 * v->page_x - b.x1;
    b.top = page_sy(v, -100);
    const GXColor wt = {255, 255, 255, b.al};
    if (left) draw_text_glow(FONT_HINT, left, strlen(left), b.x0, b.x1, b.top, wt, ALIGN_LEFT, ~0u);
    if (right) draw_text_glow(FONT_HINT, right, strlen(right), b.x0, b.x1, b.top, wt, ALIGN_RIGHT, ~0u);
    b.rows_y = b.top + 28;
    return b;
}
/* Row r's band: dark; the chosen one blue with light edges. Returns its top. */
static float row_band(const PageBox *b, unsigned r, bool sel)
{
    const float ry = b->rows_y + r * ROW_H, rh = ROW_H - 5;
    graphics_rect(b->x0, ry, b->x1 - b->x0, rh, sel ? (GXColor){30, 60, 120, (unsigned char)(200 * b->a)}
                                                   : (GXColor){0, 0, 0, (unsigned char)(120 * b->a)});
    if (sel) {
        graphics_rect(b->x0, ry, b->x1 - b->x0, 2, (GXColor){120, 180, 255, b->al});
        graphics_rect(b->x0, ry + rh - 2, b->x1 - b->x0, 2, (GXColor){120, 180, 255, b->al});
    }
    return ry;
}
/* A part fitted into a screen box (kept in proportion, centred). */
static void part_fit(unsigned id, float x, float y, float w, float h, unsigned char alpha)
{
    if (id >= PAUSE_PARTS) return;
    const PausePart *pp = &pause_parts[id];
    const float pw = pp->w / PAUSE_ZOOM, ph = pp->h / PAUSE_ZOOM;
    const float k = fminf(w / pw, h / ph);
    const float left = x + (w - pw * k) / 2, top = y + (h - ph * k) / 2;
    part_screen(id, left - pp->ox * k, top - pp->oy * k, k, alpha);
}
/* The row's words: a name, a note on the right, a line under it. */
static void row_words(const PageBox *b, float tx, float ry, const char *name, const char *right, GXColor rc,
                      const char *desc, bool lit)
{
    const GXColor c = lit ? (GXColor){255, 255, 255, b->al} : (GXColor){170, 175, 185, b->al};
    if (name) draw_text(FONT_TALK, name, strlen(name), tx, b->x1 - 70, ry + 1, c, ALIGN_LEFT, ~0u);
    if (right) draw_cstring(FONT_HINT, right, tx, b->x1 - 8, ry + 4, rc, ALIGN_RIGHT);
    if (desc) draw_text(FONT_HINT, desc, strlen(desc), tx, b->x1 - 8, ry + 29, c, ALIGN_LEFT, ~0u);
}
static void page_footer(const PageBox *b, const char *text)
{
    draw_text_glow(FONT_HINT, text, strlen(text), b->x0, b->x1, b->rows_y + PAUSE_LIST_ROWS * ROW_H,
                   (GXColor){255, 255, 255, b->al}, ALIGN_CENTRE, ~0u);
}
/* The controls line of a page: what jump does here, Left / spin goes back. */
static void page_controls(const PageBox *b, const CourseSession *s, const char *what)
{
    const char *choose, *back;
    menu_buttons(s->controller, &choose, &back);
    char line[96];
    if (what) snprintf(line, sizeof line, "Up / Down: choose      %s: %s      %s: back", choose, what, back);
    else snprintf(line, sizeof line, "%s: back", back);
    page_footer(b, line);
}
static PauseContext pause_context(const CourseSession *s)
{
    const Player *pl = &s->game.player;
    return (PauseContext){true, pl->has_fludd, pl->nozzles, s->ach ? ach_popcount(s->ach->p.shines) : 0};
}
/* PAUSED: 794 'default' - the course, its star, and _root.Coins / TotalStars / TotalStarCoins. */
static void draw_paused_page(const CourseSession *s, const PauseView *v, float a, float width)
{
    const PauseMenu *m = &s->pause;
    const PageBox b = page_box(v, PP_TITLE_PAUSED, "Castle Grounds", NULL, a, width);   /* Starnamenum 0 */
    float ry = row_band(&b, 0, false);
    const float icon = ROW_H - 11;
    part_fit(PP_SHINE_OFF, b.x0 + 4, ry + 3, icon, icon, b.al);
    row_words(&b, b.x0 + icon + 12, ry, "No Star Selected", NULL, (GXColor){0}, "You are within the Castle Grounds", true);
    const unsigned shines = s->ach ? ach_popcount(s->ach->p.shines) : 0;
    const unsigned coins = s->ach ? ach_popcount(s->ach->p.star_coins) : 0;
    static const unsigned char coin_order[16] = {PP_COIN_1, PP_COIN_2, PP_COIN_3, PP_COIN_4, PP_COIN_5, PP_COIN_6,
                                                 PP_COIN_7, PP_COIN_8, PP_COIN_9, PP_COIN_10, PP_COIN_11, PP_COIN_12,
                                                 PP_COIN_13, PP_COIN_14, PP_COIN_15, PP_COIN_16};
    const struct { unsigned icon; const char *name; unsigned n; } rows[3] = {
        {PP_SHINE_ON, "Shine Sprites", shines}, {PP_SCOIN_ON, "Star Coins", coins},
        {coin_order[m->clock % 16], "Coins", s->progress.coins}};
    for (unsigned i = 0; i < 3; ++i) {
        ry = row_band(&b, 1 + i, false);
        part_fit(rows[i].icon, b.x0 + 4, ry + 3, icon, icon, b.al);
        char num[16];
        snprintf(num, sizeof num, "%u", rows[i].n);
        row_words(&b, b.x0 + icon + 12, ry, rows[i].name, NULL, (GXColor){0}, NULL, true);
        draw_cstring(FONT_SIGN, num, b.x0, b.x1 - 12, ry + 10, (GXColor){255, 255, 255, b.al}, ALIGN_RIGHT);
    }
    const char *choose, *back;
    menu_buttons(s->controller, &choose, &back);
    char line[96];
    snprintf(line, sizeof line, "Up / Down: page      %s: open      %s: back to the game", choose, back);
    page_footer(&b, line);
}
/* Star Map: 737's rows with their Shine Sprites and Star Coins (and coin records). */
static void draw_starmap_page(const CourseSession *s, const PauseView *v, float a, float width)
{
    const PauseMenu *m = &s->pause;
    const uint64_t got = s->ach ? s->ach->p.shines : 0, coins = s->ach ? s->ach->p.star_coins : 0;
    char left[64], right[64];
    snprintf(left, sizeof left, "Shine Sprites %u / %u", s->ach ? ach_popcount(got) : 0, ACH_SHINES);
    snprintf(right, sizeof right, "Star Coins %u / %u", s->ach ? ach_popcount(coins) : 0, ACH_STAR_COINS);
    const PageBox b = page_box(v, PP_TITLE_STARMAP, left, right, a, width);
    for (unsigned r = 0; r < PAUSE_LIST_ROWS; ++r) {
        const unsigned i = m->list_top + r;
        if (i >= PAUSE_STARMAP_ROWS) break;
        const PauseStarRow *row = &pause_star_rows[i];
        const float ry = row_band(&b, r, m->focus == PI_LIST && i == m->list_sel);
        const bool known = !row->hidden || (got & row->shines);
        char name[96];
        snprintf(name, sizeof name, "%s%s%s", row->label, row->name[0] || row->hidden ? "  " : "", known ? row->name : "???");
        row_words(&b, b.x0 + 10, ry, name, NULL, (GXColor){0}, NULL, true);
        /* Pause New STA: 2 not collected, 3 collected, 4 the row complete; SMSC: 1 / 2 */
        const bool all = row->shines && (got & row->shines) == row->shines;
        float x = b.x0 + 10;
        const float ic = 22, iy = ry + 27;
        for (unsigned n = 1; n <= 64; ++n)
            if (row->shines >> (n - 1) & 1) {
                part_fit(all ? PP_SHINE_ALL : (got >> (n - 1) & 1) ? PP_SHINE_ON : PP_SHINE_OFF, x, iy, ic, ic, b.al);
                x += ic + 3;
            }
        if (row->shines && row->coins) x += 14;
        for (unsigned n = 1; n <= 64; ++n)
            if (row->coins >> (n - 1) & 1) {
                part_fit((coins >> (n - 1) & 1) ? PP_SCOIN_ON : PP_SCOIN_OFF, x, iy, ic, ic, b.al);
                x += ic + 3;
            }
        if (row->record) {   /* the gold coin and RecordCoin<n> (the port keeps no records: 0) */
            part_fit(PP_COIN_1, b.x1 - 70, ry + 12, 26, 26, b.al);
            draw_cstring(FONT_SIGN, "0", b.x0, b.x1 - 12, ry + 10, (GXColor){255, 255, 160, b.al}, ALIGN_RIGHT);
        }
    }
    const char *choose, *back;
    menu_buttons(s->controller, &choose, &back);
    char line[96];
    snprintf(line, sizeof line, "%u of %u      Up / Down: scroll      %s: back", m->list_sel + 1u, PAUSE_STARMAP_ROWS, back);
    page_footer(&b, line);
}
/* Fludd: the nozzles (owned art, or locked), the one worn marked. */
static void draw_fludd_page(const CourseSession *s, const PauseView *v, float a, float width)
{
    const PauseMenu *m = &s->pause;
    const Player *pl = &s->game.player;
    const PauseContext pc = pause_context(s);
    static const char *const nozzle_name[] = {"Hover", "Rocket", "Turbo"};   /* the Nozzle enum's order */
    char left[48];
    snprintf(left, sizeof left, "Worn: %s", pl->has_fludd && pl->nozzle < NOZZLE_COUNT ? nozzle_name[pl->nozzle] : "None");
    const PageBox b = page_box(v, PP_TITLE_FLUDD, left, NULL, a, width);
    const struct { PauseItem item; unsigned owned, locked; int nozzle; const char *name; } noz[4] = {
        {PI_NOZ_NONE, PP_NOZ_NONE, PP_NOZ_NONE, -1, "None"},
        {PI_NOZ_HOVER, PP_NOZ_HOVER, PP_NOZ_HOVER_LOCKED, NOZZLE_HOVER, "Hover"},
        {PI_NOZ_TURBO, PP_NOZ_TURBO, PP_NOZ_TURBO_LOCKED, NOZZLE_TURBO, "Turbo"},
        {PI_NOZ_ROCKET, PP_NOZ_ROCKET, PP_NOZ_ROCKET_LOCKED, NOZZLE_ROCKET, "Rocket"}};
    for (unsigned i = 0; i < 4; ++i) {
        const bool own = pause_item_live(PAUSE_FLUDD, noz[i].item, &pc);
        const bool worn = noz[i].nozzle < 0 ? !pl->has_fludd : pl->has_fludd && (int)pl->nozzle == noz[i].nozzle;
        const float ry = row_band(&b, i, m->focus == (int)noz[i].item);
        const float icon = ROW_H - 11;
        part_fit(own ? noz[i].owned : noz[i].locked, b.x0 + 4, ry + 3, icon, icon, own ? b.al : (unsigned char)(b.al * 0.6f));
        row_words(&b, b.x0 + icon + 12, ry, noz[i].name, worn ? "Worn" : NULL, (GXColor){255, 214, 70, b.al},
                  !own ? "Not found yet" : noz[i].nozzle < 0 ? "Mario without FLUDD" : "Owned", own);
    }
    page_controls(&b, s, "wear");
}
/* Options: Mute Music (773), the Flash settings shown, unavailable. */
static void draw_options_page(const CourseSession *s, const PauseView *v, float a, float width)
{
    const PauseMenu *m = &s->pause;
    const PageBox b = page_box(v, PP_TITLE_OPTIONS, NULL, NULL, a, width);
    const struct { PauseItem item; unsigned part; const char *desc; } opt[3] = {
        {PI_MUTE, PP_OPT_MUTE, NULL},
        {PI_QUALITY, PP_OPT_QUALITY, "A Flash setting: not used on the Wii"},
        {PI_SPEED, PP_OPT_SPEED, "A Flash setting: not used on the Wii"}};
    for (unsigned i = 0; i < 3; ++i) {
        const bool live = opt[i].item == PI_MUTE;
        const float ry = row_band(&b, i, m->focus == (int)opt[i].item);
        const float w = (b.x1 - b.x0) * 0.42f;
        part_fit(opt[i].part, b.x0 + 6, ry + 4, w, ROW_H - 13, live ? b.al : (unsigned char)(b.al * 0.45f));
        const float tx = b.x0 + w + 18;
        if (live)   /* _root.MuteBGMusic */
            row_words(&b, tx, ry, NULL, m->muted ? "Mute: On" : "Mute: Off", (GXColor){255, 214, 70, b.al},
                      m->muted ? "The music is off" : "The music is on", true);
        else
            row_words(&b, tx, ry, NULL, NULL, (GXColor){0}, opt[i].desc, false);
    }
    page_controls(&b, s, "switch");
}
/* Exit: 793 Exit Level (the Castle Grounds: not in the port), 788 Quit Game. */
static void draw_exit_page(const CourseSession *s, const PauseView *v, float a, float width)
{
    const PauseMenu *m = &s->pause;
    const PageBox b = page_box(v, PP_TITLE_EXIT, NULL, NULL, a, width);
    const struct { PauseItem item; unsigned part; const char *desc; } ex[2] = {
        {PI_EXIT_LEVEL, PP_EXIT_EXITLEVEL, "No Castle Grounds in the port yet"},
        {PI_QUIT, PP_EXIT_QUIT, "Back to the title screen"}};
    for (unsigned i = 0; i < 2; ++i) {
        const bool live = ex[i].item == PI_QUIT;
        const float ry = row_band(&b, i, m->focus == (int)ex[i].item);
        const float w = (b.x1 - b.x0) * 0.42f;
        part_fit(ex[i].part, b.x0 + 6, ry + 4, w, ROW_H - 13, live ? b.al : (unsigned char)(b.al * 0.45f));
        row_words(&b, b.x0 + w + 18, ry, NULL, NULL, (GXColor){0}, ex[i].desc, live);
    }
    page_controls(&b, s, "choose");
}
/* The port's Achievements page: the summary, then PAUSE_LIST_ROWS achievements. */
static void draw_achievement_page(const CourseSession *s, const PauseView *v, float a, float width)
{
    const Achievements *ach = s->ach;
    const PauseMenu *m = &s->pause;
    char left[64], right[96];
    const AchSummary sum = ach ? ach_summary(ach) : (AchSummary){0, {0}, 0};
    snprintf(left, sizeof left, "%u / %u unlocked (%u%%)", sum.total, ACH_COUNT, sum.percent);
    snprintf(right, sizeof right, "Bronze %u   Silver %u   Gold %u   Platinum %u", sum.per_tier[ACH_BRONZE],
             sum.per_tier[ACH_SILVER], sum.per_tier[ACH_GOLD], sum.per_tier[ACH_PLATINUM]);
    const PageBox b = page_box(v, PP_TITLE_ACHIEVEMENTS, left, right, a, width);
    char line[96];
    for (unsigned r = 0; r < PAUSE_LIST_ROWS; ++r) {
        const unsigned i = m->list_top + r;
        if (i >= ACH_COUNT) break;
        const AchDef *d = &ach_defs[i];
        const bool lit = ach && ach_unlocked(ach, d->id);
        const float ry = row_band(&b, r, m->focus == PI_LIST && i == m->list_sel);
        const float icon = ROW_H - 11;
        draw_trophy(d->tier, b.x0 + 4, ry + 3, icon, lit, a);
        const unsigned char *c = tier_rgb[d->tier];
        snprintf(line, sizeof line, "%u. %s", d->id, d->name);
        row_words(&b, b.x0 + icon + 12, ry, line, ach_tier_name(d->tier),
                  (GXColor){c[0], c[1], c[2], lit ? b.al : (unsigned char)(b.al * 0.6f)}, d->description, lit);
    }
    const char *choose, *back;
    menu_buttons(s->controller, &choose, &back);
    (void)choose;
    snprintf(line, sizeof line, "%u of %u      Up / Down: scroll      %s: back", m->list_sel + 1u, ACH_COUNT, back);
    page_footer(&b, line);
}
static void draw_pause(const CourseSession *s, float width)
{
    const PauseMenu *m = &s->pause;
    if (!pause_active(m) || !load_parts()) return;
    float dy, a;
    pause_clip_look(m, &dy, &a);
    const unsigned char al = (unsigned char)(255 * a);
    const PauseView v = pause_view(width, dy);
    {   /* 629, the panel's three gradient bands, over the whole screen */
        const PausePart *pp = &pause_parts[PP_PANEL];
        const float aw = (float)parts_layer->width, ah = (float)parts_layer->height;
        graphics_sprite_alpha(&parts_tex, 0, v.dy, width, 480, (pp->x + 0.5f) / aw, (pp->y + 0.5f) / ah,
                              (pp->x + pp->w - 0.5f) / aw, (pp->y + pp->h - 0.5f) / ah, al);
    }
    /* the column: the original buttons, the port's Achievements among them */
    static const unsigned char column[PI_COLUMN] = {PP_BTN_STARMAP, PP_BTN_FLUDD, PP_BTN_OPTIONS, PP_BTN_ACHIEVEMENTS,
                                                    PP_BTN_EXIT};
    for (unsigned i = 0; i < PI_COLUMN; ++i) {
        const float y = 480 * 0.56f + ((float)i - 2) * 66 + v.dy;
        part_screen(column[i], v.col_x, y, HUD_K * COLUMN_K * m->scale[i] / 100, al);
    }
    switch (m->page) {   /* 'refresh' (the page changing) shows no page */
    case PAUSE_DEFAULT: draw_paused_page(s, &v, a, width); break;
    case PAUSE_STARMAP: draw_starmap_page(s, &v, a, width); break;
    case PAUSE_FLUDD: draw_fludd_page(s, &v, a, width); break;
    case PAUSE_OPTIONS: draw_options_page(s, &v, a, width); break;
    case PAUSE_EXIT: draw_exit_page(s, &v, a, width); break;
    case PAUSE_ACHIEVEMENTS: draw_achievement_page(s, &v, a, width); break;
    default: break;
    }
    unsigned char grey;
    const float ta = pause_trans_look(m, &grey);   /* 661 over the panel */
    if (ta > 0)
        graphics_rect(0, v.dy, width, 480, (GXColor){grey, grey, grey, (unsigned char)(255 * ta * a)});
}
/* "Achievement Unlocked!": a dark card with a blue edge sliding in from the
   right edge at the top right (inside the safe area), over everything. */
static void draw_achievement_card(const CourseSession *s, float width)
{
    const float k = ach_notice_slide(&s->notice);
    if (k <= 0 || s->notice.id < 1 || s->notice.id > ACH_COUNT) return;
    const AchDef *d = &ach_defs[s->notice.id - 1];
    const float w = 392, h = 80, margin = 0.05f * width, top = 0.06f * 480;
    const float x = width - margin - w + (1 - k) * (w + margin + 8), y = top;
    const unsigned char a = (unsigned char)(255 * (k < 1 ? k : 1));
    graphics_rect(x, y, w, h, (GXColor){12, 16, 26, (unsigned char)(a * 0.85f)});
    graphics_rect(x, y, w, 2, (GXColor){110, 170, 255, a});                                  /* top highlight */
    graphics_rect(x, y + h - 1, w, 1, (GXColor){50, 80, 140, a});
    graphics_rect(x, y, 3, h, (GXColor){90, 150, 255, a});
    draw_trophy(d->tier, x + 10, y + 8, h - 16, true, a / 255.0f);
    const float tx = x + h + 6, tx1 = x + w - 10;
    draw_cstring(FONT_HINT, "Achievement Unlocked!", tx, tx1, y + 5, (GXColor){160, 200, 255, a}, ALIGN_LEFT);
    draw_text(FONT_TALK, d->name, strlen(d->name), tx, tx1 + 40, y + 24, (GXColor){255, 255, 255, a}, ALIGN_LEFT, ~0u);
    const unsigned char *c = tier_rgb[d->tier];
    draw_cstring(FONT_HINT, ach_tier_name(d->tier), tx, tx1, y + 54, (GXColor){c[0], c[1], c[2], a}, ALIGN_LEFT);
}

void course_render(const CourseSession *s, float t)
{
    const CourseRuntime *c = &s->runtime;
    if (c->area != loaded_area) load_area(c->area);
    const Game *g = &s->game;
    /* The camera works in the original framing (base); the player's zoom and
       Toad's CameraZoom scale the picture about the camera centre. */
    const float width = graphics_width(), base = course_view_zoom(), zoom = base * course_session_zoom(s);
    const float vw = width / zoom, vh = 480.0f / zoom;
    const float centre_x = g->camera.previous_x + (g->camera.x - g->camera.previous_x) * t + width / base / 2;
    const float centre_y = g->camera.previous_y + (g->camera.y - g->camera.previous_y) * t + 480.0f / base / 2;
    /* The view and Mario are placed on whole screen pixels: Mario's art is
       sampled nearest-texel, so a sub-pixel origin changes which texel lands on
       which pixel from frame to frame (shimmering outlines while moving). */
    const float camx = roundf((centre_x - vw / 2) * zoom) / zoom;
    const float camy = roundf((centre_y - vh / 2) * zoom) / zoom;
    const float ccx = course_cx(c, camx + vw / 2), ccy = course_cy(c, camy + vh / 2); /* camera centre */
    graphics_rect(0, 0, width, 480, (GXColor){0, 0, 0, 255});
    /* Backgrounds follow the zoom less than the course, as in the original:
       BGCourse (Background) (coursescale - 100) / 5 + 100, MidBackground / 2,
       BGCloseCourse (CloseBackground) / 1; the far background is not scaled.
       Each is drawn in its own view about the screen centre. */
    const float z = course_session_zoom(s);
    {
        const float zf = base, vwf = width / zf, vhf = 480.0f / zf;
        graphics_set_zoom(zf);
        /* Far background: fixed at the view centre; widened to fill 16:9. */
        draw_layer(ART_LAYER_FAR, vwf / 2, vhf / 2, fmaxf(1, vwf / COURSE_SCALE / 450.0f));
    }
    static const struct { CourseArtKind kind; float follow, parallax; } bgs[] = {
        {ART_LAYER_BG, 0.2f, 0.1f}, {ART_LAYER_MID, 0.5f, 0.2f}, {ART_LAYER_CLOSE, 1.0f, 0.5f}};
    for (unsigned i = 0; i < 3; ++i) {
        const float zl = base * (1 + (z - 1) * bgs[i].follow), sx = width / zl / 2, sy = 240.0f / zl;
        graphics_set_zoom(zl);
        const float ax = sx - ccx * bgs[i].parallax * COURSE_SCALE, ay = sy - ccy * bgs[i].parallax * COURSE_SCALE;
        /* Dynamic Water (802) sits at depth 1 of its background, under the art */
        for (unsigned k = 0; k < s->bg_water_count; ++k) {
            const BgWaterPlace *wp = &bg_waters[s->bg_water_index[k]];
            if (wp->kind != bgs[i].kind) continue;
            const BgWave *w = &s->bg_wave[k];
            const GXColor col = {wp->rgba[0], wp->rgba[1], wp->rgba[2], wp->rgba[3]};
            const float k2 = wp->scale * COURSE_SCALE, bx = ax + wp->x * COURSE_SCALE, by = ay + wp->y * COURSE_SCALE;
            for (unsigned j = 0; j + 1 < w->n; ++j) {
                const float xs[4] = {bx + w->x[j] * k2, bx + w->x[j + 1] * k2, bx + w->x[j + 1] * k2, bx + w->x[j] * k2};
                const float ys[4] = {by + w->y[j] * k2, by + w->y[j + 1] * k2, by + wp->deep * k2, by + wp->deep * k2};
                graphics_quad_color(xs, ys, col);
            }
        }
        draw_layer(bgs[i].kind, ax, ay, 1);
    }
    graphics_set_zoom(zoom);
    const float ox = course_wx(c, 0) - camx, oy = course_wy(c, 0) - camy; /* course origin on screen */
    draw_layer(ART_LAYER_BACK, ox, oy, 1);
    draw_objects(s, ox, oy, false);
    const Player *p = &g->player;
    const float px = roundf((p->previous_x + (p->x - p->previous_x) * t - camx) * zoom) / zoom;
    const float py = roundf((p->previous_y + (p->y - p->previous_y) * t - camy) * zoom) / zoom;
    /* The 0 HP faint: the hurt clips' later frames come from the object packs
       (MARIO_FAINT1 from hurt1 frame 10, MARIO_FAINT3 from hurt3 frame 24);
       during MarioDeathGFX Mario is not drawn (1703 'nes'). */
    const bool faint1 = p->fainting && p->action == ACTION_HURT && p->hurt_kind == 1 && p->hurt_frame >= 9;
    const bool faint3 = p->fainting && p->action == ACTION_HURT && p->hurt_kind == 3 && p->hurt_frame >= 23;
    /* CharCode blink: charClip.frame._alpha 20 when blink is a multiple of 3, else 70. */
    const unsigned char alpha = p->blink > 0 ? (p->blink % 3 == 0 ? 51 : 179) : 255;
    const unsigned v = p->has_fludd ? 1u + (unsigned)p->nozzle : 0u;   /* none, Hover, Rocket, Turbo */
    const unsigned vi = v < 4 ? v : 0;
    /* Mario clips from the object packs (the player atlas is full): the faint,
       Drown, EJump and the idle ones (Idle, Idle2 / Tired, Sleep), per FLUDD state. */
    int clip = -1; unsigned frame = 0;
    if (faint1 || faint3) {
        static const int faint_clips[2][4] = {{OBJ_MARIO_FAINT1_0, OBJ_MARIO_FAINT1_1, OBJ_MARIO_FAINT1_2, OBJ_MARIO_FAINT1_3},
                                              {OBJ_MARIO_FAINT3_0, OBJ_MARIO_FAINT3_1, OBJ_MARIO_FAINT3_2, OBJ_MARIO_FAINT3_3}};
        clip = faint_clips[faint1 ? 0 : 1][vi]; frame = p->hurt_frame - (faint1 ? 9u : 23u);
    } else if (p->drowning) {
        static const int drown[4] = {OBJ_MARIO_DROWN_0, OBJ_MARIO_DROWN_1, OBJ_MARIO_DROWN_2, OBJ_MARIO_DROWN_3};
        clip = drown[vi]; frame = p->drown_frame;
    } else if (p->ejump && p->ejump_frame > 0 && p->action == ACTION_NONE) {
        static const int ejump[4] = {OBJ_MARIO_EJUMP_0, OBJ_MARIO_EJUMP_1, OBJ_MARIO_EJUMP_2, OBJ_MARIO_EJUMP_3};
        clip = ejump[vi]; frame = p->ejump_frame - 1;
    } else if (p->sleeping && p->action == ACTION_NONE) {
        static const int sleep[4] = {OBJ_MARIO_SLEEP_0, OBJ_MARIO_SLEEP_1, OBJ_MARIO_SLEEP_2, OBJ_MARIO_SLEEP_3};
        clip = sleep[vi]; frame = p->sleep_frame;
    } else if (p->idle_kind && p->action == ACTION_NONE) {
        static const int idle[2][4] = {{OBJ_MARIO_IDLE_0, OBJ_MARIO_IDLE_1, OBJ_MARIO_IDLE_2, OBJ_MARIO_IDLE_3},
                                       {OBJ_MARIO_TIRED_0, OBJ_MARIO_TIRED_1, OBJ_MARIO_TIRED_2, OBJ_MARIO_TIRED_3}};
        clip = idle[p->idle_kind == 1 ? 0 : 1][vi]; frame = p->idle_frame;
    }
    if (s->death) {
    } else if (clip >= 0) {
        const ObjectDraw d = {.clip = clip, .frame = frame, .alpha = alpha * 100.0f / 255,
                              .scale = 1, .flip_x = !p->facing_left};   /* the art faces left (_xscale > 0) */
        draw_object_frame(&d, px + PLAYER_WIDTH / 2, py + player_height(p));
    } else {
        user_art_set_alpha(alpha);
        user_art_draw_player(p, px, py);
        user_art_set_alpha(255);
    }
    draw_level_water(s, ox, oy);
    draw_layer(ART_LAYER_FRONT, ox, oy, 1);
    draw_objects(s, ox, oy, true);
    for (unsigned i = 0; i < s->fx_count; ++i) {   /* FrontGFX effects: WaterBubble, Med-Splash */
        const ObjectDraw d = {.clip = s->fx[i].kind == COURSE_FX_BUBBLE ? OBJ_WATER_BUBBLE : OBJ_MED_SPLASH,
                              .frame = s->fx[i].frame, .alpha = 100, .scale = 1};
        draw_object_frame(&d, course_wx(c, s->fx[i].x) - camx, course_wy(c, s->fx[i].y) - camy);
    }
    if (s->death) {   /* MarioDeathGFX in FrontGFX, where Mario was */
        const ObjectDraw d = {.clip = OBJ_MARIO_DEATH, .frame = s->death - 1, .alpha = 100, .scale = 1};
        draw_object_frame(&d, s->death_x - camx, s->death_y - camy);
    }
    graphics_set_zoom(1);
    const GXColor white = {255, 255, 255, 255};
    if (s->hint.length) {
        /* Stats '_root.TextHint' (621 at (6, 40.5), field from -2 to 363.85 with 2 px
           gutters): Arial 11 (font 187), white, left aligned, no box; a black Glow
           filter (blur 2, strength 10) outlines it. Left-anchored like the HUD. */
        const float x0 = hud_x(6, width), x1 = x0 + (363.85f - 2) * HUD_K, y = 40.5f * HUD_K;
        draw_text_glow(FONT_HINT, s->hint.text, s->hint.length, x0, x1, y, white, ALIGN_LEFT, ~0u);
    }
    draw_hud(s, width);
    if (s->tm_alpha > 2 && s->toad_text.length) {
        /* Stats.tm (186 at (246, 259)): the cloud at its _alpha; the text (field
           185: font 155 size 15, light yellow, left aligned, at (-143.45, -28.9),
           a blank first line when the page is short) typed two characters a frame;
           bz, the "Z" key wobbling between two poses, is the controller's button
           at bz._alpha inside the cloud's. */
        const float ca = s->tm_alpha / 100;
        const ObjectDraw cloud = {.clip = OBJ_HUD_TALK_BOX, .alpha = s->tm_alpha, .scale = HUD_K / COURSE_SCALE};
        draw_object_frame(&cloud, hud_x(246, width), 259 * HUD_K);
        /* field 185 at (-143.45, -28.9), bounds x -2..288.9, y -2..59.85, 2 px gutters */
        const float x0 = hud_x(246, width) + (-143.45f - 2 + 2) * HUD_K, x1 = hud_x(246, width) + (-143.45f + 288.9f - 2) * HUD_K;
        const float lh = text_line_height(&font_info[FONT_TALK], 2 * HUD_K);
        const float ty = (259 - 28.9f - 2 + 2) * HUD_K;
        /* txt2: a page under 40 characters starts with " \r " (a blank line, then
           the text after a space) */
        static char page[1024];
        const bool shortp = s->toad_text.length < 40;
        size_t n = 0;
        if (shortp) { page[n++] = ' '; page[n++] = '\n'; page[n++] = ' '; }
        for (size_t i = 0; i < s->toad_text.length && n < sizeof page; ++i) page[n++] = s->toad_text.text[i];
        /* field 185's filters (a Bevel: white / black, blur 5, distance 5, inner;
           a black Glow: blur 2, strength 10) on its light yellow text, baked per
           glyph from FFDec's render (export_fonts.py): the whole look first, then
           the bevelled fill alone so no outline covers a neighbour's letter. */
        const GXColor tint = {255, 255, 255, (unsigned char)(255 * ca)};
        const unsigned shown = s->toad_chars + (shortp ? 3u : 0u);
        draw_text(FONT_TALK_FX, page, n, x0, x1, ty, tint, ALIGN_LEFT, shown);
        draw_text(FONT_TALK_FILL, page, n, x0, x1, ty, tint, ALIGN_LEFT, shown);
        (void)lh;
        const unsigned char za = (unsigned char)(255 * ca * s->tm_bz / 100);
        char key[12];
        snprintf(key, sizeof key, "TalkZ@%u", (unsigned)s->controller);
        const CourseArtLayer *z = course_art_picture(&art, key, strlen(key));
        if (za && load_prompt_layer(z)) {
            const unsigned pose = (s->hud.ticks / 2) % 50 < 24 ? 0 : 1;   /* 184: 182 on frames 1-24, 183 on 25-50 */
            const float cw = z->width / 2.0f;
            graphics_sprite_alpha(&prompt_tex, hud_x(246, width) + z->ox * HUD_K, (259 + z->oy) * HUD_K, cw,
                                  (float)z->height, pose * 0.5f, 0, pose * 0.5f + 0.5f, 1, za);
        }
    }
    if (s->area_text)   /* AreaTextClip (area_text_art.py): the bar and text as the original draws them */
        draw_fx("AreaText~", area_text_tile[course_area_text_frame(s->area_text) - 1], width);
    if (s->state == SESSION_SIGN && s->sign_picture && s->sign_phase == 1) {
        /* Stats.Picture: the page (one texel per screen pixel) at its stage position. */
        const CourseArtLayer *pic = course_art_controller_picture(&art, s->sign_text, s->controller);
        if (load_picture(pic))
            graphics_sprite(&picture_tex, stage_x(pic->ox, width), pic->oy * HUD_K, (float)pic->width, (float)pic->height,
                            0, 0, 1, 1);
        /* The page's moving parts ("<label>~", tools/sign_anim.py), one frame a source frame. */
        char akey[32];
        const int al = snprintf(akey, sizeof akey, "%.*s~", (int)s->sign_text.length, s->sign_text.text);
        const CourseArtLayer *an = al > 0 && al < (int)sizeof akey ? course_art_picture(&art, akey, (size_t)al) : NULL;
        if (an && an->tile_count) {
            const CourseArtTile *T = &an->tiles[sign_anim_frame(s->sign_frames, an->tile_count)];
            if (T != loaded_anim) {
                if (upload(T, anim_mem, ANIM_BYTES, &anim_tex)) { GX_InvalidateTexAll(); loaded_anim = T; }
            }
            if (T == loaded_anim)
                graphics_sprite(&anim_tex, stage_x(an->ox, width) + T->x, an->oy * HUD_K + T->y, (float)T->width,
                                (float)T->height, 0, 0, 1, 1);
        }
        draw_prompt(s, s->sign_text.text, s->sign_text.length, width, pic);
    }
    const unsigned mf = course_message_frame(s);
    if (mf > 1) {
        /* Stats.Message (253): the box opens (frames 2-6) and closes (7-11) as the
           original morph, scaled into each frame's rectangle; text on frame 6. */
        const CourseArtLayer *box = course_art_picture(&art, "Message", 7);
        const float *r = message_box_rects[mf - 1];
        if (load_picture(box))
            graphics_sprite(&picture_tex, stage_x(r[0], width), r[1] * HUD_K, (r[2] - r[0]) * HUD_K, (r[3] - r[1]) * HUD_K,
                            0, 0, 1, 1);
        if (mf == 6) {
            const CourseString m = course_sign_message(s->sign_text, s->controller);
            /* centred up and down between the field's top and the prompt (docs/sign-design) */
            const float x0 = stage_x(MESSAGE_TEXT_X0 + 2, width), x1 = stage_x(MESSAGE_TEXT_X1 - 2, width);
            const unsigned lines = text_lines(FONT_SIGN, m.text, m.length, x1 - x0);
            char key[12];
            snprintf(key, sizeof key, "Prompt@%u", (unsigned)s->controller);
            const CourseArtLayer *pl = course_art_picture(&art, key, strlen(key));
            float prompt_top = SIGN_STAGE_H;
            for (unsigned i = 0; i < PROMPT_PLACES; ++i)
                if (!strcmp(prompt_places[i].page, "Message")) prompt_top = prompt_places[i].y + (pl ? pl->oy : 0);
            const float lh = text_line_height(&font_info[FONT_SIGN], 2 * HUD_K) / HUD_K;
            const float top = sign_text_top(MESSAGE_TEXT_Y0 + 2, prompt_top, lines, lh);
            draw_text(FONT_SIGN, m.text, m.length, x0, x1, top * HUD_K, white, ALIGN_CENTRE, ~0u);
            draw_prompt(s, "Message", 7, width, NULL);
        }
    }
    {   /* area change wipes: "<dir>transition", then "<dir>transition2" in the new area */
        static const char *in[3] = {"WipeRight~", "WipeLeft~", "WipeDown~"};
        static const char *out[3] = {"WipeRight2~", "WipeLeft2~", "WipeDown2~"};
        const unsigned k = s->wipe < 3 ? s->wipe : 0;
        if (s->wipe_in) draw_fx(in[k], s->wipe_in - 1 < wipe_warp_frame(k) ? s->wipe_in - 1 : wipe_warp_frame(k) - 1, width);
        if (s->wipe_out) draw_fx(out[k], s->wipe_out - 1, width);
    }
    /* The faint's screens, over everything (attached to _root): the Blackout and
       StarIn star wipes, and the LifeLost screen on its flat colour. */
    if (s->lifelost) {
        const unsigned f = s->lifelost - 1 < FAINT_LIFELOST_FRAMES ? s->lifelost - 1 : FAINT_LIFELOST_FRAMES - 1;
        const unsigned char *c = faint_lifelost_bg[f];
        graphics_rect(0, 0, width, 480, (GXColor){c[0], c[1], c[2], 255});
        draw_fx("LifeLost~", f, width);
    }
    if (s->blackout) draw_fx("StarOut~", s->blackout - 1, width);
    if (s->starin) draw_fx("StarIn~", s->starin - 1, width);
    if (s->state == SESSION_COMPLETE) {
        graphics_rect(0, 0, width, 480, (GXColor){0, 0, 0, 230});
        draw_cstring(FONT_SIGN, "Tutorial complete!", 20, width - 20, 190, white, ALIGN_CENTRE);
        draw_cstring(FONT_TALK, "Intro movie 2 and the castle are not in this port yet.", 20, width - 20, 240, white,
                     ALIGN_CENTRE);
    }
    graphics_set_zoom(1);
    draw_pause(s, width);
    draw_achievement_card(s, width);
}

/* ---- for the front end (front_render.c) ---- */
unsigned char *course_render_scratch(size_t *size)
{
    loaded_area = NULL;          /* its textures are overwritten: reload on the next course frame */
    loaded_picture = NULL;
    *size = MEM2_RESERVE;
    return mem2;
}
void course_render_text(unsigned font, const char *text, float x0, float x1, float y, GXColor colour, int align,
                        bool glow)
{
    if (glow) draw_text_glow(font, text, strlen(text), x0, x1, y, colour, align, ~0u);
    else draw_text(font, text, strlen(text), x0, x1, y, colour, align, ~0u);
}
void course_render_trophy(unsigned tier, float x, float y, float size, bool lit, float alpha)
{
    draw_trophy(tier, x, y, size, lit, alpha);
}

