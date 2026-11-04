#ifndef SM63_BG_WATER_SIM_H
#define SM63_BG_WATER_SIM_H
/* Dynamic Water (802) in a background: its makeWave / simWave spring strip.
   Points in the clip's own px; the fill runs from the surface down to `deep`. */
#include <stdbool.h>
#define BG_WAVE_MAX 64
typedef struct {
    unsigned n;
    float x[BG_WAVE_MAX], y[BG_WAVE_MAX], vx[BG_WAVE_MAX], vy[BG_WAVE_MAX], fx[BG_WAVE_MAX], fy[BG_WAVE_MAX];
    float rest;
    unsigned seed;
} BgWave;
/* makeWave: n points from x -50, (length + 100) / n apart, rest length half that. */
void bg_wave_init(BgWave *w, unsigned n, float length, unsigned seed);
/* simWave, one source frame (graphics = true: Mario does not stir it). */
void bg_wave_step(BgWave *w, float stiffness, float randomwave);
#endif
