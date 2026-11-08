#include "battle_items_6.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000,
       HANDLER=0x6000, SCRATCH=0x7900, ENTRIES=0x7a00 };
enum { REGISTERED=0x20000000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static uint32_t param,moved;static unsigned aborts;static char log_[512];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static const struct px_battle_ability_api base={.events=&events};
static uint32_t item_param(void *u,uint32_t item,uint32_t k) { (void)u;CHECK(k==3);notef("ip%x ",item,0);return param; }
static const struct px_battle_modifier_api modifier={.base=&base,.item_param=item_param};
static uint32_t turn_flag(void *u,uint32_t p,uint32_t f) { (void)u;notef("tf%x/%u ",(p-MON)>>8,f);return moved; }
/* The helpers rewrite the word so the result can be traced: low byte the value, high byte a tag. */
static void set_turns(void *u,uint32_t w,uint32_t t) { (void)u;CHECK(w==SCRATCH);notef("st%x/%x ",r32(NULL,w),t);w32(NULL,w,0x11000000|t); }
static void set_trap_turns(void *u,uint32_t w,uint32_t t) { (void)u;CHECK(w==SCRATCH);notef("tt%x/%x ",r32(NULL,w),t);w32(NULL,w,0x22000000|t); }
static void set_trap_flag(void *u,uint32_t w,uint32_t v) { (void)u;CHECK(w==SCRATCH);notef("tb%x/%x ",r32(NULL,w),v);w32(NULL,w,0x33000000|v); }
static const struct px_battle_items_6_api api={&modifier,turn_flag,set_turns,set_trap_turns,set_trap_flag,SCRATCH};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_items_6_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_bright_powder,px_battle_heat_rock,px_battle_light_clay};
    CHECK(f<3);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);param=10;moved=0;aborts=0;log_[0]=0;
    w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
    w16(NULL,HANDLER+0x38,0xd5);w32(NULL,SCRATCH,0xdeadbeef);
}
static void frame(uint32_t k2,uint32_t k3,uint32_t k4)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x02,k2,&events);px_battle_event_set_id(0x03,k3,&events);
    px_battle_event_set_id(0x04,k4,&events);px_battle_event_set_multiplier(0x36,0x1000,0,0,&events);log_[0]=0;
}
static uint32_t factor_now(void) { return px_battle_event_get(0x36,&events); }
static uint32_t times(uint32_t v,uint32_t f) { return (uint32_t)((uint64_t)((int64_t)(int32_t)v*(int32_t)f+0x800)>>12); }
/* The item factor reference, (p + 100)% in 4096 units, for small p. */
static uint32_t item_factor(uint32_t p) { return (uint32_t)(((uint64_t)p*4096+409600)/100); }
static unsigned cases;
static void expect(const char *s) { if(strcmp(log_,s)){fprintf(stderr,"got  [%s]\nwant [%s]\n",log_,s);} CHECK(!strcmp(log_,s));++cases; }
int main(void)
{
    /* The accuracy-drop factor: exact for reductions 1..100, -0.5 to 0 at zero, and the wrap. */
    {
        CHECK(px_battle_accuracy_drop(0)==0);++cases;
        for(uint32_t r=1;r<=0x10000;++r){ CHECK(px_battle_accuracy_drop(r)==(r<<12)/100);++cases; }
        /* 0xfffff000 is a float exactly; it saturates to INT32_MAX before the division. */
        CHECK(px_battle_accuracy_drop(0xffffffff)==21474836);++cases;
        /* 0x80000 << 12 = 2^31: saturates; 0x7ffff << 12 = 2^31 - 4096 is exact. */
        CHECK(px_battle_accuracy_drop(0x80000)==21474836);++cases;
        CHECK(px_battle_accuracy_drop(0x7ffff)==(0x7ffffu<<12)/100);++cases;
        /* 0x1000001 << 12 wraps to 0x1000: exact. */
        CHECK(px_battle_accuracy_drop(0x1000001)==0x1000/100);++cases;
        /* 0x10001 << 12 = 0x10001000 = 268439552 is a float exactly (17 significant bits), and
         * the +0.5 rounds back to it. */
        CHECK(px_battle_accuracy_drop(0x10001)==268439552u/100);++cases;
        /* 0x1fffff << 12 = 0x1fffff000 wraps to 0xfffff000: saturates. */
        CHECK(px_battle_accuracy_drop(0x1fffff)==21474836);++cases;
    }
    /* BrightPowder and Lax Incense: the defender, parameters 0..120 and wrapping ones. */
    {
        const uint32_t extra[]={0xffff,0x10000,0x10063,0x10064,0x10065,0xffffff9c,0xffffffff};
        for(uint32_t k=0;k<121+7;++k)for(uint32_t holder=2;holder<5;++holder)for(unsigned lax=0;lax<2;++lax){
            const uint32_t p=k<121?k:extra[k-121];
            reset();param=p;frame(7,holder==3?2:7,holder==4?2:7);
            if(lax)px_battle_lax_incense(HANDLER,CTX,2,0,&api); else px_battle_bright_powder(HANDLER,CTX,2,0,&api);
            const int hit=holder==4;
            expect(hit?"ipd5 ":"");
            const uint32_t r=lax?((100-p)&0xffff):100-p;
            CHECK(factor_now()==(hit?times(0x1000,px_battle_accuracy_drop(r)):0x1000u)&&aborts==0);
            if(hit&&p<=100)CHECK(px_battle_accuracy_drop(r)==((100-p)<<12)/100);
        }
        /* The usual 10: 90% = 0xe66. */
        reset();param=10;frame(7,7,2);px_battle_bright_powder(HANDLER,CTX,2,0,&api);CHECK(factor_now()==0xe66);++cases;
        /* p = 101: BrightPowder saturates, Lax Incense narrows first. */
        reset();param=101;frame(7,7,2);px_battle_lax_incense(HANDLER,CTX,2,0,&api);CHECK(factor_now()==times(0x1000,2684313));++cases;
    }
    /* Wide Lens and Zoom Lens: the attacker, the item factor, the defender's turn flag. */
    {
        for(uint32_t p=0;p<60;p+=10)for(uint32_t holder=2;holder<4;++holder)for(uint32_t mv=0;mv<2;++mv)for(uint32_t d=0;d<3;++d){
            const uint32_t defender[]={1,5,0x104};
            reset();param=p;moved=mv?0x80:0;frame(7,holder,defender[d]);
            px_battle_wide_lens(HANDLER,CTX,2,0,&api);
            expect(holder==2?"ipd5 ":"");CHECK(factor_now()==(holder==2?item_factor(p):0x1000u));
            px_battle_event_pop(&events);frame(7,holder,defender[d]);
            px_battle_zoom_lens(HANDLER,CTX,2,0,&api);
            char b[48]="";if(holder==2){ snprintf(b,sizeof b,"tf%x/1 %s",defender[d]&0xff,mv?"ipd5 ":""); }
            expect(b);CHECK(factor_now()==(holder==2&&mv?item_factor(p):0x1000u)&&aborts==0);
        }
    }
    /* The weather rocks: the setter, the weather, the turns written to key 0x25. */
    {
        static const fn h[]={px_battle_heat_rock,px_battle_damp_rock,px_battle_icy_rock,px_battle_smooth_rock};
        const uint32_t params[]={0,5,8,0x108,0xffffffff};
        for(unsigned i=0;i<4;++i)for(uint32_t w=0;w<7;++w)for(uint32_t holder=2;holder<4;++holder)for(unsigned k=0;k<5;++k){
            const uint32_t weather=w<6?w:0x101+i;
            reset();param=params[k];frame(7,holder,7);px_battle_event_set_value(0x3a,weather,&events);px_battle_event_set_value(0x25,5,&events);
            log_[0]=0;h[i](HANDLER,CTX,2,0,&api);
            const int hit=holder==2&&weather==i+1;
            expect(hit?"ipd5 ":"");CHECK(px_battle_event_get(0x25,&events)==(hit?params[k]:5u)&&aborts==0);
        }
    }
    /* Light Clay: the holder (key 0x02), key 0x56's low byte, the parameter's low byte. */
    {
        const uint32_t screens[]={0,1,2,3,0x100,0x101,0x102,0xff};const uint32_t params[]={8,5,0x108,0xffffffff};
        for(unsigned s=0;s<8;++s)for(uint32_t holder=1;holder<4;++holder)for(unsigned k=0;k<4;++k){
            reset();param=params[k];frame(holder,7,7);px_battle_event_set_value(0x56,screens[s],&events);
            px_battle_event_set_value(0x1f,0x4321,&events);log_[0]=0;
            px_battle_light_clay(HANDLER,CTX,2,0,&api);
            const int hit=holder==2&&(screens[s]&0xff)<2;
            char b[48]="";if(hit)snprintf(b,sizeof b,"ipd5 st4321/%x ",params[k]&0xff);
            expect(b);CHECK(px_battle_event_get(0x1f,&events)==(hit?0x11000000u|(params[k]&0xff):0x4321u));
            CHECK(r32(NULL,SCRATCH)==(hit?0x11000000u|(params[k]&0xff):0xdeadbeefu)&&aborts==0);
        }
    }
    /* Grip Claw (key 0x1e = 8) and Binding Band: the attacker. */
    {
        const uint32_t kinds[]={0,7,8,9,0x108};
        for(unsigned k=0;k<5;++k)for(uint32_t holder=2;holder<4;++holder){
            reset();frame(7,holder,7);px_battle_event_set_value(0x1e,kinds[k],&events);px_battle_event_set_value(0x1f,0x55,&events);log_[0]=0;
            px_battle_grip_claw(HANDLER,CTX,2,0,&api);
            const int hit=holder==2&&kinds[k]==8;
            expect(hit?"tt55/8 ":"");CHECK(px_battle_event_get(0x1f,&events)==(hit?0x22000008u:0x55u));
            reset();frame(7,holder,7);px_battle_event_set_value(0x1e,kinds[k],&events);px_battle_event_set_value(0x1f,0x55,&events);log_[0]=0;
            px_battle_binding_band(HANDLER,CTX,2,0,&api);
            expect(holder==2?"tb55/1 ":"");CHECK(px_battle_event_get(0x1f,&events)==(holder==2?0x33000001u:0x55u)&&aborts==0);
        }
    }
    /* Through the dispatcher: BrightPowder on 0x39, Heat Rock on 0x85, Light Clay on 0xab. */
    {
        reset();param=10;
        const uint8_t ev[]={0x39,0x85,0xab};
        for(uint32_t i=0;i<3;++i){ w8(NULL,ENTRIES+8*i,ev[i]);w32(NULL,ENTRIES+8*i+4,i); }
        w32(NULL,HANDLER+0x18,REGISTERED|1u<<16);w8(NULL,HANDLER+0x3a,2);w32(NULL,BATTLE+0x60,HANDLER);
        frame(2,2,2);px_battle_event_set_value(0x3a,1,&events);px_battle_event_set_value(0x25,5,&events);
        px_battle_event_set_value(0x56,1,&events);px_battle_event_set_value(0x1f,0x77,&events);log_[0]=0;
        w32(NULL,HANDLER+8,ENTRIES);px_battle_event_dispatch(CTX,0x39,&events);expect("ipd5 ");CHECK(factor_now()==0xe66);
        w32(NULL,HANDLER+8,ENTRIES+8);log_[0]=0;px_battle_event_dispatch(CTX,0x85,&events);expect("ipd5 ");CHECK(px_battle_event_get(0x25,&events)==10);
        w32(NULL,HANDLER+8,ENTRIES+16);log_[0]=0;px_battle_event_dispatch(CTX,0xab,&events);expect("ipd5 st77/a ");
        CHECK(px_battle_event_get(0x1f,&events)==0x1100000a&&aborts==0);
    }
    printf("battle_items_6: %u cases, all tests passed\n",cases);
    return 0;
}
