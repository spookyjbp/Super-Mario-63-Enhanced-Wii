#ifndef SM63_ART_PACK_H
#define SM63_ART_PACK_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#define ART_CLIP_COUNT 64
#define ART_MAX_FRAMES 256
/* 0..9: body; 16+body*3+nozzle: fully equipped body variants;
   48..50: equipment-only fallback; 51..53: nozzle water effects. */
typedef enum { ART_DOWN,ART_RISE,ART_FALL,ART_DOUBLE,ART_TRIPLE,ART_BACKFLIP,
               ART_SWIM_FLOAT,ART_SWIM_STROKE,ART_LAND,ART_TAKEOFF } ArtBody;
typedef struct {uint16_t first,count,flags;} ArtClip;
typedef struct {uint16_t x,y,width,height,duration;int16_t offset_x,offset_y;} ArtFrame;
typedef struct {
    unsigned width,height,frame_count;
    ArtClip clips[ART_CLIP_COUNT];
    ArtFrame frames[ART_MAX_FRAMES];
    const unsigned char *texture;
} ArtPack;
bool art_pack_parse(const unsigned char *data,size_t size,ArtPack *pack);
const ArtFrame *art_pack_frame(const ArtPack *pack,unsigned clip,float seconds);
#endif
