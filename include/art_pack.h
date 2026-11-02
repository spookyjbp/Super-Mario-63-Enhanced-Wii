#ifndef SM63_ART_PACK_H
#define SM63_ART_PACK_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#define ART_CLIP_COUNT 256
#define ART_MAX_FRAMES 4096
#define ART_PACK_VERSION 2
#define ART_PACK_VERSION_COMPRESSED 3
/* Clip ids (tools/export_movement_clips.py): body = state*4 + variant, where
   variant 0 is no FLUDD and 1..3 follow Nozzle (Hover, Rocket, Turbo).
   ART_BACK_EFFECT/ART_FRONT_EFFECT + body id hold the FLUDD water blast drawn
   behind/in front of that body clip, as layered in Mario Main. */
typedef enum { ART_WALK, ART_RISE, ART_FALL, ART_DIVE, ART_DIVEFLIP, ART_BACKFLIP,
               ART_FRONTFLIP, ART_SPIN, ART_SWIM_FLOAT, ART_SWIM_STROKE, ART_LAND,
               ART_RUN, ART_GROUND, ART_HURT2, ART_SIGN, ART_HURT1, ART_HURT3, ART_STATE_COUNT } ArtState;
#define ART_VARIANTS 4
#define ART_BACK_EFFECT 80
#define ART_FRONT_EFFECT 160
static inline unsigned art_clip(ArtState state, unsigned variant) {return (unsigned)state*ART_VARIANTS+variant;}
typedef struct {uint16_t first,count,flags;} ArtClip;
typedef struct {uint16_t x,y,width,height,duration;int16_t offset_x,offset_y;} ArtFrame;
typedef struct {
    unsigned width,height,frame_count;
    ArtClip clips[ART_CLIP_COUNT];
    ArtFrame frames[ART_MAX_FRAMES];
    const unsigned char *texture;
    /* Version 3 packs: GX RGB5A3 texture, LZ4 block compressed. The caller
       decodes `packed` (raw_size bytes) and points `texture` at the result. */
    bool rgb5a3;
    const unsigned char *packed;
    unsigned packed_size, raw_size;
} ArtPack;
bool art_pack_parse(const unsigned char *data,size_t size,ArtPack *pack);
const ArtFrame *art_pack_frame(const ArtPack *pack,unsigned clip,float seconds);
/* Frame by zero-based timeline index (clamped), for script-driven clips. */
const ArtFrame *art_pack_frame_index(const ArtPack *pack,unsigned clip,unsigned index);
unsigned art_pack_clip_length(const ArtPack *pack,unsigned clip);
#endif
