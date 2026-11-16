#include "battle_moves_13.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, BUF=0x7800, ENTRIES=0x7900, NODES=0x8000, SIDES=0x9000 };
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
/* Mock state. */
static uint32_t records,level,coins,sel,found,shrink;
static uint32_t conds,prot,tflag,cond_calls,prot_calls,side_base;
static uint32_t listed[8],asleep[8],locked[8];
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;notef("val%u/%x ",slot(p),k,0);return level; }
static uint32_t side_battlers(void *u,uint32_t c,uint32_t s,uint32_t out) { (void)u;CHECK(c==CTX&&out==BUF+1);notef("sb%x ",s,0,0);
    for(uint32_t i=0;i<8;++i) { w8(NULL,out+i,(uint8_t)listed[i]); }
    return found; }
static const struct px_battle_ability_api base={&events,pokemon_value,NULL,NULL,NULL,side_battlers,NULL};
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX&&records<12);notef("rec%x/%u ",k,id,0);return RECORD+0x40*records++; }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX);notef("commit%u ",(r-RECORD)/0x40,0,0); }
static void message(void *u,uint32_t at,uint32_t k,uint32_t m) { (void)u;notef("msg%x/%x ",m,(at-RECORD)*16+k,0); }
static void message_arg(void *u,uint32_t at,uint32_t v) { (void)u;notef("arg%x/%x ",v,at-RECORD,0); }
static uint32_t side_of(void *u,uint32_t id) { (void)u;CHECK(id<0x100);notef("side%u ",id,0,0);return id+side_base; }
static uint32_t opposing_side(void *u,uint32_t s) { (void)u;notef("opp%x ",s,0,0);return s^0x301; }
static uint32_t side_condition(void *u,uint32_t c,uint32_t s,uint32_t i) { (void)u;CHECK(c==CTX&&i==cond_calls&&i<0x11);
    CHECK(s==5+side_base);++cond_calls;return conds>>i&1?0x100u<<i:0; }
static uint32_t protection_condition(void *u,uint32_t i) { (void)u;CHECK(i<0x11);++prot_calls;return prot>>i&1?0x10000u<<(i&7):0; }
static uint32_t turn_flag(void *u,uint32_t p,uint32_t f) { (void)u;notef("tf%u/%u ",slot(p),f,0);return tflag; }
static void break_protection(void *u,uint32_t c,uint32_t id,uint32_t p,uint32_t m) { (void)u;CHECK(c==CTX);notef("break%u/%u/%x ",id,slot(p),m); }
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;const unsigned k=slot(p);notef("st%u/%x ",k,s,0);
    if(s==2){ if(shrink) { w8(NULL,BUF,(uint8_t)shrink); } return asleep[k]; }
    CHECK(s==0x19);return locked[k]; }
static uint32_t lock_count(void *u,uint32_t t,uint32_t m) { (void)u;notef("lc%u/%u ",t,m,0);return 0x12345678; }
static uint32_t opposing_selector(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("os%u ",id,0,0);return sel; }
static uint32_t temp_buffer(void *u,uint32_t c,uint32_t n) { (void)u;CHECK(c==CTX);notef("buf%x ",n,0,0);return BUF; }
static uint32_t scatter_money(void *u,uint32_t c,uint32_t a,uint32_t id) { (void)u;CHECK(c==CTX);notef("coin%u/%u ",a,id,0);return coins; }
static const struct px_battle_moves_13_api api={&base,action_record,commit,message,message_arg,side_of,opposing_side,side_condition,
    protection_condition,turn_flag,break_protection,has_status,lock_count,opposing_selector,temp_buffer,scatter_money,SIDES};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_13_api*);
static const fn dispatched[]={px_battle_explosion_faint,px_battle_feint_bypass};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w) { (void)u;CHECK(f<2);dispatched[f](h,c,v,w,&api); }
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;log_[0]=0;records=level=coins=sel=found=shrink=0;
    conds=prot=tflag=cond_calls=prot_calls=side_base=0;
    for(unsigned i=0;i<8;++i){ listed[i]=asleep[i]=locked[i]=0; }
    for(unsigned i=0;i<12;++i){ memset(mem+RECORD+0x40*i,0xa5,0x40); }
    memset(mem+BUF,0xcc,0x10);
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
/* Keys: 0x03 attacker, 0x04 defender, 0x06 target (ids); 0x53 (value). */
static void frame(uint32_t attacker,uint32_t defender,uint32_t target)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x03,attacker,&events);px_battle_event_set_id(0x04,defender,&events);
    px_battle_event_set_id(0x06,target,&events);px_battle_event_set_value(0x53,0,&events);log_[0]=0;
}
static uint32_t key(uint32_t k) { return px_battle_event_get(k,&events); }
static int same(const char *expected) { if(strcmp(log_,expected)){ fprintf(stderr,"log [%s] expected [%s]\n",log_,expected);return 0; } return 1; }
#define R(n) (RECORD+0x40*(n))
#define W(i) r32(NULL,WORK+4*(i))
int main(void)
{
    unsigned cases=0;
    /* Nothing for another attacker. */
    {
        const fn by3[]={px_battle_feint_break,px_battle_hyperspace_fury_break,px_battle_shadow_force_break,px_battle_feint_bypass,
                        px_battle_brick_break,px_battle_uproar_start,px_battle_pay_day,px_battle_explosion_faint};
        for(unsigned k=0;k<8;++k)for(uint32_t o=0;o<2;++o){ reset();conds=prot=0x1ffff;tflag=1;w32(NULL,SIDES,1);coins=1;
            frame(o?0x102:3,5,5);by3[k](HANDLER,CTX,ATT,WORK,&api);
            CHECK(log_[0]==0&&records==0&&cond_calls==0&&W(6)==0&&key(0x53)==0&&aborts==0);++cases; }
    }
    /* Feint, Hyperspace Fury (defender, key 0x04) and Shadow/Phantom Force (target, key 0x06): a lone
     * condition i, protecting or not, the turn flag, and the condition-0x10 case. */
    {
        const fn f[3]={px_battle_feint_break,px_battle_hyperspace_fury_break,px_battle_shadow_force_break};
        const uint32_t msg[3]={0x20e,0x208,0x208};
        for(unsigned k=0;k<3;++k)for(uint32_t i=0;i<0x12;++i)for(uint32_t p=0;p<2;++p)for(uint32_t t=0;t<2;++t){
            reset();side_base=k*0x10;conds=i<0x11?1u<<i:0;prot=p?(i<0x11?1u<<i:0x1ffff):0x1ffff&~(i<0x11?1u<<i:0);tflag=t?0x100:0;
            if(k<2) frame(ATT,0x105,6);else frame(ATT,6,0x105);
            f[k](HANDLER,CTX,ATT,WORK,&api);
            const int hit=i<0x11&&p;char e[128];
            if(hit||t) snprintf(e,sizeof e,"side5 tf5/7 break2/5/%x ",msg[k]);else snprintf(e,sizeof e,"side5 tf5/7 ");
            CHECK(same(e)&&records==0&&aborts==0);
            CHECK(cond_calls==(hit?i+1:0x11));
            CHECK(prot_calls==(i<0x11?1u:0u));
            ++cases;
        }
        /* The first protecting condition ends the search; non-protecting ones before it are passed over. */
        reset();conds=0x1ffff;prot=0x14000;frame(ATT,5,5);px_battle_feint_break(HANDLER,CTX,ATT,WORK,&api);
        CHECK(same("side5 tf5/7 break2/5/20e ")&&cond_calls==0xf&&prot_calls==0xf);++cases;
        reset();conds=0x10000;prot=0x10000;frame(ATT,5,5);px_battle_hyperspace_fury_break(HANDLER,CTX,ATT,WORK,&api);
        CHECK(same("side5 tf5/7 break2/5/208 ")&&cond_calls==0x11);++cases;   /* condition 0x10 counts */
    }
    /* The bypass key. */
    reset();frame(ATT,5,5);px_battle_feint_bypass(HANDLER,CTX,ATT,WORK,&api);CHECK(key(0x53)==1&&log_[0]==0&&records==0);++cases;
    reset();frame(ATT,5,5);px_battle_event_rewrite(0x53,7,&events);px_battle_feint_bypass(HANDLER,CTX,ATT,WORK,&api);CHECK(key(0x53)==7);++cases;
    /* Unprotect (event 0x32, byte +9) and its early form (event 0x2c): no key test; the opposing side's mask. */
    {
        const uint32_t masks[]={0,0x1ffff,1,0x80,0x100,0x8000,0x10000,0x15a5a,0x0a5a5,0x1fffe};
        for(uint32_t e=0;e<2;++e)for(unsigned n=0;n<sizeof masks/sizeof *masks+17;++n){
            reset();prot=n<10?masks[n]:1u<<(n-10);side_base=0x40;frame(3,3,3);
            (e?px_battle_feint_unprotect_early:px_battle_feint_unprotect)(HANDLER,CTX,ATT,WORK,&api);
            CHECK(same("side2 opp42 rec1b/2 commit0 ")&&prot_calls==0x11&&records==1);
            CHECK(mem[R(0)+8]==0x43&&mem[R(0)+9]==(e?0xa5:1)&&mem[R(0)+4]==4);
            CHECK(mem[R(0)+5]==(prot&0xff)&&mem[R(0)+6]==(prot>>8&0xff)&&mem[R(0)+7]==(prot>>16&0xff));
            CHECK(mem[R(0)+3]==0xa5&&mem[R(0)+0xa]==0xa5);
            ++cases;
        }
    }
    /* Brick Break: Reflect (entry 0) or Light Screen (entry 1) of the defender's side. */
    for(uint32_t s=0;s<4;++s)for(uint32_t r=0;r<4;++r)for(uint32_t l=0;l<4;++l){
        reset();const uint32_t at=SIDES+s*0x110;
        const uint32_t v[4]={0,1,0x80000000u,0x100};
        w32(NULL,at,v[r]);w32(NULL,at+0x10,v[l]);w32(NULL,at+0x20,1);w32(NULL,at+4,1);w32(NULL,at+0x14,1);
        if(s) { w32(NULL,at-0x110,1);w32(NULL,at-0x100,1); }
        frame(ATT,0x100+s,7);px_battle_brick_break(HANDLER,CTX,ATT,WORK,&api);
        char e[96];snprintf(e,sizeof e,"side%u ",s);
        if(r==0&&l==0) { CHECK(same(e)&&records==0); }
        else {
            snprintf(e,sizeof e,"side%u rec1a/2 commit0 rec3b/2 commit1 ",s);
            CHECK(same(e)&&mem[R(0)+8]==s&&mem[R(0)+4]==4&&mem[R(0)+5]==3&&mem[R(0)+6]==0&&mem[R(0)+7]==0&&mem[R(0)+9]==0xa5);
            CHECK(mem[R(1)+4]==1&&mem[R(1)+5]==0xa5&&mem[R(1)+3]==0xa5);
        }
        ++cases;
    }
    /* Uproar: locked or started already, nothing; otherwise the lock, the message and the wake-ups. */
    for(uint32_t lk=0;lk<2;++lk)for(uint32_t st=0;st<2;++st){
        reset();locked[ATT]=lk?0x100:0;w32(NULL,WORK+0x18,st?0x10000:0);frame(ATT,5,5);px_battle_uproar_start(HANDLER,CTX,ATT,WORK,&api);
        if(lk) CHECK(same("st2/19 ")&&W(6)==(st?0x10000u:0));
        else if(st) CHECK(same("st2/19 ")&&W(6)==0x10000);
        else CHECK(records==2&&W(6)==1&&strstr(log_,"sb800 ")!=NULL);
        ++cases;
    }
    {
        /* Sleep patterns over a two-battler list, the count stored as a byte, and the count re-read each round. */
        for(uint32_t pat=0;pat<4;++pat){
            reset();w16(NULL,HANDLER+0x38,253);sel=0x31;found=2;listed[0]=4;listed[1]=7;asleep[4]=pat&1?0x100:0;asleep[7]=pat&2?1:0;
            frame(ATT,5,5);px_battle_uproar_start(HANDLER,CTX,ATT,WORK,&api);
            char e[512];
            snprintf(e,sizeof e,"st2/19 recb/2 lc3/253 commit0 rec3/2 msg2bf/442 arg2/44 commit1 os2 buf%x sb%x st4/2 %sst7/2 %s",
                     0xb,0x831,pat&1?"reca/2 msg2c2/942 arg4/94 commit2 ":"",
                     pat&2?(pat&1?"reca/2 msg2c2/d42 arg7/d4 commit3 ":"reca/2 msg2c2/942 arg7/94 commit2 "):"");
            CHECK(same(e)&&W(6)==1&&mem[BUF]==2);
            CHECK(r16(NULL,R(0)+4)==0x19&&r32(NULL,R(0)+8)==0x12345678&&mem[R(0)+0xc]==0&&mem[R(0)+0xf]==ATT&&mem[R(0)+0xd]==0xa5&&mem[R(0)+0xe]==0xa5);
            if(pat&1) CHECK(mem[R(2)+4]==2&&mem[R(2)+5]==4&&mem[R(2)+0x11]==1&&mem[R(2)+6]==0xa5);
            if(pat==2) CHECK(mem[R(2)+5]==7);
            if(pat==3) CHECK(mem[R(3)+5]==7&&mem[R(3)+4]==2&&mem[R(3)+0x11]==1);
            ++cases;
        }
        reset();w16(NULL,HANDLER+0x38,0x1234);sel=0x800;found=0;frame(ATT,5,5);px_battle_uproar_start(HANDLER,CTX,ATT,WORK,&api);
        CHECK(same("st2/19 recb/2 lc3/4660 commit0 rec3/2 msg2bf/442 arg2/44 commit1 os2 bufb sb800 ")&&records==2&&mem[BUF]==0&&W(6)==1);++cases;
        /* 0x100 found: the byte stored is 0, yet the first round runs. */
        reset();found=0x100;listed[0]=3;asleep[3]=1;frame(ATT,5,5);px_battle_uproar_start(HANDLER,CTX,ATT,WORK,&api);
        CHECK(mem[BUF]==0&&records==3&&mem[R(2)+5]==3&&strstr(log_,"st3/2 reca/2 msg2c2/942 arg3/94 commit2 ")!=NULL&&W(6)==1);++cases;
        /* The stored count shrinks during the first round: the loop re-reads it. */
        reset();found=3;listed[0]=3;listed[1]=4;listed[2]=5;asleep[4]=1;shrink=1;frame(ATT,5,5);px_battle_uproar_start(HANDLER,CTX,ATT,WORK,&api);
        CHECK(records==2&&strstr(log_,"st3/2 ")!=NULL&&strstr(log_,"st4/2")==NULL);++cases;
        /* Grows past the first count: three rounds from a count of 1. */
        reset();found=1;listed[0]=3;listed[1]=4;listed[2]=5;asleep[5]=1;shrink=3;frame(ATT,5,5);px_battle_uproar_start(HANDLER,CTX,ATT,WORK,&api);
        CHECK(records==3&&strstr(log_,"st3/2 st4/2 st5/2 reca/2 msg2c2/942 arg5/94 commit2 ")!=NULL&&mem[R(2)+5]==5);++cases;
    }
    /* Pay Day: five times the level; the message only when coins were added. */
    {
        const uint32_t levels[]={1,50,100,0x33333334u};
        for(unsigned l=0;l<4;++l)for(uint32_t c=0;c<2;++c){
            reset();level=levels[l];coins=c?0x100:0;frame(ATT,5,5);px_battle_pay_day(HANDLER,CTX,ATT,WORK,&api);
            char e[128];snprintf(e,sizeof e,"val2/f coin%u/2 %s",levels[l]*5u,c?"rec3/2 msg80/41 commit0 ":"");
            CHECK(same(e));++cases;
        }
    }
    /* Self-Destruct and Explosion. */
    reset();frame(ATT,5,5);px_battle_explosion_faint(HANDLER,CTX,ATT,WORK,&api);
    CHECK(same("rec12/2 commit0 ")&&mem[R(0)+4]==ATT&&mem[R(0)+5]==0xa5);++cases;
    /* Through the dispatcher on event 0x4d: Explosion for battler 2 (the attacker) and battler 3, then the bypass. */
    {
        reset();
        for(unsigned i=0;i<3;++i){ w8(NULL,ENTRIES+0x10*i,0x4d);w32(NULL,ENTRIES+0x10*i+4,i==2?1:0); }
        const uint8_t ids[3]={ATT,3,ATT};
        for(unsigned i=0;i<3;++i) {
            const uint32_t hn=NODES+0x40*i;
            w32(NULL,hn+8,ENTRIES+0x10*i);w32(NULL,hn+0x18,0x20000000u|1u<<16);w8(NULL,hn+0x3a,ids[i]);
            w32(NULL,hn,i?hn-0x40:0);w32(NULL,hn+4,i<2?hn+0x40:0);
        }
        w32(NULL,BATTLE+0x60,NODES);frame(ATT,5,5);px_battle_event_dispatch(CTX,0x4d,&events);
        CHECK(same("rec12/2 commit0 ")&&key(0x53)==1&&aborts==0);++cases;
    }
    printf("battle_moves_13: %u cases, all tests passed\n",cases);
    return 0;
}
