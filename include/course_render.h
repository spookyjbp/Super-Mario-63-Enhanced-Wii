#ifndef SM63_COURSE_RENDER_H
#define SM63_COURSE_RENDER_H
#include "course_session.h"
/* Wii renderer for original course areas (artwork from course_8e1_art.bin). */
/* The course's art pack (a data file, data_files.h; it must stay loaded). */
bool course_render_init(const uint8_t *art_data, size_t art_size);
/* Another course art data file (or the same one loaded again) in place of
   the one given before: its areas are uploaded when next shown. */
bool course_render_set_art(const uint8_t *art_data, size_t art_size);
/* Only the text (course_render_text), for a message when the data files are missing. */
bool course_render_init_text(void);
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
/* The pause menu's row band (dark; chosen: blue with light edges), screen px. */
void course_render_row(float x0, float y, float x1, float h, bool sel, float alpha);
/* A pause menu piece (pause_layout.h PP_*) fitted into a screen box. */
bool course_render_pause_part(unsigned id, float x, float y, float w, float h, unsigned char alpha);
#endif
