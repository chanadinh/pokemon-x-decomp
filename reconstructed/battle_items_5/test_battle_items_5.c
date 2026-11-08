#include "battle_items_5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000,
       HANDLER=0x6000, RECORD=0x7000, ENTRIES=0x7a00, P=MON+0x200 };
enum { REGISTERED=0x20000000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
/* Answers: move category from data, transformed, can evolve. */
static uint32_t category,transformed_v,evolves;static unsigned aborts,records;static char log_[512];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a) { char t[48];snprintf(t,sizeof t,f,a);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static uint32_t move_category(void *u,uint32_t move) { (void)u;notef("mc%x ",move);return category; }
static const struct px_battle_ability_api base={.events=&events,.move_category=move_category};
static uint32_t transformed(void *u,uint32_t p) { (void)u;CHECK(p==P);note("tr ");return transformed_v; }
static uint32_t can_evolve(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("ce%u ",id);return evolves; }
static uint32_t lock_condition(void *u,uint32_t move) { (void)u;notef("lc%x ",move);return 0xc0de0000|move; }
static uint32_t condition(void *u,uint32_t s) { (void)u;notef("cond%x ",s);return 0xc0de0000|s; }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id)
{ (void)u;CHECK(c==CTX&&records<1);char t[32];snprintf(t,sizeof t,"rec%x/%x ",k,id);note(t);++records;return RECORD; }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD);note("commit "); }
static const struct px_battle_items_5_api api={&base,transformed,can_evolve,lock_condition,condition,action_record,commit};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_items_5_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_choice_band,px_battle_choice_lock,px_battle_choice_release};
    CHECK(f<3);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);category=0;transformed_v=0;evolves=0;aborts=0;records=0;log_[0]=0;
    w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
/* A frame: key 0x02, 0x03 and 0x04 battlers, the move, the move's category key 0x1b, the
 * multiplier 0x36 and the critical-hit stage 0x2d. */
static void frame(uint32_t k2,uint32_t k3,uint32_t k4,uint32_t move,uint32_t kind,uint32_t stage)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x02,k2,&events);px_battle_event_set_id(0x03,k3,&events);
    px_battle_event_set_id(0x04,k4,&events);px_battle_event_set_value(0x12,move,&events);px_battle_event_set_value(0x1b,kind,&events);
    px_battle_event_set_value(0x2d,stage,&events);px_battle_event_set_multiplier(0x36,0x1000,0,0,&events);log_[0]=0;
}
static uint32_t factor_now(void) { return px_battle_event_get(0x36,&events); }
static unsigned cases;
static void expect(const char *s) { if(strcmp(log_,s)){fprintf(stderr,"got  [%s]\nwant [%s]\n",log_,s);} CHECK(!strcmp(log_,s));++cases; }
/* One multiplier handler: holder key, species (0 none, else up to two), transformed test,
 * category source (0 none, 1 move data, 2 key 0x1b) and value, factor. */
struct rule { fn h; uint32_t key, sp1, sp2; int transform; int source; uint32_t want; uint32_t factor; };
static const struct rule rules[]={
    {px_battle_macho_brace,2,0,0,0,0,0,0x800},{px_battle_iron_ball_speed,2,0,0,0,0,0,0x800},
    {px_battle_choice_scarf,2,0,0,0,0,0,0x1800},{px_battle_power_item,2,0,0,0,0,0,0x800},
    {px_battle_quick_powder,2,0x84,0,1,0,0,0x2000},
    {px_battle_choice_band,3,0,0,0,1,1,0x1800},{px_battle_choice_specs,3,0,0,0,1,2,0x1800},
    {px_battle_soul_dew_attack,3,0x17c,0x17d,0,2,2,0x1800},{px_battle_deep_sea_tooth,3,0x16e,0,0,1,2,0x2000},
    {px_battle_light_ball,3,0x19,0,0,0,0,0x2000},{px_battle_thick_club,3,0x68,0x69,0,1,1,0x2000},
    {px_battle_soul_dew_defense,4,0x17c,0x17d,0,2,2,0x1800},{px_battle_deep_sea_scale,4,0x16e,0,0,2,2,0x2000},
    {px_battle_metal_powder,4,0x84,0,1,2,1,0x2000},{px_battle_assault_vest,4,0,0,0,2,2,0x1800}};
int main(void)
{
    /* The multiplier handlers: holder keys, eight species, transformed, both category sources, move words. */
    {
        const uint32_t species_list[]={0,0x19,0x68,0x69,0x84,0x16e,0x17c,0x17d,0x284,0xffff};
        const uint32_t moves[]={0x21,0x10021};
        for(unsigned r=0;r<15;++r)for(unsigned s=0;s<10;++s)for(uint32_t tf=0;tf<2;++tf)for(uint32_t mc=0;mc<4;++mc)
        for(uint32_t kind=0;kind<4;++kind)for(unsigned holder=2;holder<5;++holder)for(unsigned mv=0;mv<2;++mv){
            const struct rule *x=&rules[r];
            reset();w16(NULL,P+0xc,(uint16_t)species_list[s]);w16(NULL,P+0xa,0xffff);w16(NULL,P+0xe,0xffff);
            category=mc;transformed_v=tf;
            frame(holder==2?2:7,holder==3?2:7,holder==4?2:7,moves[mv],kind,0);
            x->h(HANDLER,CTX,2,0,&api);
            char b[64]="";int ok=holder==x->key;
            if(ok&&x->sp1)ok=species_list[s]==x->sp1||(x->sp2&&species_list[s]==x->sp2);
            if(ok&&x->transform){ strcat(b,"tr ");ok=!tf; }
            if(ok&&x->source==1){ char c[16];snprintf(c,sizeof c,"mc%x ",moves[mv]&0xffff);strcat(b,c);ok=mc==x->want; }
            if(ok&&x->source==2)ok=kind==x->want;
            expect(b);CHECK(factor_now()==(ok?x->factor:0x1000u)&&aborts==0);
        }
    }
    /* Eviolite: Floette (670) form 5 is excluded; otherwise the evolution answer decides. */
    {
        const uint32_t species_list[]={0x29e,0x29d,0x19,0x49e};
        for(unsigned s=0;s<4;++s)for(uint32_t form=0;form<8;++form)for(uint32_t ev=0;ev<2;++ev)for(unsigned holder=2;holder<5;++holder){
            reset();w16(NULL,P+0xc,(uint16_t)species_list[s]);w8(NULL,P+0x153,(uint8_t)form);w8(NULL,P+0x152,5);w8(NULL,P+0x154,5);evolves=ev;
            frame(2,2,holder,0x21,1,0);px_battle_eviolite(HANDLER,CTX,2,0,&api);
            const int reach=holder==2&&!(species_list[s]==0x29e&&form==5);
            expect(reach?"ce2 ":"");CHECK(factor_now()==(reach&&ev?0x1800u:0x1000u));
        }
    }
    /* Critical-hit stages: the attacker, the species and the byte wrap. */
    {
        const uint32_t stages[]={0,1,3,0xfe,0xff,0x1ff};const uint32_t species_list[]={0,0x53,0x71,0x153};
        for(unsigned st=0;st<6;++st)for(unsigned s=0;s<4;++s)for(unsigned holder=2;holder<4;++holder)for(unsigned k=0;k<3;++k){
            reset();w16(NULL,P+0xc,(uint16_t)species_list[s]);frame(2,holder,2,0x21,1,stages[st]);
            static const fn h[]={px_battle_scope_lens,px_battle_lucky_punch,px_battle_stick};
            h[k](HANDLER,CTX,2,0,&api);
            const uint32_t need[]={0,0x71,0x53};const uint32_t add[]={1,2,2};
            const int ok=holder==2&&(k==0||species_list[s]==need[k]);
            expect("");CHECK(px_battle_event_get(0x2d,&events)==(ok?(stages[st]+add[k])&0xff:stages[st])&&aborts==0);
        }
    }
    /* Choice lock: the attacker and Struggle; the record. */
    {
        const uint32_t moves[]={0x21,0xa5,0x100a5,0x10021,0xa4};
        for(unsigned mv=0;mv<5;++mv)for(unsigned holder=2;holder<4;++holder){
            reset();frame(7,holder,7,moves[mv],0,0);px_battle_choice_lock(HANDLER,CTX,2,0,&api);
            const uint32_t m=moves[mv]&0xffff;
            if(holder!=2||m==0xa5){ expect("");CHECK(records==0);continue; }
            char b[64];snprintf(b,sizeof b,"recb/2 lc%x commit ",m);expect(b);
            CHECK(r8(NULL,RECORD+0xf)==2&&r16(NULL,RECORD+4)==0x1b&&r32(NULL,RECORD+8)==(0xc0de0000|m)&&r8(NULL,RECORD+0x10)==2);
            for(uint32_t k=0;k<0x100;++k)if(k!=4&&k!=5&&k!=0xf&&k!=0x10&&(k<8||k>0xb))CHECK(r8(NULL,RECORD+k)==0);
        }
        /* Choice release: key 0x02. */
        for(uint32_t holder=0;holder<5;++holder){
            reset();frame(holder,7,7,0,0,0);px_battle_choice_release(HANDLER,CTX,2,0,&api);
            if(holder!=2){ expect("");continue; }
            expect("reca/2 commit ");
            CHECK(r8(NULL,RECORD+4)==0x1b&&r8(NULL,RECORD+5)==2&&r8(NULL,RECORD+0x11)==1&&r8(NULL,RECORD+0x12)==1);
            for(uint32_t k=0;k<0x100;++k)if(k!=4&&k!=5&&k!=0x11&&k!=0x12)CHECK(r8(NULL,RECORD+k)==0);
        }
        reset();frame(4,7,7,0,0,0);px_battle_choice_release(HANDLER,CTX,4,0,&api);expect("reca/4 commit ");CHECK(r8(NULL,RECORD+5)==4);
    }
    /* Iron Ball grounds the holder (key 0x42 = 1); Light Ball's use paralyzes key 0x03. */
    {
        for(uint32_t holder=0;holder<5;++holder){
            reset();frame(holder,7,7,0,0,0);px_battle_event_set_value(0x42,0,&events);
            px_battle_iron_ball_ground(HANDLER,CTX,2,0,&api);
            expect("");CHECK(px_battle_event_get(0x42,&events)==(holder==2));
        }
        const uint32_t targets[]={0,1,3,5,0x103,0xffffff05};
        for(uint32_t holder=0;holder<5;++holder)for(unsigned t=0;t<6;++t){
            reset();px_battle_event_push(&events);px_battle_event_set_id(0x02,holder,&events);px_battle_event_set_value(0x03,targets[t],&events);log_[0]=0;
            px_battle_light_ball_use(HANDLER,CTX,2,0,&api);
            if(holder!=2){ expect("");continue; }
            char b[48];snprintf(b,sizeof b,"recb/%x cond1 commit ",targets[t]&0xff);expect(b);
            CHECK(r8(NULL,RECORD+0xf)==2&&r16(NULL,RECORD+4)==1&&r32(NULL,RECORD+8)==0xc0de0001);
            for(uint32_t k=0;k<0x100;++k)if(k!=4&&k!=5&&k!=0xf&&(k<8||k>0xb))CHECK(r8(NULL,RECORD+k)==0);
        }
    }
    /* Another holder's species is read. */
    {
        reset();w32(NULL,TABLE+0x84+4*6,P);w16(NULL,P+0xc,0x19);frame(7,6,7,0x21,1,0);
        px_battle_light_ball(HANDLER,CTX,6,0,&api);expect("");CHECK(factor_now()==0x2000);
    }
    /* Through the dispatcher: Choice Band's entries on events 0x40, 0x22 and 0xa8. */
    {
        reset();category=1;
        const uint8_t ev[]={0x40,0x22,0xa8};
        for(uint32_t i=0;i<3;++i){ w8(NULL,ENTRIES+8*i,ev[i]);w32(NULL,ENTRIES+8*i+4,i); }
        w32(NULL,HANDLER+0x18,REGISTERED|1u<<16);w8(NULL,HANDLER+0x3a,2);w32(NULL,BATTLE+0x60,HANDLER);
        frame(2,2,7,0x21,1,0);
        w32(NULL,HANDLER+8,ENTRIES);px_battle_event_dispatch(CTX,0x40,&events);expect("mc21 ");CHECK(factor_now()==0x1800);
        w32(NULL,HANDLER+8,ENTRIES+8);log_[0]=0;px_battle_event_dispatch(CTX,0x22,&events);expect("recb/2 lc21 commit ");
        records=0;w32(NULL,HANDLER+8,ENTRIES+16);log_[0]=0;px_battle_event_dispatch(CTX,0xa8,&events);expect("reca/2 commit ");
        CHECK(aborts==0);
    }
    printf("battle_items_5: %u cases, all tests passed\n",cases);
    return 0;
}
