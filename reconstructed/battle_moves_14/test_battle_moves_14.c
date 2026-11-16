#include "battle_moves_14.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, PROTECT=0x7440, COUNTS=0x7490, ENTRIES=0x7900, NODES=0x8000 };
enum { ATT=2 };
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
static void notef(const char *f,uint32_t a,uint32_t b,uint32_t c) { char t[64];snprintf(t,sizeof t,f,a,b,c);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned slot(uint32_t p) { CHECK(p>=MON&&p<MON+0x800&&!(p&0xff));return (p-MON)>>8; }
static uint32_t records,last,mode,chosen,turn_v,refl,sidev;
static uint32_t side_of(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("side%u ",id,0,0);return sidev; }
static const struct px_battle_ability_api base={&events,NULL,NULL,NULL,side_of,NULL,NULL};
static const struct px_battle_handler_api handlers={&memory,NULL,abort_battle,NULL,NULL,NULL,BATTLE,POOL,0,0,0,0,COUNTS};
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX&&records<8);notef("rec%x/%u ",k,id,0);return RECORD+0x40*records++; }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX);notef("commit%u ",(r-RECORD)/0x40,0,0); }
static void message(void *u,uint32_t at,uint32_t k,uint32_t m) { (void)u;notef("msg%x/%x ",m,(at-RECORD)*16+k,0); }
static void message_arg(void *u,uint32_t at,uint32_t v) { (void)u;notef("arg%x/%x ",v,at-RECORD,0); }
static uint32_t last_move(void *u,uint32_t p) { (void)u;notef("last%u ",slot(p),0,0);return last; }
static void condition_for(void *u,uint32_t s,uint32_t p,uint32_t out) { (void)u;notef("cond%u/%u/%x ",s,slot(p),out-RECORD);w32(NULL,out,0xc0ffee); }
static uint32_t battle_mode(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("mode ");return mode; }
static uint32_t target_selector(void *u,uint32_t m,uint32_t s,uint32_t z) { (void)u;notef("sel%x/%x/%u ",m,s,z);return chosen; }
static uint32_t turn(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("turn ");return turn_v; }
static uint32_t reflection_blocked(void *u,uint32_t p) { (void)u;notef("refl%u ",slot(p),0,0);return refl; }
static const struct px_battle_moves_14_api api={&base,&handlers,action_record,commit,message,message_arg,last_move,condition_for,
    battle_mode,target_selector,turn,reflection_blocked,PROTECT};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_14_api*);
static const fn dispatched[]={px_battle_uproar_fail,px_battle_teleport};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w) { (void)u;CHECK(f<2);dispatched[f](h,c,v,w,&api); }
static const uint16_t protect[8]={182,197,203,469,501,596,588,561};
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;log_[0]=0;records=last=mode=chosen=turn_v=refl=sidev=0;
    for(unsigned i=0;i<8;++i){ memset(mem+RECORD+0x40*i,0xa5,0x40); }
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i){ w32(NULL,TABLE+0x84+4*i,MON+0x100*i);w8(NULL,MON+0x100*i+0x19,(uint8_t)(i==ATT?5:i)); }
    for(unsigned i=0;i<PX_BATTLE_HANDLER_POOL;++i)w32(NULL,POOL+4*i,NODES+0x40*i);
    for(unsigned i=0;i<8;++i) w16(NULL,PROTECT+2*i,protect[i]);
}
/* Keys: 0x02 battler, 0x03 attacker (ids); 0x0c, 0x0d target, 0x12, 0x42, 0x53 (values; 0x0d is rewritten). */
static uint32_t kind_v=1,move_v=0x99,target_v=6;
static void frame(uint32_t battler,uint32_t attacker)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x02,battler,&events);px_battle_event_set_id(0x03,attacker,&events);
    px_battle_event_set_value(0x0d,target_v,&events);px_battle_event_set_value(0x0c,kind_v,&events);px_battle_event_set_value(0x12,move_v,&events);
    px_battle_event_set_value(0x42,0,&events);px_battle_event_set_value(0x53,0,&events);log_[0]=0;
}
static uint32_t key(uint32_t k) { return px_battle_event_get(k,&events); }
static void run(fn f) { frame(ATT,ATT);f(HANDLER,CTX,ATT,WORK,&api);CHECK(aborts==0); }
static int same(const char *expected) { if(strcmp(log_,expected)){ fprintf(stderr,"log [%s] expected [%s]\n",log_,expected);return 0; } return 1; }
#define R(n) (RECORD+0x40*(n))
#define W(i) r32(NULL,WORK+4*(i))
/* Five handlers of owner 0 (values 5, 5, 3, 5, 5; moves 37, 33, 37, 37, 37 unless given): which are removed. */
static uint32_t hs[5];
static void register5(uint32_t value,uint32_t move)
{
    const uint32_t vals[5]={value,value,3,value,value},ids[5]={move,33,move,move,move},owners[5]={0,0,0,4,0};
    for(unsigned i=0;i<5;++i) hs[i]=px_battle_handler_register(owners[i],ids[i],1,i,vals[i],ENTRIES,1,&handlers);
    w16(NULL,HANDLER+0x38,(uint16_t)move);
}
static unsigned removed(void) { unsigned m=0;for(unsigned i=0;i<5;++i) { if(r32(NULL,hs[i]+0x18)==0) m|=1u<<i; } return m; }
int main(void)
{
    unsigned cases=0;
    /* Nothing for another battler (key 0x02 or 0x03 as each tests). */
    {
        const fn by2[]={px_battle_rage_end,px_battle_destiny_bond_end,px_battle_charge_end,px_battle_uproar_interrupted,px_battle_thrash_turn,
                        px_battle_rollout_interrupted,px_battle_snatch_taken,px_battle_magic_coat_reflect};
        const fn by3[]={px_battle_charge_check,px_battle_protect_streak,px_battle_future_sight_set,px_battle_doom_desire_set,
                        px_battle_future_sight_message,px_battle_doom_desire_message};
        for(unsigned k=0;k<8;++k)for(uint32_t o=0;o<2;++o){ reset();w32(NULL,WORK,2);w32(NULL,WORK+0x18,1);register5(5,37);
            frame(o?0x102:3,ATT);by2[k](HANDLER,CTX,ATT,WORK,&api);
            CHECK(log_[0]==0&&records==0&&W(0)==2&&W(6)==1&&removed()==0&&key(0x53)==0);++cases; }
        for(unsigned k=0;k<6;++k)for(uint32_t o=0;o<2;++o){ reset();w32(NULL,WORK,1);frame(ATT,o?0x102:3);by3[k](HANDLER,CTX,ATT,WORK,&api);
            CHECK(log_[0]==0&&records==0&&W(0)==1&&key(0x53)==0);++cases; }
    }
    /* Rage: work word 6, the action kind, and removal by the user's value byte (5). */
    for(uint32_t a=0;a<2;++a)for(uint32_t k=0;k<3;++k){
        reset();w32(NULL,WORK+0x18,a?0x100:0);kind_v=k==0?8:k==1?0x108:1;register5(5,99);run(px_battle_rage_end);
        CHECK(removed()==(a&&k?0x11u:0u)&&log_[0]==0);++cases;
    }
    kind_v=1;
    /* Destiny Bond: its own handler only. */
    for(uint32_t k=0;k<2;++k){
        reset();kind_v=k?8:9;register5(5,194);frame(ATT,ATT);px_battle_destiny_bond_end(hs[1],CTX,ATT,hs[1]+0x1c,&api);
        CHECK(removed()==(k?0u:2u));++cases;
    }
    kind_v=1;
    /* Charge: another move marks it; then its end handler removes it. */
    for(uint32_t m=0;m<3;++m){
        reset();move_v=m==0?268:m==1?0x10000+268:269;w16(NULL,HANDLER+0x38,268);w32(NULL,WORK,7);run(px_battle_charge_check);
        CHECK(W(0)==(m==0?7u:2u)&&log_[0]==0);++cases;
    }
    move_v=0x99;
    for(uint32_t w=0;w<4;++w){
        reset();register5(5,268);frame(ATT,ATT);w32(NULL,hs[0]+0x1c,w==0?2:w==1?0x102:w==2?1:0);px_battle_charge_end(hs[0],CTX,ATT,hs[0]+0x1c,&api);
        CHECK(removed()==(w==0?1u:0u));++cases;
    }
    /* Protect streak: every table move keeps it; others (and a table move + 0x10000) reset it. */
    for(unsigned i=0;i<12;++i){
        reset();last=i<8?protect[i]:i==8?0:i==9?0x10000u+182:i==10?470:561+1;run(px_battle_protect_streak);
        if(i<8) CHECK(same("last2 ")&&records==0);
        else CHECK(same("last2 rec27/2 commit0 ")&&mem[R(0)+4]==ATT&&mem[R(0)+5]==3&&mem[R(0)+6]==0&&mem[R(0)+7]==0xa5);
        ++cases;
    }
    /* A changed table is followed. */
    reset();w16(NULL,PROTECT+14,999);last=999;run(px_battle_protect_streak);CHECK(records==0);++cases;
    reset();last=561;w16(NULL,PROTECT+14,999);run(px_battle_protect_streak);CHECK(records==1);++cases;
    /* Uproar interrupted. */
    for(uint32_t d=0;d<2;++d){
        reset();frame(ATT,ATT);if(d) px_battle_event_rewrite(0x53,1,&events);log_[0]=0;px_battle_uproar_interrupted(HANDLER,CTX,ATT,WORK,&api);
        if(d) CHECK(log_[0]==0);
        else CHECK(same("reca/2 commit0 ")&&mem[R(0)+4]==0x19&&mem[R(0)+5]==ATT&&mem[R(0)+0x11]==1&&mem[R(0)+6]==0xa5);
        ++cases;
    }
    /* Thrash: active flag, turns left (0, 1, 2, 0x100), interrupted or not. */
    for(uint32_t a=0;a<2;++a)for(uint32_t t=0;t<4;++t)for(uint32_t d=0;d<2;++d){
        reset();register5(5,37);w32(NULL,WORK+0x18,a?0x10000:0);const uint32_t turns[4]={0,1,2,0x100};w32(NULL,WORK,turns[t]);
        frame(ATT,ATT);if(d) px_battle_event_rewrite(0x53,1,&events);log_[0]=0;px_battle_thrash_turn(HANDLER,CTX,ATT,WORK,&api);
        CHECK(aborts==0);
        if(!a){ CHECK(log_[0]==0&&W(0)==turns[t]&&removed()==0); ++cases; continue; }
        const uint32_t left=turns[t]?turns[t]-1:0;CHECK(W(0)==left);
        char e[256];
        snprintf(e,sizeof e,"%s%s",d?"":"reca/2 commit0 ",left==0?(d?"recb/2 cond6/2/8 msg168/142 arg2/14 commit0 ":"recb/2 cond6/2/48 msg168/542 arg2/54 commit1 "):"");
        CHECK(same(e));
        CHECK(removed()==(left==0||!d?0x11u:0u));
        if(left==0){ const uint32_t r=R(d?0:1);CHECK(r16(NULL,r+4)==6&&r32(NULL,r+8)==0xc0ffee&&mem[r+0xf]==ATT&&mem[r+0xd]==1&&mem[r+0xe]==0xa5&&mem[r+0xc]==0xa5); }
        if(!d) CHECK(mem[R(0)+4]==0x19&&mem[R(0)+5]==ATT&&mem[R(0)+0x11]==1);
        ++cases;
    }
    /* Thrash looks the user up before testing work word 6: a missing battler aborts even when inactive. */
    reset();w32(NULL,TABLE+0x84+4*ATT,0);frame(ATT,ATT);px_battle_thrash_turn(HANDLER,CTX,ATT,WORK,&api);CHECK(aborts>=1);++cases;
    /* Rollout interrupted: removal by the battler id itself (2), not its value byte. */
    for(uint32_t d=0;d<2;++d){
        reset();register5(ATT,205);w32(NULL,WORK+0x18,1);frame(ATT,ATT);if(d) px_battle_event_rewrite(0x53,1,&events);log_[0]=0;
        px_battle_rollout_interrupted(HANDLER,CTX,ATT,WORK,&api);
        if(d) CHECK(log_[0]==0&&W(6)==1&&removed()==0);
        else CHECK(same("reca/2 commit0 ")&&W(6)==0&&removed()==0x11&&mem[R(0)+4]==0x19&&mem[R(0)+5]==ATT&&mem[R(0)+0x11]==1);
        ++cases;
    }
    reset();register5(5,205);w32(NULL,WORK+0x18,1);run(px_battle_rollout_interrupted);CHECK(removed()==0);++cases;
    /* Future Sight and Doom Desire: announced already, a given target, or none (0xa) chosen. */
    for(unsigned f=0;f<2;++f)for(uint32_t t=0;t<3;++t)for(uint32_t d=0;d<2;++d){
        reset();target_v=t==0?6:t==1?0xa:0x10a;sidev=0x1234;mode=0x77;chosen=0x105;turn_v=0xfffffffeu+t;w16(NULL,HANDLER+0x38,f?353:248);
        frame(ATT,ATT);if(d) px_battle_event_rewrite(0x53,9,&events);log_[0]=0;
        (f?px_battle_doom_desire_set:px_battle_future_sight_set)(HANDLER,CTX,ATT,WORK,&api);
        if(d){ CHECK(log_[0]==0&&key(0x53)==9);++cases;continue; }
        const int none=t!=0;
        CHECK(key(0x53)==1&&key(0x0d)==(none?0x105u:6u));
        CHECK(same(none?"side2 mode sel77/1234/0 rec1f/2 turn commit0 ":"side2 rec1f/2 turn commit0 "));
        CHECK(mem[R(0)+5]==(none?5:6)&&mem[R(0)+4]==3&&r32(NULL,R(0)+8)==turn_v+2&&r32(NULL,R(0)+0xc)==(f?353u:248u)&&mem[R(0)+0x18]==3&&r32(NULL,R(0)+0x10)==0x1234);
        CHECK(mem[R(0)+6]==0xa5&&mem[R(0)+0x19]==0xa5);
        ++cases;
    }
    target_v=6;
    /* Their messages. */
    reset();run(px_battle_future_sight_message);CHECK(same("rec3/2 msg438/42 arg2/4 commit0 "));++cases;
    reset();run(px_battle_doom_desire_message);CHECK(same("rec3/2 msg43b/42 arg2/4 commit0 "));++cases;
    /* Snatch. */
    for(uint32_t w=0;w<2;++w){
        reset();register5(5,289);frame(ATT,3);
        w32(NULL,hs[3]+0x1c,w?0x100:0);log_[0]=0;px_battle_snatch_taken(hs[3],CTX,ATT,hs[3]+0x1c,&api);
        if(w) CHECK(same("rec3/2 msg2f2/42 arg2/4 arg3/4 commit0 ")&&removed()==8);
        else CHECK(log_[0]==0&&removed()==0);
        ++cases;
    }
    /* Magic Coat: key 0x53 taken already, blocked, or reflected. */
    for(uint32_t d=0;d<2;++d)for(uint32_t b=0;b<2;++b){
        reset();refl=b?0x100:0;move_v=0x1234;frame(ATT,ATT);if(d) px_battle_event_rewrite(0x53,1,&events);log_[0]=0;
        px_battle_magic_coat_reflect(HANDLER,CTX,ATT,WORK,&api);
        if(d) CHECK(log_[0]==0);
        else if(b) CHECK(same("refl2 ")&&key(0x53)==1);
        else CHECK(same("refl2 rec3/2 msg2fc/42 arg2/4 arg1234/4 commit0 "));
        ++cases;
    }
    move_v=0x99;
    /* Teleport (no key test). */
    for(uint32_t d=0;d<2;++d){
        reset();frame(3,3);if(d) px_battle_event_rewrite(0x53,1,&events);log_[0]=0;px_battle_teleport(HANDLER,CTX,ATT,WORK,&api);
        CHECK(d?log_[0]==0:same("rec3/2 msg2ff/42 arg2/4 commit0 "));++cases;
    }
    /* Uproar fails while started. */
    for(uint32_t w=0;w<3;++w){
        reset();w32(NULL,WORK+0x18,w==0?0:w==1?1:0x80000000u);frame(3,3);px_battle_uproar_fail(HANDLER,CTX,ATT,WORK,&api);
        CHECK(key(0x42)==(w?1u:0u)&&log_[0]==0);++cases;
    }
    /* Through the dispatcher on event 0x0e: Uproar's check for battler 2 (started) and Teleport for battler 3. */
    {
        reset();
        for(unsigned i=0;i<2;++i){ w8(NULL,ENTRIES+0x10*i,0x0e);w32(NULL,ENTRIES+0x10*i+4,i); }
        const uint8_t ids[2]={ATT,3};
        for(unsigned i=0;i<2;++i) {
            const uint32_t hn=NODES+0x40*i;
            w32(NULL,hn+8,ENTRIES+0x10*i);w32(NULL,hn+0x18,0x20000000u|1u<<16);w8(NULL,hn+0x3a,ids[i]);
            w32(NULL,hn,i?hn-0x40:0);w32(NULL,hn+4,i<1?hn+0x40:0);
        }
        w32(NULL,NODES+0x1c+0x18,1);w32(NULL,BATTLE+0x60,NODES);frame(ATT,ATT);px_battle_event_dispatch(CTX,0x0e,&events);
        CHECK(same("rec3/3 msg2ff/42 arg3/4 commit0 ")&&key(0x42)==1&&key(0x53)==1&&aborts==0);++cases;
    }
    printf("battle_moves_14: %u cases, all tests passed\n",cases);
    return 0;
}
