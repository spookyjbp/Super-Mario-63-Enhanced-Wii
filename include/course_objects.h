#ifndef SM63_COURSE_OBJECTS_H
#define SM63_COURSE_OBJECTS_H
/* Live original course objects (coins, signs, Toad, birds, butterflies,
   falling log, enemies...). Behaviour follows each object's clip script; see
   docs/course-objects.md. Runs at the original 32 Hz on source frames. */
#include "course_world.h"
#ifdef __cplusplus
extern "C" {
#endif
#define COURSE_MAX_LIVE 192
enum { CHEEP_SWIM, CHEEP_CHASE, CHEEP_TURN, CHEEP_JUMP, CHEEP_SPIN, CHEEP_GONE };
enum { GOOMBA_IDLE, GOOMBA_JUMP, GOOMBA_WALKR, GOOMBA_WALKL, GOOMBA_SQUISH, GOOMBA_TUMBLE };
typedef enum {
    LIVE_NONE, LIVE_COIN, LIVE_BLUE_COIN, LIVE_SIGN, LIVE_BUTTERFLY, LIVE_GOONIE, LIVE_TOAD, LIVE_LOG,
    LIVE_ARROW, LIVE_GOOMBA, LIVE_CHEEP, LIVE_BOMB, LIVE_KOOPA, LIVE_BOX, LIVE_FLUDD_BOX, LIVE_BOTTLE,
    LIVE_GREEN_PLAT, LIVE_TIP_LOG, LIVE_ROT_BLOCK, LIVE_KIND_COUNT
} LiveKind;
typedef struct {
    LiveKind kind;
    const CourseObject *src;
    unsigned index;            /* index of src in the area (coin persistence) */
    float x, y, vx, vy;        /* course px, px per 32 Hz frame */
    float home_x, home_y, alpha, wave, scale;
    unsigned frame;            /* clip frame counter (32 Hz) */
    int state, timer, count, dir;
    bool alive, collected, facing_left, carrying;
    float p0, p1, p2, p3;      /* per-kind parameters */
    bool air, tumble, hurt, seeing, see, jump, powerhit, bounce, spawned;
    float tumblerot, rotation, spinrot;
    bool lastwater;
    int frame_label;          /* enemy clip label (see course_objects.c) */
} LiveObject;
typedef struct CourseObjects {
    LiveObject live[COURSE_MAX_LIVE];
    unsigned count;
    unsigned seed;
} CourseObjects;
/* Shared player progress (_root.Coins, CharHP, hpcoincount). */
typedef struct { unsigned coins, coincount, hpcoins; int hp; } CourseProgress;
/* _root.MarioHit(a, b, c, d, e): lose a HP, blink b frames, knock (d, c), stun e. */
void course_mario_hit(Player *p, CourseProgress *g, int hp, int blink, float vy, float vx, int stun);
/* Persistent per-area pickups (_root["coindata"+course]). */
#define COURSE_MAX_PICKUPS 192
typedef struct { const CourseArea *area; uint8_t taken[COURSE_MAX_PICKUPS]; } CoursePickups;

void course_objects_init(CourseObjects *o, const CourseArea *area, const CoursePickups *taken, unsigned back_frame,
                         unsigned seed);
/* One 60 Hz tick after the player has moved; objects advance on source frames. */
/* cam_x/cam_y: camera centre (course px) for the original 600 px activity range. */
void course_objects_step(CourseObjects *o, CourseRuntime *c, Player *p, CourseProgress *progress,
                         CoursePickups *taken, bool frozen, float cam_x, float cam_y, bool jump_held);
/* Moving one-way platforms (falling log, birds) for collision: feet point test. */
bool course_objects_platform(const CourseObjects *o, float x, float y);
/* Sign / Toad within reach of Mario's feet (for prompts and Z). */
const LiveObject *course_objects_near(const CourseObjects *o, LiveKind kind, float fx, float fy);
#ifdef __cplusplus
}
#endif
#endif
