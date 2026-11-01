#include "battle_ability.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000, HANDLER=0x6000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
/* Battlers: id i lives at MON + 0x100 i with hp, max hp, ability and status. */
struct mon { uint32_t hp,max,ability,status; };
static struct mon mons[8];static uint32_t categories[0x400],side_value,side_ids[8],side_count,field;static unsigned aborts;
static char log_[1024];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon_index(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static uint32_t value(void *u,uint32_t p,uint32_t k)
{
    (void)u;const struct mon *m=&mons[mon_index(p)];char b[16];snprintf(b,sizeof b,"v%x ",k);note(b);
    switch(k){case 0xd:return m->hp;case 0xe:return m->max;case 0x11:return m->ability;default:CHECK(0);return 0;}
}
static uint32_t category(void *u,uint32_t move) { (void)u;CHECK(move<0x400);note("C ");return categories[move]; }
static uint32_t status(void *u,uint32_t p) { (void)u;note("S ");return mons[mon_index(p)].status; }
static uint32_t side_of(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);(void)id;note("side ");return side_value; }
static uint32_t side_battlers(void *u,uint32_t c,uint32_t sel,uint32_t out)
{
    (void)u;CHECK(c==CTX&&sel==(side_value|0x700)&&out==WORK);note("list ");
    for(uint32_t i=0;i<side_count;++i) { w8(NULL,out+i,(uint8_t)side_ids[i]); }
    return side_count;
}
static uint32_t field_check(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("F ");return field; }
static const struct px_battle_ability_api api={&events,value,category,status,side_of,side_battlers,field_check};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static void (*const table[])(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_ability_api*)={
        px_battle_thick_fat,px_battle_huge_power,px_battle_blaze,px_battle_guts};
    CHECK(f<4);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(mons,0,sizeof mons);memset(categories,0,sizeof categories);aborts=0;log_[0]=0;
    side_value=0;side_count=0;field=0;w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i){w32(NULL,TABLE+0x84+4*i,MON+0x100*i);mons[i].max=300;mons[i].hp=300;}
}
/* The attack frame as the attack routine builds it before event 0x40. */
static void frame(uint32_t attacker,uint32_t defender,uint32_t move,uint32_t type,uint32_t cat_byte,uint32_t value34)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x03,attacker,&events);px_battle_event_set_id(0x04,defender,&events);
    px_battle_event_set_id(0x12,move,&events);px_battle_event_set_id(0x16,type,&events);px_battle_event_set_id(0x1b,cat_byte,&events);
    px_battle_event_set_scaled(0x34,value34,&events);px_battle_event_set_multiplier(0x36,0x1000,0x19a,0x20000,&events);
    px_battle_event_set_value(0x54,0,&events);
}
static unsigned count_side_calls(void) { unsigned n=0;for(const char *q=strstr(log_,"side ");q;q=strstr(q+1,"side "))++n;return n; }
static uint32_t mult(void) { return px_battle_event_get(0x36,&events); }
static uint32_t ref(uint32_t base,uint32_t f) { const uint64_t p=(uint64_t)base*f+0x800;return (uint32_t)(p>>12); }
int main(void)
{
    unsigned cases=0;
    /* Multiply: rounding, signs, clamps, kinds and missing keys. */
    {
        const int32_t values[]={0,1,0x1000,0x1555,-0x1000,-1,0x7fffffff,(int32_t)0x80000000};
        const uint32_t factors[]={0,0x800,0x1000,0x1800,0x2000,0xffffffff,0x80000000};
        for(unsigned v=0;v<8;++v)for(unsigned f=0;f<7;++f)for(unsigned clamp=0;clamp<4;++clamp) {
            reset();px_battle_event_push(&events);
            const uint32_t lo=clamp&1?0x19a:0,hi=clamp&2?0x20000:0;
            px_battle_event_set_multiplier(0x36,(uint32_t)values[v],lo,hi,&events);
            px_battle_event_multiply(0x36,factors[f],&events);
            int64_t p=(int64_t)values[v]*(int32_t)factors[f]+0x800;uint32_t want=(uint32_t)((uint64_t)p>>12);
            if(clamp){ if((int32_t)lo>(int32_t)want)want=lo; if((int32_t)hi<(int32_t)want)want=hi; }
            CHECK(mult()==want&&aborts==0);++cases;
        }
        for(uint32_t kind=0;kind<5;++kind) {
            reset();px_battle_event_push(&events);
            if(kind==3)px_battle_event_set_multiplier(9,0x1000,0,0,&events);
            else if(kind==0)px_battle_event_set_scaled(9,0x1000,&events);
            else if(kind==1)px_battle_event_set_value(9,0x1000,&events);
            else px_battle_event_set_id(9,0x1000,&events);
            if(kind==2){w8(NULL,STACK+0x545,2);}
            px_battle_event_multiply(9,0x2000,&events);
            CHECK(r32(NULL,STACK+0xc8)==(kind==3?0x2000u:0x1000u)&&aborts==(kind==3?0u:1u));++cases;
        }
        reset();px_battle_event_push(&events);px_battle_event_multiply(0x36,0x2000,&events);CHECK(aborts==1);++cases;
        reset();px_battle_event_push(&events);px_battle_event_set_multiplier(5,0x1000,0,0,&events);
        px_battle_event_multiply(0,0x2000,&events);CHECK(aborts==2&&r32(NULL,STACK+0xc8)==0x1000);++cases;
        /* Only one clamp bound set still applies both comparisons. */
        reset();px_battle_event_push(&events);px_battle_event_set_multiplier(5,0x1000,0,0x1800,&events);
        px_battle_event_multiply(5,0x2000,&events);CHECK(r32(NULL,STACK+0xc8)==0x1800);++cases;
        reset();px_battle_event_push(&events);px_battle_event_set_multiplier(5,0x1000,0x1800,0,&events);
        px_battle_event_multiply(5,0x1000,&events);CHECK(r32(NULL,STACK+0xc8)==0);++cases;  /* min then max 0 */
    }
    /* Rewrite: kinds 0 and 1 (once), 2 silently refused, 3 and 4 abort. */
    for(uint32_t kind=0;kind<5;++kind)for(unsigned twice=0;twice<2;++twice) {
        reset();px_battle_event_push(&events);px_battle_event_set_value(9,7,&events);w8(NULL,STACK+0x545,(uint8_t)kind);
        uint32_t r=px_battle_event_rewrite(9,8,&events);
        CHECK(r==(kind<=1)&&r32(NULL,STACK+0xc8)==(kind<=1?8u:7u)&&aborts==(kind>=3));
        CHECK(r8(NULL,STACK+0x545)==(kind==1?2u:kind));
        if(twice){r=px_battle_event_rewrite(9,9,&events);CHECK(r==(kind==0)&&r32(NULL,STACK+0xc8)==(kind==0?9u:kind==1?8u:7u));}
        ++cases;
    }
    reset();px_battle_event_push(&events);CHECK(px_battle_event_rewrite(9,1,&events)==0&&aborts==1);++cases;
    /* Battler lookup and HP fractions. */
    reset();CHECK(px_battle_pokemon_by_id(CTX,5,&api)==MON+0x500&&aborts==0);++cases;
    reset();w32(NULL,TABLE+0x84+4*0x18,0x77);CHECK(px_battle_pokemon_by_id(CTX,0x18,&api)==0x77&&aborts==1);++cases;
    reset();w32(NULL,TABLE+0x84+8,0);CHECK(px_battle_pokemon_by_id(CTX,2,&api)==0&&aborts==1);++cases;
    for(uint32_t max=0;max<400;++max) { reset();mons[1].max=max;CHECK(px_battle_hp_fraction(MON+0x100,3,&api)==max/3);++cases; }
    for(uint32_t v=0;v<0x3000;v+=7)for(uint32_t f=0x800;f<=0x2000;f+=0x400) { CHECK(px_battle_modify(v,f)==((v*f)>>12)+(((v*f)&0xfff)>0x800));++cases; }
    CHECK(px_battle_modify(0x100000,0x1800)==((0x100000u*0x1800u)>>12));++cases;
    /* Thick Fat: the defender, Fire or Ice (low byte of the type). */
    for(uint32_t self=0;self<2;++self)for(uint32_t type=0;type<0x210;type+=1) {
        if(type>0x20&&type<0x200)continue;
        reset();frame(1,self?2:3,10,type,1,100);px_battle_thick_fat(HANDLER,CTX,2,WORK,&api);
        const int on=self&&((type&0xff)==9||(type&0xff)==14);CHECK(mult()==(on?0x800u:0x1000u)&&aborts==0);++cases;
    }
    /* Huge Power: the attacker, physical moves. */
    for(uint32_t self=0;self<2;++self)for(uint32_t cat=0;cat<4;++cat) {
        reset();categories[0x123]=cat;frame(self?2:3,1,0x10123,9,1,100);px_battle_huge_power(HANDLER,CTX,2,WORK,&api);
        CHECK(mult()==(self&&cat==1?0x2000u:0x1000u));++cases;   /* move id narrowed to 16 bits */
    }
    /* Pinch abilities: every HP around a third, the matching type only (full word). */
    {
        void (*const fns[4])(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_ability_api*)={px_battle_blaze,px_battle_torrent,px_battle_overgrow,px_battle_swarm};
        const uint32_t types[4]={9,10,11,6};
        for(unsigned k=0;k<4;++k)for(uint32_t max=1;max<40;++max)for(uint32_t hp=0;hp<=max;++hp)for(uint32_t t=0;t<3;++t) {
            const uint32_t type=t==0?types[k]:t==1?types[k]+0x100:types[(k+1)%4];
            reset();mons[2].max=max;mons[2].hp=hp;frame(2,1,5,type,1,100);fns[k](HANDLER,CTX,2,WORK,&api);
            CHECK(mult()==(hp<=max/3&&t==0?0x1800u:0x1000u));++cases;
        }
        reset();frame(3,1,5,9,1,100);px_battle_blaze(HANDLER,CTX,2,WORK,&api);CHECK(mult()==0x1000&&!strchr(log_,'v'));++cases;
        reset();mons[2].hp=1;frame(2,1,5,9,1,100);log_[0]=0;px_battle_blaze(HANDLER,CTX,2,WORK,&api);CHECK(!strcmp(log_,"ve vd "));++cases;
    }
    /* Guts: status, physical, the burn exception; it rewrites key 0x54 once. */
    for(uint32_t st=0;st<3;++st)for(uint32_t cat=0;cat<3;++cat) {
        reset();mons[2].status=st;categories[5]=cat;frame(2,1,5,9,1,100);px_battle_guts(HANDLER,CTX,2,WORK,&api);
        const int on=st&&cat==1;CHECK(mult()==(on?0x1800u:0x1000u)&&px_battle_event_get(0x54,&events)==(on?1u:0u)&&aborts==0);++cases;
    }
    /* Marvel Scale, Fur Coat and Grass Pelt: the defender and category byte 1. */
    for(uint32_t self=0;self<2;++self)for(uint32_t st=0;st<2;++st)for(uint32_t cat=0;cat<3;++cat)for(uint32_t f=0;f<3;++f) {
        reset();mons[2].status=st;field=f;frame(1,self?2:3,5,9,cat,100);px_battle_marvel_scale(HANDLER,CTX,2,WORK,&api);
        CHECK(mult()==(self&&st&&cat==1?0x1800u:0x1000u));
        reset();field=f;frame(1,self?2:3,5,9,cat,100);px_battle_fur_coat(HANDLER,CTX,2,WORK,&api);
        CHECK(mult()==(self&&cat==1?0x2000u:0x1000u));
        reset();field=f;frame(1,self?2:3,5,9,cat,100);px_battle_grass_pelt(HANDLER,CTX,2,WORK,&api);
        CHECK(mult()==(self&&cat==1&&f==1?0x1800u:0x1000u));++cases;
    }
    /* Hustle: the attack value itself, rounded, rewritten. */
    for(uint32_t v=0;v<2000;v+=13)for(uint32_t cat=0;cat<3;++cat) {
        reset();categories[5]=cat;frame(2,1,5,9,1,v);px_battle_hustle(HANDLER,CTX,2,WORK,&api);
        CHECK(px_battle_event_get(0x34,&events)==(cat==1?px_battle_modify(v,0x1800):v)&&mult()==0x1000);++cases;
    }
    /* Defeatist: at or below half HP, any move. */
    for(uint32_t max=1;max<30;++max)for(uint32_t hp=0;hp<=max;++hp) {
        reset();mons[2].max=max;mons[2].hp=hp;frame(2,1,5,9,1,100);px_battle_defeatist(HANDLER,CTX,2,WORK,&api);
        CHECK(mult()==(hp<=max/2?0x800u:0x1000u));++cases;
    }
    /* Plus/Minus: an ally (not itself) with Plus or Minus, special moves; no side ends a pass. */
    for(uint32_t side=0;side<2;++side)for(uint32_t ally=0;ally<4;++ally)for(uint32_t self_ability=0;self_ability<2;++self_ability)for(uint32_t cat=0;cat<3;++cat) {
        reset();side_value=side?0xa:1;side_count=3;side_ids[0]=2;side_ids[1]=4;side_ids[2]=5;
        mons[2].ability=self_ability?0x39:0;mons[4].ability=ally==1?0x39:ally==2?0x3a:ally==3?0x1039:0;categories[5]=cat;
        frame(2,1,5,9,1,100);px_battle_plus_minus(HANDLER,CTX,2,WORK,&api);
        const int on=!side&&(ally==1||ally==2)&&cat==2;CHECK(mult()==(on?0x1800u:0x1000u));
        CHECK(count_side_calls()==(side?2u:(ally==1?1u:2u)));++cases;
    }
    reset();side_value=1;side_count=0;categories[5]=2;frame(2,1,5,9,1,100);px_battle_plus_minus(HANDLER,CTX,2,WORK,&api);CHECK(mult()==0x1000);++cases;
    /* Through the dispatcher: three handlers registered for event 0x40 run in list order. */
    {
        reset();mons[2].status=1;mons[2].hp=50;categories[5]=1;
        static const uint8_t e40[1]={0x40};
        for(unsigned i=0;i<3;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7000+0x10*i;
            w8(NULL,entries,e40[0]);w32(NULL,entries+4,i==0?1:i==1?2:3);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,2);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<2?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);
        frame(2,1,5,9,1,100);px_battle_event_dispatch(CTX,0x40,&events);
        /* Huge Power x2, Blaze x1.5 (Fire at 50/300 HP), Guts x1.5 with the burn exception. */
        CHECK(mult()==ref(ref(ref(0x1000,0x2000),0x1800),0x1800)&&px_battle_event_get(0x54,&events)==1&&aborts==0);++cases;
    }
    printf("battle_ability: %u cases, all tests passed\n",cases);
    return 0;
}
