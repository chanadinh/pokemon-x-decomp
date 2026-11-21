#include "battle_mid_2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x4000, OBJ=0x1000 };
static uint8_t mem[SIZE];
static char log_[4096];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v;char t[40];snprintf(t,sizeof t,"w%x=%x ",a-OBJ,v);note(t); }
static void w16(void *u,uint32_t a,uint16_t v) { (void)u;(void)a;(void)v;CHECK(0); }
static void w32(void *u,uint32_t a,uint32_t v) { (void)u;(void)a;(void)v;CHECK(0); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static uint32_t saved_state,advance_ret;
static uint32_t external(void *u,uint32_t f,uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e)
{
    (void)u;char t[120];snprintf(t,sizeof t,"c%x(%x,%x,%x,%x,%x) ",f,a,b,c,d,e);note(t);
    if(f==0x10801c54) return saved_state;
    if(f==0x10801bdc) return advance_ret;
    return 0x1000000u+(a&0xffff);
}
static const struct px_battle_mid_2_api api={&memory,NULL,external};
/* Plain word writes for setup (the module itself stores only bytes). */
static void set32(uint32_t off,uint32_t v) { mem[OBJ+off]=(uint8_t)v;mem[OBJ+off+1]=(uint8_t)(v>>8);mem[OBJ+off+2]=(uint8_t)(v>>16);mem[OBJ+off+3]=(uint8_t)(v>>24); }
static void reset(void)
{
    memset(mem,0xcc,sizeof mem);log_[0]=0;saved_state=0x77;advance_ret=0;
    set32(0xe4,0xe4e4);set32(0xec,0xecec);set32(0xfc,0xfcfc);set32(0xf4,0xf4f4);set32(0xe0,0xe0e0);set32(0xd8,0xd8d8);
}
static int same(const char *expected) { if(strcmp(log_,expected)){ fprintf(stderr,"log [%s]\nexpected [%s]\n",log_,expected);return 0; } return 1; }
int main(void)
{
    unsigned cases=0;
    char e[1024];
    /* Init, variants a and b. */
    {
        void (*const a[])(uint32_t,const struct px_battle_mid_2_api*)={px_battle_mid_10892810,px_battle_mid_1089289c};
        for(unsigned i=0;i<2;++i){ reset();a[i](OBJ,&api);
            CHECK(same("w24=0 wd9=3 c10801574(ecec,0,0,0,0) wf4=0 wf5=3 wf6=41 c1085168c(1000,0,0,0,0) wd9=2 c1080157c(ecec,0,0,0,0) wf4=1 wf5=2 "));++cases; }
        const struct { void (*f)(uint32_t,const struct px_battle_mid_2_api*); uint32_t fn,v; } b[]={
            {px_battle_mid_10893568,0x108511c8,0x41},{px_battle_mid_108935f4,0x108511c8,0x41},{px_battle_mid_1089498c,0x10850cf4,0x5f},{px_battle_mid_10894a18,0x10850cf4,0x5f}};
        for(unsigned i=0;i<4;++i){ reset();b[i].f(OBJ,&api);
            snprintf(e,sizeof e,"w24=0 wd9=3 c10801574(ecec,0,0,0,0) wf4=0 wf5=3 wf6=%x c%x(1000,0,0,0,0) c1080157c(ecec,0,0,0,0) wf4=1 wf5=2 wd9=2 ",b[i].v,b[i].fn);
            CHECK(same(e));++cases; }
    }
    /* Init with the reset, over the saved state. */
    {
        const struct { uint32_t (*f)(uint32_t,const struct px_battle_mid_2_api*); uint32_t fn,v; } r[]={
            {px_battle_mid_10892c34,0x1085168c,0x41},{px_battle_mid_108939f0,0x108511c8,0x41},{px_battle_mid_10894e14,0x10850cf4,0x5f}};
        for(unsigned i=0;i<3;++i)for(uint32_t s=0;s<3;++s){ reset();saved_state=s==0?0:s==1?0x77:0x12345;const uint32_t got=r[i].f(OBJ,&api);
            snprintf(e,sizeof e,"w24=0 wd9=3 c10801574(ecec,0,0,0,0) wf4=0 wf5=3 wf6=%x c%x(1000,0,0,0,0) c10801c54(e4e4,0,0,0,0) c1080085c(e4e4,0,0,0,0) c10801c1c(e4e4,0,0,0,0) c1080085c(e4e4,%x,0,0,0) ",
                     r[i].v,r[i].fn,saved_state);
            CHECK(same(e)&&got==0x100e4e4u);++cases; }
    }
    /* Teardown: the handle at its offset, set or clear. */
    {
        const struct { uint32_t (*f)(uint32_t,uint32_t,const struct px_battle_mid_2_api*); uint32_t off; } t[]={
            {px_battle_mid_10892874,0xe4},{px_battle_mid_108935cc,0xe4},{px_battle_mid_108949f0,0xe4},{px_battle_mid_108954a0,0xf4},{px_battle_mid_10896744,0xf4},{px_battle_mid_10897960,0xd8}};
        for(unsigned i=0;i<6;++i)for(uint32_t set=0;set<2;++set)for(uint32_t arg=0;arg<3;++arg){
            reset();set32(t[i].off,set?0x4000+i:0);const uint32_t argument=arg==0?0:arg==1?1:0xdeadbeef;
            const uint32_t got=t[i].f(OBJ,argument,&api);
            if(!set) { CHECK(log_[0]==0&&got==0); }
            else { snprintf(e,sizeof e,"c1080085c(%x,%x,0,0,0) c108568ec(%x,0,0,0,0) ",0x4000+i,argument,argument);CHECK(same(e)&&got==0x1000000u+(argument&0xffff)); }
            ++cases; }
    }
    /* The per-frame updates over every state, frame value, answer, next state and +0xd9. */
    {
        const struct { void (*f)(uint32_t,const struct px_battle_mid_2_api*); uint32_t child,limit; } u[]={
            {px_battle_mid_10892ca4,0x10892900,0x40},{px_battle_mid_10893a60,0x10893658,0x40},{px_battle_mid_10894e84,0x10894a7c,0x5e}};
        for(unsigned i=0;i<3;++i)for(uint32_t state=0;state<4;++state)for(uint32_t fv=0;fv<4;++fv)for(uint32_t ans=0;ans<2;++ans)for(uint32_t next=0;next<4;++next)for(uint32_t d9=2;d9<4;++d9){
            reset();advance_ret=ans?0x55:0;mem[OBJ+0xf4]=(uint8_t)state;mem[OBJ+0xf5]=(uint8_t)next;mem[OBJ+0xd9]=(uint8_t)d9;
            const uint32_t fvals[4]={0,u[i].limit-1,u[i].limit,0xff};mem[OBJ+0xf6]=(uint8_t)fvals[fv];
            u[i].f(OBJ,&api);
            char m[1024]="c108019f4(ecec,0,0,0,0) ";uint32_t st=state;int fin=0;
            (void)fin;
            if(state==0) { char t[80];snprintf(t,sizeof t,"c%x(1000,0,0,0,0) ",u[i].child);strcat(m,t); }
            else if(state==1) {
                int go=1;
                if(fvals[fv]<u[i].limit) { strcat(m,"c10801bdc(1004,e4e4,0,0,0) ");go=ans; }
                if(go) {
                    char t[80];snprintf(t,sizeof t,"c108015ac(1004,e4e4,%x,0,0) ",fvals[fv]);strcat(m,t);
                    if(next==0) { strcat(m,"c10801574(ecec,0,0,0,0) "); }
                    else if(next==1||next==2) { strcat(m,"c1080157c(ecec,0,0,0,0) "); }
                    snprintf(t,sizeof t,"wf4=%x wf5=3 ",next);strcat(m,t);st=next;
                }
            }
            if(st==2) { strcat(m,d9==3?"c10800394(0,0,0,0,0) w24=1 ":"w24=1 "); }
            CHECK(same(m));++cases; }
    }
    /* The late pages. */
    {
        const struct { uint32_t (*f)(uint32_t,const struct px_battle_mid_2_api*); uint32_t fn,limit,result; } p[]={
            {px_battle_mid_10895410,0x10850998,0xf,1},{px_battle_mid_108954c8,0x10850998,0x10,0},{px_battle_mid_1089676c,0x1084fe30,0x10,0}};
        for(unsigned i=0;i<3;++i){ reset();const uint32_t got=p[i].f(OBJ,&api);
            snprintf(e,sizeof e,"w24=0 we3=8 c10801574(fcfc,0,0,0,0) w104=0 w105=3 w106=88 c%x(1000,0,0,0,0) c1080157c(fcfc,0,0,0,0) w104=1 w105=2 we3=7 w106=%x c108015b4(1004,f4f4,%x,1,1) c108568ec(%x,0,0,0,0) ",
                     p[i].fn,p[i].limit,p[i].limit,p[i].result);
            CHECK(same(e)&&got==0x1000000u+p[i].result);++cases; }
    }
    /* The +0xe0 page. */
    reset();{ const uint32_t got=px_battle_mid_10897bf4(OBJ,&api);
        CHECK(same("w24=0 wd5=2 c10801574(e0e0,0,0,0,0) we8=0 we9=3 wea=10 c108015ec(e4e4,0,0,0,0) c108568ec(1,0,0,0,0) c10853d20(d8d8,f,0,0,0) ")&&got==0x100d8d8u);++cases; }
    printf("battle_mid_2: %u cases, all tests passed\n",cases);
    return 0;
}
