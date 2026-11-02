/* Course art pack: every area layer and picture page parses and decodes. */
#include "course_art.h"
#include "game.h"
#include "lz4_block.h"
#include <assert.h>
#include <math.h>
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
    /* Bug #5: adjacent tiles share their edge coordinates exactly at any
       camera offset / scale, so no hairline opens between 1024 px tiles. */
    const float bases[] = {-1234.567f, -0.3f, 0, 17.123f, 999.99f}, scales[] = {0.7272727f, 0.8593750f, 1, 1.0078f};
    for (unsigned l = 0; l < pack.layer_count; ++l)
        for (unsigned a = 0; a < 5; ++a)
            for (unsigned s = 0; s < 4; ++s)
                for (unsigned t = 0; t < pack.layers[l].tile_count; ++t)
                    for (unsigned u = 0; u < pack.layers[l].tile_count; ++u) {
                        const CourseArtTile *p = &pack.layers[l].tiles[t], *q = &pack.layers[l].tiles[u];
                        float px[4], py[4], qx[4], qy[4];
                        course_layer_tile_quad(bases[a], bases[4 - a], scales[s], p->x, p->y, p->width, p->height, px, py);
                        course_layer_tile_quad(bases[a], bases[4 - a], scales[s], q->x, q->y, q->width, q->height, qx, qy);
                        if (q->x == p->x + p->width && q->y == p->y) assert(qx[0] == px[1] && qy[0] == py[0]);
                        if (q->y == p->y + p->height && q->x == p->x) assert(qy[0] == py[2] && qx[0] == px[0]);
                    }
    /* Phase 2 item 2: every keyboard page has a version per controller (GameCube, Classic, Wii Remote); the
       others (goomba, refill, ttt) fall back to the original page. */
    const char *keyboard[] = {"Start", "controls", "bomb", "hover", "stomp", "zoom"};
    for (unsigned i = 0; i < 6; ++i) {
        const CourseString label = {keyboard[i], (uint16_t)strlen(keyboard[i])};
        const CourseArtLayer *orig = course_art_picture(&pack, keyboard[i], strlen(keyboard[i]));
        for (unsigned k = 0; k < CONTROLLER_KINDS; ++k) {
            const CourseArtLayer *l = course_art_controller_picture(&pack, label, k);
            assert(l && l != orig && l->width == orig->width && l->height == orig->height);
            assert(l->tile_count == 1 && (size_t)l->width * l->height * 2 <= 1024u * 1024); /* one picture texture */
            for (unsigned j = 0; j < k; ++j) assert(course_art_controller_picture(&pack, label, j) != l);
        }
        assert(course_art_controller_picture(&pack, label, 9) == orig);
    }
    const CourseString goomba = {"goomba", 6};
    assert(course_art_controller_picture(&pack, goomba, 0) == course_art_picture(&pack, "goomba", 6));
    /* Phase 2 item 5: water quads tile one texel per course px, abut, and keep
       the surface frame's V range half a texel inside its frame. */
    for (float t = 0; t < 30; t += 0.37f) {
        CourseWaterQuad q[2];
        const float scale = 1.375f;
        assert(course_water_quads(100, 200, 400 * scale, 150 * scale, scale, t, q) == 2);
        assert(q[0].y0 == q[1].y1 && q[1].y0 < 200 && q[0].y1 == 200 + 150 * scale);   /* body under the strip */
        assert(fabsf((q[0].u1 - q[0].u0) - 400.0f / 64) < 1e-3f && fabsf((q[1].u1 - q[1].u0) - 400.0f / 64) < 1e-3f);
        const unsigned f = (unsigned)(t * 8) % 8;
        assert(q[1].v0 > f / 8.0f && q[1].v1 < (f + 1) / 8.0f);                       /* inside its frame */
        assert(fabsf(q[0].u0) < 1.01f && fabsf(q[1].u0) < 1.01f);                     /* wrapped: no precision loss */
    }
    {
        CourseWaterQuad q[2];
        assert(course_water_quads(0, 0, 50, 5 * 1.375f, 1.375f, 0, q) == 1);             /* shallow: surface only */
        assert(course_water_quads(0, 0, 0, 50, 1.375f, 0, q) == 0);
    }
    pack.layer_count = 0;
    assert(!course_art_parse(blob, 100, &pack));
    printf("course art: %u layers, %u tiles decoded\n", 0u + (unsigned)(sizeof(areas) / sizeof(*areas)), tiles);
    puts("3 course-art pack/controller page/water test groups passed"); return 0;
}
