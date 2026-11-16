#include "battle_abilities_13.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, HANDLER=0x6000,
       WORK=HANDLER+0x1c, RECORD=0x7000, SPEEDS=0x7400, ME=2, OTHER=5 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static uint32_t speeds[8],ally_mask,blocked_mask;
static unsigned aborts,ncommit;static uint8_t commits[4][0x20];static char log_[2048];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static const struct px_battle_ability_api base={&events,NULL,NULL,NULL,NULL,NULL,NULL};
static uint32_t speed(void *u,uint32_t c,uint32_t p,uint32_t f) { (void)u;CHECK(c==CTX&&f==1);notef("spd%u ",mon(p),0);return speeds[mon(p)]; }
static uint32_t is_ally(void *u,uint32_t id,uint32_t o) { (void)u;notef("ally%u/%u ",id,o);CHECK(o<8);return ally_mask>>o&1; }
static uint32_t steal_blocked(void *u,uint32_t c,uint32_t t,uint32_t v) { (void)u;CHECK(c==CTX);notef("sb%u/%u ",t,v);return blocked_mask>>v&1; }
static void remove_handler(void *u,uint32_t h) { (void)u;CHECK(h==HANDLER);note("remove "); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);
    memset(mem+RECORD,0xa5,0x20);w32(NULL,RECORD,0x11);return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;notef("msg%x/%x ",id,(r-RECORD)*16+k); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;(void)r;notef("arg%x ",v,0); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD&&ncommit<4);memcpy(commits[ncommit++],mem+RECORD,0x20);note("commit "); }
static const struct px_battle_abilities_13_api api={&base,speed,is_ally,steal_blocked,remove_handler,action_record,message,message_arg,commit,SPEEDS};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_abilities_13_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_magician,px_battle_scrappy_unhook};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(speeds,0,sizeof speeds);memset(commits,0,sizeof commits);aborts=ncommit=0;log_[0]=0;ally_mask=blocked_mask=0;
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
static void frame(uint32_t k3,uint32_t k4e,uint32_t n,const uint32_t *targets)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x03,k3,&events);px_battle_event_set_value(0x4e,k4e,&events);px_battle_event_set_value(0x05,n,&events);
    for(unsigned i=0;i<6;++i)px_battle_event_set_id(0x06+i,targets[i],&events);
    log_[0]=0;
}
static void cat(char *w,size_t n,const char *f,uint32_t a,uint32_t b) { size_t l=strlen(w);snprintf(w+l,n-l,f,a,b); }
#define EXPECT(w) do { if(strcmp(log_,w)){fprintf(stderr,"got  %s\nwant %s\n",log_,w);CHECK(0);} } while (0)
static void work_cleared(void) { for(unsigned o=0x1c;o<=0x34;o+=4)CHECK(r32(NULL,HANDLER+o)==0);CHECK(r32(NULL,HANDLER+0x18)==0x99&&r32(NULL,HANDLER+0x38)==0x77); }
int main(void)
{
    unsigned cases=0;char want[2048];
    /* The early returns. */
    for(unsigned bits=0;bits<8;++bits) {
        static const uint32_t t[6]={3,4,5,6,7,1};
        reset();w32(NULL,HANDLER+0x18,0x99);w32(NULL,HANDLER+0x38,0x77);w16(NULL,MON+0x200+0x12,bits&1?0x40:0);w32(NULL,WORK,bits&2?1:0);w32(NULL,HANDLER+0x34,0x55);
        frame(ME,bits&4?0x100:0,0,t);px_battle_magician(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0&&log_[0]==0);
        if(bits==0)work_cleared();else CHECK(r32(NULL,HANDLER+0x34)==0x55);
        reset();w32(NULL,HANDLER+0x34,0x55);frame(OTHER,0,0,t);px_battle_magician(HANDLER,CTX,ME,WORK,&api);CHECK(log_[0]==0&&r32(NULL,HANDLER+0x34)==0x55);cases+=2;
    }
    /* Targets sorted slowest first (allies +5), then the first holding a stealable item is robbed. */
    for(uint32_t n=1;n<=5;++n)for(unsigned sp=0;sp<4;++sp)for(unsigned held=0;held<4;++held)for(unsigned bl=0;bl<3;++bl)for(unsigned al=0;al<2;++al) {
        static const uint32_t ids[6]={3,4,5,6,7,1};static const uint32_t pat[4][6]={{10,20,30,40,50,60},{60,50,40,30,20,10},{7,7,3,9,3,0xfffe},{0x10,0xb,0x10,0x6,0x0,0x1}};
        reset();w32(NULL,HANDLER+0x18,0x99);w32(NULL,HANDLER+0x38,0x77);
        for(unsigned i=0;i<6;++i){speeds[ids[i]]=pat[sp][i];w16(NULL,MON+0x100*ids[i]+0x12,(uint16_t)((held>>(i&1)&1)&&i!=2?0x200+ids[i]:0));}
        blocked_mask=bl==0?0:bl==1?1u<<4|1u<<6:0xff;ally_mask=al?1u<<5|1u<<7:0;
        uint32_t targets[6];for(unsigned i=0;i<6;++i)targets[i]=0x100+ids[i];
        frame(ME,0,n,targets);px_battle_magician(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0);
        /* Model. */
        uint32_t order[6],key[6];want[0]=0;
        for(unsigned i=0;i<n;++i){order[i]=ids[i];}
        for(unsigned i=0;i<n;++i){cat(want,sizeof want,"spd%u ",order[i],0);key[i]=pat[sp][i]&0xffff;cat(want,sizeof want,"ally2/%u ",order[i],0);if(ally_mask>>order[i]&1)key[i]=(key[i]+5)&0xffff;}
        for(unsigned i=0;i<n;++i)for(unsigned j=i+1;j<n;++j)if(key[j]<key[i]){uint32_t t=key[i];key[i]=key[j];key[j]=t;t=order[i];order[i]=order[j];order[j]=t;}
        int robbed=-1;
        for(unsigned i=0;i<n;++i){const uint32_t item=r16(NULL,MON+0x100*order[i]+0x12);if(!item)continue;cat(want,sizeof want,"sb2/%u ",order[i],0);
            if(blocked_mask>>order[i]&1){continue;}
            cat(want,sizeof want,"rec25/2 msg427/82 arg2 arg%x arg%x commit ",order[i],item);robbed=(int)i;break;}
        EXPECT(want);
        if(robbed>=0)CHECK(commits[0][4]==order[robbed]&&(uint32_t)(commits[0][0]|commits[0][2]<<16)==0x800011);else CHECK(ncommit==0);
        for(unsigned i=0;i<n;++i)CHECK(r16(NULL,SPEEDS+2*i)==key[i]);
        work_cleared();++cases;
    }
    /* Six targets or more: the fault, no sorting, then the theft loop over all of them. */
    {
        static const uint32_t targets[6]={0x103,0x104,0x105,0x106,0x107,0x101};
        reset();w32(NULL,HANDLER+0x18,0x99);w32(NULL,HANDLER+0x38,0x77);w16(NULL,MON+0x100*1+0x12,0x31);
        frame(ME,0,6,targets);px_battle_magician(HANDLER,CTX,ME,WORK,&api);
        EXPECT("ABORT sb2/1 rec25/2 msg427/82 arg2 arg1 arg31 commit ");CHECK(aborts==1&&r16(NULL,SPEEDS)==0);work_cleared();++cases;
    }
    /* The helper on its own. */
    reset();for(unsigned o=0x18;o<=0x38;o+=4)w32(NULL,HANDLER+o,0xabcd0000u+o);px_battle_clear_work(HANDLER,&api);
    for(unsigned o=0x1c;o<=0x34;o+=4){CHECK(r32(NULL,HANDLER+o)==0);}
    CHECK(r32(NULL,HANDLER+0x18)==0xabcd0018u&&r32(NULL,HANDLER+0x38)==0xabcd0038u);++cases;
    /* Scrappy (event 0x04): removed once its own-move mark (work +0x10) is set; the -ate reset (event 0x91). */
    for(uint32_t w=0;w<3;++w) {
        static const uint32_t t[6]={0};
        reset();w32(NULL,WORK+0x10,w==2?0x10000:w);frame(OTHER,0,0,t);px_battle_scrappy_unhook(HANDLER,CTX,ME,WORK,&api);EXPECT(w?"remove ":"");
        reset();w32(NULL,WORK,0xab1234+w);w32(NULL,WORK+4,0x55);frame(OTHER,0,0,t);px_battle_ate_reset(HANDLER,CTX,ME,WORK,&api);
        CHECK(log_[0]==0&&r32(NULL,WORK)==0&&r32(NULL,WORK+4)==0x55);cases+=2;
    }
    /* Through the dispatcher: Magician on event 0x8c. */
    {
        static const uint32_t targets[6]={0x104,0x105,0,0,0,0};
        reset();speeds[4]=9;speeds[5]=3;w16(NULL,MON+0x400+0x12,0x44);w16(NULL,MON+0x500+0x12,0x45);
        const uint32_t entries=0x7c00;w8(NULL,entries,0x8c);w32(NULL,entries+4,0);
        w32(NULL,HANDLER+8,entries);w32(NULL,HANDLER+0x18,0x20000000u|1u<<16);w8(NULL,HANDLER+0x3a,ME);w32(NULL,BATTLE+0x60,HANDLER);
        frame(ME,0,2,targets);px_battle_event_dispatch(CTX,0x8c,&events);
        EXPECT("spd4 ally2/4 spd5 ally2/5 sb2/5 rec25/2 msg427/82 arg2 arg5 arg45 commit ");CHECK(aborts==0);++cases;
    }
    printf("battle_abilities_13: %u cases, all tests passed\n",cases);
    return 0;
}
