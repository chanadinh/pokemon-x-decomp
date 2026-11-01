#include "battle_stage.h"
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
static uint32_t ally,grass,commits;static unsigned aborts;static char log_[512];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b,uint32_t c,uint32_t d) { char t[64];snprintf(t,sizeof t,f,a,b,c,d);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static const struct px_battle_ability_api base={&events,NULL,NULL,NULL,NULL,NULL,NULL};
static uint32_t is_ally(void *u,uint32_t id,uint32_t o) { (void)u;CHECK(id<8&&o<8);notef("ally%u/%u ",id,o,0,0);return ally>>(id*8+o)&1; }
static uint32_t has_type(void *u,uint32_t p,uint32_t t) { (void)u;CHECK(t==11);return grass>>mon(p)&1; }
static void blocked_message(void *u,uint32_t c,uint32_t h,uint32_t id,uint32_t w,uint32_t msg) { (void)u;CHECK(c==CTX&&w==WORK);notef("msg%x/%u/%u ",msg,h,id,0); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id,0,0);return RECORD; }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD);++commits;note("commit "); }
static const struct px_battle_stage_api api={&base,is_ally,has_type,blocked_message,action_record,commit};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static void (*const table[])(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_stage_api*)={px_battle_simple,px_battle_contrary};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);ally=grass=commits=0;aborts=0;log_[0]=0;w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
/* The stage frame: the changed battler, the cause, the stat and the stages (a scaled entry). */
static void frame(uint32_t target,uint32_t cause,uint32_t stat,int32_t stages)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x02,target,&events);px_battle_event_set_id(0x03,cause,&events);
    px_battle_event_set_id(0x20,stat,&events);px_battle_event_set_scaled(0x21,(uint32_t)stages,&events);
    px_battle_event_set_value(0x42,0,&events);log_[0]=0;
}
static int32_t stages(void) { return (int32_t)px_battle_event_get(0x21,&events); }
static uint32_t blocked_key(void) { return px_battle_event_get(0x42,&events); }
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_stage_api*);
int main(void)
{
    unsigned cases=0;
    /* Simple doubles and Contrary negates the change of their own battler; Contrary records the rewrite. */
    for(int32_t s=-6;s<=6;++s)for(uint32_t self=0;self<2;++self) {
        reset();frame(self?2:3,1,1,s);w32(NULL,WORK,9);px_battle_simple(HANDLER,CTX,2,WORK,&api);
        CHECK(stages()==(self?s*2:s)&&r32(NULL,WORK)==9);
        reset();frame(self?2:3,1,1,s);w32(NULL,WORK,9);px_battle_contrary(HANDLER,CTX,2,WORK,&api);
        CHECK(stages()==(self?-s:s)&&r32(NULL,WORK)==(self?1u:9u));++cases;
    }
    reset();frame(2,1,1,0x40000001);px_battle_simple(HANDLER,CTX,2,WORK,&api);CHECK(stages()==(int32_t)0x80000002u);++cases;  /* 32-bit */
    /* Simple then Contrary, both through rewrites of the scaled entry. */
    reset();frame(2,1,1,-2);px_battle_simple(HANDLER,CTX,2,WORK,&api);px_battle_contrary(HANDLER,CTX,2,WORK,&api);CHECK(stages()==4);++cases;
    /* Drop blockers: a drop on this battler by another battler, of their stat. */
    {
        const fn f[4]={px_battle_clear_body,px_battle_hyper_cutter,px_battle_keen_eye,px_battle_big_pecks};const int stat[4]={-1,1,6,2};
        for(unsigned k=0;k<4;++k)for(uint32_t target=1;target<4;++target)for(uint32_t cause=1;cause<4;++cause)for(uint32_t st=0;st<8;++st)for(int32_t s=-2;s<=2;++s) {
            reset();frame(target,cause,st,s);w32(NULL,WORK,7);f[k](HANDLER,CTX,2,WORK,&api);
            const int on=target==2&&cause!=2&&(stat[k]<0||st==(uint32_t)stat[k])&&s<0;
            CHECK(blocked_key()==(uint32_t)on&&r32(NULL,WORK)==(on?1u:7u));++cases;
        }
        reset();frame(2,1,1,-1);px_battle_event_rewrite(0x42,1,&events);px_battle_clear_body(HANDLER,CTX,2,WORK,&api);CHECK(r32(NULL,WORK)==0);++cases;
    }
    /* Flower Veil: a Grass-type ally (or itself) losing stats to another battler. */
    for(uint32_t target=1;target<5;++target)for(uint32_t cause=1;cause<5;++cause)for(uint32_t al=0;al<2;++al)for(uint32_t gr=0;gr<2;++gr)for(int32_t s=-1;s<=1;++s) {
        reset();if(al)ally=1u<<(2*8+target);if(gr)grass=1u<<target;frame(target|0x100,cause,1,s);w32(NULL,WORK,7);
        px_battle_flower_veil(HANDLER,CTX,2,WORK,&api);
        /* Key 0x02 is compared in full with its low byte, so a wide id never matches. */
        CHECK(blocked_key()==0);
        reset();if(al)ally=1u<<(2*8+target);if(gr)grass=1u<<target;frame(target,cause,1,s);w32(NULL,WORK,7);
        px_battle_flower_veil(HANDLER,CTX,2,WORK,&api);
        const int on=al&&gr&&cause!=target&&s<0;CHECK(blocked_key()==(uint32_t)on&&r32(NULL,WORK)==(on?1u:7u));++cases;
    }
    /* Messages: the blocked-drop message with the battler and the work area. */
    {
        const fn f[4]={px_battle_clear_body_message,px_battle_hyper_cutter_message,px_battle_keen_eye_message,px_battle_big_pecks_message};
        const uint32_t ids[4]={0xc6,0xc9,0xcf,0xcc};
        for(unsigned k=0;k<4;++k) { reset();frame(5,1,1,-1);f[k](HANDLER,CTX,2,WORK,&api);char b[32];snprintf(b,sizeof b,"msg%x/2/2 ",ids[k]);CHECK(!strcmp(log_,b));++cases; }
        reset();frame(0x305,1,1,-1);px_battle_flower_veil_message(HANDLER,CTX,2,WORK,&api);CHECK(!strcmp(log_,"msg49c/5/2 "));++cases;
    }
    /* Defiant and Competitive: a drop on this battler caused by a non-ally: +2 through a record. */
    {
        const fn f[2]={px_battle_defiant,px_battle_competitive};const uint32_t stat[2]={1,3};
        for(unsigned k=0;k<2;++k)for(uint32_t target=1;target<4;++target)for(uint32_t cause=1;cause<5;++cause)for(uint32_t al=0;al<2;++al)for(int32_t s=-1;s<=1;++s) {
            reset();if(al)ally=1u<<(2*8+cause);w32(NULL,RECORD,0x11);mem[RECORD+0x18]=0x41;frame(target,cause|0x200,1,s);
            f[k](HANDLER,CTX,2,WORK,&api);
            const int on=target==2&&!al&&s<0;CHECK(commits==(uint32_t)on);
            if(on) { CHECK(mem[RECORD+4]==stat[k]&&r32(NULL,RECORD)==0x800011u&&mem[RECORD+0x17]==2&&mem[RECORD+0x18]==0x43&&mem[RECORD+0x16]==1&&mem[RECORD+0xc]==2);
                     char b[48];snprintf(b,sizeof b,"ally2/%u rec%s/2 commit ",cause,"d");CHECK(!strcmp(log_,b)); }
            ++cases;
        }
    }
    /* Through the dispatcher: Simple and Contrary on event 0x63. */
    {
        reset();
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7800+0x10*i;
            w8(NULL,entries,0x63);w32(NULL,entries+4,i);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,2);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);frame(2,1,1,1);px_battle_event_dispatch(CTX,0x63,&events);
        CHECK(stages()==-2&&r32(NULL,HANDLER+0x40+0x1c)==1&&aborts==0);++cases;
    }
    printf("battle_stage: %u cases, all tests passed\n",cases);
    return 0;
}
