#ifndef SM63_FRONT_RENDER_H
#define SM63_FRONT_RENDER_H
/* Wii renderer of the front end (front_end.h): the opening, title and main
   menu from front_end.bin. */
#include "front_end.h"
#include "save_data.h"
#include <stdbool.h>
/* After course_render_init() (it borrows the course's texture memory).
   `data`: front_end.bin (a data file, data_files.h; it must stay loaded). */
bool front_render_init(const SaveData *save, const uint8_t *data, size_t size);
void front_render(const FrontEnd *f);
/* The course is about to use its memory again. */
void front_render_release(void);
#endif
