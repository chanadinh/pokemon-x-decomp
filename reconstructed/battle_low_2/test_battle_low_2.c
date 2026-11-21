#include "battle_low_2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, PAGE=0x1000, SUB=0x2800, STATE=0x4000 };
static uint8_t mem[SIZE],before[SIZE],expect_mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static char log_[1024];static uint32_t svc_ready,busy_ret;
static void notef(const char *f,uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t g,uint32_t h)
{ char t[200];snprintf(t,sizeof t,f,a,b,c,d,e,g,h);strncat(log_,t,sizeof log_-strlen(log_)-1); }
static uint32_t service_value(uint32_t t,uint32_t a) { if(t==0x10858cb8||t==0x10801094)return svc_ready;return (t^(a*0x9e3779b1u))&0xffffffffu; }
static uint32_t call(void *u,uint32_t t,uint32_t a,uint32_t b,uint32_t c,uint32_t d)
{ (void)u;notef("c%x(%x,%x,%x,%x) ",t,a,b,c,d,0,0);return service_value(t,a); }
static uint32_t call_stack(void *u,uint32_t t,uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t s0,uint32_t s1)
{ (void)u;notef("s%x(%x,%x,%x,%x|%x,%x) ",t,a,b,c,d,s0,s1);return service_value(t,a); }
static uint32_t vcall(void *u,uint32_t o,uint32_t off,uint32_t a)
{ (void)u;notef("v%x+%x(%x) ",o,off,a,0,0,0,0);return off==0xc0?busy_ret:(o^off^0x5a5a5a5au); }
static const struct px_battle_low_2_api api={&memory,NULL,STATE,call,call_stack,vcall};
static unsigned long long lcg=0x2468ace;
static uint32_t rnd(void) { lcg=lcg*6364136223846793005ULL+1442695040888963407ULL;return (uint32_t)(lcg>>33); }
static void setup(uint32_t id)
{
    for(unsigned i=0;i<SIZE;++i)mem[i]=(uint8_t)rnd();
    w32(NULL,PAGE+0x190,SUB);w32(NULL,PAGE+0x6b8,rnd()%4==0?0u:rnd());
    const uint32_t mode=rnd()%3;w32(NULL,STATE+0x6c,mode==0?id:mode==1?id+1:rnd());
    log_[0]=0;svc_ready=rnd()%2?0u:rnd()|1;busy_ret=rnd()%2?0u:rnd()|1;
    memcpy(before,mem,SIZE);memcpy(expect_mem,mem,SIZE);
}
static void e8(uint32_t a,uint32_t v) { expect_mem[a]=(uint8_t)v; }
static void e32(uint32_t a,uint32_t v) { e8(a,v);e8(a+1,v>>8);e8(a+2,v>>16);e8(a+3,v>>24); }
static void e16(uint32_t a,uint32_t v) { e8(a,v);e8(a+1,v>>8); }
static void done(uint32_t id,const char *want)
{
    e32(STATE+0x6c,id);   /* the tracker always ends with the id stored */
    if(strcmp(log_,want)){fprintf(stderr,"got  %s\nwant %s\n",log_,want);CHECK(0);}
    if(memcmp(mem,expect_mem,SIZE)){for(unsigned i=0;i<SIZE;++i)if(mem[i]!=expect_mem[i]){fprintf(stderr,"byte %x = %x want %x\n",i,mem[i],expect_mem[i]);break;}CHECK(0);}
}
static unsigned long cases;
#define TRIALS for(unsigned t=0;t<60;++t)
static uint32_t sv(uint32_t t,uint32_t a) { return service_value(t,a); }
int main(void)
{
    char want[1024];
    /* The plain trackers. */
    TRIALS{setup(0x115b);CHECK(px_battle_state_1083b44c(&api)==1);done(0x115b,"");++cases;}
    TRIALS{setup(0xdf3);px_battle_state_1083c518(&api);done(0xdf3,"");++cases;}
    TRIALS{setup(0x14c7);CHECK(px_battle_state_1083c960(&api)==1);done(0x14c7,"");++cases;}
    TRIALS{setup(0x1598);CHECK(px_battle_state_1083c984(&api)==1);done(0x1598,"");++cases;}
    TRIALS{setup(0x14e0);CHECK(px_battle_state_1083e7dc(&api)==1);done(0x14e0,"");++cases;}
    TRIALS{setup(0x1592);px_battle_state_1083e800(&api);done(0x1592,"");++cases;}
    /* 108399ec: the registers go through to 10858318. */
    TRIALS{setup(0x11fd);const uint32_t a=rnd(),b=rnd(),c=rnd(),d=rnd();const uint32_t r=px_battle_state_108399ec(a,b,c,d,&api);
        snprintf(want,sizeof want,"c10858318(%x,%x,%x,%x) ",a,b,c,d);CHECK(r==sv(0x10858318,a));done(0x11fd,want);++cases;}
    /* 1083f134. */
    TRIALS{setup(0x9e1);px_battle_state_1083f134(PAGE,&api);e32(PAGE+0xf0,0);done(0x9e1,"");++cases;}
    /* 1083b17c, 1083c424. */
    TRIALS{for(unsigned w=0;w<2;++w){setup(w?0x11a6:0x1180);const uint32_t busy=r32(NULL,PAGE+0x6b8)!=0;
        const uint32_t r=w?px_battle_state_1083c424(PAGE,&api):px_battle_state_1083b17c(PAGE,&api);
        snprintf(want,sizeof want,"c10858cb8(%x,0,0,0) ",PAGE);CHECK(r==(busy?1u:sv(0x10858cb8,PAGE)));done(w?0x11a6:0x1180,busy?"":want);++cases;}}
    /* 1083bad8, 1083f270. */
    TRIALS{for(unsigned w=0;w<2;++w){setup(w?0x1162:0x1155);const uint32_t pv=r32(NULL,SUB+0x65c);
        const uint32_t r=w?px_battle_state_1083f270(PAGE,&api):px_battle_state_1083bad8(PAGE,&api);
        snprintf(want,sizeof want,"c10800864(%x,%x,0,0) c10800874(%x,%x,1,0) ",pv,w?2u:3u,pv,w?3u:2u);
        CHECK(r==sv(0x10800874,pv));done(w?0x1162:0x1155,want);++cases;}}
    /* 1083bdd4. */
    TRIALS{setup(0xdf9);CHECK(px_battle_state_1083bdd4(PAGE,&api)==1);snprintf(want,sizeof want,"c10859cc4(%x,0,0,0) ",SUB+0x280);done(0xdf9,want);++cases;}
    /* 1083e7a0. */
    TRIALS{setup(0x14ab);const uint32_t p=r32(NULL,SUB+0x4f4);const uint32_t r=px_battle_state_1083e7a0(PAGE,&api);
        snprintf(want,sizeof want,"c10801094(%x,0,0,0) ",p);CHECK(r==(svc_ready!=0?1u:0u));done(0x14ab,want);++cases;}
    /* 1083e920. */
    TRIALS{setup(0x97d);const uint32_t a=rnd(),b=rnd();px_battle_state_1083e920(PAGE,a,b,&api);
        snprintf(want,sizeof want,"c108a26c8(%x,%x,0,0) ",SUB,a);e32(SUB+0x448,b);done(0x97d,want);++cases;}
    /* 1083e960. */
    TRIALS{setup(0x969);const uint32_t a=rnd();const uint32_t r=px_battle_state_1083e960(PAGE,a,&api);
        snprintf(want,sizeof want,"s10858150(%x,%x,0,%x|0,0) ",SUB+0x3a0,r32(NULL,SUB+0x7b0),a);CHECK(r==sv(0x10858150,SUB+0x3a0));done(0x969,want);++cases;}
    /* 1083ea0c. */
    TRIALS{setup(0x146f);const uint32_t a=rnd(),b=rnd();const uint32_t obj=r32(NULL,SUB);px_battle_state_1083ea0c(PAGE,a,b,&api);
        const uint32_t made=obj^0x24^0x5a5a5a5au;snprintf(want,sizeof want,"v%x+24(0) c10876384(%x,%x,0,0) ",obj,made,a);
        e32(SUB+0x278,sv(0x10876384,made));e16(SUB+0x27c,b);e32(SUB+0x3c,0);done(0x146f,want);++cases;}
    /* 1083f348. */
    TRIALS{setup(0xa44);const uint32_t a=rnd(),b=rnd();const uint32_t r=px_battle_state_1083f348(PAGE,a,b,&api);
        snprintf(want,sizeof want,"c10856954(%x,0,0,0) c1084a214(%x,%x,0,0) ",SUB,SUB+0x140,SUB+0x1c4);
        e32(PAGE+0x764,0);e8(SUB+8,0);e8(SUB+0x1c9,1);e8(SUB+0x1ca,a);e8(SUB+0x1cb,b);e8(SUB+0x1cc,1);
        CHECK(r==sv(0x1084a214,SUB+0x140));done(0xa44,want);++cases;}
    /* 1083cb44 and 10839818. */
    TRIALS{for(unsigned w=0;w<2;++w){setup(w?0xfc4:0x1057);if(!w&&rnd()%3==0)w32(NULL,PAGE+0x6b8,0xff);memcpy(expect_mem,mem,SIZE);memcpy(before,mem,SIZE);
        const uint32_t early=!w&&r32(NULL,PAGE+0x6b8)==0xff;
        const uint32_t r=w?px_battle_state_10839818(PAGE,&api):px_battle_state_1083cb44(PAGE,&api);
        want[0]=0;if(!early){snprintf(want,sizeof want,"c10858cb8(%x,0,0,0) ",PAGE);
            if(svc_ready)snprintf(want+strlen(want),sizeof want-strlen(want),"c1085db0c(%x,0,0,0) c10859cc4(%x,0,0,0) ",PAGE,SUB+0x280);}
        CHECK(r==(early?1u:svc_ready?1u:0u));done(w?0xfc4:0x1057,want);++cases;}}
    /* 108395d8. */
    TRIALS{setup(0xfb3);const uint32_t a=rnd();const uint32_t r=px_battle_state_108395d8(PAGE,a,&api);
        snprintf(want,sizeof want,"c1086fd18(%x,ff,0,0) c10859464(%x,%x,0,0) ",PAGE+0x6ca,PAGE,a);
        e8(PAGE+0x25d,0);e8(PAGE+0x6c8,0);e8(PAGE+0x6c9,0);e8(PAGE+0x6ca,0);e32(PAGE+0x6b8,0);e32(PAGE+0x6e0,a);
        CHECK(r==sv(0x10859464,PAGE));done(0xfb3,want);++cases;}
    /* 1083b110. */
    TRIALS{setup(0x11f1);const uint32_t a=rnd(),b=rnd();const uint32_t r=px_battle_state_1083b110(PAGE,a,b,&api);
        snprintf(want,sizeof want,"c1086fd18(%x,ff,0,0) c1085746c(%x,%x,273,0) ",PAGE+0x6ca,PAGE,a);
        e8(PAGE+0x25d,0);e8(PAGE+0x6c8,a);e8(PAGE+0x6c9,b);e8(PAGE+0x6ca,b);CHECK(r==sv(0x1085746c,PAGE));done(0x11f1,want);++cases;}
    /* 1083cbb0. */
    TRIALS{setup(0x145e);const uint32_t obj88=r32(NULL,SUB+0x88);const uint32_t r=px_battle_state_1083cbb0(PAGE,&api);
        const uint32_t w238=r32(NULL,SUB+0x238),w74=r32(NULL,SUB+0x74);
        snprintf(want,sizeof want,"c108568ec(0,0,0,0) c1085a18c(%x,1,0,0) v%x+10(0) c108568ec(0,0,0,0) c1085a18c(%x,1,0,0) ",w238,obj88,w74);
        e32(SUB+0x23c,0);e8(SUB+0x244,0);e32(SUB+0x88,0);e8(SUB+0x93,0);CHECK(r==sv(0x1085a18c,w74));done(0x145e,want);++cases;}
    /* 1083f1f4. */
    TRIALS{setup(0x14d8);const uint32_t w4e0=r32(NULL,SUB+0x4e0);const uint32_t r=px_battle_state_1083f1f4(PAGE,&api);
        snprintf(want,sizeof want,"c10860270(%x,%x,0,0) v%x+c0(0) ",SUB+0x4ec,w4e0,PAGE);
        if(busy_ret==0)strcat(want,"c1080024c(60000,0,ffffffff,0) ");
        CHECK(r==(busy_ret!=0?busy_ret:sv(0x1080024c,0x60000)));done(0x14d8,want);++cases;}
    printf("battle_low_2: %lu cases, all tests passed\n",cases);
    return 0;
}
