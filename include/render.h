#ifndef SM63_RENDER_H
#define SM63_RENDER_H
#include "game.h"
void render_init(void);
void render_game(const Game *game, const World *world, float interpolation);
#endif
