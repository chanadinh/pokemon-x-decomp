#include "archive_io.h"
#include "../pokemon_growth/pokemon_growth.h"
#include "../pokemon_data/pokemon_data.h"
#include "../pokemon_stats/stats_math.h"
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x5f0000, ARCHIVE=0x1000, STREAM=ARCHIVE+0x18, ROWS=0x2000, BUFFER=0x8000, FILE_SIZE=0x3000,
       HANDLE=0x4440, LOG=1024 };
struct fixture {
    uint8_t *bytes;uint8_t file[FILE_SIZE];struct px_pokemon_api memory;struct px_archive_api api;
    unsigned chunk,size_calls,read_calls,locks,unlocks,open_calls,info_calls,panics,hook_reads,hook_after;
    uint32_t fail_size,fail_read_at,open_value,last_handle,last_fatal,last_site,fatal_calls;uint64_t reported_size;
    int in_lock,giant;char log[LOG];unsigned log_count;jmp_buf escape;
};
static struct fixture *active;
static void note(struct fixture *f,char c) { if(f->log_count<LOG-1)f->log[f->log_count++]=c; }
static uint8_t r8(void *u,uint32_t a) { struct fixture *f=u;CHECK(a<SIZE);return f->bytes[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)((uint32_t)r8(u,a)|(uint32_t)r8(u,a+1)<<8); }
static uint32_t get32(struct fixture *f,uint32_t a) { return (uint32_t)r16(f,a)|(uint32_t)r16(f,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { struct fixture *f=u;CHECK(a<SIZE);f->bytes[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void put32(struct fixture *f,uint32_t a,uint32_t v) { w16(f,a,(uint16_t)v);w16(f,a+2,(uint16_t)(v>>16)); }
static uint32_t r32(void *u,uint32_t a)
{
    struct fixture *f=u;
    if(a==PX_ARCHIVE_PANIC_HOOK&&++f->hook_reads==f->hook_after)put32(f,a,0x7777);
    return get32(f,a);
}
static void w32(void *u,uint32_t a,uint32_t v)
{
    struct fixture *f=u;if(a==STREAM+0x0c)note(f,'l');if(a==STREAM+0x10)note(f,'h');
    if(a==STREAM+4)note(f,'p');
    if(a==STREAM+8)note(f,'q');
    put32(f,a,v);
}
static uint64_t get64(struct fixture *f,uint32_t a) { return get32(f,a)|(uint64_t)get32(f,a+4)<<32; }
static void put64(struct fixture *f,uint32_t a,uint64_t v) { put32(f,a,(uint32_t)v);put32(f,a+4,(uint32_t)(v>>32)); }
static void lock(void *u,uint32_t s) { struct fixture *f=u;CHECK(s==ARCHIVE+0x60&&!f->in_lock);f->in_lock=1;++f->locks;note(f,'L'); }
static void unlock(void *u,uint32_t s) { struct fixture *f=u;CHECK(s==ARCHIVE+0x60&&f->in_lock);f->in_lock=0;++f->unlocks;note(f,'U'); }
static uint32_t is_open(void *u,uint32_t o) { struct fixture *f=u;CHECK(o==ARCHIVE+8);++f->open_calls;return f->open_value; }
static uint32_t get_size(void *u,uint32_t h,uint64_t *size)
{
    struct fixture *f=u;++f->size_calls;f->last_handle=h;note(f,'S');
    if(f->fail_size)return f->fail_size;
    *size=f->reported_size;return 0;
}
static uint32_t file_read(void *u,uint32_t h,uint64_t position,uint32_t buffer,uint32_t size,uint32_t *count)
{
    struct fixture *f=u;++f->read_calls;f->last_handle=h;note(f,'R');
    if(f->fail_read_at&&f->read_calls==f->fail_read_at)return 0xc8804478u;
    if(f->giant) { *count=size;return 0; }
    uint32_t n=size<f->chunk?size:f->chunk;
    if(position>=FILE_SIZE)n=0;else if(position+n>FILE_SIZE)n=(uint32_t)(FILE_SIZE-position);
    for(uint32_t i=0;i<n;++i)w8(f,buffer+i,f->file[position+i]);
    *count=n;return 0;
}
static void fatal(void *u,uint32_t result,uint32_t site) { struct fixture *f=u;++f->fatal_calls;f->last_fatal=result;f->last_site=site;note(f,'F'); }
static void stream_info(void *u,uint32_t archive,uint32_t index,uint32_t info[3])
{ struct fixture *f=u;CHECK(archive==ARCHIVE);++f->info_calls;info[0]=16;info[1]=0;info[2]=index*16; }
static void panic(void *u,uint32_t hook,uint32_t message)
{ struct fixture *f=u;CHECK(hook==0x7777&&message==PX_ARCHIVE_PANIC_MESSAGE);++f->panics;longjmp(f->escape,1); }
static void reset(struct fixture *f)
{
    uint8_t *bytes=f->bytes;memset(f,0,sizeof *f);f->bytes=bytes;memset(bytes,0xa5,0x10000);
    for(unsigned i=0;i<FILE_SIZE;++i)f->file[i]=(uint8_t)(i*37u+(i>>8));
    f->memory=(struct px_pokemon_api){.user=f,.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
    f->api=(struct px_archive_api){&f->memory,f,lock,unlock,is_open,get_size,file_read,fatal,stream_info,panic};
    f->chunk=0x40;f->open_value=1;f->reported_size=FILE_SIZE;
    put32(f,STREAM,HANDLE|1);put64(f,STREAM+4,0);put64(f,STREAM+0x0c,0);
    put32(f,ARCHIVE+0x4c,0);w16(f,ARCHIVE+0x34,8);put32(f,ARCHIVE+0x3c,ROWS);
    for(unsigned i=0;i<8;++i) { put32(f,ROWS+12*i,0x101+i*13);put32(f,ROWS+12*i+4,0xdead0000u|i);put32(f,ROWS+12*i+8,0x200*i+i); }
    put32(f,PX_ARCHIVE_PANIC_HOOK,0);active=f;
}
static void streams(struct fixture *f)
{
    unsigned cases=0;uint64_t size=7;
    reset(f);CHECK(px_file_get_size(0,&size,&f->api)==PX_FS_RESULT_NULL_ARGUMENT&&f->size_calls==0&&size==7);++cases;
    /* Negative positions fail before any size query. */
    reset(f);CHECK(px_file_stream_set_position(STREAM,UINT64_C(0x8000000000000000),&f->api)==PX_FS_RESULT_BAD_POSITION&&f->log_count==0);++cases;
    /* Below the cached size: no query. At or above it: refresh high then low word. */
    reset(f);put64(f,STREAM+0x0c,0x100);CHECK(px_file_stream_set_position(STREAM,0xff,&f->api)==0&&!strcmp(f->log,"pq")&&get64(f,STREAM+4)==0xff);++cases;
    reset(f);put64(f,STREAM+0x0c,0x100);CHECK(px_file_stream_set_position(STREAM,0x100,&f->api)==0);
    CHECK(!strcmp(f->log,"Shlpq")&&f->last_handle==HANDLE&&get64(f,STREAM+0x0c)==FILE_SIZE);++cases;
    reset(f);CHECK(px_file_stream_set_position(STREAM,FILE_SIZE,&f->api)==0&&get64(f,STREAM+4)==FILE_SIZE);++cases;
    reset(f);put64(f,STREAM+4,5);CHECK(px_file_stream_set_position(STREAM,FILE_SIZE+1,&f->api)==PX_FS_RESULT_BAD_POSITION);
    CHECK(get64(f,STREAM+4)==5&&get64(f,STREAM+0x0c)==FILE_SIZE);++cases;
    reset(f);put64(f,STREAM+0x0c,99);f->fail_size=0xd8604664u;put64(f,STREAM+4,5);
    CHECK(px_file_stream_set_position(STREAM,0x200,&f->api)==0xd8604664u&&!strcmp(f->log,"Slh")&&get64(f,STREAM+0x0c)==0&&get64(f,STREAM+4)==5);++cases;
    /* A negative cached size forces a query. */
    reset(f);put64(f,STREAM+0x0c,UINT64_C(0x8000000000000000));CHECK(px_file_stream_set_position(STREAM,0,&f->api)==0&&f->size_calls==1);++cases;
    reset(f);f->reported_size=UINT64_C(0x123456789);CHECK(px_file_stream_set_position(STREAM,UINT64_C(0x100000000),&f->api)==0&&get64(f,STREAM+4)==UINT64_C(0x100000000));++cases;
    /* Seek origins. */
    for(uint32_t whence=0;whence<6;++whence)
        for(int64_t offset=-0x30;offset<=0x30;offset+=0x10) {
            reset(f);put64(f,STREAM+4,0x80);const uint32_t result=px_file_stream_seek(STREAM,(uint64_t)offset,whence,&f->api);
            int64_t base=whence==0?0:whence==1?0x80:FILE_SIZE;
            if(whence>2) CHECK(result==PX_FS_RESULT_BAD_WHENCE&&f->log_count==0);
            else if(base+offset<0) CHECK(result==PX_FS_RESULT_BAD_POSITION&&get64(f,STREAM+4)==0x80);
            else if(base+offset>FILE_SIZE) CHECK(result==PX_FS_RESULT_BAD_POSITION);
            else CHECK(result==0&&get64(f,STREAM+4)==(uint64_t)(base+offset));
            if(whence==2) CHECK(f->log[0]=='S'&&f->log[1]=='h');
            ++cases;
        }
    reset(f);f->fail_size=0xc8804464u;put64(f,STREAM+0x0c,0x55);CHECK(px_file_stream_seek(STREAM,0,2,&f->api)==0xc8804464u&&get64(f,STREAM+0x0c)==0&&f->size_calls==1);++cases;
    /* Chunked reads, short reads at end of file, and failures. */
    for(unsigned chunk=1;chunk<=9;++chunk)
        for(uint32_t start=FILE_SIZE-20;start<=FILE_SIZE;start+=5)
            for(uint32_t length=0;length<=24;length+=3) {
                reset(f);f->chunk=chunk;put64(f,STREAM+4,start);uint32_t count=0xffffffffu;
                CHECK(px_file_stream_read(STREAM,BUFFER,length,&count,&f->api)==0);
                const uint32_t expected=start+length>FILE_SIZE?FILE_SIZE-start:length;
                CHECK(count==expected&&get64(f,STREAM+4)==start+expected&&!memcmp(f->bytes+BUFFER,f->file+start,expected));
                CHECK(f->bytes[BUFFER+expected]==0xa5);
                unsigned calls=0;
                for(uint32_t left=length,at=start;left;) {
                    ++calls;uint32_t n=left<chunk?left:chunk;if(at+n>FILE_SIZE)n=at>=FILE_SIZE?0:FILE_SIZE-at;
                    if(n==left||n==0)break;
                    left-=n;at+=n;
                }
                CHECK(f->read_calls==calls);
                ++cases;
            }
    reset(f);f->chunk=4;f->fail_read_at=3;put64(f,STREAM+4,0x10);uint32_t count=0x1234;
    CHECK(px_file_stream_read(STREAM,BUFFER,32,&count,&f->api)==0xc8804478u&&count==0x1234&&get64(f,STREAM+4)==0x18);++cases;
    /* The stored position advances by the sign-extended count. */
    reset(f);f->giant=1;put64(f,STREAM+4,UINT64_C(0x100000010));
    CHECK(px_file_stream_read(STREAM,BUFFER,0x80000000u,&count,&f->api)==0&&count==0x80000000u&&get64(f,STREAM+4)==UINT64_C(0x80000010));++cases;
    reset(f);put32(f,STREAM,HANDLE|3);CHECK(px_file_stream_read(STREAM,BUFFER,1,&count,&f->api)==0&&f->last_handle==(HANDLE|2));++cases;
    printf("stream: %u size/position/seek/read cases incl. signed 64-bit bounds, refresh order, short reads and failures\n",cases);
}
static void members(struct fixture *f)
{
    unsigned cases=0;uint32_t info[3];
    for(unsigned i=0;i<8;++i) {
        reset(f);px_archive_member_info(ARCHIVE,i,info,&f->api);
        CHECK(info[0]==0x101+i*13&&info[1]==(0xdead0000u|i)&&info[2]==0x200*i+i&&f->info_calls==0);++cases;
    }
    /* Out-of-range indices spin on the hook until it becomes nonzero. */
    const uint32_t bad[]={8,9,0xffff,0x10000,0xffffffffu};
    for(unsigned i=0;i<5;++i) {
        reset(f);f->hook_after=4;
        if(!setjmp(f->escape)) { px_archive_member_info(ARCHIVE,bad[i],info,&f->api);CHECK(0); }
        CHECK(f->panics==1&&f->hook_reads==4);++cases;
    }
    reset(f);w16(f,ARCHIVE+0x34,0);f->hook_after=1;
    if(!setjmp(f->escape)) { px_archive_member_info(ARCHIVE,0,info,&f->api);CHECK(0); }++cases;
    reset(f);put32(f,ARCHIVE+0x4c,0x80000000u);w16(f,ARCHIVE+0x34,0);px_archive_member_info(ARCHIVE,77,info,&f->api);
    CHECK(f->info_calls==1&&info[2]==77*16);++cases;
    reset(f);put32(f,ARCHIVE+0x4c,0x7fffffffu);px_archive_member_info(ARCHIVE,1,info,&f->api);CHECK(f->info_calls==0&&info[0]==0x10e);++cases;
    /* Reads. */
    reset(f);f->open_value=0;CHECK(px_archive_read(ARCHIVE,16,BUFFER,&f->api)==0&&f->read_calls==0&&f->open_calls==1);++cases;
    reset(f);f->open_value=0x100;CHECK(px_archive_read(ARCHIVE,16,BUFFER,&f->api)==16);++cases;
    reset(f);f->fail_read_at=1;CHECK(px_archive_read(ARCHIVE,16,BUFFER,&f->api)==0&&f->last_site==0x127bcc&&f->last_fatal==0xc8804478u);++cases;
    for(unsigned i=0;i<8;++i) {
        reset(f);f->chunk=3+i;put64(f,STREAM+4,0x999);const uint32_t got=px_read_archive_member(ARCHIVE,i,BUFFER,&f->api);
        CHECK(got==0x101+i*13&&!memcmp(f->bytes+BUFFER,f->file+0x200*i+i,got)&&get64(f,STREAM+4)==0x200*i+i+got);
        CHECK(f->locks==1&&f->unlocks==1&&f->log[0]=='L'&&f->log[f->log_count-1]=='U'&&f->fatal_calls==0);++cases;
    }
    /* A failed seek is reported, then the read still runs from the old position. */
    reset(f);put32(f,ROWS+8,FILE_SIZE+4);put64(f,STREAM+4,0x40);
    CHECK(px_read_archive_member(ARCHIVE,0,BUFFER,&f->api)==0x101&&f->last_site==0x127ca4&&f->last_fatal==PX_FS_RESULT_BAD_POSITION);
    CHECK(!memcmp(f->bytes+BUFFER,f->file+0x40,0x101)&&f->unlocks==1);++cases;
    reset(f);px_archive_read_at(ARCHIVE,0x10,8,BUFFER,NULL,&f->api);CHECK(!memcmp(f->bytes+BUFFER,f->file+0x10,8));++cases;
    reset(f);uint32_t count=5;f->open_value=0;px_archive_read_at(ARCHIVE,0x10,8,BUFFER,&count,&f->api);CHECK(count==0&&f->unlocks==1);++cases;
    /* Member seeks add a 32-bit intra-member offset and take no lock. */
    for(unsigned i=0;i<8;++i)
        for(uint32_t offset=0;offset<0x40;offset+=0x13) {
            reset(f);px_seek_archive_member(ARCHIVE,i,offset,&f->api);
            CHECK(get64(f,STREAM+4)==0x200*i+i+offset&&f->locks==0&&f->fatal_calls==0);++cases;
        }
    reset(f);put32(f,ROWS+8,0xfffffff0u);put64(f,STREAM+4,3);px_seek_archive_member(ARCHIVE,0,0,&f->api);
    CHECK(f->last_site==0x138208&&f->last_fatal==PX_FS_RESULT_BAD_POSITION&&get64(f,STREAM+4)==3&&f->size_calls==0);++cases;
    reset(f);put32(f,ROWS+8,0xfffffff0u);px_seek_archive_member(ARCHIVE,0,0x20,&f->api);CHECK(get64(f,STREAM+4)==0x10&&f->fatal_calls==0);++cases;
    printf("archive: %u member-information, panic-hook, read, read-at and seek cases incl. lock order and fatal sites\n",cases);
}
static uint32_t curve(unsigned rate,unsigned level) { return level<2?0:level*level*(rate+3)+rate; }
static void growth_read(void *u,uint32_t archive,uint32_t index,uint32_t destination)
{ struct fixture *f=u;CHECK(px_read_archive_member(archive,index,destination,&f->api)==404); }
static void growth_select(void *u,uint32_t species,uint32_t form)
{
    struct fixture *f=u;const struct px_growth_api g={&f->memory,f,growth_read};px_select_growth_resource(species,form,&g);
}
static void integration(struct fixture *f)
{
    reset(f);f->chunk=100;w16(f,ARCHIVE+0x34,6);
    for(unsigned r=0;r<6;++r) {
        put32(f,ROWS+12*r,404);put32(f,ROWS+12*r+8,0x40+404*r);
        for(unsigned l=0;l<101;++l) { const uint32_t v=curve(r,l);memcpy(f->file+0x40+404*r+4*l,&v,4); }
    }
    enum { TABLE=0x10000,DESCRIPTOR=0x9000,PAYLOAD=0x9100,RECORD=0xa000,CACHE=0xb000,SLOTS=0xc000 };
    for(unsigned s=0;s<=721;++s) { for(unsigned i=0;i<64;++i)w8(f,TABLE+64*s+i,0);w8(f,TABLE+64*s+0x15,(uint8_t)(s%6)); }
    put32(f,0x5d177c,TABLE);put32(f,0x5d17a8,DESCRIPTOR);put32(f,DESCRIPTOR+8,PAYLOAD);put32(f,0x5d17ac,RECORD);
    put32(f,0x5d17b0,CACHE);put32(f,0x5e52b0,ARCHIVE);put32(f,RECORD+4,0xffffffffu);put32(f,CACHE+4,CACHE+0x10);
    w8(f,CACHE+8,2);w8(f,CACHE+9,0);
    for(unsigned i=0;i<2;++i) { put32(f,CACHE+0x10+4*i,SLOTS+0x200*i);put32(f,SLOTS+0x200*i+4,0xfffffff0u); }
    const struct px_data_api data={&f->memory,f,growth_select};unsigned cases=0;
    for(unsigned species=1;species<=24;++species)
        for(unsigned level=1;level<=100;level+=3) {
            CHECK(px_pokemon_level_from_growth(species,0,curve(species%6,level),&data)==level);++cases;
        }
    CHECK(f->fatal_calls==0&&f->locks==f->unlocks&&f->locks==24);
    printf("integration: %u level resolutions through growth selection, member lookup, locked seek and chunked platform reads\n",cases);
}
int main(void)
{
    struct fixture *f=calloc(1,sizeof *f);CHECK(f);f->bytes=calloc(1,SIZE);CHECK(f->bytes);
    streams(f);members(f);integration(f);
    puts("archive_io: all tests passed");free(f->bytes);free(f);return 0;
}
