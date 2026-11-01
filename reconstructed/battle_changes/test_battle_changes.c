#include "battle_changes.h"
#include "../battle_action/battle_action.h"
#include "../battle_ability/battle_ability.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n  log: %s\n",__FILE__,__LINE__,#c,log_);exit(1); } } while (0)
enum { SIZE=0x10000, CTX=0x1000, WORD=CTX+0x437c, TABLE=0x6000, MON=0x7000, REC=0xa000,
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

static uint32_t hp[8],noposition;static unsigned faults,routed,refuse89;
static struct { uint32_t address,value; } rets[16];static unsigned nrets;
static void ret(uint32_t a,uint32_t v) { rets[nrets].address=a;rets[nrets++].value=v; }
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static const struct px_battle_process_api api;
static uint32_t call(void *u,uint32_t a,const uint32_t *args,unsigned n)
{
    (void)u;
    if(a==PX_BATTLE_POKEMON_VALUE){CHECK(n==2&&args[1]==0xd);return hp[mon(args[0])];}
    if(a==0x1081f7a0){++routed;return px_battle_kind_hp(args[0],args[1],&api);}
    if(a==0x10822e28){++routed;return px_battle_kind_types(args[0],args[1],&api);}
    char t[32];snprintf(t,sizeof t,"%x(",a);note(t);
    for(unsigned i=0;i<n;++i){snprintf(t,sizeof t,i?",%x":"%x",args[i]);note(t);}
    note(") ");
    if(a==PX_BATTLE_POSITION&&n==2&&args[1]<32)return noposition>>args[1]&1?0xa:0;
    for(unsigned i=0;i<nrets;++i)if(rets[i].address==a)return rets[i].value;
    if(a==0x1086b284)return r32(NULL,args[0]);
    return 0;
}
static void fault(void *u) { (void)u;++faults;note("FAULT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static const struct px_battle_event_api events;
static void handler(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;(void)h;(void)c;(void)v;(void)w;char t[48];
    if(f==0){snprintf(t,sizeof t,"ev89[%x,%x] ",px_battle_event_get(4,&events),px_battle_event_get(0x53,&events));note(t);if(refuse89)px_battle_event_rewrite(0x53,0,&events);}
    else{snprintf(t,sizeof t,"ev9c[%x,%x] ",px_battle_event_get(2,&events),px_battle_event_get(0x2f,&events));note(t);w32(NULL,WORD,0xbad);}
}
static const struct px_battle_event_api events={&memory,NULL,fault,overflow,blocked,handler,STACK,BATTLE,POOL};
static const struct px_battle_process_api api={&events,call,SCRATCH};
static void reset(void)
{
    memset(mem,0,sizeof mem);log_[0]=0;nrets=0;faults=0;routed=0;noposition=0;refuse89=0;
    w32(NULL,CTX+8,TABLE);w32(NULL,CTX+0xc,STATE);
    for(unsigned i=0;i<6;++i){const uint32_t p=MON+0x100*i;w32(NULL,TABLE+0x84+4*i,p);w8(NULL,p+0x19,(uint8_t)(0x10+i));hp[i]=100;}
    mem[0x19]=0xee;   /* read through a null source with the popup flag, as the original does */
    static const uint8_t ev[2]={0x89,0x9c};
    for(unsigned i=0;i<2;++i){
        const uint32_t h=HANDLER+0x40*i,e=ENTRIES+0x10*i;
        w8(NULL,e,ev[i]);w32(NULL,e+4,i);
        w32(NULL,h+8,e);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x10,7);
        w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
    }
    w32(NULL,BATTLE+0x60,HANDLER);
}
#define OK(s) (strcmp(log_,(s))==0)
static uint8_t sizes[2*PX_BATTLE_ACTION_KINDS];
static void execute(void *u,uint32_t c,uint32_t r) { (void)u;px_battle_action_execute(c,r,&api); }
static const struct px_battle_action_api action={&memory,sizes,execute,fault};

int main(void)
{
    unsigned cases=0;
    /* Kind 6. */
    for(uint32_t v=0;v<128;++v){
        reset();if(v&1)noposition=1u<<3;hp[3]=v&2?0:9;ret(0x10874348,v&4?0x12:0x13);refuse89=v>>3&1;
        w32(NULL,REC,6u|(v&16?0x1fu:1u)<<8|(v&32?0x800000u:0));w8(NULL,REC+6,3);w8(NULL,REC+7,(uint8_t)((v&64?3:0)|(v&4?1:0)|(v>>4&1)));   /* bit 0 with either value of 10874348 */
        w16(NULL,REC+4,0x321);w16(NULL,REC+8,0x654);w8(NULL,REC+0xa,7);w8(NULL,REC+0xb,8);
        const uint32_t depth=r32(NULL,STACK);
        const uint32_t r=px_battle_kind_condition(CTX,REC,&api);
        CHECK(r32(NULL,STACK)==depth);
        const uint32_t flags=mem[REC+7];
        char want[400]="1086c2cc(220c,3) ";
        int ok=1;
        if(v&1)ok=0;
        else if(v&2)ok=0;
        else{
            if(flags&1){strcat(want,"10874348(7300) ");if(!(v&4))ok=0;}
            if(ok){strcat(want,"ev89[13,1] ");if(v&8)ok=0;}
        }
        if(ok){
            char t[200];const uint32_t sid=v&16?0xee:0x11;
            if(v&32){snprintf(t,sizeof t,"1086c22c(5555,66,%x) ",sid);strcat(want,t);}
            if(flags&2)strcat(want,"1086a6e0(1000,654,7,8,0,0) ");
            strcat(want,"108671c0(1000,7300,321,a00c) ");
            if(v&32){snprintf(t,sizeof t,"1086c22c(5555,67,%x) ",sid);strcat(want,t);}
        }
        CHECK(OK(want)&&r==(uint32_t)ok);++cases;
    }
    /* Kind 7: three targets, positive, negative and zero changes. */
    for(uint32_t v=0;v<64;++v){
        reset();w8(NULL,REC+4,3);w8(NULL,REC+5,(uint8_t)(v&1));w8(NULL,REC+6,(uint8_t)(v>>1&1));
        static const uint8_t ids[3]={1,2,3};static const int32_t change[3]={30,-12,0};
        for(unsigned i=0;i<3;++i){w8(NULL,REC+7+i,ids[i]);w32(NULL,REC+0x14+4*i,(uint32_t)change[i]);w16(NULL,MON+0x100*ids[i]+0x10,80);w16(NULL,MON+0x100*ids[i]+0xe,100);}
        if(v&4){noposition|=1u<<2;}
        if(v&8){hp[3]=0;}
        if(v&16){w16(NULL,MON+0x112,0x44);}
        if(v&32){noposition|=1u<<0x11;}
        const uint32_t r=px_battle_kind_hp(CTX,REC,&api);
        char want[800]="",t[160];
        for(unsigned i=0;i<3;++i){
            snprintf(t,sizeof t,"1086c2cc(220c,%x) ",ids[i]);strcat(want,t);
            if(i==1&&(v&4))continue;
            if(i==2&&(v&8))continue;
            if(i==0)strcat(want,"1086c22c(5555,3,11,1e) ");
            if(i==1)strcat(want,"10867a04(1000,7200,c,0) ");
            if(!(v&1)){snprintf(t,sizeof t,"1086c2cc(220c,%x) ",0x10+ids[i]);strcat(want,t);if(!(i==0&&(v&32))){snprintf(t,sizeof t,"1086c22c(5555,48,%x) ",0x10+ids[i]);strcat(want,t);}}
            if(i==0&&!(v&2)&&(v&16))strcat(want,"1086b284(537c) ev9c[11,1] ");
        }
        CHECK(OK(want)&&r==1&&r16(NULL,MON+0x110)==100&&r32(NULL,WORD)==0);++cases;
    }
    /* Kind 7: none processed; HP 16-bit wrap. */
    reset();w8(NULL,REC+4,1);w8(NULL,REC+7,1);noposition=2;CHECK(px_battle_kind_hp(CTX,REC,&api)==0);++cases;
    reset();w8(NULL,REC+4,1);w8(NULL,REC+5,1);w8(NULL,REC+6,1);w8(NULL,REC+7,1);w32(NULL,REC+0x14,0x1fff0);w16(NULL,MON+0x110,0x20);w16(NULL,MON+0x10e,0x100);
    px_battle_kind_hp(CTX,REC,&api);CHECK(r16(NULL,MON+0x110)==0x10&&strstr(log_,"1086c22c(5555,3,11,fff0) "));++cases;
    /* Kind 0xb. */
    for(uint32_t v=0;v<128;++v){
        reset();if(v&1)noposition=1u<<4;hp[4]=v&2?0:5;ret(0x10862858,v>>2&1);
        w32(NULL,REC,0xbu|(v&8?0x1fu:2u)<<8|(v&16?0x800000u:0));w16(NULL,REC+4,0x77);w32(NULL,REC+8,0xabcd);w8(NULL,REC+0xc,(uint8_t)(v&32?5:0));
        w8(NULL,REC+0xd,(uint8_t)(v&64?1:0));w8(NULL,REC+0xe,(uint8_t)(v&32?0:3));w8(NULL,REC+0xf,4);w8(NULL,REC+0x10,9);w16(NULL,REC+0x16,(uint16_t)(v&64?0:0x81));
        const uint32_t r=px_battle_kind_status(CTX,REC,&api);
        const uint32_t src=v&8?0:MON+0x200,sid=v&8?0xee:0x12,quiet=0;
        char want[400];
        if((v&1)||(v&2)){CHECK(OK("1086c2cc(220c,4) ")&&r==0);++cases;continue;}
        snprintf(want,sizeof want,"1086c2cc(220c,4) 10862858(1000,7400,%x,77,abcd,9,%u) ",src,v&32?1u:0u);
        if(v&4){CHECK(OK(want)&&r==0);++cases;continue;}
        char t[200];
        if(v&16){snprintf(t,sizeof t,"1086c22c(5555,66,%x) ",sid);strcat(want,t);}
        snprintf(t,sizeof t,"108629b8(1000,7400,%x,77,abcd,%u,%u,a014) ",src,quiet,v&32?0u:1u);strcat(want,t);
        if(v&16){snprintf(t,sizeof t,"1086c22c(5555,67,%x) ",sid);strcat(want,t);}
        CHECK(OK(want)&&r==1);++cases;
    }
    /* Kind 0xb: quiet when the message has no kind and +0xd is 0. */
    reset();w32(NULL,REC,0xb|2u<<8);w8(NULL,REC+0xf,4);CHECK(px_battle_kind_status(CTX,REC,&api)==1&&strstr(log_,"108629b8(1000,7400,7200,0,0,1,0,a014) "));++cases;
    /* Kind 0x13: set types; popup only on a change; the messages. */
    for(uint32_t v=0;v<256;++v){
        reset();hp[2]=1;w8(NULL,MON+0x200+0x100,3);w8(NULL,MON+0x200+0x101,5);w8(NULL,MON+0x200+0x102,0x12);
        static const uint16_t sets[8]={3|5<<5|0x12<<10, 5|3<<5|0x12<<10, 3|5<<5|7<<10, 9|9<<5|0x12<<10, 4|5<<5|0x12<<10, 3|3<<5|0x12<<10, 9|9<<5|0x11<<10, 3|4<<5|0x12<<10};
        const uint16_t set=sets[v&7];
        w32(NULL,REC,0x13|2u<<8|(v&8?0x800000u:0));w16(NULL,REC+4,set);w8(NULL,REC+6,2);w8(NULL,REC+7,(uint8_t)(v>>4&1));w8(NULL,REC+8,(uint8_t)(v>>5&1));
        ret(0x10870994,v&64?set:0x9999);
        if(v&128)w16(NULL,MON+0x200+0xc,PX_BATTLE_ARCEUS);
        const uint32_t r=px_battle_kind_types(CTX,REC,&api);
        if(v&128){CHECK(r==0&&OK("1086c2cc(220c,2) "));++cases;continue;}
        const int changed=(v&7)>=2;
        char want[400],t[160];snprintf(want,sizeof want,"1086c2cc(220c,2) 1086c22c(5555,17,2,%x) 10870994(7200) ",set);
        if(changed&&(v&8))strcat(want,"1086c22c(5555,66,12) ");
        if(!(v&16)){
            const int single=(set&0x1f)==(set>>5&0x1f)&&(set&0x7c00)==0x4800&&!(v&64);
            if(single){snprintf(t,sizeof t,"1086b5e0(5555,6a,383,2,%x,ffff0000) ",set&0x1f);strcat(want,t);}
            else if(v&32)strcat(want,"1086b5e0(5555,69,4a,ffff0000) ");
        }
        if(changed&&(v&8))strcat(want,"1086c22c(5555,67,12) ");
        CHECK(OK(want)&&r==1&&mem[MON+0x300]==(set&0x1f)&&mem[MON+0x301]==(set>>5&0x1f)&&mem[MON+0x302]==(set>>10&0x1f));++cases;
    }
    /* End to end: an HP record and a gated type record through battle_action and the processor. */
    {
        reset();w32(NULL,WORD,0);
        for(unsigned i=0;i<PX_BATTLE_ACTION_KINDS;++i){sizes[2*i]=(uint8_t)i;sizes[2*i+1]=0x20;}
        uint32_t r=px_battle_action_record(CTX,7,0,&action);w8(NULL,r+4,1);w8(NULL,r+5,1);w8(NULL,r+6,1);w8(NULL,r+7,4);w32(NULL,r+0x14,0xfffffff6);
        px_battle_action_commit(CTX,r,&action);
        r=px_battle_action_record(CTX,0x13,0,&action);w16(NULL,r+4,0xd|0xd<<5|0x12<<10);w8(NULL,r+6,4);w8(NULL,r+7,1);w32(NULL,r,r32(NULL,r)|0x1000000u);
        px_battle_action_commit(CTX,r,&action);
        CHECK(routed==2&&OK("1086c2cc(220c,4) 10867a04(1000,7400,a,0) 1086c2cc(220c,4) 1086c22c(5555,17,4,49ad) 10870994(7400) ")&&mem[MON+0x500]==0xd&&faults==0);
        CHECK((r32(NULL,WORD)&0x70000000u)==0x70000000u&&(r32(NULL,WORD)>>10&0x1ff)==0);++cases;
    }
    printf("battle_changes: %u cases, all tests passed\n",cases);
    return 0;
}
