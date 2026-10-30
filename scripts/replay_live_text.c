/* Replay captured text loads and name builds through the reconstructed chain.
 * Input lines (written by scripts/replay_live_calls.py):
 *   T path member mode language index capacity active metadata length units_hex
 *   B path member mode language species result units_hex64
 * Each call constructs a fresh message resource for the captured archive file
 * and member (archive index, file stream, member opening) and loads through
 * px_load_message / px_build_pokemon_name. */
#define _FILE_OFFSET_BITS 64
#define _POSIX_C_SOURCE 200809L
#include "../reconstructed/message_construction/message_construction.h"
#include "../reconstructed/message_resources/message_resources.h"
#include "../reconstructed/pokemon_names/pokemon_names.h"
#include "../native/platform/guest.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"replay_text:%d: %s\n",__LINE__,#c);exit(2); } } while (0)
enum { HEAP=0x40, SCRATCH=0x100, HEADER_SCRATCH=0x120, RESOURCE=0x300, STRING=0x340, NAME_DEST=0x400,
       BUFFER=0x1000, ARENA=0x10000000, ARENA_SIZE=64u<<20, HANDLE=0x2000, FILES=16 };
struct context { struct px_guest guest;struct px_pokemon_api memory;struct px_archive_api io;struct px_archive_index_api index;
                 struct px_resource_api resources;struct px_message_construction_api api;struct px_message_api messages;
                 char paths[FILES][64];uint8_t *files[FILES];uint64_t sizes[FILES];unsigned count;uint32_t next;const char *romfs; };
static struct context c;
static unsigned file_id(const char *path)
{
    for(unsigned i=0;i<c.count;++i)if(!strcmp(c.paths[i],path))return i;
    CHECK(c.count<FILES&&strlen(path)<sizeof c.paths[0]);
    const char *rel=strstr(path,":/");rel=rel?rel+2:path;
    char full[4096];CHECK(snprintf(full,sizeof full,"%s/%s",c.romfs,rel)<(int)sizeof full);
    FILE *f=fopen(full,"rb");CHECK(f);CHECK(fseeko(f,0,SEEK_END)==0);c.sizes[c.count]=(uint64_t)ftello(f);rewind(f);
    c.files[c.count]=malloc(c.sizes[c.count]);CHECK(c.files[c.count]&&fread(c.files[c.count],1,c.sizes[c.count],f)==c.sizes[c.count]);fclose(f);
    strcpy(c.paths[c.count],path);return c.count++;
}
static uint32_t bump(uint32_t size) { const uint32_t p=c.next;c.next+=(size+15)&~15u;CHECK(c.next-ARENA<=ARENA_SIZE);return p; }
static uint32_t object_allocate(void *u,uint32_t size,uint32_t heap) { (void)u;CHECK(heap==HEAP);return bump(size); }
static unsigned handle_file(uint32_t h) { CHECK(h>=HANDLE&&h<HANDLE+2*FILES&&h%2==0&&(h-HANDLE)/2<c.count);return (h-HANDLE)/2; }
static void lock(void *u,uint32_t s) { (void)u;(void)s; }
static uint32_t is_open(void *u,uint32_t o) { (void)u;(void)o;return 1; }
static uint32_t get_size(void *u,uint32_t h,uint64_t *size) { (void)u;*size=c.sizes[handle_file(h)];return 0; }
static uint32_t file_read(void *u,uint32_t h,uint64_t position,uint32_t buffer,uint32_t size,uint32_t *count)
{
    (void)u;const unsigned i=handle_file(h);uint32_t n=size;CHECK(position<=c.sizes[i]);
    if(position+n>c.sizes[i])n=(uint32_t)(c.sizes[i]-position);
    uint8_t *to=px_guest_span(&c.guest,buffer,n?n:1);CHECK(to);memcpy(to,c.files[i]+position,n);*count=n;return 0;
}
static void fatal(void *u,uint32_t r,uint32_t s) { (void)u;fprintf(stderr,"fatal %08x at %08x\n",r,s);exit(2); }
static void panic(void *u,uint32_t h,uint32_t m) { (void)u;(void)h;(void)m;CHECK(0); }
static void stream_info(void *u,uint32_t a,uint32_t i,uint32_t info[3]) { (void)u;px_archive_member_info_streamed(a,i,info,&c.index); }
static uint32_t try_enter(void *u,uint32_t s) { (void)u;(void)s;return 1; }
static void signal_event(void *u,uint32_t e) { (void)u;px_guest_store32(&c.guest,e,1); }
static uint32_t temporary_heap(void *u,uint32_t h) { (void)u;CHECK(h==HEAP);return HEAP; }
static uint32_t alignment(void *u,uint32_t h) { (void)u;CHECK(h==HEAP);return 4; }
static uint32_t heap_allocate(void *u,uint32_t h,uint32_t size,uint32_t a,uint32_t f) { (void)u;(void)a;(void)f;CHECK(h==HEAP);return bump(size); }
static void release(void *u,uint32_t p) { (void)u;(void)p; }
static uint32_t construct(void *u,uint32_t object,uint32_t heap,uint32_t id,uint32_t flags)
{
    (void)u;CHECK(heap==HEAP&&id<c.count);
    px_guest_store32(&c.guest,object+4,HEAP);px_guest_store32(&c.guest,object+0x18,HANDLE+2*id);
    for(uint32_t o=0x1c;o<0x2c;o+=4)px_guest_store32(&c.guest,object+o,0);
    px_guest_store16(&c.guest,object+0x34,0);px_guest_store32(&c.guest,object+0x3c,0);px_guest_store32(&c.guest,object+0x4c,0xfffffffeu);
    if((~flags&5)==0)px_build_archive_index(object,HEAP,&c.index);
    return object;
}
static void release_block(void *u,uint32_t b) { (void)u;(void)b; }
static void destroy_archive(void *u,uint32_t a) { (void)u;(void)a; }
static uint32_t allocate_block(void *u,uint32_t h,uint32_t size) { (void)u;(void)h;return bump(size); }
static uint32_t construct_message(void *u,uint32_t o,uint32_t a,uint32_t m,uint32_t h,uint32_t mode,uint32_t f)
{ (void)u;return px_construct_message_resource(o,a,m,h,mode,f,&c.api); }
static void msg_seek(void *u,uint32_t a,uint32_t m,uint32_t o) { (void)u;px_seek_archive_member(a,m,o,&c.io); }
static void msg_read(void *u,uint32_t a,uint32_t s,uint32_t d) { (void)u;(void)px_archive_read(a,s,d,&c.io); }
static uint32_t msg_alignment(void *u,uint32_t h) { (void)u;CHECK(h==HEAP);return 4; }
static uint32_t msg_allocate(void *u,uint32_t h,uint32_t s,uint32_t a,uint32_t t) { (void)u;(void)a;(void)t;CHECK(h==HEAP);return bump(s); }
static void msg_release(void *u,uint32_t p) { (void)u;(void)p; }
static uint32_t name_load(void *u,uint32_t resource,uint32_t index,uint32_t string) { (void)u;return px_load_message(resource,index,string,&c.messages); }
static void unhex(const char *s,uint8_t *out,size_t n) { for(size_t i=0;i<n;++i) { unsigned v;CHECK(sscanf(s+2*i,"%2x",&v)==1);out[i]=(uint8_t)v; } }
static void open_resource(const char *path,uint32_t member,uint32_t mode,uint32_t language)
{
    c.next=ARENA;px_guest_store32(&c.guest,PX_MESSAGE_LANGUAGE_CELL,language);
    px_construct_message_resource(RESOURCE,file_id(path),member,HEAP,mode,5,&c.api);
}
int main(int argc,char **argv)
{
    if(argc!=3) { fprintf(stderr,"usage: replay_live_text ROMFS CALLS.txt\n");return 2; }
    c.romfs=argv[1];px_guest_init(&c.guest);
    CHECK(px_guest_map(&c.guest,0,0x10000,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"scratch"));
    CHECK(px_guest_map(&c.guest,0x5d1000,0x1000,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"cells"));
    CHECK(px_guest_map(&c.guest,ARENA,ARENA_SIZE,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"heap"));
    c.memory=px_guest_pokemon_api(&c.guest);CHECK(c.memory.user==(void *)&c);c.memory.allocate=object_allocate;
    c.io=(struct px_archive_api){&c.memory,&c,lock,lock,is_open,get_size,file_read,fatal,stream_info,panic};
    c.index=(struct px_archive_index_api){&c.io,&c,try_enter,signal_event,temporary_heap,alignment,heap_allocate,release,construct,SCRATCH};
    c.resources=(struct px_resource_api){&c.index,&c,allocate_block,construct_message};
    c.api=(struct px_message_construction_api){&c.resources,&c,release_block,destroy_archive,construct,HEADER_SCRATCH};
    c.messages=(struct px_message_api){&c.memory,&c,msg_seek,msg_read,msg_alignment,msg_allocate,msg_release};
    px_guest_store32(&c.guest,0x5d1124,2);
    FILE *in=fopen(argv[2],"r");CHECK(in);static char line[8192];uint64_t done[2]={0},bad[2]={0};
    while(fgets(line,sizeof line,in)) {
        char path[64];static char hex[4096];
        if(line[0]=='T') {
            uint32_t member,mode,language,index,capacity,active,metadata,length;
            CHECK(sscanf(line+2,"%63s %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %4095s",
                         path,&member,&mode,&language,&index,&capacity,&active,&metadata,&length,hex)==10);
            open_resource(path,member,mode,language);
            px_guest_store32(&c.guest,STRING+4,BUFFER);px_guest_store16(&c.guest,STRING+8,(uint16_t)capacity);
            px_guest_store16(&c.guest,STRING+10,0);px_guest_store8(&c.guest,STRING+12,(uint8_t)active);
            const uint32_t got=px_load_message(RESOURCE,index,STRING,&c.messages);
            const size_t units=hex[0]=='-'?0:strlen(hex)/4,compare=length+1<units?length+1:units;
            uint8_t expected[2048];if(units)unhex(hex,expected,2*units);
            const int ok=got==metadata&&px_guest_load16(&c.guest,STRING+10)==length&&
                         (!compare||!memcmp(px_guest_span(&c.guest,BUFFER,(uint32_t)(2*compare)),expected,2*compare));
            ++done[0];bad[0]+=!ok;
            if(!ok)fprintf(stderr,"text %s member %u line %u: metadata %u/%u length %u/%u\n",path,member,index,got,metadata,
                           px_guest_load16(&c.guest,STRING+10),length);
        } else if(line[0]=='B') {
            uint32_t member,mode,language,species,result;
            CHECK(sscanf(line+2,"%63s %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %4095s",
                         path,&member,&mode,&language,&species,&result,hex)==7);
            open_resource(path,member,mode,language);
            /* Name globals: the 0x100-unit string object at 005d1790 and the resource at 005d1794. */
            const uint32_t names=bump(0x10),buffer=bump(0x200);
            px_guest_store32(&c.guest,names+4,buffer);px_guest_store16(&c.guest,names+8,0x100);
            px_guest_store16(&c.guest,names+10,0);px_guest_store8(&c.guest,names+12,1);
            px_guest_store32(&c.guest,0x5d1790,names);px_guest_store32(&c.guest,0x5d1794,RESOURCE);
            memset(px_guest_span(&c.guest,NAME_DEST,64),0xa5,64);
            const struct px_name_api names_api={&c.memory,&c,name_load};
            const uint32_t got=px_build_pokemon_name(NAME_DEST,species,&names_api);
            uint8_t expected[64];unhex(hex,expected,64);
            uint32_t n=0;while(n<32&&(expected[2*n]|expected[2*n+1]))++n;
            const int ok=got==result&&!memcmp(px_guest_span(&c.guest,NAME_DEST,2*(n+1)),expected,2*(n+1));
            ++done[1];bad[1]+=!ok;
            if(!ok)fprintf(stderr,"name species %u: result %u/%u\n",species,got,result);
        }
    }
    fclose(in);CHECK(c.guest.fault==PX_FAULT_NONE);
    printf("{\"load_message\":[%" PRIu64 ",%" PRIu64 "],\"build_name\":[%" PRIu64 ",%" PRIu64 "]}\n",done[0],done[0]-bad[0],done[1],done[1]-bad[1]);
    for(unsigned i=0;i<c.count;++i)free(c.files[i]);
    px_guest_free(&c.guest);return bad[0]||bad[1];
}
