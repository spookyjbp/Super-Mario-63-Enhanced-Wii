#ifndef SM63_COURSE_PACK_H
#define SM63_COURSE_PACK_H
/* Original course areas exported by tools/export_course.py (format v1).
   All coordinates are original course pixels (Char at (0,0) for StartChar(a,0,0,0,0)).
   Parsing keeps pointers into the immutable blob: no copies or allocation. */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#define COURSE_MAX_AREAS 8
#define COURSE_MAX_MASKS 12
#define COURSE_MAX_WATER 16
#define COURSE_MAX_OBJECTS 160
typedef enum { COURSE_PLATFORMS, COURSE_BPLATFORMS, COURSE_CAMEDGE, COURSE_ICE, COURSE_PUDDLE,
               COURSE_LAVA, COURSE_LAYER_COUNT } CourseLayer;
typedef struct { const char *text; uint16_t length; } CourseString;
typedef struct {
    CourseLayer layer; unsigned frame;
    int x0, y0; unsigned width, height;
    const unsigned char *offsets, *runs;
} CourseMask;
typedef struct { float x, y, w, h; } CourseWater;
typedef struct {
    CourseString type, name, layer;
    float x, y, sx, sy;
    unsigned frame_start, frame_end; /* keyframe range [start,end) in its layer symbol */
    unsigned param_count;
    const unsigned char *params, *params_end;
} CourseObject;
typedef struct {
    CourseString id;
    unsigned mask_count, water_count, object_count;
    CourseMask masks[COURSE_MAX_MASKS];
    CourseWater water[COURSE_MAX_WATER];
    CourseObject objects[COURSE_MAX_OBJECTS];
} CourseArea;
typedef struct { unsigned area_count; CourseArea areas[COURSE_MAX_AREAS]; } CoursePack;

bool course_pack_parse(const unsigned char *data, size_t size, CoursePack *pack);
const CourseArea *course_pack_area(const CoursePack *pack, const char *id);
bool course_string_equals(CourseString s, const char *text);
/* Mask for a layer/frame, or NULL (a missing layer is empty). */
const CourseMask *course_area_mask(const CourseArea *area, CourseLayer layer, unsigned frame);
/* Flash hitTest(x, y, true) against a mask, in course pixels. */
bool course_mask_hit(const CourseMask *mask, float x, float y);
bool course_area_hit(const CourseArea *area, CourseLayer layer, unsigned frame, float x, float y);
bool course_area_water(const CourseArea *area, float x, float y, float *surface);
/* Parameter value of an object's onClipEvent(load) assignment, or false. */
bool course_object_param(const CourseObject *object, const char *key, CourseString *value);
#ifdef __cplusplus
}
#endif
#endif
