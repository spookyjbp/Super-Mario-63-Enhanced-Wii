#ifndef SM63_USER_ART_H
#define SM63_USER_ART_H
#include "game.h"
void user_art_init(void);
void user_art_shutdown(void);
bool user_art_available(void);
bool user_art_fludd_available(const Player *p);
bool user_art_draw_player(const Player *p,float x,float y);
void user_art_draw_equipment(const Player *p,float x,float y);
#endif
