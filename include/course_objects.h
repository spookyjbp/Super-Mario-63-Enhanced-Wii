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
/* Toad 'head' (2498) moods: the label frames (0-based) of each segment. A mood
   loops from its label to the frame before the next label (ChangeFrame). */
enum { TOAD_SMILE, TOAD_TALK, TOAD_SAD, TOAD_MAD, TOAD_SHOCKED, TOAD_HAPPY, TOAD_CONFUSED, TOAD_CRAZY, TOAD_MOODS };
/* Mood named by a Toad's Mood / M<n>Moo parameter (Smile when unknown). */
int course_toad_mood(CourseString name);
/* One source frame of the head clip: next frame, the clip's Skip(n) jumps,
   back to the mood's label at the segment end. `rand` returns 0..n-1. */
unsigned course_toad_head_step(unsigned frame, int mood, unsigned (*rand)(void *ctx, unsigned n), void *ctx);
static inline unsigned course_toad_mood_label(int mood)
{
    static const unsigned label[TOAD_MOODS] = {0, 68, 98, 159, 184, 269, 298, 359};
    return label[mood >= 0 && mood < TOAD_MOODS ? mood : 0];
}
enum { GOOMBA_IDLE, GOOMBA_JUMP, GOOMBA_WALKR, GOOMBA_WALKL, GOOMBA_SQUISH, GOOMBA_TUMBLE };
/* Bob-omb `frame` variable. Its clip (3310) has no "Idle" label, so Idle keeps
   the frame shown last (LiveObject.state holds the shown label). */
enum { BOMB_IDLE, BOMB_WALKL, BOMB_WALKR, BOMB_TUMBLE, BOMB_EXPLODE, BOMB_EXPLODE2 };
/* Koopa `State` (l->state) and clip 3402 label (l->frame_label). */
enum { KOOPA_FLYING_STATE, KOOPA_WALK_STATE, KOOPA_SHELL_STATE };
enum { KOOPA_WALK, KOOPA_TURN, KOOPA_FLYING, KOOPA_SHELL };
typedef enum {
    LIVE_NONE, LIVE_COIN, LIVE_BLUE_COIN, LIVE_SIGN, LIVE_BUTTERFLY, LIVE_GOONIE, LIVE_TOAD, LIVE_LOG,
    LIVE_ARROW, LIVE_GOOMBA, LIVE_CHEEP, LIVE_BOMB, LIVE_KOOPA, LIVE_BOX, LIVE_FLUDD_BOX, LIVE_BOTTLE,
    LIVE_GREEN_PLAT, LIVE_TIP_LOG, LIVE_ROT_BLOCK, LIVE_CAMBOX, LIVE_NOZZLE, LIVE_KIND_COUNT
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
    bool lastwater, exploding;
    int frame_label;          /* enemy clip label (see course_objects.c) */
    unsigned eye;             /* eye clip 2385 frame 0..19 (19 = seeing) */
    int deadcount, timer2;    /* Bob-omb deadcount / explodetimer */
    float q0, q1, q2, q3;      /* more per-kind state (platform offsets, speeds) */
} LiveObject;
/* _root.Course.Enemyhurt children (EnemyHurtBox, SWF 63): invisible boxes that
   knock enemies; removed when count reaches time. */
#define COURSE_MAX_HURT 16
typedef struct { float x0, y0, x1, y1; int count, time; } EnemyHurtBox;
typedef struct CourseObjects {
    LiveObject live[COURSE_MAX_LIVE];
    unsigned count;
    unsigned seed;
    EnemyHurtBox hurt[COURSE_MAX_HURT];
    unsigned hurt_count;
    /* Camera Control Box: _root.CamXchangeVar / CamYchangeVar while any box exists. */
    bool camboxes;
    float cam_x_var, cam_y_var;
} CourseObjects;
/* Shared player progress (_root.Coins, CharHP, hpcoincount). */
typedef struct { unsigned coins, coincount, hpcoins; int hp; } CourseProgress;
/* _root.MarioHit(a, b, c, d, e, f, g): lose a HP, blink b frames, stun e. With
   a > 0 the speed halves and (d, c) replaces it ("hurt1"), or is added when g
   (add) is true; with a == 0, (d, c) is added and the attack ends (a shove). */
void course_mario_hit(Player *p, CourseProgress *g, int hp, int blink, float vy, float vx, int stun, bool add);
/* Persistent per-area pickups (_root["coindata"+course]). */
#define COURSE_MAX_PICKUPS 192
typedef struct { const CourseArea *area; uint8_t taken[COURSE_MAX_PICKUPS]; } CoursePickups;

void course_objects_init(CourseObjects *o, const CourseArea *area, const CoursePickups *taken, unsigned back_frame,
                         unsigned seed);
/* One 60 Hz tick after the player has moved; objects advance on source frames. */
/* cam_x/cam_y: camera centre (course px) for the original 600 px activity range. */
void course_objects_step(CourseObjects *o, CourseRuntime *c, Player *p, CourseProgress *progress,
                         CoursePickups *taken, bool frozen, float cam_x, float cam_y, bool jump_held);
/* Moving one-way platforms (falling log, birds, green circle platforms,
   tipping logs) in BPlatforms: point test. */
bool course_objects_platform(const CourseObjects *o, float x, float y);
/* Solid object platforms in Platforms (breakable boxes, rotating blocks). */
bool course_objects_solid(const CourseObjects *o, float x, float y);
/* Sign / Toad within reach of Mario's feet (for prompts and Z). */
const LiveObject *course_objects_near(const CourseObjects *o, LiveKind kind, float fx, float fy);
#ifdef __cplusplus
}
#endif
#endif
