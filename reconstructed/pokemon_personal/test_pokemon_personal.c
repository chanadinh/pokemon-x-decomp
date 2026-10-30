#include "pokemon_personal.h"
#include "../pokemon_learnset/pokemon_learnset.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x5e0000, RECORD=0x100, RECORD2=0x120, PAYLOAD=0x400,
       PAYLOAD2=0x500, LEARNSET=0x600, ENTRIES=0x700, TABLE=0x10000,
       TABLE_CELL=0x5d177c, CURRENT=0x5d17a4, LEARNSET_PERSONAL=0x5d17e8,
       LEARNSET_ARCHIVE=0x5d17ec };
struct fixture { uint8_t *bytes;unsigned table_reads,word_reads,source_reads,group_writes;uint32_t member;struct px_pokemon_api memory; };
static uint8_t r8(void *u,uint32_t a) { struct fixture *f=u;CHECK(a<SIZE);return f->bytes[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)((uint32_t)r8(u,a)|(uint32_t)r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a)
{
    struct fixture *f=u;if(a==TABLE_CELL)++f->table_reads;
    if(a>=TABLE&&a<TABLE+0x80000)++f->source_reads;
    ++f->word_reads;return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16;
}
static void w8(void *u,uint32_t a,uint8_t v) { struct fixture *f=u;CHECK(a<SIZE);f->bytes[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v)
{ struct fixture *f=u;if(a>=PAYLOAD&&a<PAYLOAD+64)++f->group_writes;w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static void reset(struct fixture *f)
{
    memset(f->bytes,0xa5,0x1000);f->table_reads=f->word_reads=f->source_reads=f->group_writes=0;
    f->memory=(struct px_pokemon_api){.user=f,.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
    w32(f,TABLE_CELL,TABLE);w32(f,CURRENT,RECORD);w32(f,RECORD+8,PAYLOAD);w32(f,RECORD2+8,PAYLOAD2);
}
static uint32_t field_reference(const uint8_t *p,unsigned selector)
{
    static const uint8_t offsets[46]={0,1,2,3,4,5,6,7,8,34,10,10,10,10,10,10,10,
        12,14,16,18,19,20,21,22,23,24,25,26,27,28,30,32,33,33,33,9,36,38,40,44,48,52,56,60,62};
    static const uint8_t widths[46]={1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,
        2,2,2,1,1,1,1,1,1,1,1,1,1,2,2,1,1,1,1,1,2,2,4,4,4,4,4,2,1};
    if(selector>=46)return 0;
    uint32_t value=0;for(unsigned i=0;i<widths[selector];++i)value|=(uint32_t)p[offsets[selector]+i]<<(8*i);
    if(selector>=10&&selector<=15)return (value>>(2*(selector-10)))&3;
    if(selector==16)return (value>>12)&1;
    if(selector==33)return value%64;
    if(selector==34)return value/64%2;
    if(selector==35)return value/128;
    return value;
}
static void fields(struct fixture *f)
{
    reset(f);unsigned cases=0;
    for(unsigned v=0;v<256;++v) {
        for(unsigned i=0;i<64;++i)w8(f,PAYLOAD+i,(uint8_t)(v*(2*i+1)+17*i));
        for(unsigned s=0;s<46;++s) { CHECK(px_personal_resource_field(RECORD,s,&f->memory)==field_reference(f->bytes+PAYLOAD,s));++cases; }
    }
    for(unsigned v=0;v<=65535;++v) {
        w16(f,PAYLOAD+10,(uint16_t)v);
        for(unsigned s=10;s<=16;++s) { CHECK(px_personal_resource_field(RECORD,s,&f->memory)==field_reference(f->bytes+PAYLOAD,s));++cases; }
        w16(f,PAYLOAD+28,(uint16_t)v);CHECK(px_personal_resource_field(RECORD,30,&f->memory)==v);++cases;
    }
    CHECK(px_personal_resource_field(UINT32_MAX,46,NULL)==0);
    CHECK(px_personal_resource_field(UINT32_MAX,UINT32_MAX,NULL)==0);
    w32(f,CURRENT,RECORD2);w8(f,PAYLOAD2,73);CHECK(px_current_personal_value(0,&f->memory)==73);
    printf("fields: %u byte/halfword/bitfield cases, invalid-selector non-access and current descriptor reload\n",cases);
}

static void selection(struct fixture *f)
{
    const uint32_t species[]={0,1,25,721,722,723,724,65537,UINT32_MAX};
    const uint32_t forms[]={0,1,2,254,255,256,65537,UINT32_MAX};
    const unsigned firsts[]={0,722,1024},counts[]={0,1,2,255};
    for(unsigned row=0;row<2048;++row)
        for(unsigned b=0;b<64;++b)w8(f,TABLE+row*64+b,(uint8_t)(row*19+row/256+b*7));
    unsigned cases=0;
    for(unsigned s=0;s<9;++s)for(unsigned v=0;v<8;++v)for(unsigned a=0;a<3;++a)for(unsigned c=0;c<4;++c) {
        reset(f);const uint32_t sp=species[s],form=forms[v];
        if(sp<=721) { w32(f,TABLE+sp*64+28,0xbeef0000u|firsts[a]);w32(f,TABLE+sp*64+32,0xa5a50000u|counts[c]); }
        uint32_t index=sp<=721?sp:0;
        if(sp<=721&&form&&firsts[a]&&form<counts[c])index=firsts[a]+form-1;
        uint8_t expected[64],descriptor[12];memcpy(expected,f->bytes+TABLE+index*64,64);memcpy(descriptor,f->bytes+RECORD,12);
        f->table_reads=f->source_reads=f->group_writes=0;
        px_load_personal_record(RECORD,sp,form,&f->memory);
        CHECK(!memcmp(expected,f->bytes+PAYLOAD,64));
        CHECK(r16(f,RECORD+4)==(uint16_t)sp&&r8(f,RECORD+6)==(uint8_t)form);
        CHECK(!memcmp(descriptor,f->bytes+RECORD,4)&&r8(f,RECORD+7)==descriptor[7]&&r32(f,RECORD+8)==PAYLOAD);
        CHECK(f->table_reads==1&&f->group_writes==16&&f->source_reads==(sp<=721&&form?18u:16u));
        memset(f->bytes+PAYLOAD,0x55,64);px_load_personal_record(RECORD,sp,form,&f->memory);
        CHECK(!memcmp(expected,f->bytes+PAYLOAD,64)&&f->group_writes==32);++cases;
    }
    reset(f);w32(f,CURRENT,RECORD2);px_select_personal_resource(25,0,&f->memory);
    CHECK(r16(f,RECORD2+4)==25&&!memcmp(f->bytes+PAYLOAD2,f->bytes+TABLE+25*64,64));
    w32(f,CURRENT,UINT32_MAX);CHECK(px_current_personal_value(46,&f->memory)==0);
    reset(f);w32(f,TABLE+25*64+28,0xabcdffffu);w32(f,TABLE+25*64+32,255);
    const uint32_t wide_row=65535+254-1;
    for(unsigned i=0;i<64;++i)w8(f,TABLE+wide_row*64+i,(uint8_t)(i*13+19));
    px_load_personal_record(RECORD,25,254,&f->memory);
    CHECK(!memcmp(f->bytes+PAYLOAD,f->bytes+TABLE+wide_row*64,64));
    /* The original copies 32-byte groups. Forward overlap changes the second
     * group after the first store; this deliberately differs from memmove. */
    const int offsets[]={-32,-4,0,4,32};
    for(unsigned i=0;i<5;++i) {
        reset(f);const uint32_t source=TABLE+4096,destination=(uint32_t)((int32_t)source+offsets[i]);
        for(unsigned j=0;j<128;++j)w8(f,source-32+j,(uint8_t)(j*13));
        uint8_t expected[128],group[32];memcpy(expected,f->bytes+source-32,128);
        for(unsigned j=0;j<2;++j) { memcpy(group,expected+32+32*j,32);memcpy(expected+32+offsets[i]+32*j,group,32); }
        w32(f,RECORD+8,destination);px_load_personal_record(RECORD,64,0,&f->memory);
        CHECK(!memcmp(expected,f->bytes+source-32,128));
    }
    printf("selection: %u species/form boundary cases, wide form-row arithmetic, unconditional reload, metadata/pointer preservation and five overlap layouts\n",cases);
}

static void select_cb(void *u,uint32_t record,uint32_t species,uint32_t form)
{ struct fixture *f=u;px_load_personal_record(record,species,form,&f->memory); }
static uint32_t field_cb(void *u,uint32_t record,uint32_t selector)
{ struct fixture *f=u;return px_personal_resource_field(record,selector,&f->memory); }
static void member_cb(void *u,uint32_t archive,uint32_t index,uint32_t destination)
{
    struct fixture *f=u;CHECK(archive==0xabc&&destination==ENTRIES);f->member=index;
    w16(f,destination,33);w16(f,destination+2,5);w16(f,destination+4,0xffff);w16(f,destination+6,0xffff);
}
static void integration(struct fixture *f)
{
    reset(f);w32(f,TABLE+25*64+28,722);w32(f,TABLE+25*64+32,3);
    w32(f,LEARNSET_PERSONAL,RECORD2);w32(f,LEARNSET_ARCHIVE,0xabc);
    w16(f,LEARNSET,0xffff);w32(f,LEARNSET+4,ENTRIES);
    const struct px_learnset_api api={&f->memory,f,select_cb,field_cb,member_cb};
    px_load_learnset_record(LEARNSET,25,2,&api);CHECK(f->member==723);
    CHECK(px_learnset_record_count(LEARNSET,&f->memory)==1);
    CHECK(px_learnset_record_move(LEARNSET,0,&f->memory)==33&&px_learnset_record_level(LEARNSET,0,&f->memory)==5);
    px_load_learnset_record(LEARNSET,25,3,&api);CHECK(f->member==25);
    puts("integration: reconstructed learnset form resolution calls the personal selector and field dispatcher");
}
int main(void)
{
    struct fixture f={0};f.bytes=calloc(SIZE,1);CHECK(f.bytes);
    fields(&f);selection(&f);integration(&f);free(f.bytes);
    puts("pokemon_personal: all tests passed");return 0;
}
