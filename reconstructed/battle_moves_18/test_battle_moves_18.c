#include "battle_moves_18.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, ENTRIES=0x7900, COUNTS=0x7a00, NODES=0x8000 };
enum { ATT=2 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static unsigned aborts;static char log_[4096];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b,uint32_t c) { char t[64];snprintf(t,sizeof t,f,a,b,c);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned slot(uint32_t p) { CHECK(p>=MON&&p<MON+0x800&&!(p&0xff));return (p-MON)>>8; }
static uint32_t records,nohp,status_v,ally,weather_v,cnt,can,claim,owner,pos,turns_v;
static uint32_t side_of(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("side%u ",id,0,0);return 0x30+id; }
static const struct px_battle_ability_api base={&events,NULL,NULL,NULL,side_of,NULL,NULL};
static const struct px_battle_handler_api handlers={&memory,NULL,abort_battle,NULL,NULL,NULL,BATTLE,POOL,0,0,0,0,COUNTS};
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX&&records<8);notef("rec%x/%u ",k,id,0);return RECORD+0x40*records++; }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX);notef("commit%u ",(r-RECORD)/0x40,0,0); }
static void message(void *u,uint32_t at,uint32_t k,uint32_t m) { (void)u;notef("msg%x/%x ",m,(at-RECORD)*16+k,0); }
static void message_arg(void *u,uint32_t at,uint32_t v) { (void)u;(void)at;notef("arg%x ",v,0,0); }
static uint32_t no_hp(void *u,uint32_t p) { (void)u;notef("nohp%u ",slot(p),0,0);return nohp; }
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;notef("st%u/%x ",slot(p),s,0);return status_v; }
static uint32_t is_ally(void *u,uint32_t id,uint32_t o) { (void)u;notef("ally%u/%u ",id,o,0);return ally; }
static uint32_t weather(void *u,uint32_t c) { (void)u;CHECK(c==CTX);return weather_v; }
static uint32_t counter(void *u,uint32_t p,uint32_t i) { (void)u;notef("cnt%u/%u ",slot(p),i,0);return cnt; }
static uint32_t can_switch(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("cs%u ",id,0,0);return can; }
static uint32_t claim_switch(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("claim ");return claim; }
static uint32_t owner_of(void *u,uint32_t id) { (void)u;notef("own%u ",id,0,0);return owner; }
static uint32_t side_id(void *u,uint32_t id) { (void)u;notef("sid%u ",id,0,0);return 0x40+(id&0xff); }
static uint32_t position(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("pos%u ",id,0,0);return pos; }
static uint32_t turn_word(void *u,uint32_t t) { (void)u;notef("tw%u ",t,0,0);return turns_v; }
static const struct px_battle_moves_18_api api={&base,&handlers,action_record,commit,message,message_arg,no_hp,has_status,is_ally,weather,counter,
    can_switch,claim_switch,owner_of,side_id,position,turn_word};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_18_api*);
static const fn dispatched[]={px_battle_sun_ready,px_battle_support_boost};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w) { (void)u;CHECK(f<2);dispatched[f](h,c,v,w,&api); }
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;log_[0]=0;records=nohp=status_v=ally=weather_v=cnt=can=claim=owner=pos=turns_v=0;
    for(unsigned i=0;i<8;++i){ memset(mem+RECORD+0x40*i,0xa5,0x40); }
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i){ w32(NULL,TABLE+0x84+4*i,MON+0x100*i);w8(NULL,MON+0x100*i+0x19,(uint8_t)(0x10+i)); }
    for(unsigned i=0;i<PX_BATTLE_HANDLER_POOL;++i)w32(NULL,POOL+4*i,NODES+0x40*i);
}
/* Keys: 0x02 battler, 0x03 attacker, 0x04 defender, 0x06 target (ids); 0x36, 0x3b, 0x42, 0x53 values. */
static uint32_t done_v=0;
static void frame(uint32_t battler,uint32_t attacker,uint32_t defender,uint32_t target)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x02,battler,&events);px_battle_event_set_id(0x03,attacker,&events);
    px_battle_event_set_id(0x04,defender,&events);px_battle_event_set_id(0x06,target,&events);
    px_battle_event_set_value(0x36,0x1000,&events);px_battle_event_set_value(0x3b,0,&events);px_battle_event_set_value(0x42,0,&events);
    px_battle_event_set_value(0x53,done_v,&events);log_[0]=0;
}
static uint32_t key(uint32_t k) { return px_battle_event_get(k,&events); }
static int same(const char *expected) { if(strcmp(log_,expected)){ fprintf(stderr,"log [%s] expected [%s]\n",log_,expected);return 0; } return 1; }
#define R(n) (RECORD+0x40*(n))
#define W(i) r32(NULL,WORK+4*(i))
/* Five handlers of owner 0 and value byte 0x12 (battler 2's): moves 37, 33, 37, 37 (owner 4), 37 (other value): which are removed. */
static uint32_t hs[5];
static void register5(uint32_t move)
{
    const uint32_t vals[5]={0x12,0x12,0x12,0x12,0x13},ids[5]={move,33,move,move,move},owners[5]={0,0,0,4,0};
    for(unsigned i=0;i<5;++i) hs[i]=px_battle_handler_register(owners[i],ids[i],1,i,vals[i],ENTRIES,1,&handlers);
    w16(NULL,HANDLER+0x38,(uint16_t)move);
}
static unsigned removed(void) { unsigned m=0;for(unsigned i=0;i<5;++i) { if(r32(NULL,hs[i]+0x18)==0) m|=1u<<i; } return m; }
int main(void)
{
    unsigned cases=0;
    /* Nothing for another battler (the key each handler tests). */
    {
        const fn by2[]={px_battle_uproar_end,px_battle_sleep_wake};
        for(unsigned k=0;k<2;++k)for(uint32_t o=0;o<2;++o){ reset();w32(NULL,WORK+0x18,1);register5(37);frame(o?0x102:3,5,5,5);by2[k](HANDLER,CTX,ATT,WORK,&api);
            CHECK(log_[0]==0&&records==0&&removed()==0&&aborts==0);++cases; }
        const fn by3[]={px_battle_support_boost,px_battle_item_flag,px_battle_fling_start,px_battle_pledge_effect,px_battle_switch_out,
                        px_battle_weather_power,px_battle_stockpile_power,px_battle_ally_target,px_battle_sun_ready,px_battle_form_check};
        for(unsigned k=0;k<10;++k)for(uint32_t o=0;o<2;++o){ reset();done_v=1;weather_v=1;w32(NULL,WORK,1);w32(NULL,WORK+4,1);w16(NULL,MON+0x200+0x12,5);w16(NULL,MON+0x200+0xc,0x288);
            can=claim=1;frame(5,o?0x102:3,5,5);by3[k](HANDLER,CTX,ATT,WORK,&api);done_v=0;
            CHECK(strstr(log_,"rec")==NULL&&key(0x36)==0x1000&&key(0x3b)==0&&key(0x42)==0&&aborts==0);
            if(k==1) { CHECK(W(0)==1); }
            ++cases; }
        for(uint32_t o=0;o<2;++o){ reset();w32(NULL,WORK+0x18,1);done_v=1;frame(5,5,o?0x102:3,5);px_battle_flagged_boost(HANDLER,CTX,ATT,WORK,&api);done_v=0;CHECK(key(0x3b)==0);++cases; }
    }
    /* Key 0x53 gates, and the flagged boost. */
    for(uint32_t d=0;d<2;++d){ reset();done_v=d;frame(5,ATT,5,5);px_battle_support_boost(HANDLER,CTX,ATT,WORK,&api);done_v=0;CHECK(key(0x3b)==(d?2u:0u));++cases; }
    for(uint32_t d=0;d<2;++d)for(uint32_t w=0;w<2;++w){ reset();done_v=d;w32(NULL,WORK+0x18,w?0x100:0);frame(5,5,ATT,5);px_battle_flagged_boost(HANDLER,CTX,ATT,WORK,&api);done_v=0;
        CHECK(key(0x3b)==(d&&w?1u:0u));++cases; }
    /* Uproar's end. */
    for(uint32_t w=0;w<2;++w)for(uint32_t h=0;h<2;++h)for(uint32_t l=0;l<2;++l){
        reset();w32(NULL,WORK+0x18,w?0x100:0);nohp=h;status_v=l?0x200:0;register5(253);frame(ATT,5,5,5);px_battle_uproar_end(HANDLER,CTX,ATT,WORK,&api);
        if(!w) CHECK(log_[0]==0);
        else if(h) CHECK(same("nohp2 "));
        else {
            char e[160];snprintf(e,sizeof e,"nohp2 st2/19 rec3/2 msg%x/42 arg2 commit0 ",l?0x2cbu:0x2ceu);
            CHECK(same(e));CHECK(removed()==(l?0u:5u));
        }
        ++cases; }
    /* Sleep wake. */
    for(uint32_t s=0;s<2;++s){
        reset();status_v=s?1:0;register5(37);frame(ATT,5,5,5);px_battle_sleep_wake(HANDLER,CTX,ATT,WORK,&api);
        if(!s) CHECK(same("st2/2 ")&&removed()==0);
        else CHECK(same("st2/2 reca/2 commit0 ")&&mem[R(0)+4]==0x19&&mem[R(0)+5]==ATT&&mem[R(0)+0x11]==1&&removed()==5);
        ++cases; }
    /* Item flag and Fling's start. */
    for(uint32_t i=0;i<3;++i){ reset();w32(NULL,WORK,9);w16(NULL,MON+0x200+0x12,(uint16_t)(i==0?0:i==1?1:0x8000));frame(5,ATT,5,5);px_battle_item_flag(HANDLER,CTX,ATT,WORK,&api);CHECK(W(0)==(i?1u:0u));++cases; }
    for(uint32_t i=0;i<2;++i){ reset();w16(NULL,MON+0x200+0x12,i?0x1234:0);frame(5,ATT,5,5);px_battle_fling_start(HANDLER,CTX,ATT,WORK,&api);
        if(!i) CHECK(log_[0]==0);
        else CHECK(same("rec3/2 msg30b/42 arg2 arg1234 commit0 rec15/2 commit1 ")&&mem[R(1)+4]==0xf&&mem[R(1)+5]==ATT&&mem[R(1)+6]==0xa5);
        ++cases; }
    /* Move 547: form and flag byte. */
    for(uint32_t f=0;f<3;++f)for(uint32_t b=0;b<3;++b){
        reset();w16(NULL,MON+0x200+0xc,(uint16_t)(f==0?0x288:f==1?0x289:0x188));w8(NULL,MON+0x200+0x153,(uint8_t)(b==0?0:b==1?1:0x80));frame(5,ATT,5,5);
        px_battle_form_check(HANDLER,CTX,ATT,WORK,&api);
        if(f) CHECK(log_[0]==0);
        else CHECK(same("rec3a/2 msgde/82 arg2 commit0 ")&&mem[R(0)+4]==ATT&&mem[R(0)+5]==(b?0u:1u)&&mem[R(0)+6]==0xa5);
        ++cases; }
    /* Pledges: the work words, the forms, the target and the position. */
    for(uint32_t w=0;w<2;++w)for(uint32_t f=0;f<5;++f)for(uint32_t t=0;t<2;++t)for(uint32_t p=0;p<3;++p){
        reset();w32(NULL,WORK,w?0x100:0);w32(NULL,WORK+4,f);turns_v=0x1234abcd;pos=p==0?0xa:p==1?3:0x105;frame(5,ATT,5,t?0x1f:0x105);
        px_battle_pledge_effect(HANDLER,CTX,ATT,WORK,&api);CHECK(aborts==0);
        if(!w||f==0||f>3) { CHECK(strstr(log_,"rec")==NULL); ++cases; continue; }
        const uint32_t sid=f==1?0x42u:(t?0x5fu:0x45u),kind=f==1?0xbu:f==2?0xcu:0xdu,txt=f==1?0xaau:f==2?0xaeu:0xb2u,hw=f==1?0x27bu:f==2?0x27cu:0x27du;
        const uint32_t tg=f==1?ATT:(t?0x1fu:5u);
        char e[240];
        snprintf(e,sizeof e,"sid2 sid%u rec19/2 tw4 msg%x/%x arg%x commit0 ",t?0x1fu:5u,txt,0x10*16+1,sid);
        char tail[80]="";
        if(tg!=0x1f) { snprintf(tail,sizeof tail,"pos%u ",tg);strcat(e,tail);if(pos!=0xa) { snprintf(tail,sizeof tail,"rec38/2 commit1 ");strcat(e,tail); } }
        CHECK(same(e));
        CHECK(mem[R(0)+4]==kind&&mem[R(0)+0xc]==sid&&r32(NULL,R(0)+8)==0x1234abcd&&mem[R(0)+5]==0xa5);
        if(tg!=0x1f&&pos!=0xa) CHECK(mem[R(1)+7]==0xa&&mem[R(1)+6]==(pos&0xff)&&r16(NULL,R(1)+4)==hw);
        ++cases; }
    /* Switch moves: both checks, and the two halfwords. */
    for(uint32_t c=0;c<2;++c)for(uint32_t l=0;l<2;++l)for(uint32_t m=0;m<3;++m){
        reset();can=c;claim=l;owner=0x77;w16(NULL,HANDLER+0x38,(uint16_t)(m==0?369:m==1?521:575));w32(NULL,R(1),0x00000f0f);frame(5,ATT,5,5);
        px_battle_switch_out(HANDLER,CTX,ATT,WORK,&api);
        if(!c) CHECK(same("cs2 "));
        else if(!l) CHECK(same("cs2 claim "));
        else {
            CHECK(same("cs2 claim own2 rec2a/2 msg302/42 arg2 arg77 commit0 rec38/2 side2 commit1 "));
            CHECK(mem[R(0)+0x4c]==ATT&&r16(NULL,R(1)+4)==(m==0?0x2bfu:0x2c0u)&&mem[R(1)+6]==0x32&&mem[R(1)+7]==0xa&&r32(NULL,R(1))==(0x00000f0fu|0x1000000u));
            CHECK((r32(NULL,R(1))&0xff000000u)==0x01000000u);
        }
        ++cases; }
    /* Weather power. */
    for(uint32_t w=0;w<7;++w){ reset();weather_v=w;frame(5,ATT,5,5);px_battle_weather_power(HANDLER,CTX,ATT,WORK,&api);
        CHECK(key(0x36)==(w==1?0xaacu:(w>=2&&w<=4)?0x400u:0x800u));++cases; }
    for(uint32_t c=0;c<5;++c){ reset();cnt=c;frame(5,ATT,5,5);px_battle_stockpile_power(HANDLER,CTX,ATT,WORK,&api);
        CHECK(same("cnt2/0 ")&&key(0x36)==(c==2?0x800u:c==3?0x1000u:0x400u));++cases; }
    /* Move 507. */
    for(uint32_t t=0;t<3;++t)for(uint32_t a=0;a<2;++a){ reset();ally=a;frame(5,ATT,t==0?5:t==1?0x1f:0x11f,5);px_battle_ally_target(HANDLER,CTX,ATT,WORK,&api);
        const uint32_t tg=t==0?5u:t==1?0x1fu:0x1fu;
        if(tg==0x1f) CHECK(log_[0]==0&&key(0x42)==0);
        else CHECK(same("ally2/5 ")&&key(0x42)==(a?1u:0u));
        ++cases; }
    /* Move 76. */
    for(uint32_t w=0;w<3;++w){ reset();weather_v=w;frame(5,ATT,5,5);px_battle_sun_ready(HANDLER,CTX,ATT,WORK,&api);CHECK(key(0x53)==(w==1?1u:0u));++cases; }
    /* Through the dispatcher on event 0x7d: move 206's boost and a sun-ready handler (no relation, keys as given). */
    {
        reset();
        for(unsigned i=0;i<2;++i){ w8(NULL,ENTRIES+0x10*i,0x7d);w32(NULL,ENTRIES+0x10*i+4,i?1:0); }
        for(unsigned i=0;i<2;++i) {
            const uint32_t hn=NODES+0x40*i;
            w32(NULL,hn+8,ENTRIES+0x10*i);w32(NULL,hn+0x18,0x20000000u|1u<<16);w8(NULL,hn+0x3a,ATT);
            w32(NULL,hn,i?hn-0x40:0);w32(NULL,hn+4,i<1?hn+0x40:0);
        }
        w32(NULL,BATTLE+0x60,NODES);weather_v=1;frame(5,ATT,5,5);px_battle_event_dispatch(CTX,0x7d,&events);
        CHECK(key(0x53)==1&&key(0x3b)==2&&aborts==0);++cases;
    }
    printf("battle_moves_18: %u cases, all tests passed\n",cases);
    return 0;
}
