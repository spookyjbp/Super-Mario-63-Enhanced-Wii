#include "graphics.h"

#include <malloc.h>
#include <math.h>
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

/* Tiny built-in 5x7 placeholder font, column bits from top to bottom. */
void graphics_text(float x, float y, const char *text, float scale, GXColor color)
{
    static const u8 letters[26][5] = {
        {126,17,17,17,126}, {127,73,73,73,54}, {62,65,65,65,34},
        {127,65,65,34,28}, {127,73,73,73,65}, {127,9,9,9,1},
        {62,65,73,73,122}, {127,8,8,8,127}, {0,65,127,65,0},
        {32,64,65,63,1}, {127,8,20,34,65}, {127,64,64,64,64},
        {127,2,12,2,127}, {127,4,8,16,127}, {62,65,65,65,62},
        {127,9,9,9,6}, {62,65,81,33,94}, {127,9,25,41,70},
        {70,73,73,73,49}, {1,1,127,1,1}, {63,64,64,64,63},
        {31,32,64,32,31}, {63,64,56,64,63}, {99,20,8,20,99},
        {7,8,112,8,7}, {97,81,73,69,67}
    };
    static const u8 digits[10][5] = {
        {62,81,73,69,62}, {0,66,127,64,0}, {66,97,81,73,70},
        {33,65,69,75,49}, {24,20,18,127,16}, {39,69,69,69,57},
        {60,74,73,73,48}, {1,113,9,5,3}, {54,73,73,73,54}, {6,73,73,41,30}
    };
    for (; *text; ++text, x += 6 * scale) {
        const u8 *glyph = NULL;
        if (*text >= 'A' && *text <= 'Z') glyph = letters[*text - 'A'];
        else if (*text >= '0' && *text <= '9') glyph = digits[*text - '0'];
        else if (*text == '.') graphics_rect(x + scale, y + 6 * scale, scale, scale, color);
        if (!glyph) continue;
        for (unsigned column = 0; column < 5; ++column)
            for (unsigned row = 0; row < 7; ++row)
                if (glyph[column] & (1 << row))
                    graphics_rect(x + column * scale, y + row * scale, scale, scale, color);
    }
}


void graphics_sprite_rotated(GXTexObj *texture, float x, float y, float width, float height,
                     float u0, float v0, float u1, float v1, float degrees)
{
    GX_SetNumTexGens(1);
    GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
    GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
    GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GX_SetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GX_LoadTexObj(texture, GX_TEXMAP0);
    const float radians=degrees*0.01745329252f, c=cosf(radians), s=sinf(radians);
    const float cx=x+width/2, cy=y+height/2;
    const float corners[4][2]={{-width/2,-height/2},{width/2,-height/2},{width/2,height/2},{-width/2,height/2}};
    float vx[4],vy[4];
    for(unsigned i=0;i<4;++i) {vx[i]=cx+corners[i][0]*c-corners[i][1]*s;vy[i]=cy+corners[i][0]*s+corners[i][1]*c;}
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
    GX_Position3f32(vx[0], vy[0], 0); GX_Color4u8(255,255,255,255); GX_TexCoord2f32(u0, v0);
    GX_Position3f32(vx[1], vy[1], 0); GX_Color4u8(255,255,255,255); GX_TexCoord2f32(u1, v0);
    GX_Position3f32(vx[2], vy[2], 0); GX_Color4u8(255,255,255,255); GX_TexCoord2f32(u1, v1);
    GX_Position3f32(vx[3], vy[3], 0); GX_Color4u8(255,255,255,255); GX_TexCoord2f32(u0, v1);
    GX_End();
    /* Restore the untextured path for terrain, effects, text and pause UI. */
    GX_SetVtxDesc(GX_VA_TEX0, GX_NONE);
    GX_SetNumTexGens(0);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GX_SetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
    GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
}

void graphics_sprite(GXTexObj *t,float x,float y,float w,float h,float u0,float v0,float u1,float v1)
{ graphics_sprite_rotated(t,x,y,w,h,u0,v0,u1,v1,0); }
void graphics_triangle(float x0,float y0,float x1,float y1,float x2,float y2,GXColor color)
{
    GX_Begin(GX_TRIANGLES,GX_VTXFMT0,3);
    GX_Position3f32(x0,y0,0);GX_Color4u8(color.r,color.g,color.b,color.a);
    GX_Position3f32(x1,y1,0);GX_Color4u8(color.r,color.g,color.b,color.a);
    GX_Position3f32(x2,y2,0);GX_Color4u8(color.r,color.g,color.b,color.a);
    GX_End();
}
