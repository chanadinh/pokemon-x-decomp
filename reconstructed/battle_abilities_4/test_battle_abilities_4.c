#include "battle_abilities_4.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, ME=2, OTHER=5 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static unsigned aborts;static char log_[512];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
/* Inputs: one value per dimension. */
enum { WHO, CAT, STATUS, CONFUSED, ALLY, EXCLUDED, FLAG, HELD, WORKW, WORKC, K1B, K2C, K15, K2D, K2A, K4E, K45, K2E, K27, DIMS };
static const uint32_t values[DIMS][8]={{0,1,2,3,4,5,6,7},{0,1,2,0x101},{0,4},{0,1},{0,1},{0,1},{0,1},{0,0x55},{0,1,2},{0,1},{0,1,0x100},{0,0x32,0x33,0x100},
    {7,6,0x107},{0,1,0xff},{1,2,0x101},{0,1},{0,1},{0,1},{0x30,0x8000,0x10001}};
static const unsigned counts[DIMS]={8,4,2,2,2,2,2,2,3,2,3,4,3,3,3,2,2,2,3};
static uint32_t in[DIMS];
static uint32_t move_category(void *u,uint32_t m) { (void)u;notef("cat%x ",m,0);return in[CAT]; }
static uint32_t pokemon_status(void *u,uint32_t p) { (void)u;notef("st%u ",mon(p),0);return in[STATUS]; }
static const struct px_battle_ability_api base={&events,NULL,move_category,pokemon_status,NULL,NULL,NULL};
static uint32_t is_ally(void *u,uint32_t id,uint32_t o) { (void)u;notef("ally%u/%u ",id,o);return in[ALLY]; }
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;notef("hs%u/%u ",mon(p),s);return s==6&&in[CONFUSED]; }
static uint32_t move_flag(void *u,uint32_t m,uint32_t f) { (void)u;notef("mf%x/%u ",m,f);return in[FLAG]; }
static uint32_t bond_excluded(void *u,uint32_t m) { (void)u;notef("bx%x ",m,0);return in[EXCLUDED]; }
static const struct px_battle_abilities_4_api api={&base,is_ally,has_status,move_flag,bond_excluded};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_abilities_4_api*);
enum { COMPOUND, VICTORY, HUSTLE, TANGLED, WONDER, KEEN, UNAWARE_S, UNAWARE_D, UNAWARE_A, SCRAPPY, SUPER_LUCK, ARMOR, STALL, QUICK, UNBURDEN,
       SLOW, UNBURDEN_ITEM, HEAVY, LIGHT, MAGIC_GUARD, ROCK_HEAD, SKILL_LINK, BOND, ADAPT, INNER, SERENE, SERENE_FLINCH, COUNT };
static const fn handlers[COUNT]={px_battle_compound_eyes,px_battle_victory_star,px_battle_hustle_accuracy,px_battle_tangled_feet,px_battle_wonder_skin,
    px_battle_keen_eye,px_battle_unaware_stages,px_battle_unaware_defense,px_battle_unaware_attack,px_battle_scrappy,px_battle_super_luck,
    px_battle_battle_armor,px_battle_stall,px_battle_quick_feet,px_battle_unburden,px_battle_slow_start_speed,px_battle_unburden_item,
    px_battle_heavy_metal,px_battle_light_metal,px_battle_magic_guard,px_battle_rock_head,px_battle_skill_link,px_battle_parental_bond,
    px_battle_adaptability,px_battle_inner_focus,px_battle_serene_grace,px_battle_serene_grace_flinch};
/* Relevant dimensions of each handler (beyond WHO). */
#define B(d) (1u<<(d))
static const uint32_t relevant[COUNT]={0,B(ALLY),B(CAT),B(CONFUSED),B(K1B)|B(K2C),0,0,0,0,B(K15),B(K2D),0,0,B(STATUS),B(WORKW)|B(HELD),B(WORKC),
    B(K2E)|B(HELD),0,0,0,0,0,B(K2A)|B(K4E)|B(EXCLUDED)|B(FLAG),B(K45),0,B(K27),0};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;CHECK(f<COUNT);handlers[f](h,c,v,w,&api);
}
/* Rewritable keys and their starting values. */
static const uint32_t keys[]={0x53,0x42,0x11,0x4c,0x29,0x28,0x2b,0x46};
#define NKEYS (sizeof keys/sizeof keys[0])
static uint32_t start_of(uint32_t key) { return 0x700+key; }
static void setup(unsigned h)
{
    memset(mem,0,sizeof mem);aborts=0;w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
    w16(NULL,MON+0x100*ME+0x12,(uint16_t)in[HELD]);w32(NULL,WORK,in[WORKW]);w32(NULL,WORK+0xc,in[WORKC]);
    px_battle_event_push(&events);
    px_battle_event_set_id(0x02,in[WHO]&1?ME:OTHER,&events);px_battle_event_set_id(0x03,in[WHO]&2?ME:OTHER,&events);
    px_battle_event_set_id(0x04,in[WHO]&4?ME:OTHER,&events);
    px_battle_event_set_value(0x12,0x10088,&events);
    px_battle_event_set_value(0x1b,in[K1B],&events);px_battle_event_set_value(0x2c,in[K2C],&events);px_battle_event_set_value(0x15,in[K15],&events);
    px_battle_event_set_value(0x2d,in[K2D],&events);px_battle_event_set_value(0x2a,in[K2A],&events);px_battle_event_set_value(0x4e,in[K4E],&events);
    px_battle_event_set_value(0x45,in[K45],&events);px_battle_event_set_value(0x2e,in[K2E],&events);px_battle_event_set_value(0x27,in[K27],&events);
    for(unsigned k=0;k<NKEYS;++k)px_battle_event_set_value(keys[k],start_of(keys[k]),&events);
    /* Adaptability rewrites key 0x36, which is then a plain value. */
    if(h==ADAPT)px_battle_event_set_value(0x36,0x1000,&events);else px_battle_event_set_multiplier(0x36,0x1000,0,0x100000,&events);
    log_[0]=0;
}
struct outcome { uint32_t mult,key[0x60],work; char log[96]; };
static struct outcome model(unsigned h)
{
    struct outcome o;memset(&o,0,sizeof o);o.mult=0x1000;o.work=in[WORKW];
    for(unsigned k=0;k<NKEYS;++k)o.key[keys[k]]=start_of(keys[k]);
    o.key[0x2d]=in[K2D];o.key[0x2c]=in[K2C];o.key[0x27]=in[K27];
    const int k2=in[WHO]&1,k3=in[WHO]>>1&1,k4=in[WHO]>>2&1;char *L=o.log;const size_t n=sizeof o.log;
    switch(h){
    case COMPOUND: if(k3)o.mult=0x14cd;break;
    case VICTORY: snprintf(L,n,"ally2/%u ",k3?ME:OTHER);if(in[ALLY])o.mult=0x119a;break;
    case HUSTLE: if(k3){snprintf(L,n,"cat88 ");if(in[CAT]==1)o.mult=0xccd;}break;
    case TANGLED: if(k4){snprintf(L,n,"hs2/6 ");if(in[CONFUSED])o.mult=0x800;}break;
    case WONDER: if(k4&&in[K1B]==0&&in[K2C]>0x32)o.key[0x2c]=0x32;break;
    case KEEN: if(k3)o.key[0x4c]=1;break;
    case UNAWARE_S: if(k3)o.key[0x29]=6;else if(k4)o.key[0x28]=6;break;
    case UNAWARE_D: if(k4)o.key[0x53]=1;break;
    case UNAWARE_A: if(k3)o.key[0x53]=1;break;
    case SCRAPPY: if(k3&&in[K15]==7)o.key[0x4c]=1;break;
    case SUPER_LUCK: if(k3)o.key[0x2d]=(in[K2D]+1)&0xff;break;
    case ARMOR: if(k4)o.key[0x42]=1;break;
    case STALL: if(k2)o.key[0x11]=0;break;
    case QUICK: if(k2){snprintf(L,n,"st2 ");if(in[STATUS]){o.mult=0x1800;o.key[0x53]=0;}}break;
    case UNBURDEN: if(k2&&in[WORKW]==1&&!in[HELD])o.mult=0x2000;break;
    case SLOW: if(k2&&in[WORKC])o.mult=0x800;break;
    case UNBURDEN_ITEM: if(k2&&!in[K2E]&&in[HELD])o.work=1;break;
    case HEAVY: if(k2)o.mult=0x2000;break;
    case LIGHT: if(k2)o.mult=0x800;break;
    case MAGIC_GUARD: if(k4)o.key[0x53]=0;break;
    case ROCK_HEAD: if(k3)o.key[0x42]=1;break;
    case SKILL_LINK: if(k3)o.key[0x53]=1;break;
    case BOND: if(k3&&in[K2A]==1&&!in[K4E]){snprintf(L,n,"bx88 ");if(!in[EXCLUDED]){snprintf(L,n,"bx88 mf88/1 ");if(!in[FLAG])o.key[0x2b]=2;}}break;
    case ADAPT: if(k2&&in[K45])o.mult=0x2000;break;
    case INNER: if(k4)o.key[0x42]=1;break;
    case SERENE: if(k3)o.key[0x27]=in[K27]<<1&0xffff;break;
    case SERENE_FLINCH: if(k3)o.key[0x46]=1;break;
    }
    return o;
}
static void check(unsigned h)
{
    setup(h);handlers[h](HANDLER,CTX,ME,WORK,&api);
    const struct outcome o=model(h);
    if(strcmp(log_,o.log)||aborts){fprintf(stderr,"handler %u: got '%s' want '%s'\n",h,log_,o.log);CHECK(0);}
    CHECK(px_battle_event_get(0x36,&events)==o.mult&&r32(NULL,WORK)==o.work);
    for(unsigned k=0;k<NKEYS;++k)CHECK(px_battle_event_get(keys[k],&events)==o.key[keys[k]]);
    CHECK(px_battle_event_get(0x2d,&events)==o.key[0x2d]&&px_battle_event_get(0x2c,&events)==o.key[0x2c]&&px_battle_event_get(0x27,&events)==o.key[0x27]);
}
int main(void)
{
    unsigned cases=0;uint32_t seed=1;
    for(unsigned h=0;h<COUNT;++h) {
        const uint32_t mask=relevant[h]|B(WHO);
        unsigned total=1;for(unsigned d=0;d<DIMS;++d)if(mask&B(d))total*=counts[d];
        for(unsigned index=0;index<total;++index)for(unsigned noise=0;noise<4;++noise) {
            unsigned rest=index;
            for(unsigned d=0;d<DIMS;++d) {
                if(mask&B(d)){in[d]=values[d][rest%counts[d]];rest/=counts[d];}
                else if(noise==0)in[d]=values[d][0];
                else{seed=seed*1103515245u+12345u;in[d]=values[d][(seed>>16)%counts[d]];}
            }
            check(h);++cases;
        }
    }
    /* Through the dispatcher: Compound Eyes then Victory Star on event 0x39. */
    {
        memset(in,0,sizeof in);in[WHO]=2;in[ALLY]=1;in[K15]=7;in[K2A]=1;in[K27]=0x30;setup(COMPOUND);
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7c00+0x10*i;
            w8(NULL,entries,0x39);w32(NULL,entries+4,i?VICTORY:COMPOUND);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ME);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);px_battle_event_dispatch(CTX,0x39,&events);
        /* 0x1000 * 0x14cd >> 12 = 0x14cd; then * 0x119a rounded. */
        const uint32_t want=(uint32_t)(((uint64_t)0x14cd*0x119a+0x800)>>12);
        CHECK(!strcmp(log_,"ally2/2 ")&&aborts==0&&px_battle_event_get(0x36,&events)==want);++cases;
    }
    printf("battle_abilities_4: %u cases, all tests passed\n",cases);
    return 0;
}
