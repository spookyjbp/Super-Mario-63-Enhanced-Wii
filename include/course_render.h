#ifndef SM63_COURSE_RENDER_H
#define SM63_COURSE_RENDER_H
#include "course_session.h"
/* Wii renderer for original course areas (artwork from course_8e1_art.bin). */
bool course_render_init(void);
void course_render(const CourseSession *session, float interpolation);
#endif
