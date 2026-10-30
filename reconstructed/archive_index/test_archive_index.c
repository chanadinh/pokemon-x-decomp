#include "archive_index.h"
#include "../pokemon_growth/pokemon_growth.h"
#include "../pokemon_data/pokemon_data.h"
#include "../pokemon_stats/stats_math.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x5f0000, ARCHIVE=0x1000, PARSER=0x2000, INDEX=0x3000, HEAP=0x40, TEMP=0x80, SCRATCH=0x900,
       ARENA=0x100000, FILE_MAX=0x20000, HANDLE=0x4440 };
struct fixture {
    uint8_t *bytes;uint8_t file[FILE_MAX];uint32_t file_size;
    struct px_pokemon_api memory;struct px_archive_api io;struct px_archive_index_api api;
    uint32_t next,enter_result,allocations,releases,released[16],signals,constructs,enter_calls,last_heap[8];
    char log[256];unsigned log_count;
};
static void note(struct fixture *f,char c) { if(f->log_count<255)f->log[f->log_count++]=c; }
static uint8_t r8(void *u,uint32_t a) { struct fixture *f=u;CHECK(a<SIZE);return f->bytes[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)((uint32_t)r8(u,a)|(uint32_t)r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { struct fixture *f=u;CHECK(a<SIZE);f->bytes[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static uint32_t allocate_object(void *u,uint32_t size,uint32_t heap) { struct fixture *f=u;CHECK(heap==HEAP&&size==0x78);note(f,'O');return 0x700; }
static void lock(void *u,uint32_t s) { (void)u;CHECK(s==ARCHIVE+0x60||s==0x700+0x60); }
static uint32_t is_open(void *u,uint32_t o) { (void)u;(void)o;return 1; }
static uint32_t get_size(void *u,uint32_t h,uint64_t *size) { struct fixture *f=u;CHECK(h==HANDLE);*size=f->file_size;return 0; }
static uint32_t file_read(void *u,uint32_t h,uint64_t position,uint32_t buffer,uint32_t size,uint32_t *count)
{
    struct fixture *f=u;CHECK(h==HANDLE);uint32_t n=size<0x30?size:0x30;
    if(position>=f->file_size)n=0;else if(position+n>f->file_size)n=(uint32_t)(f->file_size-position);
    for(uint32_t i=0;i<n;++i)w8(f,buffer+i,f->file[position+i]);
    *count=n;return 0;
}
static void fatal(void *u,uint32_t r,uint32_t s) { (void)u;fprintf(stderr,"fatal %08x %08x\n",r,s);exit(1); }
static void stream_info(void *u,uint32_t a,uint32_t i,uint32_t info[3]) { (void)u;(void)a;(void)i;(void)info;CHECK(0); }
static void panic(void *u,uint32_t h,uint32_t m) { (void)u;(void)h;(void)m;CHECK(0); }
static uint32_t try_enter(void *u,uint32_t s) { struct fixture *f=u;++f->enter_calls;CHECK(s%0x100==0x6c);note(f,'E');return f->enter_result; }
static void signal_event(void *u,uint32_t e) { struct fixture *f=u;CHECK(e%0x100==0x4c);++f->signals;note(f,'G'); }
static uint32_t temporary_heap(void *u,uint32_t heap) { struct fixture *f=u;CHECK(heap==HEAP);note(f,'T');return TEMP; }
static uint32_t alignment(void *u,uint32_t heap) { struct fixture *f=u;CHECK(heap==HEAP||heap==TEMP);note(f,'a');return 4; }
static uint32_t allocate(void *u,uint32_t heap,uint32_t size,uint32_t align,uint32_t flag)
{
    struct fixture *f=u;CHECK(align==4&&flag==1);note(f,heap==TEMP?'t':'h');
    if(f->allocations<8)f->last_heap[f->allocations]=heap;
    ++f->allocations;
    const uint32_t p=f->next;f->next+=(size+15)&~15u;CHECK(f->next<ARENA+0x80000);return p;
}
static void release(void *u,uint32_t p) { struct fixture *f=u;if(f->releases<16)f->released[f->releases]=p;++f->releases;note(f,'r'); }
static uint32_t construct(void *u,uint32_t object,uint32_t heap,uint32_t id,uint32_t flags)
{
    struct fixture *f=u;CHECK(object==0x700&&heap==HEAP&&id==0x1d&&flags==1);++f->constructs;note(f,'C');
    w32(f,object+4,HEAP);w32(f,object+0x18,HANDLE);w32(f,object+0x1c,0);w32(f,object+0x20,0);w32(f,object+0x24,0);w32(f,object+0x28,0);
    w32(f,object+0x3c,0);w32(f,object+0x4c,0xfffffffeu);return object;
}
/* GARC writer: entries list record counts per entry and selector bitmaps. */
struct record { uint32_t start,end,length; };
static uint32_t build_garc(struct fixture *f,unsigned entries,const uint32_t *bitmaps,struct record (*records)[32],uint32_t data_size)
{
    uint8_t *g=f->file;memset(g,0,FILE_MAX);uint32_t total=0;
    for(unsigned e=0;e<entries;++e)total+=(uint32_t)__builtin_popcount(bitmaps[e]);
    const uint32_t fato=0x1c,fatb=fato+12+4*entries;uint32_t p=fatb+12;
    memcpy(g,"CRAG",4);memcpy(g+fato,"OTAF",4);memcpy(g+fatb,"BTAF",4);
    g[fato+8]=(uint8_t)entries;g[fato+9]=(uint8_t)(entries>>8);
    memcpy(g+fatb+8,&total,4);
    for(unsigned e=0;e<entries;++e) {
        const uint32_t off=p-fatb-12;memcpy(g+fato+12+4*e,&off,4);memcpy(g+p,&bitmaps[e],4);p+=4;
        unsigned k=0;for(unsigned b=0;b<32;++b)if(bitmaps[e]&(1u<<b)) { memcpy(g+p,&records[e][k],12);p+=12;++k; }
    }
    const uint32_t fimb=p;memcpy(g+fimb,"BMIF",4);const uint32_t index_length=fimb+12;
    memcpy(g+0x10,&index_length,4);const uint32_t largest=0x1234567;memcpy(g+0x18,&largest,4);
    for(uint32_t i=0;i<data_size;++i)g[index_length+i]=(uint8_t)(i*13u+7);
    f->file_size=index_length+data_size;return index_length;
}
static void reset(struct fixture *f)
{
    uint8_t *bytes=f->bytes;memset(f,0,sizeof *f);f->bytes=bytes;memset(bytes,0xa5,0x10000);memset(bytes+ARENA,0x5a,0x80000);
    f->memory=(struct px_pokemon_api){.user=f,.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32,.allocate=allocate_object};
    f->io=(struct px_archive_api){&f->memory,f,lock,lock,is_open,get_size,file_read,fatal,stream_info,panic};
    f->api=(struct px_archive_index_api){&f->io,f,try_enter,signal_event,temporary_heap,alignment,allocate,release,construct,SCRATCH};
    f->next=ARENA;f->enter_result=1;
    w32(f,ARCHIVE+4,HEAP);w32(f,ARCHIVE+0x18,HANDLE);w32(f,ARCHIVE+0x1c,0);w32(f,ARCHIVE+0x20,0);
    w32(f,ARCHIVE+0x24,0);w32(f,ARCHIVE+0x28,0);w32(f,ARCHIVE+0x3c,0);w32(f,ARCHIVE+0x4c,0xfffffffeu);
    w32(f,PX_ARCHIVE_SELECTOR_CELL,0);
}
/* Independent model: ARM shifts use the low byte, so wide selectors revisit bits. */
static uint32_t popcount_below(uint32_t bits,uint32_t s)
{
    uint32_t n=0;
    for(uint32_t i=0;i<s;++i)if((i&0xff)<32)n+=(bits>>(i&0xff))&1;
    return n&0xffff;
}
static void parser_cases(struct fixture *f)
{
    unsigned cases=0;static struct record records[6][32];
    const uint32_t bitmaps[6]={1,0x3,0x80000001u,0xa5,0xffffffffu,0x10};
    for(unsigned e=0;e<6;++e)for(unsigned k=0;k<32;++k)records[e][k]=(struct record){0x100*e+16*k,0x100*e+16*k+12+k%4,0x100*e+16*k+9};
    reset(f);const uint32_t length=build_garc(f,6,bitmaps,records,0x800);
    for(uint32_t i=0;i<length;++i)w8(f,INDEX+i,f->file[i]);
    px_garc_parser_parse(PARSER,INDEX,&f->memory);
    CHECK(r32(f,PARSER+4)==INDEX&&r32(f,PARSER+8)==INDEX+0x1c&&r32(f,PARSER+0x0c)==INDEX+0x1c+12+24);
    CHECK(r32(f,PARSER+0x10)==INDEX+length-12&&px_garc_parser_count(PARSER,&f->memory)==6&&px_garc_parser_largest(PARSER,&f->memory)==0x1234567);
    CHECK(px_garc_parser_length_offset()==0x10);cases+=3;
    const uint32_t selectors[]={0,1,2,3,4,5,7,8,15,16,30,31,32,33,63,0x100,0x101,0x107,0x11f,0x120,0x1ff,0xffff};
    for(unsigned e=0;e<6;++e)
        for(unsigned s=0;s<sizeof selectors/sizeof *selectors;++s) {
            const uint32_t sel=selectors[s],low=sel&0xff;
            const uint32_t slot=(low<32&&(bitmaps[e]>>low&1))?popcount_below(bitmaps[e],sel):0;
            /* Slots past the entry's own records read the following FATB bytes;
             * that case is checked separately with a hand-built table. */
            if(slot>=(uint32_t)__builtin_popcount(bitmaps[e]))continue;
            const struct record *r=&records[e][slot];
            for(uint32_t wide=0;wide<2;++wide) {
                const uint32_t entry=e+(wide?0x10000u:0);
                CHECK(px_garc_parser_span(PARSER,entry,sel,&f->memory)==r->end-r->start);
                CHECK(px_garc_parser_length(PARSER,entry,sel,&f->memory)==r->length);
                CHECK(px_garc_parser_offset(PARSER,entry,sel,&f->memory)==length+r->start);cases+=3;
            }
        }
    /* A set selector bit counts bits only below the full-width selector. */
    reset(f);w32(f,PARSER+8,0x5000);w32(f,PARSER+0x0c,0x6000);w32(f,PARSER+4,0x7000);w32(f,0x5000+0x0c,0);
    w32(f,0x6000+0x0c,0x3);w32(f,0x7000+0x10,0x40);
    for(unsigned k=0;k<4;++k) { w32(f,0x6010+12*k,0x10*k);w32(f,0x6014+12*k,0x10*k+5);w32(f,0x6018+12*k,k); }
    /* Selector 0x101 tests bit 1, then counts bits 0, 1 and (at i=0x100) bit 0 again. */
    CHECK(px_garc_parser_length(PARSER,0,0x100,&f->memory)==2&&px_garc_parser_length(PARSER,0,0x101,&f->memory)==3);
    CHECK(px_garc_parser_offset(PARSER,0,0x101,&f->memory)==0x70&&px_garc_parser_span(PARSER,0,0x101,&f->memory)==5);cases+=4;
    w32(f,0x6000+0x0c,0x80000000u);CHECK(px_garc_parser_length(PARSER,0,31,&f->memory)==0);cases+=1;
    /* Wrapped 32-bit arithmetic. */
    w32(f,0x6010,0xfffffff0u);w32(f,0x6014,0x10);w32(f,0x7010,0x20);
    CHECK(px_garc_parser_span(PARSER,0,0,&f->memory)==0x20&&px_garc_parser_offset(PARSER,0,0,&f->memory)==0x10);cases+=2;
    printf("parser: %u parse/count/span/length/offset cases incl. 16-bit entries, ARM shift selectors and wrap\n",cases);
}
static void builder_cases(struct fixture *f)
{
    unsigned cases=0;static struct record records[40][32];uint32_t bitmaps[40];
    for(unsigned e=0;e<40;++e) {
        bitmaps[e]=e%5==0?0x1:e%5==1?0x3:e%5==2?0x6:e%5==3?0x80000001u:0xf;
        for(unsigned k=0;k<32;++k) { const uint32_t start=0x40*e+0x10*k;records[e][k]=(struct record){start,start+0x10-(k&3),start+0x0c-(k&3)}; }
    }
    for(uint32_t selector=0;selector<5;++selector) {
        reset(f);const uint32_t length=build_garc(f,40,bitmaps,records,0x40*40+0x40);w32(f,PX_ARCHIVE_SELECTOR_CELL,selector);
        px_build_archive_index(ARCHIVE,HEAP,&f->api);
        CHECK(!strncmp(f->log,"ETahratatah",11)&&f->log[f->log_count-1]=='G'&&f->log[f->log_count-2]=='r'&&f->log[f->log_count-3]=='r');
        CHECK(f->last_heap[0]==HEAP&&f->last_heap[1]==TEMP&&f->last_heap[2]==TEMP&&f->last_heap[3]==HEAP);
        const uint32_t table=r32(f,ARCHIVE+0x3c);CHECK(r16(f,ARCHIVE+0x34)==40&&r32(f,ARCHIVE+0x38)==0x1234567&&f->signals==1);
        CHECK(f->releases==3&&f->released[0]==ARENA&&f->released[1]==ARENA+0x20+((length+15)&~15u)&&f->released[2]==ARENA+0x20);
        for(unsigned e=0;e<40;++e) {
            const uint32_t low=selector;const uint32_t slot=(bitmaps[e]>>low&1)?popcount_below(bitmaps[e],low):0;
            const struct record *r=&records[e][slot];
            CHECK(r32(f,table+12*e)==r->end-r->start&&r32(f,table+12*e+4)==r->length&&r32(f,table+12*e+8)==length+r->start);++cases;
        }
        /* The built table drives reconstructed member reads: span bytes from the offset. */
        w32(f,ARCHIVE+0x4c,1);
        for(unsigned e=0;e<40;e+=7) {
            const struct record *r=&records[e][(bitmaps[e]>>selector&1)?popcount_below(bitmaps[e],selector):0];
            CHECK(px_read_archive_member(ARCHIVE,e,0x9000,&f->io)==r->end-r->start&&!memcmp(f->bytes+0x9000,f->file+length+r->start,r->end-r->start));
            px_read_archive_member_part(ARCHIVE,e,2,3,0x9100,&f->io);CHECK(!memcmp(f->bytes+0x9100,f->file+length+r->start+2,3));cases+=2;
        }
    }
    /* A second build by the same owner frees the previous table first. */
    reset(f);build_garc(f,40,bitmaps,records,0x40*40+0x40);px_build_archive_index(ARCHIVE,HEAP,&f->api);
    const uint32_t first=r32(f,ARCHIVE+0x3c);px_build_archive_index(ARCHIVE,HEAP,&f->api);
    CHECK(f->releases==7&&f->released[3]!=first&&f->released[4]==first&&r32(f,ARCHIVE+0x3c)!=first&&f->signals==2);++cases;
    /* No entry: nothing else happens. */
    reset(f);f->enter_result=0;px_build_archive_index(ARCHIVE,HEAP,&f->api);CHECK(!strcmp(f->log,"E")&&r32(f,ARCHIVE+0x3c)==0);++cases;
    /* The selector cell is reread for every row word. */
    printf("builder: %u table/read cases over five selectors; call order, heaps, releases, rebuild and skipped entry\n",cases);
}
static uint32_t curve(unsigned rate,unsigned level) { return level<2?0:level*level*(rate+5)+rate; }
static struct fixture *growth_fixture;
static void growth_read(void *u,uint32_t archive,uint32_t index,uint32_t destination)
{ struct fixture *f=u;CHECK(px_read_archive_member(archive,index,destination,&f->io)==404); }
static void growth_select(void *u,uint32_t species,uint32_t form)
{ struct fixture *f=u;const struct px_growth_api g={&f->memory,f,growth_read};px_select_growth_resource(species,form,&g); }
static void integration(struct fixture *f)
{
    reset(f);static struct record records[6][32];uint32_t bitmaps[6];
    for(unsigned r=0;r<6;++r) { bitmaps[r]=1;records[r][0]=(struct record){404*r,404*r+404,404}; }
    const uint32_t length=build_garc(f,6,bitmaps,records,404*6);
    for(unsigned r=0;r<6;++r)for(unsigned l=0;l<101;++l) { const uint32_t v=curve(r,l);memcpy(f->file+length+404*r+4*l,&v,4); }
    w32(f,PX_GROWTH_ARCHIVE_CELL,0);px_open_resource_archive(PX_GROWTH_ARCHIVE_CELL,HEAP,0x1d,&f->api);
    CHECK(r32(f,PX_GROWTH_ARCHIVE_CELL)==0x700&&f->constructs==1&&f->log[0]=='O'&&f->log[1]=='C'&&f->log[2]=='E');
    px_open_resource_archive(PX_GROWTH_ARCHIVE_CELL,HEAP,0x1d,&f->api);CHECK(f->constructs==1&&f->signals==1);
    w32(f,0x700+0x4c,1);
    enum { TABLE=0x10000,DESCRIPTOR=0xa000,PAYLOAD=0xa100,RECORD=0xb000,CACHE=0xc000,SLOTS=0xd000 };
    for(unsigned s=0;s<=721;++s) { for(unsigned i=0;i<64;++i)w8(f,TABLE+64*s+i,0);w8(f,TABLE+64*s+0x15,(uint8_t)(s%6)); }
    w32(f,0x5d177c,TABLE);w32(f,0x5d17a8,DESCRIPTOR);w32(f,DESCRIPTOR+8,PAYLOAD);w32(f,0x5d17ac,RECORD);w32(f,0x5d17b0,CACHE);
    w32(f,RECORD+4,0xffffffffu);w32(f,CACHE+4,CACHE+0x10);w8(f,CACHE+8,1);w8(f,CACHE+9,0);w32(f,CACHE+0x10,SLOTS);w32(f,SLOTS+4,0xfffffff0u);
    const struct px_data_api data={&f->memory,f,growth_select};unsigned cases=0;
    for(unsigned species=1;species<=12;++species)for(unsigned level=1;level<=100;level+=9) {
        CHECK(px_pokemon_level_from_growth(species,0,curve(species%6,level),&data)==level);++cases;
    }
    printf("integration: archive opened from synthetic GARC bytes, index built and %u level resolutions through growth loading\n",cases);
}
int main(void)
{
    struct fixture *f=calloc(1,sizeof *f);CHECK(f);f->bytes=calloc(1,SIZE);CHECK(f->bytes);growth_fixture=f;
    parser_cases(f);builder_cases(f);integration(f);
    puts("archive_index: all tests passed");free(f->bytes);free(f);return 0;
}
