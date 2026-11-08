#ifndef SM63_COURSE_RENDER_H
#define SM63_COURSE_RENDER_H
#include "course_session.h"
/* Wii renderer for original course areas (artwork from course_8e1_art.bin). */
bool course_render_init(void);
void course_render(const CourseSession *session, float interpolation);
/* For the front end (front_render.c), which runs while no course is shown:
   the course's MEM2 texture area (the course reloads its area afterwards),
   text in the original fonts (font_metrics.h FONT_*), and the trophies. */
#include <gccore.h>
enum { ALIGN_LEFT, ALIGN_CENTRE, ALIGN_RIGHT };
unsigned char *course_render_scratch(size_t *size);
void course_render_text(unsigned font, const char *text, float x0, float x1, float y, GXColor colour, int align,
                        bool glow);
void course_render_trophy(unsigned tier, float x, float y, float size, bool lit, float alpha);
#endif
