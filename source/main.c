#include "graphics.h"
#include "input.h"
#include "game.h"
#include "render.h"
#include "course_render.h"
#include "course_8e1_bin.h"
#include "audio.h"
#include "save_io.h"
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

    /* Boot into the original tutorial (Intro Movie: StartChar("8-E1-1", 0, 0, ...)).
       Falls back to the movement lab if the course data cannot be used. */
    static CoursePack pack;
    static CourseSession session;
    const bool tutorial = course_pack_parse(course_8e1_bin, course_8e1_bin_size, &pack) && course_render_init() &&
                          course_session_start(&session, &pack, "8-E1-1", 0, 0, graphics_width());
    if (tutorial) audio_init(); /* after the course renderer's MEM2 reservation */
    /* The save file (SD card): achievements and progress of the slot played,
       and the Mute Music option. A save from before achievements existed, or
       progress the file already proves, is awarded on load (queued cards). */
    static SaveData save;
    static Achievements ach;
    save_io_load(&save);
    SaveSlot *slot = &save.slot[save.current];
    ach_init(&ach, slot->used ? &slot->progress : NULL);
    ach_evaluate_all(&ach);
    sfx_set_music_muted(save.music_muted);
    if (tutorial) { session.ach = &ach; pause_init(&session.pause, save.music_muted); }
    unsigned since_write = 0;
    Game lab;
    if (!tutorial) game_init(&lab, &movement_world, graphics_width());
    Game *game = tutorial ? &session.game : &lab;
    GameInput pending = {0};
    u64 previous = gettime();
    double accumulator = 0;
    while (!exit_requested && !power_requested && !game->quit && SYS_MainLoop()) {
        const u64 now = gettime();
        double elapsed = ticks_to_microsecs(now - previous) / 1000000.0;
        previous = now;
        if (elapsed > 0.25) elapsed = 0.25;
        accumulator += elapsed;
        game_input_latch(&pending, input_poll());
        unsigned steps = 0;
        while (accumulator >= GAME_DT && steps < 8) {
            if (tutorial) course_session_step(&session, pending);
            else game_step(&lab, &movement_world, pending, graphics_width());
            game_input_consume(&pending);
            accumulator -= GAME_DT;
            ++steps;
        }
        /* Bound catch-up after a stall; never replace fixed ticks with a large dt.
           PAL 50 Hz renders normally execute one or two 60 Hz simulation ticks. */
        if (steps == 8 && accumulator >= GAME_DT) accumulator = 0;
        /* Write at the game's save points, or soon after an unlock; never every frame. */
        ++since_write;
        if (tutorial && (session.save_request || (ach.dirty && since_write > 120))) {
            slot->used = true; slot->progress = ach.p;
            save.music_muted = sfx_music_muted();
            if (save_io_write(&save)) ach.dirty = false;
            session.save_request = false; since_write = 0;
        }
        if (tutorial) audio_update(session.game.paused);
        if (tutorial) course_render(&session, (float)(accumulator / GAME_DT));
        else render_game(&lab, &movement_world, (float)(accumulator / GAME_DT));
        graphics_present();
    }
    audio_shutdown();
    graphics_shutdown();
    input_shutdown();
    if (power_requested) SYS_ResetSystem(SYS_POWEROFF, 0, 0);
    return EXIT_SUCCESS; /* Homebrew Channel's loader return path. */
}
