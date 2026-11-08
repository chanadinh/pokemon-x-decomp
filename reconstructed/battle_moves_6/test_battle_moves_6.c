#include "battle_moves_6.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, WORK=0x5000, HANDLER=0x6000, RECORD=0x7000,
       ENTRIES=0x7900, NODES=0x8000 };
enum { ATT=2 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static unsigned aborts;static char log_[1024];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[64];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static const struct px_battle_ability_api base={&events,NULL,NULL,NULL,NULL,NULL,NULL};
static uint32_t records;
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX&&records<8);notef("rec%x/%u ",k,id);return RECORD+0x40*records++; }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX);notef("commit%u ",(r-RECORD)/0x40,0); }
static void message(void *u,uint32_t at,uint32_t k,uint32_t m) { (void)u;notef("msg%x/%x ",m,(at-RECORD)*16+k); }
static void message_arg(void *u,uint32_t at,uint32_t v) { (void)u;notef("arg%x/%x ",v,at-RECORD); }
static const struct px_battle_moves_6_api api={&base,action_record,commit,message,message_arg};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_6_api*);
static const fn dispatched[]={px_battle_dig_charge,px_battle_solar_beam_charge};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w) { (void)u;CHECK(f<2);dispatched[f](h,c,v,w,&api); }
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;log_[0]=0;records=0;
    for(unsigned i=0;i<8;++i){ memset(mem+RECORD+0x40*i,0xa5,0x40);w32(NULL,RECORD+0x40*i,0x11); }
}
static void frame(uint32_t attacker) { px_battle_event_push(&events);px_battle_event_set_id(0x03,attacker,&events);log_[0]=0; }
static int same(const char *expected) { if(strcmp(log_,expected)){ fprintf(stderr,"log [%s] expected [%s]\n",log_,expected);return 0; } return 1; }
int main(void)
{
    unsigned cases=0;
    /* Semi-invulnerable charges. */
    {
        const fn f[5]={px_battle_fly_charge,px_battle_bounce_charge,px_battle_dive_charge,px_battle_dig_charge,px_battle_shadow_force_charge};
        const uint32_t state[5]={3,3,4,5,6},msg[5]={0x211,0x220,0x217,0x21a,0x21d};
        for(unsigned k=0;k<5;++k)for(uint32_t id=1;id<4;++id) {
            reset();frame(id==3?0x102:ATT);f[k](HANDLER,CTX,id==3?ATT:id,WORK,&api);
            if(id!=ATT){ CHECK(log_[0]==0&&records==0);++cases;continue; }
            char b[128];snprintf(b,sizeof b,"rec17/2 commit0 rec3/2 msg%x/442 arg2/44 commit1 ",msg[k]);
            CHECK(same(b)&&mem[RECORD+4]==state[k]&&mem[RECORD+5]==ATT&&mem[RECORD+6]==0xa5&&mem[RECORD+3]==0&&aborts==0);++cases;
        }
    }
    /* Charge messages. */
    {
        const fn f[4]={px_battle_razor_wind_charge,px_battle_sky_attack_charge,px_battle_solar_beam_charge,px_battle_geomancy_charge};
        const uint32_t msg[4]={0x223,0x226,0x229,0x4bd};
        for(unsigned k=0;k<4;++k)for(uint32_t id=1;id<3;++id) {
            reset();frame(ATT);f[k](HANDLER,CTX,id,WORK,&api);
            if(id!=ATT){ CHECK(log_[0]==0);++cases;continue; }
            char b[96];snprintf(b,sizeof b,"rec3/2 msg%x/42 arg2/4 commit0 ",msg[k]);CHECK(same(b));++cases;
        }
        const uint32_t moves[]={553,554,552,0,0xffff};
        for(unsigned k=0;k<5;++k) {
            reset();w16(NULL,HANDLER+0x38,(uint16_t)moves[k]);w16(NULL,HANDLER+0x3a,0x229);frame(ATT);
            px_battle_freeze_shock_charge(HANDLER,CTX,ATT,WORK,&api);
            char b[96];snprintf(b,sizeof b,"rec3/2 msg%x/42 arg2/4 commit0 ",moves[k]==553?0x362:0x365);CHECK(same(b));++cases;
        }
        reset();frame(3);px_battle_freeze_shock_charge(HANDLER,CTX,ATT,WORK,&api);CHECK(log_[0]==0);++cases;
    }
    /* Skull Bash: the message, then Defense +1 keeping the other flag bits. */
    {
        const uint8_t flags[]={0,0xa5,0xf9,0xff};
        for(unsigned k=0;k<4;++k) {
            reset();mem[RECORD+0x40+0x18]=flags[k];frame(ATT);px_battle_skull_bash_charge(HANDLER,CTX,ATT,WORK,&api);
            const uint32_t s=RECORD+0x40;
            CHECK(same("rec3/2 msg22c/42 arg2/4 commit0 recd/2 commit1 "));
            CHECK(mem[s+4]==2&&mem[s+0xc]==ATT&&mem[s+0x16]==1&&mem[s+0x17]==1&&mem[s+0x18]==(uint8_t)(flags[k]|6)&&mem[s+5]==0xa5&&r32(NULL,s)==0x11);++cases;
        }
        reset();frame(0x102);px_battle_skull_bash_charge(HANDLER,CTX,ATT,WORK,&api);CHECK(log_[0]==0);++cases;
    }
    /* Release turns do nothing, whatever the frame (even an empty one). */
    {
        const fn f[5]={px_battle_fly_release,px_battle_bounce_release,px_battle_dive_release,px_battle_dig_release,px_battle_shadow_force_release};
        for(unsigned k=0;k<5;++k)for(uint32_t fr=0;fr<2;++fr) {
            reset();if(fr) frame(ATT);
            static uint8_t copy[SIZE];memcpy(copy,mem,SIZE);f[k](HANDLER,CTX,ATT,WORK,&api);
            CHECK(!memcmp(copy,mem,SIZE)&&log_[0]==0&&aborts==0);++cases;
        }
    }
    /* Through the dispatcher on event 0xa0: Dig for battler 2, Solar Beam for battler 3. */
    {
        reset();
        for(unsigned i=0;i<2;++i){ w8(NULL,ENTRIES+0x10*i,0xa0);w32(NULL,ENTRIES+0x10*i+4,i); }
        const uint8_t ids[2]={ATT,3};
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=NODES+0x40*i;
            w32(NULL,h+8,ENTRIES+0x10*i);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ids[i]);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,NODES);frame(ATT);px_battle_event_dispatch(CTX,0xa0,&events);
        CHECK(same("rec17/2 commit0 rec3/2 msg21a/442 arg2/44 commit1 ")&&mem[RECORD+4]==5&&aborts==0);++cases;
    }
    printf("battle_moves_6: %u cases, all tests passed\n",cases);
    return 0;
}
