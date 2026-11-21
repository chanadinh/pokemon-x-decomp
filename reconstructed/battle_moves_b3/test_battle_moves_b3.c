#include "battle_moves_b3.h"
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
/* Values returned by the external services. */
static uint32_t tf_value,bf_value,sw_value,hs_value,lm_value,mu_value,ct_value,ty_value,ab_value,fa_value,oc_value,pv_value,first_word;
static unsigned aborts,ncommit;static uint8_t commits[4][0x40];static char log_[512];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[64];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;notef("v%u/%x ",mon(p),k);return pv_value; }
static uint32_t side_of(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("side%u ",id,0);return 0x1a7; }
static const struct px_battle_ability_api base={&events,pokemon_value,NULL,NULL,side_of,NULL,NULL};
static uint32_t turn_flag(void *u,uint32_t p,uint32_t f) { (void)u;notef("tf%u/%u ",mon(p),f);return tf_value; }
static uint32_t battle_flag(void *u,uint32_t p,uint32_t f) { (void)u;notef("bf%u/%u ",mon(p),f);return bf_value; }
static uint32_t can_switch(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("sw%u ",id,0);return sw_value; }
static uint32_t condition_word(void *u,uint32_t a,uint32_t b) { (void)u;notef("cw%x/%x ",a,b);return 0xa0000000u+a*0x100+b; }
static uint32_t target_condition(void *u,uint32_t id,uint32_t t) { (void)u;notef("tc%u/%u ",id,t);return 0xb0000000u+id*16+t; }
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;notef("hs%u/%x ",mon(p),s);return hs_value; }
static uint32_t last_move(void *u,uint32_t p) { (void)u;notef("lm%u ",mon(p),0);return lm_value; }
static uint32_t move_usable(void *u,uint32_t p,uint32_t m) { (void)u;notef("mu%u/%x ",mon(p),m);return mu_value; }
static uint32_t counter(void *u,uint32_t p,uint32_t i) { (void)u;notef("ct%u/%u ",mon(p),i);return ct_value; }
static uint32_t types(void *u,uint32_t p) { (void)u;notef("ty%u ",mon(p),0);return ty_value; }
static uint32_t raw_stat(void *u,uint32_t p,uint32_t k) { (void)u;notef("rs%u/%u ",mon(p),k);return 0x1230000u+k*0x111; }
static uint32_t ability_blocked(void *u,uint32_t a) { (void)u;notef("ab%x ",a,0);return ab_value; }
static uint32_t fainted(void *u,uint32_t p) { (void)u;notef("fa%u ",mon(p),0);return fa_value; }
static uint32_t owner_of(void *u,uint32_t id) { (void)u;notef("ow%u ",id,0);return 9; }
static uint32_t owner_count(void *u,uint32_t c,uint32_t o) { (void)u;CHECK(c==CTX);notef("oc%x ",o,0);return oc_value; }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);
    memset(mem+RECORD,0xa5,0x40);w32(NULL,RECORD,first_word);return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;notef("msg%x/%x ",id,(r-RECORD)*16+k); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;notef("arg%x/%x ",v,r-RECORD); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD&&ncommit<4);memcpy(commits[ncommit++],mem+RECORD,0x40);note("commit "); }
static const struct px_battle_moves_b3_api api={&base,turn_flag,battle_flag,can_switch,condition_word,target_condition,has_status,
    last_move,move_usable,counter,types,raw_stat,ability_blocked,fainted,owner_of,owner_count,action_record,message,message_arg,commit};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_b3_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_endure_effect,px_battle_focus_energy_effect};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(commits,0,sizeof commits);aborts=ncommit=0;log_[0]=0;
    tf_value=bf_value=sw_value=hs_value=lm_value=mu_value=ct_value=ty_value=ab_value=fa_value=pv_value=0;oc_value=2;first_word=0x11;
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
static void frame(uint32_t k3,uint32_t target)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x03,k3,&events);px_battle_event_set_id(0x06,target,&events);log_[0]=0;
}
static void run(fn f,uint32_t k3,uint32_t target)
{
    frame(k3,target);f(HANDLER,CTX,ME,WORK,&api);if(aborts)fprintf(stderr,"log %s\n",log_);CHECK(aborts==0);
}
static uint32_t c16(unsigned n,unsigned o) { return (uint32_t)(commits[n][o]|commits[n][o+1]<<8); }
static uint32_t c32(unsigned n,unsigned o) { return c16(n,o)|c16(n,o+2)<<16; }
/* Bytes of commit n other than 0xe/0xf and those listed (1-based offsets, as escape digits) keep the 0xa5 fill. */
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
int main(void)
{
    unsigned cases=0;char want[320];
    static const uint32_t ids[3]={ME,OTHER,0x100+ME};
    static const uint32_t targets[3]={OTHER,1,0x103};
    static const uint32_t work_in[3]={0,1,0x80000000u};
    /* Mind Reader and Lock-On. */
    for(unsigned a=0;a<3;++a)for(unsigned t=0;t<3;++t) {
        reset();run(px_battle_mind_reader_effect,ids[a],targets[t]);const uint32_t tg=targets[t]&0xff;
        snprintf(want,sizeof want,"recb/2 tc%u/2 msg28b/142 arg2/14 arg%x/14 commit ",tg,tg);EXPECT(ids[a]==ME?want:"");
        if(ids[a]==ME){CHECK(commits[0][0xf]==ME&&c16(0,4)==0x1d&&c32(0,8)==0xb0000000u+tg*16+2);UNT(0,0x11,4,5,8,9,10,11,0xf);}
        ++cases;
    }
    /* Electrify: fails when the target has moved. */
    for(unsigned a=0;a<3;++a)for(unsigned t=0;t<3;++t)for(unsigned f=0;f<3;++f) {
        static const uint32_t flags[3]={0,1,0x100};
        reset();tf_value=flags[f];run(px_battle_electrify_effect,ids[a],targets[t]);const uint32_t tg=targets[t]&0xff;
        if(ids[a]!=ME)EXPECT("");
        else if(flags[f]){snprintf(want,sizeof want,"tf%u/1 ",tg);EXPECT(want);}
        else{snprintf(want,sizeof want,"tf%u/1 recb/2 cw1/c msg4ab/142 arg%x/14 commit ",tg,tg);EXPECT(want);
            CHECK(commits[0][0xf]==tg&&c16(0,4)==0x24&&c32(0,8)==0xa000010cu);UNT(0,0x11,4,5,8,9,10,11,0xf);}
        ++cases;
    }
    /* Endure: message, work word 6, then the counter record without flag 0x1000000. */
    for(unsigned a=0;a<3;++a)for(unsigned wi=0;wi<3;++wi)for(unsigned c=0;c<3;++c) {
        static const uint32_t counts[3]={0,2,0xff};
        reset();first_word=0x1000011;ct_value=counts[c];w32(NULL,WORK+0x18,work_in[wi]);run(px_battle_endure_effect,ids[a],OTHER);
        const int me=ids[a]==ME;
        EXPECT(me?"rec3/2 msg1ff/42 arg2/4 commit rec27/2 ct2/3 commit ":"");
        CHECK(r32(NULL,WORK+0x18)==(me?1u:work_in[wi]));
        if(me){CHECK(c32(0,0)==0x1000011&&commits[1][4]==ME&&commits[1][5]==3&&commits[1][6]==((counts[c]+1)&0xff));
            UNT(1,0x11,4,5,6);}
        ++cases;
    }
    /* Recycle: the held item (+0x12) must be empty, the previous one (+0x14) not. */
    for(unsigned a=0;a<3;++a)for(unsigned h=0;h<3;++h)for(unsigned p=0;p<3;++p) {
        static const uint32_t held[3]={0,5,0x100},prev[3]={0,0x1234,0x100};
        reset();w16(NULL,MON+0x100*ME+0x12,(uint16_t)held[h]);w16(NULL,MON+0x100*ME+0x14,(uint16_t)prev[p]);
        run(px_battle_recycle_effect,ids[a],OTHER);const int on=ids[a]==ME&&held[h]==0&&prev[p]!=0;
        snprintf(want,sizeof want,"rec21/2 msg2dd/c2 arg2/c arg%x/c commit ",prev[p]);EXPECT(on?want:"");
        if(on){CHECK(c16(0,4)==prev[p]&&commits[0][6]==ME&&commits[0][7]==1);UNT(0,0x11,4,5,6,7);}
        ++cases;
    }
    /* Focus Energy. */
    for(unsigned a=0;a<3;++a)for(unsigned f=0;f<3;++f) {
        static const uint32_t flags[3]={0,1,0x100};
        reset();bf_value=flags[f];run(px_battle_focus_energy_effect,ids[a],OTHER);
        const int me=ids[a]==ME;
        EXPECT(!me?"":flags[f]?"bf2/9 ":"bf2/9 rec17/2 commit rec3/2 msg417/42 arg2/4 commit ");
        if(me&&!flags[f]){CHECK(commits[0][4]==9&&commits[0][5]==ME);UNT(0,0x11,4,5);}
        ++cases;
    }
    /* Role Play: the target's ability (low byte) unless blocked. */
    for(unsigned a=0;a<3;++a)for(unsigned t=0;t<3;++t)for(unsigned b=0;b<2;++b)for(unsigned v=0;v<3;++v) {
        static const uint32_t abilities[3]={0x22,0x1ab,0};
        reset();ab_value=b;pv_value=abilities[v];run(px_battle_role_play_effect,ids[a],targets[t]);
        const uint32_t tg=targets[t]&0xff,ab=abilities[v]&0xff;
        if(ids[a]!=ME)EXPECT("");
        else if(b){snprintf(want,sizeof want,"v%u/10 ab%x ",tg,ab);EXPECT(want);}
        else{snprintf(want,sizeof want,"v%u/10 ab%x rec20/2 msg26b/c2 arg2/c arg%x/c arg%x/c commit ",tg,ab,tg,ab);EXPECT(want);
            CHECK(c16(0,4)==ab&&commits[0][6]==ME);UNT(0,0x11,4,5,6);}
        ++cases;
    }
    /* Disable: no status, a usable last move that is neither 0 nor 0xa5. */
    for(unsigned a=0;a<3;++a)for(unsigned t=0;t<2;++t)for(unsigned hs=0;hs<2;++hs)for(unsigned m=0;m<4;++m)for(unsigned mu=0;mu<2;++mu)for(unsigned tf=0;tf<2;++tf) {
        static const uint32_t moves[4]={0,0xa5,0x33,0x1a5};
        reset();hs_value=hs;lm_value=moves[m];mu_value=mu;tf_value=tf;run(px_battle_disable_effect,ids[a],targets[t]);
        const uint32_t tg=targets[t]&0xff;want[0]=0;char part[80];
        if(ids[a]==ME){
            snprintf(part,sizeof part,"hs%u/d ",tg);strcat(want,part);
            if(!hs){snprintf(part,sizeof part,"lm%u ",tg);strcat(want,part);
              if(moves[m]!=0&&moves[m]!=0xa5){snprintf(part,sizeof part,"mu%u/%x ",tg,moves[m]);strcat(want,part);
                if(mu){snprintf(part,sizeof part,"tf%u/1 recb/2 cw%x/%x commit ",tg,tf?5:4,moves[m]);strcat(want,part);
                    CHECK(c16(0,4)==0xd&&c32(0,8)==0xa0000000u+(tf?5:4)*0x100+moves[m]&&commits[0][0xf]==tg);UNT(0,0x11,4,5,8,9,10,11,0xf);}}}
        }
        EXPECT(want);++cases;
    }
    /* Reflect Type. */
    for(unsigned a=0;a<3;++a)for(unsigned t=0;t<3;++t) {
        static const uint32_t tys[3]={0x4aaa,0x12345,0};
        reset();first_word=0x11;ty_value=tys[t];run(px_battle_reflect_type_effect,ids[a],targets[t]);const uint32_t tg=targets[t]&0xff;
        snprintf(want,sizeof want,"rec13/2 ty%u commit rec3/2 msg447/42 arg2/4 arg%x/4 commit ",tg,tg);EXPECT(ids[a]==ME?want:"");
        if(ids[a]==ME){CHECK(c16(0,4)==(tys[t]&0xffff)&&commits[0][6]==ME&&commits[0][7]==1&&c32(1,0)==0x1000011&&c32(0,0)==0x11);UNT(0,0x11,4,5,6,7);}
        ++cases;
    }
    /* Psych Up. */
    for(unsigned a=0;a<3;++a)for(unsigned f=0;f<2;++f) {
        reset();first_word=f?0x1000011:0x11;run(px_battle_psych_up_effect,ids[a],OTHER);
        EXPECT(ids[a]==ME?"rec10/2 rs2/9 rs2/8 msg305/102 arg2/10 commit rec17/2 commit ":"");
        if(ids[a]==ME){CHECK(commits[0][0xe]==ME&&c16(0,4)==((0x1230000u+9*0x111)&0xffff)&&c16(0,6)==((0x1230000u+8*0x111)&0xffff)&&commits[0][0xf]==0xa7);
            CHECK(commits[1][4]==0xa&&commits[1][5]==ME&&c32(0,0)==first_word);}
        ++cases;
    }
    /* Healing Wish and Lunar Dance. */
    for(unsigned a=0;a<3;++a)for(unsigned s=0;s<3;++s)for(unsigned w=0;w<2;++w) {
        static const uint32_t switches[3]={0,1,0x100};
        reset();sw_value=switches[s];run(w?px_battle_lunar_dance_effect:px_battle_healing_wish_effect,ids[a],OTHER);
        const int me=ids[a]==ME;
        EXPECT(!me?"":switches[s]?"sw2 rec1f/2 side2 commit rec12/2 commit ":"sw2 ");
        if(me&&switches[s]){CHECK(commits[0][4]==(w?1u:2u)&&commits[0][5]==0xa7&&commits[1][4]==ME);UNT(0,0x11,4,5);UNT(1,0x11,4);}
        ++cases;
    }
    /* Helping Hand: a partner, the target neither fainted nor moved. */
    for(unsigned a=0;a<3;++a)for(unsigned oc=0;oc<4;++oc)for(unsigned fa=0;fa<2;++fa)for(unsigned tf=0;tf<2;++tf)for(unsigned t=0;t<2;++t) {
        reset();oc_value=oc;fa_value=fa;tf_value=tf;w32(NULL,WORK,0x99);w8(NULL,HANDLER+0x10,0x33);
        run(px_battle_helping_hand_effect,ids[a],targets[t]);const uint32_t tg=targets[t]&0xff;
        want[0]=0;char part[80];strcat(want,"ow");snprintf(part,sizeof part,"%u oc9 ",ids[a]);want[0]=0;
        snprintf(part,sizeof part,"ow%u oc9 ",ME);
        /* owner_of receives the handler's own id argument (ME), the key only gates afterwards. */
        strcat(want,part);
        const int on=oc>1&&ids[a]==ME&&!fa&&!tf;
        if(oc>1&&ids[a]==ME){snprintf(part,sizeof part,"fa%u ",tg);strcat(want,part);
            if(!fa){snprintf(part,sizeof part,"tf%u/1 ",tg);strcat(want,part);
                if(!tf){snprintf(part,sizeof part,"rec3/2 msg41a/42 arg2/4 arg%x/4 commit ",tg);strcat(want,part);}}}
        EXPECT(want);
        CHECK(r32(NULL,WORK)==(on?tg:0x99u)&&r8(NULL,HANDLER+0x10)==(on?6u:0x33u));
        ++cases;
    }
    /* Through the dispatcher: Endure then Focus Energy on event 0xac. */
    {
        reset();first_word=0x1000011;
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7c00+0x10*i;
            w8(NULL,entries,0xac);w32(NULL,entries+4,i);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ME);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);frame(ME,OTHER);px_battle_event_dispatch(CTX,0xac,&events);
        EXPECT("rec3/2 msg1ff/42 arg2/4 commit rec27/2 ct2/3 commit bf2/9 rec17/2 commit rec3/2 msg417/42 arg2/4 commit ");
        CHECK(aborts==0&&ncommit==4);++cases;
    }
    printf("battle_moves_b3: %u cases, all tests passed\n",cases);
    return 0;
}
