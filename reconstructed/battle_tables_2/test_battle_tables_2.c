#include "battle_tables_2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x20000, BATTLE=0x1000, POOL=0x1100, NODES=0x2000, MOVE=0x8000, SCRATCH=0xb000, POKEMON=0xc000, TABLES=0xe000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static unsigned aborts;
static void abort_battle(void *u) { (void)u;++aborts; }
/* The guest table function f (0x100 and up) is a move stub: count word f * 0x10001 + 3, entries TABLES + 0x40 (f - 0x100). */
static uint32_t entries(void *u,uint32_t f,uint32_t out)
{
    (void)u;CHECK(out==SCRATCH&&f>=0x100&&f<0x200);
    return px_battle_move_table_function(out,(f&0xffu)*0x10001u+3,TABLES+0x40*(f-0x100),&memory);
}
static const struct px_battle_handler_api api={&memory,NULL,abort_battle,NULL,NULL,entries,BATTLE,POOL,0,0,MOVE,0,SCRATCH};
static uint32_t node(unsigned i) { return NODES+0x40*i; }
int main(void)
{
    unsigned cases=0;
    const uint32_t words[]={0,1,2,3,0x10001,0x7fff0005,0xffffffffu,0x12345678};
    for(unsigned i=0;i<sizeof words/sizeof *words;++i)for(uint32_t e=0;e<0x50;e+=7) {
        memset(mem,0,sizeof mem);w32(NULL,SCRATCH,0xa5a5a5a5);
        CHECK(px_battle_move_table_function(SCRATCH,words[i],0x10d00000+e,&memory)==0x10d00000+e&&r32(NULL,SCRATCH)==words[i]);
        CHECK(r32(NULL,SCRATCH-4)==0&&r32(NULL,SCRATCH+4)==0);++cases;
    }
    /* Registering every move of a 296-row table through the stubs: 248 distinct functions, shared by some rows. */
    for(uint32_t row=0;row<296;++row) {
        memset(mem,0,sizeof mem);
        for(unsigned i=0;i<PX_BATTLE_HANDLER_POOL;++i)w32(NULL,POOL+4*i,node(i));
        for(uint32_t k=0;k<296;++k){w16(NULL,MOVE+8*k,(uint16_t)(0x20+k));w32(NULL,MOVE+8*k+4,0x100+(k<248?k:k-248));}
        w8(NULL,POKEMON+0x19,5);
        CHECK(px_battle_register_move(POKEMON,0x20+row,0x55,&api)==1&&aborts==0);
        const uint32_t f=0x100+(row<248?row:row-248),h=node(0);
        CHECK(r32(NULL,BATTLE+0x64)==1&&r32(NULL,h+8)==TABLES+0x40*(f-0x100)&&r8(NULL,h+0x1a)==(uint8_t)(((f&0xffu)*0x10001u+3)&0xffu));
        CHECK(r8(NULL,h+0x10)==0&&r16(NULL,h+0x38)==0x20+row&&r8(NULL,h+0x3a)==5&&r32(NULL,h+0x14)==(7u<<24|0x55));++cases;
    }
    printf("battle_tables_2: %u cases, all tests passed\n",cases);
    return 0;
}
