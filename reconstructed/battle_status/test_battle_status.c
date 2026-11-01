#include "battle_status.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static uint32_t ally,grass,weather_value;static unsigned aborts;static char log_[512];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static const struct px_battle_ability_api base={&events,NULL,NULL,NULL,NULL,NULL,NULL};
static uint32_t is_ally(void *u,uint32_t id,uint32_t o) { (void)u;CHECK(id<8&&o<8);return ally>>(id*8+o)&1; }
static uint32_t has_type(void *u,uint32_t p,uint32_t t) { (void)u;CHECK(t==11);return grass>>mon(p)&1; }
static uint32_t weather(void *u,uint32_t c) { (void)u;CHECK(c==CTX);return weather_value; }
static uint32_t major_status(void *u,uint32_t s) { (void)u;return s>=1&&s<=5; }
static uint32_t mental_status(void *u,uint32_t s) { (void)u;return s==7||s==0xb||s==0x10; }
static void popup(void *u,uint32_t c,uint32_t p,uint32_t id) { (void)u;CHECK(c==CTX);notef("pop%u/%u ",p,id); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;CHECK(k==2&&r==RECORD+4);notef("msg%x ",id,0); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;CHECK(r==RECORD+4);notef("arg%x ",v,0); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD);note("commit "); }
static const struct px_battle_status_api api={&base,is_ally,has_type,weather,major_status,mental_status,popup,action_record,message,message_arg,commit};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static void (*const table[])(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_status_api*)={
        px_battle_limber,px_battle_status_blocked_message};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);ally=grass=weather_value=0;aborts=0;log_[0]=0;w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
static void frame(uint32_t cause,uint32_t target,uint32_t code)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x03,cause,&events);px_battle_event_set_id(0x04,target,&events);px_battle_event_set_id(0x1e,code,&events);
    px_battle_event_set_value(0x42,0,&events);px_battle_event_set_value(0x44,0,&events);log_[0]=0;
}
static uint32_t blocked_key(void) { return px_battle_event_get(0x42,&events); }
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_status_api*);
int main(void)
{
    unsigned cases=0;
    /* One- and two-status abilities: the target, the 16-bit code, the work word (0 when not blocked). */
    {
        const fn f[7]={px_battle_limber,px_battle_magma_armor,px_battle_water_veil,px_battle_immunity_status,px_battle_own_tempo,
                       px_battle_insomnia,px_battle_oblivious_status};
        const uint32_t a[7]={1,3,4,5,6,2,7},b[7]={99,99,99,99,99,0xe,0xb};
        for(unsigned k=0;k<7;++k)for(uint32_t target=1;target<4;++target)for(uint32_t code=0;code<0x12;++code)for(uint32_t wide=0;wide<2;++wide) {
            reset();frame(1,target,code|(wide?0x10000:0));w32(NULL,WORK,7);f[k](HANDLER,CTX,2,WORK,&api);
            const int on=target==2&&(code==a[k]||code==b[k]);
            CHECK(blocked_key()==(uint32_t)on&&r32(NULL,WORK)==(uint32_t)on);++cases;
        }
        /* An already blocked status leaves 0 in the work word. */
        reset();frame(1,2,1);px_battle_event_rewrite(0x42,1,&events);w32(NULL,WORK,7);px_battle_limber(HANDLER,CTX,2,WORK,&api);CHECK(r32(NULL,WORK)==0);++cases;
    }
    /* Leaf Guard: sun, major statuses or drowsiness; the work word is left alone otherwise. */
    for(uint32_t w=0;w<3;++w)for(uint32_t code=0;code<0x12;++code)for(uint32_t target=1;target<4;++target) {
        reset();weather_value=w;frame(1,target,code);w32(NULL,WORK,7);px_battle_leaf_guard(HANDLER,CTX,2,WORK,&api);
        const int on=target==2&&w==1&&((code>=1&&code<=5)||code==0xe);CHECK(blocked_key()==(uint32_t)on&&r32(NULL,WORK)==(on?1u:7u));++cases;
    }
    /* Sweet Veil: sleep and drowsiness on allies (or itself); others untouched. */
    for(uint32_t target=1;target<5;++target)for(uint32_t al=0;al<2;++al)for(uint32_t code=0;code<0x10;++code) {
        reset();if(al)ally=1u<<(2*8+target);frame(1,target,code);w32(NULL,WORK,7);px_battle_sweet_veil(HANDLER,CTX,2,WORK,&api);
        const int on=al&&(code==2||code==0xe);CHECK(blocked_key()==(uint32_t)on&&r32(NULL,WORK)==(al?(uint32_t)on:7u));++cases;
    }
    /* Flower Veil: major statuses and drowsiness on Grass-type allies, not self-inflicted. */
    for(uint32_t target=1;target<5;++target)for(uint32_t cause=1;cause<5;++cause)for(uint32_t al=0;al<2;++al)for(uint32_t gr=0;gr<2;++gr)for(uint32_t code=0;code<0x10;++code) {
        reset();if(al)ally=1u<<(2*8+target);if(gr)grass=1u<<target;frame(cause|0x100,target,code);w32(NULL,WORK,7);
        px_battle_flower_veil_status(HANDLER,CTX,2,WORK,&api);
        const int on=al&&gr&&cause!=target&&(code<6||code==0xe);CHECK(blocked_key()==(uint32_t)on&&r32(NULL,WORK)==(on?1u:7u));++cases;
    }
    /* Aroma Veil: the mental statuses on allies; key 0x44 as well. */
    for(uint32_t target=1;target<5;++target)for(uint32_t al=0;al<2;++al)for(uint32_t code=0;code<0x12;++code) {
        reset();if(al)ally=1u<<(2*8+target);frame(1,target,code);w32(NULL,WORK,7);px_battle_aroma_veil(HANDLER,CTX,2,WORK,&api);
        const int on=al&&(code==7||code==0xb||code==0x10);
        CHECK(blocked_key()==(uint32_t)on&&px_battle_event_get(0x44,&events)==(uint32_t)on&&r32(NULL,WORK)==(on?1u:7u));++cases;
    }
    reset();ally=1u<<(2*8+3);frame(1,3,7);px_battle_event_rewrite(0x42,1,&events);w32(NULL,WORK,7);px_battle_aroma_veil(HANDLER,CTX,2,WORK,&api);
    CHECK(r32(NULL,WORK)==0&&px_battle_event_get(0x44,&events)==0);++cases;
    /* Uproar: while work word 6 is set, sleep (full word) is blocked with a message naming the target. */
    for(uint32_t up=0;up<2;++up)for(uint32_t target=1;target<4;++target)for(uint32_t code=0;code<4;++code)for(uint32_t wide=0;wide<2;++wide) {
        reset();w32(NULL,WORK+0x18,up);frame(1,target|0x300,code|(wide?0x10000:0));px_battle_uproar(HANDLER,CTX,2,WORK,&api);
        const int on=up&&code==2&&!wide;CHECK(blocked_key()==(uint32_t)on);
        if(on){char b[64];snprintf(b,sizeof b,"rec3/2 msg%x arg%x commit ",target==2?0x2c8:0x2c5,target);CHECK(!strcmp(log_,b));}
        ++cases;
    }
    /* Messages of event 0x70. */
    for(uint32_t target=1;target<4;++target)for(uint32_t wv=0;wv<3;++wv) {
        reset();frame(1,target,1);w32(NULL,WORK,wv);px_battle_status_blocked_message(HANDLER,CTX,2,WORK,&api);
        const int on=target==2&&wv==1;
        CHECK(!strcmp(log_,on?"pop1/2 rec3/2 msgd2 arg2 commit pop2/2 ":"")&&r32(NULL,WORK)==(on?0u:wv));
        reset();frame(1,target,1);w32(NULL,WORK,wv);px_battle_own_tempo_message(HANDLER,CTX,2,WORK,&api);
        CHECK(!strcmp(log_,on?"pop1/2 rec3/2 msg165 arg2 commit pop2/2 ":""));++cases;
    }
    {
        const fn f[3]={px_battle_aroma_veil_message,px_battle_flower_veil_status_message,px_battle_sweet_veil_message};const uint32_t ids[3]={0x49f,0x49c,0x4a2};
        for(unsigned k=0;k<3;++k)for(uint32_t target=1;target<4;++target)for(uint32_t al=0;al<2;++al)for(uint32_t wv=0;wv<3;++wv) {
            reset();if(al)ally=1u<<(2*8+target);frame(1,target|0x100,1);w32(NULL,WORK,wv);f[k](HANDLER,CTX,2,WORK,&api);
            const int on=al&&wv;char b[96];snprintf(b,sizeof b,"pop1/2 rec3/2 msg%x arg%x commit pop2/2 ",ids[k],target);
            CHECK(!strcmp(log_,on?b:"")&&r32(NULL,WORK)==(on?0u:wv));++cases;
        }
    }
    /* Through the dispatcher: Limber blocks paralysis on event 0x6e, its message appears on event 0x70. */
    {
        reset();
        const uint32_t h=HANDLER,entries=0x7800;
        w8(NULL,entries,0x6e);w32(NULL,entries+4,0);w8(NULL,entries+8,0x70);w32(NULL,entries+12,1);
        w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|2u<<16);w8(NULL,h+0x3a,2);w32(NULL,BATTLE+0x60,h);
        frame(1,2,1);px_battle_event_dispatch(CTX,0x6e,&events);CHECK(blocked_key()==1&&r32(NULL,h+0x1c)==1);
        log_[0]=0;px_battle_event_dispatch(CTX,0x70,&events);CHECK(!strcmp(log_,"pop1/2 rec3/2 msgd2 arg2 commit pop2/2 ")&&r32(NULL,h+0x1c)==0&&aborts==0);++cases;
    }
    printf("battle_status: %u cases, all tests passed\n",cases);
    return 0;
}
