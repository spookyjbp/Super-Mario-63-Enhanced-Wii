#include "graphics.h"
#include "input.h"
#include "game.h"
#include "render.h"

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
    input_init();
    SYS_SetResetCallback(on_reset);
    SYS_SetPowerCallback(on_power);
    WPAD_SetPowerButtonCallback(on_remote_power);

    Game game;
    game_init(&game, &movement_world, graphics_width());
    GameInput pending = {0};
    u64 previous = gettime();
    double accumulator = 0;
    while (!exit_requested && !power_requested && !game.quit && SYS_MainLoop()) {
        const u64 now = gettime();
        double elapsed = ticks_to_microsecs(now - previous) / 1000000.0;
        previous = now;
        if (elapsed > 0.25) elapsed = 0.25;
        accumulator += elapsed;
        game_input_latch(&pending, input_poll());
        unsigned steps = 0;
        while (accumulator >= GAME_DT && steps < 8) {
            game_step(&game, &movement_world, pending, graphics_width());
            game_input_consume(&pending);
            accumulator -= GAME_DT;
            ++steps;
        }
        /* Bound catch-up after a stall; never replace fixed ticks with a large dt.
           PAL 50 Hz renders normally execute one or two 60 Hz simulation ticks. */
        if (steps == 8 && accumulator >= GAME_DT) accumulator = 0;
        render_game(&game, &movement_world, (float)(accumulator / GAME_DT));
        graphics_present();
    }
    graphics_shutdown();
    input_shutdown();
    if (power_requested) SYS_ResetSystem(SYS_POWEROFF, 0, 0);
    return EXIT_SUCCESS; /* Homebrew Channel's loader return path. */
}
