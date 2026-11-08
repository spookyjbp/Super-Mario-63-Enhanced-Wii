#include "pause_menu.h"
#include <math.h>
#include <string.h>

/* 795 frames 2..15: 794's alpha multiplier (PlaceObject colour transforms) */
static const float open_alpha[PAUSE_OPEN_FRAMES + 1] = {
    0, 0, 18 / 256.0f, 36 / 256.0f, 55 / 256.0f, 73 / 256.0f, 91 / 256.0f, 110 / 256.0f, 128 / 256.0f,
    146 / 256.0f, 164 / 256.0f, 183 / 256.0f, 201 / 256.0f, 219 / 256.0f, 238 / 256.0f, 1};
/* 661 frames 1..13 as FFDec draws them: grey level and alpha */
static const unsigned char trans_grey[PAUSE_TRANS_FRAMES + 1] = {0, 0, 213, 171, 127, 86, 42, 0, 0, 51, 102, 152, 205, 0};
static const float trans_alpha[PAUSE_TRANS_FRAMES + 1] = {0, 0, .17f, .33f, .5f, .67f, .84f, 1, 1, .8f, .6f, .4f, .2f, 0};

#define S(n) (1ULL << ((n) - 1))
#define RANGE(a, b) ((((b) >= 64 ? ~0ULL : (1ULL << (b)) - 1)) & ~((1ULL << ((a) - 1)) - 1))
/* 737 (Pause New SM): the rows of its Shine Sprite view (STA instances named by
   Star[] number, one row per y) and of its Star Coin view (SMSC instances by
   StarCoin[] number); the courses' SMC1-7 / SMM1-3 clips show "???" until a
   shine of theirs is collected; the coin records are hs1-hs10 = RecordCoin1-7,
   9-11. Labels as the original draws them. */
const PauseStarRow pause_star_rows[PAUSE_STARMAP_ROWS] = {
    {"Course 1:", "Bob-Omb Battle Field", RANGE(1, 5), RANGE(1, 6), 1, true},
    {"Course 2:", "Snowman's Land", RANGE(6, 10), RANGE(7, 12), 2, true},
    {"Course 3:", "Hazy Maze Cave", RANGE(11, 15), RANGE(13, 18), 3, true},
    {"Course 4:", "Boo's Mansion", RANGE(16, 20), RANGE(19, 24), 4, true},
    {"Course 5:", "Lethal Lava Land", RANGE(21, 25), RANGE(25, 30), 5, true},
    {"Course 6:", "Tall Tall Mountain", RANGE(26, 30), RANGE(31, 36), 6, true},
    {"Course 7:", "Rainbow Ride", RANGE(31, 35), RANGE(37, 42), 7, true},
    {"Mini Course 1:", "Shifting Sand Land", RANGE(55, 57), RANGE(43, 45), 9, true},
    {"Mini Course 2:", "Wet-Dry World", RANGE(58, 60), RANGE(46, 48), 10, true},
    {"Mini Course 3:", "Tick Tock Clock", RANGE(61, 63), RANGE(49, 51), 11, true},
    {"Storyline", "", S(36) | S(39) | S(41) | S(50) | S(51), S(52) | S(53) | S(54) | S(56) | S(57), 0, false},
    {"Bowser:", "Red-Coins", S(37) | S(40) | S(42), 0, 0, false},
    {"Castle Secret Stars:", "Group A", RANGE(44, 46), 0, 0, false},
    {"Castle Secret Stars:", "Group B", RANGE(47, 49), 0, 0, false},
    {"Castle Secret Stars:", "Group C", RANGE(52, 54), 0, 0, false},
    {"Castle Secret Stars:", "???", S(38) | S(43) | S(64), 0, 0, false},
    {"Castle Secret Star Courses:", "", 0, S(55) | S(58) | S(59) | S(60) | S(63), 0, false},
    {"Castle Star Coins:", "", 0, S(61) | S(62) | S(64), 0, false},
};

unsigned pause_column_page(PauseItem item)
{
    switch (item) {
    case PI_STARMAP: return PAUSE_STARMAP;
    case PI_FLUDD: return PAUSE_FLUDD;
    case PI_OPTIONS: return PAUSE_OPTIONS;
    case PI_ACHIEVEMENTS: return PAUSE_ACHIEVEMENTS;
    case PI_EXIT: return PAUSE_EXIT;
    default: return PAUSE_DEFAULT;
    }
}
bool pause_page_has(unsigned page, PauseItem item)
{
    switch (item) {
    case PI_STARMAP: case PI_FLUDD: case PI_OPTIONS: case PI_ACHIEVEMENTS: case PI_EXIT: return page != PAUSE_REFRESH;
    case PI_NOZ_NONE: case PI_NOZ_HOVER: case PI_NOZ_TURBO: case PI_NOZ_ROCKET: return page == PAUSE_FLUDD;
    case PI_MUTE: case PI_QUALITY: case PI_SPEED: return page == PAUSE_OPTIONS;
    case PI_EXIT_LEVEL: case PI_QUIT: return page == PAUSE_EXIT;
    case PI_LIST: return page == PAUSE_ACHIEVEMENTS || page == PAUSE_STARMAP;
    default: return false;
    }
}
unsigned pause_page_rows(unsigned page, PauseItem *out)
{
    static const PauseItem fludd[] = {PI_NOZ_NONE, PI_NOZ_HOVER, PI_NOZ_TURBO, PI_NOZ_ROCKET};
    static const PauseItem options[] = {PI_MUTE, PI_QUALITY, PI_SPEED};
    static const PauseItem exits[] = {PI_EXIT_LEVEL, PI_QUIT};
    const PauseItem *r = NULL; unsigned n = 0;
    switch (page) {
    case PAUSE_FLUDD: r = fludd; n = 4; break;
    case PAUSE_OPTIONS: r = options; n = 3; break;
    case PAUSE_EXIT: r = exits; n = 2; break;
    case PAUSE_STARMAP: case PAUSE_ACHIEVEMENTS: { static const PauseItem list[] = {PI_LIST}; r = list; n = 1; break; }
    default: break;
    }
    for (unsigned i = 0; i < n; ++i) out[i] = r[i];
    return n;
}
unsigned pause_list_length(unsigned page)
{
    return page == PAUSE_ACHIEVEMENTS ? ACH_COUNT : page == PAUSE_STARMAP ? PAUSE_STARMAP_ROWS : 0;
}
static bool in_column(int item) { return item >= 0 && item < PI_COLUMN; }
bool pause_item_live(unsigned page, PauseItem item, const PauseContext *c)
{
    if (!pause_page_has(page, item)) return false;
    switch (item) {
    case PI_QUALITY: case PI_SPEED: return false;          /* Flash rendering settings: nothing on the Wii */
    case PI_EXIT_LEVEL: return false;                      /* to the Castle Grounds: not in the port (793 is
                                                              off at TotalStars 0 in the original) */
    case PI_NOZ_HOVER: return c->nozzles >> NOZZLE_HOVER & 1;
    case PI_NOZ_ROCKET: return c->nozzles >> NOZZLE_ROCKET & 1;
    case PI_NOZ_TURBO: return c->nozzles >> NOZZLE_TURBO & 1;
    default: return true;
    }
}
static bool selectable(unsigned page, PauseItem item, const PauseContext *c) { return pause_item_live(page, item, c); }
/* The page's first row that can be chosen, or -1 (PAUSED). */
static int page_first(unsigned page, const PauseContext *c)
{
    PauseItem rows[4];
    const unsigned n = pause_page_rows(page, rows);
    for (unsigned i = 0; i < n; ++i)
        if (selectable(page, rows[i], c)) return rows[i];
    return -1;
}
/* The next row that can be chosen from `item` going `dy` (-1 up, 1 down), or -1. */
static int row_step(unsigned page, int item, int dy, const PauseContext *c)
{
    PauseItem rows[4];
    const int n = (int)pause_page_rows(page, rows);
    int at = -1;
    for (int i = 0; i < n; ++i) if ((int)rows[i] == item) at = i;
    if (at < 0) return -1;
    for (int i = at + dy; i >= 0 && i < n; i += dy)
        if (selectable(page, rows[i], c)) return rows[i];
    return -1;
}
void pause_init(PauseMenu *m, bool muted)
{
    memset(m, 0, sizeof *m);
    m->frame = 1; m->focus = -1; m->muted = muted;
    for (unsigned i = 0; i < PI_COUNT; ++i) { m->scale[i] = 100; m->alpha[i] = 50; }
}
bool pause_active(const PauseMenu *m) { return m->frame > 1; }

static void go(PauseMenu *m, unsigned page, int focus_after)
{
    if (m->trans) return;
    if (page != m->page) { m->list_top = m->list_sel = 0; }   /* a new page's list starts at its top */
    m->next = (unsigned char)page;
    m->next_focus = (signed char)focus_after;
    m->trans = 1;
    if (m->focus >= 0) m->scale[m->focus] = 140;
}
/* Open the column button's page (or step into it when it is already shown). */
static void enter(PauseMenu *m, const PauseContext *c)
{
    const unsigned page = pause_column_page((PauseItem)m->focus);
    const int first = page_first(page, c);
    if (m->page == page) { if (first >= 0) { m->column = m->focus; m->focus = (signed char)first; } return; }
    m->column = m->focus;
    go(m, page, first >= 0 ? first : m->focus);
}
static void choose(PauseMenu *m, const PauseContext *c)
{
    const int f = m->focus;
    if (f < 0) return;
    if (in_column(f)) {
        enter(m, c);
    } else if (f >= PI_NOZ_NONE && f <= PI_NOZ_ROCKET) {
        static const int nozzle_of[] = {-1, NOZZLE_HOVER, NOZZLE_TURBO, NOZZLE_ROCKET};
        m->action = PAUSE_ACT_NOZZLE;
        m->action_arg = (unsigned char)(nozzle_of[f - PI_NOZ_NONE] < 0 ? 0xFF : nozzle_of[f - PI_NOZ_NONE]);
        m->scale[f] = 140;
    } else if (f == PI_MUTE) {
        m->muted = !m->muted;                             /* 773: Mode On <-> Off */
        m->action = PAUSE_ACT_MUTE; m->action_arg = m->muted;
        m->scale[f] = 110;
    } else if (f == PI_QUIT) {
        m->action = PAUSE_ACT_QUIT;                       /* 788: Blackout(), ReturnToTitle() */
        m->scale[f] = 140;
    }
}
static void list_move(PauseMenu *m, int dy)
{
    const int len = (int)pause_list_length(m->page);
    int sel = (int)m->list_sel + dy;
    if (sel < 0) sel = 0;
    if (sel > len - 1) sel = len - 1;
    m->list_sel = (unsigned char)sel;
    if (sel < m->list_top) m->list_top = (unsigned char)sel;
    if (sel >= m->list_top + PAUSE_LIST_ROWS) m->list_top = (unsigned char)(sel - PAUSE_LIST_ROWS + 1);
}
/* one original (32 Hz) frame */
static void source_frame(PauseMenu *m)
{
    if (m->frame > 1) ++m->clock;
    if (m->rewind) {
        if (m->frame > 1) --m->frame;
        if (m->frame == 1) { m->rewind = false; m->trans = 0; m->page = PAUSE_DEFAULT; }   /* frame 1: PauseGame = false */
    } else if (m->frame > 1 && m->frame < PAUSE_OPEN_FRAMES) {
        ++m->frame;
    }
    if (m->trans) {
        if (++m->trans == PAUSE_TRANS_SWAP) { m->page = m->next; m->focus = m->next_focus; }
        if (m->trans >= PAUSE_TRANS_FRAMES) m->trans = 0;      /* 13: clicked = false */
    }
    for (int i = 0; i < PI_COUNT; ++i) {
        /* the column button of the page shown stays lit while its items are used */
        const bool on = !m->trans && (i == m->focus || (in_column(i) && !in_column(m->focus) && i == m->column));
        const float sc = i >= PI_MUTE && i <= PI_SPEED ? (on ? 100 : 75) : (on ? 120 : 100);
        m->scale[i] += (sc - m->scale[i]) / 2;
        m->alpha[i] += ((on ? 100 : 50) - m->alpha[i]) / 2;
    }
}
void pause_step(PauseMenu *m, const GameInput *in, const PauseContext *c)
{
    if (in->pause_pressed) {
        if (m->frame == 1 && !m->rewind) {
            if (c->can_pause) {   /* PauseGame = true; play() */
                m->frame = 2; m->page = PAUSE_DEFAULT; m->trans = 0; m->focus = PI_STARMAP; m->column = PI_STARMAP;
                m->phase = 0; m->held_dx = m->held_dy = 0;
            }
        } else {
            m->rewind = true;   /* the key again: play it backwards (PauseGame stays true till frame 1) */
        }
    }
    m->phase += 32;
    while (m->phase >= 60) { m->phase -= 60; source_frame(m); }
    if (!pause_active(m) || m->rewind || m->trans) return;
    if (m->focus < 0 || !selectable(m->page, (PauseItem)m->focus, c)) m->focus = (signed char)m->column;
    /* directions: a press moves once, holding repeats after 0.4 s every 0.1 s */
    const int dx = in->horizontal > 0.5f ? 1 : in->horizontal < -0.5f ? -1 : 0;
    const int dy = in->vertical > 0.5f ? 1 : in->vertical < -0.5f ? -1 : 0;
    bool fire = false;
    if (dx != m->held_dx || dy != m->held_dy) { m->held_dx = (signed char)dx; m->held_dy = (signed char)dy; m->repeat = 24; fire = dx || dy; }
    else if ((dx || dy) && --m->repeat == 0) { m->repeat = 6; fire = true; }
    if (fire) {
        if (in_column(m->focus)) {
            if (dy) {   /* up / down the column, wrapping */
                m->focus = (signed char)((m->focus + dy + PI_COLUMN) % PI_COLUMN);
                m->column = m->focus;
            } else if (dx > 0) {
                enter(m, c);
            }
        } else if (dx < 0) {
            m->focus = (signed char)m->column;                       /* Left: back to the column */
        } else if (dy && m->focus == PI_LIST) {
            list_move(m, dy);
        } else if (dy) {
            const int n = row_step(m->page, m->focus, dy, c);
            if (n >= 0) m->focus = (signed char)n;
        }
    }
    if (in->confirm_pressed) choose(m, c);
    else if (in->cancel_pressed) {
        if (!in_column(m->focus)) m->focus = (signed char)m->column;     /* back to the column */
        else if (m->page != PAUSE_DEFAULT) go(m, PAUSE_DEFAULT, m->focus);
        else m->rewind = true;
    }
}
PauseAction pause_take_action(PauseMenu *m, unsigned *arg)
{
    const PauseAction a = (PauseAction)m->action;
    if (arg) *arg = m->action_arg;
    m->action = PAUSE_ACT_NONE;
    return a;
}
void pause_clip_look(const PauseMenu *m, float *dy, float *alpha)
{
    const unsigned f = m->frame > PAUSE_OPEN_FRAMES ? PAUSE_OPEN_FRAMES : m->frame;
    *dy = f <= 1 ? 20 : 20.0f * (PAUSE_OPEN_FRAMES - f) / (PAUSE_OPEN_FRAMES - 2);
    *alpha = open_alpha[f];
}
float pause_trans_look(const PauseMenu *m, unsigned char *grey)
{
    const unsigned f = m->trans;
    if (grey) *grey = f <= PAUSE_TRANS_FRAMES ? trans_grey[f] : 0;
    return f <= PAUSE_TRANS_FRAMES ? trans_alpha[f] : 0;
}
