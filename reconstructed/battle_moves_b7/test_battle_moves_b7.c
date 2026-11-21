#include "battle_moves_b7.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, LIST0=0x8000, MEM0=0x8100, LIST1=0x8800, MEM1=0x8900, ME=2, MISSING=7 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static uint32_t state_mask,status_mask,partner;
static unsigned aborts,ncommit;static uint8_t commits[4][0x20];static char log_[1024];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[64];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static uint32_t pokemon_status(void *u,uint32_t p) { (void)u;notef("ps%u ",mon(p),0);return status_mask>>mon(p)&1?3:0; }
static const struct px_battle_ability_api base={&events,NULL,NULL,pokemon_status,NULL,NULL,NULL};
static uint32_t side_list(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("sl ");return LIST0; }
static uint32_t partner_list(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("pl%u ",id,0);return partner?LIST1:0; }
static uint32_t list_member(void *u,uint32_t list,uint32_t i) { (void)u;notef("lm%x/",list,0);notef("%u ",i,0);
    return (list==LIST0?MEM0:MEM1)+0x20*i; }
static uint32_t member_state(void *u,uint32_t c,uint32_t b) { (void)u;CHECK(c==CTX);notef("ms%u ",b,0);return state_mask>>b&1?0xa:(b&1?0x10a:5); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);
    memset(mem+RECORD,0xa5,0x20);w32(NULL,RECORD,0x11);return RECORD; }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD&&ncommit<4);memcpy(commits[ncommit++],mem+RECORD,0x20);note("commit "); }
static const struct px_battle_moves_b7_api api={&base,side_list,partner_list,list_member,member_state,action_record,commit};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_b7_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_heal_bell_effect,px_battle_aromatherapy_effect};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static const uint8_t ids0[3]={3,4,6},ids1[2]={5,MISSING};
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(commits,0,sizeof commits);aborts=ncommit=0;log_[0]=0;state_mask=status_mask=partner=0;
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,i==MISSING?0:MON+0x100*i);
    for(unsigned i=0;i<3;++i)w8(NULL,MEM0+0x20*i+0x19,ids0[i]);
    for(unsigned i=0;i<2;++i)w8(NULL,MEM1+0x20*i+0x19,ids1[i]);
}
static void frame(uint32_t k3,uint32_t count)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x03,k3,&events);px_battle_event_set_value(0x05,count,&events);
    px_battle_event_set_id(0x06,4,&events);px_battle_event_set_id(0x07,5,&events);
    px_battle_event_set_value(0x53,9,&events);log_[0]=0;
}
#define EXPECT(w) do { if(strcmp(log_,w)){fprintf(stderr,"got  %s\nwant %s\n",log_,w);CHECK(0);} } while (0)
static void appendf(char *s,size_t n,const char *f,uint32_t a,uint32_t b) { char t[64];snprintf(t,sizeof t,f,a,b);strncat(s,t,n-strlen(s)-1); }
int main(void)
{
    unsigned cases=0;char want[1024];
    static const uint32_t ids[3]={ME,5,0x100+ME};
    static const unsigned sizes0[3]={0,1,3},states[4]={0,0x8,0x58,0xff},statuses[3]={0,0xff,0x28},counts[3]={0,1,2};
    for(unsigned h=0;h<2;++h)for(unsigned a=0;a<3;++a)for(unsigned s0=0;s0<3;++s0)for(unsigned pl=0;pl<3;++pl)
    for(unsigned st=0;st<4;++st)for(unsigned ss=0;ss<3;++ss)for(unsigned n=0;n<3;++n) {
        reset();partner=pl>0;w8(NULL,LIST0+0x18,(uint8_t)sizes0[s0]);w8(NULL,LIST1+0x18,pl==2?2:0);
        state_mask=states[st];status_mask=statuses[ss];frame(ids[a],counts[n]);
        w32(NULL,WORK,0xffffffffu);
        (h?px_battle_aromatherapy_effect:px_battle_heal_bell_effect)(HANDLER,CTX,ME,WORK,&api);
        want[0]=0;uint8_t cured[8];unsigned nc=0;unsigned expected_aborts=0;
        if(ids[a]==ME){
            strcat(want,"sl ");appendf(want,sizeof want,"pl%u ",ME,0);
            for(unsigned l=0;l<(pl?2u:1u);++l){
                const unsigned size=l?(pl==2?2u:0u):sizes0[s0];
                for(unsigned m=0;m<size;++m){
                    const uint32_t b=l?ids1[m]:ids0[m];
                    appendf(want,sizeof want,"lm%x/",l?LIST1:LIST0,0);appendf(want,sizeof want,"%u ",m,0);
                    appendf(want,sizeof want,"ms%u ",b,0);
                    int wanted=((states[st]>>b)&1)!=0;
                    if(!wanted)for(unsigned i=0;i<counts[n];++i)if((i?5u:4u)==b)wanted=1;
                    if(!wanted)continue;
                    if(b==MISSING){strcat(want,"ABORT ");++expected_aborts;continue;}
                    appendf(want,sizeof want,"ps%u ",b,0);
                    if((statuses[ss]>>b)&1)cured[nc++]=(uint8_t)b;
                }
            }
            if(nc)strcat(want,"reca/2 commit ");
        }
        EXPECT(want);CHECK(aborts==expected_aborts);
        CHECK(ncommit==(nc?1u:0u));
        CHECK(px_battle_event_get(0x53,&events)==(ids[a]==ME&&!nc?1u:9u));
        if(nc){
            CHECK(commits[0][0x11]==nc&&commits[0][4]==0x26);
            for(unsigned k=0;k<nc;++k)CHECK(commits[0][5+k]==cured[k]&&r8(NULL,WORK+k)==cured[k]);
            for(unsigned o=5+nc;o<0x20;++o)if(o!=0x11)CHECK(commits[0][o]==0xa5);
            CHECK(commits[0][0]==0x11&&commits[0][1]==0&&commits[0][2]==0&&commits[0][3]==0);
        }
        ++cases;
    }
    /* Through the dispatcher: Heal Bell then Aromatherapy on event 0xac. */
    {
        reset();w8(NULL,LIST0+0x18,1);state_mask=1u<<3;status_mask=1u<<3;
        for(unsigned i=0;i<2;++i) {
            const uint32_t hh=HANDLER+0x40*i,entries=0x7c00+0x10*i;
            w8(NULL,entries,0xac);w32(NULL,entries+4,i);
            w32(NULL,hh+8,entries);w32(NULL,hh+0x18,0x20000000u|1u<<16);w8(NULL,hh+0x3a,ME);
            w32(NULL,hh,i?hh-0x40:0);w32(NULL,hh+4,i<1?hh+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);frame(ME,0);px_battle_event_dispatch(CTX,0xac,&events);
        EXPECT("sl pl2 lm8000/0 ms3 ps3 reca/2 commit sl pl2 lm8000/0 ms3 ps3 reca/2 commit ");
        CHECK(aborts==0&&ncommit==2&&r8(NULL,HANDLER+0x1c)==3&&r8(NULL,HANDLER+0x40+0x1c)==3);++cases;
    }
    printf("battle_moves_b7: %u cases, all tests passed\n",cases);
    return 0;
}
