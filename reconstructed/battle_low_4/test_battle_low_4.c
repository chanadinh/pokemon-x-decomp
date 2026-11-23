#include "battle_low_4.h"
#include "../battle_ability/battle_ability.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, PAGE=0x1000, SUB=0x2800, STATE=0x4000, TABLE=0x5000, OBJ=0x6000, CTX=0x8000, LIST=0xa000, BAT=0xaf00, ENT=0xb000,
       STACK=0xc000, BATTLE=0xd000, POOL=0xd100, HANDLER=0xd800, ENTRIES=0xdc00, OBJS=0xe000 };
static uint8_t mem[SIZE],expect_mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static char log_[2048];static unsigned trace_state,aborts;
static uint32_t ok_a,ok_b,okv[6],idv[6],h43,h50;
static void notef(const char *f,uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t g,uint32_t h)
{ char t[200];snprintf(t,sizeof t,f,a,b,c,d,e,g,h);strncat(log_,t,sizeof log_-strlen(log_)-1); }
static uint32_t hash(uint32_t t,uint32_t a) { return t^(a*0x9e3779b1u); }
static void tag(void) { if(trace_state)notef("<%x> ",r32(NULL,STATE+0x6c),0,0,0,0,0,0); }
static uint32_t service_value(uint32_t t,uint32_t a,uint32_t b,uint32_t c)
{
    (void)b;
    switch(t){
        case 0x1080103c:return ok_a;
        case 0x10858cb8:return ok_b;
        case 0x10865ae0:return okv[((c-ENT)/0x40)%6];
        case 0x10874348:return idv[((a-ENT)/0x40)%6];
        default:return hash(t,a);}
}
static uint32_t call(void *u,uint32_t t,uint32_t a,uint32_t b,uint32_t c,uint32_t d)
{ (void)u;notef("c%x(%x,%x,%x,%x) ",t,a,b,c,d,0,0);tag();return service_value(t,a,b,c); }
static uint32_t call_stack(void *u,uint32_t t,uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t s0,uint32_t s1)
{ (void)u;notef("s%x(%x,%x,%x,%x|%x,%x) ",t,a,b,c,d,s0,s1);tag();return hash(t,a); }
static uint32_t vcall(void *u,uint32_t o,uint32_t off,uint32_t a)
{ (void)u;notef("v%x+%x(%x) ",o,off,a,0,0,0,0);tag();return off==0xbc?ok_b:off==0x10c?0xff:hash(off,o); }
static const struct px_battle_low_2_api base={&memory,NULL,STATE,call,call_stack,vcall};
static void abort_battle(void *u) { (void)u;++aborts; }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void event_call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,event_call,STACK,BATTLE,POOL};
static const struct px_battle_low_4_api api={&base,&events,TABLE};
static void event_call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;(void)f;(void)h;(void)c;(void)v;(void)w;
    notef("h3=%x h4=%x h22=%x ",px_battle_event_get(0x03,&events),px_battle_event_get(0x04,&events),px_battle_event_get(0x22,&events),0,0,0,0);
    if(!h43)px_battle_event_rewrite(0x43,0,&events);
    if(!h50)px_battle_event_rewrite(0x50,0,&events);
}
static unsigned long long lcg=0x97531;
static uint32_t rnd(void) { lcg=lcg*6364136223846793005ULL+1442695040888963407ULL;return (uint32_t)(lcg>>33); }
static void setup(uint32_t id)
{
    for(unsigned i=0;i<SIZE;++i)mem[i]=(uint8_t)rnd();
    w32(NULL,PAGE+0x190,SUB);
    const uint32_t mode=rnd()%3;w32(NULL,STATE+0x6c,mode==0?id:mode==1?id+1:rnd());
    log_[0]=0;trace_state=0;aborts=0;ok_a=rnd()%2;ok_b=rnd()%2;h43=rnd()%2;h50=rnd()%2;
    for(unsigned i=0;i<6;++i){okv[i]=rnd()%2;idv[i]=rnd()%3==0?0x12u:rnd()%0x12;}
    memcpy(expect_mem,mem,SIZE);
}
static void e8(uint32_t a,uint32_t v) { expect_mem[a]=(uint8_t)v; }
static void e16(uint32_t a,uint32_t v) { e8(a,v);e8(a+1,v>>8); }
static void e32(uint32_t a,uint32_t v) { e16(a,v);e16(a+2,v>>16); }
static void done(int final_state,uint32_t id,const char *want)
{
    if(final_state)e32(STATE+0x6c,id);
    if(strcmp(log_,want)){fprintf(stderr,"got  %s\nwant %s\n",log_,want);CHECK(0);}
    if(memcmp(mem,expect_mem,SIZE)){for(unsigned i=0;i<SIZE;++i)if(mem[i]!=expect_mem[i]){fprintf(stderr,"byte %x = %x want %x\n",i,mem[i],expect_mem[i]);break;}CHECK(0);}
}
static void cat(char *w,const char *f,uint32_t a,uint32_t b,uint32_t c,uint32_t d)
{ char t[200];snprintf(t,sizeof t,f,a,b,c,d);strcat(w,t); }
static unsigned long cases;
#define TRIALS for(unsigned t=0;t<80;++t)
int main(void)
{
    char want[2048];
    /* 10836230. */
    TRIALS{setup(0);for(uint32_t off=0x50;off<=0x70;off+=4)if(rnd()%3==0)w32(NULL,PAGE+off,0);memcpy(expect_mem,mem,SIZE);
        want[0]=0;
        for(uint32_t off=0x70;off>=0x50;off-=4){const uint32_t o=r32(NULL,PAGE+off);if(o){cat(want,"v%x+4(0) ",o,0,0,0);e32(PAGE+off,0);}}
        px_battle_release_objects_10836230(PAGE,&api);
        done(0,0,want);++cases;}
    /* 1083b268. */
    TRIALS{setup(0xbf7);const uint32_t a=rnd();px_battle_state_1083b268(PAGE,a,&api);
        snprintf(want,sizeof want,"c1086fd18(%x,ff,0,0) v%x+10c(%x) v%x+10c(%x) v%x+bc(0) ",PAGE+0x6ca,PAGE,a,PAGE,a,PAGE);
        e32(PAGE+0x6b4,0);e32(PAGE+0x6b8,0);e8(PAGE+0x6c8,0xff);e8(PAGE+0x6c9,0xff);e8(PAGE+0x25d,0);e8(PAGE+0x6c8,0xff);e8(PAGE+0x6ca,0xff);
        if(ok_b){e32(PAGE+0x6b8,0xff);}
        done(1,0xbf7,want);++cases;}
    /* 1083ef8c. */
    TRIALS{setup(0x197c);trace_state=1;static const uint32_t steps[7]={0,0x64,0xc8,1,0x65,0xc9,0xffffffffu};const uint32_t step=steps[rnd()%7];
        const uint32_t idx=rnd()%3==0?0xffffffffu:rnd()%6;w32(NULL,PAGE+0x6b8,step);w32(NULL,PAGE+0x1b0,idx);memcpy(expect_mem,mem,SIZE);
        const uint32_t w=r32(NULL,PAGE+0xcc);const uint32_t r=px_battle_state_1083ef8c(PAGE,&api);want[0]=0;uint32_t ret=0;uint32_t st=0x197c;
        if(step==0){st=0xfc4;cat(want,"c10858cb8(%x,0,0,0) <fc4> ",PAGE,0,0,0);
            if(ok_b){cat(want,"c1085db0c(%x,0,0,0) <fc4> ",PAGE,0,0,0);cat(want,"c10859cc4(%x,0,0,0) <fc4> ",SUB+0x280,0,0,0);ret=1;}}
        else if(step==0x64){int go=1;if(idx!=0xffffffffu){cat(want,"c1080103c(%x,",w,0,0,0);cat(want,"%x,0,0) <197c> ",PAGE+4*idx+0x1b8,0,0,0);if(!ok_a)go=0;}
            if(go){cat(want,"c1083ac28(%x,0,0,0) <197c> ",PAGE,0,0,0);ret=1;}}
        else if(step==0xc8){cat(want,"c108574e8(%x,0,0,0) <197c> ",PAGE,0,0,0);ret=1;}
        CHECK(r==ret);done(1,st,want);++cases;}
    /* 10839a0c. */
    TRIALS{setup(0x154a);static const uint32_t steps[6]={0,1,2,0x64,3,0xffffffffu};static const uint32_t waits[5]={0,0x5a,0x5b,0xffff,0x1234};
        for(unsigned i=0;i<6;++i)w32(NULL,SUB+0x798+4*i,OBJS+0x40*i);
        w8(NULL,PAGE+0x494,(uint8_t)(rnd()%2));w8(NULL,SUB+0x84d,(uint8_t)(rnd()%2));w8(NULL,SUB+0x84c,(uint8_t)(rnd()%2));w16(NULL,SUB+0x880,(uint16_t)waits[rnd()%5]);
        const uint32_t step=steps[rnd()%6];w32(NULL,PAGE+0x6b8,step);const uint32_t a=rnd()%6;memcpy(expect_mem,mem,SIZE);
        const uint32_t f494=r8(NULL,PAGE+0x494),b84d=r8(NULL,SUB+0x84d),b84c=r8(NULL,SUB+0x84c);
        const uint32_t obj=OBJS+0x40*a,o10=r8(NULL,obj+0x10);const uint32_t wait=r16(NULL,SUB+0x880);const uint32_t r=px_battle_state_10839a0c(PAGE,a,&api);
        want[0]=0;uint32_t ret=0;
        if(f494!=0){
            if(b84d!=0&&b84c!=0)ret=0;
            else{e8(PAGE+0x494,0);e8(SUB+0x84c,0);e16(SUB+0x880,0);e8(SUB+0x84d,0);if(wait>0x5a){e32(PAGE+0x6b8,0);ret=0;}else ret=1;}}
        else if(step==0){cat(want,"c1086fd18(%x,ff,0,0) ",PAGE+0x6ca,0,0,0);e8(PAGE+0x25d,0);e8(PAGE+0x6c8,a);e8(PAGE+0x6c9,a);e8(PAGE+0x6ca,a);
            e8(obj+0x35,1);if(o10){e8(obj+0x10,0);e8(obj+0x44,1);}cat(want,"c10859464(%x,280,0,0) ",PAGE,0,0,0);e32(PAGE+0x6b8,1);ret=0;}
        else if(step==1){cat(want,"c10858cb8(%x,0,0,0) ",PAGE,0,0,0);if(ok_b){cat(want,"c10859464(%x,275,0,0) c10859464(%x,278,0,0) ",PAGE,PAGE,0,0);e32(PAGE+0x6b8,2);}}
        else if(step==2){cat(want,"c10858cb8(%x,0,0,0) ",PAGE,0,0,0);if(ok_b){cat(want,"c108582a0(%x,%x,0,0) ",SUB,a,0,0);ret=1;}}
        else ret=step==0x64?1:0;
        if(r!=ret){fprintf(stderr,"r=%u ret=%u step=%x okb=%u log=%s want=%s\n",r,ret,step,ok_b,log_,want);}
        CHECK(r==ret);done(1,0x154a,want);++cases;}
    /* 1083c0b8. */
    TRIALS{for(uint32_t code=0;code<12;++code){setup(0);const uint32_t kind=(uint32_t[]){0,1,2,0xff}[rnd()%4];
        const uint32_t objp=OBJ;w8(NULL,objp+0x82c,(uint8_t)kind);memcpy(expect_mem,mem,SIZE);
        px_battle_pair_for_code_1083c0b8(objp,code,&api);
        uint32_t c=code,src=0,flag=0;int go=1;if(c>=9)go=0;else if(c==8){if(kind==0||kind==1)c=1;else go=0;}
        if(go){static const uint32_t offs[8]={0,0xa4,0xac,0xb4,0xbc,0xc4,0xcc,0xd4};if(c==0){src=0x9c;flag=1;}else src=offs[c];
            e32(objp+0x730,r32(NULL,TABLE+src));e32(objp+0x734,r32(NULL,TABLE+src+4));e8(objp+0x72c,flag);}
        done(0,0,"");++cases;}}
    {setup(0);px_battle_pair_for_code_1083c0b8(OBJ,0xffffffffu,&api);done(0,0,"");++cases;}
    /* 1081cd58. */
    TRIALS{static const uint32_t kinds[4]={0,1,2,3},codes[12]={0,1,2,3,4,5,6,7,8,0xf,0x14,0xffffffffu};
        for(unsigned k=0;k<4;++k)for(unsigned c=0;c<12;++c){setup(0);const uint32_t kind=kinds[k],code=codes[c];
            w32(NULL,CTX+0xc,0x7777+t);w8(NULL,BAT+0x19,(uint8_t)(rnd()%32));memcpy(expect_mem,mem,SIZE);
            px_battle_write_message_1081cd58(CTX,BAT,kind,code,&api);
            const uint32_t id=r8(NULL,BAT+0x19),writer=0x7777+t;uint32_t first=0x6a,msg=0,cc=id,st=0xffff0000u;int go=1;
            if(kind==1){static const uint32_t ms[6]={0,0x11a,0x13b,0x129,0x10b,0xf9};
                if(code==0x14)go=0;else if(code>=1&&code<=5)msg=ms[code];else if(code==6)msg=0x162;else if(code==0xf)msg=0x18;
                else{first=0x69;msg=0x4a;cc=0xffff0000u;st=code;}}
            else if(kind==2)msg=0xd2;else{first=0x69;msg=0x4a;cc=0xffff0000u;st=code;}
            want[0]=0;if(go){snprintf(want,sizeof want,"s1086b5e0(%x,%x,%x,%x|%x,0) ",writer,first,msg,cc,st);e8(CTX+0x1093,r8(NULL,CTX+0x1093)|0x10);}
            done(0,0,want);++cases;}
        break;}
    /* 1082399c. */
    TRIALS{setup(0);memset(mem+STACK,0,0x2000);const uint32_t n=rnd()%6;const uint32_t idxstart=rnd();(void)idxstart;
        w32(NULL,BATTLE+0x60,HANDLER);w8(NULL,ENTRIES,0xa5);w32(NULL,ENTRIES+4,0);w32(NULL,HANDLER+8,ENTRIES);w32(NULL,HANDLER+0x18,0x20000000u|1u<<16);w32(NULL,HANDLER,0);w32(NULL,HANDLER+4,0);
        w32(NULL,CTX+0xc,0x7777);w8(NULL,BAT+0x19,0x21);w8(NULL,LIST+0x6e,(uint8_t)n);
        for(unsigned i=0;i<6;++i){w32(NULL,LIST+4*i,(rnd()%6==0&&i>1)?0u:ENT+0x40*i);w8(NULL,ENT+0x40*i+0x19,(uint8_t)(0x30+i));}
        px_battle_event_push(&events);px_battle_event_pop(&events);memcpy(expect_mem,mem,SIZE);
        px_battle_event_0xa5_walk_1082399c(CTX,0,BAT,LIST,&api);
        want[0]=0;e8(LIST+0x70,0);uint32_t idx=0;
        for(;;){if(idx>=n)break;e8(LIST+0x70,idx+1);const uint32_t other=r32(NULL,LIST+4*idx);++idx;if(other==0)break;
            const uint32_t ix=(other-ENT)/0x40;cat(want,"c10865ae0(%x,%x,",CTX,BAT,0,0);cat(want,"%x,0) ",other,0,0,0);
            if(okv[ix%6])continue;
            cat(want,"c10874348(%x,0,0,0) ",other,0,0,0);if(idv[ix%6]==0x12)continue;
            cat(want,"h3=%x h4=%x ",0x21,0x30+ix,0,0);cat(want,"h22=%x ",idv[ix%6],0,0,0);
            if(!h43){continue;}
            cat(want,"c10869784(%x,%x,0,0) ",LIST,other,0,0);
            if(!h50){continue;}
            cat(want,"s1086b5e0(7777,6a,d5,%x|ffff0000,0) ",0x30+ix,0,0,0);}
        memcpy(expect_mem+STACK,mem+STACK,0x2000);   /* the event stack's own bookkeeping is not part of the check */
        done(0,0,want);CHECK(aborts==0);++cases;}
    printf("battle_low_4: %lu cases, all tests passed\n",cases);
    return 0;
}
