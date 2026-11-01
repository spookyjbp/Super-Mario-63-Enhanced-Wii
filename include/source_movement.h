#ifndef SM63_SOURCE_MOVEMENT_H
#define SM63_SOURCE_MOVEMENT_H
#include "game.h"
void player_source_begin(Player *p, GameInput input);
void player_spin_prepare(Player *p, GameInput input);
void player_spin_update(Player *p, GameInput input);
bool player_swim_step(Player *p, const World *w, GameInput input);
#endif
