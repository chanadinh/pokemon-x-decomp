#include "battle_moves_11.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, BUFFER=0x7000, PARTY=0x7100, PM=0x7200, SCRATCH=0x7800, ACTION=0x7810, MSGBUF=0x7900, ENTRIES=0x7a00, NODES=0x8000 };
enum { ATT=2, A=MON+0x200 };
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
/* Pokemon slots: MON + 0x100 i -> i (0..7), party members PM + 0x40 j -> 8 + j. */
static unsigned slot(uint32_t p) { if(p>=PM&&p<PM+0x200&&!(p&0x3f)) return 8+((p-PM)>>6); CHECK(p>=MON&&p<MON+0x800&&!(p&0xff));return (p-MON)>>8; }
static uint32_t listed_n,listed_ids[4];
static uint32_t side_battlers(void *u,uint32_t c,uint32_t sel,uint32_t out) { (void)u;CHECK(c==CTX);notef("sb%x ",sel,0);
    for(uint32_t i=0;i<listed_n;++i) { w8(NULL,out+i,(uint8_t)listed_ids[i]); }
    return listed_n; }
static const struct px_battle_ability_api base={&events,NULL,NULL,NULL,NULL,side_battlers,NULL};
static uint32_t asleep,moves[16][8],banned_mask,flag_mask,rnd,party_n,fainted_m,able_m,terrain_v,env_v,last_used,last_mv,moved,chosen,present,charging_v,damaging,pool_count;
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;notef("hs%u/%u ",slot(p),s);return s==2?asleep:0; }
static uint32_t move_at(void *u,uint32_t p,uint32_t s) { (void)u;CHECK(s<8);notef("ma%u/%u ",slot(p),s);return moves[slot(p)][s]; }
static uint32_t move_flag(void *u,uint32_t m,uint32_t f) { (void)u;notef("mf%u/%u ",m,f);return flag_mask>>(m&31)&1; }
static uint32_t random_(void *u,uint32_t r) { (void)u;notef("rnd%u ",r,0);return rnd; }
static uint32_t choose_target(void *u,uint32_t c,uint32_t id,uint32_t m) { (void)u;CHECK(c==CTX);notef("ct%u/%u ",id,m);return 0x40+m%16; }
static uint32_t banned(uint32_t m) { return m<32&&(banned_mask>>m&1); }
static uint32_t sleep_talk_banned(void *u,uint32_t m) { (void)u;notef("stb%u ",m,0);return banned(m); }
static uint32_t assist_banned(void *u,uint32_t m) { (void)u;notef("asb%u ",m,0);return banned(m); }
static uint32_t copycat_banned(void *u,uint32_t m) { (void)u;notef("ccb%u ",m,0);return banned(m); }
static uint32_t me_first_banned(void *u,uint32_t m) { (void)u;notef("mfb%u ",m,0);return banned(m); }
static uint32_t metronome_pool(void *u,uint32_t out) { (void)u;CHECK(out==SCRATCH);note("pool ");w32(NULL,out,pool_count);return 0x9900; }
static uint32_t metronome_pick(void *u,uint32_t p,uint32_t n) { (void)u;notef("pick%x/%x ",p,n);return 0x77; }
static uint32_t party(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("pa%u ",id,0);return PARTY; }
static uint32_t party_member(void *u,uint32_t p,uint32_t i) { (void)u;CHECK(p==PARTY&&i<6);notef("pm%u ",i,0);return PM+0x40*i; }
static uint32_t temp_buffer(void *u,uint32_t c,uint32_t n) { (void)u;CHECK(c==CTX&&n==0x30);note("buf ");return BUFFER; }
static uint32_t fainted(void *u,uint32_t p) { (void)u;notef("f%u ",slot(p),0);return fainted_m>>slot(p)&1; }
static uint32_t can_battle(void *u,uint32_t p) { (void)u;notef("cb%u ",slot(p),0);return able_m>>slot(p)&1; }
static uint32_t terrain(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("ter ");return terrain_v; }
static uint32_t environment(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("env ");return env_v; }
static uint32_t last_move_used(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("lmu ");return last_used; }
static uint32_t last_move(void *u,uint32_t p) { (void)u;notef("lm%u ",slot(p),0);return last_mv; }
static uint32_t opposing_selector(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("opp%u ",id,0);return 0x31; }
static uint32_t battle_format(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("fmt ");return 2; }
static uint32_t target_selector(void *u,uint32_t f,uint32_t s,uint32_t z) { (void)u;CHECK(z==0);notef("ts%u/%x ",f,s);return 0x55; }
static uint32_t turn_flag(void *u,uint32_t p,uint32_t f) { (void)u;notef("tf%u/%u ",slot(p),f);return moved; }
static uint32_t chosen_action(void *u,uint32_t c,uint32_t id,uint32_t out) { (void)u;CHECK(c==CTX&&out==ACTION);notef("act%u ",id,0);
    if(chosen) { w32(NULL,out,chosen); }
    return chosen!=0; }
static uint32_t slot_present(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("sp%u ",id,0);return present; }
static uint32_t charging(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("chg%u ",id,0);return charging_v; }
static uint32_t action_move(void *u,uint32_t a) { (void)u;CHECK(a==ACTION);note("am ");return r32(NULL,a)&0xffff; }
static uint32_t move_damages(void *u,uint32_t m) { (void)u;notef("dmg%u ",m,0);return damaging; }
static void message(void *u,uint32_t at,uint32_t k,uint32_t m) { (void)u;CHECK(at==MSGBUF);notef("msg%x/%u ",m,k); }
static void message_arg(void *u,uint32_t at,uint32_t v) { (void)u;CHECK(at==MSGBUF);notef("arg%x ",v,0); }
static const struct px_battle_moves_11_api api={&base,has_status,move_at,move_flag,random_,choose_target,sleep_talk_banned,assist_banned,
    copycat_banned,me_first_banned,metronome_pool,metronome_pick,party,party_member,temp_buffer,fainted,can_battle,terrain,environment,
    last_move_used,last_move,opposing_selector,battle_format,target_selector,turn_flag,chosen_action,slot_present,charging,action_move,
    move_damages,message,message_arg,SCRATCH,ACTION};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_11_api*);
static const fn dispatched[]={px_battle_copycat_call,px_battle_metronome_call};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w) { (void)u;CHECK(f<2);dispatched[f](h,c,v,w,&api); }
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;log_[0]=0;memset(moves,0,sizeof moves);
    asleep=banned_mask=flag_mask=rnd=party_n=fainted_m=able_m=terrain_v=env_v=last_used=last_mv=moved=chosen=present=charging_v=damaging=pool_count=0;
    listed_n=0;
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i){ w32(NULL,TABLE+0x84+4*i,MON+0x100*i);mem[MON+0x100*i+0x19]=(uint8_t)i; }
}
/* Keys: 0x02 battler (id); 0x0d, 0x0e, 0x12, 0x40, 0x42, 0x53 (values). */
static uint32_t original=7,move_key=0x99;
static void frame(uint32_t battler)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x02,battler,&events);px_battle_event_set_value(0x0d,0x77,&events);
    px_battle_event_set_value(0x0e,original,&events);px_battle_event_set_value(0x12,move_key,&events);
    px_battle_event_set_value(0x40,MSGBUF,&events);px_battle_event_set_value(0x42,0,&events);px_battle_event_set_value(0x53,0,&events);log_[0]=0;
}
static uint32_t key(uint32_t k) { return px_battle_event_get(k,&events); }
static void run(fn f) { frame(ATT);f(HANDLER,CTX,ATT,WORK,&api);CHECK(aborts==0); }
static int untouched(void) { return key(0x0d)==0x77&&key(0x12)==move_key&&key(0x42)==0&&key(0x53)==0; }
static int same(const char *expected) { if(strcmp(log_,expected)){ fprintf(stderr,"log [%s] expected [%s]\n",log_,expected);return 0; } return 1; }
int main(void)
{
    unsigned cases=0;
    const fn all[]={px_battle_sleep_talk_call,px_battle_assist_call,px_battle_metronome_call,px_battle_nature_power_call,px_battle_copycat_call,
        px_battle_me_first_call,px_battle_mirror_move_call,px_battle_bide_announce,px_battle_metronome_announce,px_battle_nature_power_announce};
    for(unsigned k=0;k<sizeof all/sizeof *all;++k)for(uint32_t other=0;other<2;++other) {
        reset();asleep=1;w32(NULL,WORK,1);last_used=5;frame(other?0x102:3);all[k](HANDLER,CTX,ATT,WORK,&api);CHECK(log_[0]==0&&untouched()&&aborts==0);++cases;
    }
    /* Sleep Talk. */
    for(uint32_t n=0;n<5;++n)for(uint32_t bm=0;bm<16;++bm)for(uint32_t fm=0;fm<16;fm+=5) {
        reset();asleep=1;mem[A+0x152]=(uint8_t)n;for(uint32_t i=0;i<n;++i) moves[ATT][i]=10+i;banned_mask=bm<<10;flag_mask=fm<<10;rnd=1;
        run(px_battle_sleep_talk_call);
        uint32_t list[4],k=0;for(uint32_t i=0;i<n;++i) if(!(bm>>i&1)&&!(fm>>i&1)) list[k++]=10+i;
        if(!k){ CHECK(key(0x42)==1&&key(0x12)==0x99);++cases;continue; }
        const uint32_t pick=1;   /* random returns 1; with one entry it indexes past the list (the work area keeps the zeros) */
        const uint32_t mv=pick<k?list[pick]:0;
        CHECK(key(0x12)==mv&&key(0x0d)==0x40+mv%16&&key(0x42)==0);
        for(uint32_t i=0;i<k;++i) { CHECK(r32(NULL,WORK+4*i)==list[i]); }
        ++cases;
    }
    reset();asleep=0;mem[A+0x152]=2;run(px_battle_sleep_talk_call);CHECK(same("hs2/2 ")&&untouched());++cases;
    reset();asleep=1;mem[A+0x152]=2;moves[ATT][0]=10;moves[ATT][1]=11;flag_mask=1u<<11;rnd=0x100;w32(NULL,WORK+4,0x10011);run(px_battle_sleep_talk_call);
    CHECK(same("hs2/2 ma2/0 stb10 mf10/1 ma2/1 stb11 mf11/1 rnd1 ct2/10 ")&&key(0x12)==10);++cases;
    reset();asleep=1;mem[A+0x152]=1;moves[ATT][0]=12;w32(NULL,WORK,0x50000);run(px_battle_sleep_talk_call);CHECK(key(0x12)==12);++cases;   /* uxth */
    reset();asleep=1;mem[A+0x152]=1;moves[ATT][0]=12;rnd=1;w32(NULL,WORK+4,0x10011);run(px_battle_sleep_talk_call);CHECK(key(0x12)==0x11);++cases;   /* a stale word past the list, narrowed */
    /* Assist over party patterns: 0 the user, 1 able, 2 fainted, 3 neither. */
    for(uint32_t count=0;count<5;++count)for(uint32_t pattern=0;pattern<256;++pattern)for(uint32_t bm=0;bm<4;++bm) {
        reset();mem[PARTY+0x18]=(uint8_t)count;rnd=2;banned_mask=bm<<20;
        uint32_t list[32],k=0;
        for(uint32_t i=0;i<4;++i){ const uint32_t kind=pattern>>(2*i)&3;const uint32_t s=8+i;
            mem[PM+0x40*i+0x19]=(uint8_t)(kind==0?ATT:10+i);if(kind==1) able_m|=1u<<s;if(kind==2) fainted_m|=1u<<s;
            mem[PM+0x40*i+0x152]=(uint8_t)(i%3+1);for(uint32_t j=0;j<3;++j) moves[s][j]=20+i*2+j; }
        for(uint32_t i=0;i<count;++i){ const uint32_t kind=pattern>>(2*i)&3;if(kind==0||kind==3) continue;
            for(uint32_t j=0;j<i%3+1;++j){ const uint32_t mv=20+i*2+j;if(!(mv<32&&(banned_mask>>mv&1))) list[k++]=mv; } }
        run(px_battle_assist_call);
        if(!k){ CHECK(untouched());++cases;continue; }
        const uint32_t mv=r16(NULL,BUFFER+2*(2&0xff));const uint32_t want=2<k?list[2]:0;
        CHECK(mv==want&&key(0x12)==want);for(uint32_t i=0;i<k;++i) { CHECK(r16(NULL,BUFFER+2*i)==list[i]); }
        ++cases;
    }
    reset();mem[PARTY+0x18]=2;mem[PM+0x19]=ATT;mem[PM+0x59]=9;fainted_m=1u<<9;mem[PM+0x40+0x152]=1;moves[9][0]=30;rnd=0x100;run(px_battle_assist_call);
    CHECK(same("pa2 buf pm0 pm1 f9 ma9/0 asb30 rnd1 ct2/30 ")&&key(0x12)==30);++cases;
    reset();mem[PARTY+0x18]=1;mem[PM+0x19]=9;able_m=1u<<8;mem[PM+0x152]=1;moves[8][0]=31;run(px_battle_assist_call);CHECK(same("pa2 buf pm0 f8 cb8 ma8/0 asb31 rnd1 ct2/31 "));++cases;
    /* Metronome. */
    reset();pool_count=0x30105;run(px_battle_metronome_call);CHECK(same("pool pick9900/105 ct2/119 ")&&key(0x12)==0x77&&key(0x0d)==0x47);++cases;
    /* Nature Power by terrain, then environment. */
    {
        const uint32_t by_terrain[6]={0,0x19c,0x249,0x55,0x38,0x3a},by_env[9]={0xa1,0x19e,0x198,0x19c,0x38,0x1aa,0x20c,0x3a,0xa1};
        for(uint32_t t=0;t<8;++t)for(uint32_t e=0;e<9;++e)for(uint32_t o=0;o<2;++o) {
            reset();terrain_v=t;env_v=e==8?0x100:e;original=o?0x10a:3;run(px_battle_nature_power_call);
            const uint32_t mv=t>=1&&t<=5?by_terrain[t]:by_env[e<8?e:8];
            CHECK(key(0x12)==mv&&key(0x0d)==(original&0xff));
            CHECK((strstr(log_,"ct2/")!=NULL)==(o==1));CHECK((strstr(log_,"env")!=NULL)==!(t>=1&&t<=5));++cases;
        }
        original=7;
    }
    /* Copycat. */
    for(uint32_t lu=0;lu<3;++lu)for(uint32_t b=0;b<2;++b) {
        reset();last_used=lu==0?0:lu==1?5:0x10005;banned_mask=b<<5;run(px_battle_copycat_call);
        const int ok=last_used!=0&&!(b&&last_used==5);
        CHECK(ok?key(0x12)==last_used&&key(0x0d)==0x40+last_used%16:untouched());++cases;
    }
    /* Me First over its conditions, with and without a target. */
    for(uint32_t o=0;o<2;++o)for(uint32_t pattern=0;pattern<256;++pattern) {
        reset();original=o?0xa:4;listed_n=pattern&1?1:0;listed_ids[0]=5;moved=pattern>>1&1;chosen=pattern&4?0x10009:0;present=pattern>>3&1;
        charging_v=pattern>>4&1;if(!(pattern&32)) chosen=chosen?0x10000:0;damaging=pattern>>6&1;banned_mask=pattern&128?1u<<9:0;
        run(px_battle_me_first_call);
        const int ok=listed_n&&!moved&&(pattern&4)&&present&&!charging_v&&(pattern&32)&&damaging&&!(pattern&128);
        if(ok) CHECK(key(0x12)==9&&key(0x0d)==(o?0x55u:4u)&&key(0x42)==0);else CHECK(key(0x42)==1&&key(0x12)==0x99);
        ++cases;
    }
    original=7;
    reset();original=0xa;listed_n=1;listed_ids[0]=5;chosen=0x10009;present=1;damaging=1;run(px_battle_me_first_call);
    CHECK(same("opp2 fmt ts2/31 sb55 tf5/3 act5 sp5 chg5 am dmg9 mfb9 "));original=7;++cases;
    /* Mirror Move. */
    for(uint32_t n=0;n<2;++n)for(uint32_t lm=0;lm<3;++lm)for(uint32_t f=0;f<2;++f) {
        reset();listed_n=n;listed_ids[0]=6;last_mv=lm==0?0:lm==1?7:0x10007;flag_mask=f<<7;run(px_battle_mirror_move_call);
        if(!n){ CHECK(untouched()&&same("sb7 "));++cases;continue; }
        const int ok=last_mv!=0&&f;
        if(ok) CHECK(key(0x12)==last_mv&&key(0x0d)==7&&key(0x42)==0);else CHECK(key(0x42)==1&&key(0x12)==0x99);
        CHECK(!strncmp(log_,"sb7 lm6 ",8));++cases;
    }
    reset();original=0xa;listed_n=1;listed_ids[0]=6;last_mv=7;flag_mask=1u<<7;run(px_battle_mirror_move_call);
    CHECK(same("opp2 fmt ts2/31 sb55 lm6 mf7/6 ")&&key(0x0d)==0x55);original=7;++cases;
    /* Event 0x19 announcements. */
    for(uint32_t t=0;t<4;++t){ reset();w32(NULL,WORK,t==3?0x101:t);run(px_battle_bide_announce);
        if(t==1) CHECK(same("msg2e9/2 arg2 ")&&key(0x53)==1);else if(t==2) CHECK(same("msg2ec/2 arg2 ")&&key(0x53)==1);else CHECK(log_[0]==0&&key(0x53)==0);
        ++cases; }
    reset();move_key=0x10077;run(px_battle_metronome_announce);CHECK(same("msg7e/1 arg77 ")&&key(0x53)==1);++cases;
    reset();run(px_battle_nature_power_announce);CHECK(same("msg7f/1 arg77 ")&&key(0x53)==1);move_key=0x99;++cases;
    /* Through the dispatcher on event 0x18: Copycat for battler 2, Metronome for battler 3. */
    {
        reset();last_used=5;
        for(unsigned i=0;i<2;++i){ w8(NULL,ENTRIES+0x10*i,0x18);w32(NULL,ENTRIES+0x10*i+4,i); }
        const uint8_t ids[2]={ATT,3};
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=NODES+0x40*i;
            w32(NULL,h+8,ENTRIES+0x10*i);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ids[i]);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,NODES);frame(ATT);px_battle_event_dispatch(CTX,0x18,&events);
        CHECK(same("lmu ccb5 ct2/5 ")&&key(0x12)==5&&aborts==0);++cases;
    }
    printf("battle_moves_11: %u cases, all tests passed\n",cases);
    return 0;
}
