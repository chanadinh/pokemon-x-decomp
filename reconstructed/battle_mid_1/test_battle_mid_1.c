#include "battle_mid_1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, OUT=0x3000, MON=0x4000, TABLE=0x5000, STATE=0x6000, BUF=0x6100, OBJ=0x7000,
       LISTS=0x8000, ARRAY=0x9000, SLOTS=0x9800, EFFECT=0xa000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static unsigned aborts;static char log_[2048];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b,uint32_t c) { char t[96];snprintf(t,sizeof t,f,a,b,c);note(t); }
static void abort_battle(void *u) { (void)u;++aborts; }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w) { (void)u;(void)f;(void)h;(void)c;(void)v;(void)w; }
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static uint32_t in_list_ret,owner_v,mod_ret,statuses[0x12];
static uint32_t in_list(void *u,uint32_t v,uint32_t t,uint32_t n) { (void)u;notef("in%x/%x/%x ",v,t,n);return in_list_ret; }
static uint32_t slot_status(void *u,uint32_t c,uint32_t s) { (void)u;CHECK(c==0x777&&s<0x12);return statuses[s]; }
static uint32_t word_owner(void *u,uint32_t w) { (void)u;notef("own%x ",w,0,0);return owner_v; }
static uint32_t modulo(void *u,uint32_t v,uint32_t d) { (void)u;notef("mod%x/%x ",v,d,0);return v%d+(v&0xffff0000u); }
static const struct px_battle_mid_1_api api={&events,in_list,slot_status,word_owner,modulo,EFFECT,STATE,
    LISTS+0x00,LISTS+0x40,LISTS+0x80,LISTS+0xc0,LISTS+0x100,LISTS+0x140,LISTS+0x180,LISTS+0x1c0,LISTS+0x200,LISTS+0x240,LISTS+0x280,LISTS+0x2c0,LISTS+0x300,LISTS+0x340};
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;log_[0]=0;in_list_ret=owner_v=mod_ret=0;memset(statuses,0,sizeof statuses);
    px_battle_event_push(&events);
    w32(NULL,STATE+0x58,BUF);w32(NULL,STATE+0x54,0);
}
static uint32_t key(uint32_t k) { return px_battle_event_get(k,&events); }
static int same(const char *expected) { if(strcmp(log_,expected)){ fprintf(stderr,"log [%s] expected [%s]\n",log_,expected);return 0; } return 1; }
static uint32_t next(uint32_t *s) { *s=*s*1664525u+1013904223u;return *s>>8; }
int main(void)
{
    unsigned cases=0;
    /* Scaling by 4096: every rounding case. */
    {
        const uint32_t vals[]={0,1,2,3,0x7ff,0x800,0x801,0xfff,0x1000,0x1001,0x1800,0x2000,0xffff,0x10000,0xfffff,0x123456};
        for(unsigned i=0;i<sizeof vals/sizeof *vals;++i)for(unsigned j=0;j<sizeof vals/sizeof *vals;++j){
            const uint32_t p=vals[i]*vals[j];uint32_t want=p>>12;if((p&0xfff)>0x800) { ++want; }if(want==0) { want=1; }
            CHECK(px_battle_mid_scale_12(vals[i],vals[j])==want);++cases; }
        CHECK(px_battle_mid_scale_12(0x1000,0x1000)==0x1000&&px_battle_mid_scale_12(2,0x800)==1&&px_battle_mid_scale_12(3,0x800)==1&&px_battle_mid_scale_12(3,0x801)==2);++cases;
    }
    /* Free slots: statuses 0 and 6 (and others) over all patterns of a few bits. */
    for(uint32_t pat=0;pat<4096;++pat){
        reset();uint32_t s=pat*2654435761u;
        for(unsigned i=0;i<0x12;++i){ const uint32_t r=next(&s)%8;statuses[i]=r<2?0:r<4?6:r; }
        memset(mem+SLOTS,0xee,0x20);
        const uint32_t n=px_battle_mid_free_slots(0x777,SLOTS,&api);uint32_t want=0;
        for(unsigned i=0;i<0x12;++i){ if(statuses[i]==0||statuses[i]==6) { CHECK(mem[SLOTS+want]==i);++want; } }
        CHECK(n==want&&mem[SLOTS+want]==0xee);++cases; }
    /* The list tests: tail calls return the service's value; the pairs return 1. */
    {
        const struct { uint32_t (*f)(uint32_t,const struct px_battle_mid_1_api*); uint32_t t1,n1,t2,n2; int never19; } c[]={
            {px_battle_listed_46c4,LISTS+0x00,0x15,0,0,0},{px_battle_listed_4714,LISTS+0x40,7,0,0,0},
            {px_battle_listed_4724,LISTS+0x80,0xe,LISTS+0xc0,0x19,0},{px_battle_listed_47b4,LISTS+0x80,0xe,LISTS+0x140,0x20,0},
            {px_battle_listed_47f4,LISTS+0x180,6,0,0,0},{px_battle_listed_4804,LISTS+0x1c0,6,0,0,0},{px_battle_listed_4814,LISTS+0x200,7,0,0,0},
            {px_battle_listed_4824,LISTS+0x240,0xb,0,0,0},{px_battle_listed_4844,LISTS+0x280,6,0,0,0},
            {px_battle_listed_48e4,LISTS+0x2c0,2,LISTS+0x300,10,0},{px_battle_listed_4854,LISTS+0x2c0,2,LISTS+0x300,10,1}};
        for(unsigned k=0;k<sizeof c/sizeof *c;++k)for(uint32_t v=0;v<2;++v)for(uint32_t ret=0;ret<3;++ret){
            reset();in_list_ret=ret==0?0:ret==1?1:0x100;const uint32_t value=v?0x19:0x1234;log_[0]=0;
            const uint32_t got=c[k].f(value,&api);
            char e[160];
            if(c[k].never19&&value==0x19) { CHECK(got==0&&log_[0]==0); ++cases; continue; }
            if(c[k].t2==0) { snprintf(e,sizeof e,"in%x/%x/%x ",value,c[k].t1,c[k].n1);CHECK(same(e)&&got==in_list_ret); }
            else if(in_list_ret!=0) { snprintf(e,sizeof e,"in%x/%x/%x ",value,c[k].t1,c[k].n1);CHECK(same(e)&&got==1); }
            else { snprintf(e,sizeof e,"in%x/%x/%x in%x/%x/%x ",value,c[k].t1,c[k].n1,value,c[k].t2,c[k].n2);CHECK(same(e)&&got==0); }
            ++cases; }
        /* 108a4764 reads its six halfwords directly. */
        for(uint32_t i=0;i<7;++i)for(uint32_t pos=0;pos<6;++pos){
            reset();for(unsigned k=0;k<6;++k) { w16(NULL,LISTS+0x140+0x0+2*k,0); }
            for(unsigned k=0;k<6;++k) { w16(NULL,LISTS+0x100+2*k,(uint16_t)(0x500+k)); }
            const uint32_t value=i==6?0x500+pos:0x500+i+0x10;
            const uint32_t got=px_battle_listed_4764(value,&api);CHECK(got==(i==6?1u:0u));++cases; }
        for(uint32_t pos=0;pos<8;++pos){ reset();for(unsigned k=0;k<8;++k) { w16(NULL,LISTS+0x100+2*k,(uint16_t)(0x500+k)); }
            CHECK(px_battle_listed_4764(0x500+pos,&api)==(pos<6?1u:0u)&&px_battle_listed_4764(0x10500+pos,&api)==0);++cases; }
        reset();CHECK(px_battle_mid_list_1476(OUT,&api)==LISTS+0x340&&r32(NULL,OUT)==0x36);++cases;
    }
    /* The random unused value. */
    for(uint32_t t=0;t<40;++t){
        reset();uint32_t s=t*7919u+1;const uint32_t count=t<5?t:next(&s)%60;
        for(uint32_t i=0;i<count;++i) { w16(NULL,ARRAY+2*i,(uint16_t)(1+next(&s)%0x270)); }
        if(t==39) { for(uint32_t i=0;i<0x269;++i) { w16(NULL,ARRAY+2*i,(uint16_t)(i+1)); } }
        const uint32_t n=t==39?0x269:count;mod_ret=next(&s)|(t&1?0x10000:0);w32(NULL,STATE+0x54,mod_ret);
        uint16_t unused[0x270];unsigned u=0;
        for(uint32_t v=1;v<0x26a;++v){ int hit=0;for(uint32_t i=0;i<n;++i) { if(r16(NULL,ARRAY+2*i)==v) { hit=1;break; } } if(!hit) { unused[u++]=(uint16_t)v; } }
        const uint32_t got=px_battle_mid_random_unused(ARRAY,n,&api);
        if(u==0) CHECK(got==0&&log_[0]==0);
        else { CHECK(got==unused[mod_ret%u]); }
        ++cases; }
    /* The effect record: every table entry, with and without the second value, the two special kinds and a full list. */
    for(uint32_t e=0;e<17;++e){ w16(NULL,EFFECT+6*e,(uint16_t)(0x20+e)); }
    {
        for(uint32_t kind=0;kind<0x30;++kind)for(uint32_t v=0;v<2;++v)for(uint32_t header=0;header<4;++header){
            reset();for(uint32_t e=0;e<17;++e) { w16(NULL,EFFECT+6*e,(uint16_t)(e==5?8:e==9?0x12:(0x20+e)));w16(NULL,EFFECT+6*e+2,(uint16_t)(e==2?0xffff:e*3+1));w16(NULL,EFFECT+6*e+4,(uint16_t)(e==2?0xffff:e%4==1?0xfff0:e*5+2)); }
            w8(NULL,MON+0x19,0x42);owner_v=0x1234;memset(mem+OUT,0xa5,0x30);w16(NULL,OUT+2,(uint16_t)(header==0?0:header==1?0xc000:header==2?0x3fff:0xffff));
            const uint32_t before=r16(NULL,OUT+2);
            const uint32_t got=px_battle_mid_effect_record(kind,0x99,MON,v?0x55:0,OUT,&api);
            int entry=-1;for(int e=0;e<17;++e) { if(r16(NULL,EFFECT+6*e)==kind) { entry=e;break; } }
            if(entry<0) { CHECK(got==0&&r16(NULL,OUT+2)==before&&r16(NULL,OUT)==0xa5a5);++cases;continue; }
            int32_t id=(int16_t)r16(NULL,EFFECT+6*entry+(v?4:2));if(id<0) { id=(int16_t)r16(NULL,EFFECT+6*entry+2); }
            if(id<0) { CHECK(got==0&&r16(NULL,OUT)==0xa5a5);++cases;continue; }
            CHECK(got==1&&r16(NULL,OUT)==(uint16_t)id);
            const uint32_t count=1+(v?1:0)+((kind==8||kind==0x12)?1:0);
            CHECK(r16(NULL,OUT+2)==(uint16_t)((before&0xc000)|2|(count<<7)));
            CHECK(r32(NULL,OUT+4)==0x42);
            unsigned at=1;if(v) { CHECK(r32(NULL,OUT+4+4*at)==0x55);++at; }
            if(kind==8) { CHECK(r32(NULL,OUT+4+4*at)==0x1234&&strstr(log_,"own99 ")!=NULL); }
            if(kind==0x12) { CHECK(r32(NULL,OUT+4+4*at)==0x49); }
            CHECK(r32(NULL,OUT+4+4*count)==0xa5a5a5a5u);
            ++cases; }
        /* A list that is full (count 8 kept at 8: header bits 7-13 as given) takes no more words, but the header is rebuilt first. */
        reset();for(uint32_t e=0;e<17;++e) { w16(NULL,EFFECT+6*e,(uint16_t)(e==5?8:(0x20+e)));w16(NULL,EFFECT+6*e+2,(uint16_t)(1+e));w16(NULL,EFFECT+6*e+4,(uint16_t)(2+e)); }
        w8(NULL,MON+0x19,7);memset(mem+OUT,0,0x60);const uint32_t got=px_battle_mid_effect_record(8,0x99,MON,3,OUT,&api);
        CHECK(got==1&&r16(NULL,OUT+2)==(2|(3<<7)));++cases;
        /* The first entry (index 0) and the last (index 16) are found. */
        for(uint32_t e=0;e<17;e+=16){ reset();for(uint32_t k=0;k<17;++k) { w16(NULL,EFFECT+6*k,(uint16_t)(0x40+k));w16(NULL,EFFECT+6*k+2,(uint16_t)(0x100+k));w16(NULL,EFFECT+6*k+4,(uint16_t)(0x200+k)); }
            w8(NULL,MON+0x19,1);CHECK(px_battle_mid_effect_record(0x40+e,0,MON,0,OUT,&api)==1&&r16(NULL,OUT)==0x100+e);
            CHECK(px_battle_mid_effect_record(0x40+e,0,MON,1,OUT,&api)==1&&r16(NULL,OUT)==0x200+e);++cases; }
        CHECK(px_battle_mid_effect_record(0x40+17,0,MON,1,OUT,&api)==0);++cases;
    }
    /* The pending flags. */
    for(uint32_t a=0;a<8;++a)for(uint32_t same_owner=0;same_owner<2;++same_owner)for(uint32_t bit25=0;bit25<2;++bit25)for(uint32_t bit31=0;bit31<2;++bit31)
        for(uint32_t b=0;b<2;++b)for(uint32_t c=0;c<2;++c)for(uint32_t k16=0;k16<2;++k16){
        reset();px_battle_event_push(&events);px_battle_event_set_value(0x15,0x1234,&events);px_battle_event_set_value(0x16,k16?4:3,&events);
        px_battle_event_set_value(0x4c,0,&events);px_battle_event_set_value(0x4d,0,&events);
        w32(NULL,OBJ+0x60,a|(bit25?0x2000000u:0)|(bit31?0x80000000u:0));w32(NULL,OBJ+0x70,b?0xff04:0xff00);w32(NULL,OBJ+0x98,c?0xff04:0xf8);
        owner_v=same_owner?0xff34:0xff35;
        px_battle_mid_pending_flags(OBJ,&api);
        uint32_t want4c=0,want4d=0;
        if(a!=0&&same_owner){ const uint32_t flag=(a==1||a==3||a==2)?bit25:(a==4?bit31:0u);const int known=a>=1&&a<=4; if(known&&flag) { want4d=1; } else { want4c=1; } }
        if(b&&k16) { want4c=1; }
        if((c)&&k16) { want4c=1; }
        CHECK(key(0x4c)==want4c&&key(0x4d)==want4d&&aborts==0);++cases; }
    /* A rewritten key does not change again (the second rewrite of the same key is refused by the stack). */
    reset();px_battle_event_push(&events);px_battle_event_set_value(0x15,1,&events);px_battle_event_set_value(0x16,4,&events);
    px_battle_event_set_value(0x4c,0,&events);px_battle_event_set_value(0x4d,0,&events);w32(NULL,OBJ+0x60,1);owner_v=1;w32(NULL,OBJ+0x70,1);w32(NULL,OBJ+0x98,1);
    px_battle_mid_pending_flags(OBJ,&api);CHECK(key(0x4c)==1&&key(0x4d)==0&&aborts==0);++cases;
    printf("battle_mid_1: %u cases, all tests passed\n",cases);
    return 0;
}
