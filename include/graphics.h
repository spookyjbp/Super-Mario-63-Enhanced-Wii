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
void graphics_text(float x, float y, const char *text, float scale, GXColor color);
void graphics_sprite(GXTexObj *texture, float x, float y, float width, float height,
                     float u0, float v0, float u1, float v1);
void graphics_present(void);
void graphics_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif
