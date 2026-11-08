#include "battle_abilities_10.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, ME=2, OTHER=5 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static uint32_t hp_value,max_value,category_value,weather_value,ally_value;
static unsigned aborts,ncommit;static uint8_t commits[4][0x20];static char log_[1024];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static uint32_t value(void *u,uint32_t p,uint32_t k) { (void)u;notef("v%u/%x ",mon(p),k);CHECK(k==0xd||k==0xe);return k==0xd?hp_value:max_value; }
static uint32_t move_category(void *u,uint32_t m) { (void)u;notef("cat%x ",m,0);return category_value; }
static const struct px_battle_ability_api base={&events,value,move_category,NULL,NULL,NULL,NULL};
static uint32_t weather(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("w ");return weather_value; }
static uint32_t is_ally(void *u,uint32_t id,uint32_t o) { (void)u;notef("ally%u/%u ",id,o);return ally_value; }
static void flower_gift(void *u,uint32_t h,uint32_t c,uint32_t id,uint32_t s,uint32_t f) { (void)u;CHECK(h==HANDLER&&c==CTX);
    char t[48];snprintf(t,sizeof t,"gift%u/%u/%u ",id,s,f);note(t); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);
    memset(mem+RECORD,0xa5,0x20);w32(NULL,RECORD,0x11);return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;notef("msg%x/%x ",id,(r-RECORD)*16+k); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;(void)r;notef("arg%x ",v,0); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD&&ncommit<4);memcpy(commits[ncommit++],mem+RECORD,0x20);note("commit "); }
static const struct px_battle_abilities_10_api api={&base,weather,is_ally,flower_gift,action_record,message,message_arg,commit};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_abilities_10_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_forecast_start,px_battle_forecast_weather};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(uint32_t species,uint32_t form)
{
    memset(mem,0,sizeof mem);memset(commits,0,sizeof commits);aborts=ncommit=0;log_[0]=0;
    hp_value=max_value=category_value=weather_value=ally_value=0;
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
    w16(NULL,MON+0x200+0xc,(uint16_t)species);w8(NULL,MON+0x200+0x153,(uint8_t)form);w16(NULL,HANDLER+0x38,0x3b);
}
struct keys { uint32_t k2,k3,k4,k10,k1b; };
static void frame(struct keys k)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x02,k.k2,&events);px_battle_event_set_id(0x03,k.k3,&events);px_battle_event_set_id(0x04,k.k4,&events);
    px_battle_event_set_value(0x10,k.k10,&events);px_battle_event_set_value(0x12,0x10088,&events);px_battle_event_set_value(0x1b,k.k1b,&events);
    px_battle_event_set_multiplier(0x36,0x1000,0,0x100000,&events);log_[0]=0;
}
static uint32_t get(uint32_t key) { return px_battle_event_get(key,&events); }
static void run(fn f,struct keys k) { frame(k);f(HANDLER,CTX,ME,WORK,&api);if(aborts)fprintf(stderr,"log %s\n",log_);CHECK(aborts==0); }
static uint32_t c32(unsigned n,unsigned o) { return (uint32_t)(commits[n][o]|commits[n][o+1]<<8|commits[n][o+2]<<16|(uint32_t)commits[n][o+3]<<24); }
static void cat(char *w,size_t n,const char *f,uint32_t a,uint32_t b) { size_t l=strlen(w);snprintf(w+l,n-l,f,a,b); }
#define EXPECT(w) do { if(strcmp(log_,w)){fprintf(stderr,"got  %s\nwant %s\n",log_,w);CHECK(0);} } while (0)
static void form_ok(uint32_t to,uint32_t flags) { CHECK(ncommit==1&&commits[0][4]==ME&&commits[0][5]==to&&c32(0,0)==flags&&commits[0][6]==0xa5); }
int main(void)
{
    unsigned cases=0;char want[512];
    static const uint32_t ids[3]={ME,OTHER,0x100+ME},species[4]={0x15f,0x1a5,0x22b,0x115f};
    /* Forecast's reset handlers: Castform out of its normal form returns to it. */
    for(unsigned a=0;a<3;++a)for(unsigned s=0;s<4;++s)for(uint32_t f=0;f<4;++f)for(uint32_t w=0;w<2;++w)for(unsigned ab=0;ab<2;++ab) {
        static const fn h[3]={px_battle_forecast_leave,px_battle_forecast_suppressed,px_battle_forecast_lost};
        const struct keys k={ids[a],OTHER,OTHER,ab?0x3b:0x3c,0};
        for(unsigned i=0;i<3;++i) {
            reset(species[s],f);w32(NULL,WORK,w);run(h[i],k);
            const int gate=i==0?k.k2==ME:i==1?(int)w:(k.k2==ME&&!ab);
            const int on=gate&&species[s]==0x15f&&f!=0;
            EXPECT(on?"rec3a/2 msgde/82 arg2 commit ":"");if(on)form_ok(0,0x11);++cases;
        }
    }
    /* Forecast following the weather. */
    for(unsigned s=0;s<4;++s)for(uint32_t f=0;f<4;++f)for(uint32_t wt=0;wt<6;++wt)for(uint32_t w=0;w<3;++w) {
        const uint32_t wanted=wt>=1&&wt<=3?wt:0;const int cast=species[s]==0x15f;
        reset(species[s],f);weather_value=wt;w32(NULL,WORK,w);run(px_battle_forecast_weather,(struct keys){OTHER,OTHER,OTHER,0,0});
        want[0]=0;if(w){strcat(want,"w ");if(cast&&wanted!=f)strcat(want,"rec3a/2 msgde/82 arg2 commit ");}
        EXPECT(want);if(ncommit)form_ok(wanted,0x800011);
        reset(species[s],f);weather_value=wt;w32(NULL,WORK,w);run(px_battle_forecast_start,(struct keys){OTHER,OTHER,OTHER,0,0});
        strcpy(want,"w ");if(cast&&wanted!=f)strcat(want,"rec3a/2 msgde/82 arg2 commit ");
        EXPECT(want);if(ncommit)form_ok(wanted,0x800011);CHECK(r32(NULL,WORK)==1);cases+=2;
    }
    /* Flower Gift's boosts in sun for itself and its allies. */
    for(unsigned s=0;s<4;++s)for(uint32_t wt=0;wt<3;++wt)for(unsigned al=0;al<2;++al)for(unsigned c=0;c<4;++c) {
        static const uint32_t cats[4]={1,2,0x101,0x102};
        const int cher=species[s]==0x1a5;
        reset(species[s],0);weather_value=wt;ally_value=al;struct keys k={OTHER,0x104,0x106,0,cats[c]};run(px_battle_flower_gift_defense,k);
        want[0]=0;if(cher){strcat(want,"w ");if(wt==1){strcat(want,"ally2/6 ");}}
        EXPECT(want);CHECK(get(0x36)==(cher&&wt==1&&al&&cats[c]==2?0x1800u:0x1000u));
        reset(species[s],0);weather_value=wt;ally_value=al;category_value=cats[c];run(px_battle_flower_gift_attack,k);
        want[0]=0;if(cher){strcat(want,"w ");if(wt==1){strcat(want,"ally2/4 ");if(al)strcat(want,"cat88 ");}}
        EXPECT(want);CHECK(get(0x36)==(cher&&wt==1&&al&&cats[c]==1?0x1800u:0x1000u));cases+=2;
    }
    /* Flower Gift's form calls. */
    for(unsigned a=0;a<3;++a)for(unsigned s=0;s<4;++s)for(uint32_t w=0;w<2;++w)for(uint32_t wt=0;wt<3;++wt)for(unsigned ab=0;ab<2;++ab) {
        const int cher=species[s]==0x1a5,me=ids[a]==ME;const struct keys k={ids[a],OTHER,OTHER,ab?0x3b:0x3c,0};
        reset(species[s],0);w32(NULL,WORK,w);run(px_battle_flower_gift_leave,k);EXPECT(w&&me&&cher?"gift2/0/0 ":"");
        reset(species[s],0);w32(NULL,WORK,w);run(px_battle_flower_gift_suppressed,k);EXPECT(w&&cher?"gift2/0/0 ":"");
        reset(species[s],0);w32(NULL,WORK,w);weather_value=wt;run(px_battle_flower_gift_weather,k);
        want[0]=0;if(w&&cher)cat(want,sizeof want,"w gift2/%u/1 ",wt==1,0);EXPECT(want);
        reset(species[s],0);w32(NULL,WORK,w);run(px_battle_flower_gift_lost,k);EXPECT(w&&cher&&me&&!ab?"gift2/0/0 ":"");
        reset(species[s],0);w32(NULL,WORK,w);weather_value=wt;run(px_battle_flower_gift_start,k);
        want[0]=0;if(cher)cat(want,sizeof want,"w gift2/%u/1 ",wt==1,0);EXPECT(want);CHECK(r32(NULL,WORK)==(cher?1u:w));
        cases+=5;
    }
    /* Zen Mode: zen form at half HP or less; its leave handler always resets. */
    for(unsigned s=0;s<4;++s)for(uint32_t f=0;f<3;++f)for(uint32_t hp=0;hp<8;++hp)for(uint32_t mx=5;mx<8;++mx) {
        const int darm=species[s]==0x22b;const uint32_t zen=hp<=mx/2;
        reset(species[s],f);hp_value=hp;max_value=mx;run(px_battle_zen_mode,(struct keys){OTHER,OTHER,OTHER,0,0});
        want[0]=0;if(darm){strcat(want,"v2/d v2/e ");if(zen!=f)cat(want,sizeof want,"rec3a/2 msg%x/81 commit ",zen?0xbf:0xc0,0);}
        EXPECT(want);if(ncommit)form_ok(zen,0x800011);++cases;
    }
    for(unsigned a=0;a<3;++a)for(unsigned s=0;s<4;++s)for(uint32_t f=0;f<2;++f) {
        reset(species[s],f);run(px_battle_zen_mode_leave,(struct keys){ids[a],OTHER,OTHER,0,0});
        const int on=ids[a]==ME&&species[s]==0x22b;EXPECT(on?"rec3a/2 msgc0/81 arg2 commit ":"");if(on)form_ok(0,0x11);++cases;
    }
    /* Through the dispatcher: Forecast starts on event 0x61, then follows the weather on 0x81. */
    {
        reset(0x15f,0);weather_value=2;
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7c00+0x10*i;
            w8(NULL,entries,i?0x81:0x61);w32(NULL,entries+4,i);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ME);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);frame((struct keys){OTHER,OTHER,OTHER,0,0});px_battle_event_dispatch(CTX,0x61,&events);
        EXPECT("w rec3a/2 msgde/82 arg2 commit ");CHECK(aborts==0&&r32(NULL,HANDLER+0x1c)==1);
        w8(NULL,MON+0x200+0x153,2);weather_value=1;w32(NULL,HANDLER+0x40+0x1c,1);ncommit=0;
        frame((struct keys){OTHER,OTHER,OTHER,0,0});px_battle_event_dispatch(CTX,0x81,&events);
        EXPECT("w rec3a/2 msgde/82 arg2 commit ");CHECK(aborts==0&&commits[0][5]==1);cases+=2;
    }
    printf("battle_abilities_10: %u cases, all tests passed\n",cases);
    return 0;
}
