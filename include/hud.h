#ifndef SM63_HUD_H
#define SM63_HUD_H
/* Stats Clip (SWF 800) state: the POWER meter (114) slides in when CharHP
   changes and out after 160 frames at full health; its pie (111) eases 0.3
   frames a frame toward CharHP + 1. Runs at 32 Hz. (The WaterHP labels are
   not used: drowning is not modelled.) */
#ifdef __cplusplus
extern "C" {
#endif
typedef struct { unsigned meter, ticks; int count, currenthp; float pie; int playing; } HudState;
void hud_init(HudState *h, int hp);
void hud_step(HudState *h, int hp);
/* 114's placement of 111 on its current frame (stage px, 0 = shown, -50 = hidden). */
float hud_meter_offset(const HudState *h);
/* 111 frame index 0..8. */
unsigned hud_pie_frame(const HudState *h);
#ifdef __cplusplus
}
#endif
#endif
