#include "lz4w.h"
#include <gccore.h>

void *mem2_alloc(size_t size)
{
    unsigned char *lo = (unsigned char *)(((uintptr_t)SYS_GetArena2Lo() + 31) & ~(uintptr_t)31);
    if ((unsigned char *)SYS_GetArena2Hi() - lo < (ptrdiff_t)size) return NULL;
    SYS_SetArena2Lo(lo + ((size + 31) & ~(size_t)31));
    return lo;
}
