#ifndef SM63_SOUND_PACK_H
#define SM63_SOUND_PACK_H
/* IMA ADPCM sound pack written by tools/export_audio.py ("SM63SND1"). */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#define SOUND_PACK_MAX 64
typedef struct { unsigned rate, samples; const unsigned char *data; size_t size; } PackedSound;
typedef struct { unsigned count; PackedSound sound[SOUND_PACK_MAX]; } SoundPack;
bool sound_pack_parse(const unsigned char *data, size_t size, SoundPack *pack);
/* Decode one sound into `samples` 16-bit native-endian PCM samples. */
bool sound_pack_decode(const PackedSound *s, int16_t *out);
#ifdef __cplusplus
}
#endif
#endif
