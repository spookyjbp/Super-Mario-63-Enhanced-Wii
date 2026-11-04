#ifndef SM63_USER_ART_H
#define SM63_USER_ART_H
#include "game.h"
void user_art_init(void);
void user_art_shutdown(void);
/* World pixels per original pixel for player art (1.5 lab, COURSE_SCALE courses). */
void user_art_set_scale(float scale);
bool user_art_available(void);
bool user_art_fludd_available(const Player *p);
bool user_art_draw_player(const Player *p,float x,float y);
/* Mario's opacity (charClip.frame._alpha) for the next draws: 255 normally. */
void user_art_set_alpha(unsigned char alpha);
#endif
