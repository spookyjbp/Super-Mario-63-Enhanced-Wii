#include "bg_water.h"
#include <math.h>
#include <string.h>

static float rnd(BgWave *w) /* Math.random() */
{
    w->seed = w->seed * 1664525u + 1013904223u;
    return (float)(w->seed >> 8) / 16777216.0f;
}
void bg_wave_init(BgWave *w, unsigned n, float length, unsigned seed)
{
    memset(w, 0, sizeof(*w));
    if (n > BG_WAVE_MAX) n = BG_WAVE_MAX;
    if (n < 2) n = 2;
    w->n = n; w->seed = seed ? seed : 1;
    const float step = (length + 100) / (float)n;
    for (unsigned i = 0; i < n; ++i) w->x[i] = -50 + (float)i * step;
    w->rest = 0.5f * step;
}
void bg_wave_step(BgWave *w, float stiffness, float randomwave)
{
    const unsigned n = w->n;
    for (unsigned i = 0; i < n; ++i) {   /* random turbulence, kept one frame in ten (turbProb 0.1) */
        w->fy[i] += rnd(w) * (randomwave * 2) - randomwave;
        w->fx[i] = 0;
        if (rnd(w) >= 0.1f) w->fy[i] = 0;
    }
    for (unsigned i = 0; i + 1 < n; ++i) {   /* springs between neighbours; the ends stay put */
        const float dx = w->x[i + 1] - w->x[i], dy = w->y[i + 1] - w->y[i];
        const float len = sqrtf(dx * dx + dy * dy);
        if (len <= 0) continue;
        const float f = stiffness * (len - w->rest), fx = f * dx / len, fy = f * dy / len;
        if (i != 0) { w->fx[i] += fx; w->fy[i] += fy; }
        if (i != n - 2) { w->fx[i + 1] -= fx; w->fy[i + 1] -= fy; }
    }
    for (unsigned i = 1; i + 1 < n; ++i) {   /* kDamp 0.85 */
        w->vx[i] = w->vx[i] * 0.85f + w->fx[i]; w->vy[i] = w->vy[i] * 0.85f + w->fy[i];
        w->x[i] += w->vx[i]; w->y[i] += w->vy[i];
    }
}
