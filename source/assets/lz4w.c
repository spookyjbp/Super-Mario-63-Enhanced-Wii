#include "lz4w.h"
#include "lz4_block.h"
#include <string.h>

static uint32_t be32(const uint8_t *p) { return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3]; }
size_t lz4w_size(const uint8_t *data, size_t size)
{
    return size >= 20 && !memcmp(data, "SM63LZ4W", 8) ? be32(data + 8) : 0;
}
bool lz4w_unpack(const uint8_t *data, size_t size, uint8_t *dst, size_t cap)
{
    const size_t raw = lz4w_size(data, size), packed = size >= 20 ? be32(data + 12) : 0;
    if (!raw || raw > cap || 20 + packed > size) return false;
    return lz4_block_decode(data + 20, packed, dst, raw) == (long)raw;
}
const uint8_t *lz4w_open(const uint8_t *data, size_t size, size_t *out_size)
{
    const size_t raw = lz4w_size(data, size);
    *out_size = raw ? raw : size;
    if (!raw) return data;
    uint8_t *dst = mem2_alloc(raw);
    return dst && lz4w_unpack(data, size, dst, raw) ? dst : NULL;
}
