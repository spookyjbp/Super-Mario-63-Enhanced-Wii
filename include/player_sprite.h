#ifndef SM63_PLAYER_SPRITE_H
#define SM63_PLAYER_SPRITE_H

#include "game.h"

typedef struct {
    unsigned frame;
    bool flip_x;
} PlayerSprite;

/* Pure presentation selection. Never change the authoritative player facing. */
PlayerSprite player_sprite_select(const Player *player);

#endif
