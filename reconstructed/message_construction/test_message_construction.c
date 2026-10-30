#include "message_construction.h"
#include "../message_resources/message_resources.h"
#include "../pokemon_move_resource/pokemon_move_resource.h"
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x600000, HEAP=0x40, SCRATCH=0x900, HEADER_SCRATCH=0x920, CONFIG=0xa00, RESOURCE=0xb00, RESIDENT=0xb40,
       STRING=0xb80, STRING2=0xba0, ARENA=0x100000, ARENA_END=0x500000, HANDLE=0x7000, FILES=4, FILE_MAX=0x10000,
       ID_TEXT=1, ID_MOVES=2, RESIDENT_FILE=0x80000 };
struct file { uint8_t bytes[FILE_MAX];uint32_t size; };
struct fixture {
    uint8_t *bytes;struct file *files;struct px_pokemon_api memory;struct px_archive_api io;
    struct px_archive_index_api index;struct px_resource_api resources;struct px_message_construction_api api;
    struct px_message_api messages;
    uint32_t next,small_objects,fail_size,released[8],release_count,destroyed,freed,constructed_id,constructed_flags,listed,panics;
    jmp_buf escape;
};
static uint8_t r8(void *u,uint32_t a) { struct fixture *f=u;CHECK(a<SIZE);return f->bytes[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)((uint32_t)r8(u,a)|(uint32_t)r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { struct fixture *f=u;CHECK(a<SIZE);f->bytes[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static uint32_t le32(const uint8_t *p) { return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24; }
static uint32_t bump(struct fixture *f,uint32_t size) { const uint32_t p=f->next;f->next+=(size+15)&~15u;CHECK(f->next<ARENA_END);memset(f->bytes+p,0xcc,size);return p; }
static uint32_t object_allocate(void *u,uint32_t size,uint32_t heap) { struct fixture *f=u;CHECK(heap==HEAP);if(size==0x0c)++f->small_objects;return size==f->fail_size?0:bump(f,size); }
static struct file *file_of(struct fixture *f,uint32_t h) { CHECK(h>=HANDLE&&h<HANDLE+2*FILES&&h%2==0);return &f->files[(h-HANDLE)/2]; }
static void lock(void *u,uint32_t s) { (void)u;(void)s; }
static uint32_t is_open(void *u,uint32_t o) { (void)u;(void)o;return 1; }
static uint32_t get_size(void *u,uint32_t h,uint64_t *size) { *size=file_of(u,h)->size;return 0; }
static uint32_t file_read(void *u,uint32_t h,uint64_t position,uint32_t buffer,uint32_t size,uint32_t *count)
{
    struct fixture *f=u;const struct file *file=file_of(f,h);uint32_t n=size<0x80?size:0x80;
    if(position>=file->size)n=0;else if(position+n>file->size)n=(uint32_t)(file->size-position);
    for(uint32_t i=0;i<n;++i)w8(f,buffer+i,file->bytes[position+i]);
    *count=n;return 0;
}
static void fatal(void *u,uint32_t r,uint32_t s) { (void)u;fprintf(stderr,"fatal %08x %08x\n",r,s);exit(1); }
static void panic(void *u,uint32_t hook,uint32_t message) { struct fixture *f=u;CHECK(hook==0x7777&&message==PX_ARCHIVE_PANIC_MESSAGE);++f->panics;longjmp(f->escape,1); }
static void stream_info(void *u,uint32_t a,uint32_t i,uint32_t info[3]) { struct fixture *f=u;px_archive_member_info_streamed(a,i,info,&f->index); }
static uint32_t try_enter(void *u,uint32_t s) { (void)u;(void)s;return 1; }
static void signal_event(void *u,uint32_t e) { w32(u,e,1); }
static uint32_t temporary_heap(void *u,uint32_t h) { (void)u;CHECK(h==HEAP);return HEAP; }
static uint32_t alignment(void *u,uint32_t h) { (void)u;CHECK(h==HEAP);return 4; }
static uint32_t heap_allocate(void *u,uint32_t h,uint32_t size,uint32_t a,uint32_t flag)
{ struct fixture *f=u;CHECK(h==HEAP&&(a==4||a==0xfffffffcu)&&flag==1);return size==f->fail_size?0:bump(f,size); }
static void release(void *u,uint32_t p) { struct fixture *f=u;f->freed=p; }
static uint32_t make_archive(struct fixture *f,uint32_t object,uint32_t id,uint32_t flags)
{
    f->constructed_id=id;f->constructed_flags=flags;
    w32(f,object+4,HEAP);w32(f,object+0x18,HANDLE+2*id);for(uint32_t o=0x1c;o<0x2c;o+=4)w32(f,object+o,0);
    w16(f,object+0x34,0);w32(f,object+0x3c,0);w32(f,object+0x4c,0xfffffffeu);
    if((~flags&5)==0)px_build_archive_index(object,HEAP,&f->index); /* constructor flag 4 builds the index */
    return object;
}
static uint32_t construct(void *u,uint32_t object,uint32_t heap,uint32_t id,uint32_t flags) { (void)heap;return make_archive(u,object,id,flags); }
static uint32_t construct_listed(void *u,uint32_t object,uint32_t heap,uint32_t id,uint32_t flags) { struct fixture *f=u;(void)heap;++f->listed;return make_archive(f,object,id,flags); }
static void release_block(void *u,uint32_t block) { struct fixture *f=u;if(f->release_count<8)f->released[f->release_count++]=block; }
static void destroy_archive(void *u,uint32_t archive) { struct fixture *f=u;f->destroyed=archive; }
static uint32_t allocate_block(void *u,uint32_t heap,uint32_t size) { (void)heap;return bump(u,size); }
static uint32_t construct_message(void *u,uint32_t o,uint32_t a,uint32_t m,uint32_t h,uint32_t mode,uint32_t flags)
{ struct fixture *f=u;return px_construct_message_resource(o,a,m,h,mode,flags,&f->api); }
/* Message-loading services run through the reconstructed archive layer. */
static void msg_seek(void *u,uint32_t a,uint32_t member,uint32_t offset) { struct fixture *f=u;px_seek_archive_member(a,member,offset,&f->io); }
static void msg_read(void *u,uint32_t a,uint32_t size,uint32_t destination) { struct fixture *f=u;(void)px_archive_read(a,size,destination,&f->io); }
static uint32_t msg_alignment(void *u,uint32_t h) { (void)u;CHECK(h==HEAP);return 4; }
static uint32_t msg_allocate(void *u,uint32_t h,uint32_t size,uint32_t a,uint32_t tail) { struct fixture *f=u;CHECK(h==HEAP&&a==0xfffffffcu&&tail==1);return bump(f,size); }
static void msg_release(void *u,uint32_t p) { (void)u;(void)p; }
/* One text member: two language sections of N lines; mode word 0 is a plain copy. */
static uint32_t text_member(uint8_t *p,uint32_t member)
{
    const uint32_t lines=3+member%3,sections=2,header=0x0c+4*sections;uint32_t at=header,largest=0;
    memset(p,0,0x1000);
    p[0]=(uint8_t)sections;p[2]=(uint8_t)lines;
    for(uint32_t s=0;s<sections;++s) {
        const uint32_t start=at;memcpy(p+0x0c+4*s,&start,4);uint32_t data=4+8*lines;
        for(uint32_t l=0;l<lines;++l) {
            const uint16_t units=(uint16_t)(2+l),meta=(uint16_t)(0x100*s+l+member);memcpy(p+start+4+8*l,&data,4);
            memcpy(p+start+8+8*l,&units,2);memcpy(p+start+10+8*l,&meta,2);
            for(uint32_t k=0;k+1<units;++k) { const uint16_t ch=(uint16_t)('a'+s*8+l+k+member);memcpy(p+start+data+2*k,&ch,2); }
            data+=2u*units;
        }
        memcpy(p+start,&data,4);if(data>largest)largest=data;at=start+((data+3)&~3u);
    }
    memcpy(p+4,&largest,4);return at;
}
static void write_garc(struct file *file,uint32_t entries,uint32_t (*fill)(uint8_t *,uint32_t))
{
    uint8_t *g=file->bytes;memset(g,0,FILE_MAX);const uint32_t fato=0x1c,fatb=fato+12+4*entries;uint32_t p=fatb+12;
    memcpy(g,"CRAG",4);g[4]=0x1c;memcpy(g+fato,"OTAF",4);g[fato+8]=(uint8_t)entries;
    memcpy(g+fatb,"BTAF",4);memcpy(g+fatb+8,&entries,4);
    static uint8_t member[0x1000];uint32_t sizes[64],data=0;CHECK(entries<=64);
    for(uint32_t e=0;e<entries;++e) {
        const uint32_t off=p-fatb-12,one=1;sizes[e]=fill(member,e);const uint32_t end=data+((sizes[e]+3)&~3u);
        memcpy(g+fato+12+4*e,&off,4);memcpy(g+p,&one,4);memcpy(g+p+4,&data,4);memcpy(g+p+8,&end,4);memcpy(g+p+12,&sizes[e],4);p+=16;data=end;
    }
    memcpy(g+p,"BMIF",4);const uint32_t length=p+12;memcpy(g+0x10,&length,4);data=0;
    for(uint32_t e=0;e<entries;++e) { fill(g+length+data,e);data+=(sizes[e]+3)&~3u; }
    file->size=length+data;CHECK(file->size<=FILE_MAX);
}
static void move_read(void *u,uint32_t archive,uint32_t index,uint32_t destination)
{ struct fixture *f=u;CHECK(px_read_archive_member(archive,index,destination,&f->io)==0x24); }
static uint32_t move_member(uint8_t *p,uint32_t member) { for(uint32_t i=0;i<0x24;++i)p[i]=(uint8_t)(member*5+i);return 0x24; }
static void reset(struct fixture *f)
{
    uint8_t *bytes=f->bytes;struct file *files=f->files;memset(f,0,sizeof *f);f->bytes=bytes;f->files=files;
    memset(bytes,0,0x10000);memset(bytes+0x5d1000,0,0x1000);memset(bytes+0x5e5000,0,0x1000);f->next=ARENA;
    f->memory=(struct px_pokemon_api){.user=f,.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32,.allocate=object_allocate};
    f->io=(struct px_archive_api){&f->memory,f,lock,lock,is_open,get_size,file_read,fatal,stream_info,panic};
    f->index=(struct px_archive_index_api){&f->io,f,try_enter,signal_event,temporary_heap,alignment,heap_allocate,release,construct,SCRATCH};
    f->resources=(struct px_resource_api){&f->index,f,allocate_block,construct_message};
    f->api=(struct px_message_construction_api){&f->resources,f,release_block,destroy_archive,construct_listed,HEADER_SCRATCH};
    f->messages=(struct px_message_api){&f->memory,f,msg_seek,msg_read,msg_alignment,msg_allocate,msg_release};
    w32(f,PX_ARCHIVE_PANIC_HOOK,0x7777);w32(f,PX_RESOURCE_CONFIG_CELL,CONFIG);
    write_garc(&f->files[ID_TEXT],6,text_member);write_garc(&f->files[ID_MOVES],20,move_member);
}
static void string_at(struct fixture *f,uint32_t s,uint32_t buffer) { w32(f,s+4,buffer);w16(f,s+8,64);w16(f,s+10,0);w8(f,s+12,1); }
/* Same member loaded through a hand-built resident resource (archive zero). */
static void resident(struct fixture *f,uint32_t member)
{
    const struct file *t=&f->files[ID_TEXT];const uint8_t *g=t->bytes;const uint32_t fatb=0x1c+12+4*6;
    const uint32_t start=le32(g+fatb+12+16*member+4)+le32(g+0x10),size=le32(g+fatb+12+16*member+12);
    for(uint32_t i=0;i<size;++i)w8(f,RESIDENT_FILE+i,g[start+i]);
    w32(f,RESIDENT+4,HEAP);w32(f,RESIDENT+8,0);w8(f,RESIDENT+0x0c,1);w32(f,RESIDENT+0x10,member);
    w32(f,RESIDENT+0x14,0xffffffffu);w32(f,RESIDENT+0x18,RESIDENT_FILE);w32(f,RESIDENT+0x1c,0);
}
static void member_cases(struct fixture *f)
{
    volatile unsigned cases=0;
    for(uint32_t mode=0;mode<2;++mode)
        for(uint32_t language=0;language<4;++language)
            for(uint32_t member=0;member<6;++member) {
                reset(f);w32(f,PX_MESSAGE_LANGUAGE_CELL,language);
                for(uint32_t o=0x20;o<0x28;++o)w8(f,RESOURCE+o,0xaa); /* entry scratch is cleared */
                CHECK(px_construct_message_resource(RESOURCE,ID_TEXT,member,HEAP,mode,5,&f->api)==RESOURCE);
                CHECK(r32(f,RESOURCE)==PX_MESSAGE_RESOURCE_VTABLE&&r32(f,RESOURCE+4)==HEAP&&r8(f,RESOURCE+0x0c)==mode);
                CHECK(f->constructed_id==ID_TEXT&&f->constructed_flags==5&&r32(f,RESOURCE+0x10)==member);
                CHECK(r32(f,RESOURCE+0x20)==0&&r32(f,RESOURCE+0x24)==0);
                const uint32_t expected_language=language<2?language:0;CHECK(r32(f,RESOURCE+0x14)==expected_language);
                const uint8_t *g=f->files[ID_TEXT].bytes;const uint32_t fatb=0x1c+12+4*6;
                const uint8_t *raw=g+le32(g+0x10)+le32(g+fatb+12+16*member+4);
                const uint32_t header=r32(f,RESOURCE+0x18);CHECK(!memcmp(f->bytes+header,raw,0x0c+4*2));
                if(mode==0) CHECK(r32(f,RESOURCE+0x1c)==0);
                else {
                    const uint32_t off=le32(raw+0x0c+4*expected_language),size=le32(raw+off);
                    CHECK(!memcmp(f->bytes+r32(f,RESOURCE+0x1c),raw+off,size));
                }
                /* Lines load identically to a resident copy of the same member. */
                resident(f,member);
                for(uint32_t line=0;line<3+member%3;++line) {
                    string_at(f,STRING,0xc00);string_at(f,STRING2,0xd00);
                    const uint32_t a=px_load_message(RESOURCE,line,STRING,&f->messages);
                    const uint32_t b=px_load_message(RESIDENT,line,STRING2,&f->messages);
                    CHECK(a==b&&a==0x100*expected_language+line+member&&r16(f,STRING+10)==r16(f,STRING2+10));
                    CHECK(!memcmp(f->bytes+0xc00,f->bytes+0xd00,2*(2+line)));++cases;
                }
            }
    /* A zero archive aborts before any field is written. */
    reset(f);w32(f,RESOURCE+8,0);w32(f,RESOURCE+0x20,0x55555555u);
    if(!setjmp(f->escape)) { px_open_message_member(RESOURCE,0,HEAP,&f->api);CHECK(0); }
    CHECK(f->panics==1&&r32(f,RESOURCE+0x20)==0x55555555u);++cases;
    /* A failed header allocation stores zero and reads the count from address zero. */
    reset(f);px_construct_message_resource(RESOURCE,ID_TEXT,1,HEAP,0,5,&f->api);
    f->fail_size=0x0c+4*2;w16(f,0,7);w32(f,PX_MESSAGE_LANGUAGE_CELL,5);px_open_message_member(RESOURCE,1,HEAP,&f->api);
    CHECK(r32(f,RESOURCE+0x18)==0&&r32(f,RESOURCE+0x14)==5);++cases;
    reset(f);px_construct_listed_message_resource(RESOURCE,ID_TEXT,2,HEAP,1,5,&f->api);CHECK(f->listed==1&&r32(f,RESOURCE+0x10)==2);++cases;
    printf("members: %u construction, language, cached-section and resident-equivalent line-load cases\n",cases);
}
static void destroy_cases(struct fixture *f)
{
    unsigned cases=0;reset(f);px_construct_message_resource(RESOURCE,ID_TEXT,0,HEAP,1,5,&f->api);
    const uint32_t archive=r32(f,RESOURCE+8),header=r32(f,RESOURCE+0x18),section=r32(f,RESOURCE+0x1c);w32(f,RESOURCE,0);
    f->freed=0; /* index building released its temporary buffers */
    CHECK(px_destroy_message_resource(RESOURCE,&f->api)==RESOURCE&&r32(f,RESOURCE)==PX_MESSAGE_RESOURCE_VTABLE);
    CHECK(f->release_count==2&&f->released[0]==section&&f->released[1]==header&&f->destroyed==archive&&f->freed==0);++cases;
    reset(f);px_construct_message_resource(RESOURCE,ID_TEXT,0,HEAP,0,5,&f->api);f->freed=0;px_delete_message_resource(RESOURCE,&f->api);
    CHECK(f->release_count==1&&f->freed==RESOURCE);++cases;
    reset(f);w32(f,RESOURCE+8,0);w32(f,RESOURCE+0x18,0x1234);px_destroy_message_resource(RESOURCE,&f->api);CHECK(f->release_count==0&&f->destroyed==0);++cases;
    printf("destroy: %u release-order, archive-delete and object-free cases\n",cases);
}
static void move_cases(struct fixture *f)
{
    unsigned cases=0;reset(f);
    const uint32_t words[22]={[12]=ID_MOVES,[13]=0x104,[0x11]=ID_TEXT,[0x15]=4};
    for(uint32_t i=0;i<22;++i)w32(f,CONFIG+4*i,words[i]);
    px_initialize_move_resources(HEAP,&f->api);
    const uint32_t message=r32(f,0x5d1888),record=r32(f,0x5d188c),cache=r32(f,0x5d1890);
    CHECK(r32(f,PX_MOVE_ARCHIVE_CELL)!=0&&r32(f,message)==PX_MESSAGE_RESOURCE_VTABLE&&r32(f,message+0x10)==4&&r8(f,message+0x0c)==1);
    CHECK(r32(f,record)==0x598aac&&r16(f,record+4)==0&&r32(f,r32(f,record+8)+0x20)==0);
    CHECK(r32(f,cache)==0x598a9c&&r8(f,cache+8)==4&&r8(f,cache+9)==0);
    for(uint32_t i=0;i<4;++i)CHECK(r32(f,r32(f,r32(f,cache+4)+4*i))==0x598aac);
    CHECK(f->small_objects==6); /* record, cache and four slots: capacity masked to 8 bits */
    cases+=4;
    px_initialize_move_resources(HEAP,&f->api);CHECK(f->constructed_id==ID_TEXT);++cases;
    /* The reconstructed move-resource service runs from this state. */
    reset(f);for(uint32_t i=0;i<22;++i)w32(f,CONFIG+4*i,words[i]);px_initialize_move_resources(HEAP,&f->api);
    const struct px_move_resource_api moves={&f->memory,f,move_read};
    for(uint32_t move=1;move<20;++move) {
        const uint8_t *g=f->files[ID_MOVES].bytes;const uint8_t *raw=g+le32(g+0x10)+0x24*move;
        CHECK(px_get_move_resource_value(move,0,&moves)==raw[0]&&px_get_move_resource_value(move,1,&moves)==raw[1]);++cases;
    }
    printf("moves: %u move-resource boot cases\n",cases);
}
int main(void)
{
    struct fixture *f=calloc(1,sizeof *f);CHECK(f);f->bytes=calloc(1,SIZE);f->files=calloc(FILES,sizeof *f->files);CHECK(f->bytes&&f->files);
    member_cases(f);destroy_cases(f);move_cases(f);
    puts("message_construction: all tests passed");free(f->files);free(f->bytes);free(f);return 0;
}
