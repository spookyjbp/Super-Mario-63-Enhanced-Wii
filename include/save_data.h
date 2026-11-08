#ifndef SM63_SAVE_DATA_H
#define SM63_SAVE_DATA_H
/* The save file (docs/achievements.md, "Save format"). Pure: encode / decode
   bytes; source/platform/save_io.c writes them to the SD card. Big-endian,
   versioned, CRC-32 checked; unknown newer fields are kept zero by older
   readers and older files load with the new fields zero. */
#include "achievements.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#define SAVE_MAGIC "SM63WIIS"
#define SAVE_VERSION 1
#define SAVE_SLOTS 3            /* the original's three files (_root["File" + n + ...]) */
#define SAVE_SLOT_BYTES 64
#define SAVE_HEADER_BYTES 16    /* magic, u16 version, u16 slots, u32 flags */
#define SAVE_BYTES (SAVE_HEADER_BYTES + SAVE_SLOTS * SAVE_SLOT_BYTES + 4)

typedef struct {
    bool used;
    AchProgress progress;
} SaveSlot;

typedef struct {
    SaveSlot slot[SAVE_SLOTS];
    unsigned char current;      /* the slot being played */
    bool music_muted;           /* pause menu Options: Mute Music */
} SaveData;

uint32_t save_crc32(const unsigned char *data, size_t len);
/* Always SAVE_BYTES. */
size_t save_encode(const SaveData *s, unsigned char out[SAVE_BYTES]);
/* false: not a save, damaged (CRC), or from a newer incompatible major format;
   `out` is left cleared then. */
bool save_decode(const unsigned char *data, size_t len, SaveData *out);

#ifdef __cplusplus
}
#endif
#endif
