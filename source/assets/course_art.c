#include "course_art.h"
#include <math.h>
#include <string.h>

static const char *const kinds[ART_LAYER_KIND_COUNT] =
    {"FarBackground", "Background", "MidBackground", "CloseBackground", "BackGFX", "FrontGFX", "Picture"};
static uint32_t be32(const unsigned char *p) {return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|p[3];}
static float bef(const unsigned char *p) {uint32_t v=be32(p);float f;memcpy(&f,&v,4);return f;}
typedef struct { const unsigned char *p, *base, *end; bool ok; } R;
static bool need(R *r,size_t n) {if(!r->ok || (size_t)(r->end-r->p)<n) r->ok=false;return r->ok;}
static uint32_t u32(R *r) {if(!need(r,4)) return 0;uint32_t v=be32(r->p);r->p+=4;return v;}
static uint32_t u16(R *r) {if(!need(r,2)) return 0;uint32_t v=(uint32_t)(r->p[0]<<8|r->p[1]);r->p+=2;return v;}
static float f(R *r) {if(!need(r,4)) return 0;float v=bef(r->p);r->p+=4;if(!isfinite(v)) r->ok=false;return v;}
static CourseString str(R *r)
{ CourseString s={0};const uint32_t n=u16(r);if(need(r,n)) {s.text=(const char *)r->p;s.length=(uint16_t)n;r->p+=n;} return s; }

bool course_art_parse(const unsigned char *d,size_t n,CourseArtPack *pack)
{
    memset(pack,0,sizeof(*pack));
    R r={d,d,d+n,true};
    if(n<20 || memcmp(d,"SM63CAR1",8) || be32(d+8)!=1) return false;
    r.p+=12;
    pack->layer_count=u32(&r);pack->zoom=f(&r);
    if(!r.ok || pack->layer_count>COURSE_ART_MAX_LAYERS || !(pack->zoom>0)) return false;
    for(unsigned i=0;i<pack->layer_count;++i) {
        CourseArtLayer *L=&pack->layers[i];
        L->area=str(&r);L->name=str(&r);L->kind=ART_LAYER_KIND_COUNT;
        for(unsigned k=0;k<ART_LAYER_KIND_COUNT;++k) if(course_string_equals(L->name,kinds[k])) L->kind=(CourseArtKind)k;
        if(course_string_equals(L->area,"*")) L->kind=ART_LAYER_PICTURE;
        L->ox=f(&r);L->oy=f(&r);L->parallax=f(&r);L->zoom=f(&r);L->width=u32(&r);L->height=u32(&r);
        L->tile_count=u32(&r);
        if(!r.ok || L->kind==ART_LAYER_KIND_COUNT || L->tile_count>COURSE_ART_MAX_TILES || L->width%4 || L->height%4) return false;
        for(unsigned t=0;t<L->tile_count;++t) {
            CourseArtTile *T=&L->tiles[t];
            T->x=u16(&r);T->y=u16(&r);T->width=u16(&r);T->height=u16(&r);T->size=u32(&r);T->packed=u32(&r);
            if(!r.ok || !T->width || !T->height || T->width>1024 || T->height>1024 || T->width%4 || T->height%4 ||
               T->x+T->width>L->width || T->y+T->height>L->height || T->size!=T->width*T->height*2 || !need(&r,T->packed))
                return false;
            T->data=r.p;r.p+=T->packed;
            while((size_t)(r.p-r.base)%4) {if(!need(&r,1)) return false;++r.p;}
        }
    }
    return r.ok;
}
const CourseArtLayer *course_art_layer(const CourseArtPack *p,const char *area,CourseArtKind kind)
{
    for(unsigned i=0;i<p->layer_count;++i)
        if(p->layers[i].kind==kind && course_string_equals(p->layers[i].area,area)) return &p->layers[i];
    return NULL;
}
const CourseArtLayer *course_art_picture(const CourseArtPack *p,const char *label,size_t length)
{
    for(unsigned i=0;i<p->layer_count;++i) {
        const CourseArtLayer *L=&p->layers[i];
        if(L->kind==ART_LAYER_PICTURE && L->name.length==length && !memcmp(L->name.text,label,length)) return L;
    }
    return NULL;
}

void course_layer_tile_quad(float bx, float by, float px, unsigned x, unsigned y, unsigned w, unsigned h,
                            float xs[4], float ys[4])
{
    const float x0 = bx + (float)x * px, x1 = bx + (float)(x + w) * px;
    const float y0 = by + (float)y * px, y1 = by + (float)(y + h) * px;
    xs[0] = x0; xs[1] = x1; xs[2] = x1; xs[3] = x0;
    ys[0] = y0; ys[1] = y0; ys[2] = y1; ys[3] = y1;
}

const CourseArtLayer *course_art_controller_picture(const CourseArtPack *pack, CourseString label, unsigned controller)
{
    char key[64];
    if ((size_t)label.length + 3 <= sizeof(key) && controller < 10) {
        memcpy(key, label.text, label.length);
        key[label.length] = '@'; key[label.length + 1] = (char)('0' + controller);
        const CourseArtLayer *l = course_art_picture(pack, key, label.length + 2);
        if (l) return l;
    }
    return course_art_picture(pack, label.text, label.length);
}

unsigned course_water_quads(float x, float y, float w, float h, float scale, float seconds, CourseWaterQuad out[2])
{
    /* tools/make_water_texture.py: body 64x64, surface 64x16 x 8 frames stacked. */
    const float body = 64, sw = 64, sh = 16, frames = 8;
    unsigned n = 0;
    const float cw = w / scale, ch = h / scale;     /* course px */
    if (cw <= 0 || ch <= 0) return 0;
    const float top = fminf(11, ch);
    if (ch > top) {
        const float du = fmodf(seconds * 3.0f, body) / body, dv = fmodf(seconds * 1.5f, body) / body; /* slow drift */
        out[n++] = (CourseWaterQuad){x, y + top * scale, x + w, y + h, du, dv, du + cw / body, dv + (ch - top) / body};
    }
    const unsigned f = (unsigned)(fmaxf(0, seconds) * 8) % (unsigned)frames;
    const float du = -fmodf(seconds * 6.0f, sw) / sw;
    const float rows = fminf(sh - 0.5f, 5 + top);  /* texture rows shown: one per course px */
    const float v0 = ((float)f * sh + 0.5f) / (sh * frames), v1 = ((float)f * sh + rows) / (sh * frames);
    out[n++] = (CourseWaterQuad){x, y - 5 * scale, x + w, y + top * scale, du, v0, du + cw / sw, v1};
    return n;
}
