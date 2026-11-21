#include "battle_moves_b6.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, RSIZE=0x60, ME=2, OTHER=5 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
/* Values returned by the external services. */
static uint32_t pv[8][8],status[8],cnt[3],sw_value,cp_value,mode_value,os_value,sl_val[3],ba_value,sa_value,pf_value,rise_mask,rnd_value,fill18;
static unsigned aborts,ncommit;static uint8_t commits[12][RSIZE];static char log_[1024];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[64];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;notef("v%u/%x ",mon(p),k);CHECK(k<8);return pv[mon(p)][k]; }
static uint32_t pokemon_status(void *u,uint32_t p) { (void)u;notef("ps%u ",mon(p),0);return status[mon(p)]; }
static uint32_t side_of(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("side%u ",id,0);return 0x10+id; }
static const struct px_battle_ability_api base={&events,pokemon_value,NULL,pokemon_status,side_of,NULL,NULL};
static uint32_t side_mask(void *u,uint32_t b) { (void)u;notef("sm%u ",b,0);return 0xc0+b; }
static uint32_t counter(void *u,uint32_t p,uint32_t i) { (void)u;notef("ct%u/%u ",mon(p),i);return cnt[i]; }
static uint32_t can_switch(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("sw%u ",id,0);return sw_value; }
static uint32_t can_pass(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("cp ");return cp_value; }
static uint32_t battle_mode(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("mode ");return mode_value; }
static uint32_t owner_of(void *u,uint32_t id) { (void)u;notef("ow%u ",id,0);return 9; }
static uint32_t owner_state(void *u,uint32_t c,uint32_t o) { (void)u;CHECK(c==CTX);notef("os%u ",o,0);return os_value; }
static uint32_t side_slot(void *u,uint32_t s,uint32_t k) { (void)u;notef("sl%x/",s,0);notef("%u ",k,0);return sl_val[k]; }
static uint32_t battler_at(void *u,uint32_t c,uint32_t slot) { (void)u;CHECK(c==CTX);notef("ba%x ",slot,0);return ba_value; }
static uint32_t side_active(void *u,uint32_t s) { (void)u;notef("sa%x ",s,0);return sa_value; }
static uint32_t status_data(void *u,uint32_t p,uint32_t s) { (void)u;notef("sd%u/",mon(p),0);notef("%x ",s,0);return 0x77; }
static uint32_t poison_flag(void *u,uint32_t d) { (void)u;notef("pf%x ",d,0);return pf_value; }
static uint32_t status_word(void *u,uint32_t s) { (void)u;notef("sx%x ",s,0);return 0x6000+s; }
static uint32_t stat_can_rise(void *u,uint32_t p,uint32_t s,uint32_t n) { (void)u;notef("cr%u/",mon(p),0);notef("%u/",s,0);notef("%u ",n,0);return rise_mask>>s&1; }
static uint32_t random_(void *u,uint32_t n) { (void)u;notef("rn%u ",n,0);return rnd_value%n; }
static void protect_start(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("protect%u ",id,0); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);
    memset(mem+RECORD,0xa5,RSIZE);w32(NULL,RECORD,0x11);if(fill18)mem[RECORD+0x18]=(uint8_t)fill18;return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;notef("msg%x/%x ",id,(r-RECORD)*16+k); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;notef("arg%x/%x ",v,r-RECORD); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD&&ncommit<12);memcpy(commits[ncommit++],mem+RECORD,RSIZE);note("commit "); }
static const struct px_battle_moves_b6_api api={&base,side_mask,counter,can_switch,can_pass,battle_mode,owner_of,owner_state,side_slot,
    battler_at,side_active,status_data,poison_flag,status_word,stat_can_rise,random_,protect_start,action_record,message,message_arg,commit};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_b6_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_spiky_shield_effect,px_battle_defog_effect};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(commits,0,sizeof commits);memset(pv,0,sizeof pv);memset(status,0,sizeof status);memset(cnt,0,sizeof cnt);
    aborts=ncommit=0;log_[0]=0;sw_value=cp_value=mode_value=os_value=ba_value=sa_value=pf_value=rise_mask=rnd_value=fill18=0;memset(sl_val,0,sizeof sl_val);
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
static void frame(uint32_t k3,uint32_t target)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x03,k3,&events);px_battle_event_set_id(0x06,target,&events);
    px_battle_event_set_value(0x53,9,&events);log_[0]=0;
}
static void run(fn f,uint32_t k3,uint32_t target)
{
    frame(k3,target);f(HANDLER,CTX,ME,WORK,&api);if(aborts)fprintf(stderr,"log %s\n",log_);CHECK(aborts==0);
}
static uint32_t c16(unsigned n,unsigned o) { return (uint32_t)(commits[n][o]|commits[n][o+1]<<8); }
static uint32_t c32(unsigned n,unsigned o) { return c16(n,o)|c16(n,o+2)<<16; }
static void untouched(unsigned n,const unsigned char *keep,unsigned count,uint32_t head)
{
    for(unsigned o=4;o<RSIZE;++o) {
        int skip=0;for(unsigned i=0;i<count;++i)if(keep[i]==o)skip=1;
        if(skip)continue;
        if(commits[n][o]!=0xa5){fprintf(stderr,"commit %u byte %x = %x\n",n,o,commits[n][o]);CHECK(0);}
    }
    CHECK(c32(n,0)==head);
}
#define KEEP(...) (const unsigned char[]){__VA_ARGS__}
#define UNT(n,head,...) untouched(n,KEEP(__VA_ARGS__),sizeof KEEP(__VA_ARGS__),head)
#define EXPECT(w) do { if(strcmp(log_,w)){fprintf(stderr,"got  %s\nwant %s\n",log_,w);CHECK(0);} } while (0)
static void appendf(char *s,size_t n,const char *f,uint32_t a,uint32_t b) { char t[64];snprintf(t,sizeof t,f,a,b);strncat(s,t,n-strlen(s)-1); }
int main(void)
{
    unsigned cases=0;char want[1024];
    static const uint32_t ids[3]={ME,OTHER,0x100+ME};
    static const uint32_t targets[3]={OTHER,1,0x103};
    /* Defog. */
    for(unsigned a=0;a<3;++a)for(unsigned t=0;t<3;++t) {
        reset();run(px_battle_defog_effect,ids[a],targets[t]);const uint32_t tg=targets[t]&0xff;
        snprintf(want,sizeof want,"recd/2 commit rec1a/2 sm%u commit rec1a/2 sm2 commit ",tg);EXPECT(ids[a]==ME?want:"");
        if(ids[a]==ME){
            CHECK(commits[0][4]==7&&commits[0][0xc]==tg&&commits[0][0x16]==1&&commits[0][0x17]==0xff&&commits[0][0x18]==0xa7);UNT(0,0x11,4,0xc,0x16,0x17,0x18);
            CHECK(commits[1][4]==4&&commits[1][5]==0xcf&&commits[1][6]==0x41&&commits[1][7]==0&&commits[1][8]==0xc0+tg);UNT(1,0x11,4,5,6,7,8);
            CHECK(commits[2][4]==4&&commits[2][5]==0xc0&&commits[2][6]==0x41&&commits[2][7]==0&&commits[2][8]==0xc2);UNT(2,0x11,4,5,6,7,8);}
        ++cases;
    }
    /* Power Swap. */
    for(unsigned a=0;a<3;++a)for(unsigned t=0;t<2;++t) {
        reset();const uint32_t tg=targets[t]&0xff;
        for(unsigned k=1;k<8;++k){pv[ME][k]=0x10+k;pv[tg][k]=0x20+k;}
        w8(NULL,MON+0x100*ME+0x155,0x91);w8(NULL,MON+0x100*tg+0x155,0x92);
        run(px_battle_power_swap_effect,ids[a],targets[t]);
        snprintf(want,sizeof want,"v2/1 v2/3 rece/2 v%u/1 v%u/3 v2/2 v2/4 v2/5 v2/6 v2/7 commit rece/2 v%u/2 v%u/4 v%u/5 v%u/6 v%u/7 commit rec3/2 msg2a4/42 arg2/4 commit ",tg,tg,tg,tg,tg,tg,tg);
        EXPECT(ids[a]==ME?want:"");
        if(ids[a]==ME){
            CHECK(commits[0][4]==ME&&commits[0][5]==0x20+1&&commits[0][6]==0x12&&commits[0][7]==0x23&&commits[0][8]==0x14&&commits[0][9]==0x15&&commits[0][0xa]==0x16&&commits[0][0xb]==0x17&&commits[0][0xc]==0x91);
            UNT(0,0x11,4,5,6,7,8,9,0xa,0xb,0xc);
            CHECK(commits[1][4]==tg&&commits[1][5]==0x11&&commits[1][6]==0x22&&commits[1][7]==0x13&&commits[1][8]==0x24&&commits[1][9]==0x25&&commits[1][0xa]==0x26&&commits[1][0xb]==0x27&&commits[1][0xc]==0x92);
            UNT(1,0x11,4,5,6,7,8,9,0xa,0xb,0xc);UNT(2,0x11,4);}
        ++cases;
    }
    /* Stockpile. */
    for(unsigned a=0;a<3;++a)for(unsigned n=0;n<6;++n) {
        static const uint32_t counts[6]={0,1,2,3,0x100,0xffffffffu};
        reset();cnt[0]=counts[n];cnt[1]=0x41;cnt[2]=0x1ff;run(px_battle_stockpile_effect,ids[a],OTHER);
        const int on=ids[a]==ME&&counts[n]<3;
        if(ids[a]!=ME)EXPECT("");
        else if(!on)EXPECT("ct2/0 ");
        else{snprintf(want,sizeof want,"ct2/0 rec27/2 commit rec3/2 msg2d1/42 arg2/4 arg%x/4 commit recd/2 commit rec27/2 ct2/1 commit recd/2 commit rec27/2 ct2/2 commit ",counts[n]+1);EXPECT(want);
            CHECK(commits[0][4]==ME&&commits[0][5]==0&&commits[0][6]==((counts[n]+1)&0xff));UNT(0,0x11,4,5,6);UNT(1,0x11,4);
            CHECK(commits[2][4]==2&&commits[2][0x17]==1&&commits[2][0xc]==ME&&commits[2][0x16]==1&&commits[2][0x18]==0xa7);UNT(2,0x11,4,0xc,0x16,0x17,0x18);
            CHECK(commits[3][4]==ME&&commits[3][5]==1&&commits[3][6]==0x42);UNT(3,0x1000011,4,5,6);
            CHECK(commits[4][4]==4&&commits[4][0x17]==1&&commits[4][0xc]==ME&&commits[4][0x18]==0xa7);UNT(4,0x11,4,0xc,0x16,0x17,0x18);
            CHECK(commits[5][4]==ME&&commits[5][5]==2&&commits[5][6]==((0x1ff+1)&0xff));UNT(5,0x1000011,4,5,6);}
        ++cases;
    }
    /* Baton Pass. */
    for(unsigned a=0;a<3;++a)for(unsigned s=0;s<3;++s)for(unsigned c=0;c<3;++c) {
        static const uint32_t flags[3]={0,1,0x100};
        reset();sw_value=flags[s];cp_value=flags[c];run(px_battle_baton_pass_effect,ids[a],OTHER);
        const int me=ids[a]==ME;
        EXPECT(!me?"":!flags[s]?"sw2 ":!flags[c]?"sw2 cp ":"sw2 cp rec1f/2 side2 commit rec17/2 commit rec2a/2 commit ");
        if(me&&flags[s]&&flags[c]){
            CHECK(commits[0][4]==4&&commits[0][5]==0x12&&c32(0,8)==ME&&commits[0][0x18]==1);UNT(0,0x11,4,5,8,9,10,11,0x18);
            CHECK(commits[1][4]==0xe&&commits[1][5]==ME);UNT(1,0x1000011,4,5);
            CHECK(commits[2][0x4c]==ME&&commits[2][0x4d]==1);UNT(2,0x1000011,0x4c,0x4d);}
        ++cases;
    }
    /* Psycho Shift. */
    for(unsigned a=0;a<3;++a)for(unsigned s=0;s<5;++s)for(unsigned ts=0;ts<2;++ts)for(unsigned pf=0;pf<2;++pf)for(unsigned t=0;t<2;++t) {
        static const uint32_t sts[5]={0,1,5,0x105,0x100};
        reset();const uint32_t tg=targets[t]&0xff;status[ME]=sts[s];status[tg]=ts*2;pf_value=pf;run(px_battle_psycho_shift_effect,ids[a],targets[t]);
        want[0]=0;int on=0;
        if(ids[a]==ME){strcat(want,"ps2 ");
            if(sts[s]){appendf(want,sizeof want,"ps%u ",tg,0);
                if(!ts){on=1;appendf(want,sizeof want,"sd2/%x ",sts[s],0);strcat(want,"recb/2 ");
                    if(sts[s]==5){strcat(want,"pf77 ");if(!pf)appendf(want,sizeof want,"sx%x ",sts[s]&0xff,0);}
                    else appendf(want,sizeof want,"sx%x ",sts[s]&0xff,0);
                    strcat(want,"commit reca/2 commit ");}}}
        EXPECT(want);
        CHECK(px_battle_event_get(0x53,&events)==(on?0u:9u));
        if(on){const uint32_t word=(sts[s]==5&&pf)?0x79u:0x6000u+(sts[s]&0xff);
            CHECK(c16(0,4)==(sts[s]&0xffff)&&c32(0,8)==word&&commits[0][0xf]==tg&&commits[0][0xc]==1);UNT(0,0x11,4,5,8,9,10,11,0xc,0xf);
            CHECK(commits[1][0x11]==1&&commits[1][5]==ME&&commits[1][4]==(sts[s]&0xff));UNT(1,0x1000011,4,5,0x11);}
        ++cases;
    }
    /* Ally Switch: the partner lookup by battle mode. */
    for(unsigned md=0;md<4;++md)for(unsigned os=0;os<2;++os)for(unsigned s0=0;s0<2;++s0)for(unsigned ba=0;ba<2;++ba)for(unsigned sa=0;sa<2;++sa) {
        reset();mode_value=md;os_value=os?4:5;sl_val[0]=s0?0x12:0x20;sl_val[1]=0x21;sl_val[2]=0x22;ba_value=ba?ME:7;sa_value=sa;
        run(px_battle_ally_switch_effect,ME,OTHER);
        strcpy(want,"side2 mode ");uint32_t partner=ME;
        if(md==1){strcat(want,"ow2 os9 ");if(os){strcat(want,"sl12/0 ");uint32_t sl=sl_val[0];if(sl==0x12){strcat(want,"sl12/1 ");sl=sl_val[1];}appendf(want,sizeof want,"ba%x ",sl,0);partner=ba_value;}}
        else if(md==2){strcat(want,"sa12 ");if(!sa){strcat(want,"sl12/0 ");uint32_t sl=sl_val[0];if(sl==0x12){strcat(want,"sl12/2 ");sl=sl_val[2];}appendf(want,sizeof want,"ba%x ",sl,0);partner=ba_value;}}
        if(partner!=ME){appendf(want,sizeof want,"rec33/2 msg477/82 arg2/8 arg%x/8 commit ",partner,0);}
        EXPECT(want);
        if(partner!=ME){CHECK(commits[0][4]==ME&&commits[0][5]==partner);UNT(0,0x11,4,5);}
        ++cases;
    }
    /* Acupressure. */
    for(unsigned a=0;a<3;++a)for(unsigned m=0;m<5;++m)for(unsigned r=0;r<5;++r)for(unsigned t=0;t<2;++t) {
        static const uint32_t masks[5]={0,0xfe,0x2a,0x80,0x04};static const uint32_t rnds[5]={0,1,2,5,300};
        static const uint8_t table[7]={1,2,5,3,4,6,7};
        reset();fill18=0xa7;rise_mask=masks[m];rnd_value=rnds[r];run(px_battle_acupressure_effect,ids[a],targets[t]);
        const uint32_t tg=targets[t]&0xff;want[0]=0;unsigned count=0,found=0,stat=0;
        if(ids[a]==ME){
            for(unsigned i=0;i<7;++i){appendf(want,sizeof want,"cr%u/",tg,0);appendf(want,sizeof want,"%u/2 ",table[i],0);if((masks[m]>>table[i])&1)++count;}
            if(count){appendf(want,sizeof want,"rn%u ",count,0);unsigned pick=(rnds[r]%count)&0xff;
                for(unsigned i=0;i<7&&!found;++i){appendf(want,sizeof want,"cr%u/",tg,0);appendf(want,sizeof want,"%u/2 ",table[i],0);
                    if(!((masks[m]>>table[i])&1))continue;
                    if(pick==0){found=1;stat=table[i];strcat(want,"recd/2 commit ");}else--pick;}}}
        EXPECT(want);
        if(found){CHECK(commits[0][0xc]==tg&&commits[0][0x16]==1&&commits[0][0x17]==2&&commits[0][4]==stat&&commits[0][0x18]==0xb5);UNT(0,0x11,4,0xc,0x16,0x17,0x18);}
        ++cases;
    }
    /* Spiky Shield: no user check, protection then work word 6. */
    for(unsigned a=0;a<3;++a)for(unsigned wi=0;wi<2;++wi) {
        reset();w32(NULL,WORK+0x18,wi?0x80000000u:0);run(px_battle_spiky_shield_effect,ids[a],OTHER);
        EXPECT("protect2 ");CHECK(r32(NULL,WORK+0x18)==1);++cases;
    }
    /* Through the dispatcher: Spiky Shield then Defog on event 0xac. */
    {
        reset();
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7c00+0x10*i;
            w8(NULL,entries,0xac);w32(NULL,entries+4,i);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ME);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);frame(ME,OTHER);px_battle_event_dispatch(CTX,0xac,&events);
        EXPECT("protect2 recd/2 commit rec1a/2 sm5 commit rec1a/2 sm2 commit ");CHECK(aborts==0&&ncommit==3&&r32(NULL,HANDLER+0x1c+0x18)==1);++cases;
    }
    printf("battle_moves_b6: %u cases, all tests passed\n",cases);
    return 0;
}
