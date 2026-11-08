#include "battle_moves_9.h"
#include "../battle_moves_5/battle_moves_5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, SCRATCH=0x7800, COUNTS=0x7840, ENTRIES=0x7900, NODES=0x8000 };
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
static void notef(const char *f,uint32_t a,uint32_t b) { char t[64];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned slot(uint32_t p) { if(!p) return 7;   /* an aborted lookup */
    CHECK(p>=MON&&p<MON+0x800&&!(p&0xff));return (p-MON)>>8; }
static uint32_t side_of(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("side%u ",id,0);return id+0x40; }
static const struct px_battle_ability_api base={&events,NULL,NULL,NULL,side_of,NULL,NULL};
static const struct px_battle_handler_api handlers={&memory,NULL,abort_battle,NULL,NULL,NULL,BATTLE,POOL,0,0,0,0,COUNTS};
static uint64_t status_mask[8];static uint32_t type_mask[8],rnd,turn_value,users,env,terrain_value,roll,records;
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX&&records<8);notef("rec%x/%u ",k,id);return RECORD+0x40*records++; }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX);notef("commit%u ",(r-RECORD)/0x40,0); }
static void message(void *u,uint32_t at,uint32_t k,uint32_t m) { (void)u;notef("msg%x/%x ",m,(at-RECORD)*16+k); }
static void message_arg(void *u,uint32_t at,uint32_t v) { (void)u;(void)at;notef("arg%x ",v,0); }
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;CHECK(s<64);notef("hs%u/%x ",slot(p),s);return (uint32_t)(status_mask[slot(p)]>>s&1); }
static uint32_t has_type(void *u,uint32_t p,uint32_t t) { (void)u;notef("ht%u/%u ",slot(p),t);return type_mask[slot(p)]>>t&1; }
static uint32_t random_(void *u,uint32_t r) { (void)u;notef("rnd%u ",r,0);return rnd; }
static uint32_t turn(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("turn ");return turn_value; }
static uint32_t move_users(void *u,uint32_t h,uint32_t m,uint32_t t) { (void)u;CHECK(h==CTX+0x20);notef("mn%u/%u ",m,t);return users; }
static uint32_t lock_count(void *u,uint32_t t,uint32_t m) { (void)u;notef("lc%u/%u ",t,m);return t*0x1000+m; }
static uint32_t move_count(void *u,uint32_t m) { (void)u;notef("mc%u ",m,0);return 0x77000+m; }
static void effect_flag(void *u,uint32_t c,uint32_t v) { (void)u;CHECK(c==CTX);notef("ef%u ",v,0); }
static uint32_t environment(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("env ");return env; }
static uint32_t terrain(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("ter ");return terrain_value; }
static uint32_t effect_roll(void *u,uint32_t c,uint32_t id,uint32_t t,uint32_t ch) { (void)u;CHECK(c==CTX);notef("roll%u/%u ",id,t);CHECK(ch==0x1e);return roll; }
static void status_value(void *u,uint32_t s,uint32_t p,uint32_t out) { (void)u;notef("sv%x/%u ",s,slot(p));w32(NULL,out,0x5500+s); }
static void explode(void *u,uint32_t c,uint32_t s,uint32_t o,uint32_t m,uint32_t z,uint32_t z2) { (void)u;CHECK(c==CTX&&z==0&&z2==0);notef("boom%x/%x ",s,o);notef("m%u ",m,0); }
static const struct px_battle_moves_9_api api={&base,&handlers,action_record,commit,message,message_arg,has_status,has_type,random_,turn,move_users,
    lock_count,move_count,effect_flag,environment,terrain,effect_roll,status_value,explode,SCRATCH};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_9_api*);
static const fn dispatched[]={px_battle_defense_curl_use,px_battle_minimize_use};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w) { (void)u;CHECK(f<2);dispatched[f](h,c,v,w,&api); }
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;log_[0]=0;records=rnd=turn_value=users=env=terrain_value=roll=0;
    memset(status_mask,0,sizeof status_mask);memset(type_mask,0,sizeof type_mask);
    for(unsigned i=0;i<8;++i){ memset(mem+RECORD+0x40*i,0xa5,0x40);w32(NULL,RECORD+0x40*i,0x11); }
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i){ w32(NULL,TABLE+0x84+4*i,MON+0x100*i);mem[MON+0x100*i+0x19]=(uint8_t)i; }
    for(unsigned i=0;i<PX_BATTLE_HANDLER_POOL;++i)w32(NULL,POOL+4*i,NODES+0x40*i);
    w16(NULL,HANDLER+0x38,0x123);w16(NULL,HANDLER+0x3a,0xffff);
}
/* Keys: 0x02 battler, 0x03 attacker, 0x06 target (ids); 0x12 move, 0x13, 0x3f, 0x48 (values). */
static uint32_t move_used=0x123;
static void frame2(uint32_t battler,uint32_t attacker)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x02,battler,&events);px_battle_event_set_id(0x03,attacker,&events);
    px_battle_event_set_id(0x06,5,&events);px_battle_event_set_value(0x12,move_used,&events);px_battle_event_set_value(0x13,0x99,&events);
    px_battle_event_set_value(0x3f,0,&events);px_battle_event_set_value(0x48,0,&events);log_[0]=0;
}
static void frame(uint32_t battler) { frame2(battler,battler); }
static uint32_t key(uint32_t k) { return px_battle_event_get(k,&events); }
static void run(fn f) { frame(ATT);f(HANDLER,CTX,ATT,WORK,&api);CHECK(aborts==0); }
static int same(const char *expected) { if(strcmp(log_,expected)){ fprintf(stderr,"log [%s] expected [%s]\n",log_,expected);return 0; } return 1; }
#define R(n) (RECORD+0x40*(n))
#define W(i) r32(NULL,WORK+4*(i))
int main(void)
{
    unsigned cases=0;
    /* Nothing for another battler (key 0x02; key 0x03 for Explosion and the hit handler). */
    {
        const fn all[]={px_battle_bide_use,px_battle_thrash_use,px_battle_rollout_use,px_battle_rage_use,px_battle_charge_use,px_battle_round_use,
            px_battle_toxic_use,px_battle_defense_curl_use,px_battle_minimize_use,px_battle_magnitude_use,px_battle_pledge_use,
            px_battle_secret_power_use,px_battle_explosion_use,px_battle_secret_power_hit};
        for(unsigned k=0;k<sizeof all/sizeof *all;++k)for(uint32_t other=0;other<2;++other) {
            reset();users=1;type_mask[ATT]=~0u;roll=1;w32(NULL,WORK,1);w32(NULL,WORK+4,1);frame(other?0x102:3);all[k](HANDLER,CTX,ATT,WORK,&api);
            CHECK(log_[0]==0&&records==0&&W(0)==1&&W(6)==0&&key(0x3f)==0&&key(0x13)==0x99&&aborts==0);++cases;
        }
        /* Event 0x24 handlers test key 0x02; Explosion and the hit handler test key 0x03. */
        for(unsigned k=0;k<sizeof all/sizeof *all;++k) {
            const int by_attacker=all[k]==px_battle_explosion_use||all[k]==px_battle_secret_power_hit;
            reset();users=1;type_mask[ATT]=~0u;roll=1;w32(NULL,WORK,1);w32(NULL,WORK+4,1);
            if(by_attacker) frame2(ATT,3);else frame2(3,ATT);
            all[k](HANDLER,CTX,ATT,WORK,&api);CHECK(log_[0]==0&&records==0&&aborts==0);++cases;
        }
    }
    /* Bide's three turns. */
    {
        reset();w32(NULL,WORK+4,9);run(px_battle_bide_use);
        CHECK(same("recb/2 mc291 commit0 ")&&r16(NULL,R(0)+4)==0x19&&r32(NULL,R(0)+8)==0x77123&&mem[R(0)+0xf]==ATT&&mem[R(0)+0xc]==0xa5);
        CHECK(key(0x3f)==2&&W(0)==1&&W(1)==0&&W(6)==1);++cases;
        reset();w32(NULL,WORK,1);w32(NULL,WORK+4,9);run(px_battle_bide_use);CHECK(log_[0]==0&&key(0x3f)==2&&W(0)==2&&W(1)==9&&records==0);++cases;
        for(uint32_t w6=0;w6<2;++w6)for(uint32_t t=2;t<5;++t) {
            reset();w32(NULL,WORK,t==4?0x80000000:t);w32(NULL,WORK+0x18,w6?0x100:0);run(px_battle_bide_use);
            CHECK(key(0x3f)==0&&W(6)==0&&W(0)==(t==4?0x80000000:t));
            if(w6) CHECK(same("ef1 reca/2 commit0 ")&&mem[R(0)+4]==0x19&&mem[R(0)+5]==ATT&&mem[R(0)+0x11]==1&&mem[R(0)+0x12]==1&&mem[R(0)+6]==0xa5);
            else CHECK(same("ef1 ")&&records==0);
            ++cases;
        }
    }
    /* Thrash: 2 or 3 turns unless locked (status 0x19 or word 6). */
    for(uint32_t locked=0;locked<3;++locked)for(uint32_t r=0;r<3;++r) {
        reset();rnd=r==2?0x1fe:r;if(locked==1) status_mask[ATT]=1ull<<0x19;if(locked==2) w32(NULL,WORK+0x18,1);
        run(px_battle_thrash_use);
        if(locked){ CHECK(records==0&&W(0)==0);++cases;continue; }
        const uint32_t n=(rnd+2)&0xff;char b[96];snprintf(b,sizeof b,"hs2/19 rnd2 recb/2 lc%u/291 commit0 ",n);
        CHECK(same(b)&&r16(NULL,R(0)+4)==0x19&&r32(NULL,R(0)+8)==n*0x1000+0x123&&mem[R(0)+0xc]==0&&mem[R(0)+0xf]==ATT&&W(6)==1&&W(0)==n);++cases;
    }
    /* Rollout and Ice Ball: 5 turns unless locked, asleep or word 6. */
    for(uint32_t c=0;c<4;++c) {
        reset();w32(NULL,WORK,7);w32(NULL,WORK+4,7);
        if(c==1) { status_mask[ATT]=1ull<<0x19; }
        if(c==2) { status_mask[ATT]=1ull<<2; }
        if(c==3) { w32(NULL,WORK+0x18,2); }
        run(px_battle_rollout_use);
        if(c){ CHECK(records==0&&W(0)==7&&W(1)==7);CHECK(same(c==1?"hs2/19 ":"hs2/19 hs2/2 "));++cases;continue; }
        CHECK(same("hs2/19 hs2/2 recb/2 lc5/291 commit0 ")&&r32(NULL,R(0)+8)==0x5123&&mem[R(0)+0xc]==0&&W(6)==1&&W(1)==5&&W(0)==0);++cases;
    }
    /* Rage: kept when used again; otherwise the battler's Rage handlers go. */
    {
        reset();run(px_battle_rage_use);CHECK(W(6)==1&&log_[0]==0);++cases;
        reset();move_used=0x10123;run(px_battle_rage_use);CHECK(W(6)==0);move_used=0x123;++cases;
        reset();
        uint32_t h[5];const uint32_t owners[5]={0,0,0,4,0},vals[5]={ATT,ATT,3,ATT,ATT},ids[5]={0x123,0x23,0x123,0x123,0x123};
        for(unsigned i=0;i<5;++i) h[i]=px_battle_handler_register(owners[i],ids[i],1,i,vals[i],ENTRIES,1,&handlers);
        mem[MON+0x219]=ATT;move_used=7;run(px_battle_rage_use);move_used=0x123;
        for(unsigned i=0;i<5;++i) { CHECK((r32(NULL,h[i]+0x18)==0)==(i==0||i==4)); }
        CHECK(W(6)==0&&r32(NULL,BATTLE+0x64)==3);++cases;
        /* the battler's own id comes from +0x19 */
        reset();h[0]=px_battle_handler_register(0,0x123,1,0,6,ENTRIES,1,&handlers);mem[MON+0x219]=6;move_used=7;run(px_battle_rage_use);move_used=0x123;
        CHECK(r32(NULL,h[0]+0x18)==0);++cases;
    }
    /* Charge. */
    for(uint32_t same_move=0;same_move<2;++same_move) {
        reset();move_used=same_move?0x123:0x124;run(px_battle_charge_use);move_used=0x123;
        if(!same_move){ CHECK(log_[0]==0&&key(0x3f)==0&&W(0)==0);++cases;continue; }
        CHECK(same("rec3/2 msg298/42 arg2 commit0 ")&&key(0x3f)==1&&W(6)==1&&W(0)==1);++cases;
    }
    /* Round: the first user this turn. */
    for(uint32_t n=0;n<4;++n){ reset();users=n==3?0x101:n;turn_value=6;run(px_battle_round_use);
        if(users==1) CHECK(same("turn mn291/6 rec31/2 commit0 ")&&r16(NULL,R(0)+4)==0x123);else CHECK(same("turn mn291/6 ")&&records==0);
        ++cases; }
    /* Toxic by a Poison type. */
    for(uint32_t t=0;t<6;++t){ reset();type_mask[ATT]=1u<<t;run(px_battle_toxic_use);
        if(t==3) CHECK(same("ht2/3 recb/2 commit0 ")&&mem[R(0)+0xf]==ATT&&r16(NULL,R(0)+4)==0x1c&&r32(NULL,R(0)+8)==px_battle_turn_count(1));
        else CHECK(same("ht2/3 ")&&records==0);
        ++cases; }
    /* Defense Curl and Minimize. */
    reset();run(px_battle_defense_curl_use);CHECK(same("rec17/2 commit0 ")&&mem[R(0)+4]==7&&mem[R(0)+5]==ATT);++cases;
    reset();run(px_battle_minimize_use);CHECK(same("rec17/2 commit0 ")&&mem[R(0)+4]==8&&mem[R(0)+5]==ATT);++cases;
    /* Magnitude over every roll (and wide ones). */
    {
        const uint8_t lim[7]={5,15,35,65,85,95,100},pow_[8]={10,30,50,70,90,110,150,0};
        for(uint32_t r=0;r<0x104;++r) {
            reset();rnd=r<0x100?r:0x300+r;run(px_battle_magnitude_use);
            const uint32_t b=rnd&0xff;uint32_t i=0;while(i<7&&lim[i]<=b) ++i;
            CHECK(W(0)==pow_[i]);char m[64];snprintf(m,sizeof m,"rnd100 rec3/2 msg%x/41 commit0 ",0x6c+i);CHECK(same(m));++cases;
        }
    }
    /* Pledges. */
    for(uint32_t w0=0;w0<2;++w0)for(uint32_t w1=0;w1<5;++w1) {
        reset();w32(NULL,WORK,w0?0x100:0);w32(NULL,WORK+4,w1==4?0x101:w1);run(px_battle_pledge_use);
        CHECK(key(0x13)==(!w0?0x99u:w1==1?0x206u:w1==2?0x207u:w1==3?0x208u:0u)&&log_[0]==0);++cases;
    }
    /* Secret Power's environment and terrain table. */
    {
        const uint32_t cls[9]={0,1,2,0,1,1,0,0,0},val[9]={1,6,0,2,1,5,3,3,1},ani[9]={0,1,6,7,5,2,3,4,0};
        for(uint32_t e=0;e<10;++e)for(uint32_t t=0;t<5;++t) {
            reset();env=e==9?0x100:e;terrain_value=t;mem[SCRATCH]=mem[SCRATCH+4]=0xee;frame(ATT);
            const uint32_t got=px_battle_secret_power_effect(CTX,SCRATCH+4,SCRATCH,&api);
            const uint32_t k=e<8?e:8;
            uint32_t c=cls[k],v=val[k],a=ani[k];
            if(t==1){ c=0;v=2;a=7; }else if(t==2){ c=1;v=3;a=8; }else if(t==3){ c=0;v=1;a=9; }
            CHECK(got==a&&mem[SCRATCH+4]==c&&mem[SCRATCH]==v&&same("env ter "));++cases;
        }
        reset();env=2;frame(ATT);{ static uint8_t copy[SIZE];memcpy(copy,mem,SIZE);CHECK(px_battle_secret_power_effect(CTX,0,0,&api)==6&&!memcmp(copy,mem,SIZE)); }++cases;   /* no outputs */
        reset();env=4;terrain_value=2;run(px_battle_secret_power_use);CHECK(same("env ter ef8 "));++cases;
    }
    /* Secret Power's added effect. */
    for(uint32_t e=0;e<9;++e)for(uint32_t r=0;r<2;++r)for(uint32_t fl=0;fl<2;++fl) {
        reset();env=e;roll=r;mem[R(0)+0x18]=(uint8_t)(fl?0xf0:0);run(px_battle_secret_power_hit);
        if(!r){ CHECK(same("roll2/5 ")&&records==0);++cases;continue; }
        const uint32_t k=e<8?e:8;const uint32_t cls[9]={0,1,2,0,1,1,0,0,0},val[9]={1,6,0,2,1,5,3,3,1};
        if(cls[k]==0){ char b[96];snprintf(b,sizeof b,"roll2/5 env ter recb/2 sv%x/2 commit0 ",val[k]);
            CHECK(same(b)&&mem[R(0)+0xf]==5&&r16(NULL,R(0)+4)==val[k]&&r32(NULL,R(0)+8)==0x5500+val[k]); }
        else if(cls[k]==1) CHECK(same("roll2/5 env ter recd/2 commit0 ")&&mem[R(0)+0xc]==5&&mem[R(0)+0x16]==1&&mem[R(0)+4]==val[k]&&mem[R(0)+0x17]==0xff&&mem[R(0)+0x18]==(fl?0xf4:4));
        else CHECK(same("roll2/5 env ter rec2c/2 commit0 ")&&mem[R(0)+4]==5&&mem[R(0)+5]==100);
        ++cases;
    }
    reset();roll=1;frame(ATT);px_battle_event_rewrite(0x48,1,&events);log_[0]=0;px_battle_secret_power_hit(HANDLER,CTX,ATT,WORK,&api);CHECK(log_[0]==0);++cases;
    reset();roll=1;w32(NULL,SCRATCH,0x03ffff03);frame(ATT);w32(NULL,TABLE+0x84+4*ATT,0);px_battle_secret_power_hit(HANDLER,CTX,ATT,WORK,&api);
    CHECK(aborts==1);++cases;   /* the attacker lookup comes after the roll */
    /* Explosion. */
    reset();run(px_battle_explosion_use);CHECK(same("ef1 side2 boom42/42 m291 "));++cases;
    /* Through the dispatcher on event 0x24: Defense Curl for battler 2, Minimize for battler 3. */
    {
        reset();
        for(unsigned i=0;i<2;++i){ w8(NULL,ENTRIES+0x10*i,0x24);w32(NULL,ENTRIES+0x10*i+4,i); }
        const uint8_t ids[2]={ATT,3};
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=NODES+0x40*i;
            w32(NULL,h+8,ENTRIES+0x10*i);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ids[i]);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,NODES);frame(ATT);px_battle_event_dispatch(CTX,0x24,&events);
        CHECK(same("rec17/2 commit0 ")&&mem[R(0)+4]==7&&aborts==0);++cases;
    }
    printf("battle_moves_9: %u cases, all tests passed\n",cases);
    return 0;
}
