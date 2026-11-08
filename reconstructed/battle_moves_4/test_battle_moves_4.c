#include "battle_moves_4.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, COUNTS=0x7800, ENTRIES=0x7900, NODES=0x8000 };
enum { ATT=2, DEF=5, A=MON+0x200, D=MON+0x500 };
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
static uint32_t values[8][0x20];
static unsigned slot(uint32_t p) { CHECK(p>=MON&&p<MON+0x800&&!(p&0xff));return (p-MON)>>8; }
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;CHECK(k<0x20);notef("v%u.%x ",slot(p),k);return values[slot(p)][k]; }
static const struct px_battle_ability_api base={&events,pokemon_value,NULL,NULL,NULL,NULL,NULL};
static const struct px_battle_handler_api handlers={&memory,NULL,abort_battle,NULL,NULL,NULL,BATTLE,POOL,0,0,0,0,COUNTS};
static uint32_t weather_value,switching,status,typed;
static uint32_t weather(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("w ");return weather_value; }
static uint32_t target_switching(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("sw ");return switching; }
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;notef("hs%u/%u ",slot(p),s);return status; }
static uint32_t has_type(void *u,uint32_t p,uint32_t t) { (void)u;notef("ht%u/%u ",slot(p),t);return typed; }
static const struct px_battle_moves_4_api api={&base,&handlers,weather,target_switching,has_status,has_type};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_4_api*);
static const fn dispatched[]={px_battle_endure_end,px_battle_helping_hand_end,px_battle_kings_shield_end};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w) { (void)u;CHECK(f<3);dispatched[f](h,c,v,w,&api); }
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;log_[0]=0;memset(values,0,sizeof values);weather_value=switching=status=typed=0;
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
    for(unsigned i=0;i<PX_BATTLE_HANDLER_POOL;++i)w32(NULL,POOL+4*i,NODES+0x40*i);
}
/* Frame keys: 0x02 battler, 0x03 attacker, 0x04 defender (ids); 0x22 state (id); 0x43, 0x50, 0x53, 0x20, 0x21 values. */
static uint32_t state=0,stages=1;
static void frame(uint32_t battler,uint32_t attacker,uint32_t def)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x02,battler,&events);px_battle_event_set_id(0x03,attacker,&events);
    px_battle_event_set_id(0x04,def,&events);px_battle_event_set_id(0x22,state,&events);px_battle_event_set_value(0x43,1,&events);
    px_battle_event_set_value(0x50,1,&events);px_battle_event_set_value(0x53,0,&events);px_battle_event_set_value(0x20,30,&events);
    px_battle_event_set_value(0x21,stages,&events);log_[0]=0;
}
static uint32_t key(uint32_t k) { return px_battle_event_get(k,&events); }
/* Keys other than `except` keep their starting values. */
static int rest(uint32_t except)
{
    const uint32_t keys[]={0x43,0x50,0x53,0x20,0x21},start[]={1,1,0,30,stages};
    for(unsigned i=0;i<5;++i) if(keys[i]!=except&&key(keys[i])!=start[i]) return 0;
    return 1;
}
static void run(fn f) { frame(ATT,ATT,DEF);f(HANDLER,CTX,ATT,WORK,&api);CHECK(aborts==0); }
static int same(const char *expected) { if(strcmp(log_,expected)){ fprintf(stderr,"log [%s] expected [%s]\n",log_,expected);return 0; } return 1; }
int main(void)
{
    unsigned cases=0;
    const fn keyed[]={px_battle_sky_uppercut_hit,px_battle_thunder_hit,px_battle_gust_hit,px_battle_earthquake_hit,px_battle_whirlpool_hit,
        px_battle_surf_hit,px_battle_heal_bell_hit,px_battle_rototiller_hit,px_battle_blizzard_accuracy,px_battle_thunder_accuracy,
        px_battle_pursuit_accuracy,px_battle_stomp_accuracy,px_battle_growth_stages,px_battle_rototiller_target,px_battle_venom_drench_target,
        px_battle_magnetic_flux_target};
    /* Nothing happens unless key 0x03 is the handler's battler. */
    for(unsigned k=0;k<sizeof keyed/sizeof *keyed;++k)for(uint32_t other=0;other<3;++other) {
        reset();weather_value=1;switching=1;mem[D+0x169+1]=1;state=3;
        const uint32_t key3=other==0?3:other==1?0x102:ATT,id=other==2?0x102:ATT;
        frame(id,key3,DEF);keyed[k](HANDLER,CTX,id,WORK,&api);CHECK(log_[0]==0&&rest(0)&&aborts==0);++cases;state=0;
    }
    /* Event 0xa5: the state each move reaches. */
    {
        const fn f[6]={px_battle_sky_uppercut_hit,px_battle_thunder_hit,px_battle_gust_hit,px_battle_earthquake_hit,px_battle_whirlpool_hit,px_battle_surf_hit};
        const uint32_t reach[6]={3,3,3,5,4,4};
        for(unsigned k=0;k<6;++k)for(uint32_t s=0;s<9;++s) {
            reset();state=s==8?0x100+reach[k]:s;run(f[k]);CHECK(key(0x43)==(state==reach[k]?0u:1u)&&rest(0x43)&&log_[0]==0);++cases;
        }
        for(uint32_t s=0;s<6;++s){ reset();state=s;run(px_battle_heal_bell_hit);CHECK(key(0x43)==0&&rest(0x43));++cases;
            reset();state=s;run(px_battle_rototiller_hit);CHECK(key(0x50)==0&&rest(0x50));++cases; }
        state=0;
    }
    /* Event 0x37: never miss in hail, in rain, against a switching target or a minimized defender. */
    for(uint32_t w=0;w<6;++w){ reset();weather_value=w==5?0x103:w;run(px_battle_blizzard_accuracy);CHECK(key(0x53)==(w==3)&&rest(0x53)&&same("w "));++cases;
        reset();weather_value=w==5?0x102:w;run(px_battle_thunder_accuracy);CHECK(key(0x53)==(w==2)&&rest(0x53)&&same("w "));++cases; }
    for(uint32_t s=0;s<3;++s){ reset();switching=s==2?0x100:s;run(px_battle_pursuit_accuracy);CHECK(key(0x53)==(s!=0)&&same("sw "));++cases; }
    for(int bit=-1;bit<24;++bit) {
        reset();if(bit>=0) mem[D+0x169+bit/8]=(uint8_t)(1u<<(bit%8));mem[A+0x169+1]=1;
        run(px_battle_stomp_accuracy);CHECK(key(0x53)==(bit==8)&&rest(0x53));++cases;
    }
    reset();mem[D+0x16a]=1;frame(ATT,ATT,0x300+DEF);px_battle_stomp_accuracy(HANDLER,CTX,ATT,WORK,&api);CHECK(key(0x53)==1);++cases;
    /* Event 0x62. */
    for(uint32_t w=0;w<4;++w)for(uint32_t s=0;s<4;++s) {
        reset();weather_value=w;stages=s==3?0x101:s;run(px_battle_growth_stages);
        CHECK(key(0x21)==(w==1&&stages==1?2u:stages)&&rest(0x21)&&same("w "));
        /* In sun key 0x21 was rewritten (a value changes once), otherwise it can still change. */
        px_battle_event_rewrite(0x21,77,&events);CHECK(key(0x21)==(w==1?(stages==1?2u:stages):77u));++cases;
    }
    stages=1;
    for(uint32_t t=0;t<3;++t){ reset();typed=t==2?0x100:t;run(px_battle_rototiller_target);CHECK(key(0x20)==(typed?30u:0u)&&rest(0x20)&&same("ht5/11 "));++cases; }
    for(uint32_t t=0;t<3;++t){ reset();status=t==2?0x100:t;run(px_battle_venom_drench_target);CHECK(key(0x20)==(status?30u:0u)&&rest(0x20)&&same("hs5/5 "));++cases; }
    {
        const uint32_t ab[]={0,56,57,58,59,0x139,0xffffffff};
        for(unsigned k=0;k<7;++k){ reset();values[DEF][0x11]=ab[k];values[ATT][0x11]=57;run(px_battle_magnetic_flux_target);
            CHECK(key(0x20)==(ab[k]==57||ab[k]==58?30u:0u)&&rest(0x20)&&same("v5.11 "));++cases; }
    }
    reset();frame(ATT,ATT,0x300+DEF);typed=0;px_battle_rototiller_target(HANDLER,CTX,ATT,WORK,&api);CHECK(same("ht5/11 "));++cases;
    /* AncientPower family: key 0x04 and key 0x03 both the handler's battler, key 0x04 tested first. */
    for(uint32_t d=0;d<3;++d)for(uint32_t a=0;a<3;++a) {
        const uint32_t ids[3]={ATT,3,0x102};reset();frame(ATT,ids[a],ids[d]);px_battle_ancient_power_chance(HANDLER,CTX,ATT,WORK,&api);
        CHECK(key(0x20)==(a==0&&d==0?10u:30u)&&rest(0x20)&&aborts==0);++cases;
    }
    reset();px_battle_event_push(&events);px_battle_event_set_id(0x04,3,&events);px_battle_event_set_value(0x20,30,&events);   /* no key 0x03 */
    px_battle_ancient_power_chance(HANDLER,CTX,ATT,WORK,&api);CHECK(aborts==0);++cases;
    reset();px_battle_event_push(&events);px_battle_event_set_id(0x04,ATT,&events);px_battle_event_set_value(0x20,30,&events);
    px_battle_ancient_power_chance(HANDLER,CTX,ATT,WORK,&api);CHECK(aborts==1);++cases;
    /* Event 0x7f: removal of a registered handler for its battler (0x1f for King's Shield and Spiky Shield). */
    {
        const fn f[5]={px_battle_endure_end,px_battle_helping_hand_end,px_battle_snatch_end,px_battle_magic_coat_end,px_battle_kings_shield_end};
        for(unsigned k=0;k<5;++k)for(uint32_t b=0;b<4;++b) {
            reset();const uint32_t battlers[4]={ATT,3,0x1f,0x102};
            const uint32_t h=px_battle_handler_register(0,203,1,0,ATT,ENTRIES,1,&handlers);
            const uint32_t other=px_battle_handler_register(0,203,1,1,ATT,ENTRIES,1,&handlers);
            frame(battlers[b],ATT,DEF);f[k](h,CTX,ATT,h+0x1c,&api);
            const int removed=k==4?battlers[b]==0x1f:battlers[b]==ATT;
            CHECK((r32(NULL,h+0x18)==0)==removed&&r32(NULL,other+0x18)!=0&&r32(NULL,BATTLE+0x64)==(removed?1u:2u)&&rest(0)&&aborts==0);++cases;
        }
    }
    /* Follow Me and Rage Powder: every move handler of the battler with the same move, without a key test. */
    {
        reset();
        uint32_t h[6];const uint32_t owners[6]={0,0,0,4,0,0},vals[6]={ATT,ATT,3,ATT,ATT,ATT},ids[6]={266,33,266,266,266,10};   /* 10 shares 266's low byte */
        for(unsigned i=0;i<6;++i) h[i]=px_battle_handler_register(owners[i],ids[i],1,i,vals[i],ENTRIES,1,&handlers);
        w16(NULL,HANDLER+0x38,266);w16(NULL,HANDLER+0x3a,0xffff);px_battle_follow_me_end(HANDLER,CTX,ATT,WORK,&api);
        for(unsigned i=0;i<6;++i) { CHECK((r32(NULL,h[i]+0x18)==0)==(i==0||i==4)); }
        CHECK(r32(NULL,BATTLE+0x64)==4&&aborts==0&&log_[0]==0);++cases;
        reset();w16(NULL,HANDLER+0x38,266);px_battle_follow_me_end(HANDLER,CTX,ATT,WORK,&api);CHECK(r32(NULL,BATTLE+0x64)==0&&aborts==0);++cases;
    }
    /* Through the dispatcher on event 0x7f: Endure and Helping Hand for battler 2 are removed after running, King's
     * Shield only on 0x1f; one for battler 3 stays. */
    for(uint32_t b=0;b<2;++b) {
        reset();
        for(unsigned i=0;i<3;++i){ w8(NULL,ENTRIES+0x10*i,0x7f);w32(NULL,ENTRIES+0x10*i+4,i); }
        const uint32_t endure=px_battle_handler_register(0,203,1,4,ATT,ENTRIES,1,&handlers);
        const uint32_t help=px_battle_handler_register(0,270,1,3,ATT,ENTRIES+0x10,1,&handlers);
        const uint32_t shield=px_battle_handler_register(0,588,1,2,ATT,ENTRIES+0x20,1,&handlers);
        const uint32_t other=px_battle_handler_register(0,203,1,1,3,ENTRIES,1,&handlers);
        frame(b?0x1f:ATT,ATT,DEF);px_battle_event_dispatch(CTX,0x7f,&events);
        CHECK(aborts==0&&r32(NULL,other+0x18)!=0);
        if(b) CHECK(r32(NULL,shield+0x18)==0&&r32(NULL,endure+0x18)!=0&&r32(NULL,help+0x18)!=0&&r32(NULL,BATTLE+0x64)==3);
        else CHECK(r32(NULL,shield+0x18)!=0&&r32(NULL,endure+0x18)==0&&r32(NULL,help+0x18)==0&&r32(NULL,BATTLE+0x64)==2);
        ++cases;
    }
    printf("battle_moves_4: %u cases, all tests passed\n",cases);
    return 0;
}
