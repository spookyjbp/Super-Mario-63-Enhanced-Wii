/* The game's data folder on the SD card / USB drive (data_files.h). */
#include "data_files.h"
#include <fat.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static char folder[256];
static bool found;

bool data_mount(void)
{
    static bool tried, mounted;
    if (!tried) { tried = true; mounted = fatInitDefault(); }
    return mounted;
}
static bool is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}
/* "sd:/apps/x/boot.dol" -> "sd:/apps/x/data" */
static bool from_launch_path(const char *argv0)
{
    if (!argv0 || !strchr(argv0, ':')) return false;
    const char *slash = strrchr(argv0, '/');
    if (!slash) return false;
    const size_t n = (size_t)(slash - argv0);
    if (n + 6 >= sizeof folder) return false;
    memcpy(folder, argv0, n);
    memcpy(folder + n, "/data", 6);
    return is_dir(folder);
}
bool data_init(int argc, char **argv)
{
    if (found) return true;
    if (!data_mount()) return false;
    if (argc > 0 && argv && from_launch_path(argv[0])) return found = true;
    /* Dolphin and loaders that pass no path: the usual app folders */
    static const char *const DATA_FOLDERS[] = {
        "sd:/apps/" DATA_APP "/data", "usb:/apps/" DATA_APP "/data",
        "sd:/apps/sm63wii/data", "usb:/apps/sm63wii/data",
    };
    for (unsigned i = 0; i < sizeof DATA_FOLDERS / sizeof DATA_FOLDERS[0]; ++i)
        if (is_dir(DATA_FOLDERS[i])) {
            snprintf(folder, sizeof folder, "%s", DATA_FOLDERS[i]);
            return found = true;
        }
    return false;
}
const char *data_folder(void) { return found ? folder : NULL; }
uint8_t *data_load(const char *name, size_t *size)
{
    if (!found) return NULL;
    char path[320];
    snprintf(path, sizeof path, "%s/%s", folder, name);
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    uint8_t *buf = NULL;
    long n = -1;
    if (fseek(f, 0, SEEK_END) == 0) n = ftell(f);
    if (n > 0 && fseek(f, 0, SEEK_SET) == 0 && (buf = memalign(32, (size_t)n + 32)) != NULL) {
        if (fread(buf, 1, (size_t)n, f) != (size_t)n) { free(buf); buf = NULL; }
        else if (size) *size = (size_t)n;
    }
    fclose(f);
    return buf;
}
void data_free(uint8_t *data) { free(data); }
