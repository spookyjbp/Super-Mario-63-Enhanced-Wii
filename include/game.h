#ifndef SM63_GAME_H
#define SM63_GAME_H

#include <stdbool.h>
#include <stddef.h>
#include "level.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GAME_DT (1.0f / 60.0f)
#define PLAYER_WIDTH 28.0f
#define PLAYER_HEIGHT 44.0f

typedef struct { float x, y, w, h; } Rect;
struct CourseRuntime;
typedef struct {
    const Rect *solids;
    size_t count;
    float width, height, spawn_x, spawn_y;
    const Level *level;
    struct CourseRuntime *course; /* original course area; NULL for tile levels */
} World;

typedef struct {
    float horizontal, vertical;
    bool crouch_held, nozzle_held, nozzle_next_pressed;
    bool jump_pressed, jump_held, spin_pressed, spin_held, pound_pressed, pound_held;
    bool pause_pressed, confirm_pressed, cancel_pressed;
    int menu_delta;
    /* Camera zoom (not in the original's controls): -1 (zoom in) .. 1 (zoom
       out), from a second stick's vertical axis or the Wii Remote's + / -. */
    float zoom_axis;
    /* Controller the player used last (for the controller picture signs). */
    unsigned char controller;
} GameInput;
/* Supported controllers (the Nunchuk is not supported: a Wii Remote with one
   attached plays as a sideways Wii Remote). */
enum { CONTROLLER_GAMECUBE, CONTROLLER_CLASSIC, CONTROLLER_WIIMOTE, CONTROLLER_KINDS };

typedef enum {
    PLAYER_IDLE, PLAYER_RUN, PLAYER_RISE, PLAYER_FALL,
    PLAYER_SPIN, PLAYER_POUND_WINDUP, PLAYER_POUND, PLAYER_LAND,
    PLAYER_CROUCH, PLAYER_DIVE, PLAYER_BACKFLIP, PLAYER_ROLL, PLAYER_SWIM,
    PLAYER_HOVER, PLAYER_ROCKET_CHARGE, PLAYER_ROCKET, PLAYER_TURBO, PLAYER_BONK,
    PLAYER_DIVE_RECOVER, /* Dive clip frames 0 and 6..9 show Mario Air (none) - 2. */
    PLAYER_ANIMATION_COUNT
} PlayerAnimation;

/* ACTION_BONK: Mario Main "hurt2" (wall bonk, or knocked backward by MarioHit);
   ACTION_HURT: "hurt1" / "hurt3" (hurt_kind 1 or 3). */
typedef enum { ACTION_NONE, ACTION_DIVE, ACTION_BACKFLIP, ACTION_ROLL, ACTION_BONK, ACTION_HURT } PlayerAction;
typedef enum { NOZZLE_HOVER, NOZZLE_ROCKET, NOZZLE_TURBO, NOZZLE_COUNT } Nozzle;

typedef struct {
    unsigned source_phase, swim_frames, spin_combo_count;
    /* Zero-based frame of Mario (None) Dive.xml: 0 entry, stop() at 2,
       recovery plays 3..9 and frame 9 clears the attack. */
    unsigned dive_frame;
    /* Zero-based frame of Mario Ground Pound.xml: 0-11 windup, 12-26 fall,
       27-34 "hit", 35-41 "end", 42 clears the attack. */
    unsigned pound_frame;
    /* Presentation counters that the original clip scripts keep (32 Hz):
       Walk.xml `frame += xspeed` (1..80), Running.xml phase, Wait WaterBlast
       level 0..4 with its `_yscale *= -1` parity, Dive.xml airborne rotation. */
    float walk_frame, run_frame, dive_rotation, dive_target;
    unsigned run_count, blast_level, art_clip;
    bool blast_parity;
    bool dive_recovering;
    bool source_step, swim_stroke, spin_requested, backflip_facing_left;
    int spin_combo_direction;
    float spin_speed, spin_frame, spin_combo_window, jump_time, normal_land_time;
    float height, action_time, wall_speed, ground_slope;
    float ground_angle;      /* course ground angle under the feet (degrees; Dive.xml groundrot) */
    float water, power, rocket_charge, rocket_boost, tap_window;
    int wall_normal, last_tap, bonk_direction;
    bool submerged, jet_active, dashing, tap_released;
    bool has_fludd;          /* FLUDD worn: _root.Fluddpow != "" (false until the FLUDD box) */
    unsigned nozzles;        /* nozzles collected, bit (1 << Nozzle) (FluddArray / SaveFludd*) */
    int blink, stun;         /* MarioHit: invulnerable blink and stun, 32 Hz frames */
    bool ejump;              /* attackFrame "EJump": held on a stomped enemy */
    float input_horizontal;  /* held direction this tick, for wall rules */
    int hurtfall;            /* CharCode hurtfallcount (0..7); "Mario-Fall" at 5 */
    unsigned bonk_frame;     /* Mario Main "hurt2" clip (1422) frame, 0-based, during ACTION_BONK */
    bool reading_sign;       /* attackFrame "Sign": the reading pose (Main "Sign", 1487) */
    unsigned hurt_kind, hurt_frame; /* ACTION_HURT: "hurt1" (1411) or "hurt3" (1430), 0-based clip frame */
    bool fainting;                  /* CharHP <= 0: the hurt clips play their faint frames (to 107 / 70) */
    float set_vx, set_vy;    /* MarioHit setXspeed / setYspeed, added at hurt1 frame 4 (px/s) */
    bool fall_sfx;           /* fallingsfx: once per fall */
    TileType material;
    PlayerAction action;
    Nozzle nozzle;
    float x, y, previous_x, previous_y, vx, vy;
    bool grounded, facing_left, jump_cut;
    unsigned jump_stage;
    float coyote, jump_buffer, chain_window, spin_time;
    float pound_windup, landing_time, animation_time;
    bool pounding;
    PlayerAnimation animation;
} Player;

/* x, y: view top-left in world px, published every 60 Hz tick. The original
   camera moves once per 32 Hz source frame; (from_x, from_y) -> (to_x, to_y)
   is the latest such move, shown progressively over the following source frame
   so the view does not jump on 32 of every 60 ticks (Mario judder / doubling). */
typedef struct { float x, y, previous_x, previous_y; float cx, cy, ychange; float from_x, from_y, to_x, to_y; } Camera;
typedef struct {
    Player player;
    Camera camera;
    bool paused, quit;
    unsigned menu_selection;
} Game;

extern const World movement_world;
void player_init(Player *player, float x, float y);
/* One fixed 1/60 second step over immutable rectangles/tiles and water volumes. */
void player_step(Player *player, const World *world, GameInput input);
float player_height(const Player *player);
bool player_special_step(Player *player, const World *world, GameInput input, bool was_grounded);
void player_fludd_step(Player *player, GameInput input);
/* Next FLUDD state: None -> Hover -> Rocket -> Turbo -> None, skipping nozzles
   not collected. False (nothing happens) before any nozzle is collected. */
bool player_change_fludd(Player *player);
/* Advance presentation-only clip counters; call once at the end of player_step. */
void player_animation_step(Player *player, GameInput input);
void player_collision_step(Player *player, const World *world);
bool player_stand_clear(const Player *player, const World *world);
bool world_water_at(const World *world, float x, float y, float *surface);
bool world_hazard_at(const World *world, float x, float y);
bool player_attack_hitbox(const Player *player, Rect *hitbox);
void camera_init(Camera *camera, const Player *player, const World *world, float view_width);
void camera_step(Camera *camera, const Player *player, const World *world, float view_width);
void game_init(Game *game, const World *world, float view_width);
void game_step(Game *game, const World *world, GameInput input, float view_width);
/* Retain presses until a simulation tick; consume them only once during catch-up. */
void game_input_latch(GameInput *pending, GameInput current);
void game_input_consume(GameInput *pending);

#ifdef __cplusplus
}
#endif

#endif
