#ifndef SM63_OBJECT_SELECT_H
#define SM63_OBJECT_SELECT_H
/* Which object-art clip/frame shows a live object (pure; host-tested). */
#include "course_objects.h"
#define OBJECT_DRAW_MAX 24 /* Green Circle Platform: 18 path dots + pl */
/* rotation (degrees, clockwise) turns about (0, pivot_y) from the registration
   point; cxform is a Flash colour transform (multipliers, offsets 0..255). */
typedef struct {
    int clip; unsigned frame; float alpha, dx, dy, scale, rotation, pivot_y;
    bool flip_x, flip_y, cxform; float cx_mul[3], cx_add[3];
} ObjectDraw;
/* Returns the number of layers to draw, back to front. `controller`
   (CONTROLLER_*) picks the button drawn in the sign and Toad talk prompts. */
unsigned object_select(const LiveObject *l, unsigned controller, ObjectDraw out[OBJECT_DRAW_MAX]);
#endif
