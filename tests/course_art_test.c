/* Course art pack: every area layer and picture page parses and decodes. */
#include "course_art.h"
#include "lz4_block.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char blob[6 << 20], out[2 << 20];
int main(void)
{
    FILE *f = fopen("../../assets/generated/course_8e1_art.bin", "rb"); assert(f);
    const size_t n = fread(blob, 1, sizeof(blob), f); fclose(f);
    static CourseArtPack pack;
    assert(course_art_parse(blob, n, &pack) && pack.zoom == 1.6f);
    const char *areas[] = {"8-E1-1", "8-E1-1-2", "8-E1-2", "8-E1-2-2"};
    for (unsigned i = 0; i < 4; ++i) {
        assert(course_art_layer(&pack, areas[i], ART_LAYER_BACK) && course_art_layer(&pack, areas[i], ART_LAYER_FRONT));
        assert(course_art_layer(&pack, areas[i], ART_LAYER_FAR) && course_art_layer(&pack, areas[i], ART_LAYER_CLOSE));
    }
    assert(!course_art_layer(&pack, "8-E1-2-2", ART_LAYER_BG)); /* no Background set for 8-E1-2-2 */
    const char *pages[] = {"Start", "controls", "goomba", "bomb", "stomp", "ttt", "hover", "refill", "zoom"};
    for (unsigned i = 0; i < 9; ++i) assert(course_art_picture(&pack, pages[i], strlen(pages[i])));
    unsigned tiles = 0;
    for (unsigned l = 0; l < pack.layer_count; ++l)
        for (unsigned t = 0; t < pack.layers[l].tile_count; ++t, ++tiles) {
            const CourseArtTile *tile = &pack.layers[l].tiles[t];
            assert(lz4_block_decode(tile->data, tile->packed, out, sizeof(out)) == (long)tile->size);
            assert(lz4_block_decode(tile->data, tile->packed - 1, out, sizeof(out)) != (long)tile->size);
            assert(lz4_block_decode(tile->data, tile->packed, out, tile->size - 1) == -1); /* no overflow */
        }
    assert(tiles > 50);
    pack.layer_count = 0;
    assert(!course_art_parse(blob, 100, &pack));
    printf("course art: %u layers, %u tiles decoded\n", 0u + (unsigned)(sizeof(areas) / sizeof(*areas)), tiles);
    puts("1 course-art pack test group passed"); return 0;
}
