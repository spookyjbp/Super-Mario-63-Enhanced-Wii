/* The front end (front_end.c): the opening's timing, the title's START,
   and the main menu's flows as the original's scripts run them. */
#include "front_end.h"
#include "sfx.h"
#include "front_end_layout.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static FrontEnd f;
static void tick(int n) { GameInput z = {0}; for (int i = 0; i < n; ++i) front_step(&f, &z); }
static void press(int what)
{
    GameInput in = {0};
    if (what == 'A') in.confirm_pressed = in.jump_pressed = true;
    if (what == 'B') in.cancel_pressed = in.spin_pressed = true;
    if (what == 'S') in.pause_pressed = true;
    front_step(&f, &in);
    tick(2);
}
static void settle(void) { for (int i = 0; i < 400 && f.menu.trans_on; ++i) tick(1); tick(10); }
static void choose(MenuItem it) { f.menu.focus = (signed char)it; press('A'); settle(); }
static bool played(SfxId id)
{
    SfxEvent ev[SFX_QUEUE];
    const unsigned n = sfx_drain(ev);
    for (unsigned i = 0; i < n; ++i) if (ev[i].id == id) return true;
    return false;
}

static void test_opening_and_title(void)
{
    front_init(&f);
    assert(f.state == FE_OPENING);
    /* 32 frames a second: frame 64 (index 63) after ~118 ticks, held 65 more frames */
    while (f.frame < FE_OPENING_HOLD_FRAME) tick(1);
    const unsigned t0 = 0; (void)t0;
    int held = 0;
    while (f.frame == FE_OPENING_HOLD_FRAME) { tick(1); ++held; }
    assert(held > 120 && held < 126);   /* 66 original frames */
    /* the sound stream starts on frame 158 */
    SfxEvent ev[SFX_QUEUE]; sfx_drain(ev);
    bool sound = false;
    while (f.state == FE_OPENING) { tick(1); sound = sound || played(SFX_OPENING); }
    assert(sound && f.state == FE_TITLE);
    int intro, repeat;
    assert(sfx_music_take(&intro, &repeat) && intro == MUS_TITLE_INTRO && repeat == MUS_TITLE_LOOP);
    /* START appears on frame 65, grows in, then pulses between 45 and 100 % */
    press('A');
    assert(!f.start_clicked);           /* not there yet: nothing happens */
    while (!f.start_placed) tick(1);
    assert(front_title_frame(&f) >= FE_TITLE_START_FRAME);
    tick(60);
    assert(fabsf(f.start_scale - 100) < 2 && f.start_alpha > 40);
    /* the title loops frames 82-337 with the music */
    f.title_ticks = (unsigned)(10.576 * 60) + 2;
    assert(front_title_frame(&f) <= 82);
    f.title_ticks = (unsigned)(18.0 * 60);
    assert(front_title_frame(&f) > 82 && front_title_frame(&f) < 338);
    /* START: the transition, the menu on its frame 20, the file select song */
    press('S');
    assert(f.start_clicked && f.out_frame);
    while (f.state == FE_TITLE) tick(1);
    assert(f.state == FE_MENU && f.menu.page == MP_START && f.out_frame);
    assert(sfx_music_take(&intro, &repeat) && intro == MUS_FILE_SELECT_INTRO);
    tick(60);
    assert(!f.out_frame);               /* it finished over the menu */
}
static void test_menu_flows(void)
{
    f.files.shines[0] = 12; f.files.shines[1] = 0; f.files.shines[2] = 3; f.files.shines[3] = 0;
    /* Start: STORY spins away while the curtain changes the page */
    assert(f.menu.focus == MI_STORY);
    press('A');
    assert(f.menu.item[MI_STORY].cl && f.menu.trans_on);
    settle();
    assert(f.menu.page == MP_STORY && f.menu.focus == MI_FILE_A && f.menu.hl < 0);
    /* a file: its buttons slide in (playable from the file menu's frame 20) */
    choose(MI_FILE_A);
    assert(f.menu.hl == 0 && f.menu.focus == MI_PLAY);
    press('A');                          /* too early: nothing yet */
    assert(!f.menu.black_on);
    tick(45);
    assert(f.menu.filem == FE_FILEM_FRAMES);
    /* Copy into a file that is not empty: refused with a message */
    choose(MI_COPY);
    assert(f.menu.mode == FM_COPYING);
    choose(MI_FILE_C);
    assert(f.menu.mode == FM_COPYING && f.menu.msg[0].on && strstr(f.menu.msg[0].text, "Empty"));
    /* into an empty one: the green flash, then the copy */
    choose(MI_FILE_B);
    assert(f.menu.green_on);
    tick(60);
    assert(f.files.changed && f.files.copy_from == 0 && f.files.copy_to == 1 && f.files.shines[1] == 12);
    f.files.changed = false;
    /* Erase file C: the question; No keeps it, Yes plays the erase and deletes */
    choose(MI_FILE_C); tick(45);
    choose(MI_ERASE);
    while (f.menu.red_frame != 10) tick(1);
    press('B');                          /* B = No */
    tick(80);
    assert(!f.menu.red_on && f.files.shines[2] == 3 && f.menu.erase[2] == 0);
    tick(45); choose(MI_ERASE);
    while (f.menu.red_frame != 10) tick(1);
    f.menu.red_choice = 0; press('A');   /* Yes, Delete */
    tick(80);
    assert(f.files.changed && f.files.erase == 2 && f.files.shines[2] == 0 && f.menu.hl < 0);
    f.files.changed = false;
    /* an empty file cannot be erased or copied (once the file menu has closed) */
    while (f.menu.filem > 1) tick(1);
    choose(MI_FILE_D); tick(45);
    choose(MI_ERASE);
    bool said = false;
    for (unsigned i = 0; i < FE_MESSAGES; ++i) said = said || (f.menu.msg[i].on && strstr(f.menu.msg[i].text, "Delete"));
    assert(!f.menu.red_on && said);
    /* Back closes the file; B again goes to the Start page */
    press('B'); tick(45);
    assert(f.menu.hl < 0);
    press('B'); settle();
    assert(f.menu.page == MP_START);
    /* Options: Mute Music toggles and saves */
    choose(MI_OPTIONS);
    assert(f.menu.page == MP_OPTIONS && f.menu.focus == MI_MUTE);
    press('A');
    assert(f.files.muted && f.files.changed && sfx_music_muted());
    press('A'); f.files.changed = false;
    press('B'); settle();
    /* Extras: Credits and Achievements, back to Extras */
    choose(MI_EXTRAS);
    assert(f.menu.page == MP_EXTRAS);
    choose(MI_CREDITS);
    assert(f.menu.page == MP_CREDITS);
    press('B'); settle();
    assert(f.menu.page == MP_EXTRAS);
    choose(MI_ACHIEVEMENTS);
    assert(f.menu.page == MP_ACHIEVEMENTS && f.menu.focus == MI_LIST);
    GameInput r = {0}; r.horizontal = 1; front_step(&f, &r); tick(2);
    assert(f.menu.ach_file == 1);
    press('B'); settle(); press('B'); settle();
    /* Play file A: the black fade, then FE_PLAY with the file and the music stopped */
    choose(MI_STORY); choose(MI_FILE_A); tick(45);
    choose(MI_PLAY);
    while (f.state != FE_PLAY) tick(1);
    assert(f.play_slot == 0);
    /* Quit Game later: the title again */
    front_to_title(&f);
    assert(f.state == FE_TITLE && f.files.shines[0] == 12);
}

int main(void)
{
    test_opening_and_title();
    test_menu_flows();
    puts("2 front end (opening and title, main menu flows) test groups passed");
    return 0;
}
