#ifndef SM63_ACHIEVEMENTS_H
#define SM63_ACHIEVEMENTS_H
/* The 64 achievements (docs/achievements.md). Pure and host-tested: the game
   reports events (a Shine Sprite, a Star Coin, a Bowser key, a story
   milestone); only the achievements that event can affect are checked, never
   every frame. Progress is kept as the original game keeps it, per save file:
   _root.Star[1..64], _root.StarCoin[1..64], _root.BowserKey1..3. */
#include <stdbool.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#define ACH_COUNT 64
#define ACH_SHINES 64
#define ACH_STAR_COINS 64
#define ACH_QUEUE 16

typedef enum { ACH_BRONZE, ACH_SILVER, ACH_GOLD, ACH_PLATINUM, ACH_TIERS } AchTier;

/* How an achievement is earned (what event can unlock it). */
typedef enum {
    ACH_SHINE_ALL,     /* every shine in `mask` */
    ACH_SHINE_ANY,     /* one shine in `mask` */
    ACH_SHINE_SOME,    /* `count` shines in `mask` */
    ACH_SHINE_TOTAL,   /* `count` shines in all */
    ACH_COIN_TOTAL,    /* `count` Star Coins in all */
    ACH_KEY,           /* Bowser key `count` (1..3) */
    ACH_MILESTONE,     /* story milestone `count` (AchMilestone) */
    ACH_COMPLETE,      /* all shines, all Star Coins and all three keys */
    ACH_BOTH_64,       /* all 64 unique shines and all 64 unique Star Coins */
    ACH_ALL_OTHERS,    /* the Platinum: achievements 1..63 */
    ACH_PENDING        /* no matching event in the original game yet: never unlocks */
} AchKind;

/* Story events the original has that are not a shine or key. */
typedef enum { ACH_MS_ENTER_BOWSER_TRAP2 = 1, ACH_MS_ENDING = 2 } AchMilestone;

typedef struct {
    unsigned char id;          /* stable 1..64: never renumber */
    unsigned char tier;        /* AchTier */
    unsigned char kind;        /* AchKind */
    unsigned char count;
    uint64_t mask;             /* shines: bit (n - 1) is _root.Star[n] */
    const char *name, *description;
    const char *pending;       /* why it cannot unlock yet (ACH_PENDING), else NULL */
} AchDef;

extern const AchDef ach_defs[ACH_COUNT];

/* One save file's progress and unlocks. */
typedef struct {
    uint64_t shines, star_coins;   /* bit n-1: Star[n] / StarCoin[n] */
    unsigned char keys;            /* bit k-1: BowserKey k */
    uint32_t milestones;           /* bit m: AchMilestone m reached */
    uint64_t unlocked;             /* bit id-1 */
} AchProgress;

typedef struct {
    AchProgress p;
    unsigned char queue[ACH_QUEUE];   /* ids waiting to be shown, in unlock order */
    unsigned head, count;
    bool dirty;                       /* unlocks or progress not yet saved */
} Achievements;

void ach_init(Achievements *a, const AchProgress *loaded);
/* Events. Each returns how many achievements it unlocked (queued). */
unsigned ach_shine(Achievements *a, unsigned star);        /* _root.Star[star] = true, 1..64 */
unsigned ach_star_coin(Achievements *a, unsigned coin);    /* _root.StarCoin[coin] = true, 1..64 */
unsigned ach_key(Achievements *a, unsigned key);           /* _root.BowserKeyN = true, 1..3 */
unsigned ach_milestone(Achievements *a, AchMilestone m);
/* After loading a save: award whatever its progress already proves (no events
   are invented: only conditions the stored data shows). */
unsigned ach_evaluate_all(Achievements *a);
/* Developer test: unlock one achievement directly (and the Platinum if due). */
unsigned ach_force(Achievements *a, unsigned id);

bool ach_unlocked(const Achievements *a, unsigned id);
/* The next unlock to announce (0 = none); pop removes it. */
unsigned ach_peek(const Achievements *a);
void ach_pop(Achievements *a);

typedef struct { unsigned total, per_tier[ACH_TIERS], percent; } AchSummary;
AchSummary ach_summary(const Achievements *a);
const char *ach_tier_name(unsigned tier);
unsigned ach_popcount(uint64_t v);

#ifdef __cplusplus
}
#endif
#endif
