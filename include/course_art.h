#ifndef SM63_COURSE_ART_H
#define SM63_COURSE_ART_H
/* Course artwork pack (tools/export_course_art.py, "SM63CAR1" v1): LZ4-compressed
   GX RGB5A3 tiles per area layer (RGBA8 when a tile's size is width * height * 4:
   the far background's sky gradient). Pure parsing; GX upload lives in platform code. */
#include "course_pack.h"
#define COURSE_ART_MAX_LAYERS 80
#define COURSE_ART_MAX_TILES 128   /* an animated picture keeps one frame per tile */
typedef enum { ART_LAYER_FAR, ART_LAYER_BG, ART_LAYER_MID, ART_LAYER_CLOSE, ART_LAYER_BACK, ART_LAYER_FRONT,
               ART_LAYER_PICTURE, ART_LAYER_KIND_COUNT } CourseArtKind;
typedef struct { unsigned x, y, width, height, size, packed; const unsigned char *data; } CourseArtTile;
typedef struct {
    CourseString area, name; CourseArtKind kind;
    float ox, oy, parallax, zoom; /* origin: course px of image (0,0); image px per course px */
    unsigned width, height, tile_count;
    CourseArtTile tiles[COURSE_ART_MAX_TILES];
} CourseArtLayer;
typedef struct { unsigned layer_count; float zoom; CourseArtLayer layers[COURSE_ART_MAX_LAYERS]; } CourseArtPack;
bool course_art_parse(const unsigned char *data, size_t size, CourseArtPack *pack);
const CourseArtLayer *course_art_layer(const CourseArtPack *pack, const char *area, CourseArtKind kind);
/* Picture-sign page by label ("controls", "Start"...), stored under area "*". */
const CourseArtLayer *course_art_picture(const CourseArtPack *pack, const char *label, size_t length);
/* The page for the controller in use ("<label>@<controller>", tools/controller_signs.py),
   else the original page. */
const CourseArtLayer *course_art_controller_picture(const CourseArtPack *pack, CourseString label, unsigned controller);
/* Screen quad of one layer tile: (bx, by) is image (0,0) on screen, px screen px
   per image px. Edges are computed from absolute image columns/rows, so a
   tile's right/bottom edge equals the next tile's left/top edge bit for bit. */
void course_layer_tile_quad(float bx, float by, float px, unsigned x, unsigned y, unsigned w, unsigned h,
                            float xs[4], float ys[4]);
/* A water block layer (water_places.h): the repeating bitmap's coordinates
   (in repeats) at course point (x, y), the block placed at (block_x, block_y),
   the layer's shape at (off_x, off_y) mirrored when sx is -1, the pattern's
   origin in the shape and its tile size (course px). */
void course_water_block_uv(float block_x, float block_y, float off_x, float off_y, int sx, float origin_x,
                           float origin_y, float tile, float x, float y, float *u, float *v);
#endif
