#ifndef SM63_SIGN_LAYOUT_H
#define SM63_SIGN_LAYOUT_H
/* Where the sign prompt and the message text go, in stage px (450 x 300).
   Port decisions (docs/sign-design/README.md), host tested. */
#include <stdbool.h>

#define SIGN_STAGE_H 300.0f
#define SIGN_PROMPT_GAP 4.0f      /* stage px between the page and the prompt below it */
#define SIGN_TEXT_GAP 6.0f        /* stage px kept above the prompt by the message text */

/* The prompt image's top-left on a picture sign: centred under the page (x0, y0,
   w, h: the page's rectangle), its top SIGN_PROMPT_GAP below the page, when it
   fits on the screen; otherwise (the Welcome page fills the screen) the
   original's own place (reg_x, reg_y: 250's registration; ox, oy: the image's
   offset from it). pw, ph: the prompt image's size. */
static inline void sign_prompt_picture(float x0, float y0, float w, float h, float pw, float ph,
                                       float reg_x, float reg_y, float ox, float oy, float *x, float *y)
{
    const float top = y0 + h + SIGN_PROMPT_GAP;
    if (top + ph <= SIGN_STAGE_H - 2) { *x = x0 + w / 2 - pw / 2; *y = top; }
    else { *x = reg_x + ox; *y = reg_y + oy; }
}
/* A page with no prompt of its own in the original that also leaves no room
   under it: the registration that puts the prompt centred on the page at the
   screen's bottom edge (for sign_prompt_picture's fallback). */
static inline void sign_prompt_fallback(float x0, float w, float pw, float ph, float ox, float oy, float *reg_x, float *reg_y)
{
    *reg_x = x0 + w / 2 - pw / 2 - ox;
    *reg_y = SIGN_STAGE_H - 2 - ph - oy;
}
/* The first line's top for a message of `lines` lines (line height lh): centred
   between the text field's top (top) and SIGN_TEXT_GAP above the prompt
   (prompt_top); text too tall for that starts at the field's top, as the
   original does. */
static inline float sign_text_top(float top, float prompt_top, unsigned lines, float lh)
{
    const float avail = prompt_top - SIGN_TEXT_GAP - top, block = (float)lines * lh;
    return block >= avail ? top : top + (avail - block) / 2;
}
/* The frame of an animated picture sign: its clips loop from the page's opening. */
static inline unsigned sign_anim_frame(unsigned sign_frames, unsigned frames) { return frames ? sign_frames % frames : 0; }
#endif
