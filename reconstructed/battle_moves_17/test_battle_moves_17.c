#include "battle_moves_17.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, SCRATCH=0x7800, TRI=0x7810, ENTRIES=0x7900, COUNTS=0x7a00, AREA=0x7c00, NODES=0x8000 };
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
static uint32_t records,burn,flam,nohp[8],blockedv,ally,status_v,pursue_ok,redirect,streak_v,rnd,sel_v;
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;(void)p;(void)k;return 0; }
static uint32_t side_of(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("side%u ",id,0,0);return 0x1301; }
static const struct px_battle_ability_api base={&events,pokemon_value,NULL,NULL,side_of,NULL,NULL};
static const struct px_battle_handler_api handlers={&memory,NULL,abort_battle,NULL,NULL,NULL,BATTLE,POOL,0,0,0,0,COUNTS};
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX&&records<8);notef("rec%x/%u ",k,id,0);return RECORD+0x40*records++; }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX);notef("commit%u ",(r-RECORD)/0x40,0,0); }
static void message(void *u,uint32_t at,uint32_t k,uint32_t m) { (void)u;notef("msg%x/%x ",m,(at<RECORD+0x400?(at-RECORD)*16:0x100000+at)+k,0); }
static void message_arg(void *u,uint32_t at,uint32_t v) { (void)u;(void)at;notef("arg%x ",v,0,0); }
static uint32_t item_burnable(void *u,uint32_t i) { (void)u;notef("burn%x ",i,0,0);return burn; }
static uint32_t item_flammable(void *u,uint32_t i) { (void)u;notef("flam%x ",i,0,0);return flam; }
static uint32_t no_hp(void *u,uint32_t p) { (void)u;notef("nohp%u ",slot(p),0,0);return nohp[slot(p)]; }
static uint32_t disable_blocked(void *u,uint32_t m) { (void)u;notef("db%x ",m,0,0);return blockedv; }
static uint32_t is_ally(void *u,uint32_t id,uint32_t o) { (void)u;notef("ally%u/%u ",id,o,0);return ally; }
static void note_handler(void *u,uint32_t h) { (void)u;notef("noteh%x ",h,0,0); }
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;notef("st%u/%x ",slot(p),s,0);return status_v>>(s==0x12?0:1)&1; }
static uint32_t side_id(void *u,uint32_t id) { (void)u;notef("sid%u ",id,0,0);return 0x40+id; }
static uint32_t can_pursue(void *u,uint32_t c,uint32_t id,uint32_t out) { (void)u;CHECK(c==CTX&&out==SCRATCH);notef("cp%u ",id,0,0);return pursue_ok; }
static uint32_t redirect_blocked(void *u,uint32_t c,uint32_t a,uint32_t id,uint32_t m) { (void)u;CHECK(c==CTX);notef("rb%u/%u/%u ",a,id,m);return redirect; }
static void pursue(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("pursue%u ",id,0,0); }
static uint32_t streak(void *u,uint32_t p) { (void)u;notef("streak%u ",slot(p),0,0);return streak_v; }
static uint32_t scale_value(void *u,uint32_t word,uint32_t par) { (void)u;CHECK(word==SCRATCH);notef("scale%x/%x ",par,r32(NULL,word),0);w32(NULL,word,r32(NULL,word)*3+par);return 0; }
static uint32_t opposing_selector(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("os%u ",id,0,0);return 0x31; }
static uint32_t selector_value(void *u,uint32_t s) { (void)u;notef("sv%x ",s,0,0);return sel_v; }
static uint32_t random_(void *u,uint32_t n) { (void)u;notef("rnd%u ",n,0,0);return rnd; }
static uint32_t type_value(void *u,uint32_t t) { (void)u;notef("tv%u ",t,0,0);return 0x100+t; }
static const struct px_battle_moves_17_api api={&base,&handlers,action_record,commit,message,message_arg,item_burnable,item_flammable,no_hp,
    disable_blocked,is_ally,note_handler,has_status,side_id,can_pursue,redirect_blocked,pursue,streak,scale_value,opposing_selector,selector_value,
    random_,type_value,TRI,SCRATCH};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_17_api*);
static const fn dispatched[]={px_battle_struggle_ready,px_battle_tri_attack_effect};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w) { (void)u;CHECK(f<2);dispatched[f](h,c,v,w,&api); }
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;log_[0]=0;records=burn=flam=blockedv=ally=status_v=pursue_ok=redirect=streak_v=rnd=sel_v=0;memset(nohp,0,sizeof nohp);
    for(unsigned i=0;i<8;++i){ memset(mem+RECORD+0x40*i,0xa5,0x40); }
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
    for(unsigned i=0;i<PX_BATTLE_HANDLER_POOL;++i)w32(NULL,POOL+4*i,NODES+0x40*i);
    w8(NULL,TRI,1);w8(NULL,TRI+1,4);w8(NULL,TRI+2,3);
}
/* Keys: 0x03 attacker, 0x04 defender, 0x06 target (ids); 0x12, 0x14, 0x1e, 0x1f, 0x27, 0x40, 0x47, 0x53 values. */
static uint32_t move_v=0x99,used_v=0x321,kind_v=8,flag_v=0;
static void frame(uint32_t attacker,uint32_t defender)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x03,attacker,&events);px_battle_event_set_id(0x04,defender,&events);
    px_battle_event_set_id(0x06,defender,&events);px_battle_event_set_value(0x12,move_v,&events);px_battle_event_set_value(0x14,used_v,&events);
    px_battle_event_set_value(0x1e,kind_v,&events);px_battle_event_set_value(0x1f,0x1111,&events);px_battle_event_set_value(0x27,0,&events);
    px_battle_event_set_value(0x40,AREA,&events);px_battle_event_set_value(0x47,flag_v,&events);px_battle_event_set_value(0x53,0,&events);log_[0]=0;
}
static uint32_t key(uint32_t k) { return px_battle_event_get(k,&events); }
static int same(const char *expected) { if(strcmp(log_,expected)){ fprintf(stderr,"log [%s] expected [%s]\n",log_,expected);return 0; } return 1; }
#define R(n) (RECORD+0x40*(n))
int main(void)
{
    unsigned cases=0;
    /* Nothing for another attacker (key 0x03, or 0x04 for Destiny Bond). */
    {
        const fn by3[]={px_battle_incinerate_item,px_battle_rapid_spin_clear,px_battle_struggle_ready,px_battle_streak_heal,px_battle_effect_message,
                        px_battle_effect_text,px_battle_scale_193,px_battle_scale_357,px_battle_selector_value,px_battle_scale_by_move,px_battle_tri_attack_effect};
        for(unsigned k=0;k<11;++k)for(uint32_t o=0;o<2;++o){ reset();burn=flam=1;streak_v=5;status_v=3;w16(NULL,HANDLER+0x38,0x14);
            frame(o?0x102:3,5);by3[k](HANDLER,CTX,ATT,WORK,&api);
            CHECK(log_[0]==0&&records==0&&key(0x53)==0&&key(0x1f)==0x1111&&key(0x1e)==8&&aborts==0);++cases; }
        for(uint32_t o=0;o<2;++o){ reset();nohp[2]=1;frame(3,o?0x102:3);px_battle_destiny_bond_faint(HANDLER,CTX,ATT,WORK,&api);CHECK(log_[0]==0&&records==0);++cases; }
        reset();pursue_ok=0;frame(3,5);px_battle_pursuit_strike(HANDLER,CTX,ATT,WORK,&api);CHECK(same("cp2 "));++cases;
    }
    /* Incinerate: flag, the two item tests (the second only when the first fails) and the record. */
    for(uint32_t f=0;f<2;++f)for(uint32_t b=0;b<2;++b)for(uint32_t l=0;l<2;++l){
        reset();flag_v=f;burn=b;flam=l;w16(NULL,MON+0x500+0x12,0x1234);frame(3,0x105);px_battle_incinerate_item(HANDLER,CTX,3,WORK,&api);flag_v=0;
        if(f) CHECK(log_[0]==0);
        else if(!b&&!l) CHECK(same("burn1234 flam1234 "));
        else {
            char e[160];snprintf(e,sizeof e,"burn1234 %srec21/3 msg45a/%x arg5 arg1234 commit0 ",b?"":"flam1234 ",(0xc)*16+2);
            CHECK(same(e)&&mem[R(0)+6]==5&&r16(NULL,R(0)+4)==0&&mem[R(0)+7]==0xa5);
        }
        ++cases;
    }
    /* Destiny Bond: no HP, disable, ally, attacker's HP, and the records; the handler removes itself after the first. */
    for(uint32_t h=0;h<2;++h)for(uint32_t d=0;d<2;++d)for(uint32_t a=0;a<2;++a)for(uint32_t n=0;n<2;++n){
        reset();nohp[2]=h;blockedv=d;ally=a;nohp[3]=n;move_v=0x10000+0x555;used_v=0x10321;
        const uint32_t hn=px_battle_handler_register(0,194,1,0,ATT,ENTRIES,1,&handlers);w16(NULL,hn+0x38,194);
        frame(4,ATT);px_battle_event_rewrite(0x53,0,&events);
        px_battle_event_push(&events);px_battle_event_set_id(0x03,3,&events);px_battle_event_set_id(0x04,ATT,&events);
        px_battle_event_set_value(0x12,move_v,&events);px_battle_event_set_value(0x14,used_v,&events);log_[0]=0;
        px_battle_destiny_bond_faint(hn,CTX,ATT,hn+0x1c,&api);used_v=0x321;move_v=0x99;
        CHECK(aborts==0);
        if(!h) { CHECK(strcmp(log_,"nohp2 ")==0&&r32(NULL,hn+0x18)!=0); ++cases; continue; }
        CHECK(r32(NULL,hn+0x18)==0);
        if(d) { CHECK(strcmp(log_,"nohp2 db555 ")==0); }
        else if(a) { CHECK(strcmp(log_,"nohp2 db555 ally2/3 ")==0); }
        else if(n) { CHECK(strcmp(log_,"nohp2 db555 ally2/3 nohp3 ")==0); }
        else {
            char e[256];snprintf(e,sizeof e,"nohp2 db555 ally2/3 nohp3 noteh%x rec12/3 commit0 rec12/2 msg275/%x arg2 commit1 ",hn,0x48*16+2);
            CHECK(same(e));
            CHECK(mem[R(0)+4]==ATT&&mem[R(0)+5]==1&&r16(NULL,R(0)+6)==0x321&&mem[R(1)+4]==3&&r16(NULL,R(1)+6)==194);
        }
        ++cases;
    }
    /* Rapid Spin. */
    for(uint32_t s=0;s<4;++s){
        reset();status_v=s;frame(ATT,5);px_battle_rapid_spin_clear(HANDLER,CTX,ATT,WORK,&api);
        char e[200]="st2/12 ";unsigned n=0;
        if(s&1) { strcat(e,"reca/2 commit0 ");n=1; }
        strcat(e,"st2/8 ");
        if(s&2) { char t[40];snprintf(t,sizeof t,"reca/2 commit%u ",n);strcat(e,t);n++; }
        char t[80];snprintf(t,sizeof t,"rec1a/2 sid2 commit%u ",n);strcat(e,t);
        CHECK(same(e));
        const uint32_t r=R(n);CHECK(mem[r+8]==0x42&&mem[r+4]==4&&mem[r+5]==0xc0&&mem[r+6]==0x41&&mem[r+7]==0);
        if(s&1) CHECK(mem[R(0)+4]==0x12&&mem[R(0)+5]==ATT&&mem[R(0)+0x11]==1);
        if(s&2) CHECK(mem[R(n-1)+4]==8);
        ++cases;
    }
    reset();frame(ATT,5);px_battle_struggle_ready(HANDLER,CTX,ATT,WORK,&api);CHECK(key(0x53)==1&&log_[0]==0);++cases;
    /* Pursuit. */
    for(uint32_t p=0;p<2;++p)for(uint32_t a=0;a<2;++a)for(uint32_t r=0;r<2;++r){
        reset();pursue_ok=p;ally=a;redirect=r;frame(3,0x105);px_battle_pursuit_strike(HANDLER,CTX,ATT,WORK,&api);
        if(!p) CHECK(same("cp2 "));
        else if(a) CHECK(same("cp2 ally5/2 "));
        else if(r) CHECK(same("cp2 ally5/2 side5 side2 rb2/5/0 "));
        else CHECK(same("cp2 ally5/2 side5 side2 rb2/5/0 pursue2 "));
        ++cases;
    }
    /* Move 475: streak thresholds. */
    {
        const uint32_t vals[]={0,1,2,1000,1001,1002,1500,0xffffffffu};
        for(unsigned i=0;i<8;++i){ reset();streak_v=vals[i];
            frame(ATT,5);px_battle_streak_heal(HANDLER,CTX,ATT,WORK,&api);
            if(vals[i]<=1) CHECK(same("streak2 "));
            else { CHECK(same("streak2 rec2e/2 msg454/82 arg2 commit0 ")&&mem[R(0)+4]==ATT&&r16(NULL,R(0)+6)==(uint16_t)(vals[i]<=1001?1u:vals[i]-1000)); }
            ++cases; }
    }
    /* Move 477. */
    reset();frame(ATT,0x105);px_battle_effect_message(HANDLER,CTX,ATT,WORK,&api);
    CHECK(key(0x1e)==0x20&&same("msg47a/107c02 arg105 "));++cases;
    /* The eight effect texts, and other moves. */
    {
        const uint32_t moves[9]={0xfa,0x14,0x23,0x53,0x80,0x148,0x1cf,0x263,0x99};
        const uint32_t texts[8]={0x33b,0x326,0x32d,0x33e,0x334,0x344,0x341,0x4d2};
        for(unsigned i=0;i<9;++i)for(uint32_t k=0;k<3;++k){
            reset();kind_v=k==0?8:k==1?0x108:7;w16(NULL,HANDLER+0x38,(uint16_t)moves[i]);frame(ATT,0x105);px_battle_effect_text(HANDLER,CTX,ATT,WORK,&api);kind_v=8;
            if(i<8&&k==0){ char e[80];snprintf(e,sizeof e,"msg%x/107c02 arg5 arg2 ",texts[i]);CHECK(same(e)); }
            else CHECK(log_[0]==0);
            ++cases; }
    }
    /* Scaling moves. */
    reset();frame(ATT,5);px_battle_scale_193(HANDLER,CTX,ATT,WORK,&api);CHECK(same("scale7/1111 ")&&key(0x1f)==0x1111*3+7);++cases;
    reset();frame(ATT,5);px_battle_scale_357(HANDLER,CTX,ATT,WORK,&api);CHECK(same("scale10/1111 ")&&key(0x1f)==0x1111*3+0x10);++cases;
    for(uint32_t m=0;m<3;++m){ reset();w16(NULL,HANDLER+0x38,(uint16_t)(m==0?20:m==1?463:0x263));frame(ATT,5);px_battle_scale_by_move(HANDLER,CTX,ATT,WORK,&api);
        const uint32_t mv=m==0?20:m==1?463:0x263;char e[40];snprintf(e,sizeof e,"scale%x/1111 ",mv);CHECK(same(e)&&key(0x1f)==0x1111*3+mv);++cases; }
    for(uint32_t v=0;v<3;++v){ reset();sel_v=v*0x1234;frame(ATT,5);px_battle_selector_value(HANDLER,CTX,ATT,WORK,&api);CHECK(same("os2 sv31 ")&&key(0x1f)==v*0x1234);++cases; }
    /* Tri Attack: each pick, and a range wider than the table's byte mask. */
    for(uint32_t r=0;r<3;++r){ reset();rnd=0x100+r;frame(ATT,5);px_battle_tri_attack_effect(HANDLER,CTX,ATT,WORK,&api);
        const uint32_t t=r==0?1u:r==1?4u:3u;char e[40];snprintf(e,sizeof e,"rnd3 tv%u ",t);
        CHECK(same(e)&&key(0x1e)==t&&key(0x1f)==0x100+t&&key(0x27)==0x14);++cases; }
    /* Through the dispatcher on event 0x6d: Tri Attack for battler 2, Struggle for 2. */
    {
        reset();
        for(unsigned i=0;i<2;++i){ w8(NULL,ENTRIES+0x10*i,0x6d);w32(NULL,ENTRIES+0x10*i+4,i?1:0); }
        for(unsigned i=0;i<2;++i) {
            const uint32_t hn=NODES+0x40*i;
            w32(NULL,hn+8,ENTRIES+0x10*i);w32(NULL,hn+0x18,0x20000000u|1u<<16);w8(NULL,hn+0x3a,ATT);
            w32(NULL,hn,i?hn-0x40:0);w32(NULL,hn+4,i<1?hn+0x40:0);
        }
        w32(NULL,BATTLE+0x60,NODES);rnd=1;frame(ATT,5);px_battle_event_dispatch(CTX,0x6d,&events);
        CHECK(same("rnd3 tv4 ")&&key(0x1e)==4&&key(0x53)==1&&aborts==0);++cases;
    }
    printf("battle_moves_17: %u cases, all tests passed\n",cases);
    return 0;
}
