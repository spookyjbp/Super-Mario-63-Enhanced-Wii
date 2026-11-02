/* Text signs (option 1): HTML stripping, word wrap and widths in the original fonts. */
#include "text_layout.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static void test_plain(void)
{
    char out[256];
    const char *a = "Oh!  You can pause by pressing <b>START</b>.";
    unsigned n = text_plain(a, strlen(a), out, sizeof out);
    assert(n == strlen("Oh!  You can pause by pressing START.") && !strcmp(out, "Oh!  You can pause by pressing START."));
    const char *b = "<p align=\"center\">Line one</p><p>Two &amp; three</p>";
    n = text_plain(b, strlen(b), out, sizeof out);
    assert(!strcmp(out, "Line one\nTwo & three"));
    const char *c = "Inside the Castle:\r<i>Main Hall</i>";
    text_plain(c, strlen(c), out, sizeof out);
    assert(!strcmp(out, "Inside the Castle:\nMain Hall"));
    assert(text_plain("abcdef", 6, out, 4) == 3 && !strcmp(out, "abc"));   /* capacity respected */
}
static void test_wrap(void)
{
    const FontInfo *f = &font_info[FONT_SIGN];
    const char *s = "You can run by double tapping Right or Left.\nIn the air you can press Down to Lunge or Dive forwards";
    TextLine l[16];
    /* the message box text field: 346.9 stage px - 4 px gutters, x 1.6 */
    const float w = (346.85f - 4) * 1.6f;
    const unsigned n = text_wrap(f, s, (unsigned)strlen(s), w, l, 16);
    assert(n >= 3);
    for (unsigned i = 0; i < n; ++i) {
        assert(l[i].width <= w && l[i].end >= l[i].start);
        if (l[i].end > l[i].start) assert(s[l[i].start] != ' ' && s[l[i].end - 1] != ' ');
    }
    assert(l[0].start == 0 && !memcmp(s, "You can run by double tapping", 29));
    bool para = false;                                               /* the paragraph break starts a line */
    for (unsigned i = 0; i < n; ++i) para |= l[i].start == 45;
    assert(para);
    /* a word wider than the field breaks by character */
    const char *longw = "Mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm";
    const unsigned m = text_wrap(f, longw, (unsigned)strlen(longw), 200, l, 16);
    assert(m > 1 && l[0].width <= 200 && l[0].end > l[0].start);
    /* widths add up from the advances */
    assert(text_width(f, "AB", 2) == f->glyphs['A' - FONT_FIRST].advance + f->glyphs['B' - FONT_FIRST].advance);
    /* every printable glyph has an advance; letters have pixels */
    for (int c = FONT_FIRST; c <= FONT_LAST; ++c) assert(f->glyphs[c - FONT_FIRST].advance > 0);
    assert(f->glyphs['q' - FONT_FIRST].w > 0);   /* 155 lacks 'q': the fallback glyph */
    assert(text_line_height(f, 3.2f) > f->size);
}
int main(void)
{
    test_plain(); test_wrap();
    puts("2 text layout (HTML, wrap, original fonts) test groups passed"); return 0;
}
