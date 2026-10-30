#include "battle_matchup.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, CTX=0x100, TABLE=0x5000, ATTACKER=0x6000, DEFENDER=0x6400, OTHER=0x6800, MOVE=0x7000,
       FIELD=0x7100, SCRATCH=0x7200, CHART=0x8000, POWERS=0x8200, RULES=0x8300, TARGET=0x1234 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
/* Scenario: handler results per event and key; other reads return the set value. */
struct scenario { int set[256][128];uint32_t over[256][128];uint32_t values[128];unsigned event,depth,max_depth,aborts;
                  uint32_t airborne[2],replacement,flags_after_1b,message_calls,airborne_calls;char log[4096]; };
static struct scenario sc;
static void note(const char *s) { strncat(sc.log,s,sizeof sc.log-strlen(sc.log)-1); }
static void push(void *u) { (void)u;note("(");if(++sc.depth>sc.max_depth)sc.max_depth=sc.depth;memset(sc.values,0,sizeof sc.values);sc.event=0; }
static void pop(void *u) { (void)u;note(")");CHECK(sc.depth>0);--sc.depth; }
static void set_id(void *u,uint32_t k,uint32_t v) { (void)u;char b[24];snprintf(b,sizeof b,"i%x=%x ",k,v);note(b);CHECK(k<128);sc.values[k]=v; }
static void set_value(void *u,uint32_t k,uint32_t v) { (void)u;char b[24];snprintf(b,sizeof b,"v%x=%x ",k,v);note(b);CHECK(k<128);sc.values[k]=v; }
static void dispatch(void *u,uint32_t c,uint32_t e) { (void)u;CHECK(c==CTX&&e<256);char b[16];snprintf(b,sizeof b,"E%x ",e);note(b);sc.event=e;
    if(e==0x1b)w32(NULL,CTX+0x437c,sc.flags_after_1b); }
static uint32_t get(void *u,uint32_t k)
{
    (void)u;char b[16];snprintf(b,sizeof b,"g%x ",k);note(b);CHECK(k<128&&sc.depth>0);
    return sc.set[sc.event][k]?sc.over[sc.event][k]:sc.values[k];
}
static void handler(uint32_t event,uint32_t key,uint32_t value) { sc.set[event][key]=1;sc.over[event][key]=value; }
static void type_state(void *u,uint32_t d) { (void)u;CHECK(d==DEFENDER||d==OTHER);note("T "); }
static uint32_t airborne(void *u,uint32_t c,uint32_t p,uint32_t f) { (void)u;CHECK(c==CTX&&(p==DEFENDER||p==OTHER)&&f<2);char b[8];snprintf(b,sizeof b,"A%u ",f);note(b);++sc.airborne_calls;return sc.airborne[f]; }
static uint32_t message_state(void *u,uint32_t s) { (void)u;CHECK(s==CTX+0x437c);note("S ");return 0x5a5a; }
static void message(void *u,uint32_t t,uint32_t g,uint32_t id,uint32_t p,uint32_t x)
{ (void)u;CHECK(t==TARGET&&g==0x6a&&id==0xd2&&x==0xffff0000u);char b[16];snprintf(b,sizeof b,"M%x ",p);note(b);++sc.message_calls; }
static uint32_t replacement(void *u,uint32_t o,uint32_t t,uint32_t p) { (void)u;CHECK(o==0x77&&t==TABLE&&p==DEFENDER);note("R ");return sc.replacement; }
static void load_move(void *u,uint32_t c,uint32_t m,uint32_t a,uint32_t out)
{ (void)u;CHECK(c==CTX&&m==123&&(a==ATTACKER||a==0)&&out==SCRATCH);note("L ");w8(NULL,SCRATCH+6,r8(NULL,MOVE+6)); }
static void abort_battle(void *u) { (void)u;note("ABORT ");++sc.aborts; }
static const struct px_battle_types_api types={&memory,NULL,abort_battle,CHART,POWERS,RULES};
static const struct px_battle_matchup_api api={&memory,NULL,&types,push,pop,set_id,set_value,dispatch,get,
                                               type_state,airborne,message_state,message,replacement,load_move,SCRATCH};
/* Synthetic chart: attack type a against defend type d. */
static uint8_t chart_value(uint32_t a,uint32_t d) { return (uint8_t)((a*7+d*3)%11==0?0:(a+d)%4==0?2:(a*d)%5==1?8:4); }
static void pokemon(uint32_t p,uint8_t id,uint8_t t1,uint8_t t2,uint8_t t3) { w8(NULL,p+0x19,id);w8(NULL,p+0x100,t1);w8(NULL,p+0x101,t2);w8(NULL,p+0x102,t3); }
static void defaults(void)
{
    memset(&sc,0,sizeof sc);memset(mem,0,sizeof mem);
    for(uint32_t a=0;a<18;++a)for(uint32_t d=0;d<18;++d)w8(NULL,CHART+a*18+d,chart_value(a,d));
    for(uint32_t i=0;i<14;++i)w32(NULL,POWERS+4*i,i?UINT32_C(1)<<(i-1):0);
    pokemon(ATTACKER,1,0,0,0x12);pokemon(DEFENDER,4,10,10,0x12);pokemon(OTHER,5,3,3,0x12);
    w8(NULL,MOVE+6,9);w32(NULL,CTX+0x1c,FIELD);w32(NULL,CTX+0xc,TARGET);w32(NULL,CTX+4,0x77);w32(NULL,CTX+8,TABLE);
}
static uint32_t code_of(uint8_t v) { return v==0?0:v==2?6:v==4?7:8; }
static uint32_t combine(uint32_t a,uint32_t b) { const int s=(int)a+(int)b-7;return !a||!b?0:s<1?1:s>13?13:(uint32_t)s; }
static unsigned count(const char *s,const char *w) { unsigned n=0;for(const char *p=strstr(s,w);p;p=strstr(p+1,w))++n;return n; }
/* Independent packed-type rule. */
static uint32_t packed(uint32_t t1,uint32_t t2,uint32_t t3,int roost)
{
    if(roost&&t1==2)t1=0x12;
    if(roost&&t2==2)t2=0x12;
    if(t1==0x12){t1=t2==0x12?0:t2;t2=t1;}else if(t2==0x12)t2=t1;
    return (t3&31)<<10|(t2&31)<<5|(t1&31);
}
int main(void)
{
    unsigned cases=0;
    /* Packed types: every type pair, typeless and out-of-range bytes, with and without Roost. */
    const uint32_t roosts[5]={0,1,4,7,8};
    for(uint32_t t1=0;t1<0x14;++t1)for(uint32_t t2=0;t2<0x14;++t2)for(unsigned r=0;r<5;++r)for(uint32_t t3=0x11;t3<0x14;++t3) {
        defaults();pokemon(DEFENDER,4,(uint8_t)t1,(uint8_t)t2,(uint8_t)t3);w32(NULL,DEFENDER+0x7c,roosts[r]);
        CHECK(px_battle_packed_types(DEFENDER,&api)==packed(t1,t2,t3,(roosts[r]&7)!=0));++cases;
    }
    defaults();pokemon(DEFENDER,4,0xff,0xe2,0xf3);CHECK(px_battle_packed_types(DEFENDER,&api)==(0x13u<<10|2u<<5|0x1fu));++cases;
    defaults();pokemon(DEFENDER,4,2,2,0x12);w32(NULL,DEFENDER+0x7c,1);
    CHECK(px_battle_packed_types(DEFENDER,&api)==(0x12u<<10));++cases;   /* roosting pure Flying: Normal */
    /* Single matchup: exact sequence. */
    defaults();CHECK(px_battle_single_matchup(CTX,ATTACKER,DEFENDER,9,11,&api)==code_of(chart_value(9,11)));
    CHECK(!strcmp(sc.log,"(i3=1 i4=4 i15=b i16=9 v4c=0 v4d=0 v39=e v17=12 T E44 g4c g4d g17 g39 )"));CHECK(sc.max_depth==1);++cases;
    /* Every type pair, normal and inverse, and every handler result. */
    for(unsigned inv=0;inv<2;++inv)for(uint32_t a=0;a<18;++a)for(uint32_t d=0;d<18;++d) {
        defaults();w8(NULL,RULES+1,(uint8_t)inv);
        const uint8_t v=chart_value(a,d);const uint32_t want=inv?(v==4?7:v==8?6:8):code_of(v);
        CHECK(px_battle_single_matchup(CTX,ATTACKER,DEFENDER,a,d,&api)==want);++cases;
    }
    for(uint32_t key4c=0;key4c<2;++key4c)for(uint32_t key4d=0;key4d<2;++key4d)for(uint32_t c39=0;c39<0x10;++c39)for(uint32_t extra=0;extra<0x14;extra+=3) {
        defaults();handler(0x44,0x4c,key4c?0x100:0);handler(0x44,0x4d,key4d?0x80000000u:0);
        handler(0x44,0x39,0x300|c39);handler(0x44,0x17,0x200|extra);
        const uint32_t got=px_battle_single_matchup(CTX,ATTACKER,DEFENDER,9,11,&api);
        uint32_t want;
        if(key4d)want=7;
        else {
            want=c39==0xe?code_of(chart_value(9,11)):c39;
            if(extra!=0x12)want=combine(want,extra<18?code_of(chart_value(extra,11)):0);
            if(want==0&&key4c)want=7;
        }
        if(!key4d&&extra!=0x12&&extra>=18)continue;  /* out-of-range chart rows read synthetic memory */
        if(!key4d&&(c39>=0xe&&c39!=0xe))continue;     /* invalid codes abort inside combine */
        CHECK(got==want);++cases;
    }
    defaults();handler(0x44,0x39,0xf);CHECK(px_battle_single_matchup(CTX,ATTACKER,DEFENDER,9,11,&api)==0xf&&sc.aborts==0);++cases; /* unvalidated code */
    defaults();handler(0x44,0x39,0xf);handler(0x44,0x17,3);
    CHECK(px_battle_single_matchup(CTX,ATTACKER,DEFENDER,9,11,&api)==1&&sc.aborts==2);++cases; /* code 0xf aborts, reads zero, then 1 */
    /* Full matchup: typeless move is neutral without any call. */
    defaults();w8(NULL,MOVE+6,0x12);CHECK(px_battle_matchup(CTX,ATTACKER,DEFENDER,MOVE,1,&api)==7&&sc.log[0]==0);++cases;
    /* Every defender type combination for every non-Ground attacking type. */
    for(uint32_t at=0;at<18;++at)for(uint32_t t1=0;t1<0x13;++t1)for(uint32_t t2=0;t2<0x13;++t2)for(uint32_t t3=0x11;t3<0x13;++t3) {
        if(at==4||at==0x12)continue;
        defaults();w8(NULL,MOVE+6,(uint8_t)at);pokemon(DEFENDER,4,(uint8_t)t1,(uint8_t)t2,(uint8_t)t3);
        handler(0x45,0x39,0);
        const uint32_t p=packed(t1,t2,t3,0),f=p&31,s=p>>5&31,t=p>>10&31;
        uint32_t want=code_of(chart_value(at,f)),calls=1;
        if(f!=s||t!=0x12) {
            if(s!=0x12&&s!=f){want=combine(want,code_of(chart_value(at,s)));++calls;}
            if(t!=0x12&&t!=f&&t!=s){want=combine(want,code_of(chart_value(at,t)));++calls;}
        }
        const int immune=want==0;
        const uint32_t got=px_battle_matchup(CTX,ATTACKER,DEFENDER,MOVE,1,&api);
        CHECK(got==want&&count(sc.log,"E44")==calls&&sc.airborne_calls==0);
        CHECK(count(sc.log,"E45")==(unsigned)immune&&sc.message_calls==(unsigned)immune&&sc.aborts==0);++cases;
    }
    /* Exact immune sequence: event 0x45 then the message. */
    defaults();w8(NULL,MOVE+6,0);pokemon(DEFENDER,4,7,7,0x12);w8(NULL,CHART+0*18+7,0);
    CHECK(px_battle_matchup(CTX,ATTACKER,DEFENDER,MOVE,1,&api)==0);
    CHECK(!strcmp(sc.log,"(i3=1 i4=4 i15=7 i16=0 v4c=0 v4d=0 v39=e v17=12 T E44 g4c g4d g17 g39 )(i3=1 i4=4 i16=0 v39=0 E45 g39 )M4 "));++cases;
    defaults();w8(NULL,MOVE+6,0);pokemon(DEFENDER,4,7,7,0x12);w8(NULL,CHART+7,0);
    CHECK(px_battle_matchup(CTX,ATTACKER,DEFENDER,MOVE,0,&api)==0&&sc.message_calls==0);++cases;
    defaults();w8(NULL,MOVE+6,0);pokemon(DEFENDER,4,7,7,0x12);w8(NULL,CHART+7,0);handler(0x45,0x39,0x107);
    CHECK(px_battle_matchup(CTX,ATTACKER,DEFENDER,MOVE,1,&api)==7&&sc.message_calls==0);++cases;
    /* Ground moves: every immunity, field word, airborne, grounding and override result. */
    for(unsigned flying=0;flying<2;++flying)for(unsigned field=0;field<2;++field)for(unsigned air=0;air<2;++air)
    for(unsigned grounded=0;grounded<2;++grounded)for(unsigned over=0;over<2;++over)for(unsigned show=0;show<2;++show)for(unsigned flags=0;flags<4;++flags) {
        defaults();w8(NULL,MOVE+6,4);
        pokemon(DEFENDER,4,(uint8_t)(flying?2:10),(uint8_t)(flying?2:10),0x12);
        w8(NULL,CHART+4*18+2,0);w8(NULL,CHART+4*18+10,8);
        w32(NULL,FIELD+0x27c,field?0x100:0);sc.airborne[0]=sc.airborne[1]=air?2:0;
        handler(0x46,0x42,grounded?0x10000:0);handler(0x45,0x39,over?0x206:0x200);
        sc.flags_after_1b=(flags&1?0x40000000u:0)|(flags&2?0x20000000u:0);w32(NULL,CTX+0x437c,0x1111);
        const uint32_t got=px_battle_matchup(CTX,ATTACKER,DEFENDER,MOVE,show,&api);
        uint32_t want;unsigned e46=0,e45=0,e1b=0,msg=0,air_flag=flying;
        if(!flying) {
            if(field||!air||(e46=1,grounded))want=8;
            else { e45=1;want=over?6:0; }
        } else {
            if(field||!air)want=7;
            else { e45=1;want=over?6:0; }
        }
        if(want==0&&show) { if(!flying){e1b=1;msg=flags!=3;} else msg=1; }
        CHECK(got==want);
        CHECK(count(sc.log,"E46")==e46&&count(sc.log,"E45")==e45&&count(sc.log,"E1b")==e1b&&sc.message_calls==msg);
        CHECK(sc.airborne_calls==(field?0u:1u)&&(field||strstr(sc.log,air_flag?"A1 ":"A0 ")));
        CHECK(r32(NULL,CTX+0x437c)==(e1b?0x5a5au:0x1111u)&&sc.depth==0);++cases;
    }
    /* Exact floating sequence with the message. */
    defaults();w8(NULL,MOVE+6,4);w8(NULL,CHART+4*18+10,8);sc.airborne[0]=1;
    CHECK(px_battle_matchup(CTX,ATTACKER,DEFENDER,MOVE,1,&api)==0);
    CHECK(!strcmp(sc.log,"(i3=1 i4=4 i15=a i16=4 v4c=0 v4d=0 v39=e v17=12 T E44 g4c g4d g17 g39 )A0 (i3=1 i4=4 v42=0 E46 g42 )"
                         "(i3=1 i4=4 i16=4 v39=0 E45 g39 )S (i2=4 E1b )M4 "));++cases;
    /* By index: battlers, replacement, move record, nesting counter and aborts. */
    defaults();w32(NULL,TABLE+0x84+4*2,ATTACKER);w32(NULL,TABLE+0x84+4*5,DEFENDER);w32(NULL,CTX+0x1080,3);
    CHECK(px_battle_matchup_by_index(CTX,2,5,123,&api)==code_of(chart_value(9,10)));
    CHECK(!strncmp(sc.log,"L (i3=1",7)&&r32(NULL,CTX+0x1080)==3&&sc.aborts==0);++cases;
    defaults();w32(NULL,TABLE+0x84+4*2,ATTACKER);w32(NULL,TABLE+0x84+4*5,DEFENDER);w8(NULL,DEFENDER+0x1b,0x40);sc.replacement=OTHER;
    CHECK(px_battle_matchup_by_index(CTX,2,5,123,&api)==code_of(chart_value(9,3)));
    CHECK(!strncmp(sc.log,"R L (i3=1 i4=5",14));++cases;
    defaults();w32(NULL,TABLE+0x84+4*2,ATTACKER);w32(NULL,TABLE+0x84+4*5,DEFENDER);w8(NULL,DEFENDER+0x1b,0xbf);
    CHECK(px_battle_matchup_by_index(CTX,2,5,123,&api)==code_of(chart_value(9,10))&&!strncmp(sc.log,"L ",2));++cases;
    /* Ground immunity through the index path shows no message. */
    defaults();w32(NULL,TABLE+0x84+4*2,ATTACKER);w32(NULL,TABLE+0x84+4*5,DEFENDER);w8(NULL,MOVE+6,4);
    pokemon(DEFENDER,4,2,2,0x12);w8(NULL,CHART+4*18+2,0);sc.airborne[1]=1;
    CHECK(px_battle_matchup_by_index(CTX,2,5,123,&api)==0&&sc.message_calls==0&&strstr(sc.log,"E45"));++cases;
    /* Aborts return: out-of-range index (slot read past the table) and empty slots. */
    defaults();w32(NULL,TABLE+0x84+4*0x18,ATTACKER);w32(NULL,TABLE+0x84+4*5,DEFENDER);
    CHECK(px_battle_matchup_by_index(CTX,0x18,5,123,&api)==code_of(chart_value(9,10))&&sc.aborts==1);++cases;
    defaults();w32(NULL,TABLE+0x84+4*2,ATTACKER);w32(NULL,TABLE+0x84+4*0x20,DEFENDER);
    CHECK(px_battle_matchup_by_index(CTX,2,0x20,123,&api)==code_of(chart_value(9,10))&&sc.aborts==1);++cases;
    defaults();w32(NULL,TABLE+0x84+4*5,DEFENDER);w8(NULL,0x19,9);w8(NULL,0x100,0);
    CHECK(px_battle_matchup_by_index(CTX,2,5,123,&api)==code_of(chart_value(9,10))&&sc.aborts==1&&strstr(sc.log,"i3=9 "));++cases;
    printf("battle_matchup: %u cases, all tests passed\n",cases);
    return 0;
}
