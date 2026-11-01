#ifndef SM63_COURSE_SESSION_H
#define SM63_COURSE_SESSION_H
/* Playing the original tutorial (course 8-E1): area changes, signs, hints,
   the scripted opening and the completion boundary. Sources and rules are in
   docs/original-tutorial-reference.md and docs/course-session.md. */
#include "course_world.h"
#include "course_objects.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum { SESSION_PLAY, SESSION_OPENING, SESSION_SIGN, SESSION_TALK, SESSION_WARP, SESSION_COMPLETE } SessionState;
typedef struct {
    const CoursePack *pack;
    CourseRuntime runtime;
    World world;
    Game game;
    SessionState state;
    unsigned back_frame;        /* BackGFX resting frame (index 2: `stop()`) */
    unsigned total_stars;       /* _root.TotalStars (0 on a new save) */
    /* sign reading */
    const CourseObject *sign;
    bool sign_picture;
    CourseString sign_text;
    float camera_target_x, camera_target_y; bool camera_override;
    /* hints and titles */
    CourseString hint;
    CourseString area_title; float area_title_time;
    /* opening (8-E1-1 Start sign) and warps */
    unsigned opening_wait, opening_count;
    int warp_target; float warp_time;
    const char *warp_area; float warp_x, warp_y;
    float view_width;
    CourseObjects objects;
    CourseProgress progress;
    CoursePickups pickups[COURSE_MAX_AREAS];
    /* Toad dialogue: Phase (1-based page), text of the current page */
    const LiveObject *toad; unsigned toad_page; CourseString toad_text; CourseString toad_mood;
} CourseSession;
bool course_session_start(CourseSession *s, const CoursePack *pack, const char *area, float cx, float cy, float view_width);
void course_session_step(CourseSession *s, GameInput input);
/* Object is on the current BackGFX/FrontGFX frame (and not a removed one). */
bool course_session_object_active(const CourseSession *s, const CourseObject *o);
#define COURSE_WARP_SECONDS 0.5f
#ifdef __cplusplus
}
#endif
#endif
