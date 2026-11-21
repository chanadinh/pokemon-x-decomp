#include "battle_tables_1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x20000, BATTLE=0x1000, POOL=0x1100, NODES=0x2000, LAYERS=0x5000, SIDE=0x6000, POSITION=0x6200,
       ITEM=0x8800, SCRATCH=0xb000, POKEMON=0xc000, TABLES=0xe000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static unsigned aborts,overflows,raw_calls;static uint32_t speed_value,last_parameter;
static void abort_battle(void *u) { (void)u;++aborts; }
static void overflow(void *u) { (void)u;++overflows; }
static uint32_t raw_value(void *u,uint32_t p,uint32_t k) { (void)u;CHECK(p==POKEMON&&k==0xc);++raw_calls;return speed_value; }
/* Table function f (0x100 and up) returns TABLES + 0x40 f - 0x4000 and stores f * 0x10001. */
static uint32_t entries(void *u,uint32_t f,uint32_t out)
{
    (void)u;CHECK(out==SCRATCH&&f>=0x100&&f<0x200);w32(NULL,out,(f&0xffu)*0x10001u+0x500u);return TABLES+0x40*(f-0x100);
}
static uint32_t position_entries(void *u,uint32_t f,uint32_t out,uint32_t parameter)
{
    last_parameter=parameter;return entries(u,f,out);
}
static const struct px_battle_handler_api handler_api={&memory,NULL,abort_battle,NULL,raw_value,entries,BATTLE,POOL,
                                                       0,ITEM,0,0,SCRATCH};
static const struct px_battle_tables_api api={&handler_api,LAYERS,SIDE,POSITION,overflow,position_entries};
static uint32_t node(unsigned i) { return NODES+0x40*i; }
static const uint8_t position_ids[11]={0,1,2,3,4,5,6,7,9,8,10};
static const uint32_t side_max[17]={1,1,1,1,1,1,3,2,1,1,1,1,1,1,1,3,1};
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=overflows=raw_calls=0;speed_value=0;last_parameter=0xdeadbeef;
    for(unsigned i=0;i<PX_BATTLE_HANDLER_POOL;++i)w32(NULL,POOL+4*i,node(i));
    for(unsigned i=0;i<17;++i){w32(NULL,SIDE+12*i,i);w32(NULL,SIDE+12*i+4,0x100+i);w32(NULL,SIDE+12*i+8,side_max[i]);}
    for(unsigned i=0;i<11;++i){w32(NULL,POSITION+8*i,position_ids[i]);w32(NULL,POSITION+8*i+4,0x120+i);}
    for(unsigned i=0;i<0xb6;++i){w32(NULL,ITEM+8*i,i?(uint32_t)(0x40+i):0);w32(NULL,ITEM+8*i+4,0x140+i);}
}
int main(void)
{
    unsigned cases=0;
    /* The regular table function. */
    const uint32_t words[]={0,1,2,3,0x10001,0x30003,0x7fff0005,0xffffffffu,0x12345678};
    for(unsigned i=0;i<sizeof words/sizeof *words;++i)for(uint32_t e=0;e<0x50;e+=7) {
        reset();w32(NULL,SCRATCH,0xa5a5a5a5);
        CHECK(px_battle_table_stub(SCRATCH,words[i],0x10d00000+e,&memory)==0x10d00000+e&&r32(NULL,SCRATCH)==words[i]);
        CHECK(r32(NULL,SCRATCH-4)==0&&r32(NULL,SCRATCH+4)==0);++cases;
    }
    /* The terrain table function: parameters 0-5 and large values. */
    const struct px_battle_terrain_tables terrain={0x1111,0x2222,0x3333,0x4444};
    for(uint32_t p=0;p<0x20;++p) {
        reset();const uint32_t got=px_battle_terrain_table(SCRATCH,p,&terrain,&memory);
        const uint32_t want=p==1?0x1111:p==2?0x2222:p==3?0x3333:0x4444,count=p==1?2:p==2?3:p==3?3:1;
        CHECK(got==want&&r32(NULL,SCRATCH)==count);++cases;
    }
    reset();CHECK(px_battle_terrain_table(SCRATCH,0xffffffffu,&terrain,&memory)==0x4444&&r32(NULL,SCRATCH)==1);++cases;
    reset();CHECK(px_battle_terrain_table(SCRATCH,1u,&terrain,&memory)==0x1111);++cases;
    /* Unmark: every slot and owner pair over a list of five handlers. */
    for(uint32_t slot=0;slot<8;++slot)for(uint32_t owner=0;owner<8;++owner)for(unsigned mask=0;mask<32;++mask) {
        reset();unsigned expect=0;
        for(unsigned i=0;i<5;++i) {
            const uint32_t n=node(i);w32(NULL,n+4,i+1<5?node(i+1):0);
            w8(NULL,n+0x3b,(uint8_t)(i&3));w8(NULL,n+0x10,(uint8_t)(mask>>i&1?owner:owner^1));w32(NULL,n+0x18,0x5fffffff);
            if((i&3)==slot&&mask>>i&1)expect=1;
        }
        w32(NULL,BATTLE+0x60,node(0));
        CHECK(px_battle_handlers_unmark(slot,owner,&handler_api)==expect);
        for(unsigned i=0;i<5;++i) {
            const int hit=(i&3)==slot&&(mask>>i&1);
            CHECK(r32(NULL,node(i)+0x18)==(hit?0x1fffffffu:0x5fffffffu));
        }
        ++cases;
    }
    reset();CHECK(px_battle_handlers_unmark(3,5,&handler_api)==0);++cases;
    reset();w32(NULL,node(0)+0x18,0x1234);px_battle_handler_mark_26(node(0),&handler_api);CHECK(r32(NULL,node(0)+0x18)==0x4001234);
    px_battle_handler_mark_26(node(0),&handler_api);CHECK(r32(NULL,node(0)+0x18)==0x4001234);++cases;
    /* Side effects: every side and id, then layers up to each row's maximum. */
    for(uint32_t side=0;side<3;++side)for(uint32_t id=0;id<0x13;++id) {
        reset();for(unsigned o=0;o<0x3c;++o)w8(NULL,node(0)+o,0xa5);
        const uint32_t rec=LAYERS+(side*17+id)*16;
        const uint32_t before=r32(NULL,rec);
        for(unsigned i=0;i<4;++i)w32(NULL,rec+4*i,0);
        const uint32_t h=px_battle_register_side(side,id,0x77+id,&api);
        CHECK(aborts==(unsigned)(side>=2)+(unsigned)(id>=0x11));
        if(id>=0x11){CHECK(h==0&&overflows==0);++cases;continue;}
        CHECK(h==node(0)&&r8(NULL,h+0x10)==2&&r16(NULL,h+0x38)==id&&r8(NULL,h+0x3a)==side&&r32(NULL,h+8)==TABLES+0x40*id);
        CHECK(r32(NULL,h+0x14)==(5u<<24)&&r8(NULL,h+0x1a)==(uint8_t)((id*0x10001u+0x500u)&0xffu));
        CHECK(r32(NULL,h+0x1c+24)==0x77+id);
        CHECK(r32(NULL,rec)==h&&r32(NULL,rec+4)==0x77+id&&r32(NULL,rec+8)==0&&r32(NULL,rec+0xc)==1&&before==0);
        /* More layers. */
        for(uint32_t layer=1;layer<5;++layer) {
            const uint32_t again=px_battle_register_side(side,id,0x99,&api);
            if(layer<side_max[id]){CHECK(again==h&&r32(NULL,rec+0xc)==layer+1);}
            else{CHECK(again==0&&r32(NULL,rec+0xc)==(side_max[id]<5?side_max[id]:layer)&&r32(NULL,rec+4)==0x77+id);}
        }
        CHECK(r32(NULL,POOL+0)==node(0));CHECK(r32(NULL,BATTLE+0x64)==1&&overflows==0);++cases;
    }
    /* A stored layer count is honoured, whatever else is in the record. */
    reset();w32(NULL,LAYERS+(17+6)*16,0x1234);w32(NULL,LAYERS+(17+6)*16+12,2);
    CHECK(px_battle_register_side(1,6,5,&api)==0x1234&&r32(NULL,LAYERS+(17+6)*16+12)==3&&r32(NULL,BATTLE+0x64)==0);++cases;
    reset();w32(NULL,LAYERS+(17+6)*16+12,3);CHECK(px_battle_register_side(1,6,5,&api)==0&&r32(NULL,LAYERS+(17+6)*16+12)==3);++cases;
    /* The pool is exhausted: the registration fails, the overflow routine runs and the work write follows. */
    reset();w32(NULL,BATTLE+0x64,0x9d);
    CHECK(px_battle_register_side(0,4,0x55,&api)==0&&aborts==1&&overflows==1&&r32(NULL,0x34)==0x55);
    CHECK(r32(NULL,LAYERS+4*16+12)==1&&r32(NULL,LAYERS+4*16)==0&&r32(NULL,LAYERS+4*16+4)==0x55);++cases;
    /* A row with a duplicate id whose maximum is reached is skipped, and the scan goes on to the next row. */
    reset();w32(NULL,SIDE+12*3,6);w32(NULL,SIDE+12*3+8,1);w32(NULL,LAYERS+6*16+12,2);w32(NULL,LAYERS+6*16,0x4321);
    CHECK(px_battle_register_side(0,6,1,&api)==0x4321&&r32(NULL,LAYERS+6*16+12)==3);
    CHECK(px_battle_register_side(0,6,1,&api)==0&&r32(NULL,LAYERS+6*16+12)==3);++cases;
    reset();w32(NULL,SIDE+12*3,6);w32(NULL,SIDE+12*3+8,5);w32(NULL,LAYERS+6*16+12,4);w32(NULL,LAYERS+6*16,0x4321);
    CHECK(px_battle_register_side(0,6,1,&api)==0x4321&&r32(NULL,LAYERS+6*16+12)==5);++cases;
    /* Position effects: each id against the table's order, and the parameter reaching the table function. */
    for(uint32_t id=0;id<14;++id) {
        reset();const uint32_t h=px_battle_register_position(id,0x31+id,&api);
        if(id>10){CHECK(h==0&&r32(NULL,BATTLE+0x64)==0&&last_parameter==0xdeadbeef);++cases;continue;}
        unsigned row=0;while(position_ids[row]!=id)++row;
        CHECK(h==node(0)&&last_parameter==0x31+id&&r8(NULL,h+0x10)==3&&r16(NULL,h+0x38)==id&&r8(NULL,h+0x3a)==0x31+id);
        CHECK(r32(NULL,h+8)==TABLES+0x40*(0x20+row)&&r32(NULL,h+0x14)==(4u<<24)&&r8(NULL,h+0x3b)==0x1f);
        CHECK(r8(NULL,h+0x1a)==(uint8_t)(((0x20+row)&0xffu)*0x10001u+0x500u)&&aborts==0&&overflows==0);++cases;
    }
    reset();w32(NULL,POSITION+8*4,10);CHECK(px_battle_register_position(10,1,&api)==node(0)&&r32(NULL,node(0)+8)==TABLES+0x40*(0x20+4));++cases;
    reset();w32(NULL,BATTLE+0x64,0x9d);CHECK(px_battle_register_position(3,1,&api)==0&&aborts==1);++cases;
    /* Checked item registration: a battler with handlers of owner 5 is skipped; otherwise the held item. */
    for(unsigned flagged=0;flagged<2;++flagged)for(uint32_t item=0;item<0x100;item+=1) {
        reset();speed_value=0x12345;
        w16(NULL,POKEMON+0x12,(uint16_t)item);w8(NULL,POKEMON+0x19,9);
        if(flagged){w32(NULL,BATTLE+0x60,node(150));w32(NULL,node(150)+4,0);w8(NULL,node(150)+0x3b,9);w8(NULL,node(150)+0x10,5);w32(NULL,node(150)+0x18,0x40000000);}
        px_battle_register_item_checked(POKEMON,&api);
        const int listed=item>=0x41&&item<0x41+0xb5;
        if(flagged){CHECK(r32(NULL,node(150)+0x18)==0&&r32(NULL,BATTLE+0x64)==0&&raw_calls==0);++cases;continue;}
        if(!listed){CHECK(r32(NULL,BATTLE+0x64)==0&&raw_calls==0);++cases;continue;}
        const uint32_t row=item-0x40;
        CHECK(r32(NULL,BATTLE+0x64)==1&&raw_calls==1&&r8(NULL,node(0)+0x10)==5&&r16(NULL,node(0)+0x38)==item);
        CHECK(r8(NULL,node(0)+0x3a)==9&&r32(NULL,node(0)+0x14)==((1u<<24)|0x2345)&&r32(NULL,node(0)+8)==TABLES+0x40*(0x40+row));++cases;
    }
    /* A flag only on another owner or another slot does not stop it. */
    reset();w16(NULL,POKEMON+0x12,0x45);w8(NULL,POKEMON+0x19,9);w32(NULL,BATTLE+0x60,node(150));w8(NULL,node(150)+0x3b,9);w8(NULL,node(150)+0x10,4);
    px_battle_register_item_checked(POKEMON,&api);CHECK(r32(NULL,BATTLE+0x64)==1);++cases;
    reset();w16(NULL,POKEMON+0x12,0x45);w8(NULL,POKEMON+0x19,9);w32(NULL,BATTLE+0x60,node(150));w8(NULL,node(150)+0x3b,8);w8(NULL,node(150)+0x10,5);
    px_battle_register_item_checked(POKEMON,&api);CHECK(r32(NULL,BATTLE+0x64)==1);++cases;
    /* Given item registration: the item is a parameter; the handler is flagged and work word 6 is 1. */
    for(uint32_t item=0;item<0x100;++item) {
        reset();speed_value=0xabcd7;w16(NULL,POKEMON+0x12,0x77);w8(NULL,POKEMON+0x19,4);
        const uint32_t h=px_battle_register_item_given(POKEMON,item,&api);
        const int listed=item>=0x41&&item<0x41+0xb5;
        if(!listed){CHECK(h==0&&r32(NULL,BATTLE+0x64)==0&&raw_calls==0);++cases;continue;}
        CHECK(h==node(0)&&r16(NULL,h+0x38)==item&&r8(NULL,h+0x3a)==4&&r32(NULL,h+0x14)==((1u<<24)|0xbcd7));
        CHECK(r32(NULL,h+0x1c+24)==1&&(r32(NULL,h+0x18)&0x4000000)&&r32(NULL,h+8)==TABLES+0x40*(0x40+item-0x40));++cases;
    }
    reset();w32(NULL,BATTLE+0x64,0x9d);CHECK(px_battle_register_item_given(POKEMON,0x45,&api)==0&&aborts==1&&r32(NULL,0x34)==0);++cases;
    printf("battle_tables_1: %u cases, all tests passed\n",cases);
    return 0;
}
