#include "graphics.h"
#include "input.h"

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

    float x = graphics_width() / 2, y = 372, velocity_y = 0;
    u64 previous = gettime();
    while (!exit_requested && !power_requested && SYS_MainLoop()) {
        const u64 now = gettime();
        float dt = ticks_to_microsecs(now - previous) / 1000000.0f;
        previous = now;
        if (dt > 0.05f) dt = 0.05f;
        const InputState input = input_poll();
        if (input.exit_pressed) break;

        /* Placeholder motion for input validation, not the original game physics. */
        x += input.horizontal * 220 * dt;
        if (x < 24) x = 24;
        if (x > graphics_width() - 52) x = graphics_width() - 52;
        if (input.jump_pressed && y >= 372) velocity_y = -400;
        velocity_y += 1000 * dt;
        y += velocity_y * dt;
        if (y > 372) { y = 372; velocity_y = 0; }

        const float width = graphics_width();
        graphics_rect(0, 0, width, 480, (GXColor){100, 177, 235, 255});
        graphics_rect(width - 130, 55, 48, 48, (GXColor){255, 226, 120, 255});
        graphics_rect(55, 95, 115, 22, (GXColor){245, 250, 255, 255});
        graphics_rect(160, 310, 190, 110, (GXColor){66, 150, 100, 255});
        graphics_rect(0, 400, width, 80, (GXColor){138, 91, 55, 255});
        graphics_rect(0, 400, width, 12, (GXColor){74, 188, 79, 255});
        graphics_rect(x, y, 28, 28, (GXColor){226, 62, 65, 255});
        graphics_present();
    }
    graphics_shutdown();
    input_shutdown();
    if (power_requested) SYS_ResetSystem(SYS_POWEROFF, 0, 0);
    return EXIT_SUCCESS; /* Homebrew Channel's loader return path. */
}
