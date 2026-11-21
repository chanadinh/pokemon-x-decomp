#include "battle_core_1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, MON=0x6000, RECORD=0x7000, HANDLER=0x9e00,
       ENTRIES=0x9f00, ARRAY=0xa000, BUFFER=0xb000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static unsigned aborts,calls;static uint32_t block_event;static char log_[512];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[64];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts; }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;(void)v;(void)w;CHECK(f==0x77&&h==HANDLER&&c==CTX);++calls;
    if(block_event)CHECK(px_battle_event_rewrite(0x42,1,&events)==1);
}
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;(void)p;(void)k;CHECK(0);return 0; }
static const struct px_battle_ability_api base={.events=&events,.pokemon_value=pokemon_value};
static unsigned records;
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id)
{ (void)u;CHECK(c==CTX&&records<2);notef("rec%x/%x ",k,id);return RECORD+0x100*records++; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id)
{ (void)u;char t[64];snprintf(t,sizeof t,"msg%x@%x/%x ",id,(r-RECORD)&0xff,k);note(t); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;notef("arg%x@%x ",v,(r-RECORD)&0xff); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r>=RECORD);notef("commit%u ",(r-RECORD)>>8,0); }
static const struct px_battle_items_api items={&base,NULL,NULL,NULL,action_record,message,message_arg,commit,0,0};
static const struct px_battle_core_api api={&items};
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;calls=0;records=0;block_event=0;log_[0]=0;
    w32(NULL,CTX+8,CTX+0x400);
    for(unsigned i=0;i<0x18;++i)w32(NULL,CTX+0x400+0x84+4*i,MON+0x200*i);
}
static uint32_t rnd(uint32_t *s) { *s=*s*1103515245u+12345u;return *s>>8; }
int main(void)
{
    unsigned cases=0;
    /* Pure helpers. */
    for(uint32_t a=0;a<0x30;++a)for(uint32_t b=0;b<0x30;++b) {
        const uint32_t want=(a==0x1f||b==0x1f)?0:((a>=12)==(b>=12));CHECK(px_battle_same_side(a,b)==want);++cases;
    }
    for(uint32_t v=0;v<0x1000;v+=3){CHECK(px_battle_pack_1(v)==(1u|v<<9)&&px_battle_pack_3(v)==((v<<3&0x1f8u)|3u));++cases;}
    CHECK(px_battle_pack_1(0xffffffffu)==0xfffffe01u&&px_battle_pack_3(0xffffffffu)==0x1fbu);
    /* Type check: every pair of type bytes, the flag, the third type byte and the asked type. */
    for(uint32_t t1=0;t1<0x14;++t1)for(uint32_t t2=0;t2<0x14;++t2)for(uint32_t flag=0;flag<9;flag+=4)for(uint32_t third=0;third<0x14;third+=7)
        for(uint32_t type=0;type<0x14;++type) {
            reset();w8(NULL,MON+0x100,(uint8_t)t1);w8(NULL,MON+0x101,(uint8_t)t2);w8(NULL,MON+0x102,(uint8_t)third);w32(NULL,MON+0x7c,flag?0x10000008u|flag:0x10000000u);
            const int f=(flag&7)!=0;uint32_t a=t1,b=t2;
            if(a==2&&f){a=0x12;}
            if(b==2&&f){b=0x12;}
            uint32_t first=a,second=b;
            if(first==0x12&&second==0x12){first=second=0;}
            else if(first==0x12){first=second;}
            else if(second==0x12){second=first;}
            const uint32_t want=type==0x12?0:(first==type||second==type||third==type);
            CHECK(px_battle_has_type(MON,type,&memory)==want);++cases;
        }
    /* Conditions: the first of five words with a low-3-bit value. */
    for(uint32_t mask=0;mask<32;++mask)for(uint32_t v=0;v<8;++v) {
        reset();uint32_t want=0;
        for(uint32_t i=1;i<=5;++i){const uint32_t on=mask>>(i-1)&1;w32(NULL,MON+0x1c+4*i,on?(0x100u|(v?v:1)):0x1f8u);if(on&&!want)want=i;}
        CHECK(px_battle_pokemon_status(MON,&memory)==want);++cases;
    }
    reset();w32(NULL,MON+0x1c+4,0xfffffff8u);CHECK(px_battle_pokemon_status(MON,&memory)==0);++cases;
    /* Ten three-byte entries. */
    for(uint32_t seed=1;seed<200;++seed) {
        reset();uint32_t s=seed;uint32_t want=10;const uint32_t value=rnd(&s)%4;
        for(uint32_t i=0;i<10;++i){const uint32_t flag=rnd(&s)%2,val=rnd(&s)%4;w8(NULL,ARRAY+3*i,(uint8_t)flag);w8(NULL,ARRAY+3*i+1,(uint8_t)rnd(&s));w8(NULL,ARRAY+3*i+2,(uint8_t)val);
            if(want==10&&flag&&val==value)want=i;}
        CHECK(px_battle_find_entry(ARRAY,value,&memory)==want);++cases;
    }
    /* First id of a four-byte table whose bit is set. */
    for(uint32_t mask=0;mask<16;++mask) {
        reset();static const uint8_t ids[4]={6,0xd,0x42,0x2b};for(unsigned i=0;i<4;++i)w8(NULL,BUFFER+i,ids[i]);
        uint32_t want=0x12;
        for(unsigned i=0;i<4;++i)if(mask>>i&1){const uint32_t id=ids[i];w8(NULL,CTX+0x169+(id>>3),(uint8_t)(r8(NULL,CTX+0x169+(id>>3))|(1u<<(id&7))));if(want==0x12&&i<4)want=id;}
        for(unsigned i=0;i<4;++i)if(mask>>i&1){want=ids[i];break;}
        if(!mask)want=0x12;
        CHECK(px_battle_first_set_id(CTX,BUFFER,&memory)==want);++cases;
    }
    /* Simple fields. */
    reset();w32(NULL,MON+0x1c,0x7100);w8(NULL,0x7109,0xa7);CHECK(px_battle_byte_9(MON,&memory)==0xa7);
    w8(NULL,MON+0x1b,0x20);CHECK(px_battle_bit_5_of_1b(MON,&memory)==1);w8(NULL,MON+0x1b,0xdf);CHECK(px_battle_bit_5_of_1b(MON,&memory)==0);
    w32(NULL,BUFFER,0x12345671u);CHECK(px_battle_field_16(BUFFER,&memory)==0x3456);w32(NULL,BUFFER,0x12345672u);CHECK(px_battle_field_16(BUFFER,&memory)==0);
    w32(NULL,BUFFER,0x1234567fu);CHECK(px_battle_field_16(BUFFER,&memory)==0);cases+=3;
    w32(NULL,BUFFER+0x14,0xabcd1234u);CHECK(px_battle_low_half(BUFFER,&memory)==0x1234);
    w16(NULL,CTX+0x160,0x4321);w16(NULL,CTX+0x15c,0x8765);w32(NULL,CTX+0x1080,0);CHECK(px_battle_half_160(CTX,&memory)==0x4321&&px_battle_half_15c(CTX,&memory)==0x8765&&px_battle_word_1080_set(CTX,&memory)==0);
    w32(NULL,CTX+0x1080,0x100);CHECK(px_battle_word_1080_set(CTX,&memory)==1);++cases;
    for(uint32_t n=0;n<8;++n){reset();for(uint32_t i=0;i<6;++i){w32(NULL,ARRAY+4*i,0x1000+i);}
        w8(NULL,ARRAY+0x18,(uint8_t)n);
        for(uint32_t i=0;i<6;++i){CHECK(px_battle_array_word(ARRAY,i,&memory)==(i<n?0x1000+i:0));}
        CHECK(px_battle_array_word(ARRAY,n,&memory)==0);
        ++cases;}
    for(uint32_t i=0;i<0x40;++i){reset();w8(NULL,CTX+0x166+(i>>3),(uint8_t)(1u<<(i&7)));CHECK(px_battle_bit_set(CTX,i,&memory)==1);
        w8(NULL,CTX+0x166+(i>>3),(uint8_t)~(1u<<(i&7)));CHECK(px_battle_bit_set(CTX,i,&memory)==0);++cases;}
    /* The history ring. */
    for(uint32_t seed=1;seed<100;++seed) {
        reset();uint32_t s=seed;const uint32_t head=rnd(&s)%400,count=rnd(&s)%400;
        w32(NULL,BUFFER,head);
        for(uint32_t i=0;i<400;++i){w16(NULL,BUFFER+i*8+8,(uint16_t)(rnd(&s)%3+(i<count?1:0)*0));w32(NULL,BUFFER+i*8+4,rnd(&s)%3);}
        if(seed%3==0)for(uint32_t k=1;k<5;++k)w16(NULL,BUFFER+((head+400-k)%400)*8+8,0);
        uint32_t want=0,idx=head;
        for(;;){idx=idx?idx-1:399;if(idx==head)break;const uint32_t kind=r16(NULL,BUFFER+idx*8+8);if(kind==0)break;if(kind==1&&r32(NULL,BUFFER+idx*8+4)==2)++want;}
        CHECK(px_battle_count_matches(BUFFER,1,2,&memory)==want);++cases;
    }
    /* A full ring: every entry matches, so a walk from any head covers the whole ring. */
    {static const uint32_t heads[]={0,1,2,5,398,399};
     for(unsigned h=0;h<sizeof heads/sizeof *heads;++h){
        reset();w32(NULL,BUFFER,heads[h]);
        for(uint32_t i=0;i<400;++i){w16(NULL,BUFFER+i*8+8,1);w32(NULL,BUFFER+i*8+4,2);}
        CHECK(px_battle_count_matches(BUFFER,1,2,&memory)==399);
        CHECK(px_battle_count_matches(BUFFER,1,3,&memory)==0&&px_battle_count_matches(BUFFER,2,2,&memory)==0);++cases;}}
    /* Battler entries: kind words, flag bytes and slots. */
    for(uint32_t seed=1;seed<400;++seed) {
        reset();uint32_t s=seed;const uint32_t count=rnd(&s)%5;w8(NULL,CTX+0x108c,(uint8_t)count);
        uint32_t want=0,wordwant=0,others=0,flagged=0,slot=rnd(&s)%3;
        for(uint32_t i=0;i<4;++i){
            const uint32_t entry=CTX+0x10bc+16*i,mon=MON+0x200*i,kind=rnd(&s)%10;
            w32(NULL,entry,mon);w8(NULL,mon+0x19,(uint8_t)(rnd(&s)%3));w32(NULL,entry+4,0x70u+kind);w8(NULL,entry+0xd,(uint8_t)(rnd(&s)%2));
            if(i<count){
                if(r8(NULL,mon+0x19)==slot){if(!want&&kind!=6&&kind!=8){want=1;wordwant=0x70u+kind;}}
                else{++others;if(r8(NULL,entry+0xd))++flagged;}
            }
        }
        w32(NULL,BUFFER,0xdeadbeefu);
        CHECK(px_battle_first_kind_word(CTX,slot,BUFFER,&memory)==want&&r32(NULL,BUFFER)==(want?wordwant:0xdeadbeefu));
        CHECK(px_battle_others_flagged(CTX,slot,&memory)==(others==flagged));++cases;
    }
    /* Word rewrites. */
    for(uint32_t seed=1;seed<500;++seed) {
        reset();uint32_t s=seed;const uint32_t w=rnd(&s)*2654435761u^rnd(&s);w32(NULL,BUFFER,w);
        uint32_t v=w>>10<<10;const uint32_t f=(v<<13)>>23;v&=~0xc000000u;v&=~0x3f80000u;v|=f<<19;v&=~0x70000000u;
        px_battle_normalize_word(BUFFER,&memory);CHECK(r32(NULL,BUFFER)==v&&r32(NULL,BUFFER+4)==0);
        for(uint32_t bit=0;bit<2;++bit){w32(NULL,BUFFER+8,w);px_battle_set_kind_bit(BUFFER+8,bit,&memory);
            const uint32_t k=w&7;uint32_t e=w;if(k>=1&&k<=3)e=(w&~0x2000000u)|bit<<25;else if(k==4)e=(w&~0x80000000u)|bit<<31;
            CHECK(r32(NULL,BUFFER+8)==e);}
        w16(NULL,BUFFER+0x102,(uint16_t)w);w32(NULL,BUFFER+0x120,0x5555);px_battle_mark_if_small(BUFFER+0x100,0x99,&memory);
        const uint32_t half=(uint16_t)w;const int small=((half<<18)>>25)<8;
        CHECK(r16(NULL,BUFFER+0x102)==(uint16_t)(small?(half|0x4000):half)&&r32(NULL,BUFFER+0x120)==(small?0x99u:0x5555u));++cases;
    }
    reset();w32(NULL,CTX+0xdb4,BUFFER);w32(NULL,BUFFER+4,0x11223344u);px_battle_store_byte_db4(CTX,0x1ab,&memory);CHECK(r32(NULL,BUFFER+4)==0x112233abu&&r32(NULL,BUFFER)==0);++cases;
    px_battle_nop();
    /* Event 0x83: no handler, then one that sets key 0x42. */
    for(uint32_t block=0;block<2;++block){
        reset();block_event=block;w32(NULL,CTX+0x1c,BUFFER);w8(NULL,BUFFER+1,0x5c);
        w32(NULL,BATTLE+0x60,HANDLER);w32(NULL,HANDLER+8,ENTRIES);w8(NULL,ENTRIES,0x83);w32(NULL,ENTRIES+4,0x77);
        w32(NULL,HANDLER+0x18,0x20000000u|1u<<16);w8(NULL,HANDLER+0x10,5);w8(NULL,HANDLER+0x3b,0);
        w32(NULL,HANDLER+4,0);w32(NULL,HANDLER,0);
        CHECK(px_battle_event_0x83(CTX,&api)==(block?0u:0x5cu)&&calls==1&&aborts==0);++cases;
        CHECK(r32(NULL,STACK)==0);   /* the frame was popped */
    }
    /* The record builder. */
    for(uint32_t seed=0;seed<200;++seed){
        reset();uint32_t s=seed+7;const uint32_t id=rnd(&s)%0x18,value=rnd(&s)%4,old=rnd(&s)%4,flag=rnd(&s)%3;
        w8(NULL,MON+0x200*id+0x153,(uint8_t)old);w32(NULL,RECORD,0xffffffffu);w8(NULL,RECORD+4,0xaa);w8(NULL,RECORD+5,0xaa);
        px_battle_record_change(0x1234,CTX,id,value,flag,&api);
        if(value==old){CHECK(log_[0]==0&&records==0&&r8(NULL,RECORD+4)==0xaa);}
        else{char want[160];snprintf(want,sizeof want,"rec3a/%x msgde@8/2 arg%x@8 commit0 ",id,id);CHECK(!strcmp(log_,want));
            CHECK(r8(NULL,RECORD+4)==id&&r8(NULL,RECORD+5)==value&&r32(NULL,RECORD)==(flag&1?0xffffffffu:0xff7fffffu));}
        ++cases;
    }
    printf("battle_core_1: %u cases, all tests passed\n",cases);
    return 0;
}
