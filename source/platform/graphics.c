#include "graphics.h"

#include <malloc.h>
#include <string.h>

#define FIFO_SIZE (256 * 1024)

static GXRModeObj mode;
static void *framebuffers[2];
static void *fifo;
static unsigned int current_frame;
static float logical_width;

bool graphics_init(void)
{
    VIDEO_Init();
    mode = *VIDEO_GetPreferredMode(NULL);
    const bool widescreen = CONF_GetAspectRatio() == CONF_ASPECT_16_9;
    logical_width = widescreen ? 854.0f : 640.0f;
    if (widescreen) {
        mode.viWidth = 678;
        mode.viXOrigin = (VI_MAX_WIDTH_NTSC - mode.viWidth) / 2;
    }

    for (unsigned int i = 0; i < 2; ++i) {
        void *buffer = SYS_AllocateFramebuffer(&mode);
        if (!buffer) return false;
        framebuffers[i] = MEM_K0_TO_K1(buffer);
        VIDEO_ClearFrameBuffer(&mode, framebuffers[i], COLOR_BLACK);
    }
    fifo = memalign(32, FIFO_SIZE);
    if (!fifo) return false;
    memset(fifo, 0, FIFO_SIZE);
    GX_Init(fifo, FIFO_SIZE);

    VIDEO_Configure(&mode);
    VIDEO_SetNextFramebuffer(framebuffers[0]);
    VIDEO_SetBlack(false);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if (mode.viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

    GX_SetCopyClear((GXColor){100, 177, 235, 255}, 0x00ffffff);
    GX_SetViewport(0, 0, mode.fbWidth, mode.efbHeight, 0, 1);
    GX_SetScissor(0, 0, mode.fbWidth, mode.efbHeight);
    const float scale = GX_GetYScaleFactor(mode.efbHeight, mode.xfbHeight);
    const u32 height = GX_SetDispCopyYScale(scale);
    GX_SetDispCopySrc(0, 0, mode.fbWidth, mode.efbHeight);
    GX_SetDispCopyDst(mode.fbWidth, height);
    GX_SetCopyFilter(mode.aa, mode.sample_pattern, GX_TRUE, mode.vfilter);
    GX_SetFieldMode(mode.field_rendering,
                   mode.viHeight == 2 * mode.xfbHeight ? GX_ENABLE : GX_DISABLE);
    GX_SetDispCopyGamma(GX_GM_1_0);
    GX_SetCullMode(GX_CULL_NONE);
    GX_SetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    GX_SetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
    GX_SetColorUpdate(GX_TRUE);
    GX_SetNumChans(1);
    GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, 0,
                   GX_DF_NONE, GX_AF_NONE);
    GX_SetNumTexGens(0);
    GX_SetNumTevStages(1);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GX_ClearVtxDesc();
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

    Mtx view;
    Mtx44 projection;
    guMtxIdentity(view);
    GX_LoadPosMtxImm(view, GX_PNMTX0);
    GX_SetCurrentMtx(GX_PNMTX0);
    guOrtho(projection, 0, 480, 0, logical_width, 0, 1);
    GX_LoadProjectionMtx(projection, GX_ORTHOGRAPHIC);
    GX_CopyDisp(framebuffers[0], GX_TRUE);
    GX_DrawDone();
    return true;
}

float graphics_width(void) { return logical_width; }

void graphics_rect(float x, float y, float width, float height, GXColor color)
{
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
    GX_Position3f32(x, y, 0);                GX_Color4u8(color.r, color.g, color.b, color.a);
    GX_Position3f32(x + width, y, 0);        GX_Color4u8(color.r, color.g, color.b, color.a);
    GX_Position3f32(x + width, y + height, 0); GX_Color4u8(color.r, color.g, color.b, color.a);
    GX_Position3f32(x, y + height, 0);       GX_Color4u8(color.r, color.g, color.b, color.a);
    GX_End();
}

void graphics_present(void)
{
    GX_DrawDone();
    current_frame ^= 1;
    GX_CopyDisp(framebuffers[current_frame], GX_TRUE);
    GX_Flush();
    VIDEO_SetNextFramebuffer(framebuffers[current_frame]);
    VIDEO_Flush();
    VIDEO_WaitVSync();
}

void graphics_shutdown(void)
{
    GX_DrawDone();
    VIDEO_SetBlack(true);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    /* Video/GX own the buffers until the loader takes over. Do not free them
       while the hardware can still reference them. */
}
