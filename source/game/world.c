#include "game.h"

/* A collision playground, not a Story Mode level. All surfaces are solid. */
static const Rect platforms[] = {
    {0, 800, 1640, 160}, {1800, 800, 800, 160},
    {300, 690, 170, 24}, {540, 590, 170, 24}, {800, 490, 180, 24},
    {1080, 690, 170, 24}, {1200, 550, 160, 24},
    {1420, 690, 48, 110}, /* Wall and ledge. */
    {1880, 650, 200, 24}, {2160, 550, 220, 24},
    {80, 640, 120, 24} /* Low ceiling above the starting area. */
};
const World movement_world = {
    platforms, sizeof(platforms) / sizeof(platforms[0]), 2600, 960, 90, 756
};
