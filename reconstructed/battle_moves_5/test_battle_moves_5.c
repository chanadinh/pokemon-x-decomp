#include "battle_moves_5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, ENTRIES=0x7900, NODES=0x8000 };
enum { ATT=2, DEF=5 };
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
static void notef(const char *f,uint32_t a,uint32_t b) { char t[64];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static uint32_t listed;
static uint32_t side_battlers(void *u,uint32_t c,uint32_t sel,uint32_t out) { (void)u;CHECK(c==CTX);notef("sb%x/%x ",sel,out-RECORD);
    for(uint32_t i=0;i<listed;++i) { w8(NULL,out+i,(uint8_t)(10+i)); }
    return listed; }
static const struct px_battle_ability_api base={&events,NULL,NULL,NULL,NULL,side_battlers,NULL};
/* Records are handed out at RECORD + 0x40 n, pre-filled with 0xa5 (word 0 = 0x11). */
static uint32_t records,active_mask,terrain_value,counter_value;
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX&&records<8);notef("rec%x/%u ",k,id);return RECORD+0x40*records++; }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX);notef("commit%u ",(r-RECORD)/0x40,0); }
static void message(void *u,uint32_t at,uint32_t k,uint32_t m) { (void)u;notef("msg%x/%x ",m,(at-RECORD)*16+k); }
static void message_arg(void *u,uint32_t at,uint32_t v) { (void)u;notef("arg%x/%x ",v,at-RECORD); }
static uint32_t field_active(void *u,uint32_t c,uint32_t k) { (void)u;CHECK(c==CTX);notef("fa%u ",k,0);return k<32&&(active_mask>>k&1); }
static uint32_t terrain(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("ter ");return terrain_value; }
static void effect(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("eff%x/%u ",k,id); }
static uint32_t opposing_selector(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("opp%u ",id,0);return 0x31; }
static uint32_t side_of(void *u,uint32_t id) { (void)u;notef("side%u ",id,0);return id&1; }
static uint32_t opposing_side_of(void *u,uint32_t id) { (void)u;notef("oside%u ",id,0);return id<0xc; }
static void happy_hour(void *u,uint32_t c,uint32_t id,uint32_t v) { (void)u;CHECK(c==CTX);notef("hh%u/%u ",id,v); }
static uint32_t counter(void *u,uint32_t p,uint32_t i) { (void)u;notef("cnt%x/%u ",p,i);return counter_value; }
static const struct px_battle_moves_5_api api={&base,action_record,commit,message,message_arg,field_active,terrain,effect,opposing_selector,
    side_of,opposing_side_of,happy_hour,counter};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_5_api*);
static const fn dispatched[]={px_battle_reflect,px_battle_spikes};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w) { (void)u;CHECK(f<2);dispatched[f](h,c,v,w,&api); }
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;log_[0]=0;records=active_mask=terrain_value=counter_value=listed=0;
    for(unsigned i=0;i<8;++i){ memset(mem+RECORD+0x40*i,0xa5,0x40);w32(NULL,RECORD+0x40*i,0x11); }
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
static void frame(uint32_t attacker) { px_battle_event_push(&events);px_battle_event_set_id(0x03,attacker,&events);log_[0]=0; }
static void run(fn f,uint32_t id) { frame(ATT);f(HANDLER,CTX,id,WORK,&api);CHECK(aborts==0); }
static int same(const char *expected) { if(strcmp(log_,expected)){ fprintf(stderr,"log [%s] expected [%s]\n",log_,expected);return 0; } return 1; }
static uint32_t turns(uint32_t n) { return (n*8&0x1f8)|2; }
int main(void)
{
    unsigned cases=0;
    /* The turn count word. */
    for(uint32_t n=0;n<0x100;++n){ CHECK(px_battle_turn_count(n)==((n&0x3f)<<3|2));++cases; }
    CHECK(px_battle_turn_count(0xffffffff)==0x1fa&&px_battle_turn_count(0x40)==2);++cases;
    /* Simple field conditions: kind 0x1c, condition, turns, message. */
    {
        const fn f[4]={px_battle_mud_sport,px_battle_water_sport,px_battle_fairy_lock,px_battle_ion_deluge};
        const uint32_t cond[4]={5,4,9,8},t[4]={5,5,2,1},msg[4]={0x78,0x76,0x102,0x101};
        for(unsigned k=0;k<4;++k) {
            reset();run(f[k],ATT);char b[96];snprintf(b,sizeof b,"rec1c/2 msg%x/c1 commit0 ",msg[k]);
            CHECK(same(b)&&mem[RECORD+4]==cond[k]&&r32(NULL,RECORD+8)==turns(t[k])&&mem[RECORD+5]==0xa5&&r32(NULL,RECORD)==0x11);++cases;
        }
    }
    /* Gravity: not twice; halfwords +0xc and +0xe; effect 0x36. */
    for(uint32_t on=0;on<2;++on) {
        reset();active_mask=on?1u<<2:~(1u<<2);run(px_battle_gravity,ATT);
        if(on){ CHECK(same("fa2 ")&&records==0);++cases;continue; }
        CHECK(same("fa2 rec1c/2 commit0 eff36/2 ")&&mem[RECORD+4]==2&&r32(NULL,RECORD+8)==turns(5)&&r16(NULL,RECORD+0xc)==0x7b&&r16(NULL,RECORD+0xe)==0xa581);
        ++cases;
    }
    reset();w16(NULL,RECORD+0xe,0x7f);run(px_battle_gravity,ATT);CHECK(r16(NULL,RECORD+0xe)==1);++cases;
    reset();w16(NULL,RECORD+0xe,0xff80);run(px_battle_gravity,ATT);CHECK(r16(NULL,RECORD+0xe)==0xff81);++cases;
    /* Rooms toggle; Trick Room names the battler. */
    {
        const fn f[3]={px_battle_wonder_room,px_battle_magic_room,px_battle_trick_room};const uint32_t cond[3]={6,7,1};
        for(unsigned k=0;k<3;++k)for(uint32_t on=0;on<2;++on) {
            reset();active_mask=on?1u<<cond[k]:~(1u<<cond[k]);run(f[k],ATT);char b[128];
            if(on){ snprintf(b,sizeof b,"fa%u rec1d/2 commit0 ",cond[k]);CHECK(same(b)&&mem[RECORD+4]==cond[k]&&r32(NULL,RECORD+8)==0xa5a5a5a5); }
            else if(k<2){ snprintf(b,sizeof b,"fa%u rec1c/2 msg%x/c1 commit0 ",cond[k],k?0xba:0xb8);CHECK(same(b)&&mem[RECORD+4]==cond[k]&&r32(NULL,RECORD+8)==turns(5)); }
            else CHECK(same("fa1 rec1c/2 msg35c/c2 arg2/c commit0 ")&&mem[RECORD+4]==1&&r32(NULL,RECORD+8)==turns(5));
            ++cases;
        }
    }
    /* Terrains: not over the same terrain. */
    {
        const fn f[3]={px_battle_grassy_terrain,px_battle_misty_terrain,px_battle_electric_terrain};const uint32_t msg[3]={0xcd,0xcf,0xd1};
        for(unsigned k=0;k<3;++k)for(uint32_t t=0;t<5;++t) {
            reset();terrain_value=t==4?0x101+k:t;run(f[k],ATT);
            if(terrain_value==k+1){ CHECK(same("ter ")&&records==0);++cases;continue; }
            char b[96];snprintf(b,sizeof b,"ter rec1c/2 msg%x/c1 commit0 ",msg[k]);
            CHECK(same(b)&&mem[RECORD+4]==10&&mem[RECORD+5]==k+1&&r32(NULL,RECORD+8)==turns(5));++cases;
        }
    }
    /* Haze: the listed battlers of the opposing selector with 0x800. */
    for(uint32_t n=0;n<4;++n) {
        reset();listed=n;run(px_battle_haze,ATT);
        CHECK(same("recf/2 opp2 sb831/5 commit0 rec3/2 msg69/441 commit1 "));
        CHECK(mem[RECORD+4]==n);for(uint32_t i=0;i<n;++i) { CHECK(mem[RECORD+5+i]==10+i); }
        CHECK(mem[RECORD+5+n]==0xa5);++cases;
    }
    /* Side conditions of the user's side, and hazards on the other side. */
    {
        const fn f[10]={px_battle_reflect,px_battle_light_screen,px_battle_mist,px_battle_tailwind,px_battle_lucky_chant,px_battle_crafty_shield,
            px_battle_spikes,px_battle_toxic_spikes,px_battle_stealth_rock,px_battle_sticky_web};
        const uint32_t cond[10]={0,1,3,4,5,0x10,6,7,8,0xe},t[10]={5,5,5,4,5,1,0,0,0,0},msg[10]={0x82,0x86,0x8e,0x92,0x96,0xd3,0x9a,0x9e,0xa2,0xd6};
        for(unsigned k=0;k<10;++k)for(uint32_t id=1;id<3;++id) {
            reset();run(f[k],id);
            if(id!=ATT){ char b[32];snprintf(b,sizeof b,k<6?"side%u ":"oside%u ",id);CHECK(same(b)&&records==0);++cases;continue; }
            const uint32_t side=k<6?(id&1):1;char b[128];
            snprintf(b,sizeof b,k<6?"side2 rec19/2 msg%x/101 arg%x/10 commit0 ":"oside2 rec19/2 msg%x/101 arg%x/10 commit0 ",msg[k],side);
            CHECK(same(b)&&mem[RECORD+4]==cond[k]&&mem[RECORD+0xc]==side&&r32(NULL,RECORD+8)==(k<6?turns(t[k]):1u)&&mem[RECORD+0xd]==0xa5);++cases;
        }
        reset();frame(ATT);px_battle_safeguard(HANDLER,CTX,ATT,WORK,&api);
        CHECK(same("side2 rec19/2 msg8a/101 arg0/10 commit0 ")&&mem[RECORD+4]==2&&r32(NULL,RECORD+8)==turns(5));++cases;
        reset();frame(ATT);px_battle_safeguard(HANDLER,CTX,3,WORK,&api);CHECK(same("side3 ")&&records==0);++cases;
    }
    /* The setter itself: message narrowed to 16 bits; handler and work unused. */
    reset();frame(ATT);px_battle_side_condition(0xdead,CTX,ATT,0xbeef,0x1ff,0x107,0x12345678,0x10082,&api);
    CHECK(same("rec19/2 msg82/101 arg1ff/10 commit0 ")&&mem[RECORD+4]==7&&mem[RECORD+0xc]==0xff&&r32(NULL,RECORD+8)==0x12345678);++cases;
    reset();frame(ATT);px_battle_side_condition(HANDLER,CTX,ATT,WORK,1,6,1,0x301a5,&api);CHECK(same("rec19/2 msg1a5/101 arg1/10 commit0 "));++cases;   /* a message id above 0xff */
    /* Quick and Wide Guard: then the counter record, also for another battler. */
    for(unsigned k=0;k<2;++k)for(uint32_t id=1;id<3;++id)for(uint32_t c=0;c<3;++c) {
        reset();counter_value=c==2?0xff:c;run(k?px_battle_wide_guard:px_battle_quick_guard,id);
        char b[160];const uint32_t other=id==ATT?1:0;
        if(id==ATT) snprintf(b,sizeof b,"side2 rec19/2 msg%x/101 arg0/10 commit0 rec27/2 cnt%x/3 commit1 ",k?0xa6:0xa8,MON+0x200);
        else snprintf(b,sizeof b,"side1 rec27/1 cnt%x/3 commit0 ",MON+0x100);
        CHECK(same(b));
        const uint32_t g=RECORD+0x40*other;
        if(id==ATT) CHECK(mem[RECORD+4]==(k?9u:0xau)&&r32(NULL,RECORD+8)==turns(1));
        CHECK(mem[g+4]==id&&mem[g+5]==3&&mem[g+6]==(uint8_t)(counter_value+1)&&r32(NULL,g)==0x1000011&&mem[g+7]==0xa5);++cases;
    }
    /* Mat Block: its own record, message kind 2 naming the battler; side and count looked up first. */
    for(uint32_t id=1;id<3;++id) {
        reset();run(px_battle_mat_block,id);
        if(id!=ATT){ CHECK(same("side1 ")&&records==0);++cases;continue; }
        CHECK(same("side2 rec19/2 msg4a5/102 arg2/10 commit0 ")&&mem[RECORD+4]==0xf&&mem[RECORD+0xc]==0&&r32(NULL,RECORD+8)==turns(1));++cases;
    }
    /* Happy Hour. */
    reset();run(px_battle_happy_hour,ATT);CHECK(same("hh2/1 rec3/2 msgff/41 commit0 "));++cases;
    /* Nothing for another battler (the handlers that test key 0x03 first). */
    {
        const fn f[]={px_battle_gravity,px_battle_mud_sport,px_battle_water_sport,px_battle_fairy_lock,px_battle_ion_deluge,px_battle_trick_room,
            px_battle_wonder_room,px_battle_magic_room,px_battle_grassy_terrain,px_battle_misty_terrain,px_battle_electric_terrain,px_battle_haze,
            px_battle_happy_hour};
        for(unsigned k=0;k<sizeof f/sizeof *f;++k)for(uint32_t other=0;other<2;++other) {
            reset();frame(other?0x102:3);f[k](HANDLER,CTX,ATT,WORK,&api);CHECK(log_[0]==0&&records==0&&aborts==0);++cases;
        }
    }
    /* Through the dispatcher on event 0xad: Reflect for battler 2 and Spikes for battler 3. */
    {
        reset();
        for(unsigned i=0;i<2;++i){ w8(NULL,ENTRIES+0x10*i,0xad);w32(NULL,ENTRIES+0x10*i+4,i); }
        const uint8_t ids[2]={ATT,3};
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=NODES+0x40*i;
            w32(NULL,h+8,ENTRIES+0x10*i);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ids[i]);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,NODES);frame(ATT);px_battle_event_dispatch(CTX,0xad,&events);
        CHECK(same("side2 rec19/2 msg82/101 arg0/10 commit0 oside3 ")&&aborts==0);++cases;
    }
    printf("battle_moves_5: %u cases, all tests passed\n",cases);
    return 0;
}
