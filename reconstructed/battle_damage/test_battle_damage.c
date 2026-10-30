#include "battle_damage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, CTX=0x100, ATTACKER=0x200, DEFENDER=0x300, MOVE=0x400, RULES=0x500, FIELD=0x600,
       OWNER=0x700, OUTPUT=0x800, FLAG=0x900 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
/* Scenario: values the scripted services return, and a log of every call. */
struct scenario {
    uint32_t fixed,power,power_mod,attack,defense,level,suppress_weather,rule,random,same_type,stab,
             effectiveness_mult,status,flag,final_mod,category,types;
    char log[2048];unsigned depth,max_depth;uint32_t last_get,scaled33;
};
static struct scenario sc;
static void note(const char *s) { strncat(sc.log,s,sizeof sc.log-strlen(sc.log)-1); }
static void push(void *u) { (void)u;note("(");++sc.depth;if(sc.depth>sc.max_depth)sc.max_depth=sc.depth; }
static void pop(void *u) { (void)u;note(")");CHECK(sc.depth>0);--sc.depth; }
static void set_id(void *u,uint32_t k,uint32_t v) { (void)u;if(k==0x1b&&sc.depth==1)CHECK(v==sc.category);char b[16];snprintf(b,sizeof b,"i%x ",k);note(b); }
static void set_value(void *u,uint32_t k,uint32_t v) { (void)u;(void)v;char b[16];snprintf(b,sizeof b,"v%x ",k);note(b); }
static void set_scaled(void *u,uint32_t k,uint32_t v) { (void)u;char b[16];snprintf(b,sizeof b,"s%x ",k);note(b);if(k==0x33)sc.scaled33=v;if(k==0x36)CHECK(v==(sc.same_type?0x1800u:0x1000u)); }
static void set_multiplier(void *u,uint32_t k,uint32_t v,uint32_t lo,uint32_t hi) { (void)u;CHECK(v==0x1000&&lo==0x29&&(hi==0x200000||hi==0x20000));char b[16];snprintf(b,sizeof b,"m%x ",k);note(b); }
static void dispatch(void *u,uint32_t c,uint32_t e) { (void)u;CHECK(c==CTX);char b[16];snprintf(b,sizeof b,"E%x ",e);note(b); }
static uint32_t get(void *u,uint32_t k)
{
    (void)u;char b[16];snprintf(b,sizeof b,"g%x ",k);note(b);
    switch(k) {
    case 0x38: return sc.fixed;
    case 0x31: return sc.power;
    case 0x32: return sc.power_mod;
    case 0x42: return sc.suppress_weather;
    case 0x53: return sc.same_type;
    case 0x36: return sc.depth==2?sc.stab:sc.final_mod;
    case 0x33: return sc.scaled33;
    default: CHECK(0);return 0;
    }
}
static uint32_t attack(void *u,uint32_t c,uint32_t a,uint32_t d,uint32_t m,uint32_t crit,uint32_t f)
{ (void)u;(void)crit;CHECK(c==CTX&&a==ATTACKER&&d==DEFENDER&&m==MOVE&&f==FLAG);note("A ");w8(NULL,FLAG,(uint8_t)sc.flag);return sc.attack; }
static uint32_t defense(void *u,uint32_t c,uint32_t a,uint32_t d,uint32_t m,uint32_t crit) { (void)u;(void)c;(void)a;(void)d;(void)m;(void)crit;note("D ");return sc.defense; }
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;CHECK(p==ATTACKER&&k==0xf);note("L ");return sc.level; }
static uint32_t attacker_types(void *u,uint32_t p) { (void)u;CHECK(p==ATTACKER);note("T ");return sc.types; }
static uint32_t status(void *u,uint32_t p) { (void)u;CHECK(p==ATTACKER);note("S ");return sc.status; }
static uint32_t effectiveness(void *u,uint32_t d,uint32_t e) { (void)u;(void)e;note("X ");return d*sc.effectiveness_mult/4; }
static uint32_t category(void *u,uint32_t m) { (void)u;CHECK(m==33);note("C ");return sc.category; }
static uint32_t power(void *u,uint32_t m) { (void)u;CHECK(m==33);note("P ");return 40; }
static uint32_t rule(void *u,uint32_t w,uint32_t f) { (void)u;CHECK(w==0xabcd&&f==7);note("R ");return sc.rule; }
static uint32_t random_value(void *u,uint32_t g,uint32_t r) { (void)u;CHECK(g==0x1234&&r==0x10);note("N ");return sc.random; }
static const struct px_battle_damage_api api={&memory,NULL,push,pop,set_id,set_value,set_scaled,set_multiplier,dispatch,get,
    attack,defense,pokemon_value,attacker_types,status,effectiveness,category,power,rule,random_value,OWNER,FLAG};
static void defaults(void)
{
    memset(&sc,0,sizeof sc);memset(mem,0,sizeof mem);
    sc.power=40;sc.power_mod=0x1000;sc.attack=50;sc.defense=40;sc.level=20;sc.random=0;sc.stab=0x1000;
    sc.effectiveness_mult=4;sc.final_mod=0x1000;sc.category=1;sc.types=0x0909;
    w8(NULL,ATTACKER+0x19,1);w8(NULL,DEFENDER+0x19,4);w16(NULL,MOVE,33);w8(NULL,MOVE+6,0);w8(NULL,MOVE+7,2);
    w32(NULL,CTX+4,RULES);w32(NULL,RULES+0x10,0xabcd);w32(NULL,CTX+0x1c,FIELD);w32(NULL,OWNER+0x54,0x1234);
}
static uint32_t run(uint32_t spread,uint32_t crit,uint32_t fixed_roll,uint32_t *returned)
{
    const struct px_battle_damage_args a={CTX,ATTACKER,DEFENDER,MOVE,7,spread,crit,fixed_roll,0,OUTPUT};
    *returned=px_battle_calculate_damage(&a,&api);CHECK(sc.depth==0);
    return r16(NULL,OUTPUT);
}
/* Independent model of the formula with the same scripted inputs. */
static uint32_t mod(uint32_t v,uint32_t m) { uint64_t p=(uint64_t)(uint32_t)(v*m);return (uint32_t)((p>>12)+((p&0xfff)>0x800)); }
static uint32_t model(uint32_t spread,uint32_t crit,uint32_t fixed_roll,uint32_t weather,uint32_t type)
{
    uint32_t power=mod(sc.power&0xffff,sc.power_mod)&0xffff;
    uint32_t d=(((sc.level&0xff)*2/5+2)*(power*sc.attack))/sc.defense/50+2;
    if(spread!=0x1000)d=mod(d,spread);
    if(!sc.suppress_weather&&((weather==1&&type==9)||(weather==2&&type==10)))d=mod(d,0x1800);
    if(!sc.suppress_weather&&((weather==1&&type==10)||(weather==2&&type==9)))d=mod(d,0x800);
    if(crit)d=d*15/10;
    if(!sc.rule)d=d*(fixed_roll?85:(100-sc.random)&0xffff)/100;
    if(type!=0x12)d=mod(d,sc.stab);
    d=d*sc.effectiveness_mult/4;
    if(sc.category==1&&sc.status==4&&sc.flag==0)d=d*50/100;
    d=mod(d,sc.final_mod);
    return d?d:1;
}
int main(void)
{
    unsigned cases=0;uint32_t ret;
    /* Exact call order for an ordinary hit. */
    defaults();CHECK(run(0x1000,0,0,&ret)==model(0x1000,0,0,0,0)&&ret==0);
    CHECK(!strcmp(sc.log,"C (i39 i3 i4 i46 i16 i12 i1b i47 s38 E4e g38 P (i3 i4 i12 i16 i1b v57 v58 v31 E3c m32 E3d g31 g32 )A D L "
                         "(v42 E83 g42 )R N T (i2 v53 E48 g53 i45 s36 E49 g36 )X S m36 s33 E4f g36 g33 E50 )"));
    CHECK(sc.max_depth==2);++cases;
    /* Fixed damage from event 0x4e skips everything else and returns 1. */
    defaults();sc.fixed=77;CHECK(run(0x1000,0,0,&ret)==77&&ret==1);
    CHECK(!strcmp(sc.log,"C (i39 i3 i4 i46 i16 i12 i1b i47 s38 E4e g38 E50 )"));++cases;
    defaults();sc.fixed=0x12345;CHECK(run(0x1000,0,0,&ret)==0x2345&&ret==1);++cases;
    /* Weather by type, suppression, critical, spread, roll, STAB, burn and final modifiers. */
    for(uint32_t weather=0;weather<4;++weather)
        for(uint32_t type=8;type<=11;++type)
            for(uint32_t suppress=0;suppress<2;++suppress)
                for(uint32_t crit=0;crit<2;++crit)
                    for(uint32_t spread=0xc00;spread<=0x1000;spread+=0x400)
                        for(uint32_t roll=0;roll<16;roll+=5) {
                            defaults();w8(NULL,FIELD+1,(uint8_t)weather);w8(NULL,MOVE+6,(uint8_t)type);
                            sc.suppress_weather=suppress;sc.random=roll;sc.level=5+roll*6;sc.attack=30+type*7;sc.defense=20+weather*9;
                            CHECK(run(spread,crit,0,&ret)==model(spread,crit,0,weather,type));++cases;
                        }
    for(uint32_t rule=0;rule<2;++rule)for(uint32_t fixed_roll=0;fixed_roll<2;++fixed_roll) {
        defaults();sc.rule=rule;sc.random=9;CHECK(run(0x1000,0,fixed_roll,&ret)==model(0x1000,0,fixed_roll,0,0));
        CHECK((strstr(sc.log,"N ")!=NULL)==(!rule&&!fixed_roll));++cases;
    }
    for(uint32_t same=0;same<2;++same) { defaults();sc.same_type=same;sc.stab=same?0x1800:0x1000;CHECK(run(0x1000,0,0,&ret)==model(0x1000,0,0,0,0));++cases; }
    defaults();w8(NULL,MOVE+6,0x12);CHECK(run(0x1000,0,0,&ret)==model(0x1000,0,0,0,0x12)&&!strstr(sc.log,"T "));++cases;
    for(uint32_t category=0;category<3;++category)for(uint32_t status=3;status<=5;++status)for(uint32_t flag=0;flag<2;++flag) {
        defaults();sc.category=category;sc.status=status;sc.flag=flag;
        CHECK(run(0x1000,0,0,&ret)==model(0x1000,0,0,0,0));
        CHECK((strstr(sc.log,"S ")!=NULL)==(category==1));++cases;
    }
    defaults();sc.power_mod=0x1a00;sc.effectiveness_mult=8;sc.final_mod=0x1555;CHECK(run(0x1000,0,0,&ret)==model(0x1000,0,0,0,0));++cases;
    /* Power from event 0x31 is narrowed to 16 bits before its modifier. */
    defaults();sc.power=0x10028;CHECK(run(0x1000,0,0,&ret)==model(0x1000,0,0,0,0));++cases;
    /* A critical multiply on a huge value wraps in 32 bits before division. */
    defaults();sc.attack=0xfffff;sc.defense=1;sc.level=200;sc.power=250;
    { const uint32_t got=run(0x1000,1,0,&ret),want=model(0x1000,1,0,0,0)&0xffff;if(got!=want)fprintf(stderr,"huge crit: reconstruction %u model %u\n",got,want);CHECK(got==want); }++cases;
    /* Minimum one and 16-bit output. */
    defaults();sc.effectiveness_mult=0;CHECK(run(0x1000,0,0,&ret)==1&&ret==0);++cases;
    defaults();sc.attack=60000;sc.defense=1;sc.level=100;sc.power=250;sc.effectiveness_mult=16;
    const uint32_t big=model(0x1000,0,0,0,0);CHECK(run(0x1000,0,0,&ret)==(big&0xffff));++cases;
    /* Rounding rule of the 4096-based modifier. */
    CHECK(px_battle_apply_modifier(1,0x800)==0&&px_battle_apply_modifier(1,0x801)==1&&px_battle_apply_modifier(3,0xc00)==2);
    /* The product is 32-bit: large values wrap as on ARM. */
    CHECK(px_battle_apply_modifier(0xfffff,0x1000)==0xfffff&&px_battle_apply_modifier(0x100000,0x1000)==0&&px_battle_apply_modifier(0x80000,0x2000)==0);cases+=2;
    printf("battle_damage: %u call-order, fixed-damage, weather, critical, spread, roll, STAB, burn, modifier and bound cases\n",cases);
    puts("battle_damage: all tests passed");return 0;
}
