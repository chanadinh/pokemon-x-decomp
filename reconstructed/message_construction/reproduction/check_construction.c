/* Optional original-data sweep; no original resources are embedded. */
#define _FILE_OFFSET_BITS 64
#define _POSIX_C_SOURCE 200809L
#include "../message_construction.h"
#include "../../message_resources/message_resources.h"
#include "../../pokemon_move_resource/pokemon_move_resource.h"
#include "../../../native/assets/garc.h"
#include "../../../native/assets/gf_text.h"
#include "../../../native/platform/guest.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"check_construction:%d: %s\n",__LINE__,#c);exit(1); } } while (0)
enum { HEAP=0x40, SCRATCH=0x100, HEADER_SCRATCH=0x120, CONFIG=0x200, RESOURCE=0x300, STRING=0x340,
       DEST=0x400, ARENA=0x10000000, ARENA_SIZE=64u<<20, HANDLE=0x1000 };
static const char *const paths[]={"a/0/7/4","a/0/8/2","a/2/1/2"};
struct context { struct px_guest guest;struct px_pokemon_api memory;struct px_archive_api io;struct px_archive_index_api index;
                 struct px_resource_api resources;struct px_message_construction_api api;struct px_message_api messages;
                 uint8_t *files[3];uint64_t sizes[3];uint32_t next,mark;uint64_t releases,destroyed; };
static uint8_t *load(const char *romfs,const char *name,uint64_t *size)
{
    char path[4096];CHECK(snprintf(path,sizeof path,"%s/%s",romfs,name)<(int)sizeof path);FILE *f=fopen(path,"rb");CHECK(f);
    CHECK(fseeko(f,0,SEEK_END)==0);*size=(uint64_t)ftello(f);rewind(f);uint8_t *b=malloc(*size);CHECK(b&&fread(b,1,*size,f)==*size);fclose(f);return b;
}
static uint32_t bump(struct context *c,uint32_t size) { const uint32_t p=c->next;c->next+=(size+15)&~15u;CHECK(c->next-ARENA<=ARENA_SIZE);return p; }
static uint32_t object_allocate(void *u,uint32_t size,uint32_t heap) { CHECK(heap==HEAP);return bump(u,size); }
static unsigned file_index(uint32_t h) { CHECK(h>=HANDLE&&h<HANDLE+6&&h%2==0);return (h-HANDLE)/2; }
static void lock(void *u,uint32_t s) { (void)u;(void)s; }
static uint32_t is_open(void *u,uint32_t o) { (void)u;(void)o;return 1; }
static uint32_t get_size(void *u,uint32_t h,uint64_t *size) { struct context *c=u;*size=c->sizes[file_index(h)];return 0; }
static uint32_t file_read(void *u,uint32_t h,uint64_t position,uint32_t buffer,uint32_t size,uint32_t *count)
{
    struct context *c=u;const unsigned i=file_index(h);uint32_t n=size;CHECK(position<=c->sizes[i]);
    if(position+n>c->sizes[i])n=(uint32_t)(c->sizes[i]-position);
    uint8_t *to=px_guest_span(&c->guest,buffer,n?n:1);CHECK(to);memcpy(to,c->files[i]+position,n);*count=n;return 0;
}
static void fatal(void *u,uint32_t r,uint32_t s) { (void)u;fprintf(stderr,"fatal %08x at %08x\n",r,s);exit(1); }
static void panic(void *u,uint32_t h,uint32_t m) { (void)u;(void)h;(void)m;CHECK(0); }
static void stream_info(void *u,uint32_t a,uint32_t i,uint32_t info[3]) { struct context *c=u;px_archive_member_info_streamed(a,i,info,&c->index); }
static uint32_t try_enter(void *u,uint32_t s) { (void)u;(void)s;return 1; }
static void signal_event(void *u,uint32_t e) { struct context *c=u;px_guest_store32(&c->guest,e,1); }
static uint32_t temporary_heap(void *u,uint32_t h) { (void)u;CHECK(h==HEAP);return HEAP; }
static uint32_t alignment(void *u,uint32_t h) { (void)u;CHECK(h==HEAP);return 4; }
static uint32_t heap_allocate(void *u,uint32_t h,uint32_t size,uint32_t a,uint32_t f) { (void)a;(void)f;CHECK(h==HEAP);return bump(u,size); }
static void release(void *u,uint32_t p) { (void)u;(void)p; }
static uint32_t construct(void *u,uint32_t object,uint32_t heap,uint32_t id,uint32_t flags)
{
    struct context *c=u;CHECK(heap==HEAP&&id<3);
    px_guest_store32(&c->guest,object+4,HEAP);px_guest_store32(&c->guest,object+0x18,HANDLE+2*id);
    for(uint32_t o=0x1c;o<0x2c;o+=4)px_guest_store32(&c->guest,object+o,0);
    px_guest_store16(&c->guest,object+0x34,0);px_guest_store32(&c->guest,object+0x3c,0);px_guest_store32(&c->guest,object+0x4c,0xfffffffeu);
    if((~flags&5)==0)px_build_archive_index(object,HEAP,&c->index);
    return object;
}
static uint32_t construct_listed(void *u,uint32_t o,uint32_t h,uint32_t i,uint32_t f) { return construct(u,o,h,i,f); }
static void release_block(void *u,uint32_t b) { struct context *c=u;(void)b;++c->releases; }
static void destroy_archive(void *u,uint32_t a) { struct context *c=u;(void)a;++c->destroyed; }
static uint32_t allocate_block(void *u,uint32_t h,uint32_t size) { (void)h;return bump(u,size); }
static uint32_t construct_message(void *u,uint32_t o,uint32_t a,uint32_t m,uint32_t h,uint32_t mode,uint32_t f)
{ struct context *c=u;return px_construct_message_resource(o,a,m,h,mode,f,&c->api); }
static void msg_seek(void *u,uint32_t a,uint32_t m,uint32_t o) { struct context *c=u;px_seek_archive_member(a,m,o,&c->io); }
static void msg_read(void *u,uint32_t a,uint32_t s,uint32_t d) { struct context *c=u;(void)px_archive_read(a,s,d,&c->io); }
static uint32_t msg_alignment(void *u,uint32_t h) { (void)u;CHECK(h==HEAP);return 4; }
static uint32_t msg_allocate(void *u,uint32_t h,uint32_t s,uint32_t a,uint32_t t) { (void)a;(void)t;CHECK(h==HEAP);return bump(u,s); }
static void msg_release(void *u,uint32_t p) { (void)u;(void)p; }
static void move_read(void *u,uint32_t a,uint32_t i,uint32_t d) { struct context *c=u;(void)px_read_archive_member(a,i,d,&c->io); }
int main(int argc,char **argv)
{
    if(argc!=2) { fprintf(stderr,"usage: check_construction ROMFS\n");return 2; }
    static struct context c;c.next=ARENA;px_guest_init(&c.guest);
    for(unsigned i=0;i<3;++i)c.files[i]=load(argv[1],paths[i],&c.sizes[i]);
    CHECK(px_guest_map(&c.guest,0,0x10000,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"scratch"));
    CHECK(px_guest_map(&c.guest,0x5d1000,0x1000,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"resource cells"));
    CHECK(px_guest_map(&c.guest,0x5e5000,0x1000,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"archive cells"));
    CHECK(px_guest_map(&c.guest,ARENA,ARENA_SIZE,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"heap"));
    c.memory=px_guest_pokemon_api(&c.guest);CHECK(c.memory.user==(void *)&c);c.memory.allocate=object_allocate;
    c.io=(struct px_archive_api){&c.memory,&c,lock,lock,is_open,get_size,file_read,fatal,stream_info,panic};
    c.index=(struct px_archive_index_api){&c.io,&c,try_enter,signal_event,temporary_heap,alignment,heap_allocate,release,construct,SCRATCH};
    c.resources=(struct px_resource_api){&c.index,&c,allocate_block,construct_message};
    c.api=(struct px_message_construction_api){&c.resources,&c,release_block,destroy_archive,construct_listed,HEADER_SCRATCH};
    c.messages=(struct px_message_api){&c.memory,&c,msg_seek,msg_read,msg_alignment,msg_allocate,msg_release};
    uint64_t members=0,constructions=0,lines=0,units=0,empty=0;char error[256];
    for(uint32_t a=0;a<2;++a) {
        char path[4096];struct px_garc *g=NULL;
        CHECK(snprintf(path,sizeof path,"%s/%s",argv[1],paths[a])<(int)sizeof path&&px_garc_open(path,&g,error,sizeof error));
        for(size_t i=0;i<px_garc_count(g);++i) {
            const struct px_garc_member *m=px_garc_member_at(g,i);uint8_t *raw=NULL;size_t size=0;
            CHECK(px_garc_read(g,m->entry,m->subentry,1u<<24,&raw,&size,error,sizeof error));
            if(size==0) { ++empty;free(raw);continue; }
            struct px_gf_text text;CHECK(px_gf_text_decode(raw,size,&text)==PX_GF_TEXT_OK);++members;
            const uint32_t section=(uint32_t)raw[12]|(uint32_t)raw[13]<<8|(uint32_t)raw[14]<<16|(uint32_t)raw[15]<<24;
            for(uint32_t mode=0;mode<2;++mode) {
                c.mark=c.next;px_guest_store32(&c.guest,PX_MESSAGE_LANGUAGE_CELL,mode?3:0);
                px_construct_message_resource(RESOURCE,a,m->entry,HEAP,mode,5,&c.api);++constructions;
                CHECK(px_guest_load32(&c.guest,RESOURCE+0x14)==0); /* single-section members select section zero */
                for(uint32_t line=0;line<text.line_count;++line) {
                    px_guest_store32(&c.guest,STRING+4,DEST);px_guest_store16(&c.guest,STRING+8,65535);px_guest_store8(&c.guest,STRING+12,1);
                    const uint32_t metadata=(uint32_t)raw[section+4+line*8+6]|(uint32_t)raw[section+4+line*8+7]<<8;
                    CHECK(px_load_message(RESOURCE,line,STRING,&c.messages)==metadata);
                    for(uint32_t k=0;k<text.length[line];++k)CHECK(px_guest_load16(&c.guest,DEST+2*k)==text.units[text.first[line]+k]);
                    ++lines;units+=text.length[line];
                }
                px_destroy_message_resource(RESOURCE,&c.api);c.next=c.mark;
            }
            px_gf_text_free(&text);free(raw);
        }
        px_garc_close(g);
    }
    /* Move resources: boot through 00112894 from a/2/1/2, then query fields. */
    px_guest_store32(&c.guest,PX_RESOURCE_CONFIG_CELL,CONFIG);
    const uint32_t words[22]={[12]=2,[13]=4,[0x11]=1,[0x15]=0};
    for(uint32_t i=0;i<22;++i)px_guest_store32(&c.guest,CONFIG+4*i,words[i]);
    px_initialize_move_resources(HEAP,&c.api);
    const struct px_move_resource_api moves={&c.memory,&c,move_read};
    const uint8_t *g=c.files[2];const uint32_t data=(uint32_t)g[0x10]|(uint32_t)g[0x11]<<8|(uint32_t)g[0x12]<<16|(uint32_t)g[0x13]<<24;
    const uint32_t fato=0x1c,fatb=fato+((uint32_t)g[fato+4]|(uint32_t)g[fato+5]<<8|(uint32_t)g[fato+6]<<16);
    uint64_t move_fields=0;
    for(uint32_t move=0;move<PX_MOVE_RESOURCE_COUNT;++move) {
        const uint8_t *off=g+fato+12+4*move;const uint32_t rec=(uint32_t)off[0]|(uint32_t)off[1]<<8|(uint32_t)off[2]<<16|(uint32_t)off[3]<<24;
        const uint8_t *r=g+fatb+12+rec+4;const uint32_t start=(uint32_t)r[0]|(uint32_t)r[1]<<8|(uint32_t)r[2]<<16|(uint32_t)r[3]<<24;
        const uint8_t *raw=g+data+start;
        static const uint32_t selectors[]={0,1,2,3,5};
        for(unsigned s=0;s<5;++s) { CHECK(px_get_move_resource_value(move,selectors[s],&moves)==raw[selectors[s]]);++move_fields; }
    }
    CHECK(c.guest.fault==PX_FAULT_NONE);
    printf("{\"text_members\":%" PRIu64 ",\"empty_members\":%" PRIu64 ",\"constructions\":%" PRIu64 ",\"lines_loaded\":%" PRIu64
           ",\"units_compared\":%" PRIu64 ",\"releases\":%" PRIu64 ",\"archives_destroyed\":%" PRIu64 ",\"move_fields\":%" PRIu64 ",\"all_match\":true}\n",
           members,empty,constructions,lines,units,c.releases,c.destroyed,move_fields);
    for(unsigned i=0;i<3;++i)free(c.files[i]);
    px_guest_free(&c.guest);return 0;
}
