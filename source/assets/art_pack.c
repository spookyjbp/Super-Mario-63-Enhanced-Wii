#include "art_pack.h"
#include <math.h>
#include <string.h>
static uint16_t u16(const unsigned char *p) {return (uint16_t)((p[0]<<8)|p[1]);}
static unsigned u32(const unsigned char *p) {return ((unsigned)p[0]<<24)|((unsigned)p[1]<<16)|(p[2]<<8)|p[3];}
static int16_t s16(const unsigned char *p) {unsigned v=u16(p);return (int16_t)(v<32768?(int)v:(int)v-65536);}
bool art_pack_parse(const unsigned char *d,size_t n,ArtPack *p)
{
    memset(p,0,sizeof(*p));
    if(n<32 || memcmp(d,"SM63ART1",8) || u32(d+8)!=1) return false;
    const unsigned width=u32(d+12),height=u32(d+16),clips=u32(d+20),frames=u32(d+24),bytes=u32(d+28);
    if(width<4 || width>1024 || height<4 || height>1024 || (width&(width-1)) ||
       (height&(height-1)) || clips>ART_CLIP_COUNT || !frames || frames>ART_MAX_FRAMES || bytes!=width*height*4) return false;
    const size_t metadata=32+clips*8+frames*16,offset=(metadata+31)&~(size_t)31;
    if(offset>n || n-offset!=bytes) return false;
    p->width=width;p->height=height;p->frame_count=frames;
    for(unsigned i=0;i<clips;++i) {
        const unsigned char *c=d+32+i*8;
        unsigned id=u16(c),first=u16(c+2),count=u16(c+4),flags=u16(c+6);
        if(id>=ART_CLIP_COUNT || !count || first+count>frames || flags>3 || p->clips[id].count) return false;
        p->clips[id]=(ArtClip){first,count,flags};
    }
    for(unsigned i=0;i<frames;++i) {
        const unsigned char *f=d+32+clips*8+i*16;
        ArtFrame frame={.x=u16(f),.y=u16(f+2),.width=u16(f+4),.height=u16(f+6),
                        .offset_x=s16(f+8),.offset_y=s16(f+10),.duration=u16(f+12)};
        if(!frame.width || !frame.height || frame.x+frame.width>width || frame.y+frame.height>height ||
           !frame.duration || u16(f+14)!=0 || frame.offset_x<-2048 || frame.offset_x>2048 ||
           frame.offset_y<-2048 || frame.offset_y>2048) return false;
        p->frames[i]=frame;
    }
    p->texture=d+offset;return true;
}
const ArtFrame *art_pack_frame(const ArtPack *p,unsigned id,float seconds)
{
    if(!p->texture || id>=ART_CLIP_COUNT || !isfinite(seconds)) return NULL;
    ArtClip c=p->clips[id];if(!c.count) return NULL;
    unsigned duration=0;
    for(unsigned i=0;i<c.count;++i) duration+=p->frames[c.first+i].duration;
    float ticks=fmaxf(0,seconds)*32;
    if(c.flags&1) ticks=fmodf(ticks,(float)duration);
    else ticks=fminf(ticks,(float)duration-1);
    for(unsigned i=0;i<c.count;++i) {
        const ArtFrame *f=&p->frames[c.first+i];
        if(ticks<f->duration) return f;
        ticks-=f->duration;
    }
    return &p->frames[c.first+c.count-1];
}
