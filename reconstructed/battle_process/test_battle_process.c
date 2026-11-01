#include "battle_process.h"
#include "../battle_action/battle_action.h"
#include "../battle_ability/battle_ability.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n  log: %s\n",__FILE__,__LINE__,#c,log_);exit(1); } } while (0)
enum { SIZE=0x10000, CTX=0x1000, WORD=CTX+0x437c, TABLE=0x6000, MON=0x7000, SIDE=0x9000, FIELD=0x9800, REC=0xa000,
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

/* Battlers 0..5 at MON + 0x100 i; values 0xd (HP) and 0xe (max HP). */
static uint32_t hp[8],maxhp[8];
/* Return values by callee; 0 unless set. */
static struct { uint32_t address,value; } rets[16];static unsigned nrets;static unsigned faults;
static void ret(uint32_t a,uint32_t v) { rets[nrets].address=a;rets[nrets++].value=v; }
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static uint32_t call(void *u,uint32_t a,const uint32_t *args,unsigned n)
{
    (void)u;
    if(a==PX_BATTLE_POKEMON_VALUE){CHECK(n==2&&(args[1]==0xd||args[1]==0xe));if(args[0]<MON||args[0]>=MON+0x800)return 0;return args[1]==0xd?hp[mon(args[0])]:maxhp[mon(args[0])];}
    char t[32];snprintf(t,sizeof t,"%x(",a);note(t);
    for(unsigned i=0;i<n;++i){snprintf(t,sizeof t,i?",%x":"%x",args[i]);note(t);}
    note(") ");
    for(unsigned i=0;i<nrets;++i)if(rets[i].address==a)return rets[i].value;
    return 0;
}
static void fault(void *u) { (void)u;++faults;note("FAULT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
/* Event handlers: 0 rewrites key 0x53 to 1, 1 rewrites key 0x1f to 0x77, 2 notes event 0x9c. */
static void handler(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,fault,overflow,blocked,handler,STACK,BATTLE,POOL};
static void handler(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;(void)h;(void)c;(void)v;(void)w;char t[48];
    if(f==0){snprintf(t,sizeof t,"ev57[%x,%x] ",px_battle_event_get(0x02,&events),px_battle_event_get(0x18,&events));note(t);px_battle_event_rewrite(0x53,1,&events);}
    else if(f==1){snprintf(t,sizeof t,"evab[%x,%x,%x,%x] ",px_battle_event_get(0x02,&events),px_battle_event_get(0x55,&events),
                           px_battle_event_get(0x56,&events),px_battle_event_get(0x1f,&events));note(t);px_battle_event_rewrite(0x1f,0x77,&events);}
    else{snprintf(t,sizeof t,"ev9c[%x,%x] ",px_battle_event_get(0x02,&events),px_battle_event_get(0x2f,&events));note(t);w32(NULL,WORD,0xdead);}
}
static const struct px_battle_process_api api={&events,call,SCRATCH};

static void reset(void)
{
    memset(mem,0,sizeof mem);log_[0]=0;nrets=0;faults=0;
    w32(NULL,CTX+8,TABLE);w32(NULL,CTX+0xc,STATE);w32(NULL,CTX+0x1c,SIDE);w32(NULL,CTX+0xdb4,FIELD);
    for(unsigned i=0;i<6;++i){w32(NULL,TABLE+0x84+4*i,MON+0x100*i);w8(NULL,MON+0x100*i+0x19,(uint8_t)(0x10+i));hp[i]=maxhp[i]=100;}
    w32(NULL,WORD,0x40000000u|0x1234);   /* a record ran, the last failed; low bits 0x234 */
}
/* Handlers on events 0x57, 0xab and 0x9c. */
static void handlers(void)
{
    static const uint8_t ev[3]={0x57,0xab,0x9c};
    for(unsigned i=0;i<3;++i){
        const uint32_t h=HANDLER+0x40*i,e=ENTRIES+0x10*i;
        w8(NULL,e,ev[i]);w32(NULL,e+4,i);
        w32(NULL,h+8,e);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x10,7);
        w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<2?h+0x40:0);
    }
    w32(NULL,BATTLE+0x60,HANDLER);
}
static void record(uint32_t kind,uint32_t id,uint32_t flags) { w32(NULL,REC,kind|id<<8|flags); }
static uint32_t word_after(void) { return r32(NULL,WORD); }
#define RUN() (log_[0]=0,px_battle_action_execute(CTX,REC,&api))
#define OK(s) (strcmp(log_,(s))==0)
static int succeeded(void) { return (word_after()&0x70000000u)==0x70000000u; }
static int failed(void) { return (word_after()&0x50000000u)==0x40000000u; }

/* battle_action wiring: commit executes through the processor. */
static void execute(void *u,uint32_t c,uint32_t r) { (void)u;px_battle_action_execute(c,r,&api); }
static uint8_t sizes[2*PX_BATTLE_ACTION_KINDS];
static const struct px_battle_action_api action={&memory,sizes,execute,fault};

int main(void)
{
    unsigned cases=0;
    /* Gating: bit 24 runs only after a success or before any record. */
    for(uint32_t w=0;w<4;++w)for(int gate=0;gate<2;++gate){
        reset();const uint32_t start=(w&1?0x40000000u:0)|(w&2?0x10000000u:0)|0x2a5;w32(NULL,WORD,start);
        record(0x39,0,gate?0x1000000u:0);RUN();
        const int skip=gate&&(w&1)&&!(w&2);
        if(skip)CHECK(OK("")&&word_after()==start);else CHECK(OK("1086c22c(5555,62,0) ")&&word_after()==(start|0x70000000u));
        ++cases;
    }
    /* Bit 25: the header battler must be alive. */
    for(int alive=0;alive<2;++alive){reset();hp[3]=(uint32_t)alive;record(0x39,3,0x2000000u);RUN();CHECK(alive?succeeded():(OK("")&&word_after()==0x40001234u));++cases;}
    reset();record(0x39,0x18,0x2000000u);RUN();CHECK(faults==2&&word_after()==0x40001234u);++cases;   /* id 0x18 faults, then its empty slot */
    reset();w32(NULL,TABLE+0x84+4*2,0);record(0x39,2,0x2000000u);RUN();CHECK(faults==1&&OK("FAULT ")&&word_after()==0x40001234u);++cases;   /* empty slot: one fault, value 0 */
    /* Routine kinds: context, record and, for four kinds, the low 10 bits; the result decides. */
    for(uint32_t k=0;k<PX_BATTLE_PROCESS_KINDS;++k){
        const uint32_t a=px_battle_process_routine(k);if(!a)continue;
        for(uint32_t r=0;r<2;++r){
            reset();ret(a,r?0x80000000u:0);record(k,1,0);RUN();
            char want[64];const int low=k==4||k==8||k==0xa||k==0xd;
            if(low)snprintf(want,sizeof want,"%x(1000,a000,234) ",a);else snprintf(want,sizeof want,"%x(1000,a000) ",a);
            CHECK(OK(want)&&(r?succeeded():failed()));++cases;
        }
    }
    {unsigned n=0;for(uint32_t k=0;k<0x50;++k)n+=px_battle_process_routine(k)!=0;CHECK(n==27);++cases;}
    /* Faulting kinds keep the previous result. */
    for(uint32_t k=0x3c;k<0x45;++k){if(k==0x3c||k==0x3e)continue;
        for(int prev=0;prev<2;++prev){reset();w32(NULL,WORD,0x40000000u|(prev?0x10000000u:0));record(k,0,0);RUN();CHECK(faults==1&&(prev?succeeded():failed()));++cases;}}
    reset();record(0xc,0,0);RUN();CHECK(faults==1&&failed());++cases;
    /* Kind 0: faint. */
    for(uint32_t v=0;v<16;++v){
        /* v: bit 0 alive, bit 1 at full HP, bit 2 word +4 bit 1, bit 3 1086c06c's result; word +4 bit 0 with bits 1 and 2. */
        reset();hp[2]=v&1?50:0;maxhp[2]=v&2?50:100;ret(0x1086c06c,v>>3&1);record(0,2,0);
        const uint32_t word4=(v&4?2u:0)|((v&2)&&(v&4)?1u:0);w32(NULL,REC+4,word4);
        RUN();
        const int alive=hp[2]!=0,forced=(word4&2)!=0,full=(word4&1)&&hp[2]==maxhp[2];
        const int calls=(alive||forced)&&!full;
        CHECK((strstr(log_,"1086c06c(1000,7200)")!=NULL)==calls);
        CHECK(calls&&(v>>3&1)?succeeded():failed());++cases;
    }
    /* Kinds 1 and 2: the popup commands; kind 1 skips a battler without a position. */
    reset();record(1,4,0);RUN();CHECK(OK("1086c2cc(220c,4) 1086c22c(5555,66,14) ")&&succeeded());++cases;
    reset();ret(PX_BATTLE_POSITION,0xa);record(1,4,0);RUN();CHECK(OK("1086c2cc(220c,4) ")&&failed());++cases;
    reset();ret(PX_BATTLE_POSITION,0xa);record(2,4,0);RUN();CHECK(OK("1086c22c(5555,67,14) ")&&succeeded());++cases;
    /* Kind 3: a message, the popup around it with bit 23, flag 0x10 from message bit 15. */
    for(uint32_t v=0;v<8;++v){
        reset();const uint32_t id=v&4?0x1f:5;record(3,id,v&1?0x800000u:0);w16(NULL,REC+6,v&2?0x8000:0x4000);w8(NULL,CTX+0x1093,0x81);RUN();
        char want[128];
        if((v&1)&&id!=0x1f)snprintf(want,sizeof want,"1086c22c(5555,66,15) 1086bbe8(1000,a004) 1086c22c(5555,67,15) ");
        else snprintf(want,sizeof want,"1086bbe8(1000,a004) ");
        CHECK(OK(want)&&succeeded()&&mem[CTX+0x1093]==(v&2?0x91:0x81));++cases;
    }
    /* Kind 9: 1086bf44, its message and event 0x57; key 0x53 faints. */
    for(uint32_t v=0;v<16;++v){
        reset();if(v&8)handlers();hp[3]=v&1?1:0;ret(0x1086bf44,v>>1&1);record(9,0,0);w8(NULL,REC+4,0x22);w8(NULL,REC+5,3);w8(NULL,REC+6,0x44);w8(NULL,REC+7,v&4?2:0);
        RUN();
        if(!(v&1)&&!(v&4)){CHECK(OK("")&&failed());++cases;continue;}
        if(!(v&2)){CHECK(OK("1086bf44(1000,7300,44,22) ")&&succeeded());++cases;continue;}
        CHECK(OK(v&8?"1086bf44(1000,7300,44,22) 1086bbe8(1000,a008) ev57[13,44] 1086c06c(1000,7300) "
                   :"1086bf44(1000,7300,44,22) 1086bbe8(1000,a008) ")&&succeeded());++cases;
    }
    /* Kind 0xf: reset stages of the live listed battlers. */
    for(uint32_t live=0;live<8;++live){
        reset();for(unsigned i=0;i<3;++i){hp[i+1]=live>>i&1;memset(mem+MON+0x100*(i+1)+0x104,1,7);}
        record(0xf,0,0);w8(NULL,REC+4,3);w8(NULL,REC+5,1);w8(NULL,REC+6,2);w8(NULL,REC+7,3);RUN();
        char want[128]="";
        for(unsigned i=0;i<3;++i){if(live>>i&1){char t[32];snprintf(t,sizeof t,"1086c22c(5555,e,%x) ",i+1);strcat(want,t);}
            for(unsigned s=0;s<8;++s)CHECK(mem[MON+0x100*(i+1)+0x104+s]==(s==7?0:(live>>i&1?6:1)));}
        CHECK(OK(want)&&(live?succeeded():failed()));++cases;
    }
    /* Kind 0x14: type change. */
    for(uint32_t v=0;v<16;++v){
        reset();ret(PX_BATTLE_POSITION,v&1?0xa:0);hp[2]=v&2?0:1;ret(0x1086f960,v>>2&1);
        w8(NULL,MON+0x200+0x100,v&8?7:1);w8(NULL,MON+0x200+0x101,2);w8(NULL,MON+0x200+0x102,0x33);
        record(0x14,0,0);w8(NULL,REC+4,7);w8(NULL,REC+5,2);RUN();
        if(v&1){CHECK(OK("1086c2cc(220c,2) ")&&failed());++cases;continue;}
        if(v&2){CHECK(OK("1086c2cc(220c,2) ")&&failed());++cases;continue;}
        if(v&4){CHECK(OK("1086c2cc(220c,2) 1086f960(7200,7) ")&&failed());++cases;continue;}
        CHECK(OK("1086c2cc(220c,2) 1086f960(7200,7) 1086c22c(5555,18,2,7) 1086b5e0(5555,6a,386,2,7,ffff0000) ")&&succeeded());
        CHECK(mem[MON+0x200+0x102]==(v&8?0x33:7));++cases;
    }
    /* The new type equal to the second type: no third type. */
    reset();w8(NULL,MON+0x200+0x100,1);w8(NULL,MON+0x200+0x101,7);w8(NULL,MON+0x200+0x102,0x33);record(0x14,0,0);w8(NULL,REC+4,7);w8(NULL,REC+5,2);RUN();
    CHECK(mem[MON+0x200+0x102]==0x33&&succeeded());++cases;
    /* Kinds 0x15-0x18: flag bits. */
    for(uint32_t k=0x15;k<=0x18;++k)for(uint32_t bit=0;bit<24;bit+=5)for(int alive=0;alive<2;++alive){
        reset();hp[4]=(uint32_t)alive;const uint32_t base=k<0x17?0x166:0x169;memset(mem+MON+0x400+base,0x5a,3);
        record(k,0,0);w8(NULL,REC+4,(uint8_t)bit);w8(NULL,REC+5,4);RUN();
        if(!alive){CHECK(OK("")&&failed());++cases;continue;}
        const uint8_t want=(k&1)?(uint8_t)(0x5a|1<<(bit&7)):(uint8_t)(0x5a&~(1<<(bit&7)));
        for(unsigned i=0;i<3;++i)CHECK(mem[MON+0x400+base+i]==(i==bit>>3?want:0x5a));
        CHECK(k<0x17?OK(""):OK(k==0x17?"1086c22c(5555,1b,14) ":"1086c22c(5555,1c,14) "));
        CHECK(succeeded());++cases;
    }
    /* Kind 0x19: event 0xab, then 108acd0c with key 0x1f. */
    for(uint32_t v=0;v<4;++v){
        reset();if(v&1)handlers();ret(0x108acd0c,v>>1&1);record(0x19,5,0);w8(NULL,REC+4,0x21);w32(NULL,REC+8,0x300);w8(NULL,REC+0xc,0x31);RUN();
        char want[160];snprintf(want,sizeof want,"%s108acd0c(31,21,%x) %s",v&1?"evab[5,31,21,300] ":"",v&1?0x77:0x300,v&2?"1086bbe8(1000,a010) ":"");
        CHECK(OK(want)&&(v&2?succeeded():failed()));++cases;
    }
    /* Kind 0x1b: flags from the bit bytes. */
    for(uint32_t v=0;v<6;++v){
        reset();record(0x1b,0,0);w8(NULL,REC+4,(uint8_t)(v<3?4:2));w8(NULL,REC+5,0x81);w8(NULL,REC+6,0x02);w8(NULL,REC+7,0x01);w8(NULL,REC+8,0x66);w8(NULL,REC+9,v&1);
        if(v&2)ret(v&1?0x108ace4c:0x108ace70,1);
        RUN();
        const char *f=v&1?"108ace4c":"108ace70";char want[160];
        if(v<3)snprintf(want,sizeof want,"%s(66,0) %s(66,7) %s(66,9) %s(66,10) ",f,f,f,f);else snprintf(want,sizeof want,"%s(66,0) %s(66,7) ",f,f);
        CHECK(OK(want)&&(v&2?succeeded():failed()));++cases;
    }
    /* Kind 0x1d: release a side entry. */
    for(uint32_t v=0;v<4;++v){
        reset();record(0x1d,0,0);w8(NULL,REC+4,3);w32(NULL,SIDE+0x274+12,v&1);w32(NULL,SIDE+0xc+12,v&2?0x777:0);RUN();
        if(!(v&1)){CHECK(OK("")&&failed());++cases;continue;}
        CHECK(OK(v&2?"1084d1c8(777,3) 1086fd30(9000,3) 1086b2b4(1000,3) ":"1086fd30(9000,3) 1086b2b4(1000,3) ")&&r32(NULL,SIDE+0x18)==0&&succeeded());++cases;
    }
    reset();record(0x1d,0,0);w8(NULL,REC+4,0xb);RUN();CHECK(faults==2&&OK("FAULT FAULT ")&&failed());++cases;
    /* Kind 0x1f: 108a5aac, command 0x34. */
    for(uint32_t r=0;r<2;++r){reset();ret(0x108a5aac,r);record(0x1f,6,0);w8(NULL,REC+4,1);w8(NULL,REC+5,2);w8(NULL,REC+0x18,9);RUN();
        CHECK(OK(r?"108a5aac(1,2,6,a008,9) 1086c22c(5555,34,1,2) ":"108a5aac(1,2,6,a008,9) ")&&(r?succeeded():failed()));++cases;}
    /* Kind 0x22: event 0x9c for a holder; the stack word comes back from 1086b284. */
    for(uint32_t v=0;v<2;++v){
        reset();handlers();w16(NULL,MON+0x100+0x12,(uint16_t)(v?0x99:0));ret(0x1086b284,0x40000000u|0x10000000u);
        record(0x22,0,0);w8(NULL,REC+4,1);w8(NULL,REC+5,0x2a);RUN();
        if(!v)CHECK(OK("")&&succeeded());
        else CHECK(OK("1086b284(537c) ev9c[11,2a] ")&&word_after()==0x70000000u);
        ++cases;
    }
    /* Kind 0x24: use an item. */
    for(uint32_t v=0;v<4;++v){
        reset();w16(NULL,MON+0x200+0x12,0x123);ret(0x108007a4,0x55);record(0x24,2,0);w8(NULL,REC+4,v&1);w8(NULL,REC+5,v>>1&1);RUN();
        char want[200];snprintf(want,sizeof want,"%s1086bbe8(1000,a008) 1086b140(1000,7200,0,1) 1086b098(1000,7200,123,%u) ",
                                v&1?"":"108007a4(123) 1086c22c(5555,49,12,55) ",v>>1&1?0u:1u);
        CHECK(OK(want)&&succeeded());++cases;
    }
    /* Kind 0x26: move slot. */
    for(uint32_t v=0;v<2;++v){reset();record(0x26,0,0);w8(NULL,REC+4,1);w8(NULL,REC+5,2);w8(NULL,REC+6,3);w8(NULL,REC+7,(uint8_t)(v*5));w16(NULL,REC+8,0x1234);RUN();
        char want[160];snprintf(want,sizeof want,"1086c22c(5555,22,1,2,3,%x,1234) 1082b064(7100,7232,1234,3,%u) ",v*5,v);CHECK(OK(want)&&succeeded());++cases;}
    /* Kind 0x27: one of five bytes. */
    for(uint32_t i=0;i<6;++i){reset();record(0x27,0,0);w8(NULL,REC+4,1);w8(NULL,REC+5,(uint8_t)i);w8(NULL,REC+6,0x5e);RUN();
        char want[96];snprintf(want,sizeof want,"1086c22c(5555,29,11,%x,5e) %s",i,i<5?"":"FAULT ");CHECK(OK(want)&&mem[MON+0x100+0x16d+i]==0x5e&&succeeded());++cases;}
    /* Kind 0x29. */
    for(uint32_t r=0;r<2;++r){reset();ret(0x1086af2c,r);record(0x29,3,0);w8(NULL,CTX+0x18,1);RUN();CHECK(OK("1086af2c(1000,7300,1) ")&&mem[CTX+0x18]==(r?5:1)&&(r?succeeded():failed()));++cases;}
    /* Kind 0x2c. */
    for(uint32_t r=0;r<2;++r){reset();ret(0x1086add4,r);record(0x2c,0,0);w8(NULL,REC+4,4);w8(NULL,REC+5,8);RUN();CHECK(OK("1086add4(1000,7400,8) ")&&(r?succeeded():failed()));++cases;}
    /* Kind 0x2e: counter, 0 stored as 1. */
    for(uint32_t v=0;v<2;++v){reset();record(0x2e,0,0);w8(NULL,REC+4,5);w16(NULL,REC+6,(uint16_t)(v*0x300));RUN();
        char want[96];snprintf(want,sizeof want,"1086c22c(5555,15,5,%x) 1086bbe8(1000,a008) ",v*0x300);CHECK(OK(want)&&r16(NULL,MON+0x500+0x150)==(v?0x300:1)&&succeeded());++cases;}
    /* Kind 0x2f. */
    for(uint32_t f=0;f<4;++f)for(uint32_t r=0;r<2;++r){reset();ret(0x1086a8f8,r);record(0x2f,1,0);w16(NULL,REC+4,0x4321);w8(NULL,REC+6,2);w8(NULL,REC+7,(uint8_t)((f&1?2:0)|(f&2?0x40:0)));RUN();
        char want[96];snprintf(want,sizeof want,"1086a8f8(1000,7100,7200,%u,c800,4321,%u,a008) ",f&1,f>>1);CHECK(OK(want)&&(r?succeeded():failed()));++cases;}
    /* Kind 0x30: the first matching entry. */
    for(uint32_t v=0;v<8;++v){
        reset();w8(NULL,CTX+0x108c,3);
        for(unsigned i=0;i<3;++i){const uint32_t e=CTX+0x10bc+0x10*i;w32(NULL,e,MON+0x100*(i==2?3:(i==0&&(v&1)?3:1)));w32(NULL,e+4,i==0&&(v&2)?0x18:1);}
        w8(NULL,CTX+0x10bc+0xd,(uint8_t)(v>>2&1));ret(0x1086a7fc,0xffffffffu);
        record(0x30,0,0);w8(NULL,REC+4,0x13);RUN();
        /* Entry 0 matches when v&1 and its low nibble is not 8; otherwise entry 2. */
        if((v&1)&&!(v&2)){CHECK(v&4?OK("")&&failed():OK("1086a7fc(1000,20bc,0) ")&&failed());}
        else CHECK(OK("1086a7fc(1000,20dc,0) ")&&failed());
        ++cases;
    }
    reset();w8(NULL,CTX+0x108c,1);w32(NULL,CTX+0x10bc,MON+0x300);record(0x30,0,0);w8(NULL,REC+4,0x13);RUN();CHECK(OK("1086a7fc(1000,20bc,0) 1086bbe8(1000,a008) ")&&succeeded());++cases;
    reset();record(0x30,0,0);RUN();CHECK(OK("")&&failed());++cases;
    reset();w8(NULL,CTX+0x108c,1);w32(NULL,CTX+0x10bc,MON+0x300);record(0x30,0,0);w8(NULL,REC+4,0x14);RUN();CHECK(OK("")&&failed());++cases;
    /* Kinds 0x31, 0x32. */
    for(uint32_t r=0;r<2;++r){reset();ret(0x108237fc,r);record(0x31,0,0);w16(NULL,REC+4,0x1ab);RUN();CHECK(OK("108237fc(1000,1ab) ")&&(r?succeeded():failed()));++cases;
        reset();ret(0x1081c950,r);record(0x32,0,0);w8(NULL,REC+4,7);RUN();CHECK(OK(r?"1081c950(1000,7) 1086bbe8(1000,a008) ":"1081c950(1000,7) ")&&(r?succeeded():failed()));++cases;}
    /* Kind 0x35: clear bit 6 of +0x1b. */
    for(uint32_t v=0;v<4;++v){reset();ret(PX_BATTLE_POSITION,v&1?0xa:0);w8(NULL,MON+0x200+0x1b,(uint8_t)(v&2?0xff:0xbf));w32(NULL,MON+0x204,0x99);record(0x35,0,0);w8(NULL,REC+4,2);RUN();
        if(v&1)CHECK(OK("1086c2cc(220c,2) ")&&failed());
        else if(!(v&2))CHECK(OK("1086c2cc(220c,2) ")&&failed()&&r32(NULL,MON+0x204)==0x99);
        else CHECK(OK("1086c2cc(220c,2) 1086c22c(5555,55,2) 1086bbe8(1000,a008) ")&&mem[MON+0x21b]==0xbf&&r32(NULL,MON+0x204)==0&&succeeded());
        ++cases;}
    /* Kind 0x37. */
    for(uint32_t v=0;v<4;++v){reset();hp[1]=v&1;ret(0x1081fc20,v>>1&1);record(0x37,0,0);w8(NULL,REC+4,1);w8(NULL,REC+5,6);RUN();
        if(!(v&1))CHECK(OK("")&&failed());else CHECK(OK(v&2?"1081fc20(1000,7100,6,0) 1086bbe8(1000,a008) ":"1081fc20(1000,7100,6,0) ")&&(v&2?succeeded():failed()));++cases;}
    /* Kinds 0x38, 0x39. */
    for(uint32_t v=0;v<4;++v){reset();record(0x38,0,0);w16(NULL,REC+4,0x111);w8(NULL,REC+6,2);w8(NULL,REC+7,3);w16(NULL,REC+8,0x444);w8(NULL,REC+0xa,(uint8_t)(v&1?9:0));w8(NULL,REC+0xb,(uint8_t)(v>>1));RUN();
        char want[128];snprintf(want,sizeof want,"%s1086a6e0(1000,111,2,3,%u,444) 1086bbe8(1000,a00c) ",v&2?"1086c22c(5555,62,0) ":"",v&1);CHECK(OK(want)&&succeeded());++cases;}
    /* Kind 0x3a: form change. */
    for(uint32_t v=0;v<16;++v){reset();hp[3]=v&1?0:1;w8(NULL,MON+0x31b,(uint8_t)(v&2?0x20:0xdf));w8(NULL,MON+0x300+0x153,(uint8_t)(v&4?2:1));record(0x3a,0,v&8?0x800000u:0);w8(NULL,REC+4,3);w8(NULL,REC+5,2);RUN();
        if((v&1)||(v&2)||(v&4)){CHECK(OK("")&&failed());++cases;continue;}
        CHECK(OK(v&8?"1086c22c(5555,66,13) 1086fff0(7300,2) 1086c22c(5555,5b,3,2) 1086bbe8(1000,a008) 1086c22c(5555,67,13) "
                    :"1086fff0(7300,2) 1086c22c(5555,5b,3,2) 1086bbe8(1000,a008) ")&&succeeded());++cases;}
    /* Kinds 0x3b, 0x3c. */
    reset();record(0x3b,0,0);w8(NULL,REC+4,0x42);RUN();CHECK(mem[FIELD+4]==0x42&&OK("")&&succeeded());++cases;
    reset();w8(NULL,FIELD+5,0x80);record(0x3c,0,0);RUN();CHECK(mem[FIELD+5]==0x81&&OK("")&&succeeded());++cases;
    /* Kind 0x3e: command 100 for a message of at most two arguments. */
    for(uint32_t n=0;n<4;++n){reset();record(0x3e,0,0);w8(NULL,REC+4,1);w8(NULL,REC+5,2);w16(NULL,REC+8,0x1c0);w16(NULL,REC+0xa,(uint16_t)(n<<7|0x45|0x8000));
        w32(NULL,REC+0xc,0x12345678);w32(NULL,REC+0x10,0x9abcdef0);RUN();
        CHECK(OK(n>2?"FAULT 1086c22c(5555,64,1,2,45,1c0,5678,def0) ":"1086c22c(5555,64,1,2,45,1c0,5678,def0) ")&&succeeded());++cases;}
    /* End to end: a Frisk-style sequence pushed and committed through battle_action. */
    {
        reset();w32(NULL,WORD,0);
        for(unsigned i=0;i<PX_BATTLE_ACTION_KINDS;++i){sizes[2*i]=(uint8_t)i;sizes[2*i+1]=8;}
        sizes[2*1+1]=sizes[2*2+1]=4;sizes[2*3+1]=0x28;
        px_battle_ability_popup(CTX,1,2,&action);
        const uint32_t r=px_battle_action_record(CTX,3,2,&action);
        px_battle_message(r+4,2,0x1b7,&action);px_battle_message_arg(r+4,2,&action);
        px_battle_action_commit(CTX,r,&action);
        px_battle_ability_popup(CTX,2,2,&action);
        CHECK(OK("1086c2cc(220c,2) 1086c22c(5555,66,12) 1086bbe8(1000,5384) 1086c22c(5555,67,12) ")&&faults==0);
        CHECK(r==CTX+0x4380&&r16(NULL,r+4)==0x1b7&&r32(NULL,r+8)==2);
        CHECK(word_after()==0x70000000u);++cases;   /* all ran, popped */
        /* Bit 24 after a failure: skipped. */
        log_[0]=0;ret(PX_BATTLE_POSITION,0xa);px_battle_ability_popup(CTX,1,2,&action);CHECK(failed());
        const uint32_t s=px_battle_action_record(CTX,3,2,&action);w32(NULL,s,r32(NULL,s)|0x1000000u);px_battle_action_commit(CTX,s,&action);
        CHECK(OK("1086c2cc(220c,2) ")&&failed()&&(word_after()>>10&0x1ff)==0);++cases;
    }
    printf("battle_process: %u cases, all tests passed\n",cases);
    return 0;
}
