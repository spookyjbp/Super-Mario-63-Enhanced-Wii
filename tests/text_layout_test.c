/* Text signs (option 1): HTML stripping, word wrap and widths in the original fonts. */
#include "text_layout.h"
#include "sign_layout.h"
#include <assert.h>
#include <math.h>
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
static void test_hint_font(void)
{
    /* The hints use font 187 (regular Arial) at 11 x 1.6, narrower than the bold talk font */
    const FontInfo *h = &font_info[FONT_HINT], *t = &font_info[FONT_TALK];
    assert(h->size == 18 && t->size == 24);
    const FontInfo *sg = &font_info[FONT_SIGN];
    assert(h->glyphs['m' - FONT_FIRST].advance < sg->glyphs['m' - FONT_FIRST].advance * 18 / 27);
}
static void test_talk_font(void)
{
    /* field 185: letterSpacing -1 (x 1.6 off every advance against the sign's
       bold 155 at the same scale); the Bevel + Glow glyphs share its layout */
    const FontInfo *t = &font_info[FONT_TALK], *sg = &font_info[FONT_SIGN];
    const float m24 = sg->glyphs['m' - FONT_FIRST].advance * 24 / 27;
    assert(fabsf(t->glyphs['m' - FONT_FIRST].advance - (m24 - 1.6f)) < 0.6f);
    for (unsigned k = FONT_TALK_FX; k <= FONT_TALK_FILL; ++k) {
        const FontInfo *f = &font_info[k];
        assert(f->ascent == t->ascent && f->descent == t->descent);
        for (unsigned c = FONT_FIRST; c <= FONT_LAST; ++c)
            assert(f->glyphs[c - FONT_FIRST].advance == t->glyphs[c - FONT_FIRST].advance);
        /* the baked glyph is bigger than the plain one: the outline around it */
        assert(f->glyphs['a' - FONT_FIRST].w > t->glyphs['a' - FONT_FIRST].w);
    }
    /* Flash lays out with DefineFont3's ascent / descent (0.905 / 0.212 em):
       field 185's lines are 30 px apart at 1.6 (24 px + leading 2 x 1.6) */
    assert(fabsf(text_line_height(t, 2 * 1.6f) - 30.0f) < 0.1f);
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
static void test_sign_layout(void)
{
    /* the prompt goes under a picture page, centred, when it fits ... */
    float x, y;
    sign_prompt_picture(100, 60, 300, 180, 200, 25, 222, 163, -100, -10, &x, &y);
    assert(x == 150 && y == 60 + 180 + SIGN_PROMPT_GAP);
    /* ... else (a page filling the screen) at the original's place */
    sign_prompt_picture(50, 20, 350, 260, 200, 25, 225, 260, -100, -10, &x, &y);
    assert(x == 125 && y == 250);
    /* message text: centred between the field top and the prompt, or from the top when tall */
    const float top = sign_text_top(30, 260, 2, 20);
    assert(top > 30 && fabsf((top - 30) - (260 - SIGN_TEXT_GAP - (top + 40))) < 1e-3f);
    assert(sign_text_top(30, 260, 20, 20) == 30);
    /* a page without its own prompt that fills the screen: centred on it at the bottom edge */
    float rx, ry;
    sign_prompt_fallback(50, 350, 200, 25, -100, -10, &rx, &ry);
    sign_prompt_picture(50, 20, 350, 270, 200, 25, rx, ry, -100, -10, &x, &y);
    assert(x == 125 && y + 25 == SIGN_STAGE_H - 2);
}

int main(void)
{
    test_plain(); test_wrap(); test_hint_font(); test_talk_font(); test_sign_layout();
    puts("4 text layout (HTML, wrap, original fonts, talk font, sign layout) test groups passed"); return 0;
}
