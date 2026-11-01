#ifndef SM63_COURSE_WORLD_H
#define SM63_COURSE_WORLD_H
/* Play-time view of one original course area. World pixels are course pixels
   times COURSE_SCALE (the scale the port's movement constants assume), offset so
   the area's camera edge starts at world (0,0). */
#include "course_pack.h"
#include "game.h"
#ifdef __cplusplus
extern "C" {
#endif
#define COURSE_SCALE 1.375f
/* Original stage: 450x300 course pixels at CameraZoom 100 (DOMDocument.xml:11815). */
#define COURSE_VIEW_HEIGHT 300.0f
struct CourseObjects;
typedef struct CourseRuntime {
    const CourseArea *area;
    const struct CourseObjects *objects; /* moving one-way platforms, or NULL */
    unsigned platform_frame; /* Course.Platforms._currentframe - 1 */
    float ox, oy;            /* course pixel at world (0,0) */
} CourseRuntime;
void course_runtime_init(CourseRuntime *course, const CourseArea *area);
/* World rectangle and spawn for StartChar(area, b, c, ...) style Char placement. */
World course_world(CourseRuntime *course);
static inline float course_cx(const CourseRuntime *c, float world_x) { return world_x / COURSE_SCALE + c->ox; }
static inline float course_cy(const CourseRuntime *c, float world_y) { return world_y / COURSE_SCALE + c->oy; }
static inline float course_wx(const CourseRuntime *c, float course_x) { return (course_x - c->ox) * COURSE_SCALE; }
static inline float course_wy(const CourseRuntime *c, float course_y) { return (course_y - c->oy) * COURSE_SCALE; }
bool course_solid(const CourseRuntime *c, float cx, float cy);   /* Platforms */
bool course_bsolid(const CourseRuntime *c, float cx, float cy);  /* BPlatforms (one-way) */
/* Place the player's feet at course pixel (cx, cy). */
void course_place_player(const CourseRuntime *c, Player *p, float cx, float cy);
void course_collision_step(Player *p, const World *w);
bool course_stand_clear(const Player *p, const World *w);
void course_camera_init(Camera *camera, const Player *p, const World *w, float view_width);
void course_camera_step(Camera *camera, const Player *p, const World *w, float view_width);
/* Screen pixels per world pixel so 300 course pixels fill the 480-line screen. */
static inline float course_view_zoom(void) { return 480.0f / (COURSE_VIEW_HEIGHT * COURSE_SCALE); }
#ifdef __cplusplus
}
#endif
#endif
