#include "hud.h"
#include <math.h>

/* DefineSprite 114 (the POWER meter), 96 frames: where it shows 111 (the HP
   pie) and 113 (the water pie) on each frame ({x, y, shown}, stage px).
   1-15 HP slides out, 16-20 back in, 21-35 the slower slide in; 36-45 the
   water meter slides in ("water"), 46-55 out; 56-65 both in ("hurtwater"),
   76-85 HP away ("hpaway"), 86-96 water away ("wateraway"). */
typedef struct { float x, y; int shown; } Place;
static const Place hp_place[96] = {
    {0.0f, 0.0f, 1}, {0.0f, -0.25f, 1}, {0.0f, -1.0f, 1}, {0.0f, -2.3f, 1}, {0.0f, -4.1f, 1}, {0.0f, -6.4f, 1},
    {0.0f, -9.2f, 1}, {0.0f, -12.5f, 1}, {0.0f, -16.35f, 1}, {0.0f, -20.65f, 1}, {0.0f, -25.5f, 1}, {0.0f, -30.85f, 1},
    {0.0f, -36.75f, 1}, {0.0f, -43.1f, 1}, {0.0f, -50.0f, 1}, {0.0f, -32.0f, 1}, {0.0f, -18.0f, 1}, {0.0f, -8.0f, 1},
    {0.0f, -2.0f, 1}, {0.0f, 0.0f, 1}, {0.0f, -50.0f, 1}, {0.0f, -43.1f, 1}, {0.0f, -36.75f, 1}, {0.0f, -30.85f, 1},
    {0.0f, -25.5f, 1}, {0.0f, -20.65f, 1}, {0.0f, -16.35f, 1}, {0.0f, -12.5f, 1}, {0.0f, -9.2f, 1}, {0.0f, -6.4f, 1},
    {0.0f, -4.1f, 1}, {0.0f, -2.3f, 1}, {0.0f, -1.0f, 1}, {0.0f, -0.25f, 1}, {0.0f, 0.0f, 1}, {0, 0, 0},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {0.0f, 0.0f, 1}, {6.3f, 0.0f, 1}, {11.85f, 0.0f, 1}, {16.65f, 0.0f, 1}, {20.75f, 0.0f, 1},
    {24.05f, 0.0f, 1}, {26.65f, 0.0f, 1}, {28.5f, 0.0f, 1}, {29.65f, 0.0f, 1}, {30.0f, 0.0f, 1}, {0.0f, -50.0f, 1},
    {6.3f, -39.5f, 1}, {11.85f, -30.25f, 1}, {16.65f, -22.2f, 1}, {20.75f, -15.45f, 1}, {24.05f, -9.9f, 1}, {26.65f, -5.55f, 1},
    {28.5f, -2.45f, 1}, {29.65f, -0.6f, 1}, {30.0f, 0.0f, 1}, {30.0f, 0.0f, 1}, {26.65f, -5.55f, 1}, {23.35f, -11.1f, 1},
    {20.0f, -16.65f, 1}, {16.65f, -22.2f, 1}, {13.35f, -27.8f, 1}, {10.0f, -33.35f, 1}, {6.65f, -38.9f, 1}, {3.35f, -44.45f, 1},
    {0.0f, -50.0f, 1}, {30.0f, 0.0f, 1}, {29.7f, 0.0f, 1}, {28.8f, 0.0f, 1}, {27.3f, 0.0f, 1}, {25.2f, 0.0f, 1},
    {22.5f, 0.0f, 1}, {19.2f, 0.0f, 1}, {15.3f, 0.0f, 1}, {10.8f, 0.0f, 1}, {5.7f, 0.0f, 1}, {0.0f, 0.0f, 1}};
static const Place water_place[96] = {
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0.0f, -50.0f, 1},
    {0.0f, -39.5f, 1}, {0.0f, -30.25f, 1}, {0.0f, -22.2f, 1}, {0.0f, -15.45f, 1}, {0.0f, -9.9f, 1}, {0.0f, -5.55f, 1},
    {0.0f, -2.45f, 1}, {0.0f, -0.6f, 1}, {0.0f, 0.0f, 1}, {0.0f, -0.5f, 1}, {0.0f, -2.0f, 1}, {0.0f, -4.5f, 1},
    {0.0f, -8.0f, 1}, {0.0f, -12.5f, 1}, {0.0f, -18.0f, 1}, {0.0f, -24.5f, 1}, {0.0f, -32.0f, 1}, {0.0f, -40.5f, 1},
    {0.0f, -50.0f, 1}, {0.0f, -50.0f, 1}, {-6.3f, -39.5f, 1}, {-11.85f, -30.25f, 1}, {-16.65f, -22.2f, 1}, {-20.75f, -15.45f, 1},
    {-24.05f, -9.9f, 1}, {-26.65f, -5.55f, 1}, {-28.5f, -2.45f, 1}, {-29.65f, -0.6f, 1}, {-30.0f, 0.0f, 1}, {0.0f, -0.05f, 1},
    {-6.3f, -0.05f, 1}, {-11.85f, -0.05f, 1}, {-16.65f, -0.05f, 1}, {-20.75f, -0.05f, 1}, {-24.05f, -0.05f, 1}, {-26.65f, -0.05f, 1},
    {-28.5f, -0.05f, 1}, {-29.65f, -0.05f, 1}, {-30.0f, -0.05f, 1}, {-30.0f, -0.05f, 1}, {-26.65f, -0.05f, 1}, {-23.35f, -0.05f, 1},
    {-20.0f, -0.05f, 1}, {-16.65f, -0.05f, 1}, {-13.35f, -0.05f, 1}, {-10.0f, -0.05f, 1}, {-6.65f, -0.05f, 1}, {-3.35f, -0.05f, 1},
    {0.0f, -0.05f, 1}, {-30.0f, -0.05f, 1}, {-29.7f, -0.55f, 1}, {-28.8f, -2.05f, 1}, {-27.3f, -4.55f, 1}, {-25.2f, -8.05f, 1},
    {-22.5f, -12.55f, 1}, {-19.2f, -18.05f, 1}, {-15.3f, -24.5f, 1}, {-10.8f, -32.0f, 1}, {-5.7f, -40.5f, 1}, {0.0f, -50.0f, 1}};

void hud_init(HudState *h, int hp)
{
    *h = (HudState){.meter = 1, .count = 0, .currenthp = hp, .pie = (float)(hp + 1), .water_pie = 9, .playing = 0};
}
static void ease(float *pie, int value)   /* 111 / 113: frame +/- 0.3 toward value + 1 */
{
    const float cur = roundf(*pie);
    if (cur - 1 < value) *pie += 0.3f;
    if (cur - 1 > value) *pie -= 0.3f;
    if (*pie < 1) *pie = 1;
    if (*pie > 9) *pie = 9;
}
/* Arriving on a frame runs its script (stop, or a gotoAndStop that runs the
   target's). */
static void arrive(HudState *h, unsigned frame, int hp, int water)
{
    h->meter = frame;
    switch (frame) {
    case 1:
        h->playing = 0; h->count = 0; h->currenthp = hp;
        if (water <= 7 && hp >= 8) arrive(h, 45, hp, water);
        else if (water <= 7 && hp <= 7) arrive(h, 65, hp, water);
        break;
    case 15: case 45: case 65: h->playing = 0; h->currenthp = hp; break;
    case 20: case 35: case 96: arrive(h, 1, hp, water); break;
    case 55: arrive(h, 15, hp, water); break;
    case 75: arrive(h, 65, hp, water); break;
    case 85: arrive(h, 45, hp, water); break;
    default: break;
    }
}
static void go_play(HudState *h, unsigned label) { h->meter = label; h->playing = 1; }   /* gotoAndStop(label); play() */
void hud_step(HudState *h, int hp, int water)
{
    ++h->ticks;
    ease(&h->pie, hp);
    ease(&h->water_pie, water);
    if (h->playing) { arrive(h, h->meter + 1, hp, water); return; }
    switch (h->meter) {
    case 1:
        if (h->currenthp == hp && hp == 8) ++h->count;
        else { h->currenthp = hp; h->count = 0; }
        if (water <= 7) go_play(h, 56);          /* "hurtwater" */
        if (h->count > 160) h->playing = 1;      /* slide away */
        break;
    case 15:
        if (h->currenthp != hp || hp < 8) {
            if (hp < h->currenthp || hp < 7) h->playing = 1;   /* frames 16-20 */
            else go_play(h, 21);                               /* "slower" */
        }
        if (water != 8) go_play(h, 36);                         /* "water" */
        h->currenthp = hp;
        break;
    case 45:
        if (hp < 7) go_play(h, 56);
        if (water >= 8) h->playing = 1;                         /* 46-55: the water meter leaves */
        h->currenthp = hp;
        break;
    case 65:
        if (hp >= 8) go_play(h, 76);                            /* "hpaway" */
        if (water >= 8) go_play(h, 86);                         /* "wateraway" */
        h->currenthp = hp;
        break;
    default: break;
    }
}
static const Place *at(const Place *t, const HudState *h) { return &t[h->meter >= 1 && h->meter <= 96 ? h->meter - 1 : 0]; }
float hud_meter_offset(const HudState *h) { return at(hp_place, h)->y; }
bool hud_hp_place(const HudState *h, float *x, float *y) { const Place *p = at(hp_place, h); *x = p->x; *y = p->y; return p->shown; }
bool hud_water_place(const HudState *h, float *x, float *y) { const Place *p = at(water_place, h); *x = p->x; *y = p->y; return p->shown; }
unsigned hud_pie_frame(const HudState *h)
{
    const int f = (int)roundf(h->pie) - 1;
    return f < 0 ? 0u : f > 8 ? 8u : (unsigned)f;
}
unsigned hud_water_frame(const HudState *h)
{
    const int f = (int)roundf(h->water_pie) - 1;
    return f < 0 ? 0u : f > 8 ? 8u : (unsigned)f;
}
