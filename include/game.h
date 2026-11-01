#ifndef SM63_GAME_H
#define SM63_GAME_H

#include <stdbool.h>
#include <stddef.h>

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
} World;

typedef struct {
    float horizontal;
    bool jump_pressed, jump_held, spin_pressed, pound_pressed;
    bool pause_pressed, confirm_pressed, cancel_pressed;
    int menu_delta;
} GameInput;

typedef enum {
    PLAYER_IDLE, PLAYER_RUN, PLAYER_RISE, PLAYER_FALL,
    PLAYER_SPIN, PLAYER_POUND_WINDUP, PLAYER_POUND, PLAYER_LAND
} PlayerAnimation;

typedef struct {
    float x, y, previous_x, previous_y, vx, vy;
    bool grounded, facing_left, air_spin_used, jump_cut;
    unsigned jump_stage;
    float coyote, jump_buffer, chain_window, spin_time, spin_cooldown;
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
/* One fixed 1/60 second step. Solid platforms are static, non-overlapping AABBs. */
void player_step(Player *player, const World *world, GameInput input);
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
