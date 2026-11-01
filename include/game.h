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
    bool jump_pressed, jump_held, spin_pressed, spin_held, pound_pressed;
    bool pause_pressed, confirm_pressed, cancel_pressed;
    int menu_delta;
} GameInput;

typedef enum {
    PLAYER_IDLE, PLAYER_RUN, PLAYER_RISE, PLAYER_FALL,
    PLAYER_SPIN, PLAYER_POUND_WINDUP, PLAYER_POUND, PLAYER_LAND,
    PLAYER_CROUCH, PLAYER_DIVE, PLAYER_BACKFLIP, PLAYER_ROLL, PLAYER_SWIM,
    PLAYER_HOVER, PLAYER_ROCKET_CHARGE, PLAYER_ROCKET, PLAYER_TURBO, PLAYER_BONK,
    PLAYER_DIVE_RECOVER, /* Dive clip frames 0 and 6..9 show Mario Air (none) - 2. */
    PLAYER_ANIMATION_COUNT
} PlayerAnimation;

typedef enum { ACTION_NONE, ACTION_DIVE, ACTION_BACKFLIP, ACTION_ROLL, ACTION_BONK } PlayerAction;
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
    bool has_fludd;          /* false until the FLUDD box (StartChar Fluddpow "") */
    int blink, stun;         /* MarioHit: invulnerable blink and stun, 32 Hz frames */
    bool ejump;              /* attackFrame "EJump": held on a stomped enemy */
    float input_horizontal;  /* held direction this tick, for wall rules */
    int hurtfall;            /* CharCode hurtfallcount (0..7); "Mario-Fall" at 5 */
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

typedef struct { float x, y, previous_x, previous_y; float cx, cy, ychange; } Camera;
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
