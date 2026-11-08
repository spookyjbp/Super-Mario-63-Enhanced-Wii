#include "graphics.h"
#include "input.h"
#include "game.h"
#include "render.h"
#include "course_render.h"
#include "course_8e1_bin.h"
#include "audio.h"
#include "save_io.h"
#include "front_end.h"
#include "front_render.h"
#include <string.h>
#include "sfx.h"

#include <ogc/lwp_watchdog.h>
#include <stdlib.h>
#include <wiiuse/wpad.h>

static volatile bool exit_requested;
static volatile bool power_requested;

static void on_reset(u32 irq, void *context)
{
    (void)irq;
    (void)context;
    exit_requested = true;
}

static void on_power(void) { power_requested = true; }
static void on_remote_power(s32 channel) { (void)channel; power_requested = true; }

int main(void)
{
    if (!graphics_init()) return EXIT_FAILURE;
    render_init();
    input_init();
    SYS_SetResetCallback(on_reset);
    SYS_SetPowerCallback(on_power);
    WPAD_SetPowerButtonCallback(on_remote_power);

    /* The original's opening, title screen and main menu (front_end.c), then
       the tutorial with the save file chosen (the Intro Movie's
       StartChar("8-E1-1", 0, 0, ...)). Falls back to the movement lab if the
       course data cannot be used. */
    static CoursePack pack;
    static CourseSession session;
    const bool tutorial = course_pack_parse(course_8e1_bin, course_8e1_bin_size, &pack) && course_render_init();
    if (tutorial) audio_init(); /* after the course renderer's MEM2 reservation */
    /* The save file (SD card): the four files' progress and achievements, and
       Mute Music. A save from before achievements existed, or progress the
       file already proves, is awarded when the file is played. */
    static SaveData save;
    static Achievements ach;
    save_io_load(&save);
    sfx_set_music_muted(save.music_muted);
    static FrontEnd front;
    front_init(&front);
    const bool menus = tutorial && front_render_init(&save);
    bool in_front = menus, in_course = false;
    if (tutorial && !menus) {   /* no front end: straight into the tutorial with the last file */
        ach_init(&ach, save.slot[save.current].used ? &save.slot[save.current].progress : NULL);
        ach_evaluate_all(&ach);
        in_course = course_session_start(&session, &pack, "8-E1-1", 0, 0, graphics_width());
        session.ach = &ach; pause_init(&session.pause, save.music_muted);
    }
    unsigned since_write = 0;
    Game lab;
    if (!tutorial) game_init(&lab, &movement_world, graphics_width());
    GameInput pending = {0};
    u64 previous = gettime();
    double accumulator = 0;
    while (!exit_requested && !power_requested && SYS_MainLoop()) {
        if (!tutorial && lab.quit) break;
        if (in_front) {   /* the menu's view of the save files: the grid's and the detail panel's */
            for (unsigned k = 0; k < FE_FILES; ++k) {
                const SaveSlot *sl = &save.slot[k];
                Achievements a;
                ach_init(&a, &sl->progress);
                const AchSummary sum = ach_summary(&a);
                front.files.shines[k] = sl->used ? ach_popcount(sl->progress.shines) : 0;
                front.files.star_coins[k] = sl->used ? ach_popcount(sl->progress.star_coins) : 0;
                front.files.achievements[k] = sl->used ? sum.total : 0;
                for (unsigned t = 0; t < 4; ++t) front.files.trophies[k][t] = sl->used ? sum.per_tier[t] : 0;
            }
        }
        const u64 now = gettime();
        double elapsed = ticks_to_microsecs(now - previous) / 1000000.0;
        previous = now;
        if (elapsed > 0.25) elapsed = 0.25;
        accumulator += elapsed;
        game_input_latch(&pending, input_poll());
        unsigned steps = 0;
        while (accumulator >= GAME_DT && steps < 8) {
            if (in_front) front_step(&front, &pending);
            else if (in_course) course_session_step(&session, pending);
            else game_step(&lab, &movement_world, pending, graphics_width());
            game_input_consume(&pending);
            accumulator -= GAME_DT;
            ++steps;
        }
        /* Bound catch-up after a stall; never replace fixed ticks with a large dt.
           PAL 50 Hz renders normally execute one or two 60 Hz simulation ticks. */
        if (steps == 8 && accumulator >= GAME_DT) accumulator = 0;
        if (in_front && front.files.changed) {   /* the menu copied or erased a file */
            if (front.files.copy_from >= 0 && front.files.copy_to >= 0)
                save.slot[front.files.copy_to] = save.slot[front.files.copy_from];   /* _root.CopyFile */
            if (front.files.erase >= 0) memset(&save.slot[front.files.erase], 0, sizeof save.slot[0]);   /* DeleteFile */
            save_io_write(&save);
            front.files.changed = false; front.files.copy_from = front.files.copy_to = front.files.erase = -1;
        }
        if (in_front && front.state == FE_PLAY) {   /* Play: LoadFile() and the tutorial */
            save.current = (unsigned char)front.play_slot;
            SaveSlot *slot = &save.slot[save.current];
            ach_init(&ach, slot->used ? &slot->progress : NULL);
            ach_evaluate_all(&ach);
            front_render_release();
            in_course = course_session_start(&session, &pack, "8-E1-1", 0, 0, graphics_width());
            session.ach = &ach; pause_init(&session.pause, sfx_music_muted());
            in_front = !in_course;
            if (!in_course) front_to_title(&front);
        }
        if (in_course && session.game.quit) {   /* Quit Game: ReturnToTitle() */
            in_course = false; in_front = true;
            front_to_title(&front);
        }
        /* Write at the game's save points, or soon after an unlock; never every frame. */
        ++since_write;
        if (in_course && (session.save_request || (ach.dirty && since_write > 120))) {
            SaveSlot *slot = &save.slot[save.current];
            slot->used = true; slot->progress = ach.p;
            save.music_muted = sfx_music_muted();
            if (save_io_write(&save)) ach.dirty = false;
            session.save_request = false; since_write = 0;
        }
        if (tutorial) audio_update(in_course && session.game.paused);
        if (in_front) front_render(&front);
        else if (in_course) course_render(&session, (float)(accumulator / GAME_DT));
        else render_game(&lab, &movement_world, (float)(accumulator / GAME_DT));
        graphics_present();
    }
    audio_shutdown();
    graphics_shutdown();
    input_shutdown();
    if (power_requested) SYS_ResetSystem(SYS_POWEROFF, 0, 0);
    return EXIT_SUCCESS; /* Homebrew Channel's loader return path. */
}
