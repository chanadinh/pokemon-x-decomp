/* Optional original-data sweep; no original resources are embedded. */
#define _FILE_OFFSET_BITS 64
#define _POSIX_C_SOURCE 200809L
#include "../archive_io.h"
#include "../../../native/assets/garc.h"
#include "../../../native/platform/guest.h"
#include <dirent.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"check_archives:%d: %s\n",__LINE__,#c);exit(1); } } while (0)
enum { ARCHIVE=0x1000, ROWS=0x100000, BUFFER=0x10000000, HANDLE=0x500 };
struct context { struct px_guest guest;struct px_pokemon_api memory;struct px_archive_api api;FILE *file;uint64_t size;
                 uint32_t chunk;uint64_t platform_reads,bytes; };
static uint32_t is_open(void *u,uint32_t o) { (void)u;CHECK(o==ARCHIVE+8);return 1; }
static void lock(void *u,uint32_t s) { (void)u;CHECK(s==ARCHIVE+0x60); }
static uint32_t get_size(void *u,uint32_t h,uint64_t *size) { struct context *c=u;CHECK(h==HANDLE);*size=c->size;return 0; }
static uint32_t file_read(void *u,uint32_t h,uint64_t position,uint32_t buffer,uint32_t size,uint32_t *count)
{
    struct context *c=u;CHECK(h==HANDLE&&position<=c->size);++c->platform_reads;
    uint32_t n=size<c->chunk?size:c->chunk;if(position+n>c->size)n=(uint32_t)(c->size-position);
    uint8_t *to=px_guest_span(&c->guest,buffer,n?n:1);CHECK(to);
    CHECK(fseeko(c->file,(off_t)position,SEEK_SET)==0&&fread(to,1,n,c->file)==n);*count=n;c->bytes+=n;return 0;
}
static void fatal(void *u,uint32_t r,uint32_t s) { (void)u;fprintf(stderr,"fatal %08x at %08x\n",r,s);exit(1); }
static void stream_info(void *u,uint32_t a,uint32_t i,uint32_t info[3]) { (void)u;(void)a;(void)i;(void)info;CHECK(0); }
static void panic(void *u,uint32_t h,uint32_t m) { (void)u;(void)h;(void)m;CHECK(0); }
static int compare(const char *path,struct context *c,uint64_t *members,uint64_t *partial)
{
    struct px_garc *g=NULL;char error[256];if(!px_garc_open(path,&g,error,sizeof error))return 0;
    const size_t count=px_garc_count(g);if(count==0||count>0xffff) { px_garc_close(g);return 0; }
    c->file=fopen(path,"rb");CHECK(c->file&&fseeko(c->file,0,SEEK_END)==0);c->size=(uint64_t)ftello(c->file);
    const struct px_pokemon_api *m=&c->memory;
    m->store32(m->user,ARCHIVE+0x18,HANDLE|1);m->store32(m->user,ARCHIVE+0x1c,0);m->store32(m->user,ARCHIVE+0x20,0);
    m->store32(m->user,ARCHIVE+0x24,0);m->store32(m->user,ARCHIVE+0x28,0);
    m->store16(m->user,ARCHIVE+0x34,(uint16_t)count);m->store32(m->user,ARCHIVE+0x3c,ROWS);m->store32(m->user,ARCHIVE+0x4c,0);
    for(size_t i=0;i<count;++i) {
        const struct px_garc_member *row=px_garc_member_at(g,i);
        m->store32(m->user,ROWS+12*(uint32_t)i,row->size);m->store32(m->user,ROWS+12*(uint32_t)i+4,row->offset+row->size);
        m->store32(m->user,ROWS+12*(uint32_t)i+8,row->offset);
    }
    for(size_t i=0;i<count;++i) {
        const struct px_garc_member *row=px_garc_member_at(g,i);uint8_t *expected=NULL;size_t n=0;
        CHECK(px_garc_read(g,row->entry,row->subentry,64u<<20,&expected,&n,error,sizeof error)&&n==row->size);
        c->chunk=(uint32_t)(0x200u<<(i%6))+(uint32_t)(i%7);
        CHECK(px_read_archive_member(ARCHIVE,(uint32_t)i,BUFFER,&c->api)==row->size);
        CHECK(!n||!memcmp(px_guest_span(&c->guest,BUFFER,(uint32_t)n),expected,n));++*members;
        if(n>=8) {
            const uint32_t at=(uint32_t)(n/3),len=(uint32_t)(n-at<64?n-at:64);
            px_seek_archive_member(ARCHIVE,(uint32_t)i,at,&c->api);
            CHECK(px_archive_read(ARCHIVE,len,BUFFER,&c->api)==len&&!memcmp(px_guest_span(&c->guest,BUFFER,len),expected+at,len));++*partial;
        }
        free(expected);
    }
    fclose(c->file);px_garc_close(g);return 1;
}
static void walk(const char *dir,struct context *c,uint64_t *archives,uint64_t *members,uint64_t *partial)
{
    DIR *d=opendir(dir);if(!d)return;struct dirent *e;char **names=NULL;size_t n=0;
    while((e=readdir(d))) { if(e->d_name[0]=='.')continue;names=realloc(names,(n+1)*sizeof *names);CHECK(names);names[n++]=strdup(e->d_name); }
    closedir(d);
    for(size_t i=0;i<n;++i)for(size_t j=i+1;j<n;++j)if(strcmp(names[i],names[j])>0) { char *t=names[i];names[i]=names[j];names[j]=t; }
    for(size_t i=0;i<n;++i) {
        char path[4096];CHECK(snprintf(path,sizeof path,"%s/%s",dir,names[i])<(int)sizeof path);
        DIR *sub=opendir(path);
        if(sub) { closedir(sub);walk(path,c,archives,members,partial); }
        else if(compare(path,c,members,partial))++*archives;
        free(names[i]);
    }
    free(names);
}
int main(int argc,char **argv)
{
    if(argc!=2) { fprintf(stderr,"usage: check_archives ROMFS\n");return 2; }
    struct context c={0};px_guest_init(&c.guest);
    CHECK(px_guest_map(&c.guest,ARCHIVE,0x100,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"archive"));
    CHECK(px_guest_map(&c.guest,ROWS,12*0x10000,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"member rows"));
    CHECK(px_guest_map(&c.guest,BUFFER,64u<<20,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"destination"));
    c.memory=px_guest_pokemon_api(&c.guest);
    c.api=(struct px_archive_api){&c.memory,&c,lock,lock,is_open,get_size,file_read,fatal,stream_info,panic};
    char root[4096];CHECK(snprintf(root,sizeof root,"%s/a",argv[1])<(int)sizeof root);
    uint64_t archives=0,members=0,partial=0;walk(root,&c,&archives,&members,&partial);
    CHECK(archives>0&&c.guest.fault==PX_FAULT_NONE);
    printf("{\"garc_archives\":%" PRIu64 ",\"members\":%" PRIu64 ",\"seek_partial_reads\":%" PRIu64 ",\"platform_reads\":%" PRIu64
           ",\"bytes_read\":%" PRIu64 ",\"all_match\":true}\n",archives,members,partial,c.platform_reads,c.bytes);
    px_guest_free(&c.guest);return 0;
}
