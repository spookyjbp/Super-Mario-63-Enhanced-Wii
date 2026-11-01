#include "lz4_block.h"
#include <string.h>

long lz4_block_decode(const uint8_t *s, size_t n, uint8_t *d, size_t cap)
{
    size_t i = 0, o = 0;
    while (i < n) {
        const unsigned token = s[i++];
        size_t lit = token >> 4;
        if (lit == 15) {
            unsigned b;
            do { if (i >= n) return -1; b = s[i++]; lit += b; } while (b == 255);
        }
        if (lit > n - i || lit > cap - o) return -1;
        memcpy(d + o, s + i, lit); i += lit; o += lit;
        if (i == n) break; /* last sequence has literals only */
        if (n - i < 2) return -1;
        const size_t off = s[i] | (s[i + 1] << 8); i += 2;
        if (!off || off > o) return -1;
        size_t len = (token & 15) + 4;
        if ((token & 15) == 15) {
            unsigned b;
            do { if (i >= n) return -1; b = s[i++]; len += b; } while (b == 255);
        }
        if (len > cap - o) return -1;
        for (size_t k = 0; k < len; ++k, ++o) d[o] = d[o - off]; /* overlapping copy */
    }
    return (long)o;
}
