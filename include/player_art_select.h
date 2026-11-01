#ifndef SM63_PLAYER_ART_SELECT_H
#define SM63_PLAYER_ART_SELECT_H
#include "art_pack.h"
#include "game.h"
/* Which original Mario Main clip and frame represent the player right now.
   Pure: no pack or GX access, so it is host-tested. */
typedef struct {
    ArtState state;
    unsigned variant;      /* 0 none, 1..3 = Nozzle + 1 */
    bool timed;            /* true: play by `seconds` at 32 Hz; false: use `index` */
    float seconds;
    unsigned index;
    unsigned effect_index; /* frame of the back/front FLUDD water layers, if any */
    float rotation;        /* degrees, canonical-left clip space */
    int pivot_x, pivot_y;  /* rotation pivot, twips from the feet (canonical-left) */
} ArtPick;
ArtPick player_art_select(const Player *player);
#endif
