#include "battle_items_7.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000,
       HANDLER=0x6000, RECORD=0x7000, CURES=0x7800, SCRATCH=0x7900, ENTRIES=0x7a00, P=MON+0x200 };
enum { RUNNING=0x1000000, REENTRY=0x10000000, REGISTERED=0x20000000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
/* State: max HP, item parameter 3, full-HP answer, packed types, decided, contact. */
static uint32_t maxhp,param,full,types,decided,contact;static unsigned aborts,records;static char log_[512];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;CHECK(p==P&&k==0xe);note("ve ");return maxhp; }
static uint32_t side_of(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("so%u ",id,0);return 0x40|id; }
static const struct px_battle_ability_api base={.events=&events,.pokemon_value=pokemon_value,.side_of=side_of};
static uint32_t item_param(void *u,uint32_t item,uint32_t k) { (void)u;CHECK(k==3);notef("ip%x ",item,0);return param; }
static void popup(void *u,uint32_t c,uint32_t p,uint32_t id) { (void)u;(void)c;(void)p;(void)id;CHECK(0); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id)
{ (void)u;CHECK(c==CTX&&records<1);notef("rec%x/%x ",k,id);++records;return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;char t[48];snprintf(t,sizeof t,"msg%x@%x/%x ",id,r-RECORD,k);note(t); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;notef("arg%x@%x ",v,r-RECORD); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD);note("commit "); }
static const struct px_battle_items_api items={&base,NULL,item_param,popup,action_record,message,message_arg,commit,CURES,SCRATCH};
static const struct px_battle_abilities_1_api abilities={.base=&base};
static const struct px_battle_modifier_api modifier={.base=&base,.item_param=item_param};
static uint32_t condition(void *u,uint32_t s) { (void)u;notef("cond%x ",s,0);return 0xc0de0000|s; }
static uint32_t full_hp(void *u,uint32_t p) { (void)u;CHECK(p==P);note("full ");return full; }
static uint32_t packed_types(void *u,uint32_t p) { (void)u;CHECK(p==P);note("types ");return types; }
static uint32_t battle_decided(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("dec ");return decided; }
static uint32_t move_flag(void *u,uint32_t m,uint32_t f) { (void)u;notef("mf%x/%u ",m,f);return contact; }
static const struct px_battle_items_7_api api={&items,&abilities,&modifier,condition,full_hp,packed_types,battle_decided,move_flag};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_items_7_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_leftovers_check,px_battle_leftovers};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);maxhp=160;param=8;full=0;types=0;decided=0;contact=1;aborts=0;records=0;log_[0]=0;
    w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
    w16(NULL,HANDLER+0x38,0x110);
}
static void frame(uint32_t k2,uint32_t k3,uint32_t k4)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x02,k2,&events);px_battle_event_set_id(0x03,k3,&events);
    px_battle_event_set_id(0x04,k4,&events);log_[0]=0;
}
/* Every byte of the record outside the listed offsets is zero. */
static void only(const uint32_t *o,unsigned n)
{
    for(uint32_t k=0;k<0x100;++k){ int listed=0;for(unsigned i=0;i<n;++i)listed|=o[i]==k;if(!listed)CHECK(r8(NULL,RECORD+k)==0); }
}
static uint32_t share(uint32_t d) { const uint32_t s=maxhp/d;return s?s:1; }
static unsigned cases;
static void expect(const char *s) { if(strcmp(log_,s)){fprintf(stderr,"got  [%s]\nwant [%s]\n",log_,s);} CHECK(!strcmp(log_,s));++cases; }
int main(void)
{
    /* Toxic Orb and Flame Orb on event 0x80: the holder (key 0x02). */
    {
        for(uint32_t holder=0;holder<5;++holder)for(unsigned flame=0;flame<2;++flame){
            reset();frame(holder,7,7);if(flame)px_battle_flame_orb(HANDLER,CTX,2,0,&api); else px_battle_toxic_orb(HANDLER,CTX,2,0,&api);
            if(holder!=2){ expect("");CHECK(records==0);continue; }
            if(flame){ expect("recb/2 cond4 msg102@14/2 arg2@14 arg110@14 commit ");
                CHECK(r16(NULL,RECORD+4)==4&&r32(NULL,RECORD+8)==0xc0de0004&&r8(NULL,RECORD+0xf)==2&&r32(NULL,RECORD+0x1c)==0x110);
                const uint32_t o[]={4,5,8,9,0xa,0xb,0xf,0x1c,0x1d};only(o,9); }
            else { expect("recb/2 msgf0@14/2 arg2@14 arg110@14 commit ");
                CHECK(r16(NULL,RECORD+4)==5&&r32(NULL,RECORD+8)==0x79&&r8(NULL,RECORD+0xf)==2);
                const uint32_t o[]={4,8,0xf};only(o,3); }
        }
        /* +0xc is cleared even when the record held something there. */
        reset();memset(mem+RECORD,0xaa,0x40);frame(2,7,7);px_battle_toxic_orb(HANDLER,CTX,2,0,&api);CHECK(r8(NULL,RECORD+0xc)==0);++cases;
        reset();memset(mem+RECORD,0xaa,0x40);frame(2,7,7);px_battle_flame_orb(HANDLER,CTX,2,0,&api);CHECK(r8(NULL,RECORD+0xc)==0);++cases;
    }
    /* Their event-0x7c uses: key 0x02 the holder, key 0x03's low byte the record's battler. */
    {
        const uint32_t targets[]={0,1,3,5,0x103,0xffffff05};
        for(uint32_t holder=0;holder<5;++holder)for(unsigned t=0;t<6;++t)for(unsigned flame=0;flame<2;++flame){
            reset();frame(holder,targets[t],7);
            if(flame)px_battle_flame_orb_use(HANDLER,CTX,2,0,&api); else px_battle_toxic_orb_use(HANDLER,CTX,2,0,&api);
            if(holder!=2){ expect("");continue; }
            char b[48];snprintf(b,sizeof b,"recb/%x %scommit ",targets[t]&0xff,flame?"cond4 ":"");expect(b);
            CHECK(r8(NULL,RECORD+0xf)==2&&r16(NULL,RECORD+4)==(flame?4:5)&&r32(NULL,RECORD+8)==(flame?0xc0de0004u:0x79u));
            const uint32_t o[]={4,8,9,0xa,0xb,0xf};only(o,6);
        }
    }
    /* Sticky Barb on event 0x80 and Black Sludge's damage: kind 6. */
    {
        const uint32_t maxima[]={0,1,7,8,9,160,0xffff,0x10000};const uint32_t params[]={1,4,8,0x10};
        for(unsigned mx=0;mx<8;++mx)for(unsigned k=0;k<4;++k)for(uint32_t holder=2;holder<4;++holder){
            reset();maxhp=maxima[mx];param=params[k];frame(holder,7,7);px_battle_sticky_barb(HANDLER,CTX,2,0,&api);
            if(holder!=2){ expect("");continue; }
            expect("rec6/2 ip110 ve so2 msg414@c/2 arg2@c arg110@c commit ");
            CHECK(r16(NULL,RECORD+4)==(share(params[k])&0xffff)&&r8(NULL,RECORD+6)==2&&r8(NULL,RECORD+7)==2&&r16(NULL,RECORD+8)==0x26f);
            CHECK(r8(NULL,RECORD+0xa)==0x42&&r8(NULL,RECORD+0xb)==0xa);
            const uint32_t o[]={4,5,6,7,8,9,0xa,0xb};only(o,8);
        }
        /* +7 keeps its other bits. */
        reset();w8(NULL,RECORD+7,0x81);frame(2,7,7);px_battle_sticky_barb(HANDLER,CTX,2,0,&api);CHECK(r8(NULL,RECORD+7)==0x83);++cases;
    }
    /* Sticky Barb on event 0x53: the defender, key 0x47, contact, the attacker's empty hand. */
    {
        for(uint32_t holder=3;holder<5;++holder)for(uint32_t k47=0;k47<2;++k47)for(uint32_t c=0;c<2;++c)for(uint32_t held=0;held<3;++held)for(unsigned mv=0;mv<2;++mv){
            const uint32_t helds[]={0,1,0x100};const uint32_t moves[]={0x21,0x10021};
            reset();contact=c;w16(NULL,MON+0x500+0x12,(uint16_t)helds[held]);w16(NULL,MON+0x500+0x10,0xffff);w16(NULL,MON+0x500+0x14,0xffff);
            px_battle_event_push(&events);px_battle_event_set_id(0x04,holder==4?2:7,&events);px_battle_event_set_value(0x03,0x105,&events);
            px_battle_event_set_value(0x47,k47,&events);px_battle_event_set_value(0x12,moves[mv],&events);log_[0]=0;
            px_battle_sticky_barb_move(HANDLER,CTX,2,0,&api);
            char b[64]="";
            if(holder==4&&!k47){ snprintf(b,sizeof b,"mf21/0 %s",c&&!helds[held]?"rec25/2 commit ":""); }
            expect(b);
            if(holder==4&&!k47&&c&&!helds[held]){ CHECK(r8(NULL,RECORD+4)==5);const uint32_t o[]={4};only(o,1); }
            else CHECK(records==0);
        }
    }
    /* Leftovers: the check (event 0x7f) and the restore (event 0x7b). */
    {
        const uint32_t flags[]={0,RUNNING,RUNNING|REENTRY,0x20000001};
        for(uint32_t holder=2;holder<4;++holder)for(uint32_t f=0;f<2;++f)for(unsigned fl=0;fl<4;++fl){
            reset();full=f;w32(NULL,HANDLER+0x18,flags[fl]);w32(NULL,RECORD+4,0x100);frame(holder,7,7);
            px_battle_leftovers_check(HANDLER,CTX,2,0,&api);
            const int ask=holder==2&&!f;
            expect(holder!=2?"":f?"full ":"full rec0/2 commit ");
            CHECK(r32(NULL,RECORD+4)==(ask?0x101u:0x100u));
            CHECK(r32(NULL,HANDLER+0x18)==(ask&&(flags[fl]&RUNNING)?flags[fl]|REENTRY:flags[fl]));
            reset();full=f;maxhp=fl*50+3;frame(holder,7,7);px_battle_leftovers(HANDLER,CTX,2,0,&api);
            expect(holder!=2?"":f?"full ":"full rec4/2 ve msg398@8/2 arg2@8 arg110@8 commit ");
            if(ask){ CHECK(r16(NULL,RECORD+4)==share(16)&&r8(NULL,RECORD+6)==2);const uint32_t o[]={4,5,6};only(o,3); }
        }
    }
    /* Black Sludge: decided, each type field, and both paths. */
    {
        const uint32_t typesets[]={0,3,3<<5,3<<10,3<<15,0x7fff&~0x63,0x12|0x12<<5|0x12<<10,2|3<<5,0xffffffff,0x1f|0x1f<<5|0x1f<<10,
            0x13,0x13<<5,0x13<<10,0x23,3<<20};
        for(unsigned t=0;t<15;++t)for(uint32_t d=0;d<2;++d)for(uint32_t holder=2;holder<4;++holder)for(uint32_t f=0;f<2;++f){
            reset();types=typesets[t];decided=d;full=f;param=8;maxhp=100;frame(holder,7,7);
            px_battle_black_sludge(HANDLER,CTX,2,0,&api);
            const uint32_t x=typesets[t];const int poison=(x&0x1f)==3||(x>>5&0x1f)==3||(x>>10&0x1f)==3;
            char b[160]="";
            if(holder==2){ strcat(b,"dec ");
                if(!d){ strcat(b,"types ");strcat(b,poison?"rec4/2 ip110 ve msg398@8/2 arg2@8 arg110@8 commit ":"rec6/2 ve so2 msg414@c/2 arg2@c arg110@c commit "); } }
            expect(b);
            if(holder==2&&!d){ CHECK(r16(NULL,RECORD+4)==(poison?100u/8:100u/8)&&r8(NULL,RECORD+6)==2);
                if(!poison)CHECK(r16(NULL,RECORD+8)==0x26f&&r8(NULL,RECORD+7)==2&&r8(NULL,RECORD+0xa)==0x42&&r8(NULL,RECORD+0xb)==0xa); }
        }
        /* The restore's divisor is parameter 3, the loss's 8. */
        reset();types=3;param=4;maxhp=100;frame(2,7,7);px_battle_black_sludge(HANDLER,CTX,2,0,&api);CHECK(r16(NULL,RECORD+4)==25);++cases;
        reset();types=4;param=4;maxhp=100;frame(2,7,7);px_battle_black_sludge(HANDLER,CTX,2,0,&api);CHECK(r16(NULL,RECORD+4)==12);++cases;
    }
    /* Shell Bell: the attacker, key 0x4e, the damage and parameter 3. */
    {
        const uint32_t damages[]={0,1,7,8,9,100,0xffff,0x10000,0xffffffff};const uint32_t params[]={1,8,0x10,0xffffffff};
        for(unsigned d=0;d<9;++d)for(unsigned k=0;k<4;++k)for(uint32_t k4e=0;k4e<2;++k4e)for(uint32_t holder=2;holder<4;++holder){
            reset();param=params[k];frame(7,holder,7);px_battle_event_set_value(0x4e,k4e,&events);px_battle_event_set_value(0x33,damages[d],&events);log_[0]=0;
            px_battle_shell_bell(HANDLER,CTX,2,0,&api);
            const int hit=holder==2&&!k4e&&damages[d];
            expect(hit?"ip110 rec4/2 msg398@8/2 arg2@8 arg110@8 commit ":"");
            if(hit){ uint32_t a=damages[d]/params[k];if(!a)a=1;CHECK(r16(NULL,RECORD+4)==(a&0xffff)&&r8(NULL,RECORD+6)==2);const uint32_t o[]={4,5,6};only(o,3); }
        }
    }
    /* Life Orb's recoil: the attacker, 1/10, message 0x3fb naming only the battler. */
    {
        const uint32_t maxima[]={0,9,10,11,100,0xffff};
        for(unsigned mx=0;mx<6;++mx)for(uint32_t holder=2;holder<4;++holder){
            reset();maxhp=maxima[mx];frame(7,holder,7);px_battle_life_orb_recoil(HANDLER,CTX,2,0,&api);
            if(holder!=2){ expect("");continue; }
            expect("rec6/2 ve msg3fb@c/2 arg2@c commit ");
            CHECK(r16(NULL,RECORD+4)==share(10)&&r8(NULL,RECORD+6)==2);const uint32_t o[]={4,5,6};only(o,3);
        }
    }
    /* Big Root: the attacker, the item factor on key 0x36. */
    {
        const uint32_t params[]={0,30,50};
        for(unsigned k=0;k<3;++k)for(uint32_t holder=2;holder<4;++holder){
            reset();param=params[k];frame(7,holder,7);px_battle_event_set_multiplier(0x36,0x1000,0,0,&events);log_[0]=0;
            px_battle_big_root(HANDLER,CTX,2,0,&api);
            expect(holder==2?"ip110 ":"");
            CHECK(px_battle_event_get(0x36,&events)==(holder==2?(uint32_t)(((uint64_t)params[k]*4096+409600)/100):0x1000u));
        }
    }
    /* Sweeps: every max HP up to 2,000 for Leftovers, Life Orb and Black Sludge's loss, and
     * parameters 1..40 for Sticky Barb, Black Sludge's restore and Shell Bell. */
    {
        for(uint32_t mx=0;mx<=2000;++mx){
            reset();maxhp=mx;frame(2,2,7);px_battle_leftovers(HANDLER,CTX,2,0,&api);CHECK(r16(NULL,RECORD+4)==share(16));++cases;
            reset();maxhp=mx;frame(2,2,7);px_battle_life_orb_recoil(HANDLER,CTX,2,0,&api);CHECK(r16(NULL,RECORD+4)==share(10));++cases;
            reset();maxhp=mx;types=0;frame(2,2,7);px_battle_black_sludge(HANDLER,CTX,2,0,&api);CHECK(r16(NULL,RECORD+4)==share(8));++cases;
        }
        for(uint32_t p=1;p<=40;++p)for(uint32_t mx=0;mx<=400;mx+=7){
            reset();maxhp=mx;param=p;frame(2,2,7);px_battle_sticky_barb(HANDLER,CTX,2,0,&api);CHECK(r16(NULL,RECORD+4)==share(p));++cases;
            reset();maxhp=mx;param=p;types=3<<10;frame(2,2,7);px_battle_black_sludge(HANDLER,CTX,2,0,&api);CHECK(r16(NULL,RECORD+4)==share(p));++cases;
            reset();param=p;frame(7,2,7);px_battle_event_set_value(0x4e,0,&events);px_battle_event_set_value(0x33,mx+1,&events);
            px_battle_shell_bell(HANDLER,CTX,2,0,&api);CHECK(r16(NULL,RECORD+4)==((mx+1)/p?(mx+1)/p:1));++cases;
        }
    }
    /* Through the dispatcher: the Leftovers check keeps the handler running; the restore follows on 0x7b. */
    {
        reset();
        w8(NULL,ENTRIES,0x7f);w32(NULL,ENTRIES+4,0);w8(NULL,ENTRIES+8,0x7b);w32(NULL,ENTRIES+12,1);
        w32(NULL,HANDLER+8,ENTRIES);w32(NULL,HANDLER+0x18,REGISTERED|1u<<16);w8(NULL,HANDLER+0x3a,2);w32(NULL,BATTLE+0x60,HANDLER);
        frame(2,7,7);px_battle_event_dispatch(CTX,0x7f,&events);expect("full rec0/2 commit ");
        CHECK(r32(NULL,HANDLER+0x18)==(REGISTERED|1u<<16|(uint32_t)RUNNING));
        records=0;w32(NULL,HANDLER+0x18,REGISTERED|1u<<16);w32(NULL,HANDLER+8,ENTRIES+8);log_[0]=0;
        px_battle_event_dispatch(CTX,0x7b,&events);expect("full rec4/2 ve msg398@8/2 arg2@8 arg110@8 commit ");CHECK(r16(NULL,RECORD+4)==10&&aborts==0);
    }
    printf("battle_items_7: %u cases, all tests passed\n",cases);
    return 0;
}
