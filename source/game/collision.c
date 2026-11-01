#include "world_query.h"
#include "course_world.h"
#include <math.h>

float player_height(const Player *p) { return p->height>0 ? p->height : PLAYER_HEIGHT; }
static bool overlap(float a,float length,float b,float other)
{ return a < b+other && a+length > b; }
static bool solid(TileType t) { return t!=TILE_EMPTY && t!=TILE_WATER; }
static bool slope(TileType t) {return t==TILE_SLOPE_UP || t==TILE_SLOPE_DOWN;}
bool player_stand_clear(const Player *p,const World *w)
{
    if(w->course) return course_stand_clear(p,w);
    float feet=p->y+player_height(p);
    Rect head={p->x,feet-PLAYER_HEIGHT,PLAYER_WIDTH,PLAYER_HEIGHT-0.1f};
    WorldQuery q;WorldShape s; world_query_begin(&q,w,head);
    while(world_query_next(&q,&s)) {
        if(!solid(s.type) || s.type==TILE_PLATFORM) continue;
        if(slope(s.type) && head.y+head.h<=world_shape_surface(s,s.type==TILE_SLOPE_UP ? head.x+head.w : head.x)) continue;
        if(overlap(head.x,head.w,s.bounds.x,s.bounds.w) &&
           overlap(head.y,head.h,s.bounds.y,s.bounds.h)) return false;
    }
    return true;
}
void player_collision_step(Player *p,const World *w)
{
    if(w->course) {course_collision_step(p,w);return;}
    const float h=player_height(p), original_dx=p->vx*GAME_DT;
    float dx=original_dx;
    WorldQuery q;WorldShape s;
    world_query_begin(&q,w,(Rect){p->x+fminf(0,dx)-1,p->y,PLAYER_WIDTH+fabsf(dx)+2,h});
    while(world_query_next(&q,&s)) {
        Rect r=s.bounds;
        if(!solid(s.type) || s.type==TILE_PLATFORM || !overlap(p->y,h,r.y,r.h)) continue;
        if(p->grounded && p->ground_slope!=0 && p->y+h<=r.y+fabsf(original_dx)+2) continue;
        /* A slope's low end is traversable; its high end is a wall. */
        if(dx>0 && s.type!=TILE_SLOPE_UP && p->x+PLAYER_WIDTH<=r.x)
            dx=fminf(dx,r.x-(p->x+PLAYER_WIDTH));
        else if(dx<0 && s.type!=TILE_SLOPE_DOWN && p->x>=r.x+r.w)
            dx=fmaxf(dx,r.x+r.w-p->x);
    }
    float target=fmaxf(0,fminf(p->x+dx,w->width-PLAYER_WIDTH));
    p->wall_normal=0;p->wall_speed=0;
    if(fabsf(target-p->x-original_dx)>0.001f) {
        p->wall_speed=p->vx;
        p->wall_normal=original_dx>0 ? -1 : 1;
        p->vx=0;
    }
    p->x=target;
    float dy=p->vy*GAME_DT, original_dy=dy;
    const bool was_grounded=p->grounded;
    const float old_slope=p->ground_slope;
    p->grounded=false;p->ground_slope=0;p->material=TILE_EMPTY;
    const float snap=was_grounded && p->vy>=0 ? fabsf(original_dx)+2 : 0;
    world_query_begin(&q,w,(Rect){p->x,p->y-fabsf(dy)-snap-1,PLAYER_WIDTH,h+2*fabsf(dy)+2*snap+2});
    float floor_y=INFINITY;
    TileType floor_type=TILE_EMPTY;
    for(;world_query_next(&q,&s);) {
        Rect r=s.bounds;
        if(!solid(s.type) || !overlap(p->x,PLAYER_WIDTH,r.x,r.w)) continue;
        /* Support the highest point under the footprint so the box cannot
           penetrate a rising slope or snag at a slope/plateau seam. */
        const float foot=s.type==TILE_SLOPE_UP ? p->x+PLAYER_WIDTH : p->x;
        const float surface=world_shape_surface(s,foot);
        const float climb=(slope(s.type) || old_slope!=0) ? snap : 0;
        if(original_dy>=0 && p->y+h<=surface+climb+0.001f &&
           p->y+h+original_dy+climb>=surface && surface<floor_y) {
            floor_y=surface;floor_type=s.type;
        }
        if(original_dy<0 && s.type!=TILE_PLATFORM && p->y>=r.y+r.h && p->y+dy<=r.y+r.h)
            dy=fmaxf(dy,r.y+r.h-p->y);
    }
    if(floor_type!=TILE_EMPTY) {
        dy=floor_y-(p->y+h);p->grounded=true;p->material=floor_type;
        p->ground_slope=floor_type==TILE_SLOPE_UP ? -1 : floor_type==TILE_SLOPE_DOWN ? 1 : 0;
    }
    if(p->y+dy<0) dy=-p->y;
    if(p->grounded || fabsf(dy-original_dy)>0.001f) p->vy=0;
    p->y+=dy;
}
