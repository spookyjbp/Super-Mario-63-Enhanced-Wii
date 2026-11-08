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
void graphics_quad_color(const float xs[4], const float ys[4], GXColor color);
void graphics_triangle(float x0,float y0,float x1,float y1,float x2,float y2,GXColor color);
void graphics_sprite_rotated(GXTexObj *texture,float x,float y,float width,float height,
                             float u0,float v0,float u1,float v1,float degrees);
void graphics_text(float x, float y, const char *text, float scale, GXColor color);
void graphics_sprite(GXTexObj *texture, float x, float y, float width, float height,
                     float u0, float v0, float u1, float v1);
/* Scale all following geometry about the screen origin (1 = none). */
void graphics_set_zoom(float zoom);
/* Textured quad with alpha 0..255 multiplied in; u/v may be swapped to flip. */
void graphics_sprite_alpha(GXTexObj *texture, float x, float y, float width, float height,
                           float u0, float v0, float u1, float v1, unsigned char alpha);
/* Arbitrary textured quad (corners clockwise from the u0,v0 corner) with alpha. */
void graphics_quad_alpha(GXTexObj *texture, const float xs[4], const float ys[4],
                         float u0, float v0, float u1, float v1, unsigned char alpha);
/* Same quad with a Flash colour transform: rgb * mul[0..2] + add (0..255),
   alpha * mul[3]; multipliers may exceed 1 (up to 2). */
void graphics_quad_cxform(GXTexObj *texture, const float xs[4], const float ys[4],
                          float u0, float v0, float u1, float v1, const float mul[4], const float add[3]);
/* A convex polygon (triangle fan, n >= 3) with per-vertex texture
   coordinates and a Flash colour transform as graphics_quad_cxform. */
void graphics_poly_cxform(GXTexObj *texture, unsigned n, const float *xs, const float *ys, const float *us,
                          const float *vs, const float mul[4], const float add[3]);
/* Quad with its own texture coordinates per corner (repeating textures) and alpha. */
void graphics_quad_uv(GXTexObj *texture, const float xs[4], const float ys[4], const float us[4], const float vs[4],
                      unsigned char alpha);
/* Clip drawing to a screen rectangle (logical px) until graphics_scissor_off(). */
void graphics_scissor(float x, float y, float w, float h);
void graphics_scissor_off(void);
void graphics_present(void);
void graphics_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif
