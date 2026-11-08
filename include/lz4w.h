#ifndef SM63_LZ4W_H
#define SM63_LZ4W_H
/* Embedded data stored LZ4-compressed to save MEM1 ("SM63LZ4W", u32 raw
   size, u32 packed size, 4 zero bytes, then the LZ4 block; tools/pack_lz4.py):
   unpacked once at boot into MEM2. Data without the header is used as is. */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
/* The unpacked size (0: not wrapped). */
size_t lz4w_size(const uint8_t *data, size_t size);
bool lz4w_unpack(const uint8_t *data, size_t size, uint8_t *dst, size_t cap);
/* `data` itself, or unpacked into memory from mem2_alloc(); NULL on failure. */
const uint8_t *lz4w_open(const uint8_t *data, size_t size, size_t *out_size);
/* 32-byte aligned memory that is never freed (the Wii: MEM2's arena). */
void *mem2_alloc(size_t size);
#endif
