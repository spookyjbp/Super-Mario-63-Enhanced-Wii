/* Course pack (format v1) for the original tutorial, course 8-E1.
   Expected values were read from the original source; see
   docs/original-tutorial-reference.md. */
#include "course_pack.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static unsigned char blob[1 << 20];
static size_t blob_size;
static CoursePack pack;

static const CourseObject *find(const CourseArea *a,const char *type,const char *name)
{
    for(unsigned i=0;i<a->object_count;++i)
        if(course_string_equals(a->objects[i].type,type) && (!name || course_string_equals(a->objects[i].name,name)))
            return &a->objects[i];
    return NULL;
}
static unsigned count(const CourseArea *a,const char *type)
{
    unsigned n=0;
    for(unsigned i=0;i<a->object_count;++i) n+=course_string_equals(a->objects[i].type,type);
    return n;
}
static bool param_is(const CourseObject *o,const char *key,const char *value)
{ CourseString v; return course_object_param(o,key,&v) && course_string_equals(v,value); }

static void test_areas_and_counts(void)
{
    assert(course_pack_parse(blob,blob_size,&pack) && pack.area_count==4);
    const char *ids[]={"8-E1-1","8-E1-1-2","8-E1-2","8-E1-2-2"};
    const unsigned objects[]={56,44,62,98},water[]={3,1,1,0},coins[]={18,13,24,33},goombas[]={3,3,2,2};
    for(unsigned i=0;i<4;++i) {
        const CourseArea *a=course_pack_area(&pack,ids[i]);
        assert(a && a->object_count==objects[i] && a->water_count==water[i]);
        assert(count(a,"Gold Coin Code")==coins[i] && count(a,"Goomba")==goombas[i]);
        assert(course_area_mask(a,COURSE_PLATFORMS,0) && course_area_mask(a,COURSE_CAMEDGE,0));
    }
    assert(!course_pack_area(&pack,"8-E1-3"));
    const CourseArea *last=course_pack_area(&pack,"8-E1-2-2");
    const CourseObject *shine=find(last,"Shine Sprite Clip",NULL);
    assert(shine && param_is(shine,"starnum","39") && shine->x==730.2f && shine->y==-813.5f);
    const CourseObject *pipe=find(last,"Invisible Door",NULL);
    assert(pipe && param_is(pipe,"doorplace","C-Opening") && param_is(pipe,"walk","warppipe"));
    assert(find(course_pack_area(&pack,"8-E1-2"),"Squirt Nozzle Box Clip",NULL));
}
static void test_collision_points(void)
{
    const CourseArea *a=course_pack_area(&pack,"8-E1-1");
    /* Spawn (0,0) is open air; Mario lands on the ground at y = 93. */
    assert(!course_area_hit(a,COURSE_PLATFORMS,0,0,0) && !course_area_hit(a,COURSE_PLATFORMS,0,0,92));
    assert(course_area_hit(a,COURSE_PLATFORMS,0,0,93));
    /* Barrier: solid in frame 1, removed in frame 2 (after the "controls" sign). */
    assert(course_area_hit(a,COURSE_PLATFORMS,0,316,0) && course_area_hit(a,COURSE_PLATFORMS,0,340,0));
    assert(!course_area_hit(a,COURSE_PLATFORMS,1,316,0) && !course_area_hit(a,COURSE_PLATFORMS,1,340,0));
    assert(course_area_hit(a,COURSE_PLATFORMS,1,0,93)); /* ground unchanged */
    /* Later frames hold the last keyframe; outside a mask is empty. */
    assert(course_area_mask(a,COURSE_PLATFORMS,5)->frame==1);
    assert(!course_area_hit(a,COURSE_PLATFORMS,0,-5000,0) && !course_area_hit(a,COURSE_LAVA,0,0,93));
    /* Camera edge: the spawn is inside the camera hole, far outside is solid. */
    assert(!course_area_hit(a,COURSE_CAMEDGE,0,0,0) && course_area_hit(a,COURSE_CAMEDGE,0,-430,0));
    /* Ice patch near the exit, pools from Water Code (WaterDeepness default 400). */
    assert(course_area_hit(a,COURSE_ICE,0,1660,10) && !course_area_hit(a,COURSE_ICE,0,1000,10));
    float surface=0;
    assert(course_area_water(a,500,200,&surface) && surface==158.75f && !course_area_water(a,500,100,NULL));
    assert(a->water[1].w==410 && a->water[1].h==400);
}
static void test_scripted_objects(void)
{
    const CourseArea *a=course_pack_area(&pack,"8-E1-1");
    const CourseObject *controls=find(a,"Coded Sign","sign2");
    assert(controls && controls->x==146 && param_is(controls,"signmessage","controls"));
    assert(param_is(controls,"picture","true"));
    assert(param_is(controls,"completefunction","_root.Course.Platforms.gotoAndStop(2);"));
    const CourseObject *warp=find(a,"Blank Movieclip","warp1");
    assert(warp && warp->x==1855.8f && warp->frame_start==2 && warp->frame_end==3);
    float box[4];course_object_bounds(warp,COURSE_BLANK_U0,COURSE_BLANK_V0,COURSE_BLANK_U1,COURSE_BLANK_V1,box);
    assert(box[0]>1830 && box[2]<1882 && box[1]<-245 && box[3]>-7); /* tall strip: a=d=0, b=4.71, c=-3.12 */
    const CourseObject *toad=find(a,"Toad Main Clip",NULL);
    CourseString text;
    assert(toad && course_object_param(toad,"M1Txt",&text) && text.length>20 && !memcmp(text.text,".....Did you know",17));
    assert(param_is(toad,"M4Moo","Mad"));
    unsigned newlines=0;
    for(unsigned i=0;i<a->object_count;++i) {
        CourseString m;
        if(course_object_param(&a->objects[i],"signmessage",&m) && m.length>40)
            for(unsigned k=0;k<m.length;++k) newlines+=m.text[k]=='\n';
    }
    assert(newlines==2); /* "run / Dive / backflip" sign joined with newline */
}
static void test_rejects_damage(void)
{
    static unsigned char bad[1 << 20];
    CoursePack p;
    assert(!course_pack_parse(blob,blob_size-40,&p));
    memcpy(bad,blob,blob_size);bad[11]=9;assert(!course_pack_parse(bad,blob_size,&p)); /* version */
    memcpy(bad,blob,blob_size);bad[0]='X';assert(!course_pack_parse(bad,blob_size,&p));
}
int main(void)
{
    FILE *f=fopen("../../assets/generated/course_8e1.bin","rb");assert(f);
    blob_size=fread(blob,1,sizeof(blob),f);fclose(f);assert(blob_size>1000 && blob_size<sizeof(blob));
    test_areas_and_counts();test_collision_points();test_scripted_objects();test_rejects_damage();
    puts("4 course-pack (original tutorial 8-E1) test groups passed");return 0;
}
