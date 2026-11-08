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
#include "cinema.h"
#include "cinema_render.h"
#include "data_files.h"
#include "font_metrics.h"
#include <string.h>
#include <stdio.h>
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

/* A data file could not be used: say which, where the game looked and why,
   until START / reset. */
static int missing_data(const char *file, bool found_folder, bool exists)
{
    if (!course_render_init_text()) return EXIT_FAILURE;
    char what[256], where[256];
    if (!found_folder) {
        snprintf(what, sizeof what, "The game's data folder was not found.");
        snprintf(where, sizeof where, "Looked for apps/" DATA_APP "/data on the SD card and USB drive%s.",
                 data_mount() ? "" : " (no SD card or USB drive could be read)");
    } else {
        snprintf(what, sizeof what, exists ? "The data file %s could not be loaded (not enough memory?)."
                                           : "The data file %s is missing.", file);
        snprintf(where, sizeof where, "Data folder: %s", data_folder());
    }
    while (!exit_requested && !power_requested && SYS_MainLoop()) {
        const GameInput in = input_poll();
        if (in.pause_pressed) break;
        const float w = graphics_width();
        graphics_rect(0, 0, w, 480, (GXColor){0, 0, 0, 255});
        course_render_text(FONT_SIGN, what, 40, w - 40, 130, (GXColor){255, 255, 255, 255}, ALIGN_CENTRE, true);
        course_render_text(FONT_HINT, where, 40, w - 40, 200, (GXColor){255, 220, 120, 255}, ALIGN_CENTRE, true);
        course_render_text(FONT_HINT,
                           "Copy the whole apps/" DATA_APP " folder, with its data folder,\rto the SD card, "
                           "then start the game from the Homebrew Channel.\r(Dolphin: Config > Wii > SD card inserted, "
                           "and its SD sync folder holds apps/" DATA_APP "/data.)\r\rPress START to leave.",
                           40, w - 40, 250, (GXColor){220, 220, 220, 255}, ALIGN_CENTRE, true);
        graphics_present();
    }
    graphics_shutdown();
    input_shutdown();
    return EXIT_SUCCESS;
}
static bool data_exists(const char *name)
{
    char path[320];
    if (!data_folder()) return false;
    snprintf(path, sizeof path, "%s/%s", data_folder(), name);
    FILE *f = fopen(path, "rb");
    if (f) fclose(f);
    return f != NULL;
}

/* The data files in MEM1 (data_files.h): the front end's and the course
   art stay while they are used; a cutscene's file takes their place while it
   plays (they are read again after it). */
static uint8_t *fe_data, *art_data, *cin_data;
static size_t fe_size, art_size, cin_size;
static SaveData save;
static CinemaData cin_file;
static Cinema cin;

static bool load_front(void)
{
    if (!fe_data && !(fe_data = data_load("front_end.bin", &fe_size))) return false;
    return front_render_init(&save, fe_data, fe_size);
}
static void drop_front(void)
{
    front_render_release();
    if (fe_data) data_free(fe_data);
    fe_data = NULL;
}
static bool load_art(void)
{
    if (art_data) return true;
    if (!(art_data = data_load("course_8e1_art.bin", &art_size))) return false;
    return course_render_set_art(art_data, art_size);
}
static void drop_art(void)
{
    if (art_data) data_free(art_data);
    art_data = NULL;
}
/* Start a story cutscene (cinema.h); false when its file cannot be used. */
static bool begin_cinema(const char *name)
{
    drop_front();
    drop_art();
    if (!(cin_data = data_load(name, &cin_size))) return false;
    if (cinema_parse(cin_data, cin_size, &cin_file) && cinema_render_begin(&cin_file)) {
        cinema_start(&cin, &cin_file);
        return true;
    }
    data_free(cin_data);
    cin_data = NULL;
    return false;
}
static void end_cinema(void)
{
    if (!cin_data) return;
    cinema_render_end();
    data_free(cin_data);
    cin_data = NULL;
}

/* StartChar(course, x, y): the session with the save slot's achievements. */
static bool start_course(CourseSession *session, const CoursePack *pack, Achievements *ach, const char *course,
                         float x, float y)
{
    drop_front();   /* its MEM1 for the course (read again for the title) */
    if (!load_art() || !course_session_start(session, pack, course, x, y, graphics_width())) return false;
    session->ach = ach;
    session->total_stars = ach_popcount(ach->p.shines);   /* _root.TotalStars */
    pause_init(&session->pause, sfx_music_muted());
    return true;
}

int main(int argc, char **argv)
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
    /* The art packs are data files next to boot.dol (data_files.h). */
    /* One at a time (MEM1): the course art for the renderer's setup, then,
       with it let go (load_art reads it again for a course), the front end. */
    if (!data_init(argc, argv)) return missing_data("", false, false);
    static const char *const needed[] = {"course_8e1_art.bin", "front_end.bin", "cinema_intro.bin", "cinema_intro2.bin"};
    for (unsigned i = 0; i < sizeof needed / sizeof needed[0]; ++i)
        if (!data_exists(needed[i])) return missing_data(needed[i], true, false);
    if (!(art_data = data_load("course_8e1_art.bin", &art_size))) return missing_data("course_8e1_art.bin", true, true);
    const bool tutorial = course_pack_parse(course_8e1_bin, course_8e1_bin_size, &pack) &&
                          course_render_init(art_data, art_size);
    if (tutorial) audio_init(); /* after the course renderer's MEM2 reservation */
    drop_art();
    if (!(fe_data = data_load("front_end.bin", &fe_size))) return missing_data("front_end.bin", true, true);
    /* The save file (SD card): the four files' progress and achievements, and
       Mute Music. A save from before achievements existed, or progress the
       file already proves, is awarded when the file is played. */
    static Achievements ach;
    save_io_load(&save);
    sfx_set_music_muted(save.music_muted);
    static FrontEnd front;
    front_init(&front);
    const bool menus = tutorial && front_render_init(&save, fe_data, fe_size);
    bool in_front = menus, in_course = false, in_cinema = false;
    if (tutorial && !menus) {   /* no front end: straight into the tutorial with the last file */
        ach_init(&ach, save.slot[save.current].used ? &save.slot[save.current].progress : NULL);
        ach_evaluate_all(&ach);
        in_course = load_art() && course_session_start(&session, &pack, "8-E1-1", 0, 0, graphics_width());
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
            else if (in_cinema)   /* the read button (or jump) goes on, START skips */
                cinema_step(&cin, pending.pound_pressed || pending.jump_pressed || pending.confirm_pressed,
                            pending.pause_pressed);
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
        if (in_front && front.state == FE_PLAY) {   /* Play: LoadFile() */
            save.current = (unsigned char)front.play_slot;
            SaveSlot *slot = &save.slot[save.current];
            ach_init(&ach, slot->used ? &slot->progress : NULL);
            ach_evaluate_all(&ach);
            front_render_release();
            in_front = false;
            /* a new file (TotalStars 0): gotoAndStop("Intro"), the story
               before the tutorial; the Intro's end starts the tutorial */
            if (ach_popcount(ach.p.shines) == 0 && begin_cinema("cinema_intro.bin")) {
                in_cinema = true;
            } else {
                /* a file with Shine Sprites: _root.CharXscale = -100;
                   StartChar("Castle", -1100, 100, ...) (no tutorial: 8-E1-1) */
                const bool castle = ach_popcount(ach.p.shines) > 0;
                in_course = start_course(&session, &pack, &ach, castle ? "Castle" : "8-E1-1", castle ? -1100 : 0,
                                         castle ? 100 : 0);
                if (in_course && castle) session.game.player.facing_left = true;
                if (!in_course && load_front()) { in_front = true; front_to_title(&front); }
            }
        }
        if (in_cinema && cin.done) {   /* the cutscene's StartChar */
            end_cinema();
            in_cinema = false;
            in_course = start_course(&session, &pack, &ach, cin.course, cin.end_x, cin.end_y);
            if (!in_course) { load_art(); in_front = load_front(); front_to_title(&front); }
        }
        if (in_course && session.state == SESSION_COMPLETE && !session.save_request) {
            /* the tutorial's last pipe: Intro Movie 2, then the Castle */
            in_course = false;
            in_cinema = begin_cinema("cinema_intro2.bin");
            if (!in_cinema) { load_art(); in_front = load_front(); front_to_title(&front); }
        }
        if (in_course && session.game.quit) {   /* Quit Game: ReturnToTitle() */
            in_course = false;
            in_front = load_front();
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
        if (!in_course) sfx_set_ambient(SFX_COUNT, 0, 0);   /* the place's sound stays with the course */
        if (tutorial && in_cinema) audio_cinema_update(&cin);
        else if (tutorial) audio_update(in_course && session.game.paused);
        if (in_front) front_render(&front);
        else if (in_cinema) cinema_render(&cin, pending.controller);
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
