#ifndef SM63_SAVE_IO_H
#define SM63_SAVE_IO_H
/* The save file on the SD card (sd:/sm63wii/save.dat). Writes go to save.tmp
   first; the old file becomes save.bak, then save.tmp becomes save.dat, so a
   power cut leaves either the old or the new file whole. Loading falls back to
   save.bak when save.dat is missing or damaged. */
#include "save_data.h"
#include <stdbool.h>

typedef enum { SAVE_IO_NONE, SAVE_IO_LOADED, SAVE_IO_LOADED_BACKUP, SAVE_IO_NEW, SAVE_IO_NO_CARD } SaveIoResult;

SaveIoResult save_io_load(SaveData *out);
bool save_io_write(const SaveData *s);
/* Developer test mode: writes are dropped so real saves are never changed. */
void save_io_set_readonly(bool readonly);
bool save_io_readonly(void);
#endif
