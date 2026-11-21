#include "battle_items_13.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000,
       HANDLER=0x6000, WORK=0x601c, RECORD=0x7000, CURES=0x7800, SCRATCH=0x7900, ENTRIES=0x7a00 };
enum { RUNNING=0x1000000, REGISTERED=0x20000000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static uint32_t ally,nocons;static unsigned aborts,records;static char log_[512];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static const struct px_battle_ability_api base={.events=&events};
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id)
{ (void)u;CHECK(c==CTX&&records<1);notef("rec%x/%x ",k,id);++records;return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;char t[48];snprintf(t,sizeof t,"msg%x@%x/%x ",id,r-RECORD,k);note(t); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;notef("arg%x@%x ",v,r-RECORD); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD);note("commit "); }
static const struct px_battle_items_api items={&base,NULL,NULL,NULL,action_record,message,message_arg,commit,CURES,SCRATCH};
static void prize_money(void *u,uint32_t c,uint32_t id,uint32_t v) { (void)u;CHECK(c==CTX);notef("pm%u/%u ",id,v); }
static uint32_t is_ally(void *u,uint32_t id,uint32_t other) { (void)u;notef("al%x/%u ",id,other);return ally; }
static void message_extra(void *u,uint32_t r,uint32_t v) { (void)u;notef("ex%x@%x ",v,r-RECORD); }
static uint32_t infatuation_condition(void *u,uint32_t id) { (void)u;notef("ic%u ",id,0);return 0xc0de0000|id; }
static uint32_t no_consume(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("nc ");return nocons; }
static const struct px_battle_items_13_api api={&items,prize_money,is_ally,message_extra,infatuation_condition,no_consume};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_items_13_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_destiny_knot,px_battle_amulet_coin_61};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);ally=0;nocons=0;aborts=0;records=0;log_[0]=0;
    w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
    w16(NULL,HANDLER+0x38,0x118);w32(NULL,HANDLER+0x18,RUNNING);
}
/* Keys 0x02, 0x03, 0x04, 0x1e, 0x33 and the rewritable 0x53, 0x4c, 0x42 and multiplier 0x36. */
static void frame(uint32_t k2,uint32_t k3,uint32_t k4,uint32_t k1e,uint32_t k33)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x02,k2,&events);px_battle_event_set_id(0x03,k3,&events);
    px_battle_event_set_id(0x04,k4,&events);px_battle_event_set_value(0x1e,k1e,&events);px_battle_event_set_value(0x33,k33,&events);
    px_battle_event_set_value(0x53,0,&events);px_battle_event_set_value(0x4c,0,&events);px_battle_event_set_value(0x42,0,&events);
    px_battle_event_set_multiplier(0x36,0x1000,0,0,&events);log_[0]=0;
}
static void lock(void) { CHECK(px_battle_event_rewrite(0x53,0,&events)==1&&px_battle_event_rewrite(0x4c,0,&events)==1&&px_battle_event_rewrite(0x42,0,&events)==1);log_[0]=0; }
static uint32_t key(uint32_t k) { return px_battle_event_get(k,&events); }
static unsigned cases;
static void expect(const char *s) { if(strcmp(log_,s)){fprintf(stderr,"got  [%s]\nwant [%s]\n",log_,s);} CHECK(!strcmp(log_,s));++cases; }
int main(void)
{
    /* Amulet Coin and Luck Incense: the holder on event 0x5e, anyone on 0x61. */
    {
        for(uint32_t holder=0;holder<5;++holder){
            reset();frame(holder,7,7,0,0);px_battle_amulet_coin_5e(HANDLER,CTX,2,WORK,&api);expect(holder==2?"pm2/0 ":"");
            reset();frame(holder,7,7,0,0);px_battle_amulet_coin_61(HANDLER,CTX,2,WORK,&api);expect("pm2/0 ");
        }
        reset();frame(7,7,7,0,0);px_battle_amulet_coin_61(HANDLER,CTX,5,WORK,&api);expect("pm5/0 ");
    }
    /* Smoke Ball: the ally test on key 0x02's low byte, and whether the rewrite took. */
    {
        const uint32_t k2s[]={0,2,3,0x103};
        for(unsigned k=0;k<4;++k)for(uint32_t a=0;a<2;++a)for(unsigned locked=0;locked<2;++locked){
            char b[160];
            reset();ally=a;frame(k2s[k],7,7,0,0);if(locked)lock();px_battle_smoke_ball_0b(HANDLER,CTX,2,WORK,&api);
            snprintf(b,sizeof b,"al%x/2 ",k2s[k]&0xff);expect(b);CHECK(key(0x53)==(a&&!locked)&&aborts==0);
            reset();ally=a;frame(k2s[k],7,7,0,0);if(locked)lock();px_battle_smoke_ball_0d(HANDLER,CTX,2,WORK,&api);
            snprintf(b,sizeof b,"al%x/2 %s",k2s[k]&0xff,a&&!locked?"rec3/2 msg3f2@4/2 arg2@4 arg118@4 exd0024@4 commit ":"");expect(b);
            CHECK(key(0x53)==(a&&!locked)&&aborts==0);
        }
    }
    /* Destiny Knot: the holder hit, condition 7, attacker 0x1f; every byte of the record. */
    {
        const uint32_t attackers[]={0,5,0x1f,0x11f,0x105};
        for(uint32_t h4=1;h4<4;++h4)for(uint32_t k1e=5;k1e<9;++k1e)for(unsigned a=0;a<5;++a){
            reset();frame(7,attackers[a],h4,k1e,0);px_battle_destiny_knot(HANDLER,CTX,2,WORK,&api);
            const uint32_t att=attackers[a]&0xff;const int hit=h4==2&&k1e==7&&att!=0x1f;
            char b[160]="";if(hit)snprintf(b,sizeof b,"recb/2 ic2 msg14a@14/2 arg%x@14 arg118@14 commit ",att);
            expect(b);
            if(hit){ CHECK(r8(NULL,RECORD+0xf)==att&&r16(NULL,RECORD+4)==7&&r32(NULL,RECORD+8)==0xc0de0002&&r8(NULL,RECORD+0xc)==1&&r8(NULL,RECORD+0xd)==1);
                for(uint32_t k=0;k<0x100;++k)if(k!=4&&k!=5&&!(k>=8&&k<=0xd)&&k!=0xf)CHECK(r8(NULL,RECORD+k)==0); }
            else CHECK(records==0);
        }
    }
    /* Float Stone: the holder's multiplier x0.5, with rounding. */
    {
        const uint32_t starts[]={0x1000,0x1001,0x3,0x7fffffff};
        for(uint32_t holder=1;holder<4;++holder)for(unsigned s=0;s<4;++s){
            reset();frame(holder,7,7,0,0);px_battle_event_pop(&events);
            px_battle_event_push(&events);px_battle_event_set_id(0x02,holder,&events);px_battle_event_set_multiplier(0x36,starts[s],0,0,&events);log_[0]=0;
            px_battle_float_stone(HANDLER,CTX,2,WORK,&api);expect("");
            const uint32_t half=(uint32_t)((uint64_t)((int64_t)(int32_t)starts[s]*0x800+0x800)>>12);
            CHECK(key(0x36)==(holder==2?half:starts[s]));
        }
    }
    /* Ring Target: the non-consuming pass first, then the holder hit; and a rewrite that does not take. */
    {
        for(uint32_t h4=1;h4<4;++h4)for(uint32_t nc=0;nc<2;++nc)for(unsigned locked=0;locked<2;++locked){
            reset();nocons=nc;frame(7,7,h4,0,0);if(locked)lock();px_battle_ring_target(HANDLER,CTX,2,WORK,&api);
            expect("nc ");CHECK(key(0x4c)==(!nc&&h4==2&&!locked)&&aborts==0);
        }
    }
    /* Safety Goggles: the holder, key 0x33 signed positive. */
    {
        const uint32_t k33s[]={0,1,5,0x7fffffff,0x80000000,0xffffffff};
        for(uint32_t holder=1;holder<4;++holder)for(unsigned k=0;k<6;++k)for(unsigned locked=0;locked<2;++locked){
            reset();frame(holder,7,7,0,k33s[k]);if(locked)lock();px_battle_safety_goggles(HANDLER,CTX,2,WORK,&api);
            expect("");CHECK(key(0x42)==(holder==2&&(int32_t)k33s[k]>0&&!locked));
        }
    }
    /* Through the dispatcher: Destiny Knot on event 0x72 and Amulet Coin's handler on 0x61. */
    {
        reset();w32(NULL,HANDLER+0x18,REGISTERED|1u<<16);
        w8(NULL,ENTRIES,0x72);w32(NULL,ENTRIES+4,0);w8(NULL,ENTRIES+8,0x61);w32(NULL,ENTRIES+12,1);
        w32(NULL,HANDLER+8,ENTRIES);w8(NULL,HANDLER+0x3a,2);w32(NULL,BATTLE+0x60,HANDLER);
        frame(7,4,2,7,0);px_battle_event_dispatch(CTX,0x72,&events);expect("recb/2 ic2 msg14a@14/2 arg4@14 arg118@14 commit ");
        w32(NULL,HANDLER+8,ENTRIES+8);log_[0]=0;px_battle_event_dispatch(CTX,0x61,&events);expect("pm2/0 ");CHECK(aborts==0);
    }
    printf("battle_items_13: %u cases, all tests passed\n",cases);
    return 0;
}
