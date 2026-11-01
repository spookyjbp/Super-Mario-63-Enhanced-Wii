#ifndef SM63_OBJECT_SELECT_H
#define SM63_OBJECT_SELECT_H
/* Which object-art clip/frame shows a live object (pure; host-tested). */
#include "course_objects.h"
#define OBJECT_DRAW_MAX 2
/* rotation (degrees, clockwise) turns about (0, pivot_y) from the registration
   point; cxform is a Flash colour transform (multipliers, offsets 0..255). */
typedef struct {
    int clip; unsigned frame; float alpha, dx, dy, scale, rotation, pivot_y;
    bool flip_x, flip_y, cxform; float cx_mul[3], cx_add[3];
} ObjectDraw;
/* Returns the number of layers to draw (0..2), back to front. */
unsigned object_select(const LiveObject *l, ObjectDraw out[OBJECT_DRAW_MAX]);
#endif
