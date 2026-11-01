#ifndef SM63_LZ4_BLOCK_H
#define SM63_LZ4_BLOCK_H
#include <stddef.h>
#include <stdint.h>
/* LZ4 block format decoder (no frame header). Returns bytes written, or -1 on
   malformed input or overflow. Never reads or writes out of bounds. */
long lz4_block_decode(const uint8_t *src, size_t src_size, uint8_t *dst, size_t dst_size);
#endif
