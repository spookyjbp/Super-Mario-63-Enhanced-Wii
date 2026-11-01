#include "player_sprite.h"
#include "mario_atlas.h"
#include <math.h>

PlayerSprite player_sprite_select(const Player *p)
{
    unsigned frame = 0;
    bool spin_mirror = false;
    switch (p->animation) {
    case PLAYER_RUN: frame = 1 + (unsigned)(p->animation_time * 12) % 8; break;
    case PLAYER_RISE: frame = 9; break;
    case PLAYER_FALL: frame = 10; break;
    case PLAYER_SPIN: {
        /* Mirror flags are per occurrence, not per image: the Flash clip reuses
           the same side/back PNGs to show opposite halves of a complete turn.
           Mario (none) Spin Attack.xml, keyframes 0,5,10,15,20,25,30,35. */
        static const struct { unsigned frame; bool mirror; } cycle[] = {
            {13, true}, {12, true}, {11, false}, {12, false},
            {13, false}, {14, false}, {15, false}, {14, true}
        };
        const unsigned tick = (unsigned)(fmaxf(0, 0.32f - p->spin_time) * 25);
        frame = cycle[tick % 8].frame;
        spin_mirror = cycle[tick % 8].mirror;
        break;
    }
    case PLAYER_POUND_WINDUP: frame = p->pound_windup > 0.06f ? 16 : 17; break;
    case PLAYER_POUND: frame = 18; break;
    case PLAYER_LAND: frame = 19; break;
    case PLAYER_IDLE: break;
    }
    const bool pose_mirror = p->animation == PLAYER_SPIN ? spin_mirror :
                             mario_frames[frame].mirror_for_right != 0;
    /* Compose once. No pixel preprocessing or second renderer flip is needed. */
    return (PlayerSprite){frame, p->facing_left != pose_mirror};
}
