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
/* FLUDD water nested in the Dive / Frontflip bodies: which clip and frame to
   draw (false: none). Hover/Turbo: Wait WaterBlast 1157 at the blast level with
   its `_yscale *= -1` parity. Rocket: Rocket Fludd Water 1349/1370 - charge
   frames 1-30 over the port's 15-frame charge, then the blast frames 31-39 over
   the 8-frame boost. `rocket_clip`/`spray_clip` are the pack's clip ids. */
typedef struct { unsigned clip, frame; bool flip; } SprayPick;
bool player_spray_pick(const Player *player, const ArtPick *pick, unsigned spray_clip, unsigned rocket_clip,
                       SprayPick *out);
#endif
