#include "hud.h"
#include <math.h>

/* DefineSprite 114 frames 1-35: 111's y (frames 1-15 slide out, 16-20 back in,
   21-35 the slower slide in). */
static const float meter_y[35] = {
    0, -0.25f, -1, -2.3f, -4.1f, -6.4f, -9.2f, -12.5f, -16.35f, -20.65f, -25.5f, -30.85f, -36.75f, -43.1f, -50,
    -32, -18, -8, -2, 0, -50, -43.1f, -36.75f, -30.85f, -25.5f, -20.65f, -16.35f, -12.5f, -9.2f, -6.4f, -4.1f,
    -2.3f, -1, -0.25f, 0};

void hud_init(HudState *h, int hp)
{
    *h = (HudState){.meter = 1, .count = 0, .currenthp = hp, .pie = (float)(hp + 1), .playing = 0};
}
void hud_step(HudState *h, int hp)
{
    ++h->ticks;
    /* 111: if(_currentframe - 1 < CharHP) frame += 0.3 (and down); gotoAndStop(round(frame)). */
    const float cur = roundf(h->pie);
    if (cur - 1 < hp) h->pie += 0.3f;
    if (cur - 1 > hp) h->pie -= 0.3f;
    if (h->pie < 1) h->pie = 1;
    if (h->pie > 9) h->pie = 9;
    if (h->playing) {
        ++h->meter;
        if (h->meter == 15) { h->playing = 0; h->currenthp = hp; }            /* frame 15: stop() */
        else if (h->meter == 20 || h->meter == 35) {                        /* gotoAndStop(1) */
            h->meter = 1; h->playing = 0; h->count = 0; h->currenthp = hp;
        }
        return;
    }
    if (h->meter == 1) {
        if (h->currenthp == hp && hp == 8) ++h->count;
        else { h->currenthp = hp; h->count = 0; }
        if (h->count > 160) h->playing = 1;                                 /* slide away */
    } else if (h->meter == 15) {
        if (h->currenthp != hp || hp < 8) {
            if (hp < h->currenthp || hp < 7) h->playing = 1;                /* frames 16-20 */
            else { h->meter = 21; h->playing = 1; }                         /* gotoAndStop("slower"); play() */
        }
        h->currenthp = hp;
    }
}
float hud_meter_offset(const HudState *h) { return meter_y[h->meter >= 1 && h->meter <= 35 ? h->meter - 1 : 0]; }
unsigned hud_pie_frame(const HudState *h)
{
    const int f = (int)roundf(h->pie) - 1;
    return f < 0 ? 0u : f > 8 ? 8u : (unsigned)f;
}
