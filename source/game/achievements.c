#include "achievements.h"
#include <string.h>

/* Shine Sprite numbers are the original's _root.Star[] indices
   (StarNameArray in frame 2 of the 2010 SWF): course n (Playinglevel 1..7)
   owns Star[5n-4 .. 5n]; Sinking Sand Land, Wet Dry World and Tick Tock Clock
   (Playinglevelnum 1..3) own Star[3k+52 .. 3k+54]; the rest are single shines
   (Bowser stages, tutorials, prizes, secret courses). */
#define S(n) (1ULL << ((n) - 1))
#define RANGE(a, b) ((((b) >= 64 ? ~0ULL : (1ULL << (b)) - 1)) & ~((1ULL << ((a) - 1)) - 1))

#define BOB RANGE(1, 5)        /* Bob-omb Battle Field */
#define SL RANGE(6, 10)        /* Snowmans Land */
#define HMC RANGE(11, 15)      /* Hazy Maze Cave */
#define BOO RANGE(16, 20)      /* Boo's Mansion */
#define LLL RANGE(21, 25)      /* Lethal Lava Land */
#define TTM RANGE(26, 30)      /* Tall Tall Mountain */
#define RR RANGE(31, 35)       /* Raindbow Ride (sic) */
#define SSL RANGE(55, 57)      /* Sinking Sand Land */
#define WDW RANGE(58, 60)      /* Wet Dry World */
#define TTC RANGE(61, 63)      /* Tick Tock Clock */
/* missions about 8 Red Coins (4 Collect the 8 Red Coins, 9 Red-Coins of the
   Summit and Skies, 14 Excavating the 8 Red Coins, 19 Hunting for Red-Coins,
   24 Red-Hot Red-Coins, 29 The Red Coins of the Mountain, 34 The Maze of Red
   Coins, 37 Bowser's Castle Red-Coins, 56 Trekking for 8 Red-Coins, 59 Red
   Coins in the Underwater City, 62 The Flippin' Red Coins) */
#define RED_COINS (S(4) | S(9) | S(14) | S(19) | S(24) | S(29) | S(34) | S(37) | S(56) | S(59) | S(62))
/* missions about Silver Stars (StarDescArray 3, 23, 27, 54, 60 name them) */
#define SILVER_STARS (S(3) | S(23) | S(27) | S(54) | S(60))
/* the castle's secret courses, shown as "Secret Course" (Playinglevel 0):
   44 The Secret of the Mushroom .. 54 A Galaxy of Stars */
#define SECRET_COURSES RANGE(44, 54)

#define DEF(i, t, k, c, m, n, d) {i, t, k, c, m, n, d, NULL}
#define PENDING(i, t, n, d, why) {i, t, ACH_PENDING, 0, 0, n, d, why}
enum { B = ACH_BRONZE, SV = ACH_SILVER, G = ACH_GOLD, P = ACH_PLATINUM };

const AchDef ach_defs[ACH_COUNT] = {
    DEF(1, B, ACH_SHINE_ALL, 0, S(39), "A New Adventure", "Complete the Mushroom Kingdom's first tutorial."),
    PENDING(2, B, "Let There Be Light", "Restore the light inside Peach's Castle.",
            "the original has no lights-restored event or shine for this"),
    DEF(3, B, ACH_SHINE_ALL, 0, S(1), "King of the Mountain", "Defeat King Bob-omb."),
    DEF(4, B, ACH_SHINE_ALL, 0, S(2), "Head in the Clouds", "Collect the Island in the Sky Shine Sprite."),
    DEF(5, B, ACH_SHINE_ALL, 0, S(3), "Five Little Stars", "Collect the five Silver Stars in Bob-omb Battlefield."),
    DEF(6, B, ACH_SHINE_ANY, 0, SL, "Winter Wonderland", "Collect your first Shine Sprite in Snowman's Land."),
    DEF(7, B, ACH_SHINE_ALL, 0, S(8), "Ice Cold Victory", "Defeat Chief Chilly."),
    DEF(8, B, ACH_SHINE_ALL, 0, S(55), "Ancient Treasure", "Collect the Shine Sprite inside the Shifting Sand Land pyramid."),
    DEF(9, B, ACH_SHINE_ALL, 0, S(40), "The First Showdown", "Defeat Bowser in the Dark World."),
    DEF(10, B, ACH_KEY, 1, 0, "Key to the Kingdom", "Obtain Bowser's first key."),
    DEF(11, B, ACH_SHINE_ALL, 0, S(41), "Full Speed Ahead", "Complete the second tutorial and unlock the remaining F.L.U.D.D. nozzles."),
    DEF(12, B, ACH_SHINE_ANY, 0, HMC, "Into the Depths", "Collect your first Shine Sprite in Hazy Maze Cave."),
    DEF(13, B, ACH_SHINE_ALL, 0, S(12), "Toxic Trouble", "Complete the Deadly Toxic Maze mission."),
    DEF(14, B, ACH_SHINE_ALL, 0, S(50), "Garden Guardian", "Complete the Peach's Garden storyline challenge."),
    DEF(15, B, ACH_SHINE_ANY, 0, BOO, "Ghost Hunter", "Collect your first Shine Sprite in Boo's Mansion."),
    DEF(16, B, ACH_SHINE_ALL, 0, S(16), "Boo Who?", "Complete the Big Boo's Balcony mission."),
    DEF(17, B, ACH_SHINE_ANY, 0, WDW, "Underwater Explorer", "Collect your first Shine Sprite in Wet-Dry World."),
    DEF(18, B, ACH_MILESTONE, ACH_MS_ENTER_BOWSER_TRAP2, 0, "Fortress Assault", "Reach Bowser's second showdown."),
    DEF(19, B, ACH_SHINE_ALL, 0, S(42), "Bowser Strikes Back", "Defeat Bowser in his second trap."),
    DEF(20, B, ACH_KEY, 2, 0, "Second Key Secured", "Obtain Bowser's second key."),
    DEF(21, B, ACH_SHINE_ANY, 0, LLL, "Hot Stuff", "Collect your first Shine Sprite in Lethal Lava Land."),
    DEF(22, B, ACH_SHINE_ALL, 0, S(22), "Volcano Explorer", "Complete the Into the Volcano mission."),
    DEF(23, B, ACH_SHINE_ALL, 0, S(51), "Magical Showdown", "Complete the Kamek's Castle storyline challenge."),
    DEF(24, B, ACH_SHINE_ALL, 0, S(26), "Mountain Climber", "Complete Scale the Mountain in Tall Tall Mountain."),
    DEF(25, B, ACH_SHINE_ALL, 0, S(27), "High Altitude", "Collect the Silver Stars in Tall Tall Mountain."),
    DEF(26, B, ACH_SHINE_ANY, 0, TTC, "Against the Clock", "Collect your first Shine Sprite in Tick Tock Clock."),
    DEF(27, B, ACH_SHINE_ANY, 0, RR, "Rainbow Traveler", "Collect your first Shine Sprite in Rainbow Ride."),
    DEF(28, B, ACH_SHINE_ALL, 0, S(44), "Mushroom Hunter", "Complete the Secret of the Mushroom course."),
    DEF(29, B, ACH_SHINE_ALL, 0, S(46), "Deep Sea Diver", "Complete the Jolly Roger Bay secret course."),
    DEF(30, B, ACH_SHINE_TOTAL, 10, 0, "First Steps to Greatness", "Collect 10 Shine Sprites."),

    DEF(31, SV, ACH_SHINE_ALL, 0, BOB, "Battlefield Champion", "Collect every Shine Sprite in Bob-omb Battlefield."),
    DEF(32, SV, ACH_SHINE_ALL, 0, SL, "Master of the Snow", "Collect every Shine Sprite in Snowman's Land."),
    DEF(33, SV, ACH_SHINE_ALL, 0, HMC, "Cave Conqueror", "Collect every Shine Sprite in Hazy Maze Cave."),
    DEF(34, SV, ACH_SHINE_ALL, 0, BOO, "Ghostbuster", "Collect every Shine Sprite in Boo's Mansion."),
    DEF(35, SV, ACH_SHINE_ALL, 0, LLL, "Lava Survivor", "Collect every Shine Sprite in Lethal Lava Land."),
    DEF(36, SV, ACH_SHINE_ALL, 0, TTM, "Summit Master", "Collect every Shine Sprite in Tall Tall Mountain."),
    DEF(37, SV, ACH_SHINE_ALL, 0, RR, "Over the Rainbow", "Collect every Shine Sprite in Rainbow Ride."),
    DEF(38, SV, ACH_SHINE_ALL, 0, SSL, "Desert Explorer", "Collect every Shine Sprite in Shifting Sand Land."),
    DEF(39, SV, ACH_SHINE_ALL, 0, WDW, "Water World", "Collect every Shine Sprite in Wet-Dry World."),
    DEF(40, SV, ACH_SHINE_ALL, 0, TTC, "Clockwork Precision", "Collect every Shine Sprite in Tick Tock Clock."),
    DEF(41, SV, ACH_SHINE_SOME, 5, RED_COINS, "Red Coin Collector", "Complete five missions centered on collecting eight Red Coins."),
    DEF(42, SV, ACH_SHINE_SOME, 3, SILVER_STARS, "Silver Star Seeker", "Complete three missions centered on collecting Silver Stars."),
    DEF(43, SV, ACH_SHINE_ALL, 0, S(48), "Secret Passage", "Complete the Secret of the Sky course."),
    DEF(44, SV, ACH_SHINE_ALL, 0, S(47), "Frozen Speed", "Complete the Frosty F.L.U.D.D. course."),
    DEF(45, SV, ACH_SHINE_ALL, 0, S(49), "Castle Crusher", "Complete Thwomp's Castle."),
    DEF(46, SV, ACH_SHINE_ALL, 0, S(52), "Puzzle Solver", "Complete the Cave of Empuzzlement."),
    DEF(47, SV, ACH_SHINE_ALL, 0, S(53), "Fire and Water", "Complete the Magma Maze."),
    DEF(48, SV, ACH_SHINE_ALL, 0, S(54), "Across the Galaxy", "Complete A Galaxy of Stars."),
    DEF(49, SV, ACH_COIN_TOTAL, 32, 0, "Treasure Hunter", "Collect 32 Star Coins."),
    DEF(50, SV, ACH_SHINE_TOTAL, 32, 0, "Halfway to the Sun", "Collect 32 Shine Sprites."),

    DEF(51, G, ACH_SHINE_ALL, 0, S(36), "The Final Showdown", "Defeat Bowser in his castle."),
    DEF(52, G, ACH_KEY, 3, 0, "The Third Key", "Obtain Bowser's final key."),
    DEF(53, G, ACH_MILESTONE, ACH_MS_ENDING, 0, "Kingdom Saved", "Complete the main Story Mode ending."),
    PENDING(54, G, "The Secret Edge", "Complete Edge of the Mushroom Kingdom.",
            "no course of that name exists in the original's course or star lists"),
    PENDING(55, G, "Red Coin Royalty", "Collect the Red Coin Shine Sprites from all three Bowser stages.",
            "the original has one Bowser red-coin Shine Sprite (37, Bowser's Castle Red-Coins), not three"),
    DEF(56, G, ACH_SHINE_ALL, 0, SECRET_COURSES, "Castle Secrets Revealed", "Collect every castle secret-course Shine Sprite."),
    DEF(57, G, ACH_SHINE_ALL, 0, S(39) | S(41) | S(47), "Master of F.L.U.D.D.",
        "Complete both F.L.U.D.D. tutorial stages and the Frosty F.L.U.D.D. secret course."),
    DEF(58, G, ACH_SHINE_ALL, 0, S(38), "The 16-Star Coin Prize", "Claim the reward for collecting 16 Star Coins."),
    DEF(59, G, ACH_SHINE_ALL, 0, S(43), "The 48-Star Coin Prize", "Claim the reward for collecting 48 Star Coins."),
    DEF(60, G, ACH_COIN_TOTAL, 64, 0, "Star Coin Legend", "Collect all 64 Star Coins."),
    DEF(61, G, ACH_SHINE_TOTAL, 64, 0, "Shine Sprite Legend", "Collect all 64 Shine Sprites."),
    DEF(62, G, ACH_COMPLETE, 0, 0, "The Ultimate Adventure",
        "Reach 100% Story Mode completion, including all Shine Sprites, Star Coins, and Bowser keys."),
    DEF(63, G, ACH_BOTH_64, 0, 0, "The Power of 64", "Collect all 64 Shine Sprites and all 64 Star Coins on the same save file."),

    DEF(64, P, ACH_ALL_OTHERS, 0, 0, "Super Mario 63", "Unlock all 63 other achievements."),
};

unsigned ach_popcount(uint64_t v)
{
    unsigned n = 0;
    for (; v; v &= v - 1) ++n;
    return n;
}
static bool met(const AchProgress *p, const AchDef *d)
{
    switch ((AchKind)d->kind) {
    case ACH_SHINE_ALL: return (p->shines & d->mask) == d->mask;
    case ACH_SHINE_ANY: return (p->shines & d->mask) != 0;
    case ACH_SHINE_SOME: return ach_popcount(p->shines & d->mask) >= d->count;
    case ACH_SHINE_TOTAL: return ach_popcount(p->shines) >= d->count;
    case ACH_COIN_TOTAL: return ach_popcount(p->star_coins) >= d->count;
    case ACH_KEY: return (p->keys >> (d->count - 1)) & 1;
    case ACH_MILESTONE: return (p->milestones >> d->count) & 1;
    case ACH_COMPLETE: return p->shines == ~0ULL && p->star_coins == ~0ULL && (p->keys & 7) == 7;
    /* bitsets of unique collectibles: collecting one twice cannot count twice */
    case ACH_BOTH_64: return p->shines == ~0ULL && p->star_coins == ~0ULL;
    case ACH_ALL_OTHERS: return (p->unlocked & ~(1ULL << 63)) == ~(1ULL << 63);
    case ACH_PENDING: break;
    }
    return false;
}
static void enqueue(Achievements *a, unsigned id)
{
    if (a->count == ACH_QUEUE) { a->head = (a->head + 1) % ACH_QUEUE; --a->count; }  /* drop the oldest notice */
    a->queue[(a->head + a->count++) % ACH_QUEUE] = (unsigned char)id;
}
static unsigned unlock(Achievements *a, unsigned id)
{
    if (id < 1 || id > ACH_COUNT || (a->p.unlocked >> (id - 1) & 1)) return 0;
    a->p.unlocked |= 1ULL << (id - 1);
    a->dirty = true;
    enqueue(a, id);
    return 1;
}
/* Checks only the kinds an event can change, then the Platinum once. */
static unsigned check(Achievements *a, unsigned kinds)
{
    unsigned n = 0;
    for (unsigned i = 0; i < ACH_COUNT - 1; ++i) {
        const AchDef *d = &ach_defs[i];
        if ((kinds >> d->kind & 1) && met(&a->p, d)) n += unlock(a, d->id);
    }
    if (n && met(&a->p, &ach_defs[ACH_COUNT - 1])) n += unlock(a, ACH_COUNT);
    return n;
}
#define K(k) (1u << (k))
#define SHINE_KINDS (K(ACH_SHINE_ALL) | K(ACH_SHINE_ANY) | K(ACH_SHINE_SOME) | K(ACH_SHINE_TOTAL) | K(ACH_COMPLETE) | K(ACH_BOTH_64))

void ach_init(Achievements *a, const AchProgress *loaded)
{
    memset(a, 0, sizeof *a);
    if (loaded) a->p = *loaded;
}
unsigned ach_shine(Achievements *a, unsigned star)
{
    if (star < 1 || star > ACH_SHINES || (a->p.shines >> (star - 1) & 1)) return 0;
    a->p.shines |= 1ULL << (star - 1); a->dirty = true;
    return check(a, SHINE_KINDS);
}
unsigned ach_star_coin(Achievements *a, unsigned coin)
{
    if (coin < 1 || coin > ACH_STAR_COINS || (a->p.star_coins >> (coin - 1) & 1)) return 0;
    a->p.star_coins |= 1ULL << (coin - 1); a->dirty = true;
    return check(a, K(ACH_COIN_TOTAL) | K(ACH_COMPLETE) | K(ACH_BOTH_64));
}
unsigned ach_key(Achievements *a, unsigned key)
{
    if (key < 1 || key > 3 || (a->p.keys >> (key - 1) & 1)) return 0;
    a->p.keys |= (unsigned char)(1u << (key - 1)); a->dirty = true;
    return check(a, K(ACH_KEY) | K(ACH_COMPLETE));
}
unsigned ach_milestone(Achievements *a, AchMilestone m)
{
    if ((unsigned)m >= 32 || (a->p.milestones >> m & 1)) return 0;
    a->p.milestones |= 1u << m; a->dirty = true;
    return check(a, K(ACH_MILESTONE));
}
unsigned ach_evaluate_all(Achievements *a) { return check(a, ~0u & ~K(ACH_ALL_OTHERS)); }
unsigned ach_force(Achievements *a, unsigned id)
{
    unsigned n = unlock(a, id);
    if (n && id != ACH_COUNT && met(&a->p, &ach_defs[ACH_COUNT - 1])) n += unlock(a, ACH_COUNT);
    return n;
}
bool ach_unlocked(const Achievements *a, unsigned id)
{ return id >= 1 && id <= ACH_COUNT && (a->p.unlocked >> (id - 1) & 1); }
unsigned ach_peek(const Achievements *a) { return a->count ? a->queue[a->head] : 0; }
void ach_pop(Achievements *a)
{
    if (!a->count) return;
    a->head = (a->head + 1) % ACH_QUEUE; --a->count;
}
AchSummary ach_summary(const Achievements *a)
{
    AchSummary s = {0, {0}, 0};
    for (unsigned i = 0; i < ACH_COUNT; ++i)
        if (a->p.unlocked >> i & 1) { ++s.total; ++s.per_tier[ach_defs[i].tier]; }
    s.percent = s.total * 100 / ACH_COUNT;
    return s;
}
const char *ach_tier_name(unsigned tier)
{
    static const char *const names[ACH_TIERS] = {"Bronze", "Silver", "Gold", "Platinum"};
    return tier < ACH_TIERS ? names[tier] : "";
}
