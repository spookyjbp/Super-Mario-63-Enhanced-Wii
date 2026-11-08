#ifndef SM63_DATA_FILES_H
#define SM63_DATA_FILES_H
/* The game's data files (user decision 2026-11-08): the large art packs (the
   front end and its cutscenes, the courses' art) are not in boot.dol but in
   the app's data folder on the SD card or USB drive, next to boot.dol:
       apps/super-mario-63-enhanced-wii/boot.dol
       apps/super-mario-63-enhanced-wii/data/front_end.bin ...
   The folder is found from the path the Homebrew Channel started the game
   with (argv[0]), else in the usual places (DATA_FOLDERS). */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DATA_APP "super-mario-63-enhanced-wii"

/* Mounts the SD card / USB drive (once; the save file uses the same mount)
   and finds the data folder. */
bool data_init(int argc, char **argv);
/* The data folder found ("sd:/apps/.../data"), or NULL. */
const char *data_folder(void);
/* Reads data/<name> whole into a 32-byte aligned buffer in MEM1 (free with
   data_free); NULL when it is missing or cannot be read. */
uint8_t *data_load(const char *name, size_t *size);
void data_free(uint8_t *data);
/* Mount the SD card / USB drive once (true: mounted). */
bool data_mount(void);
#endif
