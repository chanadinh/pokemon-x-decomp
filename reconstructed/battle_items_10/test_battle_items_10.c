#include "battle_items_10.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000,
       HANDLER=0x6000, WORK=0x601c, RECORD=0x7000, CURES=0x7800, SCRATCH=0x7900, ENTRIES=0x7a00, P=MON+0x200 };
enum { RUNNING=0x1000000, REENTRY=0x10000000, REGISTERED=0x20000000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static unsigned aborts,records;static char log_[512];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;(void)p;(void)k;CHECK(0);return 0; }
static uint32_t pokemon_status(void *u,uint32_t p) { (void)u;(void)p;CHECK(0);return 0; }
static const struct px_battle_ability_api base={.events=&events,.pokemon_value=pokemon_value,.pokemon_status=pokemon_status};
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;(void)p;(void)s;CHECK(0);return 0; }
static uint32_t item_param(void *u,uint32_t item,uint32_t k) { (void)u;(void)item;(void)k;CHECK(0);return 0; }
static void popup(void *u,uint32_t c,uint32_t p,uint32_t id) { (void)u;CHECK(c==CTX);notef("pop%u/%u ",p,id); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id)
{ (void)u;CHECK(c==CTX&&records<2);notef("rec%x/%x ",k,id);return RECORD+0x100*records++; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;char t[48];snprintf(t,sizeof t,"msg%x@%x/%x ",id,(r-RECORD)&0xff,k);note(t); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;notef("arg%x@%x ",v,(r-RECORD)&0xff); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r>=RECORD&&!(r&0xff));notef("commit%u ",(r-RECORD)>>8,0); }
static const struct px_battle_items_api items={&base,has_status,item_param,popup,action_record,message,message_arg,commit,CURES,SCRATCH};
static const struct px_battle_items_10_api api={&items};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_items_10_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_power_herb_charge,px_battle_power_herb_check,px_battle_power_herb_use};
    CHECK(f<3);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;records=0;log_[0]=0;
    w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
    for(unsigned i=0;i<7;++i)w8(NULL,P+0x104+i,6);
    const uint16_t list[]={7,0x14,0x1e,0x1d,0x1c,0x25};
    for(unsigned i=0;i<6;++i)w16(NULL,CURES+2*i,list[i]);
    w16(NULL,HANDLER+0x38,0xd6);w32(NULL,HANDLER+0x18,RUNNING);
}
static void frame(uint32_t k2,uint32_t k3,uint32_t k0c,uint32_t k2f,uint32_t move)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x02,k2,&events);px_battle_event_set_id(0x03,k3,&events);
    px_battle_event_set_value(0x0c,k0c,&events);px_battle_event_set_value(0x2f,k2f,&events);px_battle_event_set_value(0x12,move,&events);
    px_battle_event_set_value(0x53,0,&events);log_[0]=0;
}
static unsigned cases;
static void expect(const char *s) { if(strcmp(log_,s)){fprintf(stderr,"got  [%s]\nwant [%s]\n",log_,s);} CHECK(!strcmp(log_,s));++cases; }
int main(void)
{
    /* 1084c140: every value of each of the seven stage bytes, with the bytes around them set. */
    {
        for(unsigned i=0;i<7;++i)for(uint32_t v=0;v<256;++v){
            reset();w8(NULL,P+0x103,0);w8(NULL,P+0x10b,0);w8(NULL,P+0x104+i,(uint8_t)v);
            CHECK(px_battle_stat_lowered(P,&memory)==((int8_t)v<6));++cases;
        }
        reset();CHECK(px_battle_stat_lowered(P,&memory)==0);++cases;
        for(uint32_t mask=0;mask<128;++mask){ reset();for(unsigned i=0;i<7;++i)if(mask>>i&1)w8(NULL,P+0x104+i,5);
            CHECK(px_battle_stat_lowered(P,&memory)==(mask!=0));++cases; }
    }
    /* White Herb's four checks. */
    {
        const uint32_t k0cs[]={0,5,6,7,0x106};
        for(unsigned low=0;low<2;++low)for(uint32_t holder=1;holder<4;++holder)for(unsigned k=0;k<5;++k)for(uint32_t f2f=0;f2f<16;f2f+=4){
            static const fn h[]={px_battle_white_herb_02,px_battle_white_herb_60,px_battle_white_herb_80,px_battle_white_herb_9c};
            for(unsigned i=0;i<4;++i){
                reset();if(low)w8(NULL,P+0x10a,0xff);frame(holder,7,k0cs[k],f2f,0);h[i](HANDLER,CTX,2,WORK,&api);
                int reach=i==0?k0cs[k]!=6:i==1?1:i==2?holder==2:((f2f&8)&&holder==2);
                const int ask=reach&&low;
                expect(ask?"pop0/2 ":"");CHECK(r32(NULL,HANDLER+0x18)==(ask?RUNNING|REENTRY:(uint32_t)RUNNING)&&aborts==0);
            }
        }
    }
    /* White Herb's uses: two records; event 0x7c needs work word 6. */
    {
        const uint32_t sixth[]={0,1,0x80000000};
        for(uint32_t holder=1;holder<4;++holder)for(unsigned s=0;s<3;++s){
            const char *want=holder==2?"rec11/2 commit0 rec3/2 msg3f8@4/2 arg2@4 argd6@4 commit1 ":"";
            reset();frame(holder,7,0,0,0);px_battle_white_herb_use(HANDLER,CTX,2,WORK,&api);expect(want);
            if(holder==2){ CHECK(r8(NULL,RECORD+4)==2);for(uint32_t k=0;k<0x100;++k)if(k!=4)CHECK(r8(NULL,RECORD+k)==0); }
            reset();w32(NULL,WORK+0x18,sixth[s]);frame(holder,7,0,0,0);px_battle_white_herb_use_7c(HANDLER,CTX,2,WORK,&api);
            expect(sixth[s]?want:"");
        }
    }
    /* Mental Herb: the condition list, and the cure body with cure 0x28. */
    {
        for(uint32_t holder=1;holder<4;++holder)for(unsigned c=0;c<3;++c){
            const uint32_t conds[]={0,7,0x25};
            reset();if(conds[c])w32(NULL,P+0x1c+4*conds[c],1);frame(holder,7,0,0,0);
            px_battle_mental_herb_check(HANDLER,CTX,2,WORK,&api);
            const int ask=holder==2&&conds[c];
            expect(ask?"pop0/2 ":"");
            reset();if(conds[c])w32(NULL,P+0x1c+4*conds[c],1);frame(holder,7,0,0,0);
            px_battle_mental_herb_use(HANDLER,CTX,2,WORK,&api);
            expect(ask?"reca/2 commit0 ":"");
            if(ask)CHECK(r8(NULL,RECORD+4)==0x28&&r8(NULL,RECORD+5)==2&&r8(NULL,RECORD+0x11)==1);
            for(uint32_t sx=0;sx<2;++sx){
                reset();if(conds[c])w32(NULL,P+0x1c+4*conds[c],1);w32(NULL,WORK+0x18,sx);frame(holder,7,0,0,0);
                px_battle_mental_herb_use_7c(HANDLER,CTX,2,WORK,&api);expect(ask&&sx?"reca/2 commit0 ":"");
            }
        }
    }
    /* Power Herb: the charge (Sky Drop, the rewrite), the check and the use. */
    {
        const uint32_t moves[]={0x21,0x1fb,0x101fb,0x1fa};
        for(uint32_t holder=2;holder<4;++holder)for(unsigned mv=0;mv<4;++mv)for(unsigned locked=0;locked<2;++locked){
            reset();w32(NULL,WORK,0x77);frame(7,holder,0,0,moves[mv]);
            if(locked)CHECK(px_battle_event_rewrite(0x53,0,&events)==1);
            px_battle_power_herb_charge(HANDLER,CTX,2,WORK,&api);
            const int reach=holder==2&&moves[mv]!=0x1fb;
            expect("");CHECK(r32(NULL,WORK)==(reach&&!locked?1u:0x77u)&&px_battle_event_get(0x53,&events)==(reach&&!locked?1u:0u)&&aborts==0);
        }
        const uint32_t works[]={0,1,0x80000000};
        for(uint32_t holder=2;holder<4;++holder)for(unsigned w=0;w<3;++w){
            reset();w32(NULL,WORK,works[w]);frame(7,holder,0,0,0);px_battle_power_herb_check(HANDLER,CTX,2,WORK,&api);
            const int ask=holder==2&&works[w];
            expect(ask?"pop0/2 ":"");CHECK(r32(NULL,WORK)==works[w]);
        }
        for(uint32_t holder=1;holder<4;++holder){
            reset();frame(holder,7,0,0,0);px_battle_power_herb_use(HANDLER,CTX,2,WORK,&api);
            expect(holder==2?"rec3/2 msg404@4/2 arg2@4 argd6@4 commit0 ":"");
        }
    }
    /* Through the dispatcher: Power Herb's charge on 0x9f, check on 0xa2 and use on 0x7b. */
    {
        reset();w32(NULL,HANDLER+0x18,REGISTERED|1u<<16);
        const uint8_t ev[]={0x9f,0xa2,0x7b};
        for(uint32_t i=0;i<3;++i){ w8(NULL,ENTRIES+8*i,ev[i]);w32(NULL,ENTRIES+8*i+4,i); }
        w8(NULL,HANDLER+0x3a,2);w32(NULL,BATTLE+0x60,HANDLER);
        frame(2,2,0,0,0x13);
        w32(NULL,HANDLER+8,ENTRIES);px_battle_event_dispatch(CTX,0x9f,&events);expect("");CHECK(r32(NULL,WORK)==1);
        w32(NULL,HANDLER+8,ENTRIES+8);log_[0]=0;px_battle_event_dispatch(CTX,0xa2,&events);expect("pop0/2 ");
        CHECK(r32(NULL,HANDLER+0x18)==(REGISTERED|1u<<16|(uint32_t)RUNNING));
        w32(NULL,HANDLER+0x18,REGISTERED|1u<<16);w32(NULL,HANDLER+8,ENTRIES+16);log_[0]=0;px_battle_event_dispatch(CTX,0x7b,&events);
        expect("rec3/2 msg404@4/2 arg2@4 argd6@4 commit0 ");CHECK(aborts==0);
    }
    printf("battle_items_10: %u cases, all tests passed\n",cases);
    return 0;
}
