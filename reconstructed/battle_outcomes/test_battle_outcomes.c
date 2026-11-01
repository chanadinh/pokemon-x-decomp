#include "battle_outcomes.h"
#include "../battle_action/battle_action.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n  log: %s\n",__FILE__,__LINE__,#c,log_);exit(1); } } while (0)
enum { SIZE=0x10000, CTX=0x1000, WORD=CTX+0x437c, LIST=CTX+0xca4, TABLE=0x6000, MON=0x7000, REC=0xa000,
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

static uint32_t hp[8],side_of[0x20],second_negative;static unsigned faults,routed;
static struct { uint32_t address,value; } rets[16];static unsigned nrets;
static void ret(uint32_t a,uint32_t v) { rets[nrets].address=a;rets[nrets++].value=v; }
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static const struct px_battle_process_api api;
static uint32_t call(void *u,uint32_t a,const uint32_t *args,unsigned n)
{
    (void)u;
    if(a==PX_BATTLE_POKEMON_VALUE){CHECK(n==2&&args[1]==0xd);return hp[mon(args[0])];}
    if(a==0x1087425c){CHECK(n==1&&args[0]<0x20);return side_of[args[0]];}
    if(a==0x1086fc8c&&second_negative&&args[1]==MON+0x300){note("1086fc8c(-) ");return 0xffff;}
    if(a==0x1081e0f8){++routed;return px_battle_kind_heal(args[0],args[1],&api);}
    if(a==0x1081cf54){++routed;return px_battle_kind_faint(args[0],args[1],&api);}
    char t[32];snprintf(t,sizeof t,"%x(",a);note(t);
    for(unsigned i=0;i<n;++i){snprintf(t,sizeof t,i?",%x":"%x",args[i]);note(t);}
    note(") ");
    for(unsigned i=0;i<nrets;++i)if(rets[i].address==a)return rets[i].value;
    return 0;
}
static void fault(void *u) { (void)u;++faults;note("FAULT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static const struct px_battle_event_api events;
static void handler(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;(void)f;(void)h;(void)c;(void)v;(void)w;
    char t[48];snprintf(t,sizeof t,"evb7[%x,%x] ",px_battle_event_get(4,&events),px_battle_event_get(3,&events));note(t);
}
static const struct px_battle_event_api events={&memory,NULL,fault,overflow,blocked,handler,STACK,BATTLE,POOL};
static const struct px_battle_process_api api={&events,call,SCRATCH};
static void reset(void)
{
    memset(mem,0,sizeof mem);log_[0]=0;nrets=0;faults=0;routed=0;memset(side_of,0,sizeof side_of);second_negative=0;
    w32(NULL,CTX,0x111);w32(NULL,CTX+4,0x222);w32(NULL,CTX+8,TABLE);w32(NULL,CTX+0xc,STATE);
    for(unsigned i=0;i<6;++i){const uint32_t p=MON+0x100*i;w32(NULL,TABLE+0x84+4*i,p);w8(NULL,p+0x19,(uint8_t)(0x10+i));w32(NULL,p,0x900+i);hp[i]=100;}
    w8(NULL,ENTRIES,0xb7);w32(NULL,ENTRIES+4,0);
    w32(NULL,HANDLER+8,ENTRIES);w32(NULL,HANDLER+0x18,0x20000000u|1u<<16);w8(NULL,HANDLER+0x10,7);w32(NULL,BATTLE+0x60,HANDLER);
}
#define OK(s) (strcmp(log_,(s))==0)
static uint8_t sizes[2*PX_BATTLE_ACTION_KINDS];
static void execute(void *u,uint32_t c,uint32_t r) { (void)u;px_battle_action_execute(c,r,&api); }
static const struct px_battle_action_api action={&memory,sizes,execute,fault};

int main(void)
{
    unsigned cases=0;
    /* Kind 0x10: masked stat values. */
    for(uint32_t mask=0;mask<64;++mask){
        reset();if(mask==63)ret(PX_BATTLE_POSITION,0xa);
        w8(NULL,REC+0xe,2);w8(NULL,REC+0xf,(uint8_t)(mask&31));for(uint32_t k=0;k<5;++k)w16(NULL,REC+4+2*k,(uint16_t)(0x1100+0x11*k));
        const uint32_t r=px_battle_kind_set_values(CTX,REC,&api);
        if(mask==63){CHECK(r==0&&OK("1086c2cc(220c,2) "));++cases;continue;}
        char want[700]="1086c2cc(220c,2) ",t[96];
        for(uint32_t k=0;k<5;++k)if(mask>>k&1){snprintf(t,sizeof t,"10871250(7200,%x,%x) 1086c22c(5555,14,2,%x,%x) ",8+k,0x1100+0x11*k,8+k,(0x1100+0x11*k)&0xff);strcat(want,t);}
        strcat(want,"1086bbe8(1000,a010) ");
        CHECK(r==1&&OK(want));++cases;
    }
    /* Kind 0x12: faint, with and without a source. */
    for(uint32_t v=0;v<32;++v){
        reset();if(v&1)ret(PX_BATTLE_POSITION,0xa);hp[3]=v&2?0:50;
        w32(NULL,REC,0x12u|(v&8?0x1fu:2u)<<8);w8(NULL,REC+4,3);w8(NULL,REC+5,(uint8_t)(v>>2&1));w16(NULL,REC+6,(uint16_t)(v&16?0x77:0));
        w16(NULL,MON+0x300+0x10,50);
        const uint32_t r=px_battle_kind_faint(CTX,REC,&api);
        if(v&1){CHECK(r==0&&OK("1086c2cc(220c,3) "));++cases;continue;}
        if((v&2)&&!(v&4)){CHECK(r==0&&OK("1086c2cc(220c,3) "));++cases;continue;}
        const int src=!(v&8)&&(v&16);
        char want[400];snprintf(want,sizeof want,"1086c2cc(220c,3) 1086bbe8(1000,a008) evb7[13,%x] 108003fc(903) 1086c22c(5555,4,13) 1086c22c(5555,4b,13,0) 108646c4(1000,7300,0,%x) ",
                                v&8?0x1f:2,src?SCRATCH:0);
        CHECK(r==1&&OK(want)&&r16(NULL,MON+0x310)==0&&r32(NULL,SCRATCH)==(src?MON+0x200:0)&&r16(NULL,SCRATCH+4)==(v&16?0x77:0));++cases;
    }
    /* Kind 0x2a: the leave checks. */
    for(uint32_t v=0;v<64;++v){
        reset();w8(NULL,REC+0x4c,1);w8(NULL,REC+0x4d,(uint8_t)(v>>5&1?2:0));
        ret(0x10865c4c,v&1);w8(NULL,MON+0x100+0x171,(uint8_t)(v&2?(v&4?0x19:5):0));w32(NULL,MON+0x100+0xa0,v&8?0x14:0x10);w8(NULL,CTX+0x18,(uint8_t)(v>>4&1));
        ret(0x10866c68,v>>5&1);ret(0x10873164,0x3c);
        const uint32_t r=px_battle_kind_leave(CTX,REC,&api);
        const int marked=(v&2)&&!(v&4);
        if(v&1){CHECK(r==0&&OK("10865c4c(1000) "));++cases;continue;}
        if(marked||(v&8)||(v&16)){CHECK(r==0&&OK("10865c4c(1000) ")&&mem[CTX+0x18]==(v>>4&1));++cases;continue;}
        char want[300];
        if(!(v&32))snprintf(want,sizeof want,"10865c4c(1000) 1086bbe8(1000,a004) 10866c68(1000,7100,0) ");
        else snprintf(want,sizeof want,"10865c4c(1000) 1086bbe8(1000,a004) 10866c68(1000,7100,1) 10873164(222,6000,1) 10810cc0(111,3c) 1086bbe8(1000,a028) ");
        CHECK(OK(want)&&r==(v>>5&1)&&mem[CTX+0x18]==(v>>5&1));++cases;
    }
    /* Kind 0x2d: heal to the maximum, the pending list, and the position refresh. */
    for(uint32_t v=0;v<32;++v){
        reset();w8(NULL,REC+4,2);w16(NULL,REC+6,(uint16_t)(v&1?80:10));w16(NULL,MON+0x200+0x10,30);w16(NULL,MON+0x200+0xe,100);
        w8(NULL,LIST+0xc4+2,(uint8_t)(v>>1&1));w8(NULL,LIST+0x10c,(uint8_t)(v&4?0x18:3));if(v&8)w8(NULL,LIST+0xf4+7,2);
        ret(0x10873164,v&16?0xa:4);ret(0x10871b10,0x33);side_of[2]=1;
        CHECK(px_battle_kind_heal(CTX,REC,&api)==1);
        CHECK(r16(NULL,MON+0x210)==(v&1?100:40));
        const int pending=v>>1&1,present=v>>3&1,full=v>>2&1;
        char want[400];snprintf(want,sizeof want,"1086c22c(5555,3,2,%x) %s1086bbe8(1000,a008) 10873164(222,6000,2) %s",v&1?80:10,pending&&!present&&full?"FAULT ":"",
                                v&16?"":"10871b10(222,4) 1086c720(1000,1,33,33) 1086c22c(5555,44,1,33,33,1) 1086c500(1000) ");
        CHECK(OK(want)&&mem[LIST+0xc4+2]==0);
        if(pending&&!present&&!full)CHECK(mem[LIST+0x10c]==4&&mem[LIST+0xf4+3]==2);else CHECK(mem[LIST+0x10c]==(full?0x18:3));
        ++cases;
    }
    reset();w8(NULL,REC+4,0x18);w32(NULL,TABLE+0x84+0x60,MON);ret(0x10873164,0xa);CHECK(px_battle_kind_heal(CTX,REC,&api)==1&&faults==2);++cases;   /* id 0x18: lookup and list fault */
    /* Heal: 16-bit wrap of the sum before the maximum. */
    reset();w8(NULL,REC+4,1);w16(NULL,REC+6,0xfff0);w16(NULL,MON+0x110,0x20);w16(NULL,MON+0x10e,0x100);ret(0x10873164,0xa);px_battle_kind_heal(CTX,REC,&api);CHECK(r16(NULL,MON+0x110)==0x10);++cases;
    /* Kind 0x33: swap two battlers of one side. */
    for(uint32_t v=0;v<32;++v){
        reset();w8(NULL,REC+4,1);w8(NULL,REC+5,(uint8_t)(v&1?1:3));side_of[1]=2;side_of[3]=v&2?1:2;hp[3]=v&4?0:5;
        rets[nrets].address=0x1086fc8c;rets[nrets++].value=v&8?0xffffu:0x10002u;   /* sxth: -1, or 2 */
        if(v&16){side_of[1]=side_of[3]=4;}
        const uint32_t r=px_battle_kind_swap(CTX,REC,&api);
        if(v&1){CHECK(r==0&&OK(""));++cases;continue;}
        if(v&2&&!(v&16)){CHECK(r==0&&OK(""));++cases;continue;}
        if(v&4){CHECK(r==0&&OK(""));++cases;continue;}
        const uint32_t s=v&16?4:2,party=TABLE+s*0x1c+4;
        char want[300];
        if(v&8)snprintf(want,sizeof want,"%s1086fc8c(%x,7100) 1086fc8c(%x,7300) ",v&16?"FAULT ":"",party,party);
        else snprintf(want,sizeof want,"%s1086fc8c(%x,7100) 1086fc8c(%x,7300) 10867e74(1000,%x,2,2,0) 1086bbe8(1000,a008) 10867d64(1000,%x,2,2) ",v&16?"FAULT ":"",party,party,s,s);
        CHECK(OK(want)&&r==!(v&8));++cases;
    }
    /* Only the second index negative. */
    reset();w8(NULL,REC+4,1);w8(NULL,REC+5,3);side_of[1]=side_of[3]=2;ret(0x1086fc8c,2);second_negative=1;
    CHECK(px_battle_kind_swap(CTX,REC,&api)==0&&OK("1086fc8c(603c,7100) 1086fc8c(-) "));++cases;
    /* End to end: a heal then a gated faint through battle_action and the processor. */
    {
        reset();w32(NULL,WORD,0);
        for(unsigned i=0;i<PX_BATTLE_ACTION_KINDS;++i){sizes[2*i]=(uint8_t)i;sizes[2*i+1]=0x2c;}
        ret(0x10873164,0xa);w16(NULL,MON+0x400+0x10,5);w16(NULL,MON+0x400+0xe,60);
        uint32_t r=px_battle_action_record(CTX,0x2d,4,&action);w8(NULL,r+4,4);w16(NULL,r+6,20);
        px_battle_action_commit(CTX,r,&action);
        r=px_battle_action_record(CTX,0x12,4,&action);w8(NULL,r+4,4);w32(NULL,r,r32(NULL,r)|0x1000000u);
        px_battle_action_commit(CTX,r,&action);
        CHECK(routed==2&&r16(NULL,MON+0x410)==0&&strstr(log_,"1086c22c(5555,3,4,14) ")&&strstr(log_,"evb7[14,4] ")&&faults==0);
        CHECK((r32(NULL,WORD)&0x70000000u)==0x70000000u&&(r32(NULL,WORD)>>10&0x1ff)==0);++cases;
    }
    printf("battle_outcomes: %u cases, all tests passed\n",cases);
    return 0;
}
