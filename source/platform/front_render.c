/* Draws the front end (front_end.c): the replayed timelines of
   front_end.bin (tools/export_front_end.py) placed as the original places
   them, with the pieces its scripts move (START, the menu's buttons, the
   backdrop's layers) moved by the port. Textures live in the course
   renderer's MEM2 area while no course is shown. */
#include "front_render.h"
#include "achievements.h"
#include "course_render.h"
#include "font_metrics.h"
#include "front_end_layout.h"
#include "front_end_bin.h"
#include "graphics.h"
#include "text_layout.h"
#include "timeline.h"
#include "lz4_block.h"
#include "pause_layout.h"
#include <gccore.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

/* Full screen (user request 2026-11-08): the opening, the title and the
   menu's backdrop fill the screen: the 450 x 300 stage as wide as the screen
   (854 px at 16:9: 1.9 screen px a stage px), the rows that are then too many
   cut around a height that keeps the screen's content in view (FOCUS_*,
   stage px). The menu's own items (the files, the buttons, the question)
   need the stage's whole height: they stay at 1.6 x, the stage's height,
   in the middle. */
#define FOCUS_OPENING 150.0f
#define FOCUS_TITLE 144.5f           /* the logo's top (19) to START's foot (270) */
#define MENU_K 1.6f                  /* screen px per stage px for the menu's items: 300 rows in 480 */
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
/* stage px -> screen px: k screen px a stage px, centred across, `focus`
   (stage y) at the screen's middle as far as the stage reaches */
static float cover_k(float width) { return width / 450; }
static Mat stage_at(float width, float k, float focus)
{
    const float h = 300 * k;
    float ty = h <= 480 ? (480 - h) / 2 : 240 - focus * k;
    if (h > 480) ty = fminf(0, fmaxf(480 - h, ty));
    return (Mat){k, 0, 0, k, (width - 450 * k) / 2, ty};
}
static Mat stage(float width, float focus) { return stage_at(width, cover_k(width), focus); }

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
    case MI_TOPBACK: return FE_AT_BACK_BOX;
    case MI_PLAY: return FE_AT_PLAY_BOX; case MI_COPY: return FE_AT_COPY_BOX; case MI_ERASE: return FE_AT_ERASE_BOX;
    case MI_FILEBACK: return FE_AT_FILEBACK_BOX;
    default: return FE_AT_FILEBOX_BOX;
    }
}
static void label_of(MenuItem i, int *clip, const float **m)
{
    *clip = -1; *m = NULL;
    switch (i) {
    case MI_TOPBACK: *clip = FE_CLIP_LBL_BACK; *m = FE_AT_BACK_LBL; break;
    case MI_PLAY: *clip = FE_CLIP_LBL_PLAY; *m = FE_AT_PLAY_LBL; break;
    case MI_COPY: *clip = FE_CLIP_LBL_COPY; *m = FE_AT_COPY_LBL; break;
    case MI_ERASE: *clip = FE_CLIP_LBL_ERASE; *m = FE_AT_ERASE_LBL; break;
    case MI_FILEBACK: *clip = FE_CLIP_LBL_FILEBACK; *m = FE_AT_FILEBACK_LBL; break;
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
/* 9535 px -> screen px through the menu's matrix (no rotation) */
static float sx_of(Mat m, float x) { return m.a * x + m.tx; }
static float sy_of(Mat m, float y) { return m.d * y + m.ty; }
static const char *choose_name(unsigned controller)
{
    return controller == CONTROLLER_CLASSIC ? "b" : controller == CONTROLLER_WIIMOTE ? "2" : "A";
}
static const char *back_name(unsigned controller)
{
    return controller == CONTROLLER_CLASSIC ? "y" : controller == CONTROLLER_WIIMOTE ? "1" : "B";
}
/* The detail panel (the port's): the file's original label, then rows in the
   pause menu's style - its Shine Sprites, Star Coins, achievements and
   trophies - and under them the file's buttons (or how to choose it). */
static void draw_panel(const FrontEnd *f, Mat menu)
{
    const MenuState *m = &f->menu;
    const MenuFiles *fl = &f->files;
    const unsigned k = m->panel_file < FE_FILES ? m->panel_file : 0;
    const float x0 = sx_of(menu, FE_PANEL_X0), x1 = sx_of(menu, FE_PANEL_X1);
    const GXColor white = {255, 255, 255, 255}, grey = {200, 205, 215, 255}, gold = {255, 214, 70, 255};
    /* the file's own label (9414's 'File A'...), larger */
    const Mat lm = mul(menu, mul(translate((FE_PANEL_X0 + FE_PANEL_X1) / 2, FE_PANEL_TITLE_Y), rotate_scale(0, 1.35f)));
    draw_clip(FE_CLIP_FILE_A + k, 0, mul(lm, mat(FE_AT_FILEBOX_LBL)), 1, NULL, 1);
    const float rh = FE_PANEL_ROW_H * menu.d, band = rh - 5;
    const float icon = band - 10;
    char line[64];
    float ry = sy_of(menu, FE_PANEL_ROWS_Y);
    if (m->mode == FM_COPYING) {
        course_render_row(x0, ry, x1, band, true, 1);
        const int from = m->hl >= 0 ? m->hl : 0;
        if ((int)k != from) snprintf(line, sizeof line, "Copy File %c to File %c", 'A' + from, 'A' + k);
        else snprintf(line, sizeof line, "Copy File %c", 'A' + from);
        course_render_text(FONT_TALK, line, x0 + 12, x1 - 8, ry + 2, white, ALIGN_LEFT, false);
        course_render_text(FONT_HINT, (int)k != from && fl->shines[k] ? "That file is not empty" : "Choose an empty file for it",
                           x0 + 12, x1 - 8, ry + 27, (int)k != from && fl->shines[k] ? (GXColor){255, 120, 120, 255} : grey,
                           ALIGN_LEFT, false);
    } else if (fl->shines[k] == 0) {   /* the original's empty file: TotalStars 0 */
        course_render_row(x0, ry, x1, band, false, 1);
        course_render_pause_part(PP_SHINE_OFF, x0 + 6, ry + 5, icon, icon, 255);
        course_render_text(FONT_TALK, "Empty file", x0 + icon + 16, x1 - 8, ry + 2, white, ALIGN_LEFT, false);
        course_render_text(FONT_HINT, "PLAY starts a new game", x0 + icon + 16, x1 - 8, ry + 27, grey, ALIGN_LEFT, false);
    } else {
        const struct { unsigned part; int tier; const char *name; unsigned n, of; } rows[3] = {
            {PP_SHINE_ON, -1, "Shine Sprites", fl->shines[k], 64},
            {PP_SCOIN_ON, -1, "Star Coins", fl->star_coins[k], 64},
            {0, 0, "Achievements", fl->achievements[k], 64}};
        for (unsigned i = 0; i < 3; ++i, ry += rh) {
            course_render_row(x0, ry, x1, band, false, 1);
            if (rows[i].tier < 0) course_render_pause_part(rows[i].part, x0 + 6, ry + 5, icon, icon, 255);
            else course_render_trophy(ACH_GOLD, x0 + 6, ry + 5, icon, rows[i].n > 0, 1);
            course_render_text(FONT_TALK, rows[i].name, x0 + icon + 16, x1 - 8, ry + 9, white, ALIGN_LEFT, false);
            snprintf(line, sizeof line, "%u / %u", rows[i].n, rows[i].of);
            course_render_text(FONT_TALK, line, x0, x1 - 12, ry + 9, gold, ALIGN_RIGHT, false);
        }
        /* the trophies by tier */
        course_render_row(x0, ry, x1, band, false, 1);
        const float cell = (x1 - x0 - 12) / 4;
        for (unsigned t = 0; t < 4; ++t) {
            const float tx = x0 + 6 + cell * t;
            course_render_trophy(t, tx, ry + 5, icon, fl->trophies[k][t] > 0, 1);
            snprintf(line, sizeof line, "%u", fl->trophies[k][t]);
            course_render_text(FONT_TALK, line, tx + icon + 6, tx + cell, ry + 9, white, ALIGN_LEFT, false);
        }
    }
    /* before a file is chosen: how to choose it, where its buttons come */
    if (m->hl < 0 && m->filem <= 1) {
        const float hy = sy_of(menu, (FE_BUTTON_Y0 + FE_BUTTON_Y1) / 2) - 12;
        if (m->focus == MI_TOPBACK)
            snprintf(line, sizeof line, "%s: back to the title", choose_name(f->controller));
        else
            snprintf(line, sizeof line, "%s: choose File %c      %s: back", choose_name(f->controller), 'A' + k,
                     back_name(f->controller));
        course_render_text(FONT_HINT, line, x0, x1, hy, white, ALIGN_CENTRE, true);
    }
}
static void draw_menu(const FrontEnd *f, float width)
{
    const MenuState *m = &f->menu;
    /* 9535 on the main timeline: its items at MENU_K, its backdrop over the screen */
    const Mat menu = mul(stage_at(width, MENU_K, 150), translate(225.15f, 150));
    const Mat back = mul(stage(width, FOCUS_TITLE), translate(225.15f, 150));
    /* the backdrop (9373) */
    draw_clip(FE_CLIP_MBG_SKY, 0, mul(back, mat(FE_AT_MBG_SKY)), 1, NULL, 1);
    {   /* 9349: _xscale 75, scrolling 0.375 a frame */
        Mat far = mul(back, mat(FE_AT_MBG_FAR));
        far = mul(far, translate(m->far_x / 0.75f, 0));
        draw_clip(FE_CLIP_MBG_FAR, 0, mul(far, rotate_scale(0, 0.75f)), 1, NULL, 1);
    }
    draw_clip(FE_CLIP_MBG_MID, 0, mul(back, mul(translate(m->mid_x, 0), mat(FE_AT_MBG_MID))), 1, NULL, 1);
    const float *trees[3] = {FE_AT_MBG_TREE14, FE_AT_MBG_TREE29, FE_AT_MBG_TREE44};
    for (unsigned k = 0; k < 3; ++k)
        draw_clip(FE_CLIP_MBG_TREE, m->tree_frame[k] ? m->tree_frame[k] - 1u : 0,
                  mul(back, mul(translate(m->tree_x[k], 0), mat(trees[k]))), 1, NULL, 1);
    draw_clip(FE_CLIP_MBG_FRONT, 0, mul(back, mat(FE_AT_MBG_FRONT)), 1, NULL, 1);
    const bool blank = m->red_on && m->red_frame >= 1 && m->red_frame <= 10;   /* 'Refresh' under the question */
    if (!blank) {
        draw_clip(FE_CLIP_MENU_PAGE, FE_MENU_PAGE_STORY, menu, 1, NULL, 1);   /* the Story page */
        draw_panel(f, menu);
        draw_files(f, menu, 1);
        draw_button(f, MI_TOPBACK, menu, m->hl < 0 ? 1 : 0.5f, -1);
        if (m->filem > 1)
            for (int i = MI_PLAY; i <= MI_FILEBACK; ++i) draw_button(f, (MenuItem)i, menu, 1, -1);
    }
    for (unsigned i = 0; i < FE_MESSAGES; ++i)   /* MenuMessage, attached at (0, 200) in 9535 */
        if (m->msg[i].on)
            text_at(menu, 0, m->msg[i].y, m->msg[i].text,
                    m->msg[i].green ? (GXColor){80, 255, 80, 255} : (GXColor){255, 60, 60, 255}, FONT_SIGN, ALIGN_CENTRE);
    if (m->red_on) {   /* MenuRedTrans and its question */
        const float a = m->red_alpha / 100;
        draw_clip(FE_CLIP_RED_TRANS, m->red_frame ? m->red_frame - 1u : 0, back, a, NULL, 1);   /* over the screen */
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
    /* the clips sit at the stage centre */
    const Mat root = mul(stage(width, f->state == FE_OPENING ? FOCUS_OPENING : FOCUS_TITLE), translate(225, 150));
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
