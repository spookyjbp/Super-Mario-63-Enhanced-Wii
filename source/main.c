#include "graphics.h"
#include "input.h"
#include "game.h"
#include "render.h"
#include "course_render.h"
#include "course_8e1_bin.h"

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
        if (tutorial) course_render(&session, (float)(accumulator / GAME_DT));
        else render_game(&lab, &movement_world, (float)(accumulator / GAME_DT));
        graphics_present();
    }
    graphics_shutdown();
    input_shutdown();
    if (power_requested) SYS_ResetSystem(SYS_POWEROFF, 0, 0);
    return EXIT_SUCCESS; /* Homebrew Channel's loader return path. */
}
