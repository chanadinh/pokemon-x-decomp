#include "battle_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x4000, CHART=0x1000, TABLE=0x2000, RULES=0x3000 };
static uint8_t mem[SIZE];
static unsigned loads,aborts;
static char log_[256];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);++loads;char b[16];snprintf(b,sizeof b,"b%x ",a);note(b);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { (void)u;(void)a;CHECK(0);return 0; }
static uint32_t r32(void *u,uint32_t a)
{
    (void)u;CHECK(a+3<SIZE);++loads;char b[16];snprintf(b,sizeof b,"w%x ",a);note(b);
    return (uint32_t)mem[a]|(uint32_t)mem[a+1]<<8|(uint32_t)mem[a+2]<<16|(uint32_t)mem[a+3]<<24;
}
static void w32(uint32_t a,uint32_t v) { for(unsigned i=0;i<4;++i)mem[a+i]=(uint8_t)(v>>8*i); }
static void abort_battle(void *u) { (void)u;note("ABORT ");++aborts; }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32};
static const struct px_battle_types_api api={&memory,NULL,abort_battle,CHART,TABLE,RULES};
static void reset(void) { loads=aborts=0;log_[0]=0; }
static void defaults(void)
{
    memset(mem,0,sizeof mem);reset();
    for(unsigned i=0;i<18*18;++i)mem[CHART+i]=(uint8_t)(i%7==0?0:i%5==0?2:i%3==0?8:4);
    for(unsigned i=0;i<14;++i)w32(TABLE+4*i,i?UINT32_C(1)<<(i-1):0);
}
/* Independent rules. */
static uint32_t expect_code(uint8_t chart,int inverse)
{
    const uint32_t normal[9]={0,99,6,99,7,99,99,99,8},inv[9]={8,99,8,99,7,99,99,99,6};
    return (inverse?inv:normal)[chart];
}
static double factor(uint32_t code) { return code==0?0.0:code<7?1.0/(1u<<(7-code)):(double)(1u<<(code-7)); }
int main(void)
{
    unsigned cases=0;
    /* Chart lookup: every pair, normal and inverse (including negative flag bytes). */
    const uint8_t flags[4]={0,1,0x80,0xff};
    for(unsigned f=0;f<4;++f)for(uint32_t a=0;a<18;++a)for(uint32_t d=0;d<18;++d) {
        defaults();mem[RULES+1]=flags[f];mem[RULES]=0xff;mem[RULES+2]=0xff;
        const uint32_t got=px_battle_type_effectiveness(a,d,&api);
        CHECK(got==expect_code(mem[CHART+a*18+d],flags[f]!=0)&&aborts==0);
        char want[32];snprintf(want,sizeof want,"b%x b%x ",CHART+a*18+d,RULES+1);CHECK(!strcmp(log_,want));++cases;
    }
    /* Typeless on either side is neutral without any read, even for unknown types. */
    for(uint32_t t=0;t<0x40;++t) {
        defaults();mem[RULES+1]=1;
        CHECK(px_battle_type_effectiveness(PX_BATTLE_TYPELESS,t,&api)==7&&loads==0);
        CHECK(px_battle_type_effectiveness(t,PX_BATTLE_TYPELESS,&api)==7&&loads==0);++cases;
    }
    /* Chart values other than 0, 2, 4 and 8 abort (which returns) and give 0. */
    for(unsigned v=0;v<256;++v) {
        if(v==0||v==2||v==4||v==8)continue;
        for(unsigned f=0;f<2;++f) {
            defaults();mem[CHART+3*18+5]=(uint8_t)v;mem[RULES+1]=(uint8_t)f;
            CHECK(px_battle_type_effectiveness(3,5,&api)==0&&aborts==1&&loads==2);++cases;
        }
    }
    /* Indices are unchecked 32-bit offsets. */
    defaults();mem[CHART+20*18+1]=8;CHECK(px_battle_type_effectiveness(20,1,&api)==8);++cases;
    defaults();mem[CHART+18*18]=2;CHECK(px_battle_type_effectiveness(0,18*18,&api)==6);++cases;
    defaults();mem[CHART-1]=8;CHECK(px_battle_type_effectiveness(0,UINT32_C(0xffffffff),&api)==8);++cases;
    /* Combination: every pair against the factor product, clamped to x4096/64 and x1/64. */
    for(uint32_t a=0;a<14;++a)for(uint32_t b=0;b<14;++b) {
        defaults();
        const uint32_t got=px_battle_combine_effectiveness(a,b,&api);
        uint32_t want;
        if(a==0||b==0)want=0;
        else { int sum=(int)a+(int)b-7;want=sum<1?1:sum>13?13:(uint32_t)sum; }
        CHECK(got==want);CHECK(factor(want)==(a==0||b==0?0.0:factor(a)*factor(b)<factor(1)?factor(1):factor(a)*factor(b)>factor(13)?factor(13):factor(a)*factor(b)));
        CHECK(aborts==((a&&b&&((int)a+(int)b-7<1||(int)a+(int)b-7>13))?1u:0u));
        char want_log[32];snprintf(want_log,sizeof want_log,"w%x w%x %s",TABLE+4*a,TABLE+4*b,aborts?"ABORT ":"");
        CHECK(!strcmp(log_,want_log));++cases;
    }
    /* Out-of-range codes abort and still read past the table (which returns). */
    defaults();w32(TABLE+4*14,0x40);w32(TABLE+4*20,0x80);
    CHECK(px_battle_combine_effectiveness(14,20,&api)==8&&aborts==2);
    CHECK(!strcmp(log_,"ABORT ABORT w2038 w2050 "));++cases;
    defaults();w32(TABLE+4*15,0x40);CHECK(px_battle_combine_effectiveness(15,7,&api)==7&&aborts==1);++cases;
    defaults();w32(TABLE+4*15,0x40);CHECK(px_battle_combine_effectiveness(7,15,&api)==7&&aborts==1);++cases;
    /* Product uses unsigned 32-bit multiplication and the lowest set bit. */
    defaults();w32(TABLE+4,0x10000);w32(TABLE+8,0x10000);
    CHECK(px_battle_combine_effectiveness(1,2,&api)==1&&aborts==1);++cases;   /* wraps to 0: forced to 1 */
    defaults();w32(TABLE+4,0x80000000);w32(TABLE+8,3);
    CHECK(px_battle_combine_effectiveness(1,2,&api)==13&&aborts==1);++cases;  /* wraps to 0x80000000: clamped */
    defaults();w32(TABLE+4,0xc0);w32(TABLE+8,1);
    CHECK(px_battle_combine_effectiveness(1,2,&api)==1&&aborts==0);++cases;   /* 3 -> bit 0 */
    defaults();w32(TABLE+4,0x1000);w32(TABLE+8,0x40);
    CHECK(px_battle_combine_effectiveness(1,2,&api)==13&&aborts==0);++cases;  /* exactly 0x1000 */
    defaults();w32(TABLE+4,0x1001);w32(TABLE+8,0x40);
    CHECK(px_battle_combine_effectiveness(1,2,&api)==13&&aborts==1);++cases;  /* above: clamped */
    defaults();w32(TABLE+4,0x800);w32(TABLE+8,0x40);
    CHECK(px_battle_combine_effectiveness(1,2,&api)==12&&aborts==0);++cases;
    defaults();CHECK(px_battle_combine_effectiveness(0,0,&api)==0&&aborts==0);++cases;
    defaults();w32(TABLE,5);CHECK(px_battle_combine_effectiveness(0,1,&api)==0&&aborts==0);++cases; /* code 0 skips the minimum */
    defaults();w32(TABLE,0x40);CHECK(px_battle_combine_effectiveness(0,0,&api)==7&&aborts==0);++cases;
    /* Application: every code and a range of damages. */
    const uint32_t damages[]={0,1,2,3,63,64,65,127,1000,0xffff,0x10000,0x02000000,0x7fffffff,0xffffffff};
    for(unsigned i=0;i<sizeof damages/sizeof *damages;++i)for(uint32_t code=0;code<0x40;++code) {
        defaults();
        const uint32_t d=damages[i],got=px_battle_apply_effectiveness(d,code,&api);
        uint32_t want=code==0?0:code<7?d>>(7-code):code<14?(uint32_t)((uint64_t)d<<(code-7)):d;
        CHECK(got==want&&aborts==(code>=14)&&loads==0);++cases;
    }
    defaults();CHECK(px_battle_apply_effectiveness(50,UINT32_C(0xffffffff),&api)==50&&aborts==1);++cases;
    defaults();CHECK(px_battle_apply_effectiveness(50,UINT32_C(0x80000007),&api)==50&&aborts==1);++cases;
    /* Pipeline example: Fire vs Grass/Steel (x2 x2) on 25 damage. */
    defaults();mem[CHART+9*18+11]=8;mem[CHART+9*18+8]=8;
    const uint32_t c=px_battle_combine_effectiveness(px_battle_type_effectiveness(9,11,&api),px_battle_type_effectiveness(9,8,&api),&api);
    CHECK(c==9&&px_battle_apply_effectiveness(25,c,&api)==100);++cases;
    printf("battle_types: %u cases, all tests passed\n",cases);
    return 0;
}
