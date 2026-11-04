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
#include "water_tex.h"
#include "font_atlas_bin.h"
#include "text_layout.h"
#include "sign_layout.h"
#include "faint_layout.h"
#include "bg_water_places.h"
#include "ui_layout.h"
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
#define OBJECT_BYTES (13u * 512 * 1024)   /* three packs of up to 2 MiB */
#define PROMPT_BYTES (128u * 1024)
#define ANIM_BYTES (320u * 1024)   /* one frame of an animated picture sign or of the faint's screens */
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
static GXTexObj water_body, water_surface, font_tex, prompt_tex;
static const CourseArtLayer *loaded_prompt;

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
enum { ALIGN_LEFT, ALIGN_CENTRE, ALIGN_RIGHT };
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
    /* POWER meter (114 at (225, 25)) with the CharHP pie (111). */
    hud_clip(OBJ_HUD_PIE, hud_pie_frame(&s->hud), 225, 25 + hud_meter_offset(&s->hud), width, 1);
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
    if ((unsigned char *)SYS_GetArena2Hi() - lo < (ptrdiff_t)(MEM2_RESERVE + PICTURE_BYTES + OBJECT_BYTES + PROMPT_BYTES + ANIM_BYTES))
        return false;
    mem2 = lo; picture_mem = lo + MEM2_RESERVE; prompt_mem = lo + MEM2_RESERVE + PICTURE_BYTES + OBJECT_BYTES;
    anim_mem = prompt_mem + PROMPT_BYTES;
    SYS_SetArena2Lo(lo + MEM2_RESERVE + PICTURE_BYTES + OBJECT_BYTES + PROMPT_BYTES + ANIM_BYTES);
    objects_ready = load_objects(lo + MEM2_RESERVE + PICTURE_BYTES, OBJECT_BYTES);
    /* The original font 155 (tools/export_fonts.py): RGBA8 after a 32-byte header. */
    GX_InitTexObj(&font_tex, (void *)(font_atlas_bin + 32), FONT_ATLAS_W, FONT_ATLAS_H, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP,
                  GX_FALSE);
    GX_InitTexObjLOD(&font_tex, GX_LINEAR, GX_LINEAR, 0, 0, 0, GX_FALSE, GX_FALSE, GX_ANISO_1);
    /* Water textures repeat across a volume (GX_REPEAT); data lives in the DOL. */
    GX_InitTexObj(&water_body, (void *)water_body_tex, WATER_BODY_W, WATER_BODY_H, GX_TF_RGB5A3, GX_REPEAT, GX_REPEAT,
                  GX_FALSE);
    GX_InitTexObjLOD(&water_body, GX_LINEAR, GX_LINEAR, 0, 0, 0, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GX_InitTexObj(&water_surface, (void *)water_surface_tex, WATER_SURFACE_W, WATER_SURFACE_H * WATER_SURFACE_FRAMES,
                  GX_TF_RGB5A3, GX_REPEAT, GX_CLAMP, GX_FALSE);
    GX_InitTexObjLOD(&water_surface, GX_LINEAR, GX_LINEAR, 0, 0, 0, GX_FALSE, GX_FALSE, GX_ANISO_1);
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
    if (s->death) {
    } else if (faint1 || faint3) {
        static const int faint_clips[2][4] = {{OBJ_MARIO_FAINT1_0, OBJ_MARIO_FAINT1_1, OBJ_MARIO_FAINT1_2, OBJ_MARIO_FAINT1_3},
                                              {OBJ_MARIO_FAINT3_0, OBJ_MARIO_FAINT3_1, OBJ_MARIO_FAINT3_2, OBJ_MARIO_FAINT3_3}};
        const unsigned v = p->has_fludd ? 1u + (unsigned)p->nozzle : 0u;   /* none, Hover, Rocket, Turbo */
        const ObjectDraw d = {.clip = faint_clips[faint1 ? 0 : 1][v < 4 ? v : 0],
                              .frame = p->hurt_frame - (faint1 ? 9u : 23u), .alpha = alpha * 100.0f / 255,
                              .scale = 1, .flip_x = !p->facing_left};   /* the art faces left (_xscale > 0) */
        draw_object_frame(&d, px + PLAYER_WIDTH / 2, py + player_height(p));
    } else {
        user_art_set_alpha(alpha);
        user_art_draw_player(p, px, py);
        user_art_set_alpha(255);
    }
    /* Water Code volumes: port-made water (tools/make_water_texture.py), drawn
       over Mario like the original's translucent water: a slowly drifting body
       and an animated wave crest along the surface. */
    const float water_seconds = (float)s->hud.ticks / 60.0f;
    for (unsigned i = 0; i < c->area->water_count; ++i) {
        const CourseWater *w = &c->area->water[i];
        CourseWaterQuad q[2];
        const unsigned n = course_water_quads(ox + w->x * COURSE_SCALE, oy + w->y * COURSE_SCALE, w->w * COURSE_SCALE,
                                              w->h * COURSE_SCALE, COURSE_SCALE, water_seconds, q);
        for (unsigned k = 0; k < n; ++k) {
            const float xs[4] = {q[k].x0, q[k].x1, q[k].x1, q[k].x0}, ys[4] = {q[k].y0, q[k].y0, q[k].y1, q[k].y1};
            graphics_quad_alpha(k + 1 == n ? &water_surface : &water_body, xs, ys, q[k].u0, q[k].v0, q[k].u1, q[k].v1,
                                255);
        }
    }
    draw_layer(ART_LAYER_FRONT, ox, oy, 1);
    draw_objects(s, ox, oy, true);
    if (s->death) {   /* MarioDeathGFX in FrontGFX, where Mario was */
        const ObjectDraw d = {.clip = OBJ_MARIO_DEATH, .frame = s->death - 1, .alpha = 100, .scale = 1};
        draw_object_frame(&d, s->death_x - camx, s->death_y - camy);
    }
    graphics_set_zoom(1);
    const GXColor white = {255, 255, 255, 255};
    if (s->hint.length) {
        graphics_rect(20, 420, width - 40, 40, (GXColor){0, 0, 0, 150});
        draw_text(FONT_TALK, s->hint.text, s->hint.length, 30, width - 30, 424, white, ALIGN_LEFT, ~0u);
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
        const float x0 = hud_x(246, width) + (-143.45f + 2) * HUD_K, x1 = hud_x(246, width) + (-143.45f + 288.9f - 2) * HUD_K;
        const float lh = text_line_height(&font_info[FONT_TALK], 2 * HUD_K);
        const float ty = (259 - 28.9f + 2) * HUD_K + (s->toad_text.length < 40 ? lh : 0);
        draw_text(FONT_TALK, s->toad_text.text, s->toad_text.length, x0, x1, ty,
                  (GXColor){255, 255, 179, (unsigned char)(255 * ca)}, ALIGN_LEFT, s->toad_chars);
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
    if (s->area_title_time > 0)
        draw_text(FONT_SIGN, s->area_title.text, s->area_title.length, 40, width - 40, 50, white, ALIGN_CENTRE, ~0u);
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
}
