#include "sound_pack.h"
#include <string.h>

static uint32_t be32(const unsigned char *p) { return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3]; }

bool sound_pack_parse(const unsigned char *data, size_t size, SoundPack *pack)
{
    memset(pack, 0, sizeof(*pack));
    if (size < 16 || memcmp(data, "SM63SND1", 8) || be32(data + 8) != 1) return false;
    const uint32_t count = be32(data + 12);
    if (count > SOUND_PACK_MAX || 16 + (size_t)count * 16 > size) return false;
    for (uint32_t i = 0; i < count; ++i) {
        const unsigned char *e = data + 16 + i * 16;
        const uint32_t rate = be32(e), samples = be32(e + 4), offset = be32(e + 8), length = be32(e + 12);
        if (rate == 0 || offset > size || length > size - offset || length < 4 || (length - 4) * 2 < samples) return false;
        pack->sound[i] = (PackedSound){rate, samples, data + offset, length};
    }
    pack->count = count;
    return true;
}

static const int16_t steps[89] = {
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 50, 55, 60, 66, 73, 80,
    88, 97, 107, 118, 130, 143, 157, 173, 190, 209, 230, 253, 279, 307, 337, 371, 408, 449, 494, 544,
    598, 658, 724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749,
    3024, 3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487,
    12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767};
static const int8_t index_step[8] = {-1, -1, -1, -1, 2, 4, 6, 8};

bool sound_pack_decode(const PackedSound *s, int16_t *out)
{
    int pred = (int16_t)(s->data[0] << 8 | s->data[1]), index = 0;
    const unsigned char *n = s->data + 4;
    for (unsigned i = 0; i < s->samples; ++i) {
        const int code = i & 1 ? n[i >> 1] & 15 : n[i >> 1] >> 4;
        const int step = steps[index];
        int delta = step >> 3;
        if (code & 4) delta += step;
        if (code & 2) delta += step >> 1;
        if (code & 1) delta += step >> 2;
        pred = code & 8 ? pred - delta : pred + delta;
        if (pred > 32767) pred = 32767;
        if (pred < -32768) pred = -32768;
        index += index_step[code & 7];
        if (index < 0) index = 0;
        if (index > 88) index = 88;
        out[i] = (int16_t)pred;
    }
    return true;
}
