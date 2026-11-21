#include "battle_moves_15.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, ENTRIES=0x7900, NODES=0x8000 };
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
static uint32_t records,state,flag,status,counters[3];
static uint32_t side_of(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("side%u ",id,0,0);return 0x1301; }
static const struct px_battle_ability_api base={&events,NULL,NULL,NULL,side_of,NULL,NULL};
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX&&records<8);notef("rec%x/%u ",k,id,0);return RECORD+0x40*records++; }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX);notef("commit%u ",(r-RECORD)/0x40,0,0); }
static void message(void *u,uint32_t at,uint32_t k,uint32_t m) { (void)u;notef("msg%x/%x ",m,(at-RECORD)*16+k,0); }
static void message_arg(void *u,uint32_t at,uint32_t v) { (void)u;notef("arg%x/%x ",v,at-RECORD,0); }
static uint32_t state_check(void *u,uint32_t p) { (void)u;notef("chk%u ",slot(p),0,0);return state; }
static uint32_t move_flag(void *u,uint32_t m,uint32_t f) { (void)u;notef("flag%x/%u ",m,f,0);return flag; }
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;notef("st%u/%x ",slot(p),s,0);return status; }
static uint32_t lock_count(void *u,uint32_t a,uint32_t b) { (void)u;notef("lc%u/%u ",a,b,0);return 0x13572468; }
static void condition_flag(void *u,uint32_t at,uint32_t v) { (void)u;notef("cf%x/%u ",at-RECORD,v,0); }
static uint32_t counter(void *u,uint32_t p,uint32_t i) { (void)u;CHECK(i<3);notef("cnt%u/%u ",slot(p),i,0);return counters[i]; }
static const struct px_battle_moves_15_api api={&base,action_record,commit,message,message_arg,state_check,move_flag,has_status,lock_count,
    condition_flag,counter};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_15_api*);
static const fn dispatched[]={px_battle_explosion_done,px_battle_natural_gift_item};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w) { (void)u;CHECK(f<2);dispatched[f](h,c,v,w,&api); }
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;log_[0]=0;records=state=flag=status=0;counters[0]=counters[1]=counters[2]=0;
    for(unsigned i=0;i<8;++i){ memset(mem+RECORD+0x40*i,0xa5,0x40); }
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
/* Keys: 0x02 battler and 0x03 attacker as ids (0x02 and 0x04 as values for Snatch); 0x12, 0x23, 0x42, 0x53 values. */
static uint32_t move_v=0x99,state_v=0;
static void frame(uint32_t battler,uint32_t attacker)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x02,battler,&events);px_battle_event_set_id(0x03,attacker,&events);
    px_battle_event_set_value(0x12,move_v,&events);px_battle_event_set_value(0x23,state_v,&events);
    px_battle_event_set_value(0x42,0,&events);px_battle_event_set_value(0x53,0,&events);log_[0]=0;
}
static void frame_snatch(uint32_t attacker)
{
    px_battle_event_push(&events);px_battle_event_set_value(0x02,5,&events);px_battle_event_set_id(0x03,attacker,&events);
    px_battle_event_set_value(0x04,6,&events);px_battle_event_set_value(0x12,move_v,&events);log_[0]=0;
}
static uint32_t key(uint32_t k) { return px_battle_event_get(k,&events); }
static void run(fn f) { frame(ATT,ATT);f(HANDLER,CTX,ATT,WORK,&api);CHECK(aborts==0); }
static int same(const char *expected) { if(strcmp(log_,expected)){ fprintf(stderr,"log [%s] expected [%s]\n",log_,expected);return 0; } return 1; }
#define R(n) (RECORD+0x40*(n))
#define W(i) r32(NULL,WORK+4*(i))
int main(void)
{
    unsigned cases=0;
    /* Nothing for another battler (key 0x02 or 0x03 as each tests). */
    {
        const fn by2[]={px_battle_focus_punch_start,px_battle_snore_awake,px_battle_roost_land,px_battle_perish_song_message,
                        px_battle_stockpile_release,px_battle_fling_item,px_battle_natural_gift_item,px_battle_explosion_done};
        const fn by3[]={px_battle_support_done,px_battle_struggle_message};
        for(unsigned k=0;k<8;++k)for(uint32_t o=0;o<2;++o){ reset();counters[0]=counters[1]=counters[2]=1;state_v=2;w16(NULL,MON+0x200+0x12,5);
            frame(o?0x102:3,ATT);by2[k](HANDLER,CTX,ATT,WORK,&api);
            CHECK(log_[0]==0&&records==0&&key(0x42)==0&&aborts==0);++cases; }
        for(unsigned k=0;k<2;++k)for(uint32_t o=0;o<2;++o){ reset();frame(ATT,o?0x102:3);by3[k](HANDLER,CTX,ATT,WORK,&api);
            CHECK(log_[0]==0&&records==0&&key(0x53)==0);++cases; }
        state_v=0;
    }
    /* Focus Punch: with and without the state check. */
    for(uint32_t s=0;s<3;++s){
        reset();state=s==0?0:s==1?1:0x100;run(px_battle_focus_punch_start);
        if(s==0){ CHECK(same("rec15/2 commit0 chk2 rec38/2 side2 commit1 rec3/2 msg268/842 arg2/84 commit2 "));
            CHECK(r16(NULL,R(1)+4)==0x2ad&&mem[R(1)+6]==1&&mem[R(1)+7]==0xa&&mem[R(1)+0xb]==1&&mem[R(1)+8]==0xa5&&mem[R(1)+0xa]==0xa5); }
        else CHECK(same("rec15/2 commit0 chk2 rec3/2 msg268/442 arg2/44 commit1 "));
        CHECK(mem[R(0)+4]==5&&mem[R(0)+5]==ATT&&mem[R(0)+6]==0xa5);
        ++cases;
    }
    /* Snatch: the user's own move, the flag, status 0x21, and the steal. */
    for(uint32_t a=0;a<3;++a)for(uint32_t f=0;f<2;++f)for(uint32_t s=0;s<2;++s){
        reset();move_v=0x10000+0x123;flag=f?0x100:0;status=s?0x200:0;w32(NULL,WORK,7);
        frame_snatch(a==0?ATT:a==1?0x102:4);px_battle_snatch_steal(HANDLER,CTX,ATT,WORK,&api);
        CHECK(aborts==0);
        if(a<2) { CHECK(log_[0]==0&&W(0)==7&&key(0x02)==5); }
        else if(!f) { CHECK(same("flag123/5 ")&&W(0)==7); }
        else if(s) { CHECK(same("flag123/5 st2/21 ")&&W(0)==7&&key(0x02)==5&&key(0x04)==6); }
        else { CHECK(same("flag123/5 st2/21 ")&&W(0)==1&&key(0x02)==ATT&&key(0x04)==ATT); }
        ++cases;
    }
    move_v=0x99;
    /* Key 0x02 already rewritten: nothing more. */
    reset();flag=1;frame_snatch(4);px_battle_event_rewrite(0x02,9,&events);log_[0]=0;px_battle_snatch_steal(HANDLER,CTX,ATT,WORK,&api);
    CHECK(key(0x02)==9&&key(0x04)==6&&W(0)==0&&aborts==0);++cases;
    /* Support moves, Snore. */
    reset();run(px_battle_support_done);CHECK(key(0x53)==1&&log_[0]==0);++cases;
    for(uint32_t v=0;v<4;++v){ reset();const uint32_t vals[4]={2,0x102,1,3};state_v=vals[v];run(px_battle_snore_awake);CHECK(key(0x42)==(v<2?1u:0u));++cases; }
    state_v=0;
    /* Struggle, Roost, Perish Song. */
    reset();run(px_battle_struggle_message);CHECK(same("rec3/2 msg347/42 arg2/4 commit0 "));++cases;
    reset();run(px_battle_roost_land);
    CHECK(same("recb/2 lc1/2 cf8/1 commit0 "));
    CHECK(r16(NULL,R(0)+4)==0x18&&r32(NULL,R(0)+8)==0x13572468&&mem[R(0)+0xd]==1&&mem[R(0)+0xf]==ATT&&mem[R(0)+0xc]==0xa5&&mem[R(0)+0xe]==0xa5);++cases;
    reset();run(px_battle_perish_song_message);CHECK(same("rec3/2 msg7d/41 commit0 "));++cases;
    /* Spit Up and Swallow: every combination of the three counters, with signed stages. */
    for(unsigned c=0;c<27;++c){
        reset();const uint32_t vals[3]={0,1,3};counters[1]=vals[c%3];counters[2]=vals[c/3%3];counters[0]=vals[c/9];
        for(unsigned i=0;i<8;++i) { w8(NULL,R(i)+0x18,(uint8_t)(0x10*i+1)); }
        run(px_battle_stockpile_release);
        char e[512]="cnt2/1 cnt2/2 cnt2/0 ";unsigned n=0;
        for(unsigned s=0;s<2;++s){ if(!counters[1+s]) continue;
            char t[96];snprintf(t,sizeof t,"recd/2 commit%u rec27/2 commit%u ",n,n+1);strcat(e,t);
            const uint32_t r=R(n);CHECK(mem[r+4]==(s?4:2)&&mem[r+0x16]==1&&mem[r+0x17]==(uint8_t)(0u-counters[1+s])&&mem[r+0x18]==((0x10*n+1)|4)&&mem[r+0xc]==ATT);
            CHECK(mem[R(n+1)+4]==ATT&&mem[R(n+1)+5]==1+s&&mem[R(n+1)+6]==0);
            n+=2; }
        if(counters[0]){ char t[96];snprintf(t,sizeof t,"rec27/2 commit%u rec3/2 msg2d4/%x arg2/%x commit%u ",n,(0x40*(n+1)+4)*16+2,0x40*(n+1)+4,n+1);strcat(e,t);
            CHECK(mem[R(n)+4]==ATT&&mem[R(n)+5]==0&&mem[R(n)+6]==0); }
        CHECK(same(e));++cases;
    }
    /* Fling: work word 0 and the held item. */
    for(uint32_t w=0;w<2;++w)for(uint32_t i=0;i<3;++i){
        reset();w32(NULL,WORK,w?0x100:0);w16(NULL,MON+0x200+0x12,(uint16_t)(i==0?0:i==1?1:0x8000));run(px_battle_fling_item);
        if(w||i==0) CHECK(log_[0]==0);
        else CHECK(same("rec24/2 commit0 ")&&mem[R(0)+4]==1&&mem[R(0)+5]==1&&mem[R(0)+6]==0xa5);
        ++cases;
    }
    reset();w8(NULL,MON+0x200+0x11,1);w8(NULL,MON+0x200+0x14,1);run(px_battle_fling_item);CHECK(log_[0]==0);++cases;
    reset();run(px_battle_natural_gift_item);CHECK(same("rec24/2 commit0 ")&&mem[R(0)+4]==1&&mem[R(0)+5]==1);++cases;
    reset();run(px_battle_explosion_done);CHECK(same("rec12/2 commit0 ")&&mem[R(0)+4]==ATT&&mem[R(0)+5]==0xa5);++cases;
    /* Through the dispatcher on event 0x27: Explosion for battler 2 and Natural Gift for battlers 3 and 2. */
    {
        reset();
        for(unsigned i=0;i<3;++i){ w8(NULL,ENTRIES+0x10*i,0x27);w32(NULL,ENTRIES+0x10*i+4,i?1:0); }
        const uint8_t ids[3]={ATT,3,ATT};
        for(unsigned i=0;i<3;++i) {
            const uint32_t hn=NODES+0x40*i;
            w32(NULL,hn+8,ENTRIES+0x10*i);w32(NULL,hn+0x18,0x20000000u|1u<<16);w8(NULL,hn+0x3a,ids[i]);
            w32(NULL,hn,i?hn-0x40:0);w32(NULL,hn+4,i<2?hn+0x40:0);
        }
        w32(NULL,BATTLE+0x60,NODES);frame(ATT,ATT);px_battle_event_dispatch(CTX,0x27,&events);
        CHECK(same("rec12/2 commit0 rec24/2 commit1 ")&&aborts==0);++cases;
    }
    printf("battle_moves_15: %u cases, all tests passed\n",cases);
    return 0;
}
