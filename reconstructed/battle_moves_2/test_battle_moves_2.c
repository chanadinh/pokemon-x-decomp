#include "battle_moves_2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, ENTRIES=0x7900, NODES=0x8000 };
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
static uint32_t values[8][0x20],statuses[8];
static unsigned slot(uint32_t p) { CHECK(p>=MON&&p<MON+0x800&&!(p&0xff));return (p-MON)>>8; }
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;CHECK(k<0x20);notef("v%u.%x ",slot(p),k);return values[slot(p)][k]; }
static uint32_t pokemon_status(void *u,uint32_t p) { (void)u;notef("st%u ",slot(p),0);return statuses[slot(p)]; }
static const struct px_battle_ability_api base={&events,pokemon_value,NULL,pokemon_status,NULL,NULL,NULL};
static uint32_t cond,percent,weather_value,power_value,turn_value,last_move,faints,faint_ids[300],allies,locked;
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;notef("hs%u/%u ",slot(p),s);return cond; }
static uint32_t hp_percent(void *u,uint32_t p) { (void)u;notef("hp%u ",slot(p),0);return percent; }
static uint32_t weather(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("w ");return weather_value; }
static uint32_t move_power(void *u,uint32_t m) { (void)u;notef("mp%u ",m,0);return power_value; }
static uint32_t turn(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("turn ");return turn_value; }
static uint32_t turn_last(void *u,uint32_t h,uint32_t t) { (void)u;CHECK(h==CTX+0x20);notef("tl%u ",t,0);return last_move; }
static void effect_flag(void *u,uint32_t c,uint32_t v) { (void)u;CHECK(c==CTX);notef("ef%u ",v,0); }
static uint32_t faint_record(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("fr ");return RECORD; }
static uint32_t faint_count(void *u,uint32_t r,uint32_t k) { (void)u;CHECK(r==RECORD);notef("fc%u ",k,0);return faints; }
static uint32_t last_fi;
static uint32_t faint_id(void *u,uint32_t r,uint32_t k,uint32_t i) { (void)u;CHECK(r==RECORD&&i<300);last_fi=i;notef("fi%u/%u ",k,i);return faint_ids[i]; }
static uint32_t ally_calls,ally_at;
static uint32_t is_ally(void *u,uint32_t id,uint32_t o) { (void)u;notef("al%u/%u ",id,o);if(ally_at) return ++ally_calls==ally_at;return o<32&&(allies>>o&1); }
static uint32_t item_locked(void *u,uint32_t c,uint32_t id,uint32_t t) { (void)u;CHECK(c==CTX);notef("il%u/%u ",id,t);return locked; }
static const struct px_battle_moves_2_api api={&base,has_status,hp_percent,weather,move_power,turn,turn_last,effect_flag,faint_record,
    faint_count,faint_id,is_ally,item_locked};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_2_api*);
static const fn dispatched[]={px_battle_helping_hand_power,px_battle_me_first_power,px_battle_venoshock};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w) { (void)u;CHECK(f<3);dispatched[f](h,c,v,w,&api); }
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;log_[0]=0;memset(values,0,sizeof values);memset(statuses,0,sizeof statuses);memset(faint_ids,0,sizeof faint_ids);
    cond=percent=weather_value=power_value=turn_value=last_move=faints=allies=locked=ally_calls=ally_at=0;
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
/* A frame: attacker 0x03, defender 0x04, type 0x16, power 0x31, multiplier 0x32 (4096 = x1),
 * attack source 0x3c (scaled, 0x1f none), stat key 0x3d (id), 0x3e (scaled), flags 0x53/0x54 (values),
 * the kinds events 0x3e, 0x3f and 0x40 give them. */
static uint32_t type_value=0,stat_key=0,stat_index=0;
static void frame(uint32_t attacker,uint32_t def,uint32_t power)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x03,attacker,&events);px_battle_event_set_id(0x04,def,&events);
    px_battle_event_set_value(0x16,type_value,&events);px_battle_event_set_value(0x31,power,&events);
    px_battle_event_set_multiplier(0x32,0x1000,0,0,&events);px_battle_event_set_scaled(0x3c,0x1f,&events);
    px_battle_event_set_id(0x3d,stat_key,&events);px_battle_event_set_scaled(0x3e,stat_index,&events);
    px_battle_event_set_value(0x53,0,&events);px_battle_event_set_value(0x54,0,&events);log_[0]=0;
}
static uint32_t key(uint32_t k) { return px_battle_event_get(k,&events); }
static uint32_t run(fn f) { frame(ATT,DEF,60);f(HANDLER,CTX,ATT,WORK,&api);CHECK(aborts==0);return key(0x32); }
static int same(const char *expected) { if(strcmp(log_,expected)){ fprintf(stderr,"log [%s] expected [%s]\n",log_,expected);return 0; } return 1; }
/* Nothing else in the frame changed. */
static int rest(uint32_t power,uint32_t mult) { return key(0x31)==power&&key(0x32)==mult&&key(0x3c)==0x1f&&key(0x3d)==stat_key&&key(0x3e)==stat_index&&key(0x53)==0&&key(0x54)==0; }
int main(void)
{
    unsigned cases=0;
    const fn all[]={px_battle_eruption,px_battle_brine,px_battle_charge_power,px_battle_retaliate,px_battle_venoshock,px_battle_fusion,
        px_battle_me_first_power,px_battle_solar_beam_power,px_battle_facade_power,px_battle_knock_off_power,px_battle_foul_play,
        px_battle_psyshock,px_battle_chip_away,px_battle_facade_burn};
    /* Nothing happens unless key 0x03 is the handler's battler. */
    for(unsigned k=0;k<sizeof all/sizeof *all;++k)for(uint32_t other=0;other<3;++other) {
        reset();cond=1;percent=0;weather_value=2;w32(NULL,WORK,2);type_value=12;stat_key=0xb;faints=1;allies=~0u;w16(NULL,HANDLER+0x38,558);last_move=559;
        statuses[ATT]=1;w16(NULL,D+0x12,1);values[ATT][0xd]=values[ATT][0xe]=1;power_value=150;
        const uint32_t key3=other==0?3:other==1?0x102:ATT,id=other==2?0x102:ATT;
        frame(key3,DEF,60);all[k](HANDLER,CTX,id,WORK,&api);
        CHECK(log_[0]==0&&key(0x31)==60&&key(0x32)==0x1000&&key(0x3c)==0x1f&&key(0x3e)==0&&key(0x53)==0&&key(0x54)==0&&aborts==0);++cases;
        type_value=stat_key=0;
    }
    /* Helping Hand compares key 0x03 with work word 0 instead of its own id. */
    for(uint32_t w=0;w<4;++w) {
        reset();const uint32_t worded[4]={ATT,3,0x102,0};w32(NULL,WORK,worded[w]);frame(ATT,DEF,60);
        px_battle_helping_hand_power(HANDLER,CTX,7,WORK,&api);CHECK(key(0x32)==(w==0?0x1800u:0x1000u)&&log_[0]==0&&rest(60,key(0x32)));++cases;
    }
    /* Eruption and Water Spout: base power * HP / max HP, at least 1. */
    for(uint32_t max=1;max<40;max+=3)for(uint32_t hp=0;hp<=max;++hp)for(uint32_t bp=0;bp<160;bp+=13) {
        reset();values[ATT][0xd]=hp;values[ATT][0xe]=max;power_value=bp;w16(NULL,HANDLER+0x38,284);w16(NULL,HANDLER+0x3a,0xffff);
        frame(ATT,DEF,60);px_battle_eruption(HANDLER,CTX,ATT,WORK,&api);
        const uint32_t e=bp*hp/max;CHECK(key(0x31)==(e?e:1)&&key(0x32)==0x1000&&aborts==0);
        CHECK(same("v2.d v2.e mp284 "));++cases;
    }
    reset();values[ATT][0xd]=0x10000;values[ATT][0xe]=3;power_value=0x10001;w16(NULL,HANDLER+0x38,323);frame(ATT,DEF,60);   /* 32-bit product */
    px_battle_eruption(HANDLER,CTX,ATT,WORK,&api);CHECK(key(0x31)==0x10000u/3u&&same("v2.d v2.e mp323 "));++cases;
    reset();values[ATT][0xd]=150;values[ATT][0xe]=0xfffffffe;power_value=150;frame(ATT,DEF,60);   /* unsigned divide */
    px_battle_eruption(HANDLER,CTX,ATT,WORK,&api);CHECK(key(0x31)==1);++cases;
    /* Brine: at most 50% (4096 per percent), signed. */
    {
        const uint32_t p[]={0,1,0x31fff,0x32000,0x32001,0x64000,0x80000000,0xffffffff};
        for(unsigned k=0;k<8;++k){ reset();percent=p[k];CHECK(run(px_battle_brine)==((int32_t)p[k]<=0x32000?0x2000u:0x1000u)&&same("hp5 "));++cases; }
    }
    /* Charge: work word 0 is 2 and the move is Electric. */
    for(uint32_t w=0;w<5;++w)for(uint32_t t=10;t<15;++t) {
        reset();w32(NULL,WORK,w==4?0x102:w);type_value=t;const uint32_t got=run(px_battle_charge_power);
        CHECK(got==(w==2&&t==12?0x2000u:0x1000u)&&log_[0]==0);++cases;
    }
    reset();w32(NULL,WORK,2);type_value=0x10c;CHECK(run(px_battle_charge_power)==0x1000);++cases;
    /* Work word 0 is tested before key 0x03: an absent key 0x03 aborts only when the word is 2. */
    for(uint32_t w=1;w<3;++w){ reset();w32(NULL,WORK,w);px_battle_event_push(&events);px_battle_event_set_value(0x32,0,&events);log_[0]=0;
        px_battle_charge_power(HANDLER,CTX,ATT,WORK,&api);CHECK(aborts==(w==2?1u:0u));++cases; }
    type_value=0;
    /* Retaliate: an ally in faint record kind 1. */
    for(uint32_t n=0;n<5;++n)for(uint32_t mask=0;mask<32;++mask) {
        reset();faints=n;for(uint32_t i=0;i<n;++i) faint_ids[i]=i+1;allies=mask<<1;
        uint32_t hit=0xff;for(uint32_t i=0;i<n;++i) if(mask>>i&1){ hit=i;break; }
        CHECK(run(px_battle_retaliate)==(hit!=0xff?0x2000u:0x1000u));
        char b[256]="fr fc1 ";for(uint32_t i=0;i<n&&i<=hit;++i){ char t[32];snprintf(t,sizeof t,"fi1/%u al2/%u ",i,i+1);strcat(b,t); }
        CHECK(same(b));++cases;
    }
    /* A full byte of indexes: the last one, 255, is reached. */
    reset();faints=256;for(uint32_t i=0;i<256;++i) faint_ids[i]=i==255?4:9;allies=1u<<4;CHECK(run(px_battle_retaliate)==0x2000);++cases;
    /* A count above 255 revisits index 0 (the byte index wraps). */
    reset();faints=0x101;for(uint32_t i=0;i<256;++i) faint_ids[i]=i;ally_at=257;
    CHECK(run(px_battle_retaliate)==0x2000&&ally_calls==257&&last_fi==0);++cases;
    /* Venoshock. */
    for(uint32_t c=0;c<3;++c){ reset();cond=c==2?0x100:c;CHECK(run(px_battle_venoshock)==(cond?0x2000u:0x1000u)&&same("hs5/5 "));++cases; }
    reset();cond=1;frame(ATT,0x300+DEF,60);px_battle_venoshock(HANDLER,CTX,ATT,WORK,&api);CHECK(key(0x32)==0x2000&&same("hs5/5 "));++cases;
    /* Fusion Flare and Fusion Bolt: the other one used earlier this turn. */
    {
        const uint32_t moves[]={558,559,557,560,0x22e + 0x10000};
        for(unsigned k=0;k<4;++k)for(uint32_t last=556;last<562;++last) {
            reset();w16(NULL,HANDLER+0x38,(uint16_t)moves[k]);w16(NULL,HANDLER+0x3a,0xffff);turn_value=4;last_move=last;
            const int fused=(moves[k]==558&&last==559)||(moves[k]==559&&last==558);
            CHECK(run(px_battle_fusion)==(fused?0x2000u:0x1000u));
            CHECK(same(k>=2?"":fused?"turn tl4 ef1 ":"turn tl4 "));++cases;
        }
        reset();w16(NULL,HANDLER+0x38,558);last_move=559+0x10000;CHECK(run(px_battle_fusion)==0x1000);++cases;
    }
    /* Me First. */
    reset();CHECK(run(px_battle_me_first_power)==0x1800&&log_[0]==0);++cases;
    /* Solar Beam: halved in rain, hail and sandstorm. */
    for(uint32_t w=0;w<7;++w){ reset();weather_value=w==6?0x102:w;CHECK(run(px_battle_solar_beam_power)==(w>=2&&w<=4?0x800u:0x1000u)&&same("w "));++cases; }
    /* Facade: poison, paralysis, burn. */
    for(uint32_t s=0;s<9;++s){ reset();statuses[ATT]=s==8?0x101:s;statuses[DEF]=4;CHECK(run(px_battle_facade_power)==(s==1||s==4||s==5?0x2000u:0x1000u)&&same("st2 "));++cases; }
    /* Knock Off: a held item that is not locked. */
    for(uint32_t item=0;item<3;++item)for(uint32_t l=0;l<3;++l) {
        reset();w16(NULL,D+0x12,(uint16_t)(item==2?0x8000:item));w16(NULL,A+0x12,1);locked=l==2?0x100:l;
        CHECK(run(px_battle_knock_off_power)==(item&&!locked?0x1800u:0x1000u));CHECK(same(item?"il2/5 ":""));++cases;
    }
    reset();w16(NULL,D+0x12,5);frame(ATT,0x400+DEF,60);px_battle_knock_off_power(HANDLER,CTX,ATT,WORK,&api);CHECK(key(0x32)==0x1800&&same("il2/5 "));++cases;
    /* Foul Play: key 0x3c becomes the defender's id (low byte). */
    for(uint32_t d=0;d<3;++d){ reset();const uint32_t k4=d==0?DEF:d==1?0x300+DEF:0xff;frame(ATT,k4,60);px_battle_foul_play(HANDLER,CTX,ATT,WORK,&api);
        CHECK(key(0x3c)==(k4&0xff)&&log_[0]==0&&key(0x31)==60);++cases; }
    /* Psyshock, Psystrike, Secret Sword: with key 0x3d 0xb, key 0x3e + 1. */
    for(uint32_t s=0;s<16;++s)for(uint32_t i=0;i<3;++i) {
        reset();stat_key=s==15?0x10b:s;stat_index=i==2?0xffffffff:i;frame(ATT,DEF,60);px_battle_psyshock(HANDLER,CTX,ATT,WORK,&api);
        CHECK(key(0x3e)==(stat_key==0xb?stat_index+1:stat_index)&&key(0x3d)==stat_key&&key(0x31)==60);++cases;
    }
    stat_key=stat_index=0;
    /* Chip Away and Facade's burn exception. */
    reset();frame(ATT,DEF,60);px_battle_chip_away(HANDLER,CTX,ATT,WORK,&api);CHECK(key(0x53)==1&&key(0x54)==0&&key(0x31)==60);++cases;
    reset();frame(ATT,DEF,60);px_battle_facade_burn(HANDLER,CTX,ATT,WORK,&api);CHECK(key(0x54)==1&&key(0x53)==0&&key(0x31)==60);++cases;
    /* A key rewritten earlier in the frame stays (value entries change once). */
    reset();frame(ATT,DEF,60);px_battle_event_rewrite(0x54,7,&events);px_battle_facade_burn(HANDLER,CTX,ATT,WORK,&api);CHECK(key(0x54)==7);++cases;
    /* Through the dispatcher on event 0x3d: Helping Hand (x1.5), Me First (x1.5) and Venoshock for another battler. */
    {
        reset();
        for(unsigned i=0;i<3;++i){ w8(NULL,ENTRIES+0x10*i,0x3d);w32(NULL,ENTRIES+0x10*i+4,i); }
        const uint8_t ids[3]={6,ATT,3};
        for(unsigned i=0;i<3;++i) {
            const uint32_t h=NODES+0x40*i;
            w32(NULL,h+8,ENTRIES+0x10*i);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ids[i]);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<2?h+0x40:0);
        }
        w32(NULL,NODES+0x1c,ATT);cond=1;w32(NULL,BATTLE+0x60,NODES);
        frame(ATT,DEF,60);px_battle_event_dispatch(CTX,0x3d,&events);
        CHECK(key(0x32)==0x2400&&log_[0]==0&&aborts==0);++cases;
    }
    printf("battle_moves_2: %u cases, all tests passed\n",cases);
    return 0;
}
