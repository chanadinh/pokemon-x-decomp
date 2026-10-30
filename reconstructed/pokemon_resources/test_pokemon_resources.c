#include "pokemon_resources.h"
#include "../pokemon_personal/pokemon_personal.h"
#include "../pokemon_growth/pokemon_growth.h"
#include "../pokemon_learnset/pokemon_learnset.h"
#include "../pokemon_data/pokemon_data.h"
#include "../pokemon_stats/stats_math.h"
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x600000, HEAP=0x40, SCRATCH=0x900, CONFIG=0xa00, ARENA=0x100000, ARENA_END=0x500000, HANDLE=0x7000,
       FILES=8, FILE_MAX=0x40000, SPECIES=730, ID_PERSONAL=10, ID_LEARNSET=11, ID_GROWTH=12, ID_A=13, ID_B=14,
       ID_TAIL=16, ID_MESSAGE=20 };
struct file { uint8_t bytes[FILE_MAX];uint32_t size; };
struct fixture {
    uint8_t *bytes;struct file *files;struct px_pokemon_api memory;struct px_archive_api io;
    struct px_archive_index_api index;struct px_resource_api api;
    uint32_t next,tail_allocations,fail_size,sizes[512],size_count,messages[3][6],message_count,blocks,panics,constructs[8],construct_count;
    int retarget;jmp_buf escape;
};
static struct fixture *active;
static uint8_t r8(void *u,uint32_t a) { struct fixture *f=u;CHECK(a<SIZE);return f->bytes[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)((uint32_t)r8(u,a)|(uint32_t)r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { struct fixture *f=u;CHECK(a<SIZE);f->bytes[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static uint32_t le32(const uint8_t *p) { return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24; }
static uint32_t bump(struct fixture *f,uint32_t size) { const uint32_t p=f->next;f->next+=(size+15)&~15u;CHECK(f->next<ARENA_END);memset(f->bytes+p,0xcc,size);return p; }
static uint32_t object_allocate(void *u,uint32_t size,uint32_t heap)
{
    struct fixture *f=u;CHECK(heap==HEAP);if(f->size_count<512)f->sizes[f->size_count++]=size;
    if(size==f->fail_size)return 0;
    if(f->retarget&&size==0x40) { f->retarget=0;w32(f,0x9100+4,0x9200); }
    return bump(f,size);
}
static struct file *file_of(struct fixture *f,uint32_t handle) { CHECK(handle>=HANDLE&&handle<HANDLE+2*FILES&&handle%2==0);return &f->files[(handle-HANDLE)/2]; }
static void lock(void *u,uint32_t s) { (void)u;(void)s; }
static uint32_t is_open(void *u,uint32_t o) { (void)u;(void)o;return 1; }
static uint32_t get_size(void *u,uint32_t h,uint64_t *size) { *size=file_of(u,h)->size;return 0; }
static uint32_t file_read(void *u,uint32_t h,uint64_t position,uint32_t buffer,uint32_t size,uint32_t *count)
{
    struct fixture *f=u;const struct file *file=file_of(f,h);uint32_t n=size<0x200?size:0x200;
    if(position>=file->size)n=0;else if(position+n>file->size)n=(uint32_t)(file->size-position);
    for(uint32_t i=0;i<n;++i)w8(f,buffer+i,file->bytes[position+i]);
    *count=n;return 0;
}
static void fatal(void *u,uint32_t r,uint32_t s) { (void)u;fprintf(stderr,"fatal %08x %08x\n",r,s);exit(1); }
static void panic(void *u,uint32_t hook,uint32_t message) { struct fixture *f=u;CHECK(hook==0x7777&&message==PX_ARCHIVE_PANIC_MESSAGE);++f->panics;longjmp(f->escape,1); }
static void stream_info(void *u,uint32_t archive,uint32_t index,uint32_t info[3]) { struct fixture *f=u;px_archive_member_info_streamed(archive,index,info,&f->index); }
static uint32_t try_enter(void *u,uint32_t s) { (void)u;(void)s;return 1; }
static void signal_event(void *u,uint32_t e) { w32(u,e,1); } /* stands in for the light event becoming signaled */
static uint32_t temporary_heap(void *u,uint32_t h) { (void)u;CHECK(h==HEAP);return HEAP; }
static uint32_t alignment(void *u,uint32_t h) { (void)u;CHECK(h==HEAP);return 4; }
static uint32_t heap_allocate(void *u,uint32_t h,uint32_t size,uint32_t a,uint32_t flag)
{ struct fixture *f=u;CHECK(h==HEAP&&(a==4||a==0xfffffffcu)&&flag==1);if(a==0xfffffffcu)++f->tail_allocations;return bump(f,size); }
static void release(void *u,uint32_t p) { (void)u;(void)p; }
static uint32_t construct(void *u,uint32_t object,uint32_t heap,uint32_t id,uint32_t flags)
{
    struct fixture *f=u;CHECK(heap==HEAP&&flags==1);if(f->construct_count<8)f->constructs[f->construct_count++]=id;
    CHECK(id>=ID_PERSONAL&&id<ID_PERSONAL+FILES);
    w32(f,object+4,HEAP);w32(f,object+0x18,HANDLE+2*(id-ID_PERSONAL)); /* bit 0 is a stream flag */
    for(uint32_t o=0x1c;o<0x2c;o+=4)w32(f,object+o,0);
    w32(f,object+0x3c,0);w16(f,object+0x34,0);w32(f,object+0x4c,0xfffffffeu);return object;
}
static uint32_t allocate_block(void *u,uint32_t heap,uint32_t size) { struct fixture *f=u;CHECK(heap==HEAP);++f->blocks;return bump(f,size); }
static uint32_t construct_message(void *u,uint32_t object,uint32_t archive,uint32_t member,uint32_t heap,uint32_t mode,uint32_t flags)
{
    struct fixture *f=u;CHECK(f->message_count<3);uint32_t *m=f->messages[f->message_count++];
    m[0]=object;m[1]=archive;m[2]=member;m[3]=heap;m[4]=mode;m[5]=flags;return object;
}
/* GARC writer with single-record entries; data members are supplied by fill. */
static void write_garc(struct file *file,uint32_t entries,uint32_t (*member_size)(uint32_t),void (*fill)(uint32_t,uint8_t *))
{
    uint8_t *g=file->bytes;memset(g,0,FILE_MAX);const uint32_t fato=0x1c,fatb=fato+12+4*entries;uint32_t p=fatb+12;
    memcpy(g,"CRAG",4);memcpy(g+4,"\x1c\0\0\0",4);memcpy(g+fato,"OTAF",4);
    const uint32_t fato_size=12+4*entries;memcpy(g+fato+4,&fato_size,4);g[fato+8]=(uint8_t)entries;g[fato+9]=(uint8_t)(entries>>8);
    memcpy(g+fatb,"BTAF",4);memcpy(g+fatb+8,&entries,4);uint32_t data=0;
    for(uint32_t e=0;e<entries;++e) {
        const uint32_t off=p-fatb-12,one=1,size=member_size(e),end=data+((size+3)&~3u);memcpy(g+fato+12+4*e,&off,4);
        memcpy(g+p,&one,4);memcpy(g+p+4,&data,4);memcpy(g+p+8,&end,4);memcpy(g+p+12,&size,4);p+=16;data=end;
    }
    memcpy(g+p,"BMIF",4);const uint32_t length=p+12;memcpy(g+0x10,&length,4);
    const uint32_t largest=0x400;memcpy(g+0x18,&largest,4);data=0;
    for(uint32_t e=0;e<entries;++e) { fill(e,g+length+data);data+=(member_size(e)+3)&~3u; }
    file->size=length+data;CHECK(file->size<=FILE_MAX);
}
static uint8_t personal_byte(uint32_t s,uint32_t i) { return i==0x15?(uint8_t)(s%6):i==0x1c||i==0x1d||i==0x20?0:(uint8_t)(s*7+i); }
static uint32_t personal_size(uint32_t e) { return e<SPECIES?64:SPECIES*64; }
static void personal_fill(uint32_t e,uint8_t *p)
{
    if(e<SPECIES) { for(uint32_t i=0;i<64;++i)p[i]=personal_byte(e,i);return; }
    for(uint32_t s=0;s<SPECIES;++s)for(uint32_t i=0;i<64;++i)p[64*s+i]=personal_byte(s,i);
}
static uint32_t learnset_size(uint32_t e) { return 4*(e%5+1)+4; }
static void learnset_fill(uint32_t e,uint8_t *p)
{
    uint32_t k=0;for(;k<e%5+1;++k) { const uint16_t move=(uint16_t)(e+k*3),level=(uint16_t)(k*10+1);memcpy(p+4*k,&move,2);memcpy(p+4*k+2,&level,2); }
    memset(p+4*k,0xff,4);
}
static uint32_t curve(uint32_t rate,uint32_t level) { return level<2?0:level*level*(rate+4)+rate; }
static uint32_t growth_size(uint32_t e) { (void)e;return 404; }
static void growth_fill(uint32_t e,uint8_t *p) { for(uint32_t l=0;l<101;++l) { const uint32_t v=curve(e,l);memcpy(p+4*l,&v,4); } }
static uint32_t small_size(uint32_t e) { return 8+e; }
static void small_fill(uint32_t e,uint8_t *p) { for(uint32_t i=0;i<8+e;++i)p[i]=(uint8_t)(e+i); }
static uint32_t tail_size(uint32_t e) { return e<722?8:722*2; }
static void tail_fill(uint32_t e,uint8_t *p) { if(e<722) { memset(p,(int)e,8);return; } for(uint32_t i=0;i<722;++i) { const uint16_t v=(uint16_t)(i*3+1);memcpy(p+2*i,&v,2); } }
static void reset(struct fixture *f)
{
    uint8_t *bytes=f->bytes;struct file *files=f->files;memset(f,0,sizeof *f);f->bytes=bytes;f->files=files;
    memset(bytes,0,SIZE);active=f;f->next=ARENA;
    f->memory=(struct px_pokemon_api){.user=f,.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32,.allocate=object_allocate};
    f->io=(struct px_archive_api){&f->memory,f,lock,lock,is_open,get_size,file_read,fatal,stream_info,panic};
    f->index=(struct px_archive_index_api){&f->io,f,try_enter,signal_event,temporary_heap,alignment,heap_allocate,release,construct,SCRATCH};
    f->api=(struct px_resource_api){&f->index,f,allocate_block,construct_message};
    w32(f,PX_ARCHIVE_PANIC_HOOK,0x7777);w32(f,PX_RESOURCE_CONFIG_CELL,CONFIG);
    const uint32_t words[21]={ID_PERSONAL,ID_LEARNSET,ID_GROWTH,ID_A,ID_B,0,ID_TAIL,0x103,5,3,4,2,0,0,0,0,0,ID_MESSAGE,30,31,32};
    for(uint32_t i=0;i<21;++i)w32(f,CONFIG+4*i,words[i]);
}
static void build_files(struct fixture *f)
{
    write_garc(&f->files[ID_PERSONAL-ID_PERSONAL],SPECIES+1,personal_size,personal_fill);
    write_garc(&f->files[ID_LEARNSET-ID_PERSONAL],SPECIES,learnset_size,learnset_fill);
    write_garc(&f->files[ID_GROWTH-ID_PERSONAL],8,growth_size,growth_fill);
    write_garc(&f->files[ID_A-ID_PERSONAL],5,small_size,small_fill);
    write_garc(&f->files[ID_B-ID_PERSONAL],3,small_size,small_fill);
    write_garc(&f->files[ID_TAIL-ID_PERSONAL],723,tail_size,tail_fill);
}
static void constructors(struct fixture *f)
{
    unsigned cases=0;reset(f);
    const uint32_t d=px_construct_personal_descriptor(0x9000,HEAP,&f->api);
    CHECK(d==0x9000&&r32(f,d)==0x5989c0&&r16(f,d+4)==0&&r8(f,d+6)==0&&f->sizes[0]==0x40);
    for(uint32_t i=0;i<64;++i)CHECK(r8(f,r32(f,d+8)+i)==0);
    cases+=2;
    w32(f,0x9010+4,7);CHECK(px_construct_growth_record(0x9010,&f->api)==0x9010&&r32(f,0x9010)==0x598a40&&r32(f,0x9014)==0xffffffffu);
    for(uint32_t i=0;i<0x194;++i)CHECK(r8(f,0x9018+i)==0);
    ++cases;
    px_construct_learnset_record(0x9200,HEAP,&f->api);CHECK(r16(f,0x9200)==0&&r8(f,0x9202)==0&&r8(f,0x9208)==0&&f->sizes[1]==0x68);
    for(uint32_t i=0;i<0x68;++i)CHECK(r8(f,r32(f,0x9204)+i)==0);
    ++cases;
    px_construct_record_30(0x9300,HEAP,&f->api);CHECK(r32(f,0x9300)==0x5989d0&&r16(f,0x9304)==0&&f->sizes[2]==0x30&&r8(f,r32(f,0x9308)+0x2f)==0);++cases;
    px_construct_record_18(0x9400,HEAP,&f->api);CHECK(r32(f,0x9400)==0x5989f0&&f->sizes[3]==0x18&&r32(f,r32(f,0x9408)+0x14)==0);++cases;
    px_construct_halfword_buffer(0x9500,0x10100,HEAP,&f->api);
    CHECK(r32(f,0x9500)==0x598570&&r16(f,0x9508)==0x100&&r16(f,0x950a)==0&&r8(f,0x950c)==1&&r16(f,r32(f,0x9504))==0);++cases;
    /* Failed payload allocation leaves a zero pointer and, for the halfword buffer, a clear flag. */
    reset(f);f->fail_size=0x40;px_construct_personal_descriptor(0x9000,HEAP,&f->api);CHECK(r32(f,0x9008)==0);
    f->fail_size=0x68;px_construct_learnset_record(0x9200,HEAP,&f->api);CHECK(r32(f,0x9204)==0);cases+=2;
    /* Caches: byte or word capacity, every slot built, table reloaded per store. */
    reset(f);px_construct_growth_cache(0x9600,HEAP,0x103,&f->api);
    CHECK(r32(f,0x9600)==0x5989e0&&r8(f,0x9608)==3&&r8(f,0x9609)==0);
    for(uint32_t i=0;i<0x103;++i)CHECK(r32(f,r32(f,r32(f,0x9604)+4*i))==0x598a40);
    ++cases;
    reset(f);px_construct_personal_cache(0x9100,HEAP,4,&f->api);
    CHECK(r32(f,0x9100)==0x598a00&&r8(f,0x9108)==4);for(uint32_t i=0;i<4;++i)CHECK(r32(f,r32(f,r32(f,0x9104)+4*i))==0x5989c0);++cases;
    reset(f);f->retarget=1;px_construct_personal_cache(0x9100,HEAP,2,&f->api);
    CHECK(r32(f,0x9104)==0x9200&&r32(f,0x9200)!=0&&r32(f,0x9204)!=0);++cases;
    reset(f);px_construct_learnset_cache(0x9700,HEAP,3,&f->api);
    CHECK(r32(f,0x9700)==0x598a10&&r32(f,0x9708)==3&&r32(f,0x970c)==0&&r32(f,r32(f,r32(f,0x9704)+8)+4)!=0);++cases;
    px_construct_cache_30(0x9800,HEAP,2,&f->api);CHECK(r32(f,0x9800)==0x598a20&&r32(f,r32(f,r32(f,0x9804)))==0x5989d0);
    px_construct_cache_18(0x9900,HEAP,0,&f->api);CHECK(r32(f,0x9900)==0x598a30&&r32(f,0x9908)==0);cases+=2;
    reset(f);f->fail_size=0xc;px_construct_learnset_cache(0x9700,HEAP,2,&f->api);CHECK(r32(f,r32(f,0x9704))==0&&r32(f,r32(f,0x9704)+4)==0);++cases;
    printf("constructors: %u record, buffer and cache cases incl. failed allocations, capacity widths and table reload\n",cases);
}
static void archives(struct fixture *f)
{
    volatile unsigned cases=0;reset(f);build_files(f);
    construct(f,0xa800,HEAP,ID_A,1);
    CHECK(px_archive_member_count(0xa800,&f->index)==5);
    uint32_t info[3];
    for(uint32_t i=0;i<5;++i) { px_archive_member_info(0xa800,i,info,&f->io);CHECK(info[0]==((8+i+3)&~3u)&&info[1]==8+i);CHECK(px_archive_member_size(0xa800,i,&f->io)==info[0]);++cases; }
    /* Each streamed lookup allocates its index buffer from the heap tail. */
    CHECK(f->tail_allocations==10);++cases;
    if(!setjmp(f->escape)) { px_archive_member_info(0xa800,5,info,&f->io);CHECK(0); }CHECK(f->panics==1);++cases;
    px_build_archive_index(0xa800,HEAP,&f->index);w16(f,0xa800+0x34,77);CHECK(px_archive_member_count(0xa800,&f->index)==77);++cases;
    CHECK(px_garc_parser_count_offset()==0x24);++cases;
    /* Personal table: loaded once from the flat member; size checks abort. */
    reset(f);build_files(f);px_load_personal_table(HEAP,ID_PERSONAL,&f->api);
    const uint32_t table=r32(f,0x5d177c);CHECK(table&&f->blocks==1&&!memcmp(f->bytes+table,f->files[0].bytes+le32(f->files[0].bytes+0x10)+SPECIES*64,SPECIES*64));
    px_load_personal_table(HEAP,ID_PERSONAL,&f->api);CHECK(f->blocks==1&&f->construct_count==1);cases+=2;
    reset(f);build_files(f);
    { uint8_t *g=f->files[0].bytes;const uint32_t fatb=0x1c+12+4*(SPECIES+1);uint32_t end;memcpy(&end,g+fatb+12+16*SPECIES+8,4);end+=4;memcpy(g+fatb+12+16*SPECIES+8,&end,4); }
    if(!setjmp(f->escape)) { px_load_personal_table(HEAP,ID_PERSONAL,&f->api);CHECK(0); }CHECK(f->panics==1&&f->blocks==0);++cases;
    reset(f);build_files(f);
    { uint8_t *g=f->files[0].bytes;const uint32_t fatb=0x1c+12+4*(SPECIES+1);uint32_t end;memcpy(&end,g+fatb+12+8,4);end+=4;memcpy(g+fatb+12+8,&end,4); }
    if(!setjmp(f->escape)) { px_load_personal_table(HEAP,ID_PERSONAL,&f->api);CHECK(0); }CHECK(f->panics==1&&f->blocks==1);++cases;
    /* Learnset archive and descriptor: indexed once; an existing descriptor is kept. */
    reset(f);build_files(f);px_open_learnset_resources(HEAP,ID_LEARNSET,&f->api);
    const uint32_t archive=r32(f,0x5d17ec);CHECK(archive&&r16(f,archive+0x34)==SPECIES&&r32(f,r32(f,0x5d17e8))==0x5989c0);
    w32(f,0x5d17e8,0x1234);px_open_learnset_resources(HEAP,ID_LEARNSET,&f->api);CHECK(r32(f,0x5d17e8)==0x1234&&f->construct_count==1);cases+=2;
    printf("archives: %u member count/info, abort, personal-table and learnset-open cases\n",cases);
}
static void select_personal(void *u,uint32_t record,uint32_t species,uint32_t form) { struct fixture *f=u;px_load_personal_record(record,species,form,&f->memory); }
static uint32_t personal_value(void *u,uint32_t record,uint32_t selector) { struct fixture *f=u;return px_personal_resource_field(record,selector,&f->memory); }
static void read_member(void *u,uint32_t archive,uint32_t index,uint32_t destination) { struct fixture *f=u;(void)px_read_archive_member(archive,index,destination,&f->io); }
static void select_growth(void *u,uint32_t species,uint32_t form)
{ struct fixture *f=u;const struct px_growth_api g={&f->memory,f,read_member};px_select_growth_resource(species,form,&g); }
static void initialization(struct fixture *f)
{
    unsigned cases=0;reset(f);build_files(f);px_initialize_pokemon_resources(HEAP,&f->api);
    CHECK(f->construct_count==6&&f->constructs[0]==ID_PERSONAL&&f->constructs[1]==ID_LEARNSET&&f->constructs[2]==ID_GROWTH);
    CHECK(f->constructs[3]==ID_A&&f->constructs[4]==ID_B&&f->constructs[5]==ID_TAIL);++cases;
    /* Object allocation sizes, in order, through 00122920. */
    uint32_t expected[512];unsigned n=0;
    const uint32_t front[]={0x78,0x78,0x0c,0x40,0x78,0x78,0x78,0x10,0x28,0x28,0x28,0x0c,0x40,0x0c};
    for(unsigned i=0;i<sizeof front/sizeof *front;++i)expected[n++]=front[i];
    for(unsigned i=0;i<3;++i) { expected[n++]=0x0c;expected[n++]=0x40; }
    expected[n++]=0x19c;expected[n++]=0x0c;for(unsigned i=0;i<5;++i)expected[n++]=0x19c;
    expected[n++]=0x0c;expected[n++]=0x68;expected[n++]=0x10;for(unsigned i=0;i<3;++i) { expected[n++]=0x0c;expected[n++]=0x68; }
    expected[n++]=0x0c;expected[n++]=0x30;expected[n++]=0x10;for(unsigned i=0;i<4;++i) { expected[n++]=0x0c;expected[n++]=0x30; }
    expected[n++]=0x0c;expected[n++]=0x18;expected[n++]=0x10;for(unsigned i=0;i<2;++i) { expected[n++]=0x0c;expected[n++]=0x18; }
    expected[n++]=0x0c;expected[n++]=0x40;expected[n++]=0x0c;expected[n++]=0x18;expected[n++]=0x78;
    CHECK(f->size_count==n);for(unsigned i=0;i<n;++i)CHECK(f->sizes[i]==expected[i]);++cases;
    const uint32_t b=PX_RESOURCE_BASE;
    CHECK(r32(f,r32(f,b))==0x598570&&r16(f,r32(f,b)+8)==0x100);
    for(unsigned i=0;i<3;++i)CHECK(f->messages[i][0]==r32(f,b+4+4*i)&&f->messages[i][1]==ID_MESSAGE&&f->messages[i][2]==30+i&&f->messages[i][4]==(i==0)&&f->messages[i][5]==5);
    CHECK(r32(f,r32(f,b+0x10))==0x598a00&&r8(f,r32(f,b+0x10)+8)==3&&r32(f,r32(f,b+0x14))==0x5989c0&&r32(f,r32(f,b+0x18))==0x5989c0);
    CHECK(r32(f,r32(f,b+0x1c))==0x598a40&&r32(f,r32(f,b+0x20))==0x5989e0&&r8(f,r32(f,b+0x20)+8)==5);
    CHECK(r32(f,r32(f,b+0x28))==0x598a10&&r32(f,r32(f,b+0x28)+8)==3&&r32(f,r32(f,b+0x2c))==0x5989d0&&r32(f,r32(f,b+0x30))==0x598a20);
    CHECK(r32(f,r32(f,b+0x34))==0x5989f0&&r32(f,r32(f,b+0x38))==0x5989f0&&r32(f,r32(f,b+0x3c))==0x598a30);
    const uint8_t *tail=f->files[ID_TAIL-ID_PERSONAL].bytes;
    CHECK(!memcmp(f->bytes+r32(f,b+0x44),tail+le32(tail+0x10)+722*8,722*2)&&f->blocks==2);cases+=6;
    /* Every reconstructed service now runs from the initialized state alone. */
    for(uint32_t s=0;s<SPECIES&&s<=721;s+=7) {
        px_select_personal_resource(s,0,&f->memory);
        for(uint32_t sel=0;sel<8;++sel)CHECK(px_current_personal_value(sel,&f->memory)==personal_byte(s,sel));
        ++cases;
    }
    w32(f,r32(f,PX_GROWTH_ARCHIVE_CELL)+0x4c,1);
    const struct px_data_api data={&f->memory,f,select_growth};
    for(uint32_t s=1;s<=60;++s)for(uint32_t level=1;level<=100;level+=11) { CHECK(px_pokemon_level_from_growth(s,0,curve(s%6,level),&data)==level);++cases; }
    const struct px_learnset_api learnsets={&f->memory,f,select_personal,personal_value,read_member};
    /* Fresh records and cache slots read as species 0, form 0, so a species-0
     * lookup after boot matches an empty slot and loads nothing. */
    px_select_pokemon_learnset(0,0,&learnsets);CHECK(px_learnset_record_count(r32(f,0x5d17b4),&f->memory)==0);++cases;
    for(uint32_t s=1;s<=721;s+=13) {
        px_select_pokemon_learnset(s,0,&learnsets);const uint32_t record=r32(f,0x5d17b4);
        CHECK(px_learnset_record_count(record,&f->memory)==s%5+1);
        for(uint32_t k=0;k<=s%5;++k)CHECK(px_learnset_record_move(record,k,&f->memory)==s+k*3&&px_learnset_record_level(record,k,&f->memory)==k*10+1);
        ++cases;
    }
    printf("initialization: %u allocation-order, cell, message and end-to-end personal/growth/learnset cases\n",cases);
}
int main(void)
{
    struct fixture *f=calloc(1,sizeof *f);CHECK(f);f->bytes=calloc(1,SIZE);f->files=calloc(FILES,sizeof *f->files);CHECK(f->bytes&&f->files);
    constructors(f);archives(f);initialization(f);
    puts("pokemon_resources: all tests passed");free(f->files);free(f->bytes);free(f);return 0;
}
