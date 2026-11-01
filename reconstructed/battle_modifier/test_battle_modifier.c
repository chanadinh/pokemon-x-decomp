#include "battle_modifier.h"
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
static uint32_t params[0x300],no_consume_value,full[8],streak[8],last[8],commits,records;static unsigned aborts;static char log_[512];
static uint32_t ally_pairs;  /* bit (id*8+other) */
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static const struct px_battle_ability_api base={&events,NULL,NULL,NULL,NULL,NULL,NULL};
static uint32_t item_param(void *u,uint32_t item,uint32_t p) { (void)u;CHECK(item<0x300&&p==3);note("ip ");return params[item]; }
static uint32_t no_consume(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("nc ");return no_consume_value; }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX&&k==0x15&&id<8);note("rec ");++records;return RECORD; }
static void commit(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("commit ");++commits; }
static uint32_t full_hp(void *u,uint32_t p) { (void)u;return full[mon(p)]; }
static uint32_t is_ally(void *u,uint32_t id,uint32_t other) { (void)u;CHECK(id<8&&other<8);note("ally ");return ally_pairs>>(id*8+other)&1; }
static uint32_t excluded_move;static uint32_t move_excluded(void *u,uint32_t m) { (void)u;note("ex ");return m==excluded_move; }
static uint32_t use_streak(void *u,uint32_t p) { (void)u;note("st ");return streak[mon(p)]; }
static uint32_t last_move(void *u,uint32_t p) { (void)u;note("lm ");return last[mon(p)]; }
static const struct px_battle_modifier_api api={&base,item_param,no_consume,action_record,commit,full_hp,is_ally,move_excluded,use_streak,last_move};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static void (*const table[])(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_modifier_api*)={
        px_battle_occa_berry,px_battle_expert_belt,px_battle_solid_rock,px_battle_sniper};
    CHECK(f<4);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(params,0,sizeof params);memset(full,0,sizeof full);memset(streak,0,sizeof streak);memset(last,0,sizeof last);
    no_consume_value=0;commits=records=0;aborts=0;log_[0]=0;ally_pairs=0;excluded_move=0xffff;w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
/* The damage frame before event 0x4f. */
static void frame(uint32_t attacker,uint32_t defender,uint32_t move,uint32_t type,uint32_t code,uint32_t critical,uint32_t suppress)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x03,attacker,&events);px_battle_event_set_id(0x04,defender,&events);
    px_battle_event_set_id(0x12,move,&events);px_battle_event_set_id(0x16,type,&events);
    px_battle_event_set_value(0x39,code,&events);px_battle_event_set_value(0x46,critical,&events);px_battle_event_set_value(0x47,suppress,&events);
    px_battle_event_set_multiplier(0x36,0x1000,0,0,&events);log_[0]=0;
}
static uint32_t mult(void) { return px_battle_event_get(0x36,&events); }
static uint32_t ref(uint32_t f) { return (uint32_t)(((uint64_t)0x1000*f+0x800)>>12); }
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_modifier_api*);
int main(void)
{
    unsigned cases=0;
    /* Effectiveness classes and state bits. */
    for(uint32_t c=0;c<0x120;++c) { const uint32_t want=c==0?0:c==7?1:c>7?2:3;CHECK(px_battle_effectiveness_class(c)==want);++cases; }
    CHECK(px_battle_effectiveness_class(0xffffffff)==2);++cases;
    for(uint32_t flag=0;flag<40;++flag)for(uint32_t bit=0;bit<40;++bit) {
        reset();mem[MON+0x169+bit/8]|=(uint8_t)(1u<<(bit%8));CHECK(px_battle_pokemon_state(MON,flag,&memory)==(flag==bit));++cases;
    }
    /* Item factor: (param + 100) x 4096 / 100, against integer arithmetic where single precision is exact. */
    for(int32_t p=-4000;p<3990;++p) {
        reset();params[268]=(uint32_t)p;w16(NULL,HANDLER+0x38,268);
        const int32_t n=(p+100)*4096;CHECK(px_battle_item_factor(HANDLER,&api)==(uint32_t)(n/100));++cases;
    }
    reset();params[268]=20;w16(NULL,HANDLER+0x38,268);CHECK(px_battle_item_factor(HANDLER,&api)==0x1333);++cases;
    reset();params[270]=30;w16(NULL,HANDLER+0x38,270);CHECK(px_battle_item_factor(HANDLER,&api)==0x14cc);++cases;
    /* Resist berries: type, defender, suppression, super effectiveness (Chilan: any), use once, consumption. */
    {
        const fn berries[18]={px_battle_occa_berry,px_battle_passho_berry,px_battle_wacan_berry,px_battle_rindo_berry,px_battle_yache_berry,
            px_battle_chople_berry,px_battle_kebia_berry,px_battle_shuca_berry,px_battle_coba_berry,px_battle_payapa_berry,px_battle_tanga_berry,
            px_battle_charti_berry,px_battle_kasib_berry,px_battle_haban_berry,px_battle_colbur_berry,px_battle_babiri_berry,px_battle_chilan_berry,
            px_battle_roseli_berry};
        const uint32_t types[18]={9,10,12,11,14,1,3,4,2,13,6,5,7,15,16,8,0,17};
        for(unsigned k=0;k<18;++k)for(uint32_t t=0;t<0x13;++t)for(uint32_t code=5;code<10;++code)for(unsigned var=0;var<5;++var) {
            reset();no_consume_value=var==4;w32(NULL,WORK,var==3);
            frame(1,var==1?3:2,5,t,code,0,var==2);berries[k](HANDLER,CTX,2,WORK,&api);
            const int on=var!=1&&var!=2&&var!=3&&t==types[k]&&(k==16||code>7);
            CHECK(mult()==(on?ref(0x800):0x1000u));
            CHECK(r32(NULL,WORK)==(var==3?1u:on&&var!=4?1u:0u)&&commits==(on&&var!=4)&&records==commits);
            if(on&&var!=4)CHECK(mem[RECORD+5]==2&&mem[RECORD+4]==0x11&&!strcmp(log_,"nc rec commit "));
            ++cases;
        }
        reset();frame(1,2,5,9,0x80000000u,0,0);px_battle_occa_berry(HANDLER,CTX,2,WORK,&api);CHECK(mult()==0x1000);++cases; /* signed */
        reset();frame(1,2,5,0x109,9,0,0);px_battle_occa_berry(HANDLER,CTX,2,WORK,&api);CHECK(mult()==0x1000);++cases;  /* full type word */
        reset();frame(1,2,5,9,9,0,0);CHECK(px_battle_resist_berry(CTX,2,WORK,9,0,&api)==1&&px_battle_resist_berry(CTX,2,WORK,9,0,&api)==0);++cases;
    }
    /* Expert Belt (super effective) and Life Orb (always): the item factor on the attacker. */
    for(uint32_t code=0;code<14;++code)for(uint32_t self=0;self<2;++self) {
        reset();params[268]=20;params[270]=30;w16(NULL,HANDLER+0x38,268);frame(self?2:3,1,5,9,code,0,0);
        px_battle_expert_belt(HANDLER,CTX,2,WORK,&api);CHECK(mult()==(self&&code>7?ref(0x1333):0x1000u));
        reset();params[270]=30;w16(NULL,HANDLER+0x38,270);frame(self?2:3,1,5,9,code,0,0);
        px_battle_life_orb(HANDLER,CTX,2,WORK,&api);CHECK(mult()==(self?ref(0x14cc):0x1000u));++cases;
    }
    reset();params[268]=20;w16(NULL,HANDLER+0x38,268);frame(2,1,5,9,0xfffffff0u,0,0);px_battle_expert_belt(HANDLER,CTX,2,WORK,&api);CHECK(mult()==0x1000);++cases;
    /* Metronome item: consecutive uses of the same move, +param% each, at most 200%. */
    for(uint32_t s=0;s<8;++s)for(uint32_t p=0;p<60;p+=7)for(unsigned same=0;same<2;++same)for(unsigned ex=0;ex<2;++ex) {
        reset();params[277]=p;w16(NULL,HANDLER+0x38,277);streak[2]=s|0x10000;last[2]=same?55:56;if(ex)excluded_move=55;
        frame(2,1,55,9,7,0,0);px_battle_metronome_item(HANDLER,CTX,2,WORK,&api);
        uint32_t pct=p*s+100;if(pct>200)pct=200;
        const int on=!ex&&s&&same;CHECK(mult()==(on?ref((pct*4096)/100):0x1000u));
        if(ex)CHECK(!strcmp(log_,"ex "));
        else if(!s)CHECK(!strcmp(log_,"ex st "));
        else CHECK(!strcmp(log_,same?"ex st lm ip ":"ex st lm "));
        ++cases;
    }
    reset();params[277]=0xffff;w16(NULL,HANDLER+0x38,277);streak[2]=1;last[2]=55;frame(2,1,55,9,7,0,0);
    px_battle_metronome_item(HANDLER,CTX,2,WORK,&api);   /* 0xffff + 100 wraps to 99% */
    CHECK(mult()==ref((99u*4096)/100));++cases;
    reset();params[277]=0xffff;w16(NULL,HANDLER+0x38,277);streak[2]=0x64;last[2]=55;frame(2,1,55,9,7,0,0);
    px_battle_metronome_item(HANDLER,CTX,2,WORK,&api);   /* 0xffff * 100 + 100 = 0x640000: 0% */
    CHECK(mult()==0);++cases;
    /* Abilities. */
    for(uint32_t code=0;code<14;++code)for(uint32_t self=0;self<2;++self)for(uint32_t crit=0;crit<2;++crit) {
        reset();frame(self?2:3,1,5,9,code,crit,0);px_battle_sniper(HANDLER,CTX,2,WORK,&api);CHECK(mult()==(self&&crit?ref(0x1800):0x1000u));
        reset();frame(1,self?2:3,5,9,code|0x100,crit,0);px_battle_solid_rock(HANDLER,CTX,2,WORK,&api);CHECK(mult()==(self&&code>7?ref(0xc00):0x1000u));
        reset();frame(self?2:3,1,5,9,code|0x100,crit,0);px_battle_tinted_lens(HANDLER,CTX,2,WORK,&api);
        CHECK(mult()==(self&&code&&code<7?ref(0x2000):0x1000u));
        reset();full[2]=crit;frame(1,self?2:3,5,9,code,0,0);px_battle_multiscale(HANDLER,CTX,2,WORK,&api);CHECK(mult()==(self&&crit?ref(0x800):0x1000u));
        ++cases;
    }
    for(uint32_t d=0;d<6;++d)for(uint32_t ally=0;ally<2;++ally) {
        reset();if(ally)ally_pairs=1u<<(2*8+d);frame(1,d|0x300,5,9,7,0,0);px_battle_friend_guard(HANDLER,CTX,2,WORK,&api);
        CHECK(mult()==(d!=2&&ally?ref(0xc00):0x1000u)&&!strcmp(log_,d==2?"":"ally "));++cases;
    }
    /* Moves: doubled against the defender's state bit. */
    {
        const fn moves[4]={px_battle_hits_underwater_surf,px_battle_hits_underwater_whirlpool,px_battle_hits_underground,px_battle_hits_minimized};
        const uint32_t flags[4]={4,4,5,8};
        for(unsigned k=0;k<4;++k)for(uint32_t bit=0;bit<16;++bit)for(uint32_t self=0;self<2;++self) {
            reset();mem[MON+0x300+0x169+bit/8]|=(uint8_t)(1u<<(bit%8));frame(self?2:1,0x103,5,9,7,0,0);moves[k](HANDLER,CTX,2,WORK,&api);
            CHECK(mult()==(self&&bit==flags[k]?ref(0x2000):0x1000u));++cases;
        }
    }
    /* Through the dispatcher: Occa Berry (defender), Expert Belt (attacker), Solid Rock (defender), Sniper (attacker). */
    {
        reset();params[268]=20;
        const uint32_t owners[4]={3,2,3,2};
        for(unsigned i=0;i<4;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7800+0x10*i;
            w8(NULL,entries,0x4f);w32(NULL,entries+4,i);w16(NULL,h+0x38,i==1?268:0);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,(uint8_t)owners[i]);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<3?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);
        frame(2,3,5,9,8,1,0);px_battle_event_dispatch(CTX,0x4f,&events);
        const uint32_t a=ref(0x800),b=(uint32_t)(((uint64_t)a*0x1333+0x800)>>12),c=(uint32_t)(((uint64_t)b*0xc00+0x800)>>12),d=(uint32_t)(((uint64_t)c*0x1800+0x800)>>12);
        CHECK(mult()==d&&commits==1&&r32(NULL,HANDLER+0x1c)==1&&aborts==0);++cases;
    }
    printf("battle_modifier: %u cases, all tests passed\n",cases);
    return 0;
}
