#include "battle_moves_b5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, ME=2, OTHER=5 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
/* Values returned by the external services: bit n of a mask is the answer for battler n. */
static uint32_t hs_mask,ht_mask,tf_value,ov_value,ab_value,pv[8];
static unsigned aborts,ncommit;static uint8_t commits[8][0x40];static char log_[1024];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[64];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;notef("v%u/%x ",mon(p),k);CHECK(k==0x10);return pv[mon(p)]; }
static const struct px_battle_ability_api base={&events,pokemon_value,NULL,NULL,NULL,NULL,NULL};
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;notef("hs%u/%x ",mon(p),s);return hs_mask>>mon(p)&1; }
static uint32_t has_type(void *u,uint32_t p,uint32_t t) { (void)u;notef("ht%u/%x ",mon(p),t);return ht_mask>>mon(p)&1; }
static uint32_t turn_flag(void *u,uint32_t p,uint32_t f) { (void)u;notef("tf%u/%u ",mon(p),f);return tf_value; }
static void header_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("hdr%x/%u ",k,id); }
static uint32_t owner_of(void *u,uint32_t id) { (void)u;notef("ow%u ",id,0);return 7; }
static uint32_t owner_valid(void *u,uint32_t c,uint32_t o) { (void)u;CHECK(c==CTX);notef("ov%u ",o,0);return ov_value; }
static uint32_t trap_condition(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("tc ");return 0x55aa0001u; }
static uint32_t ability_blocked(void *u,uint32_t a) { (void)u;notef("ab%x ",a,0);return ab_value; }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);
    memset(mem+RECORD,0xa5,0x40);w32(NULL,RECORD,0x11);return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;notef("msg%x/%x ",id,(r-RECORD)*16+k); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;notef("arg%x/%x ",v,r-RECORD); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD&&ncommit<8);memcpy(commits[ncommit++],mem+RECORD,0x40);note("commit "); }
static const struct px_battle_moves_b5_api api={&base,has_status,has_type,turn_flag,header_record,owner_of,owner_valid,trap_condition,
    ability_blocked,action_record,message,message_arg,commit};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_b5_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_destiny_bond_effect,px_battle_splash_effect};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(commits,0,sizeof commits);memset(pv,0,sizeof pv);aborts=ncommit=0;log_[0]=0;
    hs_mask=ht_mask=tf_value=ov_value=ab_value=0;
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
static const uint32_t targets[3]={OTHER,1,0x103};
/* Key 0x03 the user, 0x05 the count, 0x06.. the targets, 0x53 a plain value. */
static uint32_t first_override;
static void frame(uint32_t k3,uint32_t count)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x03,k3,&events);px_battle_event_set_value(0x05,count,&events);
    for(unsigned i=0;i<3;++i)px_battle_event_set_id(0x06+i,i==0&&first_override?first_override:targets[i],&events);
    first_override=0;
    px_battle_event_set_value(0x53,9,&events);log_[0]=0;
}
static void run(fn f,uint32_t k3,uint32_t count)
{
    frame(k3,count);f(HANDLER,CTX,ME,WORK,&api);if(aborts)fprintf(stderr,"log %s\n",log_);CHECK(aborts==0);
}
static uint32_t c16(unsigned n,unsigned o) { return (uint32_t)(commits[n][o]|commits[n][o+1]<<8); }
static uint32_t c32(unsigned n,unsigned o) { return c16(n,o)|c16(n,o+2)<<16; }
static void untouched(unsigned n,const unsigned char *keep,unsigned count,uint32_t head)
{
    for(unsigned o=4;o<0x40;++o) {
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
static unsigned low(uint32_t t) { return t&0xff; }
static unsigned monof(uint32_t t) { return t&0xff; }
int main(void)
{
    unsigned cases=0;char want[1024];
    static const uint32_t ids[3]={ME,OTHER,0x100+ME};
    /* Gastro Acid. */
    for(unsigned a=0;a<3;++a) {
        reset();run(px_battle_gastro_acid_effect,ids[a],1);
        snprintf(want,sizeof want,"recb/2 msg235/142 arg%x/14 commit ",low(targets[0]));EXPECT(ids[a]==ME?want:"");
        if(ids[a]==ME){CHECK(commits[0][0xf]==OTHER&&c16(0,4)==0x10&&c32(0,8)==1);UNT(0,0x11,4,5,8,9,10,11,0xf);}
        ++cases;
    }
    /* Celebrate: key 0x53 rewritten, kind 0x3c, the owner message. */
    for(unsigned a=0;a<3;++a)for(unsigned ov=0;ov<3;++ov) {
        reset();ov_value=ov;run(px_battle_celebrate_effect,ids[a],1);const int me=ids[a]==ME;
        snprintf(want,sizeof want,"hdr3c/2 ow2 ov7 rec3/2 msg10b/41 arg%x/4 commit ",ov?7u:0u);EXPECT(me?want:"");
        CHECK(px_battle_event_get(0x53,&events)==(me?0u:9u));
        CHECK(px_battle_event_rewrite(0x53,5,&events)==(me?0u:1u));
        if(me)UNT(0,0x11,4);
        ++cases;
    }
    /* Powder. */
    for(unsigned a=0;a<3;++a)for(unsigned n=0;n<4;++n) {
        reset();run(px_battle_powder_effect,ids[a],n);want[0]=0;
        for(unsigned i=0;i<n&&ids[a]==ME;++i){appendf(want,sizeof want,"recb/2 msg4ba/142 arg%x/14 commit ",low(targets[i]),0);}
        EXPECT(want);CHECK(ncommit==(ids[a]==ME?n:0));
        for(unsigned i=0;i<ncommit;++i){CHECK(commits[i][0xf]==low(targets[i])&&c16(i,4)==0x25&&c32(i,8)==px_battle_turn_count(1)&&c32(i,8)==0xa);UNT(i,0x11,4,5,8,9,10,11,0xf);}
        ++cases;
    }
    /* Splash. */
    for(unsigned a=0;a<3;++a) {
        reset();run(px_battle_splash_effect,ids[a],1);EXPECT(ids[a]==ME?"rec3/2 msg6a/41 commit ":"");
        if(ids[a]==ME)UNT(0,0x11,4);
        ++cases;
    }
    /* Aqua Ring. */
    for(unsigned a=0;a<3;++a)for(unsigned h=0;h<2;++h) {
        reset();hs_mask=h<<ME;run(px_battle_aqua_ring_effect,ids[a],1);
        EXPECT(ids[a]!=ME?"":h?"hs2/23 ":"hs2/23 recb/2 msg259/142 arg2/14 commit ");
        if(ids[a]==ME&&!h){CHECK(c16(0,4)==0x23&&c32(0,8)==1&&commits[0][0xf]==ME);UNT(0,0x11,4,5,8,9,10,11,0xf);}
        ++cases;
    }
    /* Taunt. */
    for(unsigned a=0;a<3;++a)for(unsigned h=0;h<2;++h)for(unsigned tf=0;tf<3;++tf) {
        static const uint32_t flags[3]={0,1,0x100};
        reset();hs_mask=h<<monof(targets[0]);tf_value=flags[tf];run(px_battle_taunt_effect,ids[a],1);
        want[0]=0;if(ids[a]==ME){appendf(want,sizeof want,"hs%u/b ",monof(targets[0]),0);
            if(!h){appendf(want,sizeof want,"tf%u/1 recb/2 commit ",monof(targets[0]),0);}}
        EXPECT(want);
        if(ids[a]==ME&&!h){CHECK(c16(0,4)==0xb&&c32(0,8)==(flags[tf]?0x22u:0x1au)&&commits[0][0xf]==OTHER);UNT(0,0x11,4,5,8,9,10,11,0xf);}
        ++cases;
    }
    /* Spider Web, Mean Look and Block: every iteration looks at key 6. */
    for(unsigned a=0;a<3;++a)for(unsigned n=0;n<4;++n)for(unsigned h=0;h<2;++h)for(unsigned t=0;t<2;++t) {
        reset();hs_mask=h<<monof(targets[0]);ht_mask=t<<monof(targets[0]);run(px_battle_trap_effect,ids[a],n);want[0]=0;
        for(unsigned i=0;i<n&&ids[a]==ME;++i){appendf(want,sizeof want,"hs%u/16 ",monof(targets[0]),0);
            if(!h){appendf(want,sizeof want,"ht%u/7 ",monof(targets[0]),0);
                if(!t)appendf(want,sizeof want,"recb/2 tc msg36b/142 arg%x/14 commit ",low(targets[0]),0);}}
        EXPECT(want);
        for(unsigned i=0;i<ncommit;++i){CHECK(commits[i][0xf]==OTHER&&c16(i,4)==0x16&&c32(i,8)==0x55aa0001u);UNT(i,0x11,4,5,8,9,10,11,0xf);}
        ++cases;
    }
    /* Trick-or-Treat. */
    for(unsigned a=0;a<3;++a)for(unsigned t=0;t<2;++t) {
        reset();ht_mask=t<<monof(targets[0]);run(px_battle_trick_or_treat_effect,ids[a],1);
        EXPECT(ids[a]!=ME?"":t?"ht5/7 ":"ht5/7 rec14/2 commit ");
        if(ids[a]==ME&&!t){CHECK(commits[0][4]==7&&commits[0][5]==OTHER);UNT(0,0x11,4,5);}
        ++cases;
    }
    /* Simple Beam and Entrainment: per target, ability 0x36 is skipped. */
    for(unsigned a=0;a<3;++a)for(unsigned n=0;n<4;++n)for(unsigned v=0;v<3;++v)for(unsigned en=0;en<3;++en) {
        static const uint32_t pvs[3][3]={{0,0,0},{0x36,0,0x36},{0x136,0x36,0}};
        static const uint32_t users[3]={0x1234,0x51234,0x36};
        reset();for(unsigned i=0;i<3;++i)pv[monof(targets[i])]=pvs[v][i];pv[ME]=users[en];
        run(px_battle_simple_beam_effect,ids[a],n);want[0]=0;unsigned exp[3],nexp=0;
        for(unsigned i=0;i<n&&ids[a]==ME;++i){appendf(want,sizeof want,"v%u/10 ",monof(targets[i]),0);
            if(pvs[v][i]!=0x36){appendf(want,sizeof want,"rec20/2 msg195/c2 arg%x/c arg56/c commit ",low(targets[i]),0);exp[nexp++]=i;}}
        EXPECT(want);CHECK(ncommit==nexp);
        for(unsigned i=0;i<ncommit;++i){CHECK(commits[i][6]==low(targets[exp[i]])&&c16(i,4)==0x56);UNT(i,0x11,4,5,6);}
        reset();for(unsigned i=0;i<3;++i)pv[monof(targets[i])]=pvs[v][i];pv[ME]=users[en];ab_value=en==2;
        run(px_battle_entrainment_effect,ids[a],n);want[0]=0;nexp=0;
        if(ids[a]==ME){appendf(want,sizeof want,"v2/10 ab%x ",users[en]&0xffff,0);
            if(!ab_value)for(unsigned i=0;i<n;++i){appendf(want,sizeof want,"v%u/10 ",monof(targets[i]),0);
                if(pvs[v][i]!=0x36){appendf(want,sizeof want,"rec20/2 msg195/c2 arg%x/c ",low(targets[i]),0);appendf(want,sizeof want,"arg%x/c commit ",users[en]&0xffff,0);exp[nexp++]=i;}}}
        EXPECT(want);CHECK(ncommit==nexp);
        for(unsigned i=0;i<ncommit;++i){CHECK(commits[i][6]==low(targets[exp[i]])&&c16(i,4)==(users[en]&0xffff));UNT(i,0x11,4,5,6);}
        cases+=2;
    }
    /* Flower Shield. */
    for(unsigned a=0;a<3;++a)for(unsigned n=0;n<4;++n)for(unsigned m=0;m<8;++m) {
        reset();for(unsigned i=0;i<3;++i)ht_mask|=((m>>i)&1)<<monof(targets[i]);
        run(px_battle_flower_shield_effect,ids[a],n);want[0]=0;unsigned fe[3],nfe=0;
        for(unsigned i=0;i<n&&ids[a]==ME;++i){appendf(want,sizeof want,"ht%u/b ",monof(targets[i]),0);
            if((m>>i)&1){strcat(want,"recd/2 commit ");fe[nfe++]=i;}}
        EXPECT(want);CHECK(ncommit==nfe);
        for(unsigned i=0;i<ncommit;++i){CHECK(commits[i][0xc]==low(targets[fe[i]])&&commits[i][0x16]==1&&commits[i][4]==2&&commits[i][0x17]==1&&commits[i][0x18]==0xa7);UNT(i,0x11,4,0xc,0x16,0x17,0x18);}
        ++cases;
    }
    /* Worry Seed: the first target unmasked. */
    for(unsigned a=0;a<3;++a)for(unsigned t=0;t<2;++t) {
        static const uint32_t firsts[2]={5,0x105};
        reset();first_override=firsts[t];frame(ids[a],1);
        px_battle_worry_seed_effect(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0);
        snprintf(want,sizeof want,"rec20/2 msg195/c2 arg%x/c arg%x/c commit ",firsts[t]&0xff,0xfu);EXPECT(ids[a]==ME?want:"");
        if(ids[a]==ME){CHECK(commits[0][6]==5&&c16(0,4)==0xf);UNT(0,0x11,4,5,6);}
        ++cases;
    }
    /* After You. */
    for(unsigned a=0;a<3;++a)for(unsigned t=0;t<2;++t) {
        static const uint32_t firsts[2]={5,0x105};
        reset();first_override=firsts[t];frame(ids[a],1);
        px_battle_after_you_effect(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0);
        snprintf(want,sizeof want,"rec30/2 msg474/82 arg%x/8 commit ",5u);EXPECT(ids[a]==ME?want:"");
        if(ids[a]==ME){CHECK(commits[0][4]==5);UNT(0,0x11,4);}
        ++cases;
    }
    /* Destiny Bond. */
    for(unsigned a=0;a<3;++a)for(unsigned wi=0;wi<3;++wi) {
        static const uint32_t work_in[3]={0,1,0x80000000u};
        reset();w32(NULL,WORK+0x18,work_in[wi]);run(px_battle_destiny_bond_effect,ids[a],1);const int me=ids[a]==ME;
        EXPECT(me?"rec3/2 msg272/42 arg2/4 commit ":"");CHECK(r32(NULL,WORK+0x18)==(me?1u:work_in[wi]));
        if(me)UNT(0,0x11,4);
        ++cases;
    }
    /* Through the dispatcher: Destiny Bond then Splash on event 0xac. */
    {
        reset();
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7c00+0x10*i;
            w8(NULL,entries,0xac);w32(NULL,entries+4,i);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ME);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);frame(ME,1);px_battle_event_dispatch(CTX,0xac,&events);
        EXPECT("rec3/2 msg272/42 arg2/4 commit rec3/2 msg6a/41 commit ");CHECK(aborts==0&&ncommit==2&&r32(NULL,HANDLER+0x1c+0x18)==1);++cases;
    }
    printf("battle_moves_b5: %u cases, all tests passed\n",cases);
    return 0;
}
