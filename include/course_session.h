#ifndef SM63_COURSE_SESSION_H
#define SM63_COURSE_SESSION_H
/* Playing the original tutorial (course 8-E1): area changes, signs, hints,
   the scripted opening and the completion boundary. Sources and rules are in
   docs/original-tutorial-reference.md and docs/course-session.md. */
#include "course_world.h"
#include "bg_water.h"
#include "course_objects.h"
#include "hud.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum { SESSION_PLAY, SESSION_OPENING, SESSION_SIGN, SESSION_TALK, SESSION_WARP, SESSION_COMPLETE } SessionState;
enum { COURSE_FX_BUBBLE, COURSE_FX_SPLASH };   /* CourseSession fx kinds */
/* AreaTextClip's last frame (9 waiting + 7514 frames 2-94); 95 removes it. */
#define COURSE_AREA_TEXT_LAST 102u
/* 7514's frame (1-based) for area_text t (>= 1). */
static inline unsigned course_area_text_frame(unsigned t) { return t <= 9 ? 1u : t - 8u; }
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
    /* AreaTextClip (the area name): 0 none, else source frames since attached
       (frame 1 is held 9 frames, then 7514 frames 2-94: area_text_layout.h). */
    CourseString area_title; unsigned area_text;
    /* opening (8-E1-1 Start sign) and warps */
    unsigned opening_wait, opening_count;
    int warp_target;
    /* Area change wipes (frame + 1, 0 = none): wipe_in before changecourse,
       wipe_out (the "transition2") in the new area. */
    unsigned char wipe; unsigned wipe_in, wipe_out;
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
    /* The 0 HP faint (frame + 1 of each clip, 0 = not playing): MarioDeathGFX
       (1702, at death_x / death_y, Mario hidden), the Blackout star wipe
       (7433), the LifeLost screen (7473, the course removed) and the StarIn
       wipe (7434) after ReviveMario. fx_phase: their own 32 Hz clock. */
    unsigned death, blackout, lifelost, starin, fx_phase, last_hurt_frame;
    /* Dynamic Water (802) in this area's backgrounds: bg_waters indices and their waves. */
    unsigned bg_water_count, bg_water_index[4];
    BgWave bg_wave[4];
    /* The level water (Water Code strips, water_places.h): water_places indices,
       their waves, and the water blocks' frame (133-frame loop, from area entry). */
    unsigned water_count, water_index[6], water_frame;
    /* CharCode underwatercount / abovewatercount (WaterHP), and the effects
       attached in FrontGFX: WaterBubble (rises 3 px a frame) and Med-Splash. */
    unsigned under_count, above_count;
    struct { int kind; float x, y, dy; unsigned frame, frames; } fx[12];   /* kind: COURSE_FX_* ; course px */
    unsigned fx_count;
    BgWave water_wave[6];
    float death_x, death_y;
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
enum { WIPE_RIGHT, WIPE_LEFT, WIPE_DOWN };
/* Frame (1-based) of "<dir>transition" that sets _root.warpframe. */
static inline unsigned wipe_warp_frame(unsigned wipe) { return wipe == WIPE_DOWN ? 15u : 16u; }
/* Frames shown of "<dir>transition2" (its last frame removes it). */
static inline unsigned wipe_out_frames(unsigned wipe) { return wipe == WIPE_RIGHT ? 17u : 16u; }
#ifdef __cplusplus
}
#endif
#endif
