#include "battle_moves_8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, ACTION=0x7000, MSGBUF=0x7400, ENTRIES=0x7900, NODES=0x8000 };
enum { ATT=2, DEF=5 };
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
static uint32_t values[9][0x20];
static unsigned slot(uint32_t p) { if(!p) return 8;CHECK(p>=MON&&p<MON+0x800&&!(p&0xff));return (p-MON)>>8; }   /* 0: an aborted lookup */
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;CHECK(k<0x20);notef("v%u.%x ",slot(p),k);return values[slot(p)][k]; }
static const struct px_battle_ability_api base={&events,pokemon_value,NULL,NULL,NULL,NULL,NULL};
static uint32_t status[9],type_mask[9],packed[9],air,moved,chosen,damaging;
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;notef("hs%u/%u ",slot(p),s);return status[slot(p)]>>s&1; }
static uint32_t has_type(void *u,uint32_t p,uint32_t t) { (void)u;notef("ht%u/%u ",slot(p),t);return type_mask[slot(p)]>>t&1; }
static uint32_t types(void *u,uint32_t p) { (void)u;notef("ty%u ",slot(p),0);return packed[slot(p)]; }
static uint32_t airborne(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("air%u ",id,0);return air; }
static uint32_t turn_flag(void *u,uint32_t p,uint32_t f) { (void)u;notef("tf%u/%u ",slot(p),f);return moved; }
static uint32_t chosen_action(void *u,uint32_t c,uint32_t id,uint32_t out) { (void)u;CHECK(c==CTX&&out==ACTION);notef("act%u ",id,0);
    if(chosen) { w32(NULL,out,chosen); }
    return chosen!=0; }
static uint32_t action_move(void *u,uint32_t a) { (void)u;CHECK(a==ACTION);note("am ");return r32(NULL,a)&0xffff; }
static uint32_t move_damages(void *u,uint32_t m) { (void)u;notef("dmg%u ",m,0);return m<32&&(damaging>>m&1); }
static void message(void *u,uint32_t at,uint32_t k,uint32_t m) { (void)u;notef("msg%x/%x ",m,at*16+k); }
static const struct px_battle_moves_8_api api={&base,has_status,has_type,types,airborne,turn_flag,chosen_action,action_move,move_damages,message,ACTION};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_8_api*);
static const fn dispatched[]={px_battle_mean_look_check,px_battle_dream_eater_check};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w) { (void)u;CHECK(f<2);dispatched[f](h,c,v,w,&api); }
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;log_[0]=0;memset(values,0,sizeof values);memset(status,0,sizeof status);memset(type_mask,0,sizeof type_mask);
    for(unsigned i=0;i<9;++i) packed[i]=0x12|0x12<<5|0x12<<10;
    air=moved=chosen=damaging=0;
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
/* Keys 0x03, 0x04 (ids), 0x40 (message buffer, value), 0x41 (failure, value). */
static void frame(uint32_t attacker,uint32_t def)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x03,attacker,&events);px_battle_event_set_id(0x04,def,&events);
    px_battle_event_set_value(0x40,MSGBUF,&events);px_battle_event_set_value(0x41,0,&events);log_[0]=0;
}
static uint32_t failed(void) { return px_battle_event_get(0x41,&events); }
static uint32_t run(fn f) { frame(ATT,DEF);f(HANDLER,CTX,ATT,WORK,&api);CHECK(aborts==0&&px_battle_event_get(0x40,&events)==MSGBUF);return failed(); }
static int same(const char *expected) { if(strcmp(log_,expected)){ fprintf(stderr,"log [%s] expected [%s]\n",log_,expected);return 0; } return 1; }
int main(void)
{
    unsigned cases=0;
    const fn all[]={px_battle_attract_check,px_battle_captivate_check,px_battle_dream_eater_check,px_battle_nightmare_check,
        px_battle_synchronoise_check,px_battle_sucker_punch_check,px_battle_rototiller_check,px_battle_sky_drop_check,
        px_battle_worry_seed_check,px_battle_mean_look_check,px_battle_endeavor_check};
    /* Nothing for another attacker. */
    for(unsigned k=0;k<sizeof all/sizeof *all;++k)for(uint32_t other=0;other<2;++other) {
        reset();air=1;type_mask[DEF]=~0u;frame(other?0x102:3,DEF);all[k](HANDLER,CTX,ATT,WORK,&api);CHECK(log_[0]==0&&failed()==0&&aborts==0);++cases;
    }
    /* Attract and Captivate: genders. */
    for(unsigned k=0;k<2;++k)for(uint32_t a=0;a<4;++a)for(uint32_t d=0;d<4;++d) {
        reset();values[ATT][0x12]=a==3?0x101:a;values[DEF][0x12]=d==3?0x200:d;
        const uint32_t ga=values[ATT][0x12]&0xff,gd=values[DEF][0x12]&0xff;
        CHECK(run(k?px_battle_captivate_check:px_battle_attract_check)==(ga==2||gd==2||ga==gd));
        CHECK(same("v2.12 v5.12 "));++cases;
    }
    /* The lookup order: an empty attacker slot aborts before (Attract) or after (Captivate) the defender lookup. */
    for(unsigned k=0;k<2;++k){ reset();w32(NULL,TABLE+0x84+4*ATT,0);frame(ATT,DEF);(k?px_battle_captivate_check:px_battle_attract_check)(HANDLER,CTX,ATT,WORK,&api);
        CHECK(aborts==1);++cases; }
    /* Dream Eater and Nightmare: the defender asleep. */
    for(unsigned k=0;k<2;++k)for(uint32_t s=0;s<4;++s){ reset();status[DEF]=1u<<s;status[ATT]=4;
        CHECK(run(k?px_battle_nightmare_check:px_battle_dream_eater_check)==(s!=2)&&same("hs5/2 "));++cases; }
    reset();status[DEF]=4;frame(ATT,0x300+DEF);px_battle_dream_eater_check(HANDLER,CTX,ATT,WORK,&api);CHECK(failed()==0&&same("hs5/2 "));++cases;
    /* Synchronoise: a shared type, over every pair of type words drawn from a small type set. */
    {
        const uint32_t set[5]={0x12,3,7,9,0x1f};
        for(uint32_t pa=0;pa<125;++pa)for(uint32_t pd=0;pd<125;pd+=3) {
            const uint32_t a[3]={set[pa%5],set[pa/5%5],set[pa/25]},d[3]={set[pd%5],set[pd/5%5],set[pd/25]};
            reset();packed[ATT]=a[0]|a[1]<<5|a[2]<<10|0xffff8000u;packed[DEF]=d[0]|d[1]<<5|d[2]<<10|0x8000u;
            int share=0;for(int i=0;i<3;++i) if(a[i]!=0x12&&(a[i]==d[0]||a[i]==d[1]||a[i]==d[2])) share=1;
            CHECK(run(px_battle_synchronoise_check)==(uint32_t)!share&&same("ty2 ty5 "));++cases;
        }
    }
    /* Sucker Punch. */
    for(uint32_t m=0;m<2;++m)for(uint32_t c=0;c<3;++c)for(uint32_t dmg=0;dmg<2;++dmg)for(uint32_t stale=0;stale<3;++stale) {
        reset();moved=m;chosen=c==0?0:c==1?0x10000:0x10007;damaging=dmg?1u<<7|1u<<9:0;w32(NULL,ACTION,stale==0?0:stale==1?7:9);
        const uint32_t got=run(px_battle_sucker_punch_check);
        /* the buffer holds the chosen action when one was copied, otherwise its old contents */
        const uint32_t buffer=!m&&c?chosen:(stale==0?0:stale==1?7:9);
        const uint32_t mv=buffer&0xffff;const int ok=mv!=0&&(damaging>>(mv<32?mv:0)&1)&&mv<32;
        CHECK(got==(uint32_t)!ok);
        if(!ok) CHECK(strstr(log_,"msg4a/74001")!=NULL);
        ++cases;
    }
    reset();chosen=0x10007;damaging=1u<<7;CHECK(run(px_battle_sucker_punch_check)==0&&same("tf5/3 act5 am dmg7 "));++cases;
    reset();moved=1;w32(NULL,ACTION,7);damaging=1u<<7;CHECK(run(px_battle_sucker_punch_check)==0&&same("tf5/3 am dmg7 "));++cases;
    reset();chosen=0x10007;CHECK(run(px_battle_sucker_punch_check)==1&&same("tf5/3 act5 am dmg7 am dmg7 msg4a/74001 "));++cases;
    reset();frame(ATT,DEF);px_battle_event_rewrite(0x41,0,&events);log_[0]=0;px_battle_sucker_punch_check(HANDLER,CTX,ATT,WORK,&api);
    CHECK(failed()==0&&same("tf5/3 act5 am "));++cases;   /* key 0x41 already rewritten: no message */
    /* Rototiller: an airborne defender (by id). */
    for(uint32_t a=0;a<3;++a){ reset();air=a==2?0x100:a;CHECK(run(px_battle_rototiller_check)==(air!=0)&&same("air5 "));++cases; }
    reset();frame(ATT,0x300+DEF);px_battle_rototiller_check(HANDLER,CTX,ATT,WORK,&api);CHECK(same("air5 "));++cases;
    /* Sky Drop (Flying) and Mean Look (Ghost). */
    for(uint32_t t=0;t<10;++t){ reset();type_mask[DEF]=1u<<t;type_mask[ATT]=~0u;CHECK(run(px_battle_sky_drop_check)==(t==2)&&same("ht5/2 "));++cases;
        reset();type_mask[DEF]=1u<<t;type_mask[ATT]=~0u;CHECK(run(px_battle_mean_look_check)==(t==7)&&same("ht5/7 "));++cases; }
    /* Worry Seed: Truant or Insomnia. */
    {
        const uint32_t ab[]={0,15,54,72,0x10f,0x136};
        for(unsigned k=0;k<6;++k){ reset();values[DEF][0x11]=ab[k];values[ATT][0x11]=54;
            CHECK(run(px_battle_worry_seed_check)==(ab[k]==15||ab[k]==54));CHECK(same(ab[k]==54?"v5.11 ":"v5.11 v5.11 "));++cases; }
    }
    /* Endeavor: defender HP above the attacker's (signed). */
    for(uint32_t d=0;d<6;++d)for(uint32_t a=0;a<6;++a){ reset();values[DEF][0xd]=d*10;values[ATT][0xd]=a*10;
        CHECK(run(px_battle_endeavor_check)==(d<=a)&&same("v5.d v2.d "));++cases; }
    reset();values[DEF][0xd]=0x80000000;values[ATT][0xd]=1;CHECK(run(px_battle_endeavor_check)==0);++cases;   /* 0x7fffffff > 0 */
    reset();values[DEF][0xd]=0x7fffffff;values[ATT][0xd]=0xffffffff;CHECK(run(px_battle_endeavor_check)==1);++cases;   /* 0x80000000 < 0 */
    reset();values[DEF][0xd]=1;values[ATT][0xd]=0x80000002;CHECK(run(px_battle_endeavor_check)==0);++cases;   /* difference 0x7fffffff: no failure */
    /* Through the dispatcher on event 0x2e: Mean Look for battler 2 against a Ghost, Dream Eater for battler 3. */
    {
        reset();type_mask[DEF]=1u<<7;
        for(unsigned i=0;i<2;++i){ w8(NULL,ENTRIES+0x10*i,0x2e);w32(NULL,ENTRIES+0x10*i+4,i); }
        const uint8_t ids[2]={ATT,3};
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=NODES+0x40*i;
            w32(NULL,h+8,ENTRIES+0x10*i);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ids[i]);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,NODES);frame(ATT,DEF);px_battle_event_dispatch(CTX,0x2e,&events);
        CHECK(failed()==1&&same("ht5/7 ")&&aborts==0);++cases;
    }
    printf("battle_moves_8: %u cases, all tests passed\n",cases);
    return 0;
}
