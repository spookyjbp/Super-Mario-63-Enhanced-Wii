#include "game.h"
#include "movement_level.h"
/* Immutable, versioned tile playground. No Story Mode or level editor code. */
const World movement_world = {
    .width=MOVEMENT_WORLD_WIDTH, .height=MOVEMENT_WORLD_HEIGHT,
    .spawn_x=MOVEMENT_SPAWN_X, .spawn_y=MOVEMENT_SPAWN_Y, .level=&movement_level
};
