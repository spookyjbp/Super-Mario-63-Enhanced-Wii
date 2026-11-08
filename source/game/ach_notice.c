#include "ach_notice.h"
#include "sfx.h"

bool ach_notice_step(AchNotice *n, Achievements *a)
{
    if (n->id && ++n->t >= ACH_NOTICE_TICKS) n->id = 0;
    if (n->id || !a) return false;
    const unsigned next = ach_peek(a);
    if (!next) return false;
    ach_pop(a);
    n->id = next; n->t = 0;
    sfx_play(SFX_ACHIEVEMENT, 70, 0);   /* once per card; effect voices leave the music alone */
    return true;
}
static float ease_out(float x) { x = 1 - x; return 1 - x * x * x; }
float ach_notice_slide(const AchNotice *n)
{
    if (!n->id) return 0;
    const unsigned t = n->t;
    if (t < ACH_NOTICE_IN) return ease_out((float)(t + 1) / ACH_NOTICE_IN);
    if (t < ACH_NOTICE_IN + ACH_NOTICE_HOLD) return 1;
    const unsigned o = t - ACH_NOTICE_IN - ACH_NOTICE_HOLD;
    if (o < ACH_NOTICE_OUT) return 1 - ease_out((float)(o + 1) / ACH_NOTICE_OUT);
    return 0;
}
