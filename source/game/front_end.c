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

static void menu_enter(FrontEnd *f, unsigned page);
void front_init(FrontEnd *f)
{
    memset(f, 0, sizeof *f);
    f->state = FE_OPENING;
    f->play_slot = -1;
    f->files.copy_from = f->files.copy_to = f->files.erase = -1;
    f->menu.rnd = 0x2545F491u;
    f->menu.credits_max = 0;
}
void front_to_title(FrontEnd *f)
{
    const MenuFiles files = f->files;
    const float credits = f->menu.credits_max;
    front_init(f);
    f->files = files; f->files.changed = false;
    f->menu.credits_max = credits;
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
            menu_enter(f, MP_START);
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
    switch (m->page) {
    case MP_START: return item == MI_STORY || item == MI_OPTIONS || item == MI_EXTRAS;
    case MP_STORY:
        if (is_file(item) || item == MI_TOPBACK) return true;
        if (item == MI_PLAY || item == MI_COPY || item == MI_ERASE || item == MI_FILEBACK)
            return FE_SLIDE_PLAY[0][0] < 1e8f ? true : m->filem > 1;
        return false;
    case MP_EXTRAS: return item == MI_ACHIEVEMENTS || item == MI_CREDITS || item == MI_BACK;
    case MP_OPTIONS: return item == MI_MUTE || item == MI_BACK;
    case MP_CREDITS: return item == MI_BACK;
    case MP_ACHIEVEMENTS: return item == MI_LIST || item == MI_BACK;
    default: return false;
    }
}
/* Can the item be chosen now (the original's frame / mode checks)? */
static bool item_live(const FrontEnd *f, int item)
{
    const MenuState *m = &f->menu;
    if (!front_item_shown(f, (MenuItem)item)) return false;
    if (m->page == MP_STORY) {
        if (m->mode == FM_DELETING || m->red_on) return false;
        if (item == MI_PLAY || item == MI_COPY || item == MI_ERASE) return m->hl >= 0 && m->filem >= 20 && m->mode != FM_COPYING;
        if (item == MI_FILEBACK) return m->hl >= 0 && m->filem >= 22;
        if (item == MI_TOPBACK) return m->hl < 0 && m->filem < 2;   /* bach.StopCoding() while a file is open */
        if (is_file(item)) return m->hl < 0 ? m->filem < 2 : m->mode == FM_COPYING;
    }
    return true;
}
static void slide_pos(const float (*t)[2], unsigned frame, float *x, float *y)
{
    unsigned i = frame ? frame - 1 : 0;
    if (i >= FE_FILEM_FRAMES) i = FE_FILEM_FRAMES - 1;
    *x = t[i][0]; *y = t[i][1];
}
bool front_item_pos(const FrontEnd *f, MenuItem item, float *x, float *y)
{
    const MenuState *m = &f->menu;
    const float fx = FE_AT_FILEM[4], fy = FE_AT_FILEM[5];
    *x = *y = 0;
    switch (item) {
    case MI_STORY: *x = FE_AT_STORY[4]; *y = FE_AT_STORY[5]; break;
    case MI_OPTIONS: *x = FE_AT_OPTIONS[4]; *y = FE_AT_OPTIONS[5]; break;
    case MI_EXTRAS: *x = FE_AT_EXTRAS[4]; *y = FE_AT_EXTRAS[5]; break;
    case MI_FILE_A: *x = fx + FE_AT_FILE_A[4]; *y = fy + FE_AT_FILE_A[5]; break;
    case MI_FILE_B: *x = fx + FE_AT_FILE_B[4]; *y = fy + FE_AT_FILE_B[5]; break;
    case MI_FILE_C: *x = fx + FE_AT_FILE_C[4]; *y = fy + FE_AT_FILE_C[5]; break;
    case MI_FILE_D: *x = fx + FE_AT_FILE_D[4]; *y = fy + FE_AT_FILE_D[5]; break;
    case MI_TOPBACK: *x = fx + FE_AT_FILEBACK_TOP[4]; *y = fy + FE_AT_FILEBACK_TOP[5]; break;
    case MI_PLAY: slide_pos(FE_SLIDE_PLAY, m->filem, x, y); *x += fx; *y += fy; break;
    case MI_COPY: slide_pos(FE_SLIDE_COPY, m->filem, x, y); *x += fx; *y += fy; break;
    case MI_ERASE: slide_pos(FE_SLIDE_ERASE, m->filem, x, y); *x += fx; *y += fy; break;
    case MI_FILEBACK: slide_pos(FE_SLIDE_FILEBACK, m->filem, x, y); *x += fx; *y += fy; break;
    case MI_ACHIEVEMENTS: *x = FE_AT_LEVELDESIGNER[4]; *y = FE_AT_LEVELDESIGNER[5]; break;
    case MI_CREDITS: *x = FE_AT_CREDITS[4]; *y = FE_AT_CREDITS[5]; break;
    case MI_BACK: *x = FE_AT_BACK[4]; *y = FE_AT_BACK[5]; break;
    case MI_MUTE: *x = FE_AT_OPT_MUTE[4]; *y = FE_AT_OPT_MUTE[5]; break;
    case MI_LIST: *x = 0; *y = 0; break;
    default: return false;
    }
    return *x < 1e8f;
}
static int first_item(const FrontEnd *f)
{
    static const MenuItem order[] = {MI_STORY, MI_FILE_A, MI_ACHIEVEMENTS, MI_MUTE, MI_LIST, MI_BACK};
    for (unsigned i = 0; i < sizeof order / sizeof order[0]; ++i)
        if (item_live(f, order[i])) return order[i];
    return -1;
}
static void menu_enter(FrontEnd *f, unsigned page)
{
    MenuState *m = &f->menu;
    m->page = (unsigned char)page;
    for (unsigned i = 0; i < MI_COUNT; ++i) float_init(m, &m->item[i]);
    m->hl = -1; m->filem = 1; m->mode = FM_NONE;
    m->credits_scroll = 0;
    m->focus = (signed char)first_item(f);
    if (page == MP_START && m->tree_frame[0] == 0)   /* the backdrop's first placement */
        for (unsigned k = 0; k < 3; ++k) { m->tree_frame[k] = (unsigned char)(rnd(m, 20) + 1); m->tree_x[k] = 0; }
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
static void go_page(FrontEnd *f, unsigned page)   /* TransitionF(page): MenuTransition */
{
    MenuState *m = &f->menu;
    if (m->trans_on) return;
    m->trans_on = true; m->trans_swapped = false; m->trans_y = 200; m->trans_to = (unsigned char)page;
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
        const bool hl = i == m->focus && !m->trans_on;
        if (is_file(i)) {
            float n, l; file_scales(m, i, &n, &l);
            float_frame(&m->item[i], hl && item_live(f, i), n, l, 15);
        } else {
            float_frame(&m->item[i], hl, 100, 125, 10);
        }
    }
    if (m->page == MP_STORY) {   /* the file menu slides its buttons in while a file is open */
        if (m->hl >= 0 && m->filem < FE_FILEM_FRAMES) ++m->filem;
        else if (m->hl < 0 && m->filem > 1) --m->filem;
    }
    if (m->trans_on) {   /* MenuTransition: up 70 px a frame; the page changes past -150 */
        m->trans_y -= 70;
        if (m->trans_y <= -150 && !m->trans_swapped) { m->trans_swapped = true; menu_enter(f, m->trans_to); }
        if (m->trans_y < -500) m->trans_on = false;
    }
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
            m->mode = FM_NONE; m->hl = -1;
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
/* The nearest live item in direction (dx, dy) (along the way more than across it). */
static int neighbour(const FrontEnd *f, int dx, int dy)
{
    const MenuState *m = &f->menu;
    float fx, fy;
    if (m->focus < 0 || !front_item_pos(f, (MenuItem)m->focus, &fx, &fy)) return -1;
    int best = -1; float best_d = 1e9f;
    for (int i = 0; i < MI_COUNT; ++i) {
        float x, y;
        if (i == m->focus || i == MI_LIST || !item_live(f, i) || !front_item_pos(f, (MenuItem)i, &x, &y)) continue;
        const float along = (x - fx) * dx + (y - fy) * dy, across = fabsf((x - fx) * dy) + fabsf((y - fy) * dx);
        if (along <= 4) continue;
        const float d = along + 2 * across;
        if (d < best_d) { best_d = d; best = i; }
    }
    return best;
}
static void click(FrontEnd *f, int item)
{
    MenuState *m = &f->menu;
    FloatItem *it = &m->item[item];
    switch (item) {
    case MI_STORY: case MI_OPTIONS: case MI_EXTRAS: case MI_CREDITS: case MI_ACHIEVEMENTS: {
        it->cl = true; it->rspeed = 0; it->scalespeed = 8;
        static const unsigned char to[MI_COUNT] = {[MI_STORY] = MP_STORY, [MI_OPTIONS] = MP_OPTIONS,
            [MI_EXTRAS] = MP_EXTRAS, [MI_CREDITS] = MP_CREDITS, [MI_ACHIEVEMENTS] = MP_ACHIEVEMENTS};
        go_page(f, to[item]);
        break;
    }
    case MI_BACK: case MI_TOPBACK:
        it->cl = true; it->rspeed = 0; it->scalespeed = 8;
        go_page(f, m->page == MP_CREDITS || m->page == MP_ACHIEVEMENTS ? MP_EXTRAS : MP_START);
        break;
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
    case MI_MUTE:   /* 773: Mute Music on <-> off (_root.MuteBGMusic) */
        f->files.muted = !f->files.muted; f->files.changed = true;
        sfx_set_music_muted(f->files.muted);
        break;
    default:
        break;
    }
}
static void menu_input(FrontEnd *f, const GameInput *in)
{
    MenuState *m = &f->menu;
    if (m->trans_on || m->black_on || m->green_on) return;
    const int dx = in->horizontal > 0.5f ? 1 : in->horizontal < -0.5f ? -1 : 0;
    const int dy = in->vertical > 0.5f ? 1 : in->vertical < -0.5f ? -1 : 0;
    bool fire = false;
    if (dx != m->held_dx || dy != m->held_dy) { m->held_dx = (signed char)dx; m->held_dy = (signed char)dy; m->repeat = 24; fire = dx || dy; }
    else if ((dx || dy) && --m->repeat == 0) { m->repeat = 6; fire = true; }
    if (m->red_on) {   /* Delete File? Yes / No */
        if (m->red_frame != 10) return;
        if (fire && dx) m->red_choice = dx < 0 ? 0 : 1;
        if (in->confirm_pressed || in->cancel_pressed) {
            const bool yes = in->confirm_pressed && m->red_choice == 0;
            m->red_yes = yes;
            m->red_frame = yes ? 11 : 40;
            if (yes) m->erase[m->red_file] = 1;   /* SpecialDeleteFile: the box's erase plays */
            else { m->mode = FM_NONE; }
        }
        return;
    }
    if (m->page == MP_CREDITS && dy && in->vertical) {   /* the arrows: scroll */
        m->credits_scroll = fmaxf(0, fminf(m->credits_max, m->credits_scroll + dy * 2.5f));
    }
    if (m->page == MP_ACHIEVEMENTS) {
        if (fire && dx) m->ach_file = (unsigned char)((m->ach_file + FE_FILES + dx) % FE_FILES);
        if (fire && dy && m->focus == MI_LIST) {
            int sel = m->ach_sel + dy;
            if (sel < 0) sel = 0;
            if (sel > 63) { m->focus = MI_BACK; sel = 63; }
            m->ach_sel = (unsigned char)sel;
            if (m->ach_sel < m->ach_top) m->ach_top = m->ach_sel;
            if (m->ach_sel >= m->ach_top + 5) m->ach_top = (unsigned char)(m->ach_sel - 4);
            fire = false;
        } else if (fire && dy < 0 && m->focus == MI_BACK) { m->focus = MI_LIST; fire = false; }
    }
    if (fire && m->page != MP_CREDITS) {
        const int n = neighbour(f, dx, dy);
        if (n >= 0) m->focus = (signed char)n;
    }
    /* the focus may wait on an item still sliding in; choosing needs it ready */
    if (m->focus < 0 || !front_item_shown(f, (MenuItem)m->focus)) m->focus = (signed char)first_item(f);
    if (in->confirm_pressed && m->focus >= 0 && m->focus != MI_LIST && item_live(f, m->focus)) click(f, m->focus);
    else if (in->cancel_pressed) {   /* B: the page's way back */
        if (m->page == MP_STORY && m->mode == FM_COPYING) m->mode = FM_NONE;
        else if (m->page == MP_STORY && m->hl >= 0) click(f, MI_FILEBACK);
        else if (m->page != MP_START) go_page(f, m->page == MP_CREDITS || m->page == MP_ACHIEVEMENTS ? MP_EXTRAS : MP_START);
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
