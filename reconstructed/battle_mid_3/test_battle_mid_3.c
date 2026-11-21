#include "battle_mid_3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x4000, OBJ=0x1000, OTHER=0x2000, SCRATCH=0x3000 };
static uint8_t mem[SIZE];
static char log_[2048];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { (void)u;(void)a;(void)v;CHECK(0); }
static void w32(void *u,uint32_t a,uint32_t v) { (void)u;(void)a;(void)v;CHECK(0); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static uint32_t scratch_word;
static uint32_t external(void *u,uint32_t f,uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e)
{
    (void)u;char t[120];snprintf(t,sizeof t,"c%x(%x,%x,%x,%x,%x) ",f,a,b,c,d,e);note(t);
    if(f==0x10800504) { mem[SCRATCH]=(uint8_t)scratch_word;mem[SCRATCH+1]=(uint8_t)(scratch_word>>8);mem[SCRATCH+2]=(uint8_t)(scratch_word>>16);mem[SCRATCH+3]=(uint8_t)(scratch_word>>24); }
    return 0x1000000u+(a&0xffff)+(f&0xff);
}
static const struct px_battle_mid_3_api api={&memory,NULL,external,SCRATCH};
static void set32(uint32_t at,uint32_t v) { mem[at]=(uint8_t)v;mem[at+1]=(uint8_t)(v>>8);mem[at+2]=(uint8_t)(v>>16);mem[at+3]=(uint8_t)(v>>24); }
static void reset(void) { memset(mem,0,sizeof mem);log_[0]=0;scratch_word=0x11223344; }
static int same(const char *expected) { if(strcmp(log_,expected)){ fprintf(stderr,"log [%s]\nexpected [%s]\n",log_,expected);return 0; } return 1; }
int main(void)
{
    unsigned cases=0;
    char e[400];
    const struct { uint32_t (*f)(uint32_t,uint32_t,const struct px_battle_mid_3_api*); uint32_t off; } t[]={
        {px_battle_mid_1088b2e8,0xe8},{px_battle_mid_1088e858,0xe8},{px_battle_mid_10898a04,0xe8},{px_battle_mid_108993b8,0xe8},
        {px_battle_mid_1088bcb8,0xec},{px_battle_mid_1088db3c,0xd8},{px_battle_mid_1088f210,0xe0},{px_battle_mid_1088fa38,0xe0},
        {px_battle_mid_10890590,0xe0},{px_battle_mid_1089c5dc,0xe0},{px_battle_mid_108910d4,0xf4},{px_battle_mid_10899ff4,0xdc},{px_battle_mid_1089a9b0,0x118}};
    for(unsigned i=0;i<13;++i)for(uint32_t set=0;set<2;++set)for(uint32_t arg=0;arg<3;++arg){
        reset();const uint32_t h=0x4000+i;
        if(set) { set32(OBJ+t[i].off,h); }
        /* the handles at every other offset are set, to catch a wrong offset */
        for(unsigned k=0;k<13;++k) { if(k!=i&&t[k].off!=t[i].off) { set32(OBJ+t[k].off,0x7000+k); } }
        const uint32_t argument=arg==0?0:arg==1?1:0xdeadbeef;
        const uint32_t got=t[i].f(OBJ,argument,&api);
        if(!set) { CHECK(log_[0]==0&&got==0); }
        else { snprintf(e,sizeof e,"c1080085c(%x,%x,0,0,0) c108568ec(%x,0,0,0,0) ",h,argument,argument);CHECK(same(e)&&got==0x1000000u+(argument&0xffff)+0xec); }
        ++cases;
    }
    /* 1088a340. */
    reset();set32(OBJ+0xe8,0x5555);set32(OBJ+0xe4,0x6666);
    CHECK(px_battle_mid_1088a340(OBJ,&api)==0x1000000u+OBJ+0x5c&&same("c108019f4(5555,0,0,0,0) c1088a35c(1000,0,0,0,0) "));++cases;
    /* 1088d064: two levels of pointer. */
    reset();set32(OBJ+0x88,OTHER);set32(OTHER+0x118,0xabcd);set32(OBJ+0x118,0x1111);
    CHECK(px_battle_mid_1088d064(OBJ,&api)==0x1000000u+0xabcd+0x2c&&same("c10801a2c(abcd,0,0,0,0) "));++cases;
    /* 1088d98c: the signed byte at +0xd5. */
    for(uint32_t b=0;b<256;b+=51){ reset();set32(OBJ+0xe0,0x7a7a);mem[OBJ+0xd5]=(uint8_t)b;
        const uint32_t got=px_battle_mid_1088d98c(OBJ,&api);
        snprintf(e,sizeof e,"c10801c0c(7a7a,%x,0,0,0) c108019f4(7a7a,0,0,0,0) c1088d9b4(1000,0,0,0,0) ",(uint32_t)(int32_t)(int8_t)b);
        CHECK(same(e)&&got==0x1000000u+OBJ+0xb4);++cases; }
    reset();mem[OBJ+0xd5]=0x80;set32(OBJ+0xe0,1);px_battle_mid_1088d98c(OBJ,&api);CHECK(strstr(log_,"c10801c0c(1,ffffff80,")!=NULL);++cases;
    /* The three accessors with a stack buffer. */
    for(uint32_t a=0;a<3;++a){ const uint32_t av=a==0?0:a==1?5:0x123456,bv=a==0?9:a==1?0:0xfedcba;
        reset();CHECK(px_battle_mid_10889aac(av,bv,&api)==0x1000000u+(bv&0xffff)+0x28);
        snprintf(e,sizeof e,"c10800504(%x,%x,1,0,0) c1083ac28(%x,0,0,0,0) ",av,SCRATCH,bv);CHECK(same(e));++cases;
        reset();CHECK(px_battle_mid_10886734(av,bv,&api)==0x1000000u+(bv&0xffff)+0xb8);
        snprintf(e,sizeof e,"c10800504(%x,%x,1,0,0) c1083c0b8(%x,11223344,0,0,0) ",av,SCRATCH,bv);CHECK(same(e));++cases;
        reset();CHECK(px_battle_mid_10886bcc(av,bv,&api)==0x1000000u+(bv&0xffff)+0xc8);
        snprintf(e,sizeof e,"c10801ab4(%x,%x,1,0,0) c108759c8(%x,%x,0,0,0) ",av,SCRATCH,bv,SCRATCH);CHECK(same(e));++cases; }
    printf("battle_mid_3: %u cases, all tests passed\n",cases);
    return 0;
}
