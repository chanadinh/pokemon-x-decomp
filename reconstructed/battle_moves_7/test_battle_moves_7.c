#include "battle_moves_7.h"
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
static unsigned aborts;static char log_[4096];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[64];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static const struct px_battle_ability_api base={&events,NULL,NULL,NULL,NULL,NULL,NULL};
static unsigned slot(uint32_t p) { CHECK(p>=MON&&p<MON+0x800&&!(p&0xff));return (p-MON)>>8; }
/* Per battler: statuses as a bit mask, fainted flag; services. */
static uint64_t status_mask[8];static uint32_t is_fainted[8],berry,locked,charging_mask,airborne_mask,records;
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX&&records<16);notef("rec%x/%u ",k,id);return RECORD+0x40*records++; }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX);notef("commit%u ",(r-RECORD)/0x40,0); }
static void message(void *u,uint32_t at,uint32_t k,uint32_t m) { (void)u;notef("msg%x/%x ",m,(at-RECORD)*16+k); }
static void message_arg(void *u,uint32_t at,uint32_t v) { (void)u;(void)at;notef("arg%x ",v,0); }
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;CHECK(s<0x40);notef("hs%u/%x ",slot(p),s);return (uint32_t)(status_mask[slot(p)]>>s&1); }
static uint32_t fainted(void *u,uint32_t p) { (void)u;notef("f%u ",slot(p),0);return is_fainted[slot(p)]; }
static uint32_t item_is_berry(void *u,uint32_t item) { (void)u;notef("berry%x ",item,0);return berry; }
static uint32_t item_locked(void *u,uint32_t c,uint32_t id,uint32_t t) { (void)u;CHECK(c==CTX);notef("lock%u/%u ",id,t);return locked; }
static uint32_t charging(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("chg%u ",id,0);return id<32&&(charging_mask>>id&1); }
static uint32_t airborne(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("air%u ",id,0);return id<32&&(airborne_mask>>id&1); }
static uint32_t trap_value(void *u,uint32_t id) { (void)u;notef("trap%u ",id,0);return 0xabc00+id; }
static const struct px_battle_moves_7_api api={&base,action_record,commit,message,message_arg,has_status,fainted,item_is_berry,item_locked,
    charging,airborne,trap_value};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_7_api*);
static const fn dispatched[]={px_battle_wake_up_slap_cure,px_battle_knock_off_hit};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w) { (void)u;CHECK(f<2);dispatched[f](h,c,v,w,&api); }
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;log_[0]=0;records=berry=locked=charging_mask=airborne_mask=0;
    memset(status_mask,0,sizeof status_mask);memset(is_fainted,0,sizeof is_fainted);
    for(unsigned i=0;i<16;++i){ memset(mem+RECORD+0x40*i,0xa5,0x40);w32(NULL,RECORD+0x40*i,0x11); }
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
/* Keys: 0x03 attacker, 0x05 count, 0x06.. targets. */
static void frame(uint32_t attacker,uint32_t n,const uint32_t *t)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x03,attacker,&events);px_battle_event_set_value(0x05,n,&events);
    for(uint32_t i=0;i<4;++i) px_battle_event_set_id(6+i,i<n?t[i]:0x1f,&events);
    log_[0]=0;
}
static void run(fn f,uint32_t n,const uint32_t *t) { frame(ATT,n,t);f(HANDLER,CTX,ATT,WORK,&api);CHECK(aborts==0); }
static int same(const char *expected) { if(strcmp(log_,expected)){ fprintf(stderr,"log [%s] expected [%s]\n",log_,expected);return 0; } return 1; }
#define R(n) (RECORD+0x40*(n))
int main(void)
{
    unsigned cases=0;
    const uint32_t one5[1]={5},two[2]={5,4},three[3]={5,4,3};
    /* Nothing for another attacker. */
    {
        const fn all[]={px_battle_scald_thaw,px_battle_thief_steal,px_battle_pluck_eat,px_battle_smack_down_hit,px_battle_thousand_arrows_hit,
            px_battle_thousand_waves_hit,px_battle_circle_throw_hit,px_battle_fell_stinger_hit,px_battle_knock_off_hit,
            px_battle_smelling_salts_cure,px_battle_wake_up_slap_cure};
        for(unsigned k=0;k<sizeof all/sizeof *all;++k)for(uint32_t other=0;other<2;++other) {
            reset();status_mask[5]=~0ull;is_fainted[5]=1;berry=1;airborne_mask=~0u;w16(NULL,MON+0x512,9);
            frame(other?0x102:3,1,one5);all[k](HANDLER,CTX,ATT,WORK,&api);CHECK(log_[0]==0&&records==0&&aborts==0);++cases;
        }
    }
    /* Scald: every standing frozen target thaws. */
    for(uint32_t n=0;n<4;++n)for(uint32_t pattern=0;pattern<64;++pattern) {
        reset();
        for(uint32_t i=0;i<3;++i){ const uint32_t p=pattern>>(2*i)&3;is_fainted[5-i]=p==1;status_mask[5-i]=p>=2?(p==2?1ull<<3:~(1ull<<3)):0; }
        run(px_battle_scald_thaw,n,three);
        uint32_t cured=0;for(uint32_t i=0;i<n;++i) if((pattern>>(2*i)&3)==2) ++cured;
        CHECK(records==cured);
        uint32_t k=0;for(uint32_t i=0;i<n;++i) if((pattern>>(2*i)&3)==2){ CHECK(mem[R(k)+4]==3&&mem[R(k)+5]==5-i&&mem[R(k)+0x11]==1&&mem[R(k)+6]==0xa5);++k; }
        ++cases;
    }
    reset();status_mask[5]=1ull<<3;status_mask[4]=1ull<<3;is_fainted[4]=1;run(px_battle_scald_thaw,2,two);
    CHECK(same("f5 hs5/3 reca/2 commit0 f4 "));++cases;
    /* Thief and Covet. */
    for(uint32_t own=0;own<2;++own)for(uint32_t w=0;w<2;++w)for(uint32_t has=0;has<2;++has)for(uint32_t l=0;l<2;++l)for(uint32_t none=0;none<2;++none) {
        reset();w16(NULL,MON+0x212,(uint16_t)own);w32(NULL,WORK,w?0x100:0);w16(NULL,MON+0x512,(uint16_t)(has?0x1a3:0));locked=l;
        const uint32_t t1[1]={none?0x1f:5};run(px_battle_thief_steal,1,t1);
        const int steal=!own&&!w&&!none&&has&&!l;
        if(steal) {
            CHECK(same("lock2/5 rec25/2 msg427/82 arg2 arg5 arg1a3 commit0 rec3b/2 commit1 "));
            CHECK(mem[R(0)+4]==5&&mem[R(1)+4]==1&&r32(NULL,R(1))==0x1000011&&r32(NULL,R(0))==0x11);
        } else CHECK(records==0&&!strcmp(log_,!own&&!w&&!none&&has?"lock2/5 ":""));
        ++cases;
    }
    /* Pluck and Bug Bite. */
    for(uint32_t b=0;b<2;++b) {
        reset();berry=b;w16(NULL,MON+0x512,0x9e);w32(NULL,R(1)+4,0);run(px_battle_pluck_eat,1,one5);
        if(!b){ CHECK(same("berry9e ")&&records==0);++cases;continue; }
        CHECK(same("berry9e rec21/2 msg308/c2 arg2 arg9e commit0 rec23/2 commit1 "));
        CHECK(mem[R(0)+6]==5&&r16(NULL,R(0)+4)==0&&mem[R(1)+4]==ATT&&mem[R(1)+5]==1&&r16(NULL,R(1)+6)==0x9e&&r32(NULL,R(1))==0x1000011);++cases;
    }
    reset();berry=1;const uint32_t t1f[1]={0x105};frame(ATT,1,t1f);   /* the target id is narrowed */
    px_battle_pluck_eat(HANDLER,CTX,ATT,WORK,&api);CHECK(mem[R(0)+6]==5&&aborts==0&&same("berry0 rec21/2 msg308/c2 arg2 arg0 commit0 rec23/2 commit1 "));++cases;
    /* Smack Down's routine over its conditions. */
    for(uint32_t chg=0;chg<2;++chg)for(uint32_t air=0;air<2;++air)for(uint32_t st=0;st<16;++st)for(uint32_t bit=0;bit<2;++bit) {
        reset();charging_mask=chg<<5;airborne_mask=air<<5;
        status_mask[5]=(st&1?1ull<<0x21:0)|(st&2?1ull<<0x1e:0)|(st&4?1ull<<0x20:0)|(st&8?1ull<<3:0);
        mem[MON+0x500+0x169]=(uint8_t)(bit?8:0xf7);
        frame(ATT,1,one5);const uint32_t got=px_battle_smack_down_target(CTX,ATT,5,&api);
        const int ground=!chg&&air&&!(st&1);
        CHECK(got==(uint32_t)ground);
        uint32_t k=0;
        if(!chg&&ground){ CHECK(mem[R(0)+0xf]==5&&r16(NULL,R(0)+4)==0x1f&&r32(NULL,R(0)+8)==1);k=1;
            if(st&2){ CHECK(mem[R(k)+4]==0x1e&&mem[R(k)+5]==5&&mem[R(k)+0x11]==1&&mem[R(k)+0x12]==1);++k; }
            if(st&4){ CHECK(mem[R(k)+4]==0x20&&mem[R(k)+5]==5&&mem[R(k)+0x11]==1&&mem[R(k)+0x12]==1);++k; } }
        if(!chg&&bit){ CHECK(mem[R(k)+4]==5&&mem[R(k)+5]==3);CHECK((strstr(log_,"msg46e/82")!=NULL)==!ground);++k; }
        CHECK(records==k);++cases;
    }
    reset();airborne_mask=1u<<5;status_mask[5]=1ull<<0x1e;mem[MON+0x669]=8;frame(ATT,1,one5);
    CHECK(px_battle_smack_down_target(CTX,ATT,5,&api)==1);
    CHECK(same("chg5 air5 hs5/21 recb/2 msg46e/142 arg5 commit0 hs5/1e reca/2 commit1 hs5/20 rec37/2 commit2 "));++cases;
    reset();mem[MON+0x669]=8;frame(ATT,1,one5);CHECK(px_battle_smack_down_target(CTX,ATT,5,&api)==0);
    CHECK(same("chg5 air5 rec37/2 msg46e/82 arg5 commit0 "));++cases;
    reset();charging_mask=1u<<5;mem[MON+0x669]=8;frame(ATT,1,one5);CHECK(px_battle_smack_down_target(CTX,ATT,5,&api)==0&&same("chg5 "));++cases;
    /* Smack Down for the first target; Thousand Arrows for each. */
    reset();airborne_mask=1u<<5;run(px_battle_smack_down_hit,2,two);CHECK(same("chg5 air5 hs5/21 recb/2 msg46e/142 arg5 commit0 hs5/1e hs5/20 "));++cases;
    for(uint32_t n=0;n<4;++n){ reset();airborne_mask=~0u;run(px_battle_thousand_arrows_hit,n,three);CHECK(records==n);
        for(uint32_t i=0;i<n;++i) { CHECK(mem[R(i)+0xf]==5-i); }
        ++cases; }
    /* Thousand Waves: each target not yet trapped. */
    for(uint32_t n=0;n<4;++n)for(uint32_t mask=0;mask<8;++mask) {
        reset();for(uint32_t i=0;i<3;++i) status_mask[5-i]=mask>>i&1?1ull<<0x16:0;
        run(px_battle_thousand_waves_hit,n,three);
        uint32_t k=0;for(uint32_t i=0;i<n;++i) if(!(mask>>i&1)){ CHECK(mem[R(k)+0xf]==5-i&&r16(NULL,R(k)+4)==0x16&&r32(NULL,R(k)+8)==0xabc02);++k; }
        CHECK(records==k);++cases;
    }
    reset();run(px_battle_thousand_waves_hit,1,one5);CHECK(same("hs5/16 recb/2 trap2 msg36b/142 arg5 commit0 "));++cases;
    /* Circle Throw and Dragon Tail. */
    {
        const uint32_t moves[]={509,525,0x2fd,0};
        for(unsigned k=0;k<4;++k){ reset();w16(NULL,HANDLER+0x38,(uint16_t)moves[k]);w16(NULL,HANDLER+0x3a,0x1fd);const uint32_t t[1]={0x305};run(px_battle_circle_throw_hit,1,t);
            CHECK(same("rec2f/2 commit0 ")&&mem[R(0)+6]==5&&r16(NULL,R(0)+4)==(moves[k]==509?0x2c7u:0x2c8u));++cases; }
    }
    /* Fell Stinger: Attack +2 when a target fainted. */
    for(uint32_t n=0;n<4;++n)for(uint32_t mask=0;mask<8;++mask)for(uint32_t fl=0;fl<2;++fl) {
        reset();for(uint32_t i=0;i<3;++i) is_fainted[5-i]=mask>>i&1;mem[R(0)+0x18]=(uint8_t)(fl?0xfb:0);
        run(px_battle_fell_stinger_hit,n,three);
        const int any=(mask&((1u<<n)-1))!=0;
        CHECK(records==(uint32_t)any);
        if(any) CHECK(mem[R(0)+4]==1&&mem[R(0)+0xc]==ATT&&mem[R(0)+0x16]==1&&mem[R(0)+0x17]==2&&mem[R(0)+0x18]==(fl?0xff:4));
        ++cases;
    }
    reset();is_fainted[5]=2;run(px_battle_fell_stinger_hit,2,two);CHECK(same("f5 f4 recd/2 commit0 "));++cases;
    /* Knock Off. */
    for(uint32_t l=0;l<2;++l)for(uint32_t item=0;item<3;++item) {
        reset();locked=l;const uint16_t it=(uint16_t)(item==2?0x8001:item*0xe8);w16(NULL,MON+0x512,it);run(px_battle_knock_off_hit,1,one5);
        if(l){ CHECK(same("lock2/5 ")&&records==0);++cases;continue; }
        if(!it){ CHECK(same("lock2/5 ")&&records==0);++cases;continue; }
        char b[128];snprintf(b,sizeof b,"lock2/5 rec21/2 msg420/c2 arg2 arg5 arg%x commit0 ",it);CHECK(same(b)&&mem[R(0)+6]==5&&r16(NULL,R(0)+4)==0);++cases;
    }
    /* Smelling Salts and Wake-Up Slap. */
    for(uint32_t s=0;s<4;++s){ reset();status_mask[5]=1ull<<s;run(px_battle_smelling_salts_cure,1,one5);
        if(s==1) CHECK(same("hs5/1 reca/2 commit0 ")&&mem[R(0)+4]==1&&mem[R(0)+5]==5&&mem[R(0)+0x11]==1);else CHECK(same("hs5/1 ")&&records==0);++cases;
        reset();status_mask[5]=1ull<<s;run(px_battle_wake_up_slap_cure,1,one5);
        if(s==2) CHECK(same("hs5/2 reca/2 commit0 ")&&mem[R(0)+4]==2&&mem[R(0)+5]==5&&mem[R(0)+0x11]==1);else CHECK(same("hs5/2 ")&&records==0);++cases; }
    reset();const uint32_t none1[1]={0x1f};run(px_battle_smelling_salts_cure,1,none1);CHECK(log_[0]==0);++cases;
    /* Through the dispatcher on event 0x8c: Wake-Up Slap for battler 2, Knock Off for battler 3. */
    {
        reset();status_mask[5]=1ull<<2;
        for(unsigned i=0;i<2;++i){ w8(NULL,ENTRIES+0x10*i,0x8c);w32(NULL,ENTRIES+0x10*i+4,i); }
        const uint8_t ids[2]={ATT,3};
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=NODES+0x40*i;
            w32(NULL,h+8,ENTRIES+0x10*i);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ids[i]);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,NODES);frame(ATT,1,one5);px_battle_event_dispatch(CTX,0x8c,&events);
        CHECK(same("hs5/2 reca/2 commit0 ")&&aborts==0);++cases;
    }
    printf("battle_moves_7: %u cases, all tests passed\n",cases);
    return 0;
}
