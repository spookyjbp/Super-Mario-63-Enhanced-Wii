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
typedef struct {
    const Rect *solids;
    size_t count;
    float width, height, spawn_x, spawn_y;
    const Level *level;
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
    PLAYER_ANIMATION_COUNT
} PlayerAnimation;

typedef enum { ACTION_NONE, ACTION_DIVE, ACTION_BACKFLIP, ACTION_ROLL, ACTION_BONK } PlayerAction;
typedef enum { NOZZLE_HOVER, NOZZLE_ROCKET, NOZZLE_TURBO, NOZZLE_COUNT } Nozzle;

typedef struct {
    unsigned source_phase, swim_frames, spin_combo_count;
    bool source_step, swim_stroke, spin_requested, backflip_facing_left;
    int spin_combo_direction;
    float spin_speed, spin_frame, spin_combo_window, jump_time, normal_land_time;
    float height, action_time, wall_speed, ground_slope;
    float water, power, rocket_charge, rocket_boost, tap_window;
    int wall_normal, last_tap, bonk_direction;
    bool submerged, jet_active, dashing, tap_released;
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

typedef struct { float x, y, previous_x, previous_y; } Camera;
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
