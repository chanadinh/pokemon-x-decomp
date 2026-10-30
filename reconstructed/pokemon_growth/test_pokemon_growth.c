#include "pokemon_growth.h"
#include "../pokemon_personal/pokemon_personal.h"
#include "../pokemon_data/pokemon_data.h"
#include "../pokemon_stats/stats_math.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x5f0000, DESCRIPTOR=0x100, PERSONAL=0x200, RECORD=0x1000, RECORD2=0x1400,
       SLOTS=0x2000, SLOT_STRIDE=0x200, CACHE=0x4000, CACHE_TABLE=0x4100, OVERLAP=0x8000,
       TABLE=0x10000, ARCHIVE=0x5e52b0, PERSONAL_TABLE=0x5d177c, PERSONAL_CELL=0x5d17a8,
       RECORD_CELL=0x5d17ac, CACHE_CELL=0x5d17b0, HANDLE=0x1234 };
struct fixture {
    uint8_t *bytes;struct px_pokemon_api memory;struct px_growth_api api;
    unsigned reads,personal_cell_reads,cache_cell_reads,record_cell_reads;
    uint32_t last_archive,last_index,last_destination,hook_address,hook_target,hook_value;
    int hook_armed,mutate_rate,trace_on;unsigned trace_count;uint32_t trace[2048];
};
static uint32_t rates[8][101];
static uint8_t r8(void *u,uint32_t a) { struct fixture *f=u;CHECK(a<SIZE);return f->bytes[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)((uint32_t)r8(u,a)|(uint32_t)r8(u,a+1)<<8); }
static void w8(void *u,uint32_t a,uint8_t v) { struct fixture *f=u;CHECK(a<SIZE);f->bytes[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void raw32(struct fixture *f,uint32_t a,uint32_t v) { w16(f,a,(uint16_t)v);w16(f,a+2,(uint16_t)(v>>16)); }
static uint32_t get32(struct fixture *f,uint32_t a) { return (uint32_t)r16(f,a)|(uint32_t)r16(f,a+2)<<16; }
static uint32_t r32(void *u,uint32_t a)
{
    struct fixture *f=u;CHECK(a%4==0);
    if(a==PERSONAL_CELL)++f->personal_cell_reads;
    if(a==CACHE_CELL)++f->cache_cell_reads;
    if(a==RECORD_CELL)++f->record_cell_reads;
    if(f->trace_on&&f->trace_count<2048)f->trace[f->trace_count++]=a;
    const uint32_t v=get32(f,a);
    if(f->hook_armed&&a==f->hook_address) { f->hook_armed=0;raw32(f,f->hook_target,f->hook_value); }
    return v;
}
static void w32(void *u,uint32_t a,uint32_t v)
{ struct fixture *f=u;CHECK(a%4==0);if(f->trace_on&&f->trace_count<2048)f->trace[f->trace_count++]=a|0x80000000u;raw32(f,a,v); }
static void read_member(void *u,uint32_t archive,uint32_t index,uint32_t destination)
{
    struct fixture *f=u;++f->reads;f->last_archive=archive;f->last_index=index;f->last_destination=destination;
    CHECK(index<8);for(unsigned i=0;i<101;++i)raw32(f,destination+4*i,rates[index][i]);
    if(f->mutate_rate)raw32(f,destination-4,0x77777777u);
}
static void reset(struct fixture *f)
{
    memset(f->bytes,0xa5,0x20000);
    uint8_t *bytes=f->bytes;memset(f,0,sizeof *f);f->bytes=bytes;
    f->memory=(struct px_pokemon_api){.user=f,.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
    f->api=(struct px_growth_api){&f->memory,f,read_member};
    raw32(f,ARCHIVE,HANDLE);raw32(f,PERSONAL_TABLE,TABLE);raw32(f,PERSONAL_CELL,DESCRIPTOR);raw32(f,DESCRIPTOR+8,PERSONAL);
    raw32(f,RECORD_CELL,RECORD);raw32(f,CACHE_CELL,CACHE);raw32(f,CACHE+4,CACHE_TABLE);
    for(unsigned i=0;i<8;++i) { raw32(f,CACHE_TABLE+4*i,SLOTS+SLOT_STRIDE*i);raw32(f,SLOTS+SLOT_STRIDE*i+4,0xffffffffu-i); }
    w8(f,CACHE+8,4);w8(f,CACHE+9,0);raw32(f,RECORD+4,0xeeeeeeeeu);
}
static void fill(struct fixture *f,uint32_t record,uint32_t rate,uint32_t seed)
{ raw32(f,record+4,rate);for(unsigned i=0;i<101;++i)raw32(f,record+8+4*i,seed*1000003u+i*7919u); }
static int same_payload(struct fixture *f,uint32_t a,uint32_t b) { return !memcmp(f->bytes+a+8,f->bytes+b+8,PX_GROWTH_PAYLOAD_SIZE); }
static void reference_copy(uint8_t *bytes,uint32_t from,uint32_t to)
{
    uint32_t w[8];static const unsigned groups[]={8,8,8,8,8,8,8,8,8,8,8,8,4,1};
    for(unsigned g=0;g<14;++g) {
        memcpy(w,bytes+from,4*groups[g]);memcpy(bytes+to,w,4*groups[g]);from+=4*groups[g];to+=4*groups[g];
    }
}
static void copies(struct fixture *f)
{
    reset(f);fill(f,RECORD,0x80000017u,3);raw32(f,RECORD2,0x11223344);raw32(f,RECORD2+4,0);
    f->trace_on=1;px_copy_growth_resource(RECORD,RECORD2,&f->memory);f->trace_on=0;
    CHECK(same_payload(f,RECORD,RECORD2)&&get32(f,RECORD2+4)==0x80000017u&&get32(f,RECORD2)==0x11223344);
    CHECK(f->bytes[RECORD2+8+PX_GROWTH_PAYLOAD_SIZE]==0xa5);
    /* 101 payload loads/stores in 8x12,4,1 groups, then the rate load/store. */
    CHECK(f->trace_count==204);unsigned t=0;const unsigned groups[]={8,8,8,8,8,8,8,8,8,8,8,8,4,1};
    uint32_t offset=8;
    for(unsigned g=0;g<14;++g) {
        for(unsigned i=0;i<groups[g];++i)CHECK(f->trace[t++]==RECORD+offset+4*i);
        for(unsigned i=0;i<groups[g];++i)CHECK(f->trace[t++]==((RECORD2+offset+4*i)|0x80000000u));
        offset+=4*groups[g];
    }
    CHECK(f->trace[t++]==RECORD+4&&f->trace[t++]==((RECORD2+4)|0x80000000u));
    unsigned layouts=0;const int deltas[]={-40,-32,-28,-16,-4,0,4,16,28,32,40,384,388};
    for(unsigned d=0;d<sizeof deltas/sizeof *deltas;++d) {
        reset(f);fill(f,OVERLAP,9,d+1);uint8_t expected[0x600];const uint32_t to=(uint32_t)(OVERLAP+deltas[d]);
        memcpy(expected,f->bytes+OVERLAP-0x100,sizeof expected);
        reference_copy(expected,0x100+8,(uint32_t)(0x100+deltas[d]+8));
        uint32_t rate;memcpy(&rate,expected+0x100+4,4);memcpy(expected+0x100+deltas[d]+4,&rate,4);
        px_copy_growth_resource(OVERLAP,to,&f->memory);
        CHECK(!memcmp(expected,f->bytes+OVERLAP-0x100,sizeof expected));++layouts;
    }
    printf("copy: grouped 404-byte transfer trace, full rate word, vtable/trailing bytes preserved; %u overlap layouts\n",layouts);
}
static void caches(struct fixture *f)
{
    unsigned cases=0;
    for(unsigned capacity=0;capacity<=8;++capacity)
        for(unsigned hit=0;hit<=8;++hit) {
            reset(f);w8(f,CACHE+8,(uint8_t)capacity);
            for(unsigned i=0;i<8;++i)fill(f,SLOTS+SLOT_STRIDE*i,100+i,i+11);
            const uint32_t rate=hit<8?100+hit:0x10000064u;fill(f,RECORD,0xdead,99);
            const uint32_t found=px_find_cached_growth_resource(CACHE,rate,RECORD,&f->memory);
            if(hit<capacity) { CHECK(found==1&&same_payload(f,RECORD,SLOTS+SLOT_STRIDE*hit)&&get32(f,RECORD+4)==rate); }
            else { CHECK(found==0&&get32(f,RECORD+4)==0xdead); }
            ++cases;
        }
    /* Full-width comparison: only the high bits differ. */
    reset(f);fill(f,SLOTS,0x00000002u,1);CHECK(!px_find_cached_growth_resource(CACHE,0x01000002u,RECORD,&f->memory));++cases;
    /* Capacity is reread after each miss. */
    reset(f);fill(f,SLOTS+SLOT_STRIDE*5,7,5);w8(f,CACHE+8,1);
    f->hook_armed=1;f->hook_address=SLOTS+4;f->hook_target=CACHE+8;f->hook_value=0xa5a5a506u;
    CHECK(px_find_cached_growth_resource(CACHE,7,RECORD,&f->memory)==1&&same_payload(f,RECORD,SLOTS+SLOT_STRIDE*5));++cases;
    /* The slot pointer is reloaded after the match. */
    reset(f);fill(f,SLOTS,7,1);fill(f,SLOTS+SLOT_STRIDE*6,8,6);
    f->hook_armed=1;f->hook_address=SLOTS+4;f->hook_target=CACHE_TABLE;f->hook_value=SLOTS+SLOT_STRIDE*6;
    CHECK(px_find_cached_growth_resource(CACHE,7,RECORD,&f->memory)==1&&same_payload(f,RECORD,SLOTS+SLOT_STRIDE*6)&&get32(f,RECORD+4)==8);++cases;
    unsigned stores=0;
    for(unsigned capacity=1;capacity<=8;++capacity)
        for(unsigned cursor=0;cursor<capacity;++cursor)
            for(unsigned duplicate=0;duplicate<=capacity;++duplicate) {
                reset(f);w8(f,CACHE+8,(uint8_t)capacity);w8(f,CACHE+9,(uint8_t)cursor);
                for(unsigned i=0;i<8;++i)fill(f,SLOTS+SLOT_STRIDE*i,200+i,i+31);
                const uint32_t rate=duplicate<capacity?200+duplicate:300;fill(f,RECORD,rate,77);
                uint8_t before[8*SLOT_STRIDE];memcpy(before,f->bytes+SLOTS,sizeof before);
                px_store_cached_growth_resource(CACHE,RECORD,&f->memory);
                if(duplicate<capacity) CHECK(!memcmp(before,f->bytes+SLOTS,sizeof before)&&f->bytes[CACHE+9]==cursor);
                else {
                    const uint32_t slot=SLOTS+SLOT_STRIDE*cursor;
                    CHECK(same_payload(f,RECORD,slot)&&get32(f,slot+4)==300&&get32(f,slot)==0xa5a5a5a5u);
                    for(unsigned i=0;i<8;++i)if(i!=cursor)CHECK(!memcmp(before+SLOT_STRIDE*i,f->bytes+SLOTS+SLOT_STRIDE*i,SLOT_STRIDE));
                    CHECK(f->bytes[CACHE+9]==(cursor+1)%capacity);
                }
                ++stores;
            }
    /* A duplicate rate leaves an older, different payload in place. */
    reset(f);w8(f,CACHE+8,1);w8(f,CACHE+9,0);fill(f,RECORD,5,5);fill(f,SLOTS,5,1);
    px_store_cached_growth_resource(CACHE,RECORD,&f->memory);CHECK(get32(f,SLOTS+8)!=get32(f,RECORD+8));++stores;
    /* The source rate is captured once before the duplicate scan. */
    reset(f);w8(f,CACHE+8,2);fill(f,SLOTS,40,1);fill(f,SLOTS+SLOT_STRIDE,41,2);fill(f,RECORD,42,3);
    f->hook_armed=1;f->hook_address=RECORD+4;f->hook_target=RECORD+4;f->hook_value=40;
    px_store_cached_growth_resource(CACHE,RECORD,&f->memory);
    CHECK(get32(f,SLOTS+4)==40&&f->bytes[CACHE+9]==1);++stores;
    /* Cursor and capacity are reread after the copy. */
    reset(f);w8(f,CACHE+8,2);w8(f,CACHE+9,0);fill(f,RECORD,50,9);
    f->hook_armed=1;f->hook_address=RECORD+8;f->hook_target=CACHE+8;f->hook_value=0xa5a50508u;
    px_store_cached_growth_resource(CACHE,RECORD,&f->memory);
    CHECK(get32(f,SLOTS+4)==50&&f->bytes[CACHE+9]==6);++stores;
    printf("cache: %u lookup cases incl. full-width rate, capacity and slot reloads; %u store/duplicate/cursor cases\n",cases,stores);
}
static void loads(struct fixture *f)
{
    unsigned cases=0;
    for(unsigned rate=0;rate<8;++rate) {
        reset(f);raw32(f,RECORD+4,rate);px_load_growth_resource(RECORD,rate,&f->api);CHECK(f->reads==0);
        raw32(f,RECORD+4,rate|0x100u);px_load_growth_resource(RECORD,rate,&f->api);
        CHECK(f->reads==1&&f->last_archive==HANDLE&&f->last_index==rate&&f->last_destination==RECORD+8);
        CHECK(get32(f,RECORD+4)==rate&&get32(f,RECORD+8+4*100)==rates[rate][100]);cases+=2;
    }
    /* The rate is written after the read callback, which may change logical memory. */
    reset(f);f->hook_armed=1;f->hook_address=ARCHIVE;f->hook_target=RECORD+4;f->hook_value=0x55;
    px_load_growth_resource(RECORD,3,&f->api);CHECK(f->reads==1&&get32(f,RECORD+4)==3);++cases;
    reset(f);f->mutate_rate=1;px_load_growth_resource(RECORD,4,&f->api);CHECK(f->reads==1&&get32(f,RECORD+4)==4);++cases;
    printf("loader: %u skip/read cases; full-width stored rate; archive cell and destination +8\n",cases);
}
static void set_personal(struct fixture *f,uint32_t index,uint8_t rate,uint16_t first,uint8_t count)
{
    for(unsigned i=0;i<64;++i)f->bytes[TABLE+64*index+i]=(uint8_t)(index+i);
    f->bytes[TABLE+64*index+0x15]=rate;w16(f,TABLE+64*index+0x1c,first);f->bytes[TABLE+64*index+0x20]=count;
}
static void selection(struct fixture *f)
{
    reset(f);unsigned cases=0;
    for(unsigned i=0;i<=721+8;++i)set_personal(f,i,(uint8_t)(i%6),0,1);
    set_personal(f,6,1,722,3);set_personal(f,722,5,0,1);set_personal(f,723,4,0,1);
    w8(f,CACHE+8,2);w8(f,CACHE+9,0);raw32(f,SLOTS+4,0xffffffffu);raw32(f,SLOTS+SLOT_STRIDE+4,0xfffffffeu);
    px_select_growth_resource(6,0,&f->api);
    CHECK(f->reads==1&&f->last_index==1&&get32(f,RECORD+4)==1&&r16(f,DESCRIPTOR+4)==6&&f->bytes[DESCRIPTOR+6]==0);
    CHECK(get32(f,SLOTS+4)==1&&same_payload(f,SLOTS,RECORD)&&f->bytes[CACHE+9]==1);++cases;
    px_select_growth_resource(6,1,&f->api);CHECK(f->reads==2&&f->last_index==5&&f->bytes[CACHE+9]==0);++cases;
    raw32(f,RECORD+4,0x99);px_select_growth_resource(1,0,&f->api);
    CHECK(f->reads==2&&get32(f,RECORD+4)==1&&same_payload(f,RECORD,SLOTS));++cases;
    px_select_growth_resource(6,2,&f->api);CHECK(f->reads==3&&f->last_index==4&&f->bytes[CACHE+9]==1&&get32(f,SLOTS+4)==4);++cases;
    px_select_growth_resource(800,0,&f->api);CHECK(f->last_index==0&&f->reads==4);++cases;
    /* The loader skips when the selected record already has the rate; it is still stored. */
    reset(f);for(unsigned i=0;i<16;++i)set_personal(f,i,(uint8_t)(i%6),0,1);
    w8(f,CACHE+8,1);raw32(f,SLOTS+4,0xffffffffu);fill(f,RECORD,3,8);
    px_select_growth_resource(3,0,&f->api);CHECK(f->reads==0&&get32(f,SLOTS+4)==3&&same_payload(f,SLOTS,RECORD));++cases;
    /* The record cell is read once, before personal selection; personal and cache cells are reread. */
    reset(f);for(unsigned i=0;i<16;++i)set_personal(f,i,2,0,1);w8(f,CACHE+8,1);raw32(f,SLOTS+4,0xffffffffu);
    f->hook_armed=1;f->hook_address=PERSONAL_TABLE;f->hook_target=RECORD_CELL;f->hook_value=RECORD2;
    px_select_growth_resource(4,0,&f->api);
    CHECK(f->last_destination==RECORD+8&&get32(f,RECORD+4)==2&&f->record_cell_reads==1);
    CHECK(f->personal_cell_reads==2&&f->cache_cell_reads==2);++cases;
    reset(f);for(unsigned i=0;i<16;++i)set_personal(f,i,2,0,1);w8(f,CACHE+8,1);fill(f,SLOTS,2,4);
    px_select_growth_resource(4,0,&f->api);CHECK(f->reads==0&&f->cache_cell_reads==1&&same_payload(f,RECORD,SLOTS));++cases;
    printf("selection: %u miss/hit/eviction/fallback/skip and cell-reload cases through reconstructed personal selection\n",cases);
}
static void select_cb(void *u,uint32_t species,uint32_t form) { struct fixture *f=u;px_select_growth_resource(species,form,&f->api); }
static void integration(struct fixture *f)
{
    reset(f);for(unsigned i=0;i<=721;++i)set_personal(f,i,(uint8_t)(i%6),0,1);w8(f,CACHE+8,6);
    const struct px_data_api data={&f->memory,f,select_cb};unsigned cases=0;
    for(unsigned species=1;species<=12;++species) {
        const unsigned rate=species%6;
        for(unsigned level=1;level<=100;++level) {
            const uint32_t growth=rates[rate][level];
            CHECK(px_pokemon_level_from_growth(species,0,growth,&data)==level);
            if(level<100&&rates[rate][level+1]>growth)CHECK(px_pokemon_level_from_growth(species,0,rates[rate][level+1]-1,&data)==level);
            CHECK(px_pokemon_growth_value(level,&data)==growth);cases+=2;
        }
    }
    CHECK(f->reads==6);
    printf("integration: %u reconstructed level/threshold cases; six archive reads through a six-slot cache\n",cases);
}
int main(void)
{
    struct fixture *f=calloc(1,sizeof *f);CHECK(f);f->bytes=malloc(SIZE);CHECK(f->bytes);memset(f->bytes,0,SIZE);
    for(unsigned r=0;r<8;++r)for(unsigned l=0;l<101;++l)rates[r][l]=l<2?0:l*l*(r+2)+r;
    copies(f);caches(f);loads(f);selection(f);integration(f);
    puts("pokemon_growth: all tests passed");free(f->bytes);free(f);return 0;
}
