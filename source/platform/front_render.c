/* Draws the front end (front_end.c): the replayed timelines of
   front_end.bin (tools/export_front_end.py) placed as the original places
   them, with the pieces its scripts move (START, the menu's buttons, the
   backdrop's layers) moved by the port. Textures live in the course
   renderer's MEM2 area while no course is shown. */
#include "front_render.h"
#include "course_render.h"
#include "font_metrics.h"
#include "front_end_layout.h"
#include "front_end_bin.h"
#include "graphics.h"
#include "text_layout.h"
#include "timeline.h"
#include "lz4_block.h"
#include <gccore.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define K 1.6f                       /* screen px per stage px */
static TimelinePack pack;
static GXTexObj page_tex[TL_MAX_PAGES];
static bool page_ready[TL_MAX_PAGES];
static unsigned char *scratch;
static size_t scratch_size, scratch_used;
static unsigned loaded_set;          /* which set of clips is uploaded (1 + set), 0 none */
static bool ready;
static const SaveData *save;

typedef struct { float a, b, c, d, tx, ty; } Mat;
static Mat mat(const float m[6]) { return (Mat){m[0], m[1], m[2], m[3], m[4], m[5]}; }
static Mat mul(Mat m, Mat n)
{
    return (Mat){m.a * n.a + m.c * n.b, m.b * n.a + m.d * n.b, m.a * n.c + m.c * n.d, m.b * n.c + m.d * n.d,
                 m.a * n.tx + m.c * n.ty + m.tx, m.b * n.tx + m.d * n.ty + m.ty};
}
static Mat translate(float x, float y) { return (Mat){1, 0, 0, 1, x, y}; }
static Mat rotate_scale(float deg, float s)
{
    const float r = deg * 3.14159265f / 180, c = cosf(r) * s, sn = sinf(r) * s;
    return (Mat){c, sn, -sn, c, 0, 0};
}
/* stage px -> screen px: the 450 x 300 stage centred on the screen */
static Mat stage(float width) { return (Mat){K, 0, 0, K, width / 2 - 225 * K, 0}; }

bool front_render_init(const SaveData *s)
{
    save = s;
    if (!timeline_parse(front_end_bin, front_end_bin_size, &pack)) return false;
    ready = true;
    return true;
}
/* Upload the pages of the load groups in `want` (bit per group,
   tools/export_front_end.py) into the scratch area. */
static bool load_set(unsigned id, const uint64_t want)
{
    if (loaded_set == id) return true;
    scratch = course_render_scratch(&scratch_size);
    scratch_used = 0;
    if (!timeline_unpack_draws(&pack, scratch, scratch_size)) return false;
    scratch_used = (pack.draw_bytes + 31) & ~(size_t)31;
    for (unsigned i = 0; i < pack.pages; ++i) {
        page_ready[i] = false;
        const TlPage *pg = &pack.page[i];
        if (!(want >> pg->clip & 1)) continue;
        const size_t bytes = (size_t)pg->w * pg->h * (pg->format ? 2 : 4);
        if (scratch_used + bytes > scratch_size) return false;
        if (lz4_block_decode(pg->data, pg->packed, scratch + scratch_used, bytes) != (long)bytes) return false;
        DCFlushRange(scratch + scratch_used, bytes);
        GX_InitTexObj(&page_tex[i], scratch + scratch_used, pg->w, pg->h, pg->format ? GX_TF_RGB5A3 : GX_TF_RGBA8,
                      GX_CLAMP, GX_CLAMP, GX_FALSE);
        GX_InitTexObjLOD(&page_tex[i], GX_LINEAR, GX_LINEAR, 0, 0, 0, GX_FALSE, GX_FALSE, GX_ANISO_1);
        page_ready[i] = true;
        scratch_used = (scratch_used + bytes + 31) & ~(size_t)31;
    }
    GX_InvalidateTexAll();
    loaded_set = id;
    return true;
}
/* Draw clip `clip`'s frame through `m` (clip px -> screen px) at alpha `a`;
   group 1 draws go through `g1` first (a script-driven child). */
static void draw_clip(unsigned clip, unsigned frame, Mat m, float a, const Mat *g1, float g1_alpha)
{
    unsigned n;
    const uint8_t *at = timeline_frame(&pack, clip, frame, &n);
    for (unsigned i = 0; i < n && at; ++i) {
        TlDraw d;
        at = timeline_next(at, &d);
        const TlBake b = timeline_bake(&pack, d.bake);
        if (b.page >= pack.pages || !page_ready[b.page]) continue;
        Mat dm = mat(d.m);
        float alpha = a * d.weight;
        if (d.group == 1 && g1) { dm = mul(*g1, dm); alpha *= g1_alpha; }
        const Mat t = mul(m, dm);
        const TlPage *pg = &pack.page[b.page];
        const float u0 = (float)b.x / pg->w, v0 = (float)b.y / pg->h, u1 = (float)(b.x + b.w) / pg->w,
                    v1 = (float)(b.y + b.h) / pg->h;
        const float mulc[4] = {d.mul[0], d.mul[1], d.mul[2], fmaxf(0, fminf(1, d.mul[3] * alpha))};
        const float add[3] = {d.add[0], d.add[1], d.add[2]};
        if (mulc[3] <= 0.004f) continue;
        if (d.nverts) {   /* clipped by a mask: the polygon in clip px */
            float xs[16], ys[16], us[16], vs[16];
            const unsigned nv = d.nverts < 16 ? d.nverts : 16;
            for (unsigned k = 0; k < nv; ++k) {
                float x, y, u, v;
                timeline_vertex(&d, k, &x, &y, &u, &v);
                const Mat mm = m;
                xs[k] = mm.a * x + mm.c * y + mm.tx; ys[k] = mm.b * x + mm.d * y + mm.ty;
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
static unsigned clip_frames(unsigned clip) { return clip < pack.clips ? pack.clip[clip].count : 1; }

/* ---- the menu ---- */
static const float *box_of(MenuItem i)
{
    switch (i) {
    case MI_STORY: return FE_AT_STORY_BOX; case MI_OPTIONS: return FE_AT_OPTIONS_BOX;
    case MI_EXTRAS: return FE_AT_EXTRAS_BOX; case MI_BACK: case MI_TOPBACK: return FE_AT_BACK_BOX;
    case MI_PLAY: return FE_AT_PLAY_BOX; case MI_COPY: return FE_AT_COPY_BOX; case MI_ERASE: return FE_AT_ERASE_BOX;
    case MI_FILEBACK: return FE_AT_FILEBACK_BOX; case MI_CREDITS: return FE_AT_CREDITS_BOX;
    case MI_ACHIEVEMENTS: return FE_AT_LEVELDESIGNER_BOX;
    default: return FE_AT_FILEBOX_BOX;
    }
}
static void label_of(MenuItem i, int *clip, const float **m)
{
    *clip = -1; *m = NULL;
    switch (i) {
    case MI_STORY: *clip = FE_CLIP_LBL_STORY; *m = FE_AT_STORY_LBL; break;
    case MI_OPTIONS: *clip = FE_CLIP_LBL_OPTIONS; *m = FE_AT_OPTIONS_LBL; break;
    case MI_EXTRAS: *clip = FE_CLIP_LBL_EXTRAS; *m = FE_AT_EXTRAS_LBL; break;
    case MI_BACK: case MI_TOPBACK: *clip = FE_CLIP_LBL_BACK; *m = FE_AT_BACK_LBL; break;
    case MI_PLAY: *clip = FE_CLIP_LBL_PLAY; *m = FE_AT_PLAY_LBL; break;
    case MI_COPY: *clip = FE_CLIP_LBL_COPY; *m = FE_AT_COPY_LBL; break;
    case MI_ERASE: *clip = FE_CLIP_LBL_ERASE; *m = FE_AT_ERASE_LBL; break;
    case MI_FILEBACK: *clip = FE_CLIP_LBL_FILEBACK; *m = FE_AT_FILEBACK_LBL; break;
    case MI_CREDITS: *clip = FE_CLIP_LBL_CREDITS; *m = FE_AT_CREDITS_LBL; break;
    default: break;
    }
}
/* An item's transform (9535 px): its place, bob, sway and scale (the float script). */
static Mat item_mat(const FrontEnd *f, MenuItem i, float x, float y)
{
    const FloatItem *it = &f->menu.item[i];
    const float yy = y + sinf(it->ywave) * 5.5f;
    const float rot = it->r + sinf(it->rwave) * it->scale / 20;
    return mul(translate(x, yy), rotate_scale(rot, it->scale / 100));
}
static void draw_button(const FrontEnd *f, MenuItem i, Mat menu, float a, int box_clip)
{
    float x, y;
    if (!front_item_pos(f, i, &x, &y)) return;
    const Mat m = mul(menu, item_mat(f, i, x, y));
    const unsigned bc = box_clip >= 0 ? (unsigned)box_clip : FE_CLIP_BTN_BOX;
    draw_clip(bc, f->menu.item[i].box_phase % clip_frames(bc), mul(m, mat(box_of(i))), a, NULL, 1);
    int lc; const float *lm;
    label_of(i, &lc, &lm);
    if (lc >= 0) {
        unsigned lf = 0;
        if (i == MI_PLAY) lf = f->files.shines[f->menu.hl < 0 ? 0 : f->menu.hl] ? 1 : 0;   /* 9420: New / Play */
        if (i == MI_COPY && f->menu.mode == FM_COPYING) lf = clip_frames((unsigned)lc) > 2 ? 2 : 0;
        draw_clip((unsigned)lc, lf, mul(m, mat(lm)), a, NULL, 1);
    }
}
static void text_at(Mat menu, float x, float y, const char *s, GXColor c, unsigned font, int align)
{
    const float sx = menu.a * x + menu.c * y + menu.tx, sy = menu.b * x + menu.d * y + menu.ty;
    const float w = 400;
    const float x0 = align == ALIGN_CENTRE ? sx - w : align == ALIGN_RIGHT ? sx - 2 * w : sx;
    course_render_text(font, s, x0, x0 + 2 * w, sy, c, align, true);
}
static void draw_files(const FrontEnd *f, Mat menu, float a)
{
    const MenuState *m = &f->menu;
    for (int k = 0; k < FE_FILES; ++k) {
        const MenuItem it = (MenuItem)(MI_FILE_A + k);
        float x, y;
        if (!front_item_pos(f, it, &x, &y)) continue;
        const Mat t = mul(menu, item_mat(f, it, x, y));
        draw_clip(FE_CLIP_BTN_BOX, m->item[it].box_phase % clip_frames(FE_CLIP_BTN_BOX), mul(t, mat(FE_AT_FILEBOX_BOX)),
                  a, NULL, 1);
        if (m->mode == FM_COPYING && m->hl == k)   /* 9414 frames 6-9: the copy mark */
            draw_clip(FE_CLIP_FILE_COPY, 0, mul(t, mat(FE_AT_FILEBOX_COPY)), a, NULL, 1);
        else
            draw_clip(FE_CLIP_FILE_A + (unsigned)k, 0, mul(t, mat(FE_AT_FILEBOX_LBL)), a, NULL, 1);
        if (m->erase[k])
            draw_clip(FE_CLIP_ERASE, m->erase[k] - 1, mul(t, mat(FE_AT_FILEBOX_EM)), a, NULL, 1);
    }
}
static void draw_options(const FrontEnd *f, Mat menu, float a)
{
    const bool hov = f->menu.focus == MI_MUTE;
    const unsigned char half = (unsigned char)(255 * a * 0.5f);
    (void)half;
    draw_clip(FE_CLIP_OPT_QUALITY, 0, mul(menu, mul(mat(FE_AT_OPT_QUALITY), rotate_scale(0, 0.75f))), a * 0.5f, NULL, 1);
    draw_clip(FE_CLIP_OPT_SPEED, 0, mul(menu, mul(mat(FE_AT_OPT_SPEED), rotate_scale(0, 0.75f))), a * 0.5f, NULL, 1);
    const Mat mm = mul(menu, mul(mat(FE_AT_OPT_MUTE), rotate_scale(0, hov ? 1.0f : 0.75f)));
    const float ma = hov ? a : a * 0.5f;
    draw_clip(FE_CLIP_OPT_MUTE, 0, mm, ma, NULL, 1);
    const unsigned on = f->files.muted ? 3 : hov ? 2 : 1, off = !f->files.muted ? 3 : hov ? 2 : 1;   /* frames 2 / 3 / 4 */
    draw_clip(FE_CLIP_MUTE_ON, on, mul(mm, mat(FE_AT_MUTE_ON)), ma, NULL, 1);
    draw_clip(FE_CLIP_MUTE_OFF, off, mul(mm, mat(FE_AT_MUTE_OFF)), ma, NULL, 1);
}
static void draw_credits(const FrontEnd *f, Mat menu, float a, float width)
{
    const Mat outer = mul(menu, mat(FE_AT_CREDITS_BOX));
    draw_clip(FE_CLIP_CREDITS_FRAME, 0, outer, a, NULL, 1);   /* 9526's box (9517), then 9525's pieces */
    const Mat box = mul(outer, mat(FE_AT_CREDITS_INNER));
    draw_clip(FE_CLIP_CREDITS_BG, 0, box, a, NULL, 1);
    /* the text, scrolled, inside the mask (9519) */
    const float *mk = FE_AT_CREDITS_MASK;   /* w, 0, 0, h, x, y */
    const float sx = box.a * mk[4] + box.tx, sy = box.d * mk[5] + box.ty;
    graphics_scissor(sx, sy, mk[0] * box.a, mk[3] * box.d);
    draw_clip(FE_CLIP_CREDITS_TXT, 0, mul(box, mul(mat(FE_AT_CREDITS_TXT), translate(0, -f->menu.credits_scroll))), a,
              NULL, 1);
    graphics_scissor_off();
    draw_clip(FE_CLIP_CREDITS_TOP, 0, box, a, NULL, 1);
    draw_clip(FE_CLIP_CREDITS_ARROW, 0, mul(box, mat(FE_AT_CREDITS_ARROW8)), a, NULL, 1);
    draw_clip(FE_CLIP_CREDITS_ARROW, 0, mul(box, mul(mat(FE_AT_CREDITS_ARROW10), rotate_scale(180, 1))), a, NULL, 1);
    (void)width;
}
static void draw_achievements(const FrontEnd *f, float width, float a)
{
    const MenuState *m = &f->menu;
    const unsigned char al = (unsigned char)(255 * a);
    const AchProgress *p = save && save->slot[m->ach_file].used ? &save->slot[m->ach_file].progress : NULL;
    unsigned total = 0;
    for (unsigned i = 0; i < ACH_COUNT; ++i) total += p && (p->unlocked >> i & 1);
    char line[96];
    snprintf(line, sizeof line, "Achievements - File %c   %u / %u (%u%%)", 'A' + m->ach_file, total, ACH_COUNT,
             total * 100 / ACH_COUNT);
    const float x0 = width / 2 - 300, x1 = width / 2 + 300;
    course_render_text(FONT_SIGN, line, x0 - 40, x1 - 120, 30, (GXColor){255, 255, 255, al}, ALIGN_LEFT, true);
    course_render_text(FONT_HINT, "Left / Right: file", x0 - 40, x1 - 120, 66, (GXColor){255, 255, 255, al}, ALIGN_LEFT, true);
    for (unsigned r = 0; r < 5; ++r) {
        const unsigned i = m->ach_top + r;
        if (i >= ACH_COUNT) break;
        const AchDef *d = &ach_defs[i];
        const bool lit = p && (p->unlocked >> i & 1), sel = m->focus == MI_LIST && i == m->ach_sel;
        const float y = 120 + r * 58, h = 53;
        graphics_rect(x0, y, x1 - x0, h, sel ? (GXColor){30, 60, 120, (unsigned char)(200 * a)}
                                             : (GXColor){0, 0, 0, (unsigned char)(130 * a)});
        if (sel) graphics_rect(x0, y, x1 - x0, 2, (GXColor){120, 180, 255, al});
        course_render_trophy(d->tier, x0 + 4, y + 3, h - 6, lit, a);
        const GXColor c = lit ? (GXColor){255, 255, 255, al} : (GXColor){170, 175, 185, al};
        snprintf(line, sizeof line, "%u. %s", d->id, d->name);
        course_render_text(FONT_TALK, line, x0 + h + 8, x1 - 70, y + 1, c, ALIGN_LEFT, false);
        course_render_text(FONT_HINT, ach_tier_name(d->tier), x0 + h + 8, x1 - 8, y + 4, c, ALIGN_RIGHT, false);
        course_render_text(FONT_HINT, d->description, x0 + h + 8, x1 - 8, y + 29, c, ALIGN_LEFT, false);
    }
}
static void draw_menu(const FrontEnd *f, float width)
{
    const MenuState *m = &f->menu;
    const Mat menu = mul(stage(width), translate(225.15f, 150));   /* 9535 on the main timeline */
    /* the backdrop (9373) */
    draw_clip(FE_CLIP_MBG_SKY, 0, mul(menu, mat(FE_AT_MBG_SKY)), 1, NULL, 1);
    {   /* 9349: _xscale 75, scrolling 0.375 a frame */
        Mat far = mul(menu, mat(FE_AT_MBG_FAR));
        far = mul(far, translate(m->far_x / 0.75f, 0));
        draw_clip(FE_CLIP_MBG_FAR, 0, mul(far, rotate_scale(0, 0.75f)), 1, NULL, 1);
    }
    draw_clip(FE_CLIP_MBG_MID, 0, mul(menu, mul(translate(m->mid_x, 0), mat(FE_AT_MBG_MID))), 1, NULL, 1);
    const float *trees[3] = {FE_AT_MBG_TREE14, FE_AT_MBG_TREE29, FE_AT_MBG_TREE44};
    for (unsigned k = 0; k < 3; ++k)
        draw_clip(FE_CLIP_MBG_TREE, m->tree_frame[k] ? m->tree_frame[k] - 1u : 0,
                  mul(menu, mul(translate(m->tree_x[k], 0), mat(trees[k]))), 1, NULL, 1);
    draw_clip(FE_CLIP_MBG_FRONT, 0, mul(menu, mat(FE_AT_MBG_FRONT)), 1, NULL, 1);
    const bool blank = m->red_on && m->red_frame >= 1 && m->red_frame <= 10;   /* 'Refresh' under the question */
    if (!blank) {
        if (m->page <= MP_CREDITS) draw_clip(FE_CLIP_MENU_PAGE, m->page, menu, 1, NULL, 1);
        switch (m->page) {
        case MP_START:
            draw_button(f, MI_STORY, menu, 1, -1); draw_button(f, MI_OPTIONS, menu, 1, -1);
            draw_button(f, MI_EXTRAS, menu, 1, -1);
            break;
        case MP_STORY:
            draw_files(f, menu, 1);
            draw_button(f, MI_TOPBACK, menu, m->hl < 0 ? 1 : 0.5f, -1);
            if (m->filem > 1)
                for (int i = MI_PLAY; i <= MI_FILEBACK; ++i) draw_button(f, (MenuItem)i, menu, 1, -1);
            break;
        case MP_EXTRAS: {
            /* the port's Achievements in Level Designer's place; Share levels and the Sound Test shown, unavailable */
            draw_button(f, MI_ACHIEVEMENTS, menu, 1, FE_CLIP_BTN_BOX2);
            float x, y;
            front_item_pos(f, MI_ACHIEVEMENTS, &x, &y);
            text_at(menu, x, y - 9, "Achievements", (GXColor){255, 255, 255, 255}, FONT_SIGN, ALIGN_CENTRE);
            draw_button(f, MI_CREDITS, menu, 1, FE_CLIP_BTN_BOX2);
            draw_clip(FE_CLIP_BTN_BOX3, 0, mul(menu, mul(mat(FE_AT_SHARE), mat(FE_AT_SHARE_BOX))), 0.4f, NULL, 1);
            draw_clip(FE_CLIP_LBL_SHARE, 0, mul(menu, mul(mat(FE_AT_SHARE), mat(FE_AT_SHARE_LBL))), 0.4f, NULL, 1);
            draw_clip(FE_CLIP_SHARE_ANIM, 0, mul(menu, mat(FE_AT_SHARE_ANIM)), 0.4f, NULL, 1);
            draw_clip(FE_CLIP_SOUND_TEST, 0, mul(menu, mat(FE_AT_SOUND_TEST)), 0.4f, NULL, 1);
            draw_button(f, MI_BACK, menu, 1, -1);
            break;
        }
        case MP_OPTIONS: draw_options(f, menu, 1); draw_button(f, MI_BACK, menu, 1, -1); break;
        case MP_CREDITS: draw_credits(f, menu, 1, width); draw_button(f, MI_BACK, menu, 1, -1); break;
        case MP_ACHIEVEMENTS: draw_achievements(f, width, 1); draw_button(f, MI_BACK, menu, 1, -1); break;
        default: break;
        }
    }
    for (unsigned i = 0; i < FE_MESSAGES; ++i)   /* MenuMessage, attached at (0, 200) in 9535 */
        if (m->msg[i].on)
            text_at(menu, 0, m->msg[i].y, m->msg[i].text,
                    m->msg[i].green ? (GXColor){80, 255, 80, 255} : (GXColor){255, 60, 60, 255}, FONT_SIGN, ALIGN_CENTRE);
    if (m->red_on) {   /* MenuRedTrans and its question */
        const float a = m->red_alpha / 100;
        draw_clip(FE_CLIP_RED_TRANS, m->red_frame ? m->red_frame - 1u : 0, menu, a, NULL, 1);
        if (m->red_frame >= 1 && m->red_frame <= 10) {
            /* txtt in the question's field (2123): "Are you sure you want to Delete File X?" */
            char q[64];
            snprintf(q, sizeof q, "Are you sure\ryou want to\rDelete File %c?", 'A' + m->red_file);
            const float *fr = FE_AT_RED_FIELD;   /* w, 0, 0, h, x, y */
            const float fx0 = menu.a * fr[4] + menu.tx, fy0 = menu.d * fr[5] + menu.ty;
            course_render_text(FONT_SIGN, q, fx0, fx0 + fr[0] * menu.a, fy0 + 10, (GXColor){170, 0, 0, (unsigned char)(255 * a)},
                               ALIGN_CENTRE, true);
            draw_clip(FE_CLIP_RED_YES, 0, mul(menu, mul(mat(FE_AT_RED_YES), rotate_scale(0, m->red_choice == 0 ? 1.25f : 1))),
                      a, NULL, 1);
            draw_clip(FE_CLIP_RED_NO, 0, mul(menu, mul(mat(FE_AT_RED_NO), rotate_scale(0, m->red_choice == 1 ? 1.25f : 1))),
                      a, NULL, 1);
        }
    }
    if (m->trans_on) draw_clip(FE_CLIP_TRANS, 0, mul(menu, translate(0, (float)m->trans_y)), 1, NULL, 1);
    /* the stage only, as the title (the backdrop and its frame end at its edges) */
    const float edge = width / 2 - 225 * K;
    if (edge > 0) { graphics_rect(0, 0, edge, 480, (GXColor){0, 0, 0, 255}); graphics_rect(width - edge, 0, edge, 480, (GXColor){0, 0, 0, 255}); }
    if (m->green_on)
        graphics_rect(0, 0, width, 480, (GXColor){60, 200, 60, (unsigned char)(255 * m->green_alpha / 100)});
    if (m->black_on)
        graphics_rect(0, 0, width, 480, (GXColor){0, 0, 0, (unsigned char)(255 * fminf(1, m->black_alpha / 100))});
}

void front_render(const FrontEnd *f)
{
    const float width = graphics_width();
    graphics_set_zoom(1);
    graphics_rect(0, 0, width, 480, (GXColor){0, 0, 0, 255});
    if (!ready) return;
    const Mat root = mul(stage(width), translate(225, 150));   /* the clips sit at the stage centre */
    switch (f->state) {
    case FE_OPENING:
        if (!load_set(1, 1ull << FE_GROUP_OPENING)) return;
        draw_clip(FE_CLIP_OPENING, f->frame, root, 1, NULL, 1);
        break;
    case FE_TITLE: {
        if (!load_set(2, 1ull << FE_GROUP_TITLE | 1ull << FE_GROUP_TITLE_OUT)) return;
        /* START (group 1) at (0, 100.65), its script's scale and alpha */
        const Mat g1 = mul(translate(0, 100.65f), mul(rotate_scale(0, f->start_scale / 100), translate(0, -100.65f)));
        draw_clip(FE_CLIP_TITLE, front_title_frame(f), root, 1, &g1, f->start_placed ? f->start_alpha / 100 : 0);
        if (f->out_frame) draw_clip(FE_CLIP_TITLE_OUT, f->out_frame - 1, root, 1, NULL, 1);
        break;
    }
    case FE_MENU: case FE_PLAY: {
        if (!load_set(3, 1ull << FE_GROUP_MENU | 1ull << FE_GROUP_TITLE_OUT)) return;
        draw_menu(f, width);
        if (f->out_frame) draw_clip(FE_CLIP_TITLE_OUT, f->out_frame - 1, root, 1, NULL, 1);
        break;
    }
    default:
        break;
    }
}
void front_render_release(void) { loaded_set = 0; }
float front_render_credits_height(void)
{
    /* the credits text's height over the mask's: how far it scrolls */
    if (!ready) return 0;
    unsigned n;
    const uint8_t *at = NULL;
    (void)at; (void)n;
    float top = 1e9f, bottom = -1e9f;
    const TlClip *c = &pack.clip[FE_CLIP_CREDITS_TXT];
    (void)c;
    if (!pack.draws) return 0;
    at = timeline_frame(&pack, FE_CLIP_CREDITS_TXT, 0, &n);
    for (unsigned i = 0; i < n && at; ++i) {
        TlDraw d;
        at = timeline_next(at, &d);
        const TlBake b = timeline_bake(&pack, d.bake);
        const float y0 = d.m[5] + b.oy, y1 = y0 + b.h * b.px;
        if (y0 < top) top = y0;
        if (y1 > bottom) bottom = y1;
    }
    const float h = bottom - top - FE_AT_CREDITS_MASK[3];
    return h > 0 ? h : 0;
}
