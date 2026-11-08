#include "save_data.h"
#include <string.h>

/* Layout (big-endian):
     0  "SM63WIIS"
     8  u16 version (SAVE_VERSION)
    10  u16 slot count
    12  u32 flags: bit 0 music muted; bits 8..15 current slot
    16  slot records, SAVE_SLOT_BYTES each:
          +0  u8 used, +1 u8 Bowser keys (bit k-1), +2 u16 reserved
          +4  u32 story milestones
          +8  u64 shines (bit n-1 = Star[n])
          +16 u64 Star Coins (bit n-1 = StarCoin[n])
          +24 u64 achievements unlocked (bit id-1)
          +32 32 bytes reserved (zero)
    end u32 CRC-32 of everything before it */

uint32_t save_crc32(const unsigned char *data, size_t len)
{
    uint32_t c = 0xFFFFFFFFu;
    for (size_t i = 0; i < len; ++i) {
        c ^= data[i];
        for (int k = 0; k < 8; ++k) c = (c >> 1) ^ (0xEDB88320u & (0u - (c & 1)));
    }
    return ~c;
}
static void put16(unsigned char *p, unsigned v) { p[0] = (unsigned char)(v >> 8); p[1] = (unsigned char)v; }
static void put32(unsigned char *p, uint32_t v) { put16(p, v >> 16); put16(p + 2, v & 0xFFFF); }
static void put64(unsigned char *p, uint64_t v) { put32(p, (uint32_t)(v >> 32)); put32(p + 4, (uint32_t)v); }
static unsigned get16(const unsigned char *p) { return (unsigned)p[0] << 8 | p[1]; }
static uint32_t get32(const unsigned char *p) { return (uint32_t)get16(p) << 16 | get16(p + 2); }
static uint64_t get64(const unsigned char *p) { return (uint64_t)get32(p) << 32 | get32(p + 4); }

size_t save_encode(const SaveData *s, unsigned char out[SAVE_BYTES])
{
    memset(out, 0, SAVE_BYTES);
    memcpy(out, SAVE_MAGIC, 8);
    put16(out + 8, SAVE_VERSION);
    put16(out + 10, SAVE_SLOTS);
    put32(out + 12, (s->music_muted ? 1u : 0u) | (uint32_t)(s->current % SAVE_SLOTS) << 8);
    for (unsigned i = 0; i < SAVE_SLOTS; ++i) {
        unsigned char *r = out + SAVE_HEADER_BYTES + i * SAVE_SLOT_BYTES;
        const SaveSlot *sl = &s->slot[i];
        r[0] = sl->used;
        r[1] = sl->progress.keys & 7;
        put32(r + 4, sl->progress.milestones);
        put64(r + 8, sl->progress.shines);
        put64(r + 16, sl->progress.star_coins);
        put64(r + 24, sl->progress.unlocked);
    }
    put32(out + SAVE_BYTES - 4, save_crc32(out, SAVE_BYTES - 4));
    return SAVE_BYTES;
}
bool save_decode(const unsigned char *data, size_t len, SaveData *out)
{
    memset(out, 0, sizeof *out);
    if (len < SAVE_HEADER_BYTES + 4 || memcmp(data, SAVE_MAGIC, 8)) return false;
    const unsigned version = get16(data + 8), slots = get16(data + 10);
    if (version < 1 || version > 0xFF) return false;          /* 0x100 and up: an incompatible format */
    const size_t size = SAVE_HEADER_BYTES + (size_t)slots * SAVE_SLOT_BYTES + 4;
    if (slots == 0 || len < size || get32(data + size - 4) != save_crc32(data, size - 4)) return false;
    const uint32_t flags = get32(data + 12);
    out->music_muted = flags & 1;
    out->current = (unsigned char)((flags >> 8 & 0xFF) % SAVE_SLOTS);
    for (unsigned i = 0; i < slots && i < SAVE_SLOTS; ++i) {
        const unsigned char *r = data + SAVE_HEADER_BYTES + i * SAVE_SLOT_BYTES;
        SaveSlot *sl = &out->slot[i];
        sl->used = r[0] != 0;
        sl->progress.keys = r[1] & 7;
        sl->progress.milestones = get32(r + 4);
        sl->progress.shines = get64(r + 8);
        sl->progress.star_coins = get64(r + 16);
        sl->progress.unlocked = get64(r + 24);
    }
    return true;
}
