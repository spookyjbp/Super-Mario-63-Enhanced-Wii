#ifndef SM63_FRONT_RENDER_H
#define SM63_FRONT_RENDER_H
/* Wii renderer of the front end (front_end.h): the opening, title and main
   menu from front_end.bin. */
#include "front_end.h"
#include "save_data.h"
#include <stdbool.h>
/* After course_render_init() (it borrows the course's texture memory). */
bool front_render_init(const SaveData *save);
void front_render(const FrontEnd *f);
/* The course is about to use its memory again. */
void front_render_release(void);
/* How far the credits text scrolls (stage px), once the menu was drawn. */
float front_render_credits_height(void);
#endif
