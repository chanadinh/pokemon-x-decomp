#include "battle_moves_12.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, STATE=0x7400, DATA=0x7440, FLAG=0x7480, COUNTS=0x7490, ENTRIES=0x7900, NODES=0x8000 };
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
static unsigned slot(uint32_t p) { CHECK(p>=MON&&p<MON+0x800&&!(p&0xff));return (p-MON)>>8; }
static const struct px_battle_ability_api base={&events,NULL,NULL,NULL,NULL,NULL,NULL};
static const struct px_battle_handler_api handlers={&memory,NULL,abort_battle,NULL,NULL,NULL,BATTLE,POOL,0,0,0,0,COUNTS};
static uint32_t records,carried,selector,lift,lift_block;
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX&&records<8);notef("rec%x/%u ",k,id);return RECORD+0x40*records++; }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX);notef("commit%u ",(r-RECORD)/0x40,0); }
static void message(void *u,uint32_t at,uint32_t k,uint32_t m) { (void)u;notef("msg%x/%x ",m,(at-RECORD)*16+k); }
static void message_arg(void *u,uint32_t at,uint32_t v) { (void)u;(void)at;notef("arg%x ",v,0); }
static uint32_t counter(void *u,uint32_t p,uint32_t i) { (void)u;notef("cnt%u/%u ",slot(p),i);return carried; }
static uint32_t opposing_selector(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("opp%u ",id,0);return selector; }
static void land(void *u,uint32_t c,uint32_t id,uint32_t z,uint32_t z2) { (void)u;CHECK(c==CTX&&z==0&&z2==0);notef("land%u ",id,0); }
static void release(void *u,uint32_t c,uint32_t t) { (void)u;CHECK(c==CTX);notef("rel%x ",t,0); }
static uint32_t lift_fn(void *u,uint32_t c,uint32_t id,uint32_t t,uint32_t st,uint32_t out) { (void)u;CHECK(c==CTX&&st==STATE&&out==FLAG);
    CHECK(mem[FLAG]==0);notef("lift%u/%u ",id,t);if(lift_block) { w8(NULL,out,(uint8_t)lift_block); }
    return lift; }
static uint32_t hp_fraction(void *u,uint32_t p,uint32_t d) { (void)u;notef("hp%u/%u ",slot(p),d);return 0x12345; }
static const struct px_battle_moves_12_api api={&base,&handlers,action_record,commit,message,message_arg,counter,opposing_selector,land,release,
    lift_fn,hp_fraction,STATE,FLAG};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_12_api*);
static const fn dispatched[]={px_battle_bide_end,px_battle_heal_bell_done};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w) { (void)u;CHECK(f<2);dispatched[f](h,c,v,w,&api); }
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;log_[0]=0;records=carried=selector=lift=lift_block=0;
    for(unsigned i=0;i<8;++i){ memset(mem+RECORD+0x40*i,0xa5,0x40); }
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
    for(unsigned i=0;i<PX_BATTLE_HANDLER_POOL;++i)w32(NULL,POOL+4*i,NODES+0x40*i);
    for(unsigned i=0;i<5;++i) w32(NULL,DATA+4*i,0x1000*i+0x11);
    w8(NULL,FLAG,0xee);
}
/* Keys: 0x02 battler, 0x03 attacker, 0x06 target (ids); 0x40 data address, 0x42, 0x50, 0x53 (values). */
static uint32_t target_key=5;
static void frame2(uint32_t battler,uint32_t attacker)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x02,battler,&events);px_battle_event_set_id(0x03,attacker,&events);
    px_battle_event_set_id(0x06,target_key,&events);px_battle_event_set_value(0x40,DATA,&events);px_battle_event_set_value(0x42,0,&events);
    px_battle_event_set_value(0x50,0,&events);px_battle_event_set_value(0x53,0,&events);log_[0]=0;
}
static uint32_t key(uint32_t k) { return px_battle_event_get(k,&events); }
static void run(fn f) { frame2(ATT,ATT);f(HANDLER,CTX,ATT,WORK,&api);CHECK(aborts==0); }
static int same(const char *expected) { if(strcmp(log_,expected)){ fprintf(stderr,"log [%s] expected [%s]\n",log_,expected);return 0; } return 1; }
#define R(n) (RECORD+0x40*(n))
#define W(i) r32(NULL,WORK+4*(i))
int main(void)
{
    unsigned cases=0;
    /* Nothing for another battler (key 0x02 or 0x03 as each tests). */
    {
        const fn by2[]={px_battle_sky_drop_missed,px_battle_bide_end,px_battle_protect_end,px_battle_rollout_end,px_battle_jump_kick_crash};
        const fn by3[]={px_battle_sky_drop_lift,px_battle_sky_drop_drop,px_battle_sky_drop_cancelled,px_battle_pledge_done};
        for(unsigned k=0;k<5;++k)for(uint32_t o=0;o<2;++o){ reset();w32(NULL,WORK,1);w32(NULL,WORK+0x18,1);frame2(o?0x102:3,ATT);by2[k](HANDLER,CTX,ATT,WORK,&api);
            CHECK(log_[0]==0&&records==0&&W(0)==1&&W(6)==1);++cases; }
        for(unsigned k=0;k<4;++k)for(uint32_t o=0;o<2;++o){ reset();w32(NULL,WORK,1);frame2(ATT,o?0x102:3);by3[k](HANDLER,CTX,ATT,WORK,&api);
            CHECK(log_[0]==0&&records==0&&W(0)==1&&key(0x42)==0);++cases; }
    }
    /* Sky Drop's charging turn. */
    for(uint32_t t=0;t<3;++t)for(uint32_t l=0;l<2;++l)for(uint32_t b=0;b<3;++b)for(uint32_t pre=0;pre<2;++pre) {
        reset();target_key=t==0?5:t==1?0x1f:0x105;lift=l;lift_block=b==2?0x100:b;frame2(ATT,ATT);if(pre) px_battle_event_rewrite(0x53,9,&events);log_[0]=0;
        px_battle_sky_drop_lift(HANDLER,CTX,ATT,WORK,&api);
        for(unsigned i=0;i<5;++i) { CHECK(r32(NULL,STATE+4*i)==0x1000*i+0x11); }
        const int lifted=t!=1&&l;
        if(lifted){ CHECK(key(0x42)==0&&key(0x50)==0);
            if(pre) CHECK(same("lift2/5 ")&&records==0&&key(0x53)==9);
            else CHECK(same("lift2/5 rec3/2 msg464/42 arg2 arg5 commit0 ")&&key(0x53)==1); }
        else { CHECK(key(0x42)==1&&key(0x50)==(t!=1&&(b&1)?1u:0u)&&records==0);CHECK(same(t==1?"":"lift2/5 ")); }
        ++cases;
    }
    target_key=5;
    reset();frame2(ATT,ATT);px_battle_event_rewrite(0x40,STATE-4,&events);for(unsigned i=0;i<6;++i) { w32(NULL,STATE-4+4*i,0x100+i); }
    px_battle_sky_drop_lift(HANDLER,CTX,ATT,WORK,&api);for(unsigned i=0;i<5;++i) { CHECK(r32(NULL,STATE+4*i)==0x100+i); }
    ++cases;   /* the source overlaps the state from below: all five read before any is written */
    /* Sky Drop's release turn. */
    for(uint32_t c=0;c<0x1b;++c)for(uint32_t s=0;s<2;++s) {
        reset();carried=c==0x1a?0x105:c;selector=s?0xa:0x31;run(px_battle_sky_drop_drop);
        const uint32_t cand=(carried-1)&0xff;const int have=carried!=0&&cand<0x18;
        const int fails=!have||s;CHECK(key(0x42)==(uint32_t)fails&&W(0)==1&&W(1)==(have?cand:0x1fu));
        CHECK(strstr(log_,"land2 ")!=NULL&&(strstr(log_,"opp")!=NULL)==have);++cases;
    }
    /* Sky Drop's release on a miss (key 0x02) or interruption (key 0x03). */
    for(uint32_t k=0;k<2;++k)for(uint32_t w=0;w<2;++w) {
        reset();w32(NULL,WORK,w?0x100:0);w32(NULL,WORK+4,0x1207);run(k?px_battle_sky_drop_cancelled:px_battle_sky_drop_missed);
        if(w) CHECK(same("rel7 ")&&W(0)==0);else CHECK(log_[0]==0&&W(0)==0);
        ++cases;
    }
    /* Bide's lock ends. */
    for(uint32_t l=0;l<2;++l){ reset();w32(NULL,WORK+0x18,l?0x100:0);run(px_battle_bide_end);
        if(l) CHECK(same("reca/2 commit0 ")&&mem[R(0)+4]==0x19&&mem[R(0)+5]==ATT&&mem[R(0)+0x11]==1&&mem[R(0)+0x12]==1&&W(6)==0&&mem[R(0)+6]==0xa5);
        else CHECK(log_[0]==0&&records==0);
        ++cases; }
    /* Protect family: streak reset and removal (a registered handler and a second one kept). */
    {
        reset();const uint32_t h=px_battle_handler_register(0,182,1,0,ATT,ENTRIES,1,&handlers);const uint32_t k2=px_battle_handler_register(0,182,1,1,ATT,ENTRIES,1,&handlers);
        frame2(ATT,ATT);px_battle_protect_end(h,CTX,ATT,h+0x1c,&api);
        CHECK(same("rec27/2 commit0 ")&&mem[R(0)+4]==ATT&&mem[R(0)+5]==3&&mem[R(0)+6]==0&&r32(NULL,h+0x18)==0&&r32(NULL,k2+0x18)!=0);++cases;
    }
    /* Rollout and Ice Ball: lock ends and the move's handlers go. */
    {
        reset();
        uint32_t h[5];const uint32_t owners[5]={0,0,0,4,0},vals[5]={ATT,ATT,3,ATT,ATT},ids[5]={205,33,205,205,205};
        for(unsigned i=0;i<5;++i) h[i]=px_battle_handler_register(owners[i],ids[i],1,i,vals[i],ENTRIES,1,&handlers);
        w16(NULL,HANDLER+0x38,205);w32(NULL,WORK+0x18,1);run(px_battle_rollout_end);
        CHECK(same("reca/2 commit0 ")&&mem[R(0)+4]==0x19&&mem[R(0)+5]==ATT&&mem[R(0)+0x11]==1&&mem[R(0)+0x12]==0xa5&&W(6)==0);
        for(unsigned i=0;i<5;++i) { CHECK((r32(NULL,h[i]+0x18)==0)==(i==0||i==4)); }
        ++cases;
    }
    /* Jump Kick and High Jump Kick crash. */
    reset();run(px_battle_jump_kick_crash);
    CHECK(same("hp2/2 rec6/2 msg38c/c2 arg2 commit0 ")&&mem[R(0)+6]==ATT&&r16(NULL,R(0)+4)==0x2345);++cases;
    /* Heal Bell, Aromatherapy (no key test) and the pledges. */
    reset();frame2(3,3);px_battle_heal_bell_done(HANDLER,CTX,ATT,WORK,&api);CHECK(same("rec3/2 msg73/41 commit0 "));++cases;
    reset();frame2(3,3);px_battle_aromatherapy_done(HANDLER,CTX,ATT,WORK,&api);CHECK(same("rec3/2 msg74/41 commit0 "));++cases;
    for(uint32_t w=0;w<2;++w){ reset();w32(NULL,WORK,w?0x100:0);run(px_battle_pledge_done);CHECK(same(w?"rec3/2 msgc1/41 commit0 ":""));++cases; }
    /* Through the dispatcher on event 0x21: Bide for battler 2, Heal Bell's done handler wired to event 0x21 for battler 3 (it has no key test). */
    {
        reset();
        for(unsigned i=0;i<2;++i){ w8(NULL,ENTRIES+0x10*i,0x21);w32(NULL,ENTRIES+0x10*i+4,i); }
        const uint8_t ids[2]={ATT,3};
        for(unsigned i=0;i<2;++i) {
            const uint32_t hn=NODES+0x40*i;
            w32(NULL,hn+8,ENTRIES+0x10*i);w32(NULL,hn+0x18,0x20000000u|1u<<16);w8(NULL,hn+0x3a,ids[i]);
            w32(NULL,hn,i?hn-0x40:0);w32(NULL,hn+4,i<1?hn+0x40:0);
        }
        w32(NULL,NODES+0x1c+0x18,1);w32(NULL,BATTLE+0x60,NODES);frame2(ATT,ATT);px_battle_event_dispatch(CTX,0x21,&events);
        CHECK(same("reca/2 commit0 rec3/3 msg73/441 commit1 ")&&r32(NULL,NODES+0x1c+0x18)==0&&aborts==0);++cases;
    }
    printf("battle_moves_12: %u cases, all tests passed\n",cases);
    return 0;
}
