#include "world_query.h"
#include <math.h>

static bool intersects(Rect a, Rect b)
{
    return a.x <= b.x+b.w && a.x+a.w >= b.x && a.y <= b.y+b.h && a.y+a.h >= b.y;
}
void world_query_begin(WorldQuery *q, const World *w, Rect area)
{
    *q = (WorldQuery){.world=w, .area=area, .x1=-1, .y1=-1};
    if (!w->level) return;
    const Level *l=w->level;
    q->x0=(int)fmaxf(0,floorf(area.x/l->tile_size));
    q->y0=(int)fmaxf(0,floorf(area.y/l->tile_size));
    q->x1=(int)fminf(l->columns-1,floorf((area.x+area.w)/l->tile_size));
    q->y1=(int)fminf(l->rows-1,floorf((area.y+area.h)/l->tile_size));
    q->x=q->x0; q->y=q->y0;
}
bool world_query_next(WorldQuery *q, WorldShape *shape)
{
    const World *w=q->world;
    while (q->legacy<w->count) {
        Rect r=w->solids[q->legacy++];
        if (intersects(r,q->area)) { *shape=(WorldShape){r,TILE_SOLID}; return true; }
    }
    if (!w->level || q->x0>q->x1) return false;
    const Level *l=w->level;
    while (q->y<=q->y1) {
        int x=q->x, y=q->y;
        if (++q->x>q->x1) {q->x=q->x0; ++q->y;}
        const TileType type=(TileType)l->tiles[y*l->columns+x];
        if (type==TILE_EMPTY) continue;
        *shape=(WorldShape){{x*l->tile_size,y*l->tile_size,l->tile_size,l->tile_size},type};
        return true;
    }
    return false;
}
float world_shape_surface(WorldShape s, float x)
{
    float t=fmaxf(0,fminf(1,(x-s.bounds.x)/s.bounds.w));
    if (s.type==TILE_SLOPE_UP) return s.bounds.y+s.bounds.h*(1-t);
    if (s.type==TILE_SLOPE_DOWN) return s.bounds.y+s.bounds.h*t;
    return s.bounds.y;
}
bool world_water_at(const World *w, float x, float y, float *surface)
{
    WorldQuery q; WorldShape s;
    world_query_begin(&q,w,(Rect){x,y,0,0});
    while(world_query_next(&q,&s)) if(s.type==TILE_WATER) {
        int row=(int)(s.bounds.y/w->level->tile_size);
        int col=(int)(s.bounds.x/w->level->tile_size);
        while(row>0 && w->level->tiles[(row-1)*w->level->columns+col]==TILE_WATER) --row;
        if(surface) *surface=row*w->level->tile_size;
        return true;
    }
    return false;
}
bool world_hazard_at(const World *w,float x,float y)
{
    WorldQuery q; WorldShape s;
    world_query_begin(&q,w,(Rect){x,y,0,0});
    while(world_query_next(&q,&s)) if(s.type==TILE_HAZARD) return true;
    return false;
}
