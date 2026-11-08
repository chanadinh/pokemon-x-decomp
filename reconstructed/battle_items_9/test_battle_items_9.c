#include "battle_items_9.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000,
       HANDLER=0x6000, WORK=0x601c, RECORD=0x7000, CURES=0x7800, SCRATCH=0x7900, ENTRIES=0x7a00, P=MON+0x200 };
enum { RUNNING=0x1000000, REENTRY=0x10000000, REGISTERED=0x20000000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static uint32_t hp,maxhp,ability,param,roll,force,full;static unsigned aborts,records;static char log_[512];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k)
{ (void)u;CHECK(p==P);notef("v%x ",k,0);if(k==0xd)return hp;if(k==0xe)return maxhp;CHECK(k==0x11);return ability; }
static const struct px_battle_ability_api base={.events=&events,.pokemon_value=pokemon_value};
static uint32_t item_param(void *u,uint32_t item,uint32_t k) { (void)u;CHECK(k==3);notef("ip%x ",item,0);return param; }
static void popup(void *u,uint32_t c,uint32_t p,uint32_t id) { (void)u;CHECK(c==CTX);notef("pop%u/%u ",p,id); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id)
{ (void)u;CHECK(c==CTX&&records<1);notef("rec%x/%x ",k,id);++records;return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;char t[48];snprintf(t,sizeof t,"msg%x@%x/%x ",id,r-RECORD,k);note(t); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;notef("arg%x@%x ",v,r-RECORD); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD);note("commit "); }
static const struct px_battle_items_api items={&base,NULL,item_param,popup,action_record,message,message_arg,commit,CURES,SCRATCH};
static uint32_t chance(void *u,uint32_t p) { (void)u;notef("ch%u ",p,0);return roll; }
static uint32_t forced(void *u,uint32_t c,uint32_t f) { (void)u;CHECK(c==CTX);notef("fo%u ",f,0);return force; }
static uint32_t full_hp(void *u,uint32_t p) { (void)u;CHECK(p==P);note("full ");return full; }
static const struct px_battle_items_9_api api={&items,chance,forced,full_hp};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_items_9_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_focus_sash_endure,px_battle_focus_sash_show};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);hp=20;maxhp=80;ability=0;param=20;roll=0;force=0;full=0;aborts=0;records=0;log_[0]=0;
    w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
    w16(NULL,HANDLER+0x38,0xd9);w32(NULL,HANDLER+0x18,RUNNING);
}
/* Keys 0x02, 0x03, 0x04 and 0x26; the bracket 0x11, endure 0x3b and flinch 0x27 are present. */
static void frame(uint32_t k2,uint32_t k3,uint32_t k4,uint32_t k26)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x02,k2,&events);px_battle_event_set_id(0x03,k3,&events);
    px_battle_event_set_id(0x04,k4,&events);px_battle_event_set_value(0x26,k26,&events);
    px_battle_event_set_value(0x11,1,&events);px_battle_event_set_value(0x3b,0,&events);px_battle_event_set_value(0x27,0,&events);log_[0]=0;
}
/* A frame whose keys 0x11, 0x3b and 0x27 were already rewritten once (kind 2): a further
 * rewrite does not take and reports 0, without aborting. */
static void bare(uint32_t k2,uint32_t k4)
{
    frame(k2,7,k4,0);
    CHECK(px_battle_event_rewrite(0x11,1,&events)==1&&px_battle_event_rewrite(0x3b,0,&events)==1&&px_battle_event_rewrite(0x27,0,&events)==1);
    log_[0]=0;
}
static unsigned cases;
static void expect(const char *s) { if(strcmp(log_,s)){fprintf(stderr,"got  [%s]\nwant [%s]\n",log_,s);} CHECK(!strcmp(log_,s));++cases; }
int main(void)
{
    /* Custap: the holder, the quarter-HP pinch with Gluttony, and whether the rewrite takes. */
    {
        for(uint32_t holder=2;holder<4;++holder)for(uint32_t h=0;h<=40;++h)for(unsigned glut=0;glut<2;++glut)for(unsigned key=0;key<2;++key){
            reset();hp=h;ability=glut?0x52:0;if(key)frame(holder,7,7,0); else bare(holder,7);
            px_battle_custap_check(HANDLER,CTX,2,WORK,&api);
            const uint32_t d=glut?2:4;const int pinch=h<=80/d;
            const int ask=holder==2&&pinch&&key;
            expect(holder!=2?"":pinch?(ask?"v11 ve vd ve pop0/2 ":"v11 ve vd ve "):"v11 ve vd ve ");
            CHECK(r32(NULL,HANDLER+0x18)==(ask?RUNNING|REENTRY:(uint32_t)RUNNING));
            CHECK(px_battle_event_get(0x11,&events)==(key&&holder==2&&pinch?2u:1u)&&aborts==0);
        }
    }
    /* Quick Claw: the roll on parameter 3's low byte, the forced flag, the rewrite. */
    {
        const uint32_t params[]={20,0x114,0};
        for(uint32_t holder=2;holder<4;++holder)for(uint32_t r=0;r<2;++r)for(uint32_t f=0;f<2;++f)for(unsigned key=0;key<2;++key)for(unsigned k=0;k<3;++k){
            reset();roll=r;force=f;param=params[k];if(key)frame(holder,7,7,0); else bare(holder,7);
            px_battle_quick_claw_check(HANDLER,CTX,2,WORK,&api);
            char b[64]="";const int go=r||f;
            if(holder==2){ char c[32];snprintf(c,sizeof c,"ipd9 ch%u ",params[k]&0xff);strcat(b,c);if(!r)strcat(b,"fo2 ");if(go&&key)strcat(b,"pop0/2 "); }
            expect(b);
            CHECK(px_battle_event_get(0x11,&events)==(key&&holder==2&&go?2u:1u)&&aborts==0);
        }
    }
    /* Their event-0x7b uses clear work word 0 after the message. */
    {
        static const fn h[]={px_battle_custap_use,px_battle_quick_claw_use};
        for(unsigned i=0;i<2;++i)for(uint32_t holder=1;holder<4;++holder){
            reset();w32(NULL,WORK,0x55);frame(holder,7,7,0);h[i](HANDLER,CTX,2,WORK,&api);
            expect(holder!=2?"":"rec3/2 msg407@4/2 arg2@4 argd9@4 commit ");CHECK(r32(NULL,WORK)==(holder==2?0u:0x55u));
        }
    }
    /* Lagging Tail and Full Incense: the holder moves last. */
    {
        for(uint32_t holder=1;holder<4;++holder){
            reset();frame(holder,7,7,0);px_battle_lagging_tail(HANDLER,CTX,2,WORK,&api);expect("");
            CHECK(px_battle_event_get(0x11,&events)==(holder==2?0u:1u));
        }
        reset();bare(2,7);px_battle_lagging_tail(HANDLER,CTX,2,WORK,&api);expect("");CHECK(aborts==0&&px_battle_event_get(0x11,&events)==1);
    }
    /* King's Rock and Razor Fang: the attacker, key 0x26, parameter 3's low byte; the event-0x7c use. */
    {
        const uint32_t params[]={10,0x10a,0xff};
        for(uint32_t holder=2;holder<4;++holder)for(uint32_t k26=0;k26<3;++k26)for(unsigned k=0;k<3;++k){
            reset();param=params[k];frame(7,holder,7,k26);px_battle_kings_rock(HANDLER,CTX,2,WORK,&api);
            const int hit=holder==2&&!k26;
            expect(hit?"ipd9 ":"");CHECK(px_battle_event_get(0x27,&events)==(hit?params[k]&0xff:0u));
        }
        for(uint32_t holder=1;holder<4;++holder){
            reset();frame(holder,7,7,0);px_battle_kings_rock_use(HANDLER,CTX,2,WORK,&api);
            if(holder!=2){ expect("");continue; }
            expect("rec2c/2 commit ");CHECK(r8(NULL,RECORD+4)==2&&r8(NULL,RECORD+5)==100);
            for(uint32_t k=0;k<0x100;++k)if(k!=4&&k!=5)CHECK(r8(NULL,RECORD+k)==0);
        }
    }
    /* Focus Band: endure on a roll, show once it held on, the event-0x7b message. */
    {
        for(uint32_t holder=2;holder<4;++holder)for(uint32_t r=0;r<2;++r)for(uint32_t f=0;f<2;++f)for(unsigned key=0;key<2;++key){
            reset();roll=r;force=f;param=10;w32(NULL,WORK,0x55);if(key)frame(7,7,holder,0); else bare(7,holder);
            px_battle_focus_band_endure(HANDLER,CTX,2,WORK,&api);
            char b[48]="";const int go=r||f;
            if(holder==2){ strcat(b,"ipd9 ch10 ");if(!r)strcat(b,"fo2 "); }
            expect(b);
            CHECK(r32(NULL,WORK)==(holder==2&&go?(key?1u:0u):0x55u));
            CHECK(px_battle_event_get(0x3b,&events)==(key&&holder==2&&go?3u:0u)&&aborts==0);
        }
        const uint32_t works[]={0,1,2,0x80000000};
        for(unsigned w=0;w<4;++w)for(uint32_t holder=1;holder<4;++holder){
            reset();w32(NULL,WORK,works[w]);frame(holder,7,7,0);px_battle_focus_band_show(HANDLER,CTX,2,WORK,&api);
            const int ask=holder==2&&works[w];
            expect(ask?"pop0/2 ":"");CHECK(r32(NULL,WORK)==(ask?0u:works[w])&&r32(NULL,HANDLER+0x18)==(ask?RUNNING|REENTRY:(uint32_t)RUNNING));
            reset();frame(holder,7,7,0);px_battle_focus_band_use(HANDLER,CTX,2,WORK,&api);
            expect(holder!=2?"":"rec3/2 msg3f5@4/2 arg2@4 argd9@4 commit ");
        }
    }
    /* Focus Sash: endure at full HP, show once it held on. */
    {
        for(uint32_t holder=2;holder<4;++holder)for(uint32_t f=0;f<2;++f)for(unsigned key=0;key<2;++key){
            reset();full=f;w32(NULL,WORK,0x55);if(key)frame(7,7,holder,0); else bare(7,holder);
            px_battle_focus_sash_endure(HANDLER,CTX,2,WORK,&api);
            expect(holder==2?"full ":"");
            CHECK(r32(NULL,WORK)==(holder==2&&f?(key?1u:0u):0x55u));
            CHECK(px_battle_event_get(0x3b,&events)==(key&&holder==2&&f?3u:0u)&&aborts==0);
        }
        const uint32_t works[]={0,1,0xffffffff};
        for(unsigned w=0;w<3;++w)for(uint32_t holder=1;holder<4;++holder){
            reset();w32(NULL,WORK,works[w]);frame(holder,7,7,0);px_battle_focus_sash_show(HANDLER,CTX,2,WORK,&api);
            const int hit=holder==2&&works[w];
            expect(hit?"rec24/2 msg3f5@8/2 arg2@8 argd9@8 commit ":"");CHECK(r32(NULL,WORK)==(hit?0u:works[w]));
        }
    }
    /* Through the dispatcher: Focus Sash endures on 0x7d and shows on 0x7e. */
    {
        reset();full=1;w32(NULL,HANDLER+0x18,REGISTERED|1u<<16);
        w8(NULL,ENTRIES,0x7d);w32(NULL,ENTRIES+4,0);w8(NULL,ENTRIES+8,0x7e);w32(NULL,ENTRIES+12,1);
        w32(NULL,HANDLER+8,ENTRIES);w8(NULL,HANDLER+0x3a,2);w32(NULL,BATTLE+0x60,HANDLER);
        frame(2,7,2,0);px_battle_event_dispatch(CTX,0x7d,&events);expect("full ");CHECK(r32(NULL,WORK)==1&&px_battle_event_get(0x3b,&events)==3);
        w32(NULL,HANDLER+8,ENTRIES+8);log_[0]=0;px_battle_event_dispatch(CTX,0x7e,&events);
        expect("rec24/2 msg3f5@8/2 arg2@8 argd9@8 commit ");CHECK(r32(NULL,WORK)==0&&aborts==0);
    }
    printf("battle_items_9: %u cases, all tests passed\n",cases);
    return 0;
}
