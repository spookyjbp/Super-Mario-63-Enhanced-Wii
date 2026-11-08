#include "achievements.h"
#include "save_data.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void test_definitions(void)
{
    /* exactly 64, ids 1..64 in order, 30 / 20 / 13 / 1 by tier, all named */
    unsigned tiers[ACH_TIERS] = {0};
    for (unsigned i = 0; i < ACH_COUNT; ++i) {
        const AchDef *d = &ach_defs[i];
        assert(d->id == i + 1 && d->name && *d->name && d->description && *d->description);
        ++tiers[d->tier];
        assert((d->kind == ACH_PENDING) == (d->pending != NULL));
    }
    assert(tiers[ACH_BRONZE] == 30 && tiers[ACH_SILVER] == 20 && tiers[ACH_GOLD] == 13 && tiers[ACH_PLATINUM] == 1);
    assert(ach_defs[63].kind == ACH_ALL_OTHERS && ach_defs[63].tier == ACH_PLATINUM);
    /* pending only where the original has no matching event */
    unsigned pending = 0;
    for (unsigned i = 0; i < ACH_COUNT; ++i) pending += ach_defs[i].kind == ACH_PENDING;
    assert(pending == 3 && ach_defs[1].kind == ACH_PENDING && ach_defs[53].kind == ACH_PENDING &&
           ach_defs[54].kind == ACH_PENDING);
}
static void test_tutorial_and_queue(void)
{
    Achievements a;
    ach_init(&a, NULL);
    /* Intro Movie 2 after the tutorial door: _root.Star[39] = true */
    assert(ach_shine(&a, 39) == 1 && ach_unlocked(&a, 1) && a.dirty);
    assert(ach_peek(&a) == 1);
    ach_pop(&a);
    assert(ach_peek(&a) == 0);
    assert(ach_shine(&a, 39) == 0);   /* the same shine again: nothing */
    /* several at once queue in unlock order: Star[1] is King Bob-omb (#3) */
    for (unsigned s = 1; s <= 5; ++s) ach_shine(&a, s);
    unsigned order[8], n = 0;
    while (ach_peek(&a)) { order[n++] = ach_peek(&a); ach_pop(&a); }
    assert(n == 4 && order[0] == 3 && order[1] == 4 && order[2] == 5 && order[3] == 31);
}
static void test_counts_and_keys(void)
{
    Achievements a;
    ach_init(&a, NULL);
    for (unsigned s = 1; s <= 9; ++s) ach_shine(&a, s);
    assert(!ach_unlocked(&a, 30));
    ach_shine(&a, 10);
    assert(ach_unlocked(&a, 30) && ach_unlocked(&a, 32));   /* 10 shines; all of Snowmans Land */
    for (unsigned k = 1; k <= 3; ++k) ach_key(&a, k);
    assert(ach_unlocked(&a, 10) && ach_unlocked(&a, 20) && ach_unlocked(&a, 52));
    for (unsigned c = 1; c <= 31; ++c) ach_star_coin(&a, c);
    assert(!ach_unlocked(&a, 49));
    ach_star_coin(&a, 31);   /* a duplicate does not count */
    assert(!ach_unlocked(&a, 49));
    ach_star_coin(&a, 32);
    assert(ach_unlocked(&a, 49));
    ach_milestone(&a, ACH_MS_ENDING);
    assert(ach_unlocked(&a, 53));
}
static void test_power_of_64_and_platinum(void)
{
    Achievements a;
    ach_init(&a, NULL);
    for (unsigned s = 1; s <= 64; ++s) ach_shine(&a, s);
    assert(ach_unlocked(&a, 61) && !ach_unlocked(&a, 63) && !ach_unlocked(&a, 62));
    for (unsigned c = 1; c <= 64; ++c) ach_star_coin(&a, c);
    assert(ach_unlocked(&a, 60) && ach_unlocked(&a, 63) && !ach_unlocked(&a, 62));   /* keys missing */
    for (unsigned k = 1; k <= 3; ++k) ach_key(&a, k);
    ach_milestone(&a, ACH_MS_ENTER_BOWSER_TRAP2);
    ach_milestone(&a, ACH_MS_ENDING);
    assert(ach_unlocked(&a, 62));
    /* everything but the three pending ones */
    assert(ach_summary(&a).total == 60 && !ach_unlocked(&a, 64));
    /* the last ones (dev test): the Platinum follows, once, queued after it */
    while (ach_peek(&a)) ach_pop(&a);
    assert(ach_force(&a, 2) == 1 && ach_force(&a, 54) == 1 && !ach_unlocked(&a, 64));
    assert(ach_force(&a, 55) == 2 && ach_unlocked(&a, 64));
    ach_pop(&a); ach_pop(&a);
    assert(ach_peek(&a) == 55);
    ach_pop(&a);
    assert(ach_peek(&a) == 64);
    assert(ach_force(&a, 64) == 0);
    const AchSummary s = ach_summary(&a);
    assert(s.total == 64 && s.percent == 100 && s.per_tier[ACH_PLATINUM] == 1 && s.per_tier[ACH_BRONZE] == 30);
}
static void test_existing_save(void)
{
    /* a loaded file with Star[39] and 10 shines: awarded on load, nothing invented */
    AchProgress p = {0};
    p.shines = 0x3FFULL | 1ULL << 38;
    Achievements a;
    ach_init(&a, &p);
    assert(ach_evaluate_all(&a) >= 3 && ach_unlocked(&a, 1) && ach_unlocked(&a, 30) && !ach_unlocked(&a, 53));
    assert(ach_evaluate_all(&a) == 0);
}
static void test_save_format(void)
{
    SaveData s = {0}, back;
    s.current = 1; s.music_muted = true;
    s.slot[1].used = true;
    s.slot[1].progress = (AchProgress){0xF0F0F0F0F0F0F0F0ULL, 0x123456789ABCDEF0ULL, 5, 6, 0x8000000000000001ULL};
    unsigned char buf[SAVE_BYTES];
    assert(save_encode(&s, buf) == SAVE_BYTES && !memcmp(buf, "SM63WIIS", 8));
    assert(save_decode(buf, sizeof buf, &back));
    assert(back.current == 1 && back.music_muted && back.slot[1].used && !back.slot[0].used);
    assert(back.slot[1].progress.shines == s.slot[1].progress.shines &&
           back.slot[1].progress.star_coins == s.slot[1].progress.star_coins &&
           back.slot[1].progress.keys == 5 && back.slot[1].progress.milestones == 6 &&
           back.slot[1].progress.unlocked == s.slot[1].progress.unlocked);
    /* one flipped bit anywhere: rejected */
    for (unsigned i = 0; i < SAVE_BYTES; i += 7) {
        buf[i] ^= 0x10;
        assert(!save_decode(buf, sizeof buf, &back));
        buf[i] ^= 0x10;
    }
    assert(!save_decode(buf, 20, &back));
    /* a newer minor version with an extra slot loads (the extra is ignored) */
    unsigned char big[SAVE_BYTES + SAVE_SLOT_BYTES];
    memcpy(big, buf, SAVE_BYTES - 4);
    memset(big + SAVE_BYTES - 4, 0, SAVE_SLOT_BYTES);
    big[9] = 2; big[11] = SAVE_SLOTS + 1;
    const uint32_t c = save_crc32(big, sizeof big - 4);
    big[sizeof big - 4] = (unsigned char)(c >> 24); big[sizeof big - 3] = (unsigned char)(c >> 16);
    big[sizeof big - 2] = (unsigned char)(c >> 8); big[sizeof big - 1] = (unsigned char)c;
    assert(save_decode(big, sizeof big, &back) && back.slot[1].progress.keys == 5);
    assert(save_crc32((const unsigned char *)"123456789", 9) == 0xCBF43926u);
}
int main(void)
{
    test_definitions(); test_tutorial_and_queue(); test_counts_and_keys(); test_power_of_64_and_platinum();
    test_existing_save(); test_save_format();
    puts("6 achievement (definitions, tutorial + queue, counts + keys, Power of 64 + Platinum, existing save, save format) test groups passed");
    return 0;
}
