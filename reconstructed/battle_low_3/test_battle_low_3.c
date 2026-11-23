#include "battle_low_3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, PAGE=0x1000, SUB=0x2800, STATE=0x4000, SCR=0x6000, LIST=0x6800, OBJ20=0x6c00, ENT=0x8000 };
static uint8_t mem[SIZE],expect_mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static char log_[2048];
static unsigned trace_state;
static uint32_t ok1,ok2,kind_ret,busy_ret,ok7c,mode44,v10c,be10[6],be10_pos;
static void notef(const char *f,uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t g,uint32_t h)
{ char t[200];snprintf(t,sizeof t,f,a,b,c,d,e,g,h);strncat(log_,t,sizeof log_-strlen(log_)-1); }
static uint32_t hash(uint32_t t,uint32_t a) { return (t^(a*0x9e3779b1u)); }
static uint32_t service_value(uint32_t t,uint32_t a)
{
    switch(t){case 0x108a2abc:return ok1;case 0x108573b4:return ok2;case 0x108a31a0:return kind_ret;
        case 0x1083be10:return be10[be10_pos++%6];default:return hash(t,a);}
}
static uint32_t call(void *u,uint32_t t,uint32_t a,uint32_t b,uint32_t c,uint32_t d)
{ (void)u;notef("c%x(%x,%x,%x,%x) ",t,a,b,c,d,0,0);if(trace_state)notef("<%x> ",r32(NULL,STATE+0x6c),0,0,0,0,0,0);return service_value(t,a); }
static uint32_t call_stack(void *u,uint32_t t,uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t s0,uint32_t s1)
{ (void)u;notef("s%x(%x,%x,%x,%x|%x,%x) ",t,a,b,c,d,s0,s1);return service_value(t,a); }
static uint32_t vcall(void *u,uint32_t o,uint32_t off,uint32_t a)
{
    (void)u;notef("v%x+%x(%x) ",o,off,a,0,0,0,0);if(trace_state)notef("<%x> ",r32(NULL,STATE+0x6c),0,0,0,0,0,0);
    switch(off){case 0x24:return LIST;case 0x20:return OBJ20;case 0xc0:case 0xbc:return busy_ret;case 0x7c:return ok7c;
        case 0x44:return mode44;case 0x10c:return v10c;default:return 0;}
}
static const struct px_battle_low_2_api base={&memory,NULL,STATE,call,call_stack,vcall};
static const struct px_battle_low_3_api api={&base,SCR};
static unsigned long long lcg=0x13579bdf;
static uint32_t rnd(void) { lcg=lcg*6364136223846793005ULL+1442695040888963407ULL;return (uint32_t)(lcg>>33); }
static void setup(uint32_t id)
{
    for(unsigned i=0;i<SIZE;++i)mem[i]=(uint8_t)rnd();
    w32(NULL,PAGE+0x190,SUB);for(unsigned i=0;i<6;++i)w32(NULL,PAGE+0x100+4*i,ENT+0x40*i);
    const uint32_t mode=rnd()%3;w32(NULL,STATE+0x6c,mode==0?id:mode==1?id+1:rnd());
    for(unsigned i=0;i<6;++i)w8(NULL,ENT+0x40*i+0x15c,rnd()%2?0:(uint8_t)(rnd()|1));
    trace_state=0;log_[0]=0;ok1=rnd()%2;ok2=rnd()%2;kind_ret=rnd()%16;busy_ret=rnd()%2?0u:(rnd()|1);ok7c=rnd()%2?0u:(rnd()|1);mode44=rnd()%2?4u:rnd()%8;
    v10c=rnd()%3==0?0u:rnd()%6;for(unsigned i=0;i<6;++i)be10[i]=rnd()%2;be10_pos=0;
    memcpy(expect_mem,mem,SIZE);
}
static void e8(uint32_t a,uint32_t v) { expect_mem[a]=(uint8_t)v; }
static void e16(uint32_t a,uint32_t v) { e8(a,v);e8(a+1,v>>8); }
static void e32(uint32_t a,uint32_t v) { e16(a,v);e16(a+2,v>>16); }
static void done(uint32_t id,const char *want)
{
    e32(STATE+0x6c,id);
    if(strcmp(log_,want)){fprintf(stderr,"got  %s\nwant %s\n",log_,want);CHECK(0);}
    if(memcmp(mem,expect_mem,SIZE)){for(unsigned i=0;i<SIZE;++i)if(mem[i]!=expect_mem[i]){fprintf(stderr,"byte %x = %x want %x\n",i,mem[i],expect_mem[i]);break;}CHECK(0);}
}
static unsigned long cases;
#define TRIALS for(unsigned t=0;t<80;++t)
static void cat(char *w,const char *f,uint32_t a,uint32_t b,uint32_t c,uint32_t d)
{ char t[200];snprintf(t,sizeof t,f,a,b,c,d);strcat(w,t); }
/* 1083b9f0 and 1083c770 share the entry choice. */
int main(void)
{
    char want[2048];
    /* 1083b3cc. */
    TRIALS{setup(0xa7f);const uint32_t a=rnd(),b=rnd(),w1e0=r32(NULL,PAGE+0x1e0);const uint32_t r=px_battle_state_1083b3cc(PAGE,a,b,&api);
        snprintf(want,sizeof want,"v%x+bc(0) ",PAGE);
        if(busy_ret==0){cat(want,"c1089d648(%x,%x,%x,0) ",w1e0,a,b,0);cat(want,"s10858150(%x,%x,0,0|0,0) ",SUB+0x3a0,w1e0,0,0);}
        CHECK(r==(busy_ret?busy_ret:hash(0x10858150,SUB+0x3a0)));done(0xa7f,want);++cases;}
    /* 1083b470. */
    TRIALS{setup(0x9bc);static const uint32_t steps[5]={0,1,2,5,0xffffffffu};const uint32_t step=steps[rnd()%5];w32(NULL,PAGE+0xf0,step);w8(NULL,SUB+0xa0,rnd()%2?0:(uint8_t)(rnd()|1));memcpy(expect_mem,mem,SIZE);
        const uint32_t w764=r32(NULL,PAGE+0x764),flag=r8(NULL,SUB+0xa0);const uint32_t r=px_battle_state_1083b470(PAGE,&api);
        want[0]=0;uint32_t ret=0;
        if(step==0||step==1){
            int go=1;uint32_t cur=step;
            if(step==0){cat(want,"c108a2abc(%x,%x,0,0) ",SUB,w764,0,0);if(!ok1)go=0;else{cat(want,"c10836230(%x,0,0,0) ",SUB+0xa8,0,0,0);cur=1;e32(PAGE+0xf0,1);}}
            (void)cur;
            if(go){cat(want,"c108573b4(%x,0,0,0) ",SUB,0,0,0);
                if(ok2){if(flag==0){cat(want,"c10857178(%x,%x,0,0) ",SUB+0x5c,SUB+0xa1,0,0);e8(SUB+0xa0,1);e8(SUB+8,1);}e32(PAGE+0xf0,0);ret=1;}}}
        CHECK(r==ret);done(0x9bc,want);++cases;}
    /* 1083b530. */
    TRIALS{setup(0x1465);if(rnd()%3==0)w32(NULL,SUB+0x240,0);memcpy(expect_mem,mem,SIZE);const uint32_t obj=r32(NULL,SUB+0x240);px_battle_state_1083b530(PAGE,&api);
        want[0]=0;if(obj){cat(want,"v%x+4(0) ",obj,0,0,0);e32(SUB+0x240,0);}done(0x1465,want);++cases;}
    /* 1083b9f0 and 1083c770. */
    TRIALS{for(unsigned w=0;w<2;++w){setup(w?0x1190:0x116b);const uint32_t a=rnd();const uint32_t r=w?px_battle_state_1083c770(PAGE,a,&api):px_battle_state_1083b9f0(PAGE,a,&api);
        snprintf(want,sizeof want,"c1086fd18(%x,ff,0,0) v%x+10c(%x) v%x+10c(%x) ",PAGE+0x6ca,PAGE,a,PAGE,a);
        e8(PAGE+0x6c8,v10c);e8(PAGE+0x6c9,v10c);e8(PAGE+0x25d,0);
        const uint32_t entry=ENT+0x40*(v10c&0xff);cat(want,"v%x+7c(0) ",entry,0,0,0);
        const uint32_t msg=w?0x270:0x26f;uint32_t ret;
        if(ok7c==0&&r8(NULL,entry+0x15c)==0){e32(PAGE+0x6b8,0x64);ret=0x64;}
        else{e32(PAGE+0x6b8,0);cat(want,"c10859464(%x,%x,0,0) ",PAGE,msg,0,0);ret=hash(0x10859464,PAGE);}
        CHECK(r==ret);done(w?0x1190:0x116b,want);++cases;}}
    /* 1083bb1c. */
    TRIALS{setup(0x993);w8(NULL,LIST+0x18,(uint8_t)(rnd()%9));memcpy(expect_mem,mem,SIZE);const uint32_t a=rnd(),b=rnd(),c=rnd(),d=rnd();
        const uint32_t size=r8(NULL,LIST+0x18);px_battle_state_1083bb1c(PAGE,a,b,c,d,&api);
        snprintf(want,sizeof want,"v%x+24(0) ",PAGE);e32(PAGE+0x760,a);e32(PAGE+0x764,d);
        for(uint32_t i=0;i<size;++i){const uint32_t m=hash(0x1085daf8,LIST);cat(want,"c1085daf8(%x,%x,0,0) ",LIST,i,0,0);cat(want,"c10856a88(%x,13,0,0) ",m,0,0,0);e8(SCR+i,hash(0x10856a88,m));}
        for(uint32_t i=size;i<6;++i)e8(SCR+i,0);
        e32(SUB+0x3c,0);e8(SUB+0x4a,c^1);e8(SUB+0x138,c^1);e8(SUB+0x13f,b);e8(SUB+0x40,b);cat(want,"c1085fa8c(%x,%x,0,0) ",SUB+0x139,SCR,0,0);
        done(0x993,want);++cases;}
    /* 1083c1d8. */
    TRIALS{setup(0x102f);trace_state=1;const uint32_t a=rnd()%6,b=rnd(),c=rnd();if(rnd()%4==0)w32(NULL,PAGE+0x100+4*a,0);memcpy(expect_mem,mem,SIZE);
        const uint32_t entry=r32(NULL,PAGE+0x100+4*a);px_battle_state_1083c1d8(PAGE,a,b,c,&api);
        snprintf(want,sizeof want,"c1086fd18(%x,ff,0,0) <102f> ",PAGE+0x6ca);e8(PAGE+0x25d,0);e8(PAGE+0x6c8,a);e8(PAGE+0x6c9,b);e8(PAGE+0x6ca,b);e32(PAGE+0x6b8,0);e32(PAGE+0x6e0,c);
        if(entry!=0){cat(want,"v%x+7c(0) <102f> ",entry,0,0,0);}
        if(entry!=0&&ok7c!=0){const uint32_t obj0=r32(NULL,SUB);cat(want,"v%x+44(0) <102f> ",obj0,0,0,0);
            const uint32_t flag=mode44==4?(a!=0):(a&1);e8(PAGE+0x25d,flag);cat(want,"c10859464(%x,%x,0,0) <102f> ",PAGE,c,0,0);}
        else e32(PAGE+0x6b8,0xff);
        done(0x1051,want);++cases;}
    /* 1083c9a8. */
    TRIALS{setup(0xabe);const uint32_t a=rnd(),b=rnd(),c=rnd();const uint32_t r=px_battle_state_1083c9a8(PAGE,a,b,c,&api);
        snprintf(want,sizeof want,"v%x+bc(0) ",PAGE);
        if(busy_ret==0){cat(want,"c1089da54(%x,%x,%x,0) ",r32(NULL,SUB+0x7b0),a&0xffff,(uint32_t)(int32_t)(int8_t)b,0);cat(want,"s10858150(%x,%x,0,%x|0,0) ",SUB+0x3a0,r32(NULL,SUB+0x7b0),c,0);}
        CHECK(r==1);done(0xabe,want);++cases;}
    /* 1083ca38. */
    TRIALS{setup(0xa18);const uint32_t a=rnd(),b=rnd(),c=rnd(),d=rnd();const uint32_t r=px_battle_state_1083ca38(PAGE,a,b,c,d,&api);
        snprintf(want,sizeof want,"c10856954(%x,0,0,0) c1084a214(%x,%x,0,0) c1084a0bc(%x,0,0,0) c108577b4(%x,0,0,0) ",SUB,SUB+0x140,SUB+0x1c4,SUB+0x140,SUB);
        e32(PAGE+0x760,a);e32(PAGE+0x764,d);e8(SUB+8,0);e8(SUB+0x1c9,0);e32(SUB+0x1c4,a);e8(SUB+0x1c8,c);e8(SUB+0x1cc,1);e32(SUB+0x1d0,b);
        CHECK(r==hash(0x108577b4,SUB));done(0xa18,want);++cases;}
    /* 1083cac0. */
    TRIALS{setup(0x1479);const uint32_t out=0x6e00;const uint32_t r=px_battle_state_1083cac0(PAGE,out,&api);
        snprintf(want,sizeof want,"c108a31a0(%x,0,0,0) ",SUB);e8(out,0);uint32_t ret=0;
        if(kind_ret==2){e8(out,4);ret=1;}else if(kind_ret>=0xa&&kind_ret<=0xd){e8(out,kind_ret-0xa);ret=1;}
        CHECK(r==ret);done(0x1479,want);++cases;}
    for(uint32_t k=0;k<0x20;++k){setup(0x1479);kind_ret=k;px_battle_state_1083cac0(PAGE,0x6e00,&api);
        const uint32_t exp=k==2?4:(k>=0xa&&k<=0xd)?k-0xa:0;CHECK(r8(NULL,0x6e00)==exp);++cases;}
    /* 1083e4c4. */
    TRIALS{setup(0x11ca);const uint32_t a=rnd();w16(NULL,OBJ20+0xc,rnd()%2?0x2a9:rnd());w8(NULL,OBJ20+0x153,(uint8_t)(rnd()%2));memcpy(expect_mem,mem,SIZE);
        const uint32_t half=r16(NULL,OBJ20+0xc),flag=r8(NULL,OBJ20+0x153);const uint32_t r=px_battle_state_1083e4c4(PAGE,a,&api);
        snprintf(want,sizeof want,"c1086fd18(%x,ff,0,0) v%x+20(%x) ",PAGE+0x6ca,PAGE,a);e8(PAGE+0x25d,0);e8(PAGE+0x6c8,a);e8(PAGE+0x6c9,a);e8(PAGE+0x6ca,a);
        const uint32_t msg=half==0x2a9?(flag?0x284:0x285):0x272;cat(want,"c1085746c(%x,%x,%x,0) ",PAGE,a,msg,0);
        CHECK(r==hash(0x1085746c,PAGE));done(0x11ca,want);++cases;}
    /* 1083e58c. */
    TRIALS{setup(0x1711);const uint32_t a=rnd(),b=rnd(),c=rnd(),d=rnd(),e=rnd();px_battle_state_1083e58c(PAGE,a,b,c,d,e,&api);
        snprintf(want,sizeof want,"c1086fd18(%x,ff,0,0) c10800c04(%x,0,0,0) ",PAGE+0x6ca,b);
        e8(PAGE+0x25d,0);e8(PAGE+0x6c8,0);e8(PAGE+0x6c9,a);e8(PAGE+0x6ca,a);e32(PAGE+0x6d0,0);e32(PAGE+0x6d4,0);e16(PAGE+0x6e4,hash(0x10800c04,b));
        e32(PAGE+0x6b8,0);e32(PAGE+0x6bc,0);e32(PAGE+0x6fc,b);e32(PAGE+0x700,c);e8(PAGE+0x71c,d);e8(PAGE+0x71d,e);done(0x1711,want);++cases;}
    /* 1083f15c. */
    TRIALS{setup(0x14a3);const uint32_t a=rnd(),b=rnd();const uint32_t r=px_battle_state_1083f15c(PAGE,a,b,&api);
        snprintf(want,sizeof want,"c108577b4(%x,0,0,0) c1082d830(%x,0,0,0) c1082d6d8(%x,%x,%x,0) v%x+c0(0) ",SUB,SUB+0x4dc,SUB+0x4dc,a,b,PAGE);
        if(busy_ret==0)strcat(want,"c1080024c(60000,0,ffffffff,0) ");
        CHECK(r==(busy_ret?busy_ret:hash(0x1080024c,0x60000)));done(0x14a3,want);++cases;}
    /* 1083f2b4. */
    TRIALS{setup(0x111c);const uint32_t a=rnd()%6;px_battle_state_1083f2b4(PAGE,a,&api);
        snprintf(want,sizeof want,"c1086fd18(%x,ff,0,0) ",PAGE+0x6ca);e8(PAGE+0x6c9,0xff);e8(PAGE+0x25d,0);e8(PAGE+0x6c8,a);
        const uint32_t entry=ENT+0x40*a;cat(want,"v%x+7c(0) ",entry,0,0,0);
        if(ok7c==0&&r8(NULL,entry+0x15c)==0)e32(PAGE+0x6b8,0x64);else{e32(PAGE+0x6b4,0);e32(PAGE+0x6b8,1);}
        done(0x111c,want);++cases;}
    /* 1083f3ac. */
    TRIALS{setup(0xd8b);for(unsigned i=0;i<6;++i)w8(NULL,PAGE+0x6ca+i,rnd()%3==0?0xff:rnd()%0x40);memcpy(expect_mem,mem,SIZE);
        const uint32_t w6b4=r32(NULL,PAGE+0x6b4);const uint32_t r=px_battle_state_1083f3ac(PAGE,&api);
        want[0]=0;uint32_t pos=0,okf=1;
        for(uint32_t i=0;i<6;++i){const uint32_t v=r8(NULL,PAGE+0x6ca+i);if(v==0xff)continue;cat(want,"c1083be10(%x,",PAGE,0,0,0);cat(want,"%x,%x,0) ",v,w6b4-10*i,0,0);if(!be10[pos++%6])okf=0;}
        e32(PAGE+0x6b4,w6b4+1);if(okf)cat(want,"c10859cc4(%x,0,0,0) ",SUB+0x280,0,0,0);
        CHECK(r==okf);done(0xd8b,want);++cases;}
    printf("battle_low_3: %lu cases, all tests passed\n",cases);
    return 0;
}
