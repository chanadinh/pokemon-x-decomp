#include "battle_items_3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000,
       HANDLER=0x6000, ENTRIES=0x7a00, P=MON+0x200 };
enum { REGISTERED=0x20000000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static uint32_t param,category;static unsigned aborts;static char log_[512];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a) { char t[48];snprintf(t,sizeof t,f,a);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static uint32_t move_category(void *u,uint32_t move) { (void)u;notef("mc%x ",move);return category; }
static const struct px_battle_ability_api base={.events=&events,.move_category=move_category};
static uint32_t item_param(void *u,uint32_t item,uint32_t k) { (void)u;CHECK(k==3);notef("ip%x ",item);return param; }
static const struct px_battle_modifier_api modifier={.base=&base,.item_param=item_param};
enum { RECORD=0x7000 };
static unsigned records;
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id)
{ (void)u;CHECK(c==CTX&&records<2);char t[32];snprintf(t,sizeof t,"rec%x/%x ",k,id);note(t);return RECORD+0x100*records++; }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD);note("commit "); }
static uint32_t condition(void *u,uint32_t s) { (void)u;notef("cond%x ",s);return 0xc0de0000|s; }
static const struct px_battle_items_3_api api={&modifier,action_record,commit,condition};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_items_3_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_charcoal,px_battle_lustrous_orb,px_battle_muscle_band};
    CHECK(f<3);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);param=20;category=0;aborts=0;records=0;log_[0]=0;
    w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
    w16(NULL,HANDLER+0x38,0xf9);
}
/* A frame with attacker, move, move type and the power multiplier. */
static void frame(uint32_t attacker,uint32_t move,uint32_t type,uint32_t power)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x03,attacker,&events);
    px_battle_event_set_value(0x12,move,&events);px_battle_event_set_value(0x16,type,&events);
    px_battle_event_set_multiplier(0x32,power,0,0,&events);log_[0]=0;
}
static uint32_t power(void) { return px_battle_event_get(0x32,&events); }
/* The reference item factor, (parameter + 100)% in 4096 units, and the rounded multiply. */
static uint32_t factor(uint32_t p) { return (uint32_t)(((uint64_t)p*4096+409600)/100); }
static uint32_t times(uint32_t v,uint32_t f) { return (uint32_t)((uint64_t)((int64_t)(int32_t)v*(int32_t)f+0x800)>>12); }
static unsigned cases;
static void expect(const char *s) { if(strcmp(log_,s)){fprintf(stderr,"got  [%s]\nwant [%s]\n",log_,s);} CHECK(!strcmp(log_,s));++cases; }
int main(void)
{
    /* The type items: every type word 0..0x13, high bits, the attacker, four parameters, two powers. */
    {
        static const fn h[]={px_battle_silk_scarf,px_battle_black_belt,px_battle_sharp_beak,px_battle_poison_barb,px_battle_soft_sand,
            px_battle_hard_stone,px_battle_rock_incense,px_battle_silver_powder,px_battle_spell_tag,px_battle_metal_coat,px_battle_charcoal,
            px_battle_mystic_water,px_battle_sea_incense,px_battle_wave_incense,px_battle_miracle_seed,px_battle_rose_incense,px_battle_magnet,
            px_battle_twisted_spoon,px_battle_odd_incense,px_battle_never_melt_ice,px_battle_dragon_fang,px_battle_black_glasses,px_battle_pixie_plate};
        static const uint32_t types[]={0,1,2,3,4,5,5,6,7,8,9,0xa,0xa,0xa,0xb,0xb,0xc,0xd,0xd,0xe,0xf,0x10,0x11};
        const uint32_t params[]={0,10,20,50};const uint32_t powers[]={0x1000,0x1555};
        for(unsigned i=0;i<23;++i)for(uint32_t t=0;t<0x14+3;++t)for(uint32_t who=2;who<4;++who)for(unsigned k=0;k<4;++k)for(unsigned v=0;v<2;++v){
            const uint32_t type=t<0x14?t:t==0x14?0x100|types[i]:t==0x15?0x80000000u|types[i]:0xffffffffu;
            reset();param=params[k];frame(who,0x55,type,powers[v]);h[i](HANDLER,CTX,2,0,&api);
            const int hit=who==2&&type==types[i];
            expect(hit?"ipf9 ":"");
            CHECK(power()==(hit?times(powers[v],factor(params[k])):powers[v])&&aborts==0);
        }
        /* Expert-Belt-like check of the factor: 20 gives 0x1333. */
        reset();frame(2,1,9,0x1000);px_battle_charcoal(HANDLER,CTX,2,0,&api);CHECK(power()==0x1333);++cases;
        /* Another holder and item. */
        reset();w16(NULL,HANDLER+0x38,0x12f);frame(5,1,0x10,0x1000);px_battle_black_glasses(HANDLER,CTX,5,0,&api);expect("ip12f ");
    }
    /* The orbs: species, type low byte and the attacker. */
    {
        static const fn h[]={px_battle_adamant_orb,px_battle_lustrous_orb,px_battle_griseous_orb};
        const uint32_t species[]={0x1e3,0x1e4,0x1e7};const uint32_t second[]={8,0xa,7};
        const uint32_t tries[]={0,0x1e2,0x1e3,0x1e4,0x1e5,0x1e6,0x1e7,0x1e8,0x3e3,0xffff};
        for(unsigned i=0;i<3;++i)for(unsigned s=0;s<10;++s)for(uint32_t t=0;t<0x16;++t)for(uint32_t who=2;who<4;++who)for(unsigned k=0;k<2;++k){
            const uint32_t type=t<0x13?t:t==0x13?0x10f:t==0x14?0xff00|second[i]:0x110;
            reset();param=k?50:20;w16(NULL,P+0xc,(uint16_t)tries[s]);w16(NULL,MON+0x300+0xc,(uint16_t)species[i]);
            frame(who,0x55,type,0x1000);h[i](HANDLER,CTX,2,0,&api);
            const uint32_t low=type&0xff;
            const int hit=who==2&&tries[s]==species[i]&&(low==0xf||low==second[i]);
            expect(hit&&i<2?"ipf9 ":"");
            CHECK(power()==(hit?(i<2?factor(k?50:20):0x1333u):0x1000u)&&aborts==0);
        }
        /* The species is a halfword: the bytes around it do not matter. */
        reset();w16(NULL,P+0xc,0x1e4);w16(NULL,P+0xa,0xffff);w16(NULL,P+0xe,0xffff);frame(2,0,0xa,0x1000);
        px_battle_lustrous_orb(HANDLER,CTX,2,0,&api);expect("ipf9 ");CHECK(power()==0x1333);
        /* Another holder's species is read. */
        reset();w16(NULL,MON+0x600+0xc,0x1e7);frame(6,0,7,0x1000);px_battle_griseous_orb(HANDLER,CTX,6,0,&api);expect("");CHECK(power()==0x1333);
    }
    /* Muscle Band and Wise Glasses: the category of the move's low half. */
    {
        const uint32_t moves[]={0,1,0x55,0xffff,0x10055,0xffff0001};
        for(unsigned i=0;i<2;++i)for(uint32_t c=0;c<4;++c)for(unsigned mv=0;mv<6;++mv)for(uint32_t who=2;who<4;++who){
            reset();category=c;frame(who,moves[mv],0,0x1000);
            if(i)px_battle_wise_glasses(HANDLER,CTX,2,0,&api); else px_battle_muscle_band(HANDLER,CTX,2,0,&api);
            const int hit=who==2&&c==i+1u;
            char b[48]="";if(who==2)snprintf(b,sizeof b,"mc%x %s",moves[mv]&0xffff,hit?"ipf9 ":"");
            expect(b);CHECK(power()==(hit?0x1333u:0x1000u));
        }
    }
    /* Poison Barb on event 0x7c: key 0x02 the holder, key 0x03's low byte the record's battler. */
    {
        const uint32_t targets[]={0,1,3,5,0x103,0xffffff05};
        for(uint32_t holder=0;holder<6;++holder)for(unsigned t=0;t<6;++t){
            reset();px_battle_event_push(&events);px_battle_event_set_id(0x02,holder,&events);
            px_battle_event_set_value(0x03,targets[t],&events);log_[0]=0;
            px_battle_poison_barb_use(HANDLER,CTX,2,0,&api);
            if(holder!=2){ expect("");CHECK(records==0);continue; }
            char b[64];snprintf(b,sizeof b,"rec%s/%x cond5 commit ","b",targets[t]&0xff);expect(b);
            CHECK(r8(NULL,RECORD+0xf)==2&&r16(NULL,RECORD+4)==5&&r32(NULL,RECORD+8)==0xc0de0005);
            for(uint32_t k=0;k<0x100;++k)if(k!=4&&k!=5&&k!=0xf&&(k<8||k>0xb))CHECK(r8(NULL,RECORD+k)==0);
            CHECK(aborts==0);
        }
        reset();px_battle_event_push(&events);px_battle_event_set_id(0x02,4,&events);px_battle_event_set_value(0x03,1,&events);log_[0]=0;
        px_battle_poison_barb_use(HANDLER,CTX,4,0,&api);expect("recb/1 cond5 commit ");CHECK(r8(NULL,RECORD+0xf)==4);
    }
    /* Through the dispatcher on event 0x3d. */
    {
        /* Three linked handlers, one event-0x3d entry each: Charcoal, Lustrous Orb, Muscle Band. */
        reset();w16(NULL,P+0xc,0x1e4);category=1;
        for(uint32_t i=0;i<3;++i){
            const uint32_t h=HANDLER+0x100*i;
            w8(NULL,ENTRIES+8*i,0x3d);w32(NULL,ENTRIES+8*i+4,i);
            w32(NULL,h+8,ENTRIES+8*i);w32(NULL,h+0x18,REGISTERED|1u<<16);w8(NULL,h+0x3a,2);w16(NULL,h+0x38,0xf9);
            w32(NULL,h+0,i?h-0x100:0);w32(NULL,h+4,i<2?h+0x100:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);frame(2,7,0xa,0x1000);px_battle_event_dispatch(CTX,0x3d,&events);
        expect("ipf9 mc7 ipf9 ");CHECK(power()==times(times(0x1000,0x1333),0x1333)&&aborts==0);
        /* A Fire move: Charcoal instead of the orb. */
        px_battle_event_pop(&events);frame(2,7,9,0x1000);px_battle_event_dispatch(CTX,0x3d,&events);
        expect("ipf9 mc7 ipf9 ");CHECK(power()==times(times(0x1000,0x1333),0x1333)&&aborts==0);
        /* Another attacker: nothing. */
        px_battle_event_pop(&events);frame(3,7,9,0x1000);px_battle_event_dispatch(CTX,0x3d,&events);
        expect("");CHECK(power()==0x1000&&aborts==0);
    }
    printf("battle_items_3: %u cases, all tests passed\n",cases);
    return 0;
}
