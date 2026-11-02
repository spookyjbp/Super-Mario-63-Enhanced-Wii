#include "text_layout.h"
#include <string.h>

static const FontGlyph *glyph(const FontInfo *f, char c)
{
    const unsigned char u = (unsigned char)c;
    return u >= FONT_FIRST && u <= FONT_LAST ? &f->glyphs[u - FONT_FIRST] : NULL;
}
float text_width(const FontInfo *f, const char *s, unsigned len)
{
    float w = 0;
    for (unsigned i = 0; i < len; ++i) { const FontGlyph *g = glyph(f, s[i]); if (g) w += g->advance; }
    return w;
}
static bool starts(const char *s, size_t n, const char *t)
{
    const size_t k = strlen(t);
    if (n < k) return false;
    for (size_t i = 0; i < k; ++i) {
        char a = s[i];
        if (a >= 'A' && a <= 'Z') a = (char)(a - 'A' + 'a');
        if (a != t[i]) return false;
    }
    return true;
}
unsigned text_plain(const char *src, size_t len, char *out, unsigned cap)
{
    unsigned n = 0;
    if (!cap) return 0;
    for (size_t i = 0; i < len && n + 1 < cap; ++i) {
        const char c = src[i];
        if (c == '<') {
            size_t j = i;
            while (j < len && src[j] != '>') ++j;
            /* a paragraph end or <br> breaks the line (not a leading one) */
            if ((starts(src + i, len - i, "</p") || starts(src + i, len - i, "<br")) && n && out[n - 1] != '\n')
                out[n++] = '\n';
            i = j;
            continue;
        }
        if (c == '&') {
            static const struct { const char *name; char c; } ent[] = {
                {"&amp;", '&'}, {"&lt;", '<'}, {"&gt;", '>'}, {"&quot;", '"'}, {"&apos;", '\''}, {"&nbsp;", ' '}};
            bool done = false;
            for (unsigned e = 0; e < sizeof(ent) / sizeof(ent[0]) && !done; ++e)
                if (starts(src + i, len - i, ent[e].name)) { out[n++] = ent[e].c; i += strlen(ent[e].name) - 1; done = true; }
            if (done) continue;
        }
        if (c == '\r') { if (i + 1 < len && src[i + 1] == '\n') ++i; out[n++] = '\n'; continue; }
        out[n++] = c;
    }
    while (n && out[n - 1] == '\n') --n;
    out[n] = 0;
    return n;
}
unsigned text_wrap(const FontInfo *f, const char *s, unsigned len, float max_width, TextLine *lines, unsigned max_lines)
{
    unsigned count = 0, i = 0;
    while (i <= len && count < max_lines) {
        /* one paragraph: [i, p) */
        unsigned p = i;
        while (p < len && s[p] != '\n') ++p;
        unsigned start = i;
        if (start == p) { lines[count++] = (TextLine){start, start, 0}; }
        while (start < p && count < max_lines) {
            unsigned end = start, last_space = 0;
            bool have_space = false;
            float w = 0;
            while (end < p) {
                const FontGlyph *g = glyph(f, s[end]);
                const float a = g ? g->advance : 0;
                if (s[end] == ' ') { last_space = end; have_space = true; }
                if (w + a > max_width && end > start && s[end] != ' ') break;
                w += a; ++end;
            }
            if (end < p && have_space && last_space > start) end = last_space; /* break at the last space */
            unsigned e = end;
            while (e > start && s[e - 1] == ' ') --e;                         /* trailing spaces don't count */
            lines[count++] = (TextLine){start, e, text_width(f, s + start, e - start)};
            start = end;
            while (start < p && s[start] == ' ') ++start;                      /* nor leading ones */
        }
        i = p + 1;
    }
    return count;
}
