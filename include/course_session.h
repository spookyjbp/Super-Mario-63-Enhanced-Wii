#ifndef SM63_COURSE_SESSION_H
#define SM63_COURSE_SESSION_H
/* Playing the original tutorial (course 8-E1): area changes, signs, hints,
   the scripted opening and the completion boundary. Sources and rules are in
   docs/original-tutorial-reference.md and docs/course-session.md. */
#include "course_world.h"
#include "course_objects.h"
#include "hud.h"
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
    /* Coded Sign timeline: 0 = frames 4-9 (walk to the sign), 1 = frame 10
       (message shown), 2 = frames 11-15 (closing); sign_frames counts each. */
    unsigned sign_phase, sign_frames;
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
    HudState hud;                /* Stats Clip POWER meter */
    CoursePickups pickups[COURSE_MAX_AREAS];
    /* Toad dialogue: Phase (1-based page), text of the current page */
    const LiveObject *toad; unsigned toad_page; CourseString toad_text; CourseString toad_mood;
    unsigned toad_chars;         /* 'tm' clip: characters typed so far (2 a frame) */
    /* Stats.tm (186): the cloud's _alpha (eases a quarter of the way to 100 while
       _root.ToadMessage is set, to 0 when it is ""), its prompt bz._alpha (half
       way to 100 once the page is typed), and whether ToadMessage is set. The
       last page's text stays in toad_text while the cloud fades out. */
    float tm_alpha, tm_bz;
    bool tm_message;
    /* _root.CameraZoom (camera_zoom: +/- 2 a frame from the zoom input, Toad's
       talk +3 to 150 and back to 100 after it, clamped to Maxzoomout 50 ..
       Maxzoomin 200) and CameraZoom2 (zoom2: eased a twentieth of the rounded
       difference a frame). zoom: CameraZoom2 / 100 between source frames (the
       view scale on top of the course scale; zoom2_from is the last frame's). */
    float zoom, camera_zoom, zoom2, zoom2_from;
    bool talk_zoomed;            /* Toad Main Clip's camerazoom flag */
    unsigned char controller;    /* GameInput.controller, for the controller picture signs */
    /* Nozzle switch banner (port HUD): ticks left, and the FLUDD state it shows
       (0 = none, 1 + Nozzle). The original only hides the tank without FLUDD. */
    unsigned nozzle_banner; unsigned char banner_fludd, last_fludd;
} CourseSession;
bool course_session_start(CourseSession *s, const CoursePack *pack, const char *area, float cx, float cy, float view_width);
void course_session_step(CourseSession *s, GameInput input);
/* Object is on the current BackGFX/FrontGFX frame (and not a removed one). */
bool course_session_object_active(const CourseSession *s, const CourseObject *o);
/* _root.Maxzoomout / Maxzoomin (the defaults; no 8-E1 area overrides them). */
#define COURSE_ZOOM_MIN 0.5f
#define COURSE_ZOOM_MAX 2.0f
/* The view zoom (1 = the original framing): coursescale / 100. The camera keeps
   the zoomed view inside CamEdge like the original (CamEdge is tested at the
   screen's edges, in the scaled course). */
static inline float course_session_zoom(const CourseSession *s) { return s->zoom; }
/* One tick of the zoom control (input axis: -1 in .. 1 out) and easing; the
   original's steps run on source frames, `frac` is the progress to the next. */
void course_session_zoom_step(CourseSession *s, float axis, bool talking, bool source_step, float frac);
/* A text sign's message with keyboard keys replaced by the controller's buttons. */
CourseString course_sign_message(CourseString text, unsigned controller);
/* The original message box (Stats.Message 253) frame for a text sign: 0 none,
   1-6 opening (PlayMessage: gotoAndPlay(1), stop at 6, text shown on 6),
   7-11 closing (StopMessage: gotoAndPlay(7)). One frame per source frame. */
static inline unsigned course_message_frame(const CourseSession *s)
{
    if (s->state != SESSION_SIGN || s->sign_picture || s->sign_phase == 0) return 0;
    if (s->sign_phase == 1) return s->sign_frames + 1 < 6 ? s->sign_frames + 1 : 6;
    return s->sign_frames + 7 < 11 ? s->sign_frames + 7 : 11;
}
/* Frames the "Press .. to Continue" prompt (250) has played, or -1: it starts
   when the message box reaches frame 6, or with the picture page. */
static inline int course_prompt_frame(const CourseSession *s)
{
    if (s->state != SESSION_SIGN || s->sign_phase != 1) return -1;
    if (s->sign_picture) return (int)s->sign_frames;
    return s->sign_frames >= 5 ? (int)s->sign_frames - 5 : -1;
}
#define COURSE_WARP_SECONDS 0.5f
#ifdef __cplusplus
}
#endif
#endif
