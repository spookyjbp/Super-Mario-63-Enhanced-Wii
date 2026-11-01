#include "course_pack.h"
#include <math.h>
#include <string.h>

static uint16_t u16(const unsigned char *p) {return (uint16_t)((p[0]<<8)|p[1]);}
static uint32_t u32(const unsigned char *p) {return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|p[3];}
static int32_t s32(const unsigned char *p) {return (int32_t)u32(p);}
static float f32(const unsigned char *p) {uint32_t v=u32(p);float f;memcpy(&f,&v,4);return f;}

typedef struct { const unsigned char *p, *end; bool ok; } Reader;
static bool need(Reader *r,size_t n) {if(!r->ok || (size_t)(r->end-r->p)<n) r->ok=false;return r->ok;}
static uint16_t take16(Reader *r) {if(!need(r,2)) return 0;uint16_t v=u16(r->p);r->p+=2;return v;}
static uint32_t take32(Reader *r) {if(!need(r,4)) return 0;uint32_t v=u32(r->p);r->p+=4;return v;}
static float takef(Reader *r) {if(!need(r,4)) return 0;float v=f32(r->p);r->p+=4;return isfinite(v) ? v : (r->ok=false,0);}
static CourseString takes(Reader *r)
{
    CourseString s={0};const uint16_t n=take16(r);
    if(need(r,n)) {s.text=(const char *)r->p;s.length=n;r->p+=n;}
    return s;
}
static bool parse_area(Reader *r,CourseArea *a)
{
    a->id=takes(r);
    a->mask_count=take32(r);
    if(!r->ok || a->mask_count>COURSE_MAX_MASKS) return false;
    for(unsigned i=0;i<a->mask_count;++i) {
        CourseMask *m=&a->masks[i];
        if(!need(r,20)) return false;
        m->layer=(CourseLayer)r->p[0];m->frame=r->p[1];
        m->x0=s32(r->p+4);m->y0=s32(r->p+8);m->width=u32(r->p+12);m->height=u32(r->p+16);r->p+=20;
        if(m->layer>=COURSE_LAYER_COUNT || m->width>65535 || m->height>65535) return false;
        if(!need(r,(size_t)m->height*4)) return false;
        m->offsets=r->p;r->p+=(size_t)m->height*4;
        const uint32_t block=take32(r);
        if(!need(r,block)) return false;
        m->runs=r->p;
        for(unsigned y=0;y<m->height;++y) { /* validate every row once */
            const uint32_t o=u32(m->offsets+y*4);
            if(o+2>block) return false;
            const uint16_t count=u16(m->runs+o);
            if(o+2+(uint32_t)count*4>block) return false;
            unsigned last=0;
            for(unsigned k=0;k<count;++k) {
                const unsigned s=u16(m->runs+o+2+k*4),n=u16(m->runs+o+4+k*4);
                if(!n || s<last || s+n>m->width) return false;
                last=s+n;
            }
        }
        r->p+=block;
    }
    a->water_count=take32(r);
    if(!r->ok || a->water_count>COURSE_MAX_WATER) return false;
    for(unsigned i=0;i<a->water_count;++i)
        a->water[i]=(CourseWater){takef(r),takef(r),takef(r),takef(r)};
    a->object_count=take32(r);
    if(!r->ok || a->object_count>COURSE_MAX_OBJECTS) return false;
    for(unsigned i=0;i<a->object_count;++i) {
        CourseObject *o=&a->objects[i];
        o->type=takes(r);o->name=takes(r);o->layer=takes(r);
        o->x=takef(r);o->y=takef(r);o->sx=takef(r);o->sy=takef(r);
        o->frame_start=take16(r);o->frame_end=take16(r);
        o->param_count=take16(r);o->params=r->p;
        for(unsigned k=0;k<o->param_count*2u;++k) takes(r);
        o->params_end=r->p;
        if(!r->ok || o->frame_end<=o->frame_start) return false;
    }
    return r->ok;
}
bool course_pack_parse(const unsigned char *d,size_t n,CoursePack *pack)
{
    memset(pack,0,sizeof(*pack));
    if(n<16 || memcmp(d,"SM63CRS1",8) || u32(d+8)!=1) return false;
    pack->area_count=u32(d+12);
    if(!pack->area_count || pack->area_count>COURSE_MAX_AREAS) return false;
    Reader r={d+16,d+n,true};
    for(unsigned i=0;i<pack->area_count;++i) {
        const uint32_t size=take32(&r);
        if(!need(&r,size)) return false;
        Reader area={r.p,r.p+size,true};
        if(!parse_area(&area,&pack->areas[i]) || area.p!=area.end) return false;
        r.p+=size;
    }
    return true;
}
bool course_string_equals(CourseString s,const char *t)
{ return s.text && strlen(t)==s.length && !memcmp(s.text,t,s.length); }
const CourseArea *course_pack_area(const CoursePack *p,const char *id)
{
    for(unsigned i=0;i<p->area_count;++i) if(course_string_equals(p->areas[i].id,id)) return &p->areas[i];
    return NULL;
}
const CourseMask *course_area_mask(const CourseArea *a,CourseLayer layer,unsigned frame)
{
    const CourseMask *best=NULL;
    for(unsigned i=0;i<a->mask_count;++i)
        if(a->masks[i].layer==layer && a->masks[i].frame<=frame && (!best || a->masks[i].frame>best->frame))
            best=&a->masks[i]; /* Flash holds the last keyframe for later frames */
    return best;
}
bool course_mask_hit(const CourseMask *m,float x,float y)
{
    if(!m) return false;
    const float fx=floorf(x)-m->x0,fy=floorf(y)-m->y0;
    if(fx<0 || fy<0 || fx>=m->width || fy>=m->height) return false;
    const unsigned px=(unsigned)fx;
    const unsigned char *row=m->runs+u32(m->offsets+(unsigned)fy*4);
    unsigned lo=0,hi=u16(row);
    while(lo<hi) { /* last run whose start <= px */
        const unsigned mid=(lo+hi)/2;
        if(u16(row+2+mid*4)<=px) lo=mid+1; else hi=mid;
    }
    if(!lo) return false;
    const unsigned s=u16(row+2+(lo-1)*4),n=u16(row+4+(lo-1)*4);
    return px<s+n;
}
bool course_area_hit(const CourseArea *a,CourseLayer layer,unsigned frame,float x,float y)
{ return course_mask_hit(course_area_mask(a,layer,frame),x,y); }
bool course_area_water(const CourseArea *a,float x,float y,float *surface)
{
    for(unsigned i=0;i<a->water_count;++i) {
        const CourseWater *w=&a->water[i];
        if(x>=w->x && x<w->x+w->w && y>=w->y && y<w->y+w->h) {if(surface) *surface=w->y;return true;}
    }
    return false;
}
bool course_object_param(const CourseObject *o,const char *key,CourseString *value)
{
    Reader r={o->params,o->params_end,true};
    for(unsigned k=0;k<o->param_count;++k) {
        const CourseString name=takes(&r),v=takes(&r);
        if(course_string_equals(name,key)) {if(value) *value=v;return true;}
    }
    return false;
}
