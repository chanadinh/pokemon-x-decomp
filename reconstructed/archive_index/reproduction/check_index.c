/* Optional original-data sweep; no original resources are embedded. */
#define _FILE_OFFSET_BITS 64
#define _POSIX_C_SOURCE 200809L
#include "../archive_index.h"
#include "../../../native/assets/garc.h"
#include "../../../native/platform/guest.h"
#include <dirent.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"check_index:%d: %s\n",__LINE__,#c);exit(1); } } while (0)
enum { ARCHIVE=0x1000, SCRATCH=0x1800, HEAP=0x40, TEMP=0x80, ARENA=0x10000000, ARENA_SIZE=96u<<20, HANDLE=0x500 };
struct context { struct px_guest guest;struct px_pokemon_api memory;struct px_archive_api io;struct px_archive_index_api api;
                 FILE *file;uint64_t size;uint32_t next,signals;uint8_t *bytes; };
static uint32_t u32(const uint8_t *p) { return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24; }
static void lock(void *u,uint32_t s) { (void)u;CHECK(s==ARCHIVE+0x60); }
static uint32_t is_open(void *u,uint32_t o) { (void)u;CHECK(o==ARCHIVE+8);return 1; }
static uint32_t get_size(void *u,uint32_t h,uint64_t *size) { struct context *c=u;CHECK(h==HANDLE);*size=c->size;return 0; }
static uint32_t file_read(void *u,uint32_t h,uint64_t position,uint32_t buffer,uint32_t size,uint32_t *count)
{
    struct context *c=u;CHECK(h==HANDLE&&position<=c->size);uint32_t n=size;if(position+n>c->size)n=(uint32_t)(c->size-position);
    uint8_t *to=px_guest_span(&c->guest,buffer,n?n:1);CHECK(to);memcpy(to,c->bytes+position,n);*count=n;return 0;
}
static void fatal(void *u,uint32_t r,uint32_t s) { (void)u;fprintf(stderr,"fatal %08x at %08x\n",r,s);exit(1); }
static void stream_info(void *u,uint32_t a,uint32_t i,uint32_t info[3]) { (void)u;(void)a;(void)i;(void)info;CHECK(0); }
static void panic(void *u,uint32_t h,uint32_t m) { (void)u;(void)h;(void)m;CHECK(0); }
static uint32_t try_enter(void *u,uint32_t s) { (void)u;CHECK(s==ARCHIVE+0x6c);return 1; }
static void signal_event(void *u,uint32_t e) { struct context *c=u;CHECK(e==ARCHIVE+0x4c);++c->signals; }
static uint32_t temporary_heap(void *u,uint32_t h) { (void)u;CHECK(h==HEAP);return TEMP; }
static uint32_t alignment(void *u,uint32_t h) { (void)u;CHECK(h==HEAP||h==TEMP);return 4; }
static uint32_t allocate(void *u,uint32_t h,uint32_t size,uint32_t a,uint32_t flag)
{ struct context *c=u;CHECK((h==HEAP||h==TEMP)&&a==4&&flag==1);const uint32_t p=c->next;c->next+=(size+15)&~15u;CHECK(c->next-ARENA<=ARENA_SIZE);return p; }
static void release(void *u,uint32_t p) { (void)u;CHECK(p>=ARENA); }
static uint32_t construct(void *u,uint32_t o,uint32_t h,uint32_t i,uint32_t f) { (void)u;(void)o;(void)h;(void)i;(void)f;CHECK(0);return 0; }
/* Independent FATB reference: record for an entry and selector, or NULL. */
static const uint8_t *reference(const uint8_t *g,uint32_t entry,uint32_t selector,uint32_t *count)
{
    const uint8_t *fato=g+u32(g+4);const uint32_t entries=fato[8]|(uint32_t)fato[9]<<8;
    const uint8_t *fatb=fato+u32(fato+4);const uint8_t *rec=fatb+12+u32(fato+12+4*entry);
    const uint32_t bits=u32(rec);*count=(uint32_t)__builtin_popcount(bits);(void)entries;
    uint32_t slot=0;
    if(selector<32&&(bits>>selector&1))for(uint32_t i=0;i<selector;++i)slot+=bits>>i&1;
    return rec+4+12*slot;
}
static int check(const char *path,struct context *c,uint64_t *archives,uint64_t *rows,uint64_t *multi,uint64_t *reads,uint64_t *padded)
{
    struct px_garc *g=NULL;char error[256];if(!px_garc_open(path,&g,error,sizeof error))return 0;
    FILE *file=fopen(path,"rb");CHECK(file&&fseeko(file,0,SEEK_END)==0);c->size=(uint64_t)ftello(file);rewind(file);
    c->bytes=malloc(c->size);CHECK(c->bytes&&fread(c->bytes,1,c->size,file)==c->size);fclose(file);
    CHECK(u32(c->bytes+4)==0x1c);const uint32_t entries=c->bytes[0x1c+8]|(uint32_t)c->bytes[0x1c+9]<<8;
    uint32_t max_bits=0;
    for(uint32_t e=0;e<entries;++e) { uint32_t n;reference(c->bytes,e,0,&n);if(n>1)max_bits=32; }
    const struct px_pokemon_api *m=&c->memory;
    for(uint32_t selector=0;selector<=max_bits;++selector) {
        c->next=ARENA;
        m->store32(m->user,ARCHIVE+4,HEAP);m->store32(m->user,ARCHIVE+0x18,HANDLE);
        for(uint32_t o=0x1c;o<0x2c;o+=4)m->store32(m->user,ARCHIVE+o,0);
        m->store32(m->user,ARCHIVE+0x3c,0);m->store32(m->user,ARCHIVE+0x4c,0xfffffffeu);
        m->store32(m->user,PX_ARCHIVE_SELECTOR_CELL,selector);
        px_build_archive_index(ARCHIVE,HEAP,&c->api);
        CHECK(m->load16(m->user,ARCHIVE+0x34)==(entries&0xffff)&&m->load32(m->user,ARCHIVE+0x38)==u32(c->bytes+0x18));
        const uint32_t table=m->load32(m->user,ARCHIVE+0x3c),data=u32(c->bytes+0x10);
        for(uint32_t e=0;e<entries;++e) {
            uint32_t n;const uint8_t *r=reference(c->bytes,e,selector,&n);
            const uint32_t start=u32(r),end=u32(r+4),length=u32(r+8);
            CHECK(m->load32(m->user,table+12*e)==end-start&&m->load32(m->user,table+12*e+4)==length);
            CHECK(m->load32(m->user,table+12*e+8)==data+start);++*rows;if(n>1)++*multi;if(end-start!=length)++*padded;
        }
        if(selector==0) {
            m->store32(m->user,ARCHIVE+0x4c,1);
            for(size_t i=0;i<px_garc_count(g);++i) {
                const struct px_garc_member *row=px_garc_member_at(g,i);if(row->subentry!=0)continue;
                uint8_t *expected=NULL;size_t size=0;
                CHECK(px_garc_read(g,row->entry,0,64u<<20,&expected,&size,error,sizeof error));
                const uint32_t buffer=c->next;
                const uint32_t got=px_read_archive_member(ARCHIVE,row->entry,buffer,&c->io);
                CHECK(got>=size&&!memcmp(px_guest_span(&c->guest,buffer,size?(uint32_t)size:1),expected,size));++*reads;free(expected);
            }
        }
    }
    ++*archives;free(c->bytes);px_garc_close(g);return 1;
}
static void walk(const char *dir,struct context *c,uint64_t *a,uint64_t *r,uint64_t *m,uint64_t *reads,uint64_t *padded)
{
    DIR *d=opendir(dir);if(!d)return;struct dirent *e;char **names=NULL;size_t n=0;
    while((e=readdir(d))) { if(e->d_name[0]=='.')continue;names=realloc(names,(n+1)*sizeof *names);CHECK(names);names[n++]=strdup(e->d_name); }
    closedir(d);
    for(size_t i=0;i<n;++i)for(size_t j=i+1;j<n;++j)if(strcmp(names[i],names[j])>0) { char *t=names[i];names[i]=names[j];names[j]=t; }
    for(size_t i=0;i<n;++i) {
        char path[4096];CHECK(snprintf(path,sizeof path,"%s/%s",dir,names[i])<(int)sizeof path);
        DIR *sub=opendir(path);
        if(sub) { closedir(sub);walk(path,c,a,r,m,reads,padded); } else check(path,c,a,r,m,reads,padded);
        free(names[i]);
    }
    free(names);
}
int main(int argc,char **argv)
{
    if(argc!=2) { fprintf(stderr,"usage: check_index ROMFS\n");return 2; }
    struct context c={0};px_guest_init(&c.guest);
    CHECK(px_guest_map(&c.guest,ARCHIVE,0x1000,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"archive"));
    CHECK(px_guest_map(&c.guest,0x5d1000,0x1000,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"globals"));
    CHECK(px_guest_map(&c.guest,ARENA,ARENA_SIZE,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"heap"));
    c.memory=px_guest_pokemon_api(&c.guest);
    c.io=(struct px_archive_api){&c.memory,&c,lock,lock,is_open,get_size,file_read,fatal,stream_info,panic};
    c.api=(struct px_archive_index_api){&c.io,&c,try_enter,signal_event,temporary_heap,alignment,allocate,release,construct,SCRATCH};
    char root[4096];CHECK(snprintf(root,sizeof root,"%s/a",argv[1])<(int)sizeof root);
    uint64_t archives=0,rows=0,multi=0,reads=0,padded=0;walk(root,&c,&archives,&rows,&multi,&reads,&padded);
    CHECK(archives>0&&c.guest.fault==PX_FAULT_NONE&&c.signals>=archives);
    printf("{\"garc_archives\":%" PRIu64 ",\"index_builds\":%u,\"rows_checked\":%" PRIu64 ",\"multi_record_rows\":%" PRIu64
           ",\"padded_rows\":%" PRIu64 ",\"member_reads\":%" PRIu64 ",\"all_match\":true}\n",archives,c.signals,rows,multi,padded,reads);
    px_guest_free(&c.guest);return 0;
}
