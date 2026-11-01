#include "battle_restores.h"
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

static uint32_t hp[8],maxhp[8],ab10[8],ab11[8];static unsigned faults,routed;static uint32_t event_word;
static struct { uint32_t address,value; } rets[16];static unsigned nrets;
static void ret(uint32_t a,uint32_t v) { rets[nrets].address=a;rets[nrets++].value=v; }
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static const struct px_battle_process_api api;
static uint32_t call(void *u,uint32_t a,const uint32_t *args,unsigned n)
{
    (void)u;
    if(a==PX_BATTLE_POKEMON_VALUE){CHECK(n==2);const unsigned m=mon(args[0]);
        switch(args[1]){case 0xd:return hp[m];case 0xe:return maxhp[m];case 0x10:return ab10[m];case 0x11:return ab11[m];}CHECK(0);}
    if(a==0x10821948){++routed;return px_battle_kind_restore(args[0],args[1],args[2],&api);}
    if(a==0x10821b80){++routed;return px_battle_kind_pp(args[0],args[1],args[2],&api);}
    char t[32];snprintf(t,sizeof t,"%x(",a);note(t);
    for(unsigned i=0;i<n;++i){snprintf(t,sizeof t,i?",%x":"%x",args[i]);note(t);}
    note(") ");
    for(unsigned i=0;i<nrets;++i)if(rets[i].address==a)return rets[i].value;
    if(a==0x1086b284)return r32(NULL,args[0]);
    return 0;
}
static void fault(void *u) { (void)u;++faults;note("FAULT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static unsigned blocked_calls;
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;++blocked_calls;return 0; }
static const struct px_battle_event_api events;
static void handler(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;(void)h;(void)c;(void)v;(void)w;char t[48];
    if(f==0){snprintf(t,sizeof t,"ev7c[%x,%x] ",px_battle_event_get(2,&events),px_battle_event_get(3,&events));note(t);w32(NULL,WORD,event_word);}
    else{snprintf(t,sizeof t,"ev94[%x] ",px_battle_event_get(2,&events));note(t);w32(NULL,WORD,0xbad);}
}
static const struct px_battle_event_api events={&memory,NULL,fault,overflow,blocked,handler,STACK,BATTLE,POOL};
static const struct px_battle_process_api api={&events,call,SCRATCH};
static void reset(void)
{
    memset(mem,0,sizeof mem);log_[0]=0;nrets=0;faults=0;routed=0;blocked_calls=0;event_word=0;
    w32(NULL,CTX+8,TABLE);w32(NULL,CTX+0xc,STATE);
    for(unsigned i=0;i<6;++i){const uint32_t p=MON+0x100*i;w32(NULL,TABLE+0x84+4*i,p);w8(NULL,p+0x19,(uint8_t)(0x10+i));w32(NULL,p,0x900+i);
        hp[i]=50;maxhp[i]=100;ab10[i]=0x20+i;ab11[i]=0x20+i;}
    static const uint8_t ev[2]={0x7c,0x94};
    for(unsigned i=0;i<2;++i){
        const uint32_t h=HANDLER+0x40*i,e=ENTRIES+0x10*i;
        w8(NULL,e,ev[i]);w32(NULL,e+4,i);
        w32(NULL,h+8,e);w32(NULL,h+0x18,0x20000000u|1u<<16|(i==0?0x4000000u:0));w8(NULL,h+0x10,7);   /* event 0x7c needs flag 0x4000000 */
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
    /* Kind 4. */
    for(uint32_t v=0;v<512;++v){
        reset();ret(0x108007cc,v&1);hp[3]=v&2?0:(v&4?100:40);w32(NULL,MON+0x358,v&8?0x24:0x20);
        if(v&16)ret(PX_BATTLE_POSITION,0xa);
        w32(NULL,REC,4u|1u<<8|(v&32?0x800000u:0));w16(NULL,REC+4,0x30);w8(NULL,REC+6,3);w8(NULL,REC+7,(uint8_t)(v>>6&1));w16(NULL,REC+0xa,(uint16_t)(v&128?0x8002:0x8000));
        const uint32_t low=v&256?0x1ab:0;
        const uint32_t r=px_battle_kind_restore(CTX,REC,low,&api);
        char want[600]="",t[200];
        if((v&1)){CHECK(OK("108007cc(903,2) ")&&r==0);++cases;continue;}
        if((v&2)||(v&4)){CHECK(OK("108007cc(903,2) ")&&r==0);++cases;continue;}
        strcat(want,"108007cc(903,2) ");
        if(v&32)strcat(want,"1086c22c(5555,66,11) ");
        const int force=v>>6&1,blockedh=!force&&(v&8);
        if(blockedh){strcat(want,"1086b5e0(5555,6a,37a,13,ffff0000) ");}
        else{
            strcat(want,"1086c2cc(220c,13) ");
            if(!(v&16))strcat(want,"1086c22c(5555,57,0,29e) ");
            strcat(want,"10865788(1000,7300,30,1) ");
            if(v&128)strcat(want,"1086bbe8(1000,a008) ");
            else if(low){snprintf(t,sizeof t,"1086b5e0(5555,6a,392,3,%x,ffff0000) ",low);strcat(want,t);}
        }
        if(v&32)strcat(want,"1086c22c(5555,67,11) ");
        CHECK(OK(want)&&r==(uint32_t)!blockedh&&mem[CTX+0x1093]==(blockedh?0x20:0));++cases;
    }
    /* Kind 8: both PP pairs, caps, links and the fainted override. */
    for(uint32_t v=0;v<256;++v){
        reset();ret(0x108007cc,v&1);hp[2]=v&2?0:9;
        const uint32_t e=MON+0x200+0x116+2*0xe;
        w16(NULL,e,(uint16_t)(v&4?0:0x55));w16(NULL,e+6,(uint16_t)(v&4?0:0x66));w8(NULL,e+2,(uint8_t)(v&8?20:10));w8(NULL,e+3,20);w8(NULL,e+8,(uint8_t)(v&8?20:4));w8(NULL,e+9,20);w8(NULL,e+0xc,(uint8_t)(v>>4&1));
        w8(NULL,REC+4,(uint8_t)(v&32?15:5));w8(NULL,REC+5,2);w8(NULL,REC+6,2);w8(NULL,REC+7,(uint8_t)((v>>6&1)|(v>>7&1)<<1));
        const uint32_t first=!(v>>6&1),r=px_battle_kind_pp(CTX,REC,0,&api);
        const int dead=(v&1)||(v&2),override=v>>7&1;
        if(dead&&!override){CHECK(r==0);++cases;continue;}
        const int full=!(v&4)&&(v&8);
        if(full){CHECK(r==0);++cases;continue;}
        const uint32_t base=first?10:4,amount=v&32?15:5;uint32_t now=(v&8)?20:base;now+=amount;if(now>20)now=20;
        CHECK(r==1&&mem[e+(first?2:8)]==now);
        if(v>>4&1)CHECK(mem[e+(first?8:2)]==now);else CHECK(mem[e+(first?8:2)]==(uint8_t)((v&8)?20:(first?4:10)));
        char t[160];snprintf(t,sizeof t,"1086c22c(5555,%x,12,2,%x) 1086bbe8(1000,a008) ",first?9:8,amount);CHECK(strstr(log_,t)!=NULL);++cases;
    }
    /* Only the second pair full: refused for the second pair, applied for the first. */
    for(uint32_t second=0;second<2;++second){
        reset();const uint32_t e=MON+0x200+0x116;w16(NULL,e+6,0x66);w8(NULL,e+2,3);w8(NULL,e+3,9);w8(NULL,e+8,7);w8(NULL,e+9,7);
        w8(NULL,REC+4,1);w8(NULL,REC+5,2);w8(NULL,REC+7,(uint8_t)second);
        CHECK(px_battle_kind_pp(CTX,REC,0,&api)==!second);++cases;
    }
    reset();w8(NULL,REC+5,2);w8(NULL,REC+6,4);CHECK(px_battle_kind_pp(CTX,REC,0,&api)==1&&faults==1);++cases;   /* slot 4 faults */
    reset();w8(NULL,REC+5,2);w8(NULL,REC+4,250);w8(NULL,MON+0x200+0x116+2,10);w8(NULL,MON+0x200+0x116+3,0xff);px_battle_kind_pp(CTX,REC,0,&api);CHECK(mem[MON+0x200+0x118]==4);++cases;   /* 8-bit wrap */
    /* Kind 0x23: the held item through event 0x7c. */
    for(uint32_t v=0;v<32;++v){
        reset();hp[1]=v&1?0:5;ret(0x108abf28,v&2?0x777:0);ret(0x10863e4c,0x444);ret(0x108007a4,v>>2&1?0x31:0);
        event_word=v&8?0x60000000u:0x40000000u;w32(NULL,WORD,0x12345678);
        w32(NULL,REC,0x23u|3u<<8);w8(NULL,REC+4,1);w8(NULL,REC+5,(uint8_t)(v>>4&1));w16(NULL,REC+6,0x99);w8(NULL,MON+0x100+0x16c,0x80);
        w16(NULL,HANDLER+0x40+0x18,5);
        const uint32_t r=px_battle_kind_item(CTX,REC,&api);
        if(v&1){CHECK(r==0&&OK(""));++cases;continue;}
        if(!(v&2)){CHECK(r==0&&OK("108abf28(7100,99) "));++cases;continue;}
        char want[500]="108abf28(7100,99) 10863e4c(5555,49,6e,0) 1086b284(537c) ev7c[11,3] ",t[120];
        if(v&8){snprintf(t,sizeof t,"108007a4(99) 108659b8(5555,444,49,1,%x) ",v>>2&1?0x31:0);strcat(want,t);}
        strcat(want,"108a5bd4(777) ");
        if(v>>4&1){strcat(want,"108007a4(99) ");if(v>>2&1)strcat(want,"1086c22c(5555,1f,11,0) ");}
        if(v&8)strcat(want,"1086b098(1000,7100,99,1) ");
        CHECK(OK(want)&&r==1&&r32(NULL,WORD)==0x12345678&&blocked_calls==0&&r16(NULL,HANDLER+0x40+0x18)==0);
        CHECK(mem[MON+0x100+0x16c]==((v>>4&1)&&(v>>2&1)?0x81:0x80));++cases;
    }
    /* Kind 0x34: transform. */
    for(uint32_t v=0;v<64;++v){
        reset();w8(NULL,MON+0x11b,(uint8_t)(v&1?0x40:0));w8(NULL,MON+0x41b,(uint8_t)(v&2?0x40:0));ret(0x10874348,v&4?0x13:0x12);ret(0x1082a720,v>>3&1);
        if(v&16)ab11[1]=0x77;
        w32(NULL,REC,0x34u|1u<<8|(v&32?0x800000u:0));w8(NULL,REC+4,4);w32(NULL,WORD,0x4321);
        const uint32_t r=px_battle_kind_transform(CTX,REC,&api);
        if((v&1)||(v&2)){CHECK(r==0&&OK(""));++cases;continue;}
        if(v&4){CHECK(r==0&&OK("10874348(7100) "));++cases;continue;}
        if(!(v&8)){CHECK(r==0&&OK("10874348(7100) 1082a720(7100,7400) "));++cases;continue;}
        char want[500]="10874348(7100) 1082a720(7100,7400) ";
        if(v&32)strcat(want,"1086c22c(5555,66,11) ");
        strcat(want,"108ae5e0(7100) 108679ac(7100) 108678f8(7100) 1086c22c(5555,5f,11,14) 1086bbe8(1000,a008) ");
        if(v&32)strcat(want,"1086c22c(5555,67,11) ");
        if(v&16)strcat(want,"1086b284(537c) ev94[11] 108677e4(1000,7100,21,77) ");
        CHECK(OK(want)&&r==1&&r32(NULL,WORD)==0x4321);++cases;
    }
    /* End to end: kinds 4 and 8 (both with the low bits) through battle_action and the processor. */
    {
        reset();w32(NULL,WORD,0);
        for(unsigned i=0;i<PX_BATTLE_ACTION_KINDS;++i){sizes[2*i]=(uint8_t)i;sizes[2*i+1]=0x2c;}
        uint32_t r=px_battle_action_record(CTX,4,2,&action);w16(NULL,r+4,0x10);w8(NULL,r+6,2);w8(NULL,r+7,1);
        px_battle_action_commit(CTX,r,&action);
        r=px_battle_action_record(CTX,8,2,&action);w8(NULL,r+4,3);w8(NULL,r+5,2);w8(NULL,r+6,0);w32(NULL,r,r32(NULL,r)|0x1000000u);
        w8(NULL,MON+0x200+0x116+3,9);
        px_battle_action_commit(CTX,r,&action);
        CHECK(routed==2&&strstr(log_,"10865788(1000,7200,10,1) ")&&strstr(log_,"1086c22c(5555,9,12,0,3) ")&&mem[MON+0x200+0x118]==3&&faults==0);
        CHECK((r32(NULL,WORD)&0x70000000u)==0x70000000u&&(r32(NULL,WORD)>>10&0x1ff)==0);++cases;
    }
    printf("battle_restores: %u cases, all tests passed\n",cases);
    return 0;
}
