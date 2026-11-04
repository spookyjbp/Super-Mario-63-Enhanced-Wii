/* Course art pack: every area layer and picture page parses and decodes. */
#include "course_art.h"
#include "water_places.h"
#include "game.h"
#include "lz4_block.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char blob[12 << 20], out[2 << 20];
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
    /* Text signs: the original message box (Stats.Message frame 6) and one
       "Press .. to Continue" prompt per controller. */
    const CourseArtLayer *box = course_art_picture(&pack, "Message", 7);
    assert(box && box->width > 500 && box->height > 100 && box->ox > 0 && box->oy > 0 && box->ox < 100);
    for (unsigned k = 0; k < CONTROLLER_KINDS; ++k) {
        char key[16]; snprintf(key, sizeof key, "Prompt@%u", k);
        const CourseArtLayer *p = course_art_picture(&pack, key, strlen(key));
        /* origin from the prompt's registration (placed by ui_layout.h), the button centred like the "Z" */
        assert(p && p->tile_count == 1 && p->width > 100 && p->height > 10 && (size_t)p->width * p->height * 4 <= 128u * 1024);
        assert(p->ox < 0 && p->ox > -(float)p->width / 1.6f && p->oy < 0 && p->oy > -(float)p->height / 1.6f);
    }
    /* Toad's talk prompt (bz): the controller's button in the key's two poses,
       side by side, placed around the key (tm (143.45, 1)) in tm px. */
    for (unsigned k = 0; k < CONTROLLER_KINDS; ++k) {
        char key[16]; snprintf(key, sizeof key, "TalkZ@%u", k);
        const CourseArtLayer *z = course_art_picture(&pack, key, strlen(key));
        assert(z && z->tile_count == 1 && z->width % 2 == 0 && (size_t)z->width * z->height * 4 <= 128u * 1024);
        const float cx = z->ox + z->width / 4.0f / 1.6f, cy = z->oy + z->height / 2.0f / 1.6f;
        assert(cx > 60 && cx < 150 && fabsf(cy - 1) < 3);
    }
    /* The water blocks: one bitmap repeat is 64 course px; the layer slides
       64 px diagonally over its 133 frames, so the loop is seamless; the
       mirrored layer runs its u the other way. */
    for (unsigned k = 0; k < WATER_BLOCKS; ++k)
        for (unsigned l = 0; l < 2; ++l) {
            const WaterBlockLayer *L = &water_blocks[k].layers[l];
            float u0, v0, u1, v1, u2, v2;
            assert(L->tile == 64 && L->alpha > 0.29f && L->alpha < 0.31f && (L->sx == 1 || L->sx == -1));
            course_water_block_uv(water_blocks[k].x, water_blocks[k].y, L->offs[0][0], L->offs[0][1], L->sx, L->origin_x,
                                  L->origin_y, L->tile, 900, 200, &u0, &v0);
            course_water_block_uv(water_blocks[k].x, water_blocks[k].y, L->offs[WATER_BLOCK_FRAMES - 1][0],
                                  L->offs[WATER_BLOCK_FRAMES - 1][1], L->sx, L->origin_x, L->origin_y, L->tile, 900, 200, &u1, &v1);
            assert(fabsf(fabsf(u1 - u0) - 1) < 0.01f && fabsf(fabsf(v1 - v0) - 1) < 0.01f);
            course_water_block_uv(water_blocks[k].x, water_blocks[k].y, L->offs[0][0], L->offs[0][1], L->sx, L->origin_x,
                                  L->origin_y, L->tile, 964, 200, &u2, &v2);
            assert(fabsf(u2 - u0 - (float)L->sx) < 1e-3f && v2 == v0);
        }
    pack.layer_count = 0;
    assert(!course_art_parse(blob, 100, &pack));
    printf("course art: %u layers, %u tiles decoded\n", 0u + (unsigned)(sizeof(areas) / sizeof(*areas)), tiles);
    puts("3 course-art pack/controller page/water test groups passed"); return 0;
}
