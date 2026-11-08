#ifndef SM63_CINEMA_RENDER_H
#define SM63_CINEMA_RENDER_H
/* Draws a story cutscene (cinema.h) and plays its sound (Wii only). */
#include "cinema.h"
#include <stdbool.h>
/* Sets up the cutscene's textures, draws and sounds in the course renderer's
   MEM2 area (no course is shown meanwhile); false when they do not fit. */
bool cinema_render_begin(const CinemaData *d);
void cinema_render(const Cinema *p, unsigned controller);
void cinema_render_end(void);
#endif
