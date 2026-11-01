#ifndef SM63_OBJECT_SELECT_H
#define SM63_OBJECT_SELECT_H
/* Which object-art clip/frame shows a live object (pure; host-tested). */
#include "course_objects.h"
#define OBJECT_DRAW_MAX 2
typedef struct { int clip; unsigned frame; float alpha, dx, dy, scale; bool flip_x, flip_y; } ObjectDraw;
/* Returns the number of layers to draw (0..2), back to front. */
unsigned object_select(const LiveObject *l, ObjectDraw out[OBJECT_DRAW_MAX]);
#endif
