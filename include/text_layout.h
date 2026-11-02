#ifndef SM63_TEXT_LAYOUT_H
#define SM63_TEXT_LAYOUT_H
/* Text in the original's fonts (assets/generated/font_metrics.h, from
   tools/export_fonts.py), laid out like the Flash text fields: HTML tags
   dropped, paragraphs on new lines, word wrap at spaces (words wider than the
   field break by character), each line centred by the caller. Pure: host tested. */
#include <stdbool.h>
#include <stddef.h>
#include "font_metrics.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct { unsigned start, end; float width; } TextLine;  /* [start, end) in the plain text */
/* HTML-ish source (<b>, <i>, <p>, </p>, <br>, &amp; ...; \r or \n) to plain text with
   '\n' line breaks. Returns the plain length (out is NUL terminated). */
unsigned text_plain(const char *src, size_t len, char *out, unsigned cap);
float text_width(const FontInfo *font, const char *s, unsigned len);
/* Lines of plain text within max_width px; returns the line count (<= max_lines). */
unsigned text_wrap(const FontInfo *font, const char *s, unsigned len, float max_width, TextLine *lines,
                   unsigned max_lines);
/* Flash line height: ascent + descent + leading (px). */
static inline float text_line_height(const FontInfo *font, float leading) { return font->ascent + font->descent + leading; }
#ifdef __cplusplus
}
#endif
#endif
