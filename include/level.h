#ifndef SM63_LEVEL_H
#define SM63_LEVEL_H
#include <stdint.h>
/* Row-major immutable tile data. Versioned offline JSON -> C conversion. */
typedef enum {
    TILE_EMPTY, TILE_SOLID, TILE_PLATFORM, TILE_SLOPE_UP, TILE_SLOPE_DOWN,
    TILE_WATER, TILE_ICE, TILE_PUDDLE, TILE_HAZARD
} TileType;
typedef struct {
    float parallax, spacing, y, width, height;
    uint8_t r, g, b;
} BackgroundLayer;
typedef struct {
    unsigned version, columns, rows, tile_size;
    const uint8_t *tiles;
    const BackgroundLayer *backgrounds;
    unsigned background_count;
} Level;
#endif
