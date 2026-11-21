#include "battle_core_3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x20000, BATTLE=0x1000, POOL=0x1100, NODES=0x2000, LAYERS=0x5000, CTX=0x8000, BUF=0xb000, MON=0xc000, PARTY=0xd000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static unsigned aborts,value_calls;static uint32_t hp_of[4],ability_of;
static void abort_battle(void *u) { (void)u;++aborts; }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t entries(void *u,uint32_t f,uint32_t out) { (void)u;(void)f;(void)out;return 0; }
static uint32_t position_entries(void *u,uint32_t f,uint32_t out,uint32_t p) { (void)u;(void)f;(void)out;(void)p;return 0; }
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k)
{
    (void)u;++value_calls;
    if(k==0xd){CHECK(p==0||(p>=MON&&p<MON+0x400&&!((p-MON)%0x100)));return p==0?0:hp_of[(p-MON)/0x100];}
    CHECK(k==0x11);return ability_of;
}
static const struct px_battle_handler_api handler_api={&memory,NULL,abort_battle,pokemon_value,NULL,entries,BATTLE,POOL,0,0,0,0,0xe000};
static const struct px_battle_tables_api tables={&handler_api,LAYERS,0,0,overflow,position_entries};
static uint32_t node(unsigned i) { return NODES+0x40*i; }
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;value_calls=0;memset(hp_of,0,sizeof hp_of);ability_of=0;
    for(unsigned i=0;i<PX_BATTLE_HANDLER_POOL;++i)w32(NULL,POOL+4*i,node(i));
}
static uint32_t rnd(uint32_t *s) { *s=*s*1103515245u+12345u;return *s>>8; }
int main(void)
{
    unsigned cases=0;
    /* Packed words. */
    for(uint32_t ai=0;ai<14;++ai)for(uint32_t b=0;b<0x20;b+=5)for(uint32_t flag=0;flag<2;++flag){
        static const uint32_t firsts[14]={0,3,6,9,12,15,18,21,24,27,30,0x100000,0x300000,0xffffffffu};const uint32_t a=firsts[ai];
        reset();w32(NULL,BUF+4,0xdeadbeefu);px_battle_pack_5(BUF,a,b,flag*7,&memory);
        const uint32_t base=((b<<4)&0xf0u)|1u|a<<8,want=flag?(base|0x10000000u):(base&~0x10000000u);
        CHECK(r32(NULL,BUF)==want&&r32(NULL,BUF+4)==0xdeadbeefu);++cases;}
    for(uint32_t a=0;a<0x20;++a)for(uint32_t b=0;b<0x20;b+=3){reset();px_battle_pack_6(BUF,a,b,&memory);
        CHECK(r32(NULL,BUF)==(((a<<4)&0x70u)|3u|((b<<7)&0x380u)));++cases;}
    for(uint32_t seed=1;seed<2000;++seed){uint32_t s=seed;const uint32_t w=rnd(&s)*2654435761u^rnd(&s);const uint32_t k=w&7;
        const uint32_t want=(k>=1&&k<=3)?((w<<7)>>16):(k==4?((w<<1)>>16):0);CHECK(px_battle_unpack_field(w)==want);++cases;}
    /* Records: push up to the eighth entry, then full; init clears everything it should. */
    for(uint32_t count=0;count<10;++count){reset();for(uint32_t i=0;i<0x30;++i)w8(NULL,BUF+i,0xa5);
        w8(NULL,BUF+3,(uint8_t)(count<<4|0x6));px_battle_push_record(BUF,0x1234,&memory);
        for(uint32_t i=0;i<0x30;++i){uint8_t want=0xa5;if(i==3){want=(uint8_t)(count>=8?(count<<4|6):((count+1)<<4|6));}
            if(count<8&&i>=4+4*count&&i<8+4*count){want=(uint8_t)(0x1234>>(8*(i-4-4*count)));}
            CHECK(r8(NULL,BUF+i)==want);}
        ++cases;}
    for(uint32_t nibble=0;nibble<0x20;nibble+=3){reset();for(uint32_t i=0;i<0x30;++i)w8(NULL,BUF+i,0xa5);px_battle_init_record(BUF,nibble,0x1abcd,&memory);
        for(uint32_t i=0;i<0x30;++i){uint8_t want=0xa5;
            if(i==0){want=0xcd;}else if(i==1){want=0xab;}else if(i==2){want=0;}else if(i==3){want=(uint8_t)(nibble&0xf);}
            else if(i>=4&&i<0x24){want=0;}
            CHECK(r8(NULL,BUF+i)==want);}
        ++cases;}
    reset();for(uint32_t i=0;i<16;++i){w8(NULL,BUF+0x40+i,(uint8_t)(0x30+i));w8(NULL,BUF+i,0xa5);}
    px_battle_copy_8(BUF,BUF+0x40,&memory);for(uint32_t i=0;i<16;++i)CHECK(r8(NULL,BUF+i)==(i<8?0x30+i:0xa5));
    px_battle_copy_6(BUF+8,BUF+0x40,&memory);for(uint32_t i=0;i<8;++i)CHECK(r8(NULL,BUF+8+i)==(i<6?0x30+i:0xa5));++cases;
    /* The pair search. */
    for(uint32_t value=0;value<6;++value)for(uint32_t pos=0;pos<11;++pos){reset();for(uint32_t i=0;i<12;++i)w8(NULL,CTX+0x7ec+i,9);
        if(pos<10){w8(NULL,CTX+0x7ec+pos,(uint8_t)value);}
        CHECK(px_battle_pair_search(CTX,value,&memory)==(uint32_t)(value<4&&pos<10));++cases;}
    /* The slot group: every slot value. */
    for(uint32_t slot=0;slot<0x30;++slot){reset();const uint32_t want=slot<=5?0:slot<=11?2:slot<=0x11?1:slot<=0x17?3:0;
        CHECK(px_battle_slot_group(slot,&handler_api)==want&&aborts==(slot>0x17?1u:0u));++cases;}
    CHECK(px_battle_slot_group(0xffffffffu,&handler_api)==0);
    /* The scaled amount. */
    for(uint32_t mask=0;mask<16;++mask)for(uint32_t amount=0;amount<4;++amount){reset();static const uint32_t amounts[4]={1000,49999,50000,0xffffffffu};
        w32(NULL,CTX+0x10,BUF);w8(NULL,BUF+0xb3,(uint8_t)(mask&1));w32(NULL,CTX+0x804,amounts[amount]);w8(NULL,CTX+0x81c,(uint8_t)((mask&2)?8:0xf7));
        w8(NULL,CTX+0xa50,(uint8_t)((mask&4)?1:0));w8(NULL,CTX+0xa51,(uint8_t)((mask&8)?1:0));
        uint32_t want=0;if(!(mask&1)){want=amounts[amount];if(!(mask&2)){uint32_t f=1;if(mask&4)f=2;if(mask&8)f<<=1;if(f>1){want*=f;if(want>99999)want=99999;}}}
        CHECK(px_battle_scaled_amount(CTX,&memory)==want);++cases;}
    /* Pokemon queries with the value callback. */
    for(uint32_t mask=0;mask<32;++mask){reset();hp_of[0]=mask&1;w32(NULL,MON+0x24,(mask&2)?0x104:0x100);w32(NULL,MON+0x28,(mask&4)?0x203:0x200);
        CHECK(px_battle_free_of_conditions_2_3(MON,&handler_api)==(uint32_t)(!(mask&6)&&(mask&1)));
        CHECK(px_battle_free_of_conditions_2_3(0,&handler_api)==0);++cases;}
    for(uint32_t mask=0;mask<16;++mask)for(uint32_t ability=0x66;ability<0x69;++ability){reset();ability_of=ability;
        w32(NULL,CTX+0x290,(mask&1)?5:0);w32(NULL,MON+0x68,(mask&2)?0x104:0x100);w16(NULL,MON+0x12,0x1234);
        CHECK(px_battle_usable_item(MON,CTX,&handler_api)==((mask&3)||ability==0x67?0:0x1234u));++cases;}
    for(uint32_t mask=0;mask<64;++mask){reset();w32(NULL,CTX+0xb8,PARTY);w8(NULL,PARTY+0x18,(uint8_t)(mask&3));w32(NULL,PARTY+4,MON);w32(NULL,PARTY+8,MON+0x100);
        hp_of[0]=mask>>2&1;hp_of[1]=mask>>3&1;w32(NULL,BUF,0x77);const uint32_t n=mask&3;
        uint32_t want=0,out=0x77;if(n>1&&hp_of[0]){want=4;out=0x16;}else if(n>2&&hp_of[1]){want=4;out=0x26;}
        CHECK(px_battle_party_has_hp(CTX,BUF,&handler_api)==want&&r32(NULL,BUF)==out&&aborts==0);++cases;}
    /* Bounded accessors. */
    for(uint32_t index=0;index<14;++index){reset();for(uint32_t i=0;i<0x400;++i)w8(NULL,CTX+0x100+i,(uint8_t)(i*7+1));
        w8(NULL,CTX+0x152,6);w32(NULL,CTX+0x1c,BUF);for(uint32_t i=0;i<14;++i)w32(NULL,BUF+0x274+4*i,i&1?0x55u:0);
        const uint32_t v16=px_battle_byte_16d(CTX,index,&handler_api);CHECK(v16==r8(NULL,CTX+0x16d+index)&&aborts==(index>=5));aborts=0;
        const uint32_t v274=px_battle_entry_274(CTX,index,&handler_api);CHECK(v274==(index<11&&(index&1))&&aborts==(index>=11));aborts=0;
        const uint32_t b=px_battle_byte_11e(CTX,index,&handler_api);CHECK(b==(index<6?r8(NULL,CTX+index*14+0x11e):0u)&&aborts==(index>=6));aborts=0;
        const uint32_t h=px_battle_half_11c(CTX,index,&handler_api);CHECK(h==r16(NULL,CTX+index*14+0x11c)&&aborts==(index>=6));++cases;}
    for(uint32_t side=0;side<4;++side){reset();CHECK(px_battle_is_side_0(side,&handler_api)==(side==0)&&aborts==(side>=2));++cases;}
    /* Clearing a side record: the handler is removed from the list. */
    for(uint32_t side=0;side<2;++side)for(uint32_t id=0;id<17;id+=4)for(uint32_t held=0;held<2;++held){
        reset();const uint32_t rec=LAYERS+(side*17+id)*16;uint32_t handler=0;
        if(held){handler=px_battle_handler_register(2,id,2,0,side,0,0,&handler_api);CHECK(handler==node(0));w32(NULL,rec,handler);}
        w32(NULL,rec+4,0x1234567fu);w32(NULL,rec+8,0x99);w32(NULL,rec+12,3);
        CHECK(px_battle_side_record_clear(side,id,&tables)==(uint32_t)held);
        if(held){CHECK(r32(NULL,rec)==0&&r32(NULL,rec+4)==0x12345678u&&r32(NULL,rec+8)==0x99&&r32(NULL,rec+12)==0&&r32(NULL,BATTLE+0x64)==0&&r32(NULL,BATTLE+0x60)==0);}
        else{CHECK(r32(NULL,rec+4)==0x1234567fu&&r32(NULL,rec+12)==3);}
        ++cases;}
    printf("battle_core_3: %u cases, all tests passed\n",cases);
    return 0;
}
