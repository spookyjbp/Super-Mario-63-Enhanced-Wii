#ifndef SM63_GRAPHICS_H
#define SM63_GRAPHICS_H

#include <stdbool.h>
#include <gccore.h>

#ifdef __cplusplus
extern "C" {
#endif

bool graphics_init(void);
float graphics_width(void);
void graphics_rect(float x, float y, float width, float height, GXColor color);
void graphics_triangle(float x0,float y0,float x1,float y1,float x2,float y2,GXColor color);
void graphics_sprite_rotated(GXTexObj *texture,float x,float y,float width,float height,
                             float u0,float v0,float u1,float v1,float degrees);
void graphics_text(float x, float y, const char *text, float scale, GXColor color);
void graphics_sprite(GXTexObj *texture, float x, float y, float width, float height,
                     float u0, float v0, float u1, float v1);
/* Scale all following geometry about the screen origin (1 = none). */
void graphics_set_zoom(float zoom);
void graphics_present(void);
void graphics_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif
