#include "battle_kinds.h"
#include "../battle_action/battle_action.h"
#include "../battle_ability/battle_ability.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n  log: %s\n",__FILE__,__LINE__,#c,log_);exit(1); } } while (0)
enum { SIZE=0x10000, CTX=0x1000, WORD=CTX+0x437c, TABLE=0x6000, MON=0x7000, FIELD=0x9800, REC=0xa000,
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

static uint32_t hp[8],ability[8];static unsigned faults,routed;
static struct { uint32_t address,value; } rets[16];static unsigned nrets;
static void ret(uint32_t a,uint32_t v) { rets[nrets].address=a;rets[nrets++].value=v; }
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static const struct px_battle_process_api api;
static uint32_t call(void *u,uint32_t a,const uint32_t *args,unsigned n)
{
    (void)u;
    if(a==PX_BATTLE_POKEMON_VALUE){CHECK(n==2&&(args[1]==0xd||args[1]==0x10));return args[1]==0xd?hp[mon(args[0])]:ability[mon(args[0])];}
    /* The processor's routines for kinds 0xd, 0x1e and 0x20 run for real. */
    if(a==0x10823014){CHECK(n==3);++routed;return px_battle_kind_stages(args[0],args[1],args[2],&api);}
    if(a==0x10824774){CHECK(n==2);++routed;return px_battle_kind_weather(args[0],args[1],&api);}
    if(a==0x10824978){CHECK(n==2);++routed;return px_battle_kind_ability(args[0],args[1],&api);}
    char t[32];snprintf(t,sizeof t,"%x(",a);note(t);
    for(unsigned i=0;i<n;++i){snprintf(t,sizeof t,i?",%x":"%x",args[i]);note(t);}
    note(") ");
    for(unsigned i=0;i<nrets;++i)if(rets[i].address==a)return rets[i].value;
    /* By default 1086b284 returns the stack word it is given. */
    if(a==0x1086b284)return r32(NULL,args[0]);
    return 0;
}
static void fault(void *u) { (void)u;++faults;note("FAULT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
/* Handlers: 0 on event 5 notes its keys and sets 0x53 when asked; 1-4 note events 0x82, 0x86, 0x87, 0x93/0x94. */
static uint32_t block5,set42;
static void handler(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,fault,overflow,blocked,handler,STACK,BATTLE,POOL};
static const struct px_battle_process_api api={&events,call,SCRATCH};
static void handler(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;(void)h;(void)c;(void)v;(void)w;char t[64];
    switch(f){
    case 0:snprintf(t,sizeof t,"ev5[%x,%x,%x,%x] ",px_battle_event_get(3,&events),px_battle_event_get(4,&events),px_battle_event_get(0x12,&events),px_battle_event_get(0x53,&events));
        note(t);if(block5)px_battle_event_rewrite(0x53,1,&events);break;
    case 1:note("ev82 ");w32(NULL,WORD,0xbad);break;
    case 2:snprintf(t,sizeof t,"ev86[%x] ",px_battle_event_get(0x3a,&events));note(t);if(set42)px_battle_event_rewrite(0x42,1,&events);break;
    case 3:note("ev87 ");break;
    case 4:snprintf(t,sizeof t,"ev93[%x,%x,%x] ",px_battle_event_get(2,&events),px_battle_event_get(0xf,&events),px_battle_event_get(0x10,&events));note(t);w32(NULL,WORD,0xbad);break;
    default:snprintf(t,sizeof t,"ev94[%x] ",px_battle_event_get(2,&events));note(t);break;
    }
}
static void reset(void)
{
    memset(mem,0,sizeof mem);log_[0]=0;nrets=0;faults=0;routed=0;block5=set42=0;
    w32(NULL,CTX+8,TABLE);w32(NULL,CTX+0xc,STATE);w32(NULL,CTX+0x1c,FIELD);
    for(unsigned i=0;i<6;++i){w32(NULL,TABLE+0x84+4*i,MON+0x100*i);w8(NULL,MON+0x100*i+0x19,(uint8_t)(0x10+i));hp[i]=100;ability[i]=0x20+i;}
    w32(NULL,WORD,0x40000000u|0x10000000u|0x1ab);
    /* Handlers on events 5, 0x82, 0x86, 0x87, 0x93, 0x94. */
    static const uint8_t ev[6]={5,0x82,0x86,0x87,0x93,0x94};
    for(unsigned i=0;i<6;++i){
        const uint32_t h=HANDLER+0x40*i,e=ENTRIES+0x10*i;
        w8(NULL,e,ev[i]);w32(NULL,e+4,i);
        w32(NULL,h+8,e);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x10,7);
        w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<5?h+0x40:0);
    }
    w32(NULL,BATTLE+0x60,HANDLER);
}
#define OK(s) (strcmp(log_,(s))==0)

/* A stat-stage record: source, stat, stages, flags, targets. */
static void stages(uint32_t source,uint32_t stat,int8_t delta,uint32_t flags,const uint8_t *ids,uint32_t n,uint32_t popup)
{
    w32(NULL,REC,0xd|source<<8|(popup?0x800000u:0));w8(NULL,REC+4,(uint8_t)stat);w32(NULL,REC+8,0xabc);
    for(uint32_t i=0;i<n;++i)w8(NULL,REC+0xc+i,ids[i]);
    w8(NULL,REC+0x16,(uint8_t)n);w8(NULL,REC+0x17,(uint8_t)delta);w8(NULL,REC+0x18,(uint8_t)flags);
}
static uint8_t sizes[2*PX_BATTLE_ACTION_KINDS];
static void execute(void *u,uint32_t c,uint32_t r) { (void)u;px_battle_action_execute(c,r,&api); }
static const struct px_battle_action_api action={&memory,sizes,execute,fault};

int main(void)
{
    unsigned cases=0;
    /* Stages: one target, over flags, event 5 blocking and 10860e74's result. */
    for(uint32_t v=0;v<64;++v){
        reset();const uint8_t ids[1]={3};const uint32_t flags=(v&1?1:0)|(v&2?2:0)|(v&4?8:0)|(v&8?0x10:0);
        block5=v>>4&1;ret(0x108722e8,1);ret(0x10860e74,v>>5&1);ret(0x1080049c,0);
        stages(1,2,-1,flags,ids,1,1);
        const uint32_t r=px_battle_kind_stages(CTX,REC,0x1ab,&api);
        char want[512],ev[64]="";
        if(!(flags&0x10))snprintf(ev,sizeof ev,"1080049c(0,d) ev5[11,13,0,0] ");
        const uint32_t blockedv=(flags&0x10)?1:(v>>4&1);
        snprintf(want,sizeof want,"1086c2cc(220c,3) 108722e8(7300,2,ffffffff) 1086c22c(5555,66,11) 1086c2cc(220c,3) %s"
                 "10860e74(1000,1,7300,2,ffffffff,1f,1ab,abc,%u,%u,%x,%u) %s1086c22c(5555,67,11) ",
                 ev,(flags>>1)&1,~flags&1,flags&8?REC+0x1c:0,blockedv,(v>>5&1)&&!(flags&8)?"1086bbe8(1000,a01c) ":"");
        CHECK(OK(want)&&r==(v>>5&1));++cases;
    }
    /* Key 0x53 starts at 1080049c's value: nonzero blocks without a handler. */
    reset();{const uint8_t ids[1]={3};ret(0x108722e8,1);ret(0x10860e74,1);ret(0x1080049c,0x99);stages(1,2,1,0,ids,1,0);
        CHECK(px_battle_kind_stages(CTX,REC,0,&api)==1&&strstr(log_,"ev5[11,13,0,99] 10860e74(1000,1,7300,2,1,1f,0,abc,0,1,0,1) "));++cases;}
    /* Source 0x1f: no popup, no event (allowed). */
    reset();{const uint8_t ids[1]={2};ret(0x108722e8,1);ret(0x10860e74,1);stages(0x1f,1,2,0,ids,1,1);
        CHECK(px_battle_kind_stages(CTX,REC,0,&api)==1&&OK("1086c2cc(220c,2) 108722e8(7200,1,2) 1086c2cc(220c,2) 10860e74(1000,1f,7200,1,2,1f,0,abc,0,1,0,0) 1086bbe8(1000,a01c) "));++cases;}
    /* Three targets: skips without a position or HP; the popup follows the first that 108722e8 accepts. */
    for(uint32_t v=0;v<16;++v){
        reset();const uint8_t ids[3]={2,3,4};hp[3]=v&1?0:1;ret(0x108722e8,v>>1&1);ret(0x10860e74,1);
        if(v&4)ret(PX_BATTLE_POSITION,0xa);
        stages(1,5,1,0x10,ids,3,v>>3&1);
        const uint32_t r=px_battle_kind_stages(CTX,REC,0,&api);
        if(v&4){CHECK(r==0&&OK("1086c2cc(220c,2) 1086c2cc(220c,3) 1086c2cc(220c,4) 1086c2cc(220c,2) 1086c2cc(220c,3) 1086c2cc(220c,4) "));++cases;continue;}
        char want[1024]="",t[160];
        /* First pass: targets 2, (3 if alive), 4 until accepted. */
        int shown=0;
        for(unsigned i=0;i<3&&!shown;++i){
            snprintf(t,sizeof t,"1086c2cc(220c,%x) ",2+i);strcat(want,t);
            if(i==1&&(v&1))continue;
            snprintf(t,sizeof t,"108722e8(%x,5,1) ",MON+0x100*(2+i));strcat(want,t);
            if(v&2){shown=1;if(v&8)strcat(want,"1086c22c(5555,66,11) ");}
        }
        for(unsigned i=0;i<3;++i){
            snprintf(t,sizeof t,"1086c2cc(220c,%x) ",2+i);strcat(want,t);
            if(i==1&&(v&1))continue;
            snprintf(t,sizeof t,"10860e74(1000,1,%x,5,1,1f,0,abc,0,1,0,1) 1086bbe8(1000,a01c) ",MON+0x100*(2+i));strcat(want,t);
        }
        if(shown&&(v&8))strcat(want,"1086c22c(5555,67,11) ");
        CHECK(OK(want)&&r==1);++cases;
    }
    /* No targets. */
    reset();stages(1,1,1,0,NULL,0,1);CHECK(px_battle_kind_stages(CTX,REC,0,&api)==0&&OK(""));++cases;

    /* Weather: setting (current, turns, field turns) and clearing. */
    for(uint32_t cur=0;cur<3;++cur)for(uint32_t turns=4;turns<=0xff;turns+=0xfb)for(uint32_t ft=4;ft<=0xff;ft+=0xfb)for(uint32_t v=0;v<4;++v){
        reset();set42=v&1;w8(NULL,FIELD+1,(uint8_t)cur);w32(NULL,FIELD+4,ft);
        w32(NULL,REC,0x1e|2u<<8|(v&2?0x800000u:0));w8(NULL,REC+4,2);w8(NULL,REC+5,(uint8_t)turns);ret(0x1086b284,0x12345678);
        const uint32_t r=px_battle_kind_weather(CTX,REC,&api);
        const int refuse=cur==2&&(turns!=0xff||ft==0xff);
        if(refuse){CHECK(r==0&&OK("")&&mem[FIELD+1]==cur);++cases;continue;}
        char want[320];
        snprintf(want,sizeof want,"%s1086c22c(5555,45,2,%x) 1086b284(537c) ev86[2] %s1086bbe8(1000,a008) %s",
                 v&2?"1086c22c(5555,66,12) ":"",turns,v&1?"":"ev87 ",v&2?"1086c22c(5555,67,12) ":"");
        CHECK(r==1&&OK(want)&&mem[FIELD+1]==2&&r32(NULL,FIELD+4)==turns&&r32(NULL,WORD)==0x12345678);++cases;
    }
    for(uint32_t w=5;w<8;++w){reset();w32(NULL,REC,0x1e);w8(NULL,REC+4,(uint8_t)w);CHECK(px_battle_kind_weather(CTX,REC,&api)==0&&OK(""));++cases;}
    for(uint32_t v=0;v<4;++v){
        reset();w8(NULL,FIELD+1,3);w32(NULL,REC,0x1e|2u<<8|(v&2?0x800000u:0));w8(NULL,REC+6,(uint8_t)(v&1));ret(0x1086b284,0x777);
        const uint32_t r=px_battle_kind_weather(CTX,REC,&api);
        if(!(v&1)){CHECK(r==0&&OK(""));++cases;continue;}
        char want[200];snprintf(want,sizeof want,"%s1086bbe8(1000,a008) 1086b284(537c) ev82 %s",v&2?"1086c22c(5555,66,12) ":"",v&2?"1086c22c(5555,67,12) ":"");
        CHECK(r==1&&OK(want)&&r32(NULL,WORD)==0x777&&mem[FIELD+1]==3);++cases;   /* clearing leaves the field to event 0x82 */
    }

    /* Ability change: fixed abilities, HP, same ability without force, and the popup. */
    for(uint32_t v=0;v<32;++v){
        reset();hp[4]=v&1?0:1;ret(0x1086fe6c,v>>1&1);ret(0x1086b284,0x4242);
        const uint32_t newab=v&4?0x24:0x1f5;   /* 0x24: ability 4 already has it */
        w32(NULL,REC,0x20|1u<<8|(v&16?0x800000u:0));w16(NULL,REC+4,(uint16_t)newab);w8(NULL,REC+6,4);w8(NULL,REC+7,(uint8_t)(v>>3&1));
        w16(NULL,MON+0x400+0x14e,0x9999);
        const uint32_t r=px_battle_kind_ability(CTX,REC,&api);
        if(v&2){CHECK(r==0&&OK("1086fe6c(24,108d10f0,2) "));++cases;continue;}
        if(v&1){CHECK(r==0&&OK("1086fe6c(24,108d10f0,2) "));++cases;continue;}
        if((v&4)&&!(v&8)){CHECK(r==0&&OK("1086fe6c(24,108d10f0,2) "));++cases;continue;}
        char want[512];
        snprintf(want,sizeof want,"1086fe6c(24,108d10f0,2) %s1086c22c(5555,53,4,%x) 1086bbe8(1000,a00c) 1086b284(537c) ev93[4,24,%x] "
                 "108679ac(7400) 1086c22c(5555,20,4,%x) 108678f8(7400) 1086c22c(5555,67,4) %s%s108677e4(1000,7400,24,%x) ",
                 v&16?"1086c22c(5555,66,1) ":"",newab,newab,newab,v&4?"":"1086b284(537c) ev94[4] ",v&16?"1086c22c(5555,67,1) ":"",newab&0xff);
        CHECK(r==1&&OK(want)&&r16(NULL,MON+0x400+0x14e)==(newab&0xff)&&r32(NULL,WORD)==0x4242);++cases;
    }

    /* End to end: records pushed and committed through battle_action and the processor. */
    {
        reset();w32(NULL,WORD,0);
        for(unsigned i=0;i<PX_BATTLE_ACTION_KINDS;++i){sizes[2*i]=(uint8_t)i;sizes[2*i+1]=0x40;}
        /* Weather from a switch-in: kind 0x1e, rain for 5 turns, popup flag. */
        uint32_t r=px_battle_action_record(CTX,0x1e,2,&action);
        w8(NULL,r+4,2);w8(NULL,r+5,5);w32(NULL,r,r32(NULL,r)|0x800000u);
        px_battle_action_commit(CTX,r,&action);
        CHECK(routed==1&&mem[FIELD+1]==2&&r32(NULL,FIELD+4)==5&&(r32(NULL,WORD)&0x70000000u)==0x70000000u);
        CHECK(strstr(log_,"1086c22c(5555,66,12) 1086c22c(5555,45,2,5) 1086b284(537c) ev86[2] ev87 1086bbe8(1000,5388) 1086c22c(5555,67,12) ")!=NULL);++cases;
        /* An Intimidate-style stage record: Attack -1 for two targets, gated on the previous success. */
        log_[0]=0;ret(0x108722e8,1);ret(0x10860e74,1);
        r=px_battle_action_record(CTX,0xd,1,&action);
        w8(NULL,r+4,1);w8(NULL,r+0xc,3);w8(NULL,r+0xd,4);w8(NULL,r+0x16,2);w8(NULL,r+0x17,0xff);w8(NULL,r+0x18,0x10);
        w32(NULL,r,r32(NULL,r)|0x1000000u);
        px_battle_action_commit(CTX,r,&action);
        CHECK(routed==2&&strstr(log_,"10860e74(1000,1,7300,1,ffffffff,1f,")&&strstr(log_,"10860e74(1000,1,7400,1,ffffffff,1f,")&&faults==0);
        CHECK((r32(NULL,WORD)>>10&0x1ff)==0);++cases;
    }
    printf("battle_kinds: %u cases, all tests passed\n",cases);
    return 0;
}
