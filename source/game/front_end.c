#include "front_end.h"
#include "front_end_layout.h"
#include "sfx.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

/* 9339 frame 2: frame = (position / duration) x 1106 with the theme 34.612 s long */
#define TITLE_RATE (1106.0 / 34.612)
#define TITLE_INTRO_S 10.576              /* MUS_TITLE_INTRO's length */
#define TITLE_LOOP_S (10.576 - 2.58)      /* MUS_TITLE_LOOP's length */
#define TITLE_LOOP_FRAME 81               /* 'repeat' (frame 82) */
#define FE_TITLE_OUT_TOTAL 40

static void menu_enter(FrontEnd *f);
void front_init(FrontEnd *f)
{
    memset(f, 0, sizeof *f);
    f->state = FE_OPENING;
    f->play_slot = -1;
    f->files.copy_from = f->files.copy_to = f->files.erase = -1;
    f->menu.rnd = 0x2545F491u;
}
void front_to_title(FrontEnd *f)
{
    const MenuFiles files = f->files;
    front_init(f);
    f->files = files; f->files.changed = false;
    f->state = FE_TITLE;                    /* ReturnToTitle(): the title screen again */
    sfx_music(MUS_TITLE_INTRO, MUS_TITLE_LOOP);
}
unsigned front_title_frame(const FrontEnd *f)
{
    const double t = f->title_ticks / 60.0;
    if (t < TITLE_INTRO_S) {
        const unsigned fr = (unsigned)(t * TITLE_RATE);
        return fr < FE_TITLE_FRAMES ? fr : FE_TITLE_FRAMES - 1;
    }
    const double l = fmod(t - TITLE_INTRO_S, TITLE_LOOP_S);
    return TITLE_LOOP_FRAME + (unsigned)(l * TITLE_RATE);
}
/* One original frame of START's onEnterFrame (DefineSprite_9298 frame 1); no
   mouse, so never highlighted. */
static void start_script(FrontEnd *f)
{
    if (f->start_clicked) {
        f->start_scale += 5;
    } else {
        f->start_scale += (100 - f->start_scale) / 5;
        f->start_a2 = fmaxf(f->start_a2 - 3, 0);
    }
    if (f->start_anum > 20) {
        f->start_wave += 0.03f;
        f->start_alpha = fminf(75 + sinf(f->start_wave) * 30 + f->start_a2, 100);
    } else {
        f->start_scale += (100 - f->start_scale) / 5;
        ++f->start_anum;
        f->start_alpha += (100 - f->start_alpha) / 5;
    }
}
static void source_frame(FrontEnd *f)
{
    switch (f->state) {
    case FE_OPENING:
        if (f->frame == FE_OPENING_HOLD_FRAME && f->hold < FE_OPENING_HOLD) { ++f->hold; break; }
        if (++f->frame == FE_OPENING_SOUND_FRAME) sfx_play(SFX_OPENING, 100, 0);
        if (f->frame >= FE_OPENING_FRAMES) {   /* 9280 frame 341: _root.nextFrame() -> Title */
            f->state = FE_TITLE;
            f->title_ticks = 0;
            sfx_music(MUS_TITLE_INTRO, MUS_TITLE_LOOP);
        }
        break;
    case FE_TITLE:
        if (!f->start_placed && front_title_frame(f) >= FE_TITLE_START_FRAME) {
            f->start_placed = true;   /* onClipEvent(load) values */
            f->start_scale = 25; f->start_alpha = 0; f->start_wave = 90; f->start_anum = 1;
        }
        if (f->start_placed) start_script(f);
        if (f->out_frame && ++f->out_frame > FE_TITLE_OUT_FRAMES) {   /* 2137 frame 20: gotoAndStop("File Select") */
            f->state = FE_MENU;
            sfx_music(MUS_FILE_SELECT_INTRO, MUS_FILE_SELECT_REPEAT);
            menu_enter(f);   /* the port: the Story page (the file select) at once */
        }
        break;
    case FE_MENU:
        /* TitleScreenTransition goes on over the menu to its frame 40 (stop()) */
        if (f->out_frame && ++f->out_frame > FE_TITLE_OUT_TOTAL) f->out_frame = 0;
        break;
    default:
        break;
    }
}

/* ---------------- the main menu ---------------- */

static unsigned rnd(MenuState *m, unsigned n)   /* random(n) */
{
    m->rnd ^= m->rnd << 13; m->rnd ^= m->rnd >> 17; m->rnd ^= m->rnd << 5;
    return m->rnd % n;
}
static void float_init(MenuState *m, FloatItem *it)
{
    memset(it, 0, sizeof *it);
    it->scale = 100;
    it->ywave = (float)rnd(m, 90); it->rwave = (float)rnd(m, 90);   /* random(90) radians, as the script */
    it->box_phase = rnd(m, 39);                                    /* 2116: gotoAndPlay(random(39) + 1) */
}
static bool is_file(int item) { return item >= MI_FILE_A && item <= MI_FILE_D; }
static unsigned file_of(int item) { return (unsigned)(item - MI_FILE_A); }

bool front_item_shown(const FrontEnd *f, MenuItem item)
{
    const MenuState *m = &f->menu;
    if (is_file(item) || item == MI_TOPBACK) return true;
    if (item >= MI_PLAY && item <= MI_FILEBACK) return m->filem > 1;
    return false;
}
/* Can the item be chosen now (the original's frame / mode checks)? */
bool front_item_live(const FrontEnd *f, MenuItem item)
{
    const MenuState *m = &f->menu;
    if (!front_item_shown(f, item)) return false;
    if (m->mode == FM_DELETING || m->red_on) return false;
    if (item == MI_PLAY || item == MI_COPY || item == MI_ERASE) return m->hl >= 0 && m->filem >= 20 && m->mode != FM_COPYING;
    if (item == MI_FILEBACK) return m->hl >= 0 && m->filem >= 22 && m->mode != FM_COPYING;
    if (item == MI_TOPBACK) return m->hl < 0 && m->filem < 2;   /* bach.StopCoding() while a file is open */
    if (is_file(item)) return m->hl < 0 ? m->filem < 2 : m->mode == FM_COPYING;
    return false;
}
static bool item_live(const FrontEnd *f, int item) { return item >= 0 && item < MI_COUNT && front_item_live(f, (MenuItem)item); }
/* The open file's buttons slide in from the right one after another (the
   original's file menu slides them in over its frames 1-23). */
static float slide_in(const MenuState *m, unsigned order)
{
    float t = ((float)m->filem - 1 - 2.0f * order) / 16.0f;
    t = t < 0 ? 0 : t > 1 ? 1 : t;
    const float u = 1 - t;
    return u * u * u * 320;   /* px still to go */
}
bool front_item_pos(const FrontEnd *f, MenuItem item, float *x, float *y)
{
    const MenuState *m = &f->menu;
    *x = *y = 0;
    switch (item) {
    case MI_FILE_A: *x = FE_GRID_X0; *y = FE_GRID_Y0; break;
    case MI_FILE_B: *x = FE_GRID_X1; *y = FE_GRID_Y0; break;
    case MI_FILE_C: *x = FE_GRID_X0; *y = FE_GRID_Y1; break;
    case MI_FILE_D: *x = FE_GRID_X1; *y = FE_GRID_Y1; break;
    case MI_TOPBACK: *x = FE_BACK_X; *y = FE_BACK_Y; break;
    case MI_PLAY: *x = FE_BUTTON_X0 + slide_in(m, 0); *y = FE_BUTTON_Y0; break;
    case MI_COPY: *x = FE_BUTTON_X1 + slide_in(m, 1); *y = FE_BUTTON_Y0; break;
    case MI_ERASE: *x = FE_BUTTON_X0 + slide_in(m, 2); *y = FE_BUTTON_Y1; break;
    case MI_FILEBACK: *x = FE_BUTTON_X1 + slide_in(m, 3); *y = FE_BUTTON_Y1; break;
    default: return false;
    }
    return true;
}
/* The D-pad's links (FE_UP, FE_DOWN, FE_LEFT, FE_RIGHT), in the order tried:
   the files' grid (A B / C D) with BACK under it, the open file's buttons
   PLAY COPY / DELETE CANCEL. */
static const signed char links[MI_COUNT][4][3] = {
    [MI_FILE_A] = {{-1, -1, -1}, {MI_FILE_C, -1, -1}, {-1, -1, -1}, {MI_FILE_B, -1, -1}},
    [MI_FILE_B] = {{-1, -1, -1}, {MI_FILE_D, -1, -1}, {MI_FILE_A, -1, -1}, {-1, -1, -1}},
    [MI_FILE_C] = {{MI_FILE_A, -1, -1}, {MI_TOPBACK, -1, -1}, {-1, -1, -1}, {MI_FILE_D, -1, -1}},
    [MI_FILE_D] = {{MI_FILE_B, -1, -1}, {MI_TOPBACK, -1, -1}, {MI_FILE_C, -1, -1}, {-1, -1, -1}},
    [MI_TOPBACK] = {{MI_FILE_C, MI_FILE_D, -1}, {-1, -1, -1}, {-1, -1, -1}, {-1, -1, -1}},
    [MI_PLAY] = {{-1, -1, -1}, {MI_ERASE, -1, -1}, {-1, -1, -1}, {MI_COPY, -1, -1}},
    [MI_COPY] = {{-1, -1, -1}, {MI_FILEBACK, -1, -1}, {MI_PLAY, -1, -1}, {-1, -1, -1}},
    [MI_ERASE] = {{MI_PLAY, -1, -1}, {-1, -1, -1}, {-1, -1, -1}, {MI_FILEBACK, -1, -1}},
    [MI_FILEBACK] = {{MI_COPY, -1, -1}, {-1, -1, -1}, {MI_ERASE, -1, -1}, {-1, -1, -1}},
};
int front_menu_link(const FrontEnd *f, MenuItem item, unsigned dir)
{
    if ((unsigned)item >= MI_COUNT || dir > FE_RIGHT) return -1;
    for (unsigned i = 0; i < 3; ++i) {
        const int to = links[item][dir][i];
        if (to >= 0 && item_live(f, to)) return to;
    }
    return -1;
}
/* Where the focus goes when it has nowhere live to be: the open file's PLAY,
   the file being copied from's neighbour, else the first file. */
static int first_item(const FrontEnd *f)
{
    const MenuState *m = &f->menu;
    if (m->hl >= 0 && m->mode != FM_COPYING) {
        for (int i = MI_PLAY; i <= MI_FILEBACK; ++i) if (item_live(f, i)) return i;
        return MI_PLAY;   /* still sliding in */
    }
    if (m->hl >= 0) return MI_FILE_A + (m->hl == 0 ? 1 : 0);
    return MI_FILE_A;
}
static void menu_enter(FrontEnd *f)
{
    MenuState *m = &f->menu;
    for (unsigned i = 0; i < MI_COUNT; ++i) float_init(m, &m->item[i]);
    m->hl = -1; m->filem = 1; m->mode = FM_NONE;
    m->focus = MI_FILE_A;
    for (unsigned k = 0; k < 3; ++k) { m->tree_frame[k] = (unsigned char)(rnd(m, 20) + 1); m->tree_x[k] = 0; }   /* the backdrop */
}
static void message(FrontEnd *f, const char *text, bool green)
{
    MenuState *m = &f->menu;
    for (unsigned i = 0; i < FE_MESSAGES; ++i)
        if (!m->msg[i].on) {   /* MenuMessage: up fast, slowing; after 60 frames it falls */
            MenuMessage *g = &m->msg[i];
            g->on = true; g->y = 200; g->speed = -(float)(rnd(m, 3) + 18); g->count = 0; g->green = green;
            snprintf(g->text, sizeof g->text, "%s", text);
            return;
        }
}
/* The float script, one frame. */
static void float_frame(FloatItem *it, bool hl, float norm, float large, float rdiv)
{
    if (it->cl) {
        it->rspeed += 1; it->r -= it->rspeed; it->rspeed *= 0.95f;
        it->scale += it->scalespeed; it->scalespeed -= 1;
        if (it->scale < 60) { it->scalespeed += 1; it->scalespeed *= 0.7f; }
    } else if (hl) {
        it->scale += (large - it->scale) / 4; it->r += (large - it->scale) / rdiv; it->ywave -= 0.03f;
    } else {
        it->scale += (norm - it->scale) / 4; it->r += (norm - it->scale) / rdiv;
    }
    it->ywave += 0.03f; it->rwave += 0.025f;
    ++it->box_phase;
}
static void file_scales(const MenuState *m, int item, float *norm, float *large)
{
    const int file = (int)file_of(item);
    if (m->mode == FM_COPYING) {
        if (m->hl == file) { *norm = 50; *large = 75; }
        else if (m->hl < 0) { *norm = 100; *large = 125; }
        else { *norm = 105; *large = 135; }
    } else if (m->hl == file) { *norm = 135; *large = 145; }
    else if (m->hl < 0) { *norm = 100; *large = 125; }
    else { *norm = 75; *large = 90; }
}
static void menu_frame(FrontEnd *f)
{
    MenuState *m = &f->menu;
    /* the backdrop (9349 / 9350 / 9371 scripts) */
    m->far_x -= 0.375f; if (m->far_x <= -225) m->far_x += 225;
    m->mid_x -= 0.75f; if (m->mid_x <= -1000) m->mid_x += 1000;
    for (unsigned k = 0; k < 3; ++k) {
        m->tree_x[k] -= 2;
        const float base = (k == 0 ? FE_AT_MBG_TREE14 : k == 1 ? FE_AT_MBG_TREE29 : FE_AT_MBG_TREE44)[4];
        if (base + m->tree_x[k] <= -345) {
            m->tree_x[k] += 675;
            unsigned n = rnd(m, 20) + 1;   /* RMP(): not one of the last two */
            for (unsigned i = 0; i < 5 && (n == m->tree_last[0] || n == m->tree_last[1]); ++i) n = rnd(m, 20) + 1;
            m->tree_last[1] = m->tree_last[0]; m->tree_last[0] = (unsigned char)n;
            m->tree_frame[k] = (unsigned char)n;
        }
    }
    for (int i = 0; i < MI_COUNT; ++i) {
        if (!front_item_shown(f, (MenuItem)i)) continue;
        const bool hl = i == m->focus;
        if (is_file(i)) {
            float n, l; file_scales(m, i, &n, &l);
            float_frame(&m->item[i], hl && item_live(f, i), n, l, 15);
        } else {
            float_frame(&m->item[i], hl, 100, 125, 10);
        }
    }
    /* the file menu slides its buttons in while a file is open */
    if (m->hl >= 0 && m->filem < FE_FILEM_FRAMES) ++m->filem;
    else if (m->hl < 0 && m->filem > 1) --m->filem;
    for (unsigned i = 0; i < FE_MESSAGES; ++i) {
        MenuMessage *g = &m->msg[i];
        if (!g->on) continue;
        if (++g->count > 60) { g->count = -500; g->speed = 0.3f; }
        else if (g->speed < 0) g->speed *= 0.9f;
        else g->speed *= 1.1f;
        g->y += g->speed;
        if (g->y > 800) g->on = false;
    }
    if (m->red_on) {   /* MenuRedTrans: fades in to the question; Yes plays 11-30, No 40-51, fading out */
        if (m->red_frame == 0) {
            m->red_alpha = fminf(100, m->red_alpha + 5);
            if (m->red_alpha >= 100) m->red_frame = 1;   /* the page goes blank ('Refresh') under it */
        } else if (m->red_frame < 10) {
            ++m->red_frame;
        } else if (m->red_frame > 10) {
            if ((m->red_frame < 30) || (m->red_frame >= 40 && m->red_frame < 51)) ++m->red_frame;
            m->red_alpha -= 5;
            if (m->red_alpha <= 0) { m->red_on = false; m->red_alpha = 0; }
        }
    }
    for (unsigned k = 0; k < FE_FILES; ++k) {   /* 9403: the erase, DeleteFile on its frame 34 */
        if (!m->erase[k]) continue;
        if (++m->erase[k] == 34) {
            f->files.erase = (int)k; f->files.shines[k] = 0; f->files.changed = true;
            f->files.star_coins[k] = f->files.achievements[k] = 0;
            for (unsigned t = 0; t < 4; ++t) f->files.trophies[k][t] = 0;
            m->mode = FM_NONE; m->hl = -1; m->focus = (signed char)(MI_FILE_A + k);
        }
        if (m->erase[k] >= 35) m->erase[k] = 0;
    }
    if (m->green_on) {   /* MenuGreenTrans: +10 alpha, copy at 100, then -10 */
        if (!m->green_back) {
            m->green_alpha += 10;
            if (m->green_alpha >= 100) {
                m->green_back = true;
                f->files.copy_from = m->copy_from; f->files.copy_to = m->copy_to;
                f->files.shines[m->copy_to] = f->files.shines[m->copy_from]; f->files.changed = true;
                f->files.star_coins[m->copy_to] = f->files.star_coins[m->copy_from];
                f->files.achievements[m->copy_to] = f->files.achievements[m->copy_from];
                for (unsigned t = 0; t < 4; ++t) f->files.trophies[m->copy_to][t] = f->files.trophies[m->copy_from][t];
            }
        } else if ((m->green_alpha -= 10) <= 0) { m->green_on = false; m->green_alpha = 0; }
    }
    if (m->black_on) {   /* MenuBlackTrans: +4 alpha; at 100 the game starts */
        m->black_alpha += 4;
        if (m->black_alpha >= 100) {
            m->black_alpha = 100;
            f->state = FE_PLAY;
            sfx_music(-1, -1);   /* StopBGsong() */
        }
    }
}
static void click(FrontEnd *f, int item)
{
    MenuState *m = &f->menu;
    switch (item) {
    case MI_TOPBACK:   /* the port: no Start page, so back to the title */
        front_to_title(f);
        return;
    case MI_FILE_A: case MI_FILE_B: case MI_FILE_C: case MI_FILE_D: {
        const int file = (int)file_of(item);
        if (m->mode == FM_COPYING) {
            if (f->files.shines[file] == 0 && file != m->hl) {   /* into an empty file */
                m->mode = FM_NONE;
                m->green_on = true; m->green_back = false; m->green_alpha = 0;
                m->copy_from = m->hl; m->copy_to = file;
                m->hl = -1; m->focus = (signed char)item;
            } else if (file != m->hl) {
                message(f, "Can Only Copy into an Empty File..", false);
            }
        } else {
            m->hl = file; m->focus = MI_PLAY;
        }
        break;
    }
    case MI_PLAY:   /* _root.File = HL; LoadFile(); TransitionF("Play") */
        f->play_slot = m->hl;
        m->black_on = true; m->black_alpha = 0;
        break;
    case MI_COPY:
        if (f->files.shines[m->hl] > 0) { m->mode = FM_COPYING; m->focus = (signed char)(MI_FILE_A + (m->hl == 0 ? 1 : 0)); }
        else message(f, "Can't Copy an Empty File!", false);
        break;
    case MI_ERASE:
        if (f->files.shines[m->hl] != 0) {
            m->mode = FM_DELETING;
            m->red_on = true; m->red_frame = 0; m->red_alpha = 0; m->red_choice = 1; m->red_file = m->hl;
        } else {
            message(f, "Can't Delete an Empty File", false);
        }
        break;
    case MI_FILEBACK:
        m->focus = (signed char)(MI_FILE_A + m->hl); m->hl = -1;
        break;
    default:
        break;
    }
}
static void menu_input(FrontEnd *f, const GameInput *in)
{
    MenuState *m = &f->menu;
    f->controller = in->controller;
    if (m->black_on || m->green_on) return;
    const int dx = in->horizontal > 0.5f ? 1 : in->horizontal < -0.5f ? -1 : 0;
    const int dy = in->vertical > 0.5f ? 1 : in->vertical < -0.5f ? -1 : 0;
    bool fire = false;   /* a press moves once; held, it repeats after 0.4 s every 0.1 s */
    if (dx != m->held_dx || dy != m->held_dy) { m->held_dx = (signed char)dx; m->held_dy = (signed char)dy; m->repeat = 24; fire = dx || dy; }
    else if ((dx || dy) && --m->repeat == 0) { m->repeat = 6; fire = true; }
    if (m->red_on) {   /* Delete File? Yes / No (Left / Right); spin: No */
        if (m->red_frame != 10) return;
        if (fire && dx) m->red_choice = dx < 0 ? 0 : 1;
        if (in->confirm_pressed || in->cancel_pressed) {
            const bool yes = in->confirm_pressed && m->red_choice == 0;
            m->red_yes = yes;
            m->red_frame = yes ? 11 : 40;
            if (yes) m->erase[m->red_file] = 1;   /* SpecialDeleteFile: the box's erase plays */
            else m->mode = FM_NONE;
        }
        return;
    }
    if (m->mode == FM_DELETING) return;   /* the erase is playing */
    /* the focus stays in the group of buttons in use (it may wait on one still
       sliding in or out): the open file's buttons, else the files and BACK */
    const bool side = m->hl >= 0 && m->mode != FM_COPYING;
    if (m->focus < 0 || m->focus >= MI_COUNT || side != (m->focus >= MI_PLAY) ||
        (m->mode == FM_COPYING && m->focus == MI_TOPBACK))
        m->focus = (signed char)first_item(f);
    if (fire) {   /* one direction at a time: up / down before left / right */
        const unsigned dir = dy < 0 ? FE_UP : dy > 0 ? FE_DOWN : dx < 0 ? FE_LEFT : FE_RIGHT;
        const int n = front_menu_link(f, (MenuItem)m->focus, dir);
        if (n >= 0) m->focus = (signed char)n;
    }
    /* the panel shows the open file, else the file under the cursor (on BACK: the last one) */
    if (m->hl >= 0 && m->mode != FM_COPYING) m->panel_file = (unsigned char)m->hl;
    else if (is_file(m->focus)) m->panel_file = (unsigned char)(m->focus - MI_FILE_A);
    if (in->confirm_pressed && item_live(f, m->focus)) click(f, m->focus);
    else if (in->cancel_pressed) {   /* spin: the way back */
        if (m->mode == FM_COPYING) { m->mode = FM_NONE; m->focus = MI_COPY; }
        else if (m->hl >= 0) click(f, MI_FILEBACK);
        else front_to_title(f);
    }
}

void front_step(FrontEnd *f, const GameInput *in)
{
    if (f->state == FE_TITLE) {
        ++f->title_ticks;
        /* START: the original takes a click on it; the port, jump or START */
        if (f->start_placed && !f->start_clicked && (in->confirm_pressed || in->pause_pressed)) {
            f->start_clicked = true;
            f->out_frame = 1;
        }
    }
    if (f->state == FE_MENU) menu_input(f, in);
    f->phase += 32;
    while (f->phase >= 60) {
        f->phase -= 60;
        source_frame(f);
        if (f->state == FE_MENU) menu_frame(f);
    }
}
