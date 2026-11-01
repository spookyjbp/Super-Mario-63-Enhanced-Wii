#ifndef SM63_WORLD_QUERY_H
#define SM63_WORLD_QUERY_H
#include "game.h"
typedef struct { Rect bounds; TileType type; } WorldShape;
typedef struct {
    const World *world;
    Rect area;
    size_t legacy;
    int x0, x1, y0, y1, x, y;
} WorldQuery;
void world_query_begin(WorldQuery *query, const World *world, Rect area);
bool world_query_next(WorldQuery *query, WorldShape *shape);
float world_shape_surface(WorldShape shape, float x);
#endif
