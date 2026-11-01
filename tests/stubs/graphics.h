#ifndef SM63_TEST_GRAPHICS_H
#define SM63_TEST_GRAPHICS_H
/* Host recording interface for the real render_game code; no GX emulation. */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
typedef struct { uint8_t r, g, b, a; } GXColor;
typedef struct { unsigned unused; } GXTexObj;
#define GX_FALSE 0
#define GX_TF_RGBA8 0
#define GX_CLAMP 0
#define GX_NEAR 0
#define GX_ANISO_1 0
void DCFlushRange(void *data, unsigned size);
void GX_InitTexObj(GXTexObj *t, void *data, unsigned w, unsigned h,
                   unsigned format, unsigned s, unsigned v, unsigned mip);
void GX_InitTexObjLOD(GXTexObj *t, unsigned min, unsigned mag, float lo,
                      float hi, float bias, unsigned clamp, unsigned edge, unsigned aniso);
void GX_InvalidateTexAll(void);
float graphics_width(void);
void graphics_triangle(float,float,float,float,float,float,GXColor);
void graphics_sprite_rotated(GXTexObj*,float,float,float,float,float,float,float,float,float);
void graphics_rect(float x, float y, float w, float h, GXColor color);
void graphics_text(float x, float y, const char *text, float scale, GXColor color);
void graphics_sprite(GXTexObj *t, float x, float y, float w, float h,
                     float u0, float v0, float u1, float v1);
#endif
