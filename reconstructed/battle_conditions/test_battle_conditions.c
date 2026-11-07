#include "battle_conditions.h"
#include "../battle_ability/battle_ability.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n  log: %s\n",__FILE__,__LINE__,#c,log_);exit(1); } } while (0)
enum { SIZE=0x10000, CTX=0x1000, WORD=CTX+0x437c, TEXT=CTX+0x433c, MON=0x7000, MSG=0xa000,
       STACK=0xb000, BATTLE=0xc000, POOL=0xc100, SCRATCH=0xc800, HANDLER=0xd000, ENTRIES=0xd400, STATE=0x5555 };
static uint8_t mem[SIZE];static char log_[4096];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static unsigned faults,blocked_calls;static uint32_t ability,noposition;
static uint32_t call(void *u,uint32_t a,const uint32_t *args,unsigned n)
{
    (void)u;
    if(a==PX_BATTLE_POKEMON_VALUE){CHECK(n==2&&args[1]==0x10);return ability;}
    char t[32];snprintf(t,sizeof t,"%x(",a);note(t);
    for(unsigned i=0;i<n;++i){snprintf(t,sizeof t,i?",%x":"%x",args[i]);note(t);}
    note(") ");
    if(a==PX_BATTLE_POSITION)return noposition?0xa:3;
    if(a==0x1086b284)return r32(NULL,args[0]);
    return 0;
}
static void fault(void *u) { (void)u;++faults;note("FAULT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;++blocked_calls;return 0; }
static const struct px_battle_event_api events;
static void handler(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;(void)h;(void)c;(void)v;(void)w;char t[80];const struct px_battle_event_api *e=&events;
    switch(f){
    case 0:snprintf(t,sizeof t,"ev71[%x,%x,%x,%x] ",px_battle_event_get(4,e),px_battle_event_get(3,e),px_battle_event_get(0x1e,e),px_battle_event_get(0x1f,e));break;
    case 1:snprintf(t,sizeof t,"ev72[%x,%x,%x] ",px_battle_event_get(4,e),px_battle_event_get(3,e),px_battle_event_get(0x1e,e));break;
    case 2:snprintf(t,sizeof t,"ev73[%x] ",px_battle_event_get(2,e));break;
    default:snprintf(t,sizeof t,"ev9c[%x,%x] ",px_battle_event_get(2,e),px_battle_event_get(0x2f,e));break;
    }
    note(t);w32(NULL,WORD,0xbad);
}
static const struct px_battle_event_api events={&memory,NULL,fault,overflow,blocked,handler,STACK,BATTLE,POOL};
static const struct px_battle_process_api api={&events,call,SCRATCH};
static void reset(void)
{
    memset(mem,0,sizeof mem);log_[0]=0;faults=blocked_calls=0;ability=0;noposition=0;
    w32(NULL,CTX+0xc,STATE);
    for(unsigned i=0;i<6;++i)w8(NULL,MON+0x100*i+0x19,(uint8_t)(0x10+i));
    static const uint8_t ev[4]={0x71,0x72,0x73,0x9c};
    for(unsigned i=0;i<4;++i){
        const uint32_t h=HANDLER+0x40*i,e=ENTRIES+0x10*i;
        w8(NULL,e,ev[i]);w32(NULL,e+4,i);
        w32(NULL,h+8,e);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x10,7);
        w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<3?h+0x40:0);
    }
    w32(NULL,BATTLE+0x60,HANDLER);
}
#define OK(s) (strcmp(log_,(s))==0)
#define P(i) (MON+0x100*(i))

int main(void)
{
    unsigned cases=0;
    {static const uint32_t want[0x12]={0,0x299,0x295,0x298,0x297,0x296,0x29a,0x29b};
     for(uint32_t s=0;s<0x12;++s){CHECK(px_battle_status_animation(s)==want[s]);}
     ++cases;}
    static const uint32_t statuses[]={0,1,2,3,4,5,6,7,8,9,0x10,0x11,0x25};
    for(unsigned k=0;k<sizeof statuses/sizeof *statuses;++k)for(uint32_t v=0;v<128;++v){
        const uint32_t s=statuses[k];
        reset();noposition=v&1;const uint32_t build=v>>1&1,msg=v&4?MSG:0,keep=v>>3&1,src=v&16?0:P(2);
        w16(NULL,P(3)+0x12,(uint16_t)(v&32?0x44:0));ability=v&64?0x7f:0x20;
        w16(NULL,TEXT,0x1234);w16(NULL,TEXT+2,0x8085);w32(NULL,TEXT+0x20,9);w32(NULL,WORD,0x4321);
        if(s==3&&(v&64)){w16(NULL,P(3)+0xc,PX_BATTLE_SHAYMIN);w8(NULL,P(3)+0x153,1);}
        w16(NULL,HANDLER+0xc0+0x18,6);   /* the item handler's registration depth: cleared by the inline 0x73 dispatch */
        px_battle_apply_status(CTX,P(3),src,s,0x77,build,keep,msg,&api);
        char want[800]="",t[200];
        snprintf(t,sizeof t,"10865118(1000,7300,%x,77) ",s);strcat(want,t);
        const uint32_t anim=px_battle_status_animation(s);
        if(anim){strcat(want,"1086c2cc(220c,13) ");if(!noposition){snprintf(t,sizeof t,"1086c22c(5555,57,3,%x) ",anim);strcat(want,t);}}
        if(build){snprintf(t,sizeof t,"1087afa0(%x,77,7300,%x) 1086bbe8(1000,%x) ",s,TEXT,TEXT);strcat(want,t);}
        else if(msg)strcat(want,"1086bbe8(1000,a000) ");
        if(s==3&&(v&64))strcat(want,"108679ac(7300) 1086fff0(7300,0) 1086c22c(5555,5b,13,0) 108678f8(7300) 1086b5e0(5555,6a,de,13,ffff0000) ");
        strcat(want,"1086b284(537c) ");
        const uint32_t from=src?0x12:0x1f;
        if(s<6){snprintf(t,sizeof t,"ev71[13,%x,%x,77] ",from,s);strcat(want,t);}
        else if(s!=0x10){snprintf(t,sizeof t,"ev72[13,%x,%x] ",from,s);strcat(want,t);}
        else{strcat(want,"ev73[13] ");if(v&64)strcat(want,"10864dc0(1000,7300) ");}
        if(!keep&&(v&32))strcat(want,"1086b284(537c) ev9c[13,4] ");
        CHECK(OK(want));
        CHECK(r32(NULL,WORD)==0x4321&&r16(NULL,HANDLER+0xc0+0x18)==0&&r32(NULL,BATTLE+0x68)==0);
        if(s==0x10&&(keep||!(v&32)))CHECK(blocked_calls==0);   /* the inline 0x73 pass skips the blocked check */
        if(build)CHECK(r32(NULL,TEXT)==0&&r32(NULL,TEXT+0x20)==0);else CHECK(r16(NULL,TEXT)==0x1234);
        ++cases;
    }
    /* Shaymin in its normal form (0) does not revert. */
    reset();w16(NULL,P(3)+0xc,PX_BATTLE_SHAYMIN);w8(NULL,P(3)+0x153,0);px_battle_apply_status(CTX,P(3),0,3,0,0,1,0,&api);
    CHECK(!strstr(log_,"108679ac"));++cases;
    printf("battle_conditions: %u cases, all tests passed\n",cases);
    return 0;
}
