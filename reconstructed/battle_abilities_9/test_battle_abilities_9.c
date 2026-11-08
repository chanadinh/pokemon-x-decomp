#include "battle_abilities_9.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, ME=2, OTHER=5 };
enum { INFIL=0x45d0, DAMP=0x4494, MOLD=0x4e48, UNNERVE=0x4e70, KLUTZ=0x3750 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static uint32_t category_value;
static unsigned aborts,ncommit;static uint8_t commits[4][0x20];static char log_[1024];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static uint32_t move_category(void *u,uint32_t m) { (void)u;notef("cat%x ",m,0);return category_value; }
static const struct px_battle_ability_api base={&events,NULL,move_category,NULL,NULL,NULL,NULL};
static uint32_t side_of(void *u,uint32_t id) { (void)u;notef("side%u ",id,0);return id&1; }
static uint32_t opposing_side(void *u,uint32_t s) { (void)u;notef("opp%u ",s,0);return s^1; }
static void popup(void *u,uint32_t c,uint32_t p,uint32_t id) { (void)u;CHECK(c==CTX);notef("pop%u/%u ",p,id); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);
    memset(mem+RECORD,0xa5,0x20);w32(NULL,RECORD,0x11);return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;notef("msg%x/%x ",id,(r-RECORD)*16+k); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;(void)r;notef("arg%x ",v,0); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD&&ncommit<4);memcpy(commits[ncommit++],mem+RECORD,0x20);note("commit "); }
static const struct px_battle_abilities_9_api api={&base,side_of,opposing_side,popup,action_record,message,message_arg,commit,
    INFIL,DAMP,MOLD,UNNERVE,KLUTZ};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_abilities_9_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_mold_breaker_hook,px_battle_mold_breaker_unhook};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(commits,0,sizeof commits);aborts=ncommit=0;log_[0]=0;category_value=0;
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
    w32(NULL,HANDLER+0xc,0xdead);w8(NULL,HANDLER+0x10,3);
}
struct keys { uint32_t k2,k3,k4,move; };
static void frame(struct keys k)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x02,k.k2,&events);px_battle_event_set_id(0x03,k.k3,&events);px_battle_event_set_id(0x04,k.k4,&events);
    px_battle_event_set_value(0x12,k.move,&events);px_battle_event_set_value(0x23,0,&events);px_battle_event_set_value(0x53,0,&events);
    px_battle_event_set_multiplier(0x36,0x1000,0,0x100000,&events);log_[0]=0;
}
static uint32_t get(uint32_t key) { return px_battle_event_get(key,&events); }
static uint32_t hook(void) { return r32(NULL,HANDLER+0xc); }
static void run(fn f,struct keys k) { frame(k);f(HANDLER,CTX,ME,WORK,&api);if(aborts)fprintf(stderr,"log %s\n",log_);CHECK(aborts==0); }
static uint32_t c32(unsigned n,unsigned o) { return (uint32_t)(commits[n][o]|commits[n][o+1]<<8|commits[n][o+2]<<16|(uint32_t)commits[n][o+3]<<24); }
#define EXPECT(w) do { if(strcmp(log_,w)){fprintf(stderr,"got  %s\nwant %s\n",log_,w);CHECK(0);} } while (0)
int main(void)
{
    unsigned cases=0;
    static const uint32_t ids[3]={ME,OTHER,0x100+ME};
    /* Every battler-key combination for the key-checking handlers. */
    for(unsigned a=0;a<3;++a)for(unsigned b=0;b<3;++b)for(unsigned c=0;c<3;++c)for(uint32_t w=0;w<3;++w) {
        const struct keys k={ids[a],ids[b],ids[c],0x10021};
        const int k2=k.k2==ME,k3=k.k3==ME,k4=k.k4==ME;
        reset();run(px_battle_damp_hook,k);CHECK(log_[0]==0&&hook()==DAMP);
        reset();run(px_battle_damp_unhook,k);CHECK(hook()==0);
        reset();run(px_battle_damp_unhook_entry,k);CHECK(hook()==(k2?0u:0xdeadu));
        reset();run(px_battle_infiltrator_hook,k);CHECK(hook()==(k3?(uint32_t)INFIL:0xdeadu));
        reset();run(px_battle_infiltrator_unhook,k);CHECK(hook()==(k2?0u:0xdeadu));
        reset();run(px_battle_infiltrator_pass,k);CHECK(get(0x53)==(uint32_t)k3&&hook()==0xdead);
        reset();w32(NULL,WORK,w);run(px_battle_mold_breaker_hook,k);
        CHECK(hook()==(k3&&w==0?(uint32_t)MOLD:0xdeadu)&&r32(NULL,WORK)==(k3&&w==0?1u:w));
        reset();w32(NULL,WORK,w);run(px_battle_mold_breaker_unhook,k);
        CHECK(hook()==(k2&&w==1?0u:0xdeadu)&&r32(NULL,WORK)==(k2&&w==1?0u:w));
        reset();w32(NULL,WORK,w);run(px_battle_mold_breaker_leave,k);
        CHECK(hook()==(k2&&w==1?0u:0xdeadu)&&r32(NULL,WORK)==(k2&&w==1?0u:w));
        reset();w32(NULL,WORK+0x10,w==2?7:0);run(px_battle_scrappy_self,k);CHECK(r32(NULL,WORK+0x10)==(k3&&k4?1u:(w==2?7u:0u)));
        reset();w8(NULL,HANDLER+0x11,0x77);w8(NULL,HANDLER+0x12,0x77);run(px_battle_liquid_ooze_owner,k);
        CHECK(mem[HANDLER+0x10]==(k2?6:3)&&mem[HANDLER+0x11]==0x77&&mem[HANDLER+0x12]==0x77&&hook()==0xdead);
        reset();run(px_battle_klutz_unhook,k);CHECK(hook()==(k2?0u:0xdeadu));
        reset();w32(NULL,WORK,w);run(px_battle_klutz_rehook,k);CHECK(log_[0]==0&&hook()==(w==0?(uint32_t)KLUTZ:0xdeadu)&&r32(NULL,WORK)==(w==0?1u:w));
        cases+=13;
    }
    /* Klutz and Fling: reason 0x13 for Fling (363) only. */
    for(unsigned a=0;a<3;++a)for(unsigned mv=0;mv<3;++mv)for(unsigned pre=0;pre<2;++pre) {
        static const uint32_t moves[3]={0x1016b,0x16b,0x16c};
        const struct keys k={ids[a],OTHER,OTHER,moves[mv]};
        reset();w32(NULL,WORK+8,9);frame(k);if(pre)px_battle_event_rewrite(0x23,0,&events);log_[0]=0;
        px_battle_klutz_fling(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0&&log_[0]==0);
        const int fling=k.k2==ME&&(k.move&0xffff)==0x16b;
        CHECK(r32(NULL,WORK+8)==(k.k2==ME?(fling&&!pre?1u:0u):9u)&&get(0x23)==(fling&&!pre?0x13u:0u));++cases;
    }
    for(unsigned a=0;a<3;++a)for(uint32_t w=0;w<2;++w) {
        const struct keys k={ids[a],OTHER,OTHER,0x1016b};
        reset();w32(NULL,WORK+8,w?5:0);run(px_battle_klutz_fling_message,k);
        const int say=k.k2==ME&&w;EXPECT(say?"rec3/2 msg38f/42 arg2 arg1016b commit ":"");CHECK(r32(NULL,WORK+8)==(say?0u:(w?5u:0u)));
        if(say)CHECK(c32(0,0)==0x800011);
        ++cases;
    }
    /* Klutz leaving with an item: kind 0x22. */
    for(unsigned a=0;a<3;++a)for(unsigned held=0;held<2;++held) {
        const struct keys k={ids[a],OTHER,OTHER,0x21};
        reset();w16(NULL,MON+0x200+0x12,held?0x99:0);run(px_battle_klutz_leave,k);
        EXPECT(k.k2==ME&&held?"rec22/2 commit ":"");if(ncommit)CHECK(commits[0][4]==ME&&c32(0,0)==0x11&&commits[0][5]==0xa5);++cases;
    }
    /* Unnerve: announced once naming the opposing side, then hooked. */
    for(unsigned a=0;a<3;++a)for(uint32_t w=0;w<2;++w)for(unsigned h=0;h<2;++h) {
        const struct keys k={ids[a],OTHER,OTHER,0x21};
        reset();w32(NULL,WORK,w);run(h?px_battle_unnerve_rehook:px_battle_unnerve_effect,k);
        const int on=w==0&&(h||k.k2==ME);
        EXPECT(on?"side2 opp0 pop1/2 rec3/2 msgb6/41 arg1 commit pop2/2 ":"");
        CHECK(hook()==(on?(uint32_t)UNNERVE:0xdeadu)&&r32(NULL,WORK)==(on?1u:w));if(on)CHECK(c32(0,0)==0x11);++cases;
    }
    /* Slow Start: restarted once with its message; physical moves halved while active. */
    for(uint32_t w=0;w<2;++w) {
        reset();w32(NULL,WORK,w);w32(NULL,WORK+4,7);w32(NULL,WORK+8,7);w32(NULL,WORK+0xc,7);run(px_battle_slow_start_restart,(struct keys){OTHER,OTHER,OTHER,0});
        EXPECT(w?"":"rec3/2 msg1f0/42 arg2 commit ");
        CHECK(r32(NULL,WORK)==1&&r32(NULL,WORK+4)==(w?7u:0u)&&r32(NULL,WORK+8)==(w?7u:0u)&&r32(NULL,WORK+0xc)==(w?7u:1u));
        if(!w){CHECK(c32(0,0)==0x800011);}
        ++cases;
    }
    for(unsigned b=0;b<3;++b)for(uint32_t act=0;act<2;++act)for(uint32_t c=0;c<3;++c) {
        static const uint32_t cats[3]={1,2,0x101};
        const struct keys k={OTHER,ids[b],OTHER,0x10044};
        reset();category_value=cats[c];w32(NULL,WORK+0xc,act);run(px_battle_slow_start_attack,k);
        EXPECT(k.k3==ME&&act?"cat44 ":"");CHECK(get(0x36)==(k.k3==ME&&act&&category_value==1?0x800u:0x1000u));++cases;
    }
    /* Through the dispatcher: Mold Breaker hooked on event 0x03, unhooked on 0x04. */
    {
        reset();
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7c00+0x10*i;
            w8(NULL,entries,i?0x04:0x03);w32(NULL,entries+4,i);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ME);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,HANDLER+0xc,0);w8(NULL,HANDLER+0x10,0);w32(NULL,HANDLER+0x40+0xc,0);w32(NULL,BATTLE+0x60,HANDLER);
        const struct keys k={ME,ME,OTHER,0x21};frame(k);px_battle_event_dispatch(CTX,0x03,&events);
        if(!(aborts==0&&r32(NULL,HANDLER+0xc)==MOLD))fprintf(stderr,"ab %u hook %x work %x log %s\n",aborts,r32(NULL,HANDLER+0xc),r32(NULL,HANDLER+0x1c),log_);
        CHECK(aborts==0&&r32(NULL,HANDLER+0xc)==MOLD&&r32(NULL,HANDLER+0x1c)==1);
        w32(NULL,HANDLER+0x40+0x1c,1);frame(k);px_battle_event_dispatch(CTX,0x04,&events);
        CHECK(aborts==0&&r32(NULL,HANDLER+0x40+0xc)==0&&r32(NULL,HANDLER+0x40+0x1c)==0);cases+=2;
    }
    printf("battle_abilities_9: %u cases, all tests passed\n",cases);
    return 0;
}
