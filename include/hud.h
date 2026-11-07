#ifndef SM63_HUD_H
#define SM63_HUD_H
/* Stats Clip (SWF 800) state: the POWER meter (114) slides in when CharHP
   changes and out after 160 frames at full health; its pie (111) eases 0.3
   frames a frame toward CharHP + 1. Under water the water pie (113, toward
   WaterHP + 1) slides in beside or instead of it. Runs at 32 Hz. */
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct { unsigned meter, ticks; int count, currenthp; float pie, water_pie; int playing; } HudState;
void hud_init(HudState *h, int hp);
void hud_step(HudState *h, int hp, int water_hp);
/* 114's placement of 111 on its current frame (stage px, 0 = shown, -50 = hidden). */
float hud_meter_offset(const HudState *h);
/* 111 frame index 0..8. */
unsigned hud_pie_frame(const HudState *h);
/* 113 (water pie) frame index 0..8. */
unsigned hud_water_frame(const HudState *h);
/* Where 114 shows 111 / 113 now (stage px from the meter's place); false: not shown. */
bool hud_hp_place(const HudState *h, float *x, float *y);
bool hud_water_place(const HudState *h, float *x, float *y);
#ifdef __cplusplus
}
#endif
#endif
