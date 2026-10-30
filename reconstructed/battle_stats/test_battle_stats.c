#include "battle_stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, CTX=0x100, TABLE=0x1000, ATTACKER=0x2000, DEFENDER=0x2400, OTHER=0x2800, MOVE=0x3000,
       FIELD=0x3100, FLAG=0x3200, OUTPUT=0x3300, RULES=0x3400, OWNER=0x3500 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
struct scenario { uint32_t category,source_index,ignore,swap,suppress,multiplier,flag54,scale34,rock,aborts;
                  char log[2048];unsigned depth,max_depth;uint32_t last34,last35,stat_calls,raw_calls; };
static struct scenario sc;
static void note(const char *s) { strncat(sc.log,s,sizeof sc.log-strlen(sc.log)-1); }
static void push(void *u) { (void)u;note("(");if(++sc.depth>sc.max_depth)sc.max_depth=sc.depth; }
static void pop(void *u) { (void)u;note(")");CHECK(sc.depth>0);--sc.depth; }
static void set_id(void *u,uint32_t k,uint32_t v) { (void)u;char b[24];snprintf(b,sizeof b,"i%x=%x ",k,v);note(b); }
static void set_value(void *u,uint32_t k,uint32_t v) { (void)u;char b[24];snprintf(b,sizeof b,"v%x=%x ",k,v);note(b); }
static void set_scaled(void *u,uint32_t k,uint32_t v) { (void)u;char b[24];snprintf(b,sizeof b,"s%x=%x ",k,v);note(b);if(k==0x34)sc.last34=v;if(k==0x35)sc.last35=v; }
static void set_multiplier(void *u,uint32_t k,uint32_t v,uint32_t lo,uint32_t hi) { (void)u;char b[40];snprintf(b,sizeof b,"m%x=%x/%x/%x ",k,v,lo,hi);note(b); }
static void dispatch(void *u,uint32_t c,uint32_t e) { (void)u;CHECK(c==CTX);char b[16];snprintf(b,sizeof b,"E%x ",e);note(b); }
static uint32_t get(void *u,uint32_t k)
{
    (void)u;char b[16];snprintf(b,sizeof b,"g%x ",k);note(b);
    switch(k) {
    case 0x3c: return sc.source_index;
    case 0x53: return sc.ignore;
    case 0x3e: return sc.swap;
    case 0x42: return sc.suppress;
    case 0x34: return sc.scale34?sc.scale34:sc.last34;
    case 0x35: return sc.last35;
    case 0x36: return sc.multiplier;
    case 0x54: return sc.flag54;
    default: CHECK(0);return 0;
    }
}
static uint32_t base_of(uint32_t p,uint32_t k) { return (p==ATTACKER?100:p==DEFENDER?200:p==OTHER?300:400)+k; }
static uint32_t stat(void *u,uint32_t p,uint32_t k) { (void)u;++sc.stat_calls;note("st ");return base_of(p,k)*2; }
static uint32_t raw_stat(void *u,uint32_t p,uint32_t k) { (void)u;++sc.raw_calls;note("raw ");return base_of(p,k); }
static uint32_t stage_key(void *u,uint32_t p,uint32_t k) { (void)u;(void)p;note("sk ");return k; }
static uint32_t has_type(void *u,uint32_t p,uint32_t t) { (void)u;CHECK(p==DEFENDER&&t==5);note("ht ");return sc.rock; }
static uint32_t category(void *u,uint32_t m) { (void)u;CHECK(m==55);note("C ");return sc.category; }
static void abort_battle(void *u) { (void)u;note("ABORT ");++sc.aborts; }
static const struct px_battle_stats_api api={&memory,NULL,push,pop,set_id,set_value,set_scaled,set_multiplier,dispatch,get,
                                             stat,raw_stat,stage_key,has_type,category,abort_battle};
static void defaults(void)
{
    memset(&sc,0,sizeof sc);memset(mem,0,sizeof mem);sc.source_index=0x1f;sc.multiplier=0x1000;sc.category=1;
    w8(NULL,ATTACKER+0x19,1);w8(NULL,DEFENDER+0x19,4);w8(NULL,OTHER+0x19,2);w16(NULL,MOVE,55);w8(NULL,MOVE+6,3);w8(NULL,MOVE+7,1);
    w32(NULL,CTX+8,TABLE);w32(NULL,CTX+0x1c,FIELD);for(unsigned i=0;i<4;++i)w8(NULL,ATTACKER+0x104+i,6),w8(NULL,DEFENDER+0x104+i,6),w8(NULL,OTHER+0x104+i,6);
}
static uint32_t mod(uint32_t v,uint32_t m) { const uint32_t p=v*m;return (p>>12)+((p&0xfff)>0x800); }
/* Independent stage rule: critical hits ignore bad stages for the side using the stat. */
static int use_raw(uint32_t key,int8_t s,uint32_t crit,uint32_t ignore)
{
    if(ignore)return 1;
    if(!crit)return 0;
    return (key==8||key==10)?s<6:s>6;
}
int main(void)
{
    unsigned cases=0;
    /* Exact attack sequence. */
    defaults();CHECK(px_battle_attack_value(CTX,ATTACKER,DEFENDER,MOVE,0,FLAG,&api)==base_of(ATTACKER,8)*2);
    CHECK(!strcmp(sc.log,"C (i3=1 i4=4 s3c=1f s53=0 E3e g3c g53 st i12=37 i16=3 i1b=1 s34=d8 m36=1000/19a/20000 v54=0 E40 g34 g36 g54 )"));
    CHECK(r8(NULL,FLAG)==0&&sc.max_depth==1);++cases;
    /* Exact defense sequence. */
    defaults();CHECK(px_battle_defense_value(CTX,ATTACKER,DEFENDER,MOVE,0,&api)==base_of(DEFENDER,9)*2);
    CHECK(!strcmp(sc.log,"C (i3=1 i4=4 i3d=9 s3e=0 v53=0 E3f g3e g53 )st (v42=0 E83 g42 )(i3=1 i4=4 i12=37 i16=3 i1b=1 s35=1a2 m36=1000/19a/20000 E41 g35 g36 )"));++cases;
    /* Stage, critical and ignore rules for every stat, stage and side. */
    for(uint32_t cat=1;cat<=2;++cat)for(uint32_t crit=0;crit<2;++crit)for(uint32_t ignore=0;ignore<3;ignore+=1)for(int s=0;s<=12;++s) {
        const uint32_t ignore_value=ignore==2?0x100:ignore;  /* defense tests only the low byte */
        defaults();sc.category=cat;sc.ignore=ignore_value;
        const uint32_t akey=cat==2?10:8,dkey=cat==2?11:9;
        w8(NULL,ATTACKER+0x104+(akey-8),(uint8_t)s);w8(NULL,DEFENDER+0x104+(dkey-8),(uint8_t)s);
        const uint32_t a=px_battle_attack_value(CTX,ATTACKER,DEFENDER,MOVE,crit,FLAG,&api);
        CHECK(a==(use_raw(akey,(int8_t)s,crit,ignore_value!=0)?base_of(ATTACKER,akey):base_of(ATTACKER,akey)*2));
        defaults();sc.category=cat;sc.ignore=ignore_value;
        w8(NULL,ATTACKER+0x104+(akey-8),(uint8_t)s);w8(NULL,DEFENDER+0x104+(dkey-8),(uint8_t)s);
        const uint32_t d=px_battle_defense_value(CTX,ATTACKER,DEFENDER,MOVE,crit,&api);
        CHECK(d==(use_raw(dkey,(int8_t)s,crit,(ignore_value&0xff)!=0)?base_of(DEFENDER,dkey):base_of(DEFENDER,dkey)*2));
        cases+=2;
    }
    /* Negative signed stages and an unmatched stage key. */
    defaults();w8(NULL,ATTACKER+0x104,0xfe);CHECK(px_battle_attack_value(CTX,ATTACKER,DEFENDER,MOVE,1,FLAG,&api)==base_of(ATTACKER,8));++cases;
    /* Battler substitution from event 0x3e, with bound and empty-slot aborts that return. */
    defaults();sc.source_index=3;w32(NULL,TABLE+0x84+12,OTHER);
    CHECK(px_battle_attack_value(CTX,ATTACKER,DEFENDER,MOVE,0,FLAG,&api)==base_of(OTHER,8)*2&&sc.aborts==0);++cases;
    defaults();w32(NULL,TABLE+0x84+12,OTHER);sc.source_index=0x103;
    CHECK(px_battle_attack_value(CTX,ATTACKER,DEFENDER,MOVE,0,FLAG,&api)==base_of(OTHER,8)*2&&sc.aborts==0);++cases; /* index is the low byte */
    defaults();sc.source_index=5;w32(NULL,TABLE+0x84+20,0);
    sc.aborts=0;(void)px_battle_attack_value(CTX,ATTACKER,DEFENDER,MOVE,0,FLAG,&api);CHECK(sc.aborts==1&&strstr(sc.log,"ABORT"));++cases;
    defaults();sc.source_index=0x18;w32(NULL,TABLE+0x84+0x60,OTHER);
    CHECK(px_battle_attack_value(CTX,ATTACKER,DEFENDER,MOVE,0,FLAG,&api)==base_of(OTHER,8)*2&&sc.aborts==1);++cases;
    /* Final multiplier, 16-bit narrowing and the burn-exception flag. */
    for(uint32_t m=0x19a;m<=0x20000;m=m*3+7) {
        defaults();sc.multiplier=m;sc.flag54=m&1?0x100:0;
        CHECK(px_battle_attack_value(CTX,ATTACKER,DEFENDER,MOVE,0,FLAG,&api)==(mod(base_of(ATTACKER,8)*2,m)&0xffff));
        CHECK(r8(NULL,FLAG)==(m&1));++cases;
        defaults();sc.multiplier=m;CHECK(px_battle_defense_value(CTX,ATTACKER,DEFENDER,MOVE,0,&api)==(mod(base_of(DEFENDER,9)*2,m)&0xffff));++cases;
    }
    defaults();sc.scale34=0x12345;CHECK(px_battle_attack_value(CTX,ATTACKER,DEFENDER,MOVE,0,FLAG,&api)==0x2345);++cases;
    /* Defense swap and the category value passed to event 0x41. */
    for(uint32_t cat=1;cat<=2;++cat)for(uint32_t swap=0;swap<4;++swap) {
        defaults();sc.category=cat;sc.swap=swap;
        const uint32_t key=(swap&1)?(cat==2?9:11):(cat==2?11:9);
        CHECK(px_battle_defense_value(CTX,ATTACKER,DEFENDER,MOVE,0,&api)==base_of(DEFENDER,key)*2);
        char want[16];snprintf(want,sizeof want,"i1b=%x s35",(swap&1)?(key==11?2:1):1);CHECK(strstr(sc.log,want));++cases;
    }
    /* Sandstorm: weather 4, rock type, special defense, not suppressed. */
    for(uint32_t weather=3;weather<=5;++weather)for(uint32_t rock=0;rock<2;++rock)for(uint32_t cat=1;cat<=2;++cat)for(uint32_t sup=0;sup<2;++sup) {
        defaults();w8(NULL,FIELD+1,(uint8_t)weather);sc.rock=rock;sc.category=cat;sc.suppress=sup;
        const uint32_t base=base_of(DEFENDER,cat==2?11:9)*2;
        const uint32_t want=(!sup&&weather==4&&rock&&cat==2)?mod(base,0x1800):base;
        CHECK(px_battle_defense_value(CTX,ATTACKER,DEFENDER,MOVE,0,&api)==want);
        CHECK((strstr(sc.log,"ht ")!=NULL)==(!sup&&weather==4));++cases;
    }
    printf("battle_stats: %u sequence, stage, critical, substitution, multiplier, swap and sandstorm cases\n",cases);
    puts("battle_stats: all tests passed");return 0;
}
