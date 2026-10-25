#include "pokemon_move_resource.h"
#include "../pokemon_moves/pokemon_moves.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c); exit(1); } } while (0)
enum { SIZE = 0x600000, RECORD = 0x100, OTHER = 0x120, CACHE = 0x200, CACHE2 = 0x220,
       TABLE = 0x400, ROWS = 0x1000, BUFFER = 0x4000, OTHER_BUFFER = 0x4100, PAYLOADS = 0x5000,
       SELECTED_CELL = 0x5d188c, CACHE_CELL = 0x5d1890, ARCHIVE_CELL = 0x5e5338 };
struct fixture { uint8_t *bytes; unsigned reads, mutation; struct px_pokemon_api memory; struct px_move_resource_api api; };
static uint8_t r8(void *u, uint32_t a) { struct fixture *f = u; CHECK(a < SIZE); return f->bytes[a]; }
static uint16_t r16(void *u, uint32_t a) { return (uint16_t)((uint32_t)r8(u,a) | (uint32_t)r8(u,a+1)<<8); }
static uint32_t r32(void *u, uint32_t a) { return (uint32_t)r16(u,a) | (uint32_t)r16(u,a+2)<<16; }
static void w8(void *u, uint32_t a, uint8_t v) { struct fixture *f = u; CHECK(a < SIZE); f->bytes[a] = v; }
static void w16(void *u, uint32_t a, uint16_t v) { w8(u,a,(uint8_t)v); w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u, uint32_t a, uint32_t v) { w16(u,a,(uint16_t)v); w16(u,a+2,(uint16_t)(v>>16)); }
static void member(void *u, uint32_t archive, uint32_t index, uint32_t destination)
{
    struct fixture *f = u; CHECK(archive == 0xabc && index < 618); ++f->reads;
    for (unsigned i=0;i<36;++i) w8(f,destination+i,(uint8_t)(index*7+i*13));
    w8(f,destination+5,(uint8_t)(5+index%60));
    if (f->mutation) {
        w32(f,SELECTED_CELL,OTHER); w32(f,CACHE_CELL,CACHE2);
        w32(f,RECORD+8,OTHER_BUFFER); w8(f,OTHER_BUFFER+5,77);
    }
}
static void reset(struct fixture *f, unsigned capacity)
{
    memset(f->bytes,0xa5,0x10000); f->reads = f->mutation = 0;
    f->memory = (struct px_pokemon_api){.user=f,.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
    f->api = (struct px_move_resource_api){&f->memory,f,member};
    w16(f,RECORD+4,0xffff); w32(f,RECORD+8,BUFFER);
    w16(f,OTHER+4,0xffff); w32(f,OTHER+8,OTHER_BUFFER);
    w32(f,SELECTED_CELL,RECORD); w32(f,CACHE_CELL,CACHE); w32(f,ARCHIVE_CELL,0xabc);
    w32(f,CACHE+4,TABLE); w8(f,CACHE+8,(uint8_t)capacity); w8(f,CACHE+9,0);
    w32(f,CACHE2+4,TABLE+4*capacity); w8(f,CACHE2+8,1); w8(f,CACHE2+9,0);
    for (unsigned i=0;i<=capacity;++i) {
        w32(f,TABLE+4*i,ROWS+16*i); w16(f,ROWS+16*i+4,0xffff);
        w32(f,ROWS+16*i+8,PAYLOADS+36*i);
    }
}
/* Table transcribed from the reviewed ARM jump table, independent of the C switch. */
static uint32_t field_reference(const uint8_t *p, unsigned s)
{
    static const uint8_t offsets[33] = {0,1,2,3,4,5,6,14,7,7,15,8,10,11,12,13,
        21,22,23,24,25,26,27,28,29,18,19,20,16,0,18,19,4};
    if (s>=33 || s==29) return 0;
    uint32_t v=p[offsets[s]];
    if (s==11 || s==28) return v | (uint32_t)p[offsets[s]+1]<<8;
    if (s==6 || (s>=19 && s<=21)) return (uint32_t)(v<128 ? (int32_t)v : (int32_t)v-256);
    if (s==8) return v/16;
    if (s==9) return v%16;
    if (s==4 && v==101) return 100;
    if (s==25 || s==26) return v<128 ? v : 0;
    if (s==30 || s==31) return v<128 ? 0 : 256-v;
    if (s==32) return v==101;
    return v;
}
static void fields(struct fixture *f)
{
    reset(f,3);
    for (unsigned v=0;v<256;++v) {
        for (unsigned i=0;i<36;++i) f->bytes[BUFFER+i]=(uint8_t)(v*(2*i+1)+i*17);
        for (unsigned s=0;s<33;++s)
            CHECK(px_move_resource_field(RECORD,s,&f->memory)==field_reference(f->bytes+BUFFER,s));
    }
    for (unsigned v=0;v<=65535;++v) {
        w16(f,BUFFER+8,(uint16_t)v); w16(f,BUFFER+16,(uint16_t)(65535-v));
        CHECK(px_move_resource_field(RECORD,11,&f->memory)==v);
        CHECK(px_move_resource_field(RECORD,28,&f->memory)==65535-v);
    }
    CHECK(px_move_resource_field(UINT32_MAX,29,NULL)==0);
    CHECK(px_move_resource_field(UINT32_MAX,33,NULL)==0);
    CHECK(px_move_resource_field(UINT32_MAX,UINT32_MAX,NULL)==0);
    puts("fields: 8448 byte-selector cases, 131072 halfword cases, 3 unmapped fallbacks");
}

static void caches(struct fixture *f)
{
    static const unsigned capacities[]={1,2,3,4,31,127,255};
    unsigned cases=0;
    for (unsigned n=0;n<sizeof capacities/sizeof *capacities;++n) {
        const unsigned capacity=capacities[n];
        for (unsigned cursor=0;cursor<capacity;++cursor) {
            reset(f,capacity); w8(f,CACHE+9,(uint8_t)cursor); w16(f,RECORD+4,2000);
            for (unsigned i=0;i<36;++i) w8(f,BUFFER+i,(uint8_t)(i*13+cursor));
            const uint32_t row=ROWS+16*cursor, payload=PAYLOADS+36*cursor;
            uint8_t descriptor[16]; memcpy(descriptor,f->bytes+row,16);
            px_store_cached_move_resource(CACHE,RECORD,&f->memory);
            CHECK(r16(f,row+4)==2000 && r8(f,CACHE+9)==(cursor+1)%capacity);
            CHECK(!memcmp(f->bytes+payload,f->bytes+BUFFER,36));
            for (unsigned i=0;i<16;++i) if (i!=4 && i!=5) CHECK(f->bytes[row+i]==descriptor[i]);
            CHECK(r8(f,CACHE+10)==0xa5 && r8(f,CACHE+11)==0xa5);
            CHECK(px_find_cached_move_resource(CACHE,2000,OTHER,&f->memory)==1);
            CHECK(r16(f,OTHER+4)==2000 && !memcmp(f->bytes+OTHER_BUFFER,f->bytes+BUFFER,36));
            CHECK(px_find_cached_move_resource(CACHE,2000+65536,OTHER,&f->memory)==0);
            const uint8_t old=f->bytes[payload]; w8(f,BUFFER,old^255);
            px_store_cached_move_resource(CACHE,RECORD,&f->memory);
            CHECK(r8(f,CACHE+9)==(cursor+1)%capacity && f->bytes[payload]==old);
            memcpy(descriptor,f->bytes+RECORD,16);
            px_copy_move_resource(RECORD,RECORD,&f->memory);
            CHECK(!memcmp(descriptor,f->bytes+RECORD,16));
            ++cases;
        }
    }
    reset(f,0); w32(f,CACHE+4,UINT32_MAX); w8(f,CACHE+9,255);
    CHECK(px_find_cached_move_resource(CACHE,1,OTHER,&f->memory)==0);
    printf("cache: %u capacity/cursor, deep-copy, hit, duplicate and full-width miss cases; empty lookup\n",cases);
}

static uint32_t move_value(void *user, uint32_t move, uint32_t selector)
{
    struct fixture *f=user; return px_get_move_resource_value(move,selector,&f->api);
}
static void loaders(struct fixture *f)
{
    static const uint32_t ids[]={0,1,617,618,619,65535,65536,UINT32_MAX};
    for (unsigned i=0;i<sizeof ids/sizeof *ids;++i) {
        reset(f,3); const uint32_t id=ids[i]<618 ? ids[i] : 0;
        px_load_move_resource(RECORD,ids[i],&f->api);
        CHECK(f->reads==1 && r16(f,RECORD+4)==id && r8(f,BUFFER+5)==5+id%60);
        px_load_move_resource(RECORD,ids[i],&f->api); CHECK(f->reads==1);
    }
    reset(f,3);
    CHECK(px_get_move_resource_value(25,5,&f->api)==30 && f->reads==1);
    w16(f,RECORD+4,0xffff); memset(f->bytes+BUFFER,0,36);
    CHECK(px_get_move_resource_value(25,5,&f->api)==30 && f->reads==1);
    reset(f,3);
    CHECK(px_get_move_resource_value(UINT32_MAX,UINT32_MAX,&f->api)==0 && f->reads==1);
    CHECK(r16(f,RECORD+4)==0 && r16(f,ROWS+4)==0 && r8(f,CACHE+9)==1);
    CHECK(px_get_move_resource_value(UINT32_MAX,5,&f->api)==5 && f->reads==1);
    reset(f,3);
    /* Cache identity has no separate validity flag or range normalization. */
    CHECK(px_get_move_resource_value(65535,5,&f->api)==0xa5 && f->reads==0);
    CHECK(r16(f,RECORD+4)==65535);
    reset(f,3); w16(f,ROWS+4,618);
    CHECK(px_get_move_resource_value(618,5,&f->api)==0xa5 && f->reads==0);
    reset(f,3); f->mutation=1;
    CHECK(px_get_move_resource_value(12,5,&f->api)==77 && f->reads==1);
    CHECK(r16(f,RECORD+4)==12 && r16(f,ROWS+16*3+4)==12);
    CHECK(r8(f,CACHE+9)==0 && r8(f,PAYLOADS+36*3+5)==77);
    reset(f,3); const struct px_move_api api={NULL,f,move_value};
    for (unsigned move=0;move<618;++move)
        for (unsigned ups=0;ups<5;++ups) {
            const unsigned base=5+move%60, u=ups>3 ? 3 : ups;
            CHECK(px_pokemon_maximum_pp(move,ups,&api)==base+base*u/5);
        }
    CHECK(f->reads==618);
    puts("loader: ID normalization, reuse, cache restoration, callback mutation; 3090 maximum-PP integrations");
}

int main(void)
{
    struct fixture f={0}; f.bytes=calloc(SIZE,1); CHECK(f.bytes);
    fields(&f); caches(&f); loaders(&f); free(f.bytes);
    puts("pokemon_move_resource: all tests passed"); return 0;
}
