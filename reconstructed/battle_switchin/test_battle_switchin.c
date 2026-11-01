#include "battle_switchin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, BUFFER=0x7800, UNNERVE=0x108c4e70, KLUTZ=0x108c3750 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static uint32_t usable,weather_value,truant,opp_count;static unsigned aborts;static char log_[512];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static const struct px_battle_ability_api base={&events,NULL,NULL,NULL,NULL,NULL,NULL};
static void cure(void *u,uint32_t c,uint32_t id,uint32_t s) { (void)u;CHECK(c==CTX);notef("cure%u/%x ",id,s); }
static uint32_t item_usable(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("iu%u ",id,0);return usable; }
static uint32_t weather(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("w ");return weather_value; }
static uint32_t opposing(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("opp%u ",id,0);return 0x30; }
static uint32_t temp_buffer(void *u,uint32_t c,uint32_t n) { (void)u;CHECK(c==CTX&&n==0xa);note("buf ");return BUFFER; }
static uint32_t side_battlers(void *u,uint32_t c,uint32_t sel,uint32_t out) { (void)u;CHECK(c==CTX&&sel==0x130&&out==BUFFER);
    for(uint32_t i=0;i<opp_count;++i) { w8(NULL,out+i,(uint8_t)(4+i)); }
    note("list ");return opp_count; }
static uint32_t side_of(void *u,uint32_t id) { (void)u;return id&1; }
static uint32_t opposing_side(void *u,uint32_t s) { (void)u;return s^1; }
static uint32_t truant_check(void *u,uint32_t p,uint32_t f) { (void)u;CHECK(p==MON+0x200&&f==0);return truant; }
static void flower_gift(void *u,uint32_t h,uint32_t c,uint32_t id,uint32_t sunny,uint32_t f) { (void)u;CHECK(h==HANDLER&&c==CTX&&f==1);notef("gift%u/%u ",id,sunny); }
static void popup(void *u,uint32_t c,uint32_t p,uint32_t id) { (void)u;CHECK(c==CTX);notef("pop%u/%u ",p,id); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;notef("msg%x/%x ",id,(r-RECORD)*16+k); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;(void)r;notef("arg%x ",v,0); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD);note("commit "); }
static const struct px_battle_switchin_api api={&base,cure,item_usable,weather,opposing,temp_buffer,side_battlers,side_of,opposing_side,
    truant_check,flower_gift,popup,action_record,message,message_arg,commit,UNNERVE,KLUTZ};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static void (*const table[])(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_switchin_api*)={px_battle_drought,px_battle_intimidate};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);usable=1;weather_value=0;truant=0;opp_count=2;aborts=0;log_[0]=0;w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
static void frame(uint32_t entering) { px_battle_event_push(&events);px_battle_event_set_id(0x02,entering,&events);log_[0]=0; }
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_switchin_api*);
int main(void)
{
    unsigned cases=0;
    /* Nothing happens for another battler (Unnerve excepted: it does not test key 0x02). */
    {
        const fn all[]={px_battle_drought,px_battle_drizzle,px_battle_sand_stream,px_battle_snow_warning,px_battle_limber_entry,px_battle_insomnia_entry,
            px_battle_immunity_entry,px_battle_own_tempo_entry,px_battle_water_veil_entry,px_battle_magma_armor_entry,px_battle_oblivious_entry,
            px_battle_intimidate,px_battle_forecast,px_battle_flower_gift,px_battle_slow_start,px_battle_truant,px_battle_mold_breaker,
            px_battle_pressure,px_battle_dark_aura_entry,px_battle_fairy_aura_entry,px_battle_aura_break,px_battle_klutz};
        for(unsigned k=0;k<sizeof all/sizeof *all;++k) {
            reset();truant=1;w16(NULL,MON+0x20c,0x15f);frame(3);all[k](HANDLER,CTX,2,WORK,&api);
            CHECK(log_[0]==0&&r32(NULL,WORK)==0&&r32(NULL,HANDLER+0xc)==0);++cases;
        }
    }
    /* Weather: the kind, 5 turns or 8 with the matching rock that may be used. */
    {
        const fn f[4]={px_battle_drought,px_battle_drizzle,px_battle_sand_stream,px_battle_snow_warning};
        const uint32_t kind[4]={1,2,4,3},rock[4]={284,285,283,282};
        for(unsigned k=0;k<4;++k)for(uint32_t held=280;held<288;++held)for(uint32_t u=0;u<2;++u) {
            reset();usable=u;w16(NULL,MON+0x212,(uint16_t)held);w32(NULL,RECORD,0x11);frame(2);f[k](HANDLER,CTX,2,WORK,&api);
            const uint32_t turns=held==rock[k]&&u?8:5;
            CHECK(mem[RECORD+4]==kind[k]&&mem[RECORD+5]==turns&&r32(NULL,RECORD)==0x800011);
            CHECK(!strcmp(log_,held==rock[k]?"iu2 rec1e/2 commit ":"rec1e/2 commit "));++cases;
        }
        reset();frame(2);px_battle_set_entry_weather(CTX,2,1,0,&api);CHECK(mem[RECORD+5]==5&&!strcmp(log_,"rec1e/2 commit "));++cases;  /* no rock */
    }
    /* Cures on entry. */
    {
        const fn f[6]={px_battle_limber_entry,px_battle_insomnia_entry,px_battle_immunity_entry,px_battle_own_tempo_entry,px_battle_water_veil_entry,px_battle_magma_armor_entry};
        const uint32_t st[6]={1,2,5,6,4,3};
        for(unsigned k=0;k<6;++k){ reset();frame(2);f[k](HANDLER,CTX,2,WORK,&api);char b[16];snprintf(b,sizeof b,"cure2/%x ",st[k]);CHECK(!strcmp(log_,b));++cases; }
        reset();frame(2);px_battle_oblivious_entry(HANDLER,CTX,2,WORK,&api);CHECK(!strcmp(log_,"cure2/7 cure2/b "));++cases;
    }
    /* Intimidate: every opposing battler listed, -1 Attack in one record. */
    for(uint32_t n=0;n<4;++n) {
        reset();opp_count=n;mem[RECORD+0x18]=0x40;frame(2);px_battle_intimidate(HANDLER,CTX,2,WORK,&api);
        if(!n) { CHECK(!strcmp(log_,"opp2 buf list "));++cases;continue; }
        CHECK(!strcmp(log_,"opp2 buf list pop1/2 recd/2 commit pop2/2 "));
        CHECK(mem[RECORD+4]==1&&mem[RECORD+0x16]==n&&mem[RECORD+0x17]==0xff&&mem[RECORD+0x18]==0x42);
        for(uint32_t i=0;i<n;++i)CHECK(mem[RECORD+0xc+i]==4+i);
        CHECK(mem[RECORD+0xc+n]==0);++cases;
    }
    /* Unnerve: once, naming the opposing side; hook stored. */
    for(uint32_t id=1;id<3;++id)for(uint32_t done=0;done<2;++done) {
        reset();w32(NULL,WORK,done);frame(7);px_battle_unnerve(HANDLER,CTX,id,WORK,&api);
        if(done) CHECK(log_[0]==0&&r32(NULL,HANDLER+0xc)==0);
        else { char b[96];snprintf(b,sizeof b,"pop1/%u rec3/%u msgb6/41 arg%x commit pop2/%u ",id,id,(id&1)^1,id);CHECK(!strcmp(log_,b)&&r32(NULL,HANDLER+0xc)==UNNERVE&&r32(NULL,WORK)==1); }
        ++cases;
    }
    /* Klutz: hook once. */
    for(uint32_t done=0;done<2;++done) { reset();w32(NULL,WORK,done);frame(2);px_battle_klutz(HANDLER,CTX,2,WORK,&api);CHECK(r32(NULL,HANDLER+0xc)==(done?0u:(uint32_t)KLUTZ)&&r32(NULL,WORK)==1&&log_[0]==0);++cases; }
    /* Forecast: Castform's form follows sun, rain and hail. */
    for(uint32_t sp=0;sp<2;++sp)for(uint32_t w=0;w<6;++w)for(uint32_t form=0;form<4;++form) {
        reset();weather_value=w;w16(NULL,MON+0x20c,sp?0x15f:0x160);mem[MON+0x200+0x153]=(uint8_t)form;w32(NULL,RECORD,0x11);
        frame(2);px_battle_forecast(HANDLER,CTX,2,WORK,&api);
        const uint32_t wanted=w>=1&&w<=3?w:0;const int change=sp&&wanted!=form;
        CHECK(r32(NULL,WORK)==1&&(strstr(log_,"rec3a")!=NULL)==change);
        if(change)CHECK(!strcmp(log_,"w rec3a/2 msgde/82 arg2 commit ")&&mem[RECORD+5]==wanted&&mem[RECORD+4]==2&&r32(NULL,RECORD)==0x800011);
        ++cases;
    }
    /* Flower Gift: Cherrim only, with sun passed on. */
    for(uint32_t sp=0;sp<2;++sp)for(uint32_t w=0;w<3;++w) {
        reset();weather_value=w;w16(NULL,MON+0x20c,sp?0x1a5:0x1a6);frame(2);px_battle_flower_gift(HANDLER,CTX,2,WORK,&api);
        char b[32];snprintf(b,sizeof b,"w gift2/%u ",w==1);CHECK(!strcmp(log_,sp?b:"")&&r32(NULL,WORK)==sp);++cases;
    }
    /* Slow Start: four work words and the message. */
    reset();for(unsigned i=0;i<4;++i)w32(NULL,WORK+4*i,9);frame(2);px_battle_slow_start(HANDLER,CTX,2,WORK,&api);
    CHECK(r32(NULL,WORK)==1&&r32(NULL,WORK+4)==0&&r32(NULL,WORK+8)==0&&r32(NULL,WORK+12)==1&&r32(NULL,RECORD)==0x800000&&!strcmp(log_,"rec3/2 msg1f0/42 arg2 commit "));++cases;
    /* Truant: the work word only when the check reports. */
    for(uint32_t t=0;t<3;++t){ reset();truant=t;w32(NULL,WORK,7);frame(2);px_battle_truant(HANDLER,CTX,2,WORK,&api);CHECK(r32(NULL,WORK)==(t?1u:7u));++cases; }
    /* Announcements. */
    {
        const uint32_t abil[4]={0xa3,0xa4,0x68,0},msg[4]={0x1f9,0x1f6,0x1ba,0x1ba};
        for(unsigned k=0;k<4;++k){ reset();w16(NULL,HANDLER+0x38,(uint16_t)abil[k]);frame(2);px_battle_mold_breaker(HANDLER,CTX,2,WORK,&api);
            char b[96];snprintf(b,sizeof b,"pop1/2 rec3/2 msg%x/42 arg2 commit pop2/2 ",msg[k]);CHECK(!strcmp(log_,b));++cases; }
        const fn f[4]={px_battle_pressure,px_battle_dark_aura_entry,px_battle_fairy_aura_entry,px_battle_aura_break};const uint32_t m[4]={0x1e7,0x4b1,0x4b4,0x4cf};
        for(unsigned k=0;k<4;++k){ reset();frame(2);f[k](HANDLER,CTX,2,WORK,&api);char b[96];snprintf(b,sizeof b,"pop1/2 rec3/2 msg%x/42 arg2 commit pop2/2 ",m[k]);CHECK(!strcmp(log_,b));++cases; }
    }
    /* Through the dispatcher: Drought then Intimidate on event 0x94. */
    {
        reset();
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7c00+0x10*i;
            w8(NULL,entries,0x94);w32(NULL,entries+4,i);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,2);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);frame(2);px_battle_event_dispatch(CTX,0x94,&events);
        CHECK(!strcmp(log_,"rec1e/2 commit opp2 buf list pop1/2 recd/2 commit pop2/2 ")&&aborts==0);++cases;
    }
    printf("battle_switchin: %u cases, all tests passed\n",cases);
    return 0;
}
