#include "battle_items_4.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, HANDLER=0x6000, RECORD=0x7000, ENTRIES=0x7a00 };
enum { REGISTERED=0x20000000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
/* Answers: no-consume pass, damaging move, fixed-target move. */
static uint32_t nocons,damaging,fixed;static unsigned aborts,records;static char log_[512];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a) { char t[48];snprintf(t,sizeof t,f,a);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static const struct px_battle_ability_api base={.events=&events};
static uint32_t no_consume(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("nc ");return nocons; }
static uint32_t damaging_move(void *u,uint32_t m) { (void)u;notef("dm%x ",m);return damaging; }
static uint32_t fixed_target(void *u,uint32_t m) { (void)u;notef("ft%x ",m);return fixed; }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id)
{ (void)u;CHECK(c==CTX&&records<1);char t[32];snprintf(t,sizeof t,"rec%x/%x ",k,id);note(t);++records;return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;CHECK(r==RECORD+8);char t[32];snprintf(t,sizeof t,"msg%x/%x ",id,k);note(t); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;CHECK(r==RECORD+8);notef("arg%x ",v); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD);note("commit "); }
static const struct px_battle_items_4_api api={&base,no_consume,damaging_move,fixed_target,action_record,message,message_arg,commit};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_items_4_api*);
static const fn power[]={px_battle_fire_gem_power,px_battle_water_gem_power,px_battle_electric_gem_power,px_battle_grass_gem_power,
    px_battle_ice_gem_power,px_battle_fighting_gem_power,px_battle_poison_gem_power,px_battle_ground_gem_power,px_battle_flying_gem_power,
    px_battle_psychic_gem_power,px_battle_bug_gem_power,px_battle_rock_gem_power,px_battle_ghost_gem_power,px_battle_dragon_gem_power,
    px_battle_dark_gem_power,px_battle_steel_gem_power,px_battle_normal_gem_power,px_battle_fairy_gem_power};
static const fn use[]={px_battle_fire_gem_use,px_battle_water_gem_use,px_battle_electric_gem_use,px_battle_grass_gem_use,
    px_battle_ice_gem_use,px_battle_fighting_gem_use,px_battle_poison_gem_use,px_battle_ground_gem_use,px_battle_flying_gem_use,
    px_battle_psychic_gem_use,px_battle_bug_gem_use,px_battle_rock_gem_use,px_battle_ghost_gem_use,px_battle_dragon_gem_use,
    px_battle_dark_gem_use,px_battle_steel_gem_use,px_battle_normal_gem_use,px_battle_fairy_gem_use};
/* Items 548-564 and 715 in that order, and their types. */
static const uint32_t gem_type[]={9,0xa,0xc,0xb,0xe,1,3,4,2,0xd,6,5,7,0xf,0x10,8,0,0x11};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_fire_gem_use,px_battle_fire_gem_power,px_battle_gem_spent};
    CHECK(f<3);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);nocons=0;damaging=1;fixed=0;aborts=0;records=0;log_[0]=0;
    w16(NULL,HANDLER+0x38,0x224);
}
static void frame(uint32_t attacker,uint32_t move,uint32_t type,uint32_t power_value)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x03,attacker,&events);
    px_battle_event_set_value(0x12,move,&events);px_battle_event_set_value(0x16,type,&events);
    px_battle_event_set_multiplier(0x32,power_value,0,0,&events);log_[0]=0;
}
static uint32_t power_now(void) { return px_battle_event_get(0x32,&events); }
static uint32_t times(uint32_t v,uint32_t f) { return (uint32_t)((uint64_t)((int64_t)(int32_t)v*(int32_t)f+0x800)>>12); }
static unsigned cases;
static void expect(const char *s) { if(strcmp(log_,s)){fprintf(stderr,"got  [%s]\nwant [%s]\n",log_,s);} CHECK(!strcmp(log_,s));++cases; }
int main(void)
{
    /* 1084c700 and 10881638 over every value of byte +0x10, with the bytes around it. */
    {
        for(uint32_t v=0;v<256;++v){
            reset();w8(NULL,HANDLER+0x10,(uint8_t)v);w8(NULL,HANDLER+0xf,0x55);w8(NULL,HANDLER+0x11,0x66);
            CHECK(px_battle_item_consumed(HANDLER,&memory)==(v==6));++cases;
            px_battle_item_mark_consumed(HANDLER,&memory);
            CHECK(r8(NULL,HANDLER+0x10)==6&&r8(NULL,HANDLER+0xf)==0x55&&r8(NULL,HANDLER+0x11)==0x66);
            CHECK(px_battle_item_consumed(HANDLER,&memory)==1);++cases;
        }
    }
    /* 1084c284 over the attacker, the type word, every answer, the consumed byte and two moves. */
    {
        const uint32_t moves[]={0x21,0x10021};const uint32_t types[]={9,8,0x109,0xa};
        for(uint32_t who=2;who<4;++who)for(unsigned t=0;t<4;++t)for(uint32_t dm=0;dm<2;++dm)for(uint32_t fx=0;fx<2;++fx)
        for(uint32_t used=0;used<2;++used)for(uint32_t nc=0;nc<2;++nc)for(unsigned mv=0;mv<2;++mv){
            reset();damaging=dm?0x80:0;fixed=fx;nocons=nc;if(used)w8(NULL,HANDLER+0x10,6);w32(NULL,HANDLER+0x1c,7);
            frame(who,moves[mv],types[t],0x1000);px_battle_gem_use(HANDLER,CTX,2,HANDLER+0x1c,9,&api);
            char b[160]="";const uint32_t m=moves[mv]&0xffff;
            int ok=who==2&&types[t]==9;
            if(ok){ char c[32];snprintf(c,sizeof c,"dm%x ",m);strcat(b,c);ok=dm!=0; }
            if(ok){ char c[32];snprintf(c,sizeof c,"ft%x ",m);strcat(b,c);ok=!fx; }
            if(ok)ok=!used;
            if(ok){ strcat(b,"nc ");ok=!nc; }
            if(ok){ char c[96];snprintf(c,sizeof c,"rec24/2 msgbc/1 arg224 arg%x commit ",moves[mv]);strcat(b,c); }
            expect(b);
            CHECK(r32(NULL,HANDLER+0x1c)==(ok?1u:7u)&&r8(NULL,HANDLER+0x10)==(ok||used?6:0)&&power_now()==0x1000&&aborts==0);
        }
        /* Another holder and gem: the work word is set even when it was 2. */
        reset();w16(NULL,HANDLER+0x38,0x2cb);w32(NULL,HANDLER+0x1c,2);frame(5,0x55,0x11,0x1000);
        px_battle_gem_use(HANDLER,CTX,5,HANDLER+0x1c,0x11,&api);
        expect("dm55 ft55 nc rec24/5 msgbc/1 arg2cb arg55 commit ");CHECK(r32(NULL,HANDLER+0x1c)==1);
    }
    /* Event 0x3d for every gem: type, attacker, work word and the no-consume pass. */
    {
        const uint32_t works[]={0,1,2,0x101};
        for(unsigned g=0;g<18;++g)for(uint32_t t=0;t<0x13;++t)for(uint32_t who=2;who<4;++who)for(unsigned w=0;w<4;++w)for(uint32_t nc=0;nc<2;++nc){
            reset();nocons=nc;w32(NULL,HANDLER+0x1c,works[w]);frame(who,0x21,t,0x1000);
            power[g](HANDLER,CTX,2,HANDLER+0x1c,&api);
            const int reach=who==2&&t==gem_type[g];
            const int hit=reach&&(works[w]==1||nc);
            expect(reach&&works[w]!=1?"nc ":"");
            CHECK(power_now()==(hit?0x14cdu:0x1000u)&&r32(NULL,HANDLER+0x1c)==works[w]&&aborts==0);
        }
        /* The whole type word, and the multiply's rounding. */
        reset();w32(NULL,HANDLER+0x1c,1);frame(2,0x21,0x109,0x1000);px_battle_fire_gem_power(HANDLER,CTX,2,HANDLER+0x1c,&api);
        expect("");CHECK(power_now()==0x1000);
        reset();w32(NULL,HANDLER+0x1c,1);frame(2,0x21,9,0x1555);px_battle_fire_gem_power(HANDLER,CTX,2,HANDLER+0x1c,&api);
        CHECK(power_now()==times(0x1555,0x14cd));++cases;
    }
    /* Event 0x8a for every gem: its type reaches 1084c284. */
    {
        for(unsigned g=0;g<18;++g)for(uint32_t t=0;t<0x13;++t){
            reset();frame(2,0x21,t,0x1000);use[g](HANDLER,CTX,2,HANDLER+0x1c,&api);
            expect(t==gem_type[g]?"dm21 ft21 nc rec24/2 msgbc/1 arg224 arg21 commit ":"");
            CHECK(r32(NULL,HANDLER+0x1c)==(t==gem_type[g]));
        }
    }
    /* Event 0x91: only 1 becomes 2, only for the attacker. */
    {
        const uint32_t works[]={0,1,2,3,0x101,0xffffffff};
        for(unsigned w=0;w<6;++w)for(uint32_t who=0;who<5;++who){
            reset();w32(NULL,HANDLER+0x1c,works[w]);frame(who,0,0,0x1000);px_battle_gem_spent(HANDLER,CTX,2,HANDLER+0x1c,&api);
            expect("");CHECK(r32(NULL,HANDLER+0x1c)==(who==2&&works[w]==1?2u:works[w]));
        }
    }
    /* Through the dispatcher: use on 0x8a, power on 0x3d, spent on 0x91, then no power again. */
    {
        reset();
        /* One gem handler with its three entries (0x8a use, 0x3d power, 0x91 spent) sharing one work area. */
        for(uint32_t i=0;i<3;++i){ w8(NULL,ENTRIES+8*i,(uint8_t)(i==0?0x8a:i==1?0x3d:0x91));w32(NULL,ENTRIES+8*i+4,i); }
        w32(NULL,HANDLER+8,ENTRIES);w32(NULL,HANDLER+0x18,REGISTERED|3u<<16);w8(NULL,HANDLER+0x3a,2);w32(NULL,BATTLE+0x60,HANDLER);
        frame(2,0x34,9,0x1000);px_battle_event_dispatch(CTX,0x8a,&events);
        expect("dm34 ft34 nc rec24/2 msgbc/1 arg224 arg34 commit ");CHECK(r32(NULL,HANDLER+0x1c)==1&&r8(NULL,HANDLER+0x10)==6);
        /* The dispatcher runs the first matching entry: give each event its own entry list. */
        w32(NULL,HANDLER+8,ENTRIES+8);w32(NULL,HANDLER+0x18,REGISTERED|1u<<16);log_[0]=0;
        px_battle_event_dispatch(CTX,0x3d,&events);expect("");CHECK(power_now()==0x14cd);
        w32(NULL,HANDLER+8,ENTRIES+16);log_[0]=0;px_battle_event_dispatch(CTX,0x91,&events);CHECK(r32(NULL,HANDLER+0x1c)==2);
        px_battle_event_pop(&events);frame(2,0x34,9,0x1000);
        w32(NULL,HANDLER+8,ENTRIES+8);log_[0]=0;px_battle_event_dispatch(CTX,0x3d,&events);expect("nc ");CHECK(power_now()==0x1000);
        w32(NULL,HANDLER+8,ENTRIES);log_[0]=0;px_battle_event_dispatch(CTX,0x8a,&events);expect("dm34 ft34 ");CHECK(records==1&&aborts==0);
    }
    printf("battle_items_4: %u cases, all tests passed\n",cases);
    return 0;
}
