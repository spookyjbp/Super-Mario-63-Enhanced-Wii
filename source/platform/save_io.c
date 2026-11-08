#include "save_io.h"
#include <fat.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define DIR_PATH "sd:/sm63wii"
#ifdef SM63_DEV   /* developer builds never touch the real save */
#define SAVE_PATH DIR_PATH "/devsave.dat"
#define TEMP_PATH DIR_PATH "/devsave.tmp"
#define BACKUP_PATH DIR_PATH "/devsave.bak"
#else
#define SAVE_PATH DIR_PATH "/save.dat"
#define TEMP_PATH DIR_PATH "/save.tmp"
#define BACKUP_PATH DIR_PATH "/save.bak"
#endif

static bool mounted, tried, readonly;

static bool mount(void)
{
    if (!tried) { tried = true; mounted = fatInitDefault(); }
    return mounted;
}
static bool read_file(const char *path, SaveData *out)
{
    FILE *f = fopen(path, "rb");
    if (!f) return false;
    unsigned char buf[SAVE_BYTES + 512];
    const size_t n = fread(buf, 1, sizeof buf, f);
    fclose(f);
    return save_decode(buf, n, out);
}
SaveIoResult save_io_load(SaveData *out)
{
    memset(out, 0, sizeof *out);
    if (!mount()) return SAVE_IO_NO_CARD;
    if (read_file(SAVE_PATH, out)) return SAVE_IO_LOADED;
    if (read_file(BACKUP_PATH, out)) return SAVE_IO_LOADED_BACKUP;
    memset(out, 0, sizeof *out);
    return SAVE_IO_NEW;
}
bool save_io_write(const SaveData *s)
{
    if (readonly) return true;
    if (!mount()) return false;
    mkdir(DIR_PATH, 0777);
    unsigned char buf[SAVE_BYTES];
    const size_t n = save_encode(s, buf);
    FILE *f = fopen(TEMP_PATH, "wb");
    if (!f) return false;
    const bool ok = fwrite(buf, 1, n, f) == n && fflush(f) == 0;
    if (fclose(f) != 0 || !ok) { remove(TEMP_PATH); return false; }
    SaveData check;   /* read it back before replacing anything */
    if (!read_file(TEMP_PATH, &check)) { remove(TEMP_PATH); return false; }
    remove(BACKUP_PATH);
    rename(SAVE_PATH, BACKUP_PATH);      /* fails harmlessly on the first save */
    if (rename(TEMP_PATH, SAVE_PATH) != 0) return false;
    return true;
}
void save_io_set_readonly(bool r) { readonly = r; }
bool save_io_readonly(void) { return readonly; }
