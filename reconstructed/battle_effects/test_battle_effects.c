#include "battle_effects.h"
#include "../battle_action/battle_action.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n  log: %s\n",__FILE__,__LINE__,#c,log_);exit(1); } } while (0)
enum { SIZE=0x10000, CTX=0x1000, WORD=CTX+0x437c, TABLE=0x6000, MON=0x7000, SIDE=0x9800, REC=0xa000,
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

static uint32_t hp[8];static unsigned faults,routed;
static struct { uint32_t address,value; } rets[16];static unsigned nrets;
static void ret(uint32_t a,uint32_t v) { rets[nrets].address=a;rets[nrets++].value=v; }
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static const struct px_battle_process_api api;
static uint32_t call(void *u,uint32_t a,const uint32_t *args,unsigned n)
{
    (void)u;
    if(a==PX_BATTLE_POKEMON_VALUE){CHECK(n==2&&args[1]==0xd);return hp[mon(args[0])];}
    if(a==0x1081d558){++routed;return px_battle_kind_value(args[0],args[1],&api);}
    if(a==0x10823704){++routed;return px_battle_kind_clear_drops(args[0],args[1],&api);}
    char t[32];snprintf(t,sizeof t,"%x(",a);note(t);
    for(unsigned i=0;i<n;++i){snprintf(t,sizeof t,i?",%x":"%x",args[i]);note(t);}
    note(") ");
    for(unsigned i=0;i<nrets;++i)if(rets[i].address==a)return rets[i].value;
    return 0;
}
static void fault(void *u) { (void)u;++faults;note("FAULT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static unsigned blocked_calls;
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;++blocked_calls;return 0; }
static const struct px_battle_event_api events;
static void handler(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;(void)f;(void)h;(void)c;(void)v;(void)w;
    char t[48];snprintf(t,sizeof t,"ev73[%x] ",px_battle_event_get(2,&events));note(t);
}
static const struct px_battle_event_api events={&memory,NULL,fault,overflow,blocked,handler,STACK,BATTLE,POOL};
static const struct px_battle_process_api api={&events,call,SCRATCH};
static void reset(void)
{
    memset(mem,0,sizeof mem);log_[0]=0;nrets=0;faults=0;routed=0;blocked_calls=0;
    w32(NULL,CTX+8,TABLE);w32(NULL,CTX+0xc,STATE);w32(NULL,CTX+0x1c,SIDE);
    for(unsigned i=0;i<6;++i){w32(NULL,TABLE+0x84+4*i,MON+0x100*i);w8(NULL,MON+0x100*i+0x19,(uint8_t)(0x10+i));hp[i]=100;}
    /* A handler on event 0x73 (owner 7). */
    w8(NULL,ENTRIES,0x73);w32(NULL,ENTRIES+4,0);
    w32(NULL,HANDLER+8,ENTRIES);w32(NULL,HANDLER+0x18,0x20000000u|1u<<16);w8(NULL,HANDLER+0x10,7);
    w32(NULL,BATTLE+0x60,HANDLER);
}
#define OK(s) (strcmp(log_,(s))==0)
static uint8_t sizes[2*PX_BATTLE_ACTION_KINDS];
static void execute(void *u,uint32_t c,uint32_t r) { (void)u;px_battle_action_execute(c,r,&api); }
static const struct px_battle_action_api action={&memory,sizes,execute,fault};

int main(void)
{
    unsigned cases=0;
    /* Kind 5. */
    for(uint32_t v=0;v<32;++v){
        reset();const uint32_t src=v&1?0x1f:2;if(v&2)ret(PX_BATTLE_POSITION,0xa);hp[3]=v&4?0:1;ret(0x10863f5c,v>>3&1);
        w8(NULL,REC+6,3);w8(NULL,REC+7,(uint8_t)src);w16(NULL,REC+4,0x1234);w16(NULL,REC+0xa,(uint16_t)(v&16?0x8005:0x8000));
        const uint32_t r=px_battle_kind_value(CTX,REC,&api);
        char want[160];
        if(v&2)snprintf(want,sizeof want,"1086c2cc(220c,3) ");
        else if(v&4)snprintf(want,sizeof want,"1086c2cc(220c,3) ");
        else snprintf(want,sizeof want,"1086c2cc(220c,3) 10863f5c(1000,7300,%x,1234) %s",src==0x1f?0:MON+0x200,(v&8)&&(v&16)?"1086bbe8(1000,a008) ":"");
        CHECK(OK(want)&&r==(!(v&2)&&!(v&4)&&(v&8)));++cases;
    }
    /* Kind 0xe. */
    for(uint32_t v=0;v<8;++v){
        reset();if(v&1)ret(PX_BATTLE_POSITION,0xa);hp[2]=v&2?0:1;w8(NULL,REC+4,2);for(uint32_t s=0;s<7;++s)w8(NULL,REC+5+s,(uint8_t)(1+s));
        w8(NULL,REC+0xc,(uint8_t)(v&4?3:4));w8(NULL,MON+0x200+0x155,0x77);
        const uint32_t r=px_battle_kind_set_stages(CTX,REC,&api);
        if(v&3){CHECK(r==0&&OK("1086c2cc(220c,2) ")&&mem[MON+0x355]==0x77);++cases;continue;}
        char want[400];snprintf(want,sizeof want,"1086c2cc(220c,2) 108714fc(7200,1,1) 108714fc(7200,2,2) 108714fc(7200,3,3) 108714fc(7200,4,4) 108714fc(7200,5,5) "
                 "108714fc(7200,6,6) 108714fc(7200,7,7) 1086c22c(5555,c,2,1,2,3,4,5,6,7,%x) ",v&4?3:4);
        CHECK(r==1&&OK(want)&&mem[MON+0x200+0x155]==(v&4?3:0x77));++cases;
    }
    /* Kind 0x11: lowered stages return to 6. */
    for(uint32_t v=0;v<130;++v){
        reset();hp[1]=v<128;w8(NULL,REC+4,1);
        for(uint32_t s=0;s<7;++s)w8(NULL,MON+0x100+0x104+s,(uint8_t)((v%128)>>s&1?2+s/3:(s&1?6:9+s)));
        if(v==129)for(uint32_t s=0;s<7;++s)w8(NULL,MON+0x100+0x104+s,0xff);   /* signed -1: below 6 */
        if(v==129)hp[1]=1;
        const uint32_t r=px_battle_kind_clear_drops(CTX,REC,&api);
        if(v==128){CHECK(r==0&&OK(""));++cases;continue;}
        CHECK(OK("1086c22c(5555,d,1) "));
        if(v==129){for(uint32_t s=0;s<7;++s)CHECK(mem[MON+0x100+0x104+s]==6);CHECK(r==1);++cases;continue;}
        for(uint32_t s=0;s<7;++s)CHECK(mem[MON+0x100+0x104+s]==((v>>s&1)?6:(s&1?6:9+s)));
        CHECK(r==(v!=0));++cases;
    }
    /* Kind 0x1a: messages per flag. */
    for(uint32_t f=0;f<0x11;++f)for(uint32_t r=0;r<2;++r){
        reset();ret(0x108ace94,r);w8(NULL,REC+4,4);w8(NULL,REC+5+(f>>3),(uint8_t)(1u<<(f&7)));w8(NULL,REC+8,1);
        const uint32_t res=px_battle_kind_clear_field(CTX,REC,&api);
        char want[160];const int32_t msg=px_battle_field_clear_message(f);
        if(r&&msg>=0)snprintf(want,sizeof want,"108ace94(1,%x) 1086b5e0(5555,69,%x,1,ffff0000) ",f,(uint32_t)msg);else snprintf(want,sizeof want,"108ace94(1,%x) ",f);
        CHECK(OK(want)&&res==r);++cases;
    }
    {static const int32_t want[0x11]={0x84,0x88,0x8c,0x90,0x94,0x98,0x9c,0xa0,0xa4,-1,-1,0xac,0xb0,0xb4,0xd8,-1,-1};
        for(uint32_t f=0;f<0x11;++f){CHECK(px_battle_field_clear_message(f)==want[f]);}
        ++cases;}
    reset();w8(NULL,REC+4,2);w8(NULL,REC+5,0xff);w8(NULL,REC+6,0xff);ret(0x108ace94,1);CHECK(px_battle_kind_clear_field(CTX,REC,&api)==1&&strstr(log_,"108ace94(0,7) ")&&!strstr(log_,"108ace94(0,8)"));++cases;   /* length 2: flags 0-7 only */
    /* Kind 0x1c: the 0xa entry. */
    for(uint32_t r=0;r<2;++r){reset();ret(0x1086fde0,r);w8(NULL,REC+4,0xa);w8(NULL,REC+5,7);w32(NULL,REC+8,0x123);
        const uint32_t res=px_battle_kind_side_entry(CTX,REC,&api);
        CHECK(res==r&&OK(r?"1086fde0(9800,7,123) 1086c22c(5555,26,7,123) 1086c22c(5555,62,0) 1086c22c(5555,59,7) 1086bbe8(1000,a00c) ":"1086fde0(9800,7,123) "));++cases;}
    /* Other entries: 108712c0, else the fallback value. */
    for(uint32_t v=0;v<24;++v){
        reset();ret(0x108712c0,v&1);w8(NULL,REC+4,3);w8(NULL,REC+0x30,(uint8_t)(v>>1&1));
        const uint32_t type=(v>>2)%6;   /* 0..5: types 3 and 4 have values */
        uint32_t word=type|(v&2?0:0);word|=(type==3?0x15u<<3:0)|(type==4?0x15u<<9:0);if(v>=18)word=(type==3?3u|0x1fu<<3:4u|0x1fu<<9);
        w32(NULL,REC+8,word);
        const uint32_t res=px_battle_kind_side_entry(CTX,REC,&api);
        char want[200];
        if(v&1)snprintf(want,sizeof want,"108712c0(9800,3,%x,0) 1086c22c(5555,24,3,%x) 1086bbe8(1000,a00c) ",word,word);
        else if(!(v&2))snprintf(want,sizeof want,"108712c0(9800,3,%x,0) ",word);
        else{const uint32_t t=word&7;const uint32_t val=t==3?word>>3&0x3f:word>>9&0x3f;
            if((t!=3&&t!=4)||val==0x1f)snprintf(want,sizeof want,"108712c0(9800,3,%x,0) ",word);
            else snprintf(want,sizeof want,"108712c0(9800,3,%x,0) 1086f878(9800,3,%x) 1086c22c(5555,25,3,%x) 1086bbe8(1000,a00c) ",word,val,val);}
        CHECK(OK(want));CHECK(res==(strstr(want,"1086bbe8")!=NULL));++cases;
    }
    reset();w8(NULL,REC+4,0xb);CHECK(px_battle_kind_side_entry(CTX,REC,&api)==0&&faults==1);++cases;
    /* Kind 0x2b: event 0x73 inline (no blocked check) when +0x5c has low bits; command 0x5d with a +0x208 value. */
    for(uint32_t v=0;v<4;++v){
        reset();w8(NULL,REC+4,1);w8(NULL,REC+5,4);w32(NULL,MON+0x100+0x5c,v&1?0x12:0x18);w16(NULL,MON+0x400+0x208,(uint16_t)(v&2?9:0));ret(PX_BATTLE_POSITION,3);
        /* A second handler (event 0x99) registered at depth 5: cleared when the inline dispatch ends. */
        w32(NULL,HANDLER+4,HANDLER+0x40);w32(NULL,HANDLER+0x40,HANDLER);w8(NULL,ENTRIES+0x10,0x99);
        w32(NULL,HANDLER+0x40+8,ENTRIES+0x10);w32(NULL,HANDLER+0x40+0x18,0x20000000u|1u<<16|5);w8(NULL,HANDLER+0x40+0x10,7);
        CHECK(px_battle_kind_transfer(CTX,REC,&api)==1);
        char want[200];snprintf(want,sizeof want,"%s1082aea4(7400,7100) 1086c22c(5555,2a,1,4) %s",v&1?"ev73[14] ":"",v&2?"1086c2cc(220c,4) 1086c22c(5555,5d,3) ":"");
        CHECK(OK(want)&&blocked_calls==0&&r32(NULL,BATTLE+0x68)==0&&r16(NULL,HANDLER+0x40+0x18)==(v&1?0:5));++cases;
    }
    /* End to end: kinds 5 and 0x11 committed through battle_action and the processor. */
    {
        reset();w32(NULL,WORD,0);
        for(unsigned i=0;i<PX_BATTLE_ACTION_KINDS;++i){sizes[2*i]=(uint8_t)i;sizes[2*i+1]=0x10;}
        ret(0x10863f5c,1);
        uint32_t r=px_battle_action_record(CTX,5,0,&action);w8(NULL,r+6,2);w8(NULL,r+7,0x1f);w16(NULL,r+4,7);
        px_battle_action_commit(CTX,r,&action);
        r=px_battle_action_record(CTX,0x11,0,&action);w8(NULL,r+4,2);w8(NULL,MON+0x200+0x104,0xfe);w32(NULL,r,r32(NULL,r)|0x1000000u);
        px_battle_action_commit(CTX,r,&action);
        CHECK(routed==2&&OK("1086c2cc(220c,2) 10863f5c(1000,7200,0,7) 1086c22c(5555,d,2) ")&&mem[MON+0x200+0x104]==6&&faults==0);
        CHECK((r32(NULL,WORD)&0x70000000u)==0x70000000u&&(r32(NULL,WORD)>>10&0x1ff)==0);++cases;
    }
    printf("battle_effects: %u cases, all tests passed\n",cases);
    return 0;
}
