#include "battle_moves_19.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, RECORD=0x7000, BUF=0x7800,
       ENTRIES=0x7900, NODES=0x8000 };
enum { ATT=2 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static unsigned aborts;static char log_[4096];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b,uint32_t c) { char t[64];snprintf(t,sizeof t,f,a,b,c);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned slot(uint32_t p) { if(p==0) { return 99; } CHECK(p>=MON&&p<MON+0x800&&!(p&0xff));return (p-MON)>>8; }
static uint32_t records,ghost,hp,full,types_cur,terr,env,flagc,forbidden,own,locked,blocked_mask,blocked_calls,mlast,mused,mslot,mpp,mban,mblock,counter_v,psych_flag,
    turnw,item_val;
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;notef("pv%u/%x ",slot(p),k,0);return k==0xd?hp:k==0xe?hp+0x30:(k*3+slot(p))&0x7f; }
static const struct px_battle_ability_api base={&events,pokemon_value,NULL,NULL,NULL,NULL,NULL};
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX&&records<8);notef("rec%x/%u ",k,id,0);return RECORD+0x40*records++; }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX);notef("commit%u ",(r-RECORD)/0x40,0,0); }
static void message(void *u,uint32_t at,uint32_t k,uint32_t m) { (void)u;notef("msg%x/%x ",m,(at-RECORD)*16+k,0); }
static void message_arg(void *u,uint32_t at,uint32_t v) { (void)u;(void)at;notef("arg%x ",v,0,0); }
static uint32_t counter(void *u,uint32_t p,uint32_t i) { (void)u;notef("cnt%u/%u ",slot(p),i,0);return counter_v; }
static uint32_t has_type(void *u,uint32_t p,uint32_t t) { (void)u;notef("ty%u/%u ",slot(p),t,0);return ghost; }
static uint32_t hp_fraction(void *u,uint32_t p,uint32_t d) { (void)u;notef("hf%u/%u ",slot(p),d,0);return 0x12345; }
static void effect_flag(void *u,uint32_t c,uint32_t v) { (void)u;CHECK(c==CTX);notef("flag%u ",v,0,0);flagc=v; }
static uint32_t full_hp(void *u,uint32_t p) { (void)u;notef("full%u ",slot(p),0,0);return full; }
static uint32_t turn_word(void *u,uint32_t t) { (void)u;notef("tw%u ",t,0,0);return turnw; }
static uint32_t trade_forbidden(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("tf ");return forbidden; }
static uint32_t owner_of(void *u,uint32_t id) { (void)u;notef("own%u ",id,0,0);return own; }
static uint32_t item_locked(void *u,uint32_t i) { (void)u;notef("lk%x ",i,0,0);return (i!=0&&locked==i)?1u:0u; }
static uint32_t item_blocked(void *u,uint32_t c,uint32_t f,uint32_t i) { (void)u;CHECK(c==CTX);notef("ib%x/%x ",f,i,0);return blocked_mask>>(blocked_calls++)&1; }
static uint32_t last_move(void *u,uint32_t p) { (void)u;notef("lm%u ",slot(p),0,0);return mlast; }
static uint32_t last_used(void *u,uint32_t p) { (void)u;notef("lu%u ",slot(p),0,0);return mused; }
static uint32_t move_slot(void *u,uint32_t p,uint32_t m) { (void)u;notef("ms%u/%x ",slot(p),m,0);return mslot; }
static uint32_t move_pp(void *u,uint32_t p,uint32_t s) { (void)u;notef("pp%u/%u ",slot(p),s,0);return mpp; }
static uint32_t mimic_banned(void *u,uint32_t p,uint32_t m) { (void)u;notef("mb%u/%x ",slot(p),m,0);return mban; }
static uint32_t mimic_blocked(void *u,uint32_t p) { (void)u;notef("mk%u ",slot(p),0,0);return mblock; }
static uint32_t stage_copy_flag(void *u,uint32_t p,uint32_t k) { (void)u;notef("sc%u/%u ",slot(p),k,0);return psych_flag; }
static uint32_t temp_buffer(void *u,uint32_t c,uint32_t n) { (void)u;CHECK(c==CTX);notef("buf%x ",n,0,0);return BUF; }
static uint32_t terrain(void *u,uint32_t c) { (void)u;CHECK(c==CTX);return terr; }
static uint32_t environment(void *u,uint32_t c) { (void)u;CHECK(c==CTX);return env; }
static uint32_t type_word(void *u,uint32_t p) { (void)u;notef("tw%u ",slot(p)+100,0,0);return types_cur; }
static const struct px_battle_moves_19_api api={&base,action_record,commit,message,message_arg,counter,has_type,hp_fraction,effect_flag,full_hp,
    turn_word,trade_forbidden,owner_of,item_locked,item_blocked,last_move,last_used,move_slot,move_pp,mimic_banned,mimic_blocked,stage_copy_flag,
    temp_buffer,terrain,environment,type_word};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_19_api*);
static const fn dispatched[]={px_battle_protect_streak_up,px_battle_rest_heal};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w) { (void)u;CHECK(f<2);dispatched[f](h,c,v,w,&api); }
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;log_[0]=0;records=ghost=hp=full=types_cur=terr=env=flagc=forbidden=own=locked=blocked_mask=blocked_calls=0;
    mlast=mused=mslot=mpp=mban=mblock=counter_v=psych_flag=turnw=item_val=0;hp=0x20;
    for(unsigned i=0;i<8;++i){ memset(mem+RECORD+0x40*i,0xa5,0x40); }
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
/* Keys: 0x03 attacker, 0x06 target (ids); 0x53 a value. */
static void frame(uint32_t attacker,uint32_t target)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x03,attacker,&events);px_battle_event_set_id(0x06,target,&events);
    px_battle_event_set_value(0x53,7,&events);log_[0]=0;
}
static uint32_t key(uint32_t k) { return px_battle_event_get(k,&events); }
static int same(const char *expected) { if(strcmp(log_,expected)){ fprintf(stderr,"log [%s] expected [%s]\n",log_,expected);return 0; } return 1; }
#define R(n) (RECORD+0x40*(n))
#define MONP(i) (MON+0x100*(i))
int main(void)
{
    unsigned cases=0;
    /* Nothing for another attacker. */
    {
        const fn all[]={px_battle_protect_streak_up,px_battle_curse_effect,px_battle_trick_swap,px_battle_spite_drain,px_battle_rest_heal,px_battle_mimic_copy,
                        px_battle_bestow_give,px_battle_psych_up_copy,px_battle_guard_swap,px_battle_heart_swap,px_battle_camouflage_type};
        for(unsigned k=0;k<11;++k)for(uint32_t o=0;o<2;++o){ reset();frame(o?0x102:3,5);all[k](0x6000,CTX,ATT,0x5000,&api);
            CHECK(log_[0]==0&&records==0&&aborts==0&&key(0x53)==7);++cases; }
    }
    /* Protect and Detect: the streak, and the first word's bit 24. */
    for(uint32_t c=0;c<3;++c){ reset();counter_v=c==0?0:c==1?4:0xff;w32(NULL,R(1),0xffffffffu);frame(ATT,5);px_battle_protect_streak_up(0x6000,CTX,ATT,0x5000,&api);
        CHECK(same("rec15/2 commit0 rec27/2 cnt2/3 commit1 rec3/2 msg205/842 arg2 commit2 "));
        CHECK(mem[R(0)+4]==7&&mem[R(0)+5]==ATT&&mem[R(1)+4]==ATT&&mem[R(1)+5]==3&&mem[R(1)+6]==(uint8_t)(counter_v+1)&&r32(NULL,R(1))==0xfeffffffu);++cases; }
    /* A missing user aborts in the first lookup of Protect and Detect. */
    reset();w32(NULL,TABLE+0x84+4*ATT,0);frame(ATT,5);px_battle_protect_streak_up(0x6000,CTX,ATT,0x5000,&api);CHECK(aborts>=1);++cases;
    /* Curse. */
    for(uint32_t g=0;g<2;++g)for(uint32_t h=0;h<3;++h)for(uint32_t t=0;t<2;++t){
        reset();ghost=g;hp=h==0?0:h==1?0x20:0xffffffffu;w32(NULL,R(1),0);frame(ATT,t?0x11f:0x105);px_battle_curse_effect(0x6000,CTX,ATT,0x5000,&api);CHECK(aborts==0);
        if(!g) {
            CHECK(same("ty2/7 recd/2 commit0 recd/2 commit1 recd/2 commit2 flag0 ")&&records==3);
            const uint8_t st[3]={5,1,2},am[3]={0xff,1,1};
            for(unsigned i=0;i<3;++i){ CHECK(mem[R(i)+4]==st[i]&&mem[R(i)+0x16]==1&&mem[R(i)+0x17]==am[i]&&mem[R(i)+0xc]==ATT&&mem[R(i)+0x18]==(0xa5|4)); }
        } else if(h==0||h==2||t) {
            char e[80];snprintf(e,sizeof e,"ty2/7 pv2/d flag1 ");CHECK(same(e)&&records==0&&flagc==1);
        } else {
            CHECK(same("ty2/7 pv2/d recb/2 msg42e/142 arg2 arg5 commit0 rec7/2 hf2/2 commit1 flag1 "));
            CHECK(mem[R(0)+0xf]==5&&r16(NULL,R(0)+4)==0xa&&r32(NULL,R(0)+8)==1&&mem[R(1)+7]==ATT&&r32(NULL,R(1)+0x14)==(uint32_t)(0-0x12345)&&mem[R(1)+4]==1);
            CHECK((r32(NULL,R(1))&0x1000000u)!=0&&mem[R(1)+0xe]==0xa5);
        }
        ++cases; }
    /* Trick and Switcheroo. */
    for(uint32_t f=0;f<2;++f)for(uint32_t o=0;o<2;++o)for(uint32_t m=0;m<4;++m)for(uint32_t lk=0;lk<3;++lk)for(uint32_t b=0;b<6;++b){
        reset();forbidden=f;own=o?1:0;const uint32_t mine=(m&1)?0x111:0,theirs=(m&2)?0x222:0;
        w16(NULL,MONP(2)+0x12,(uint16_t)mine);w16(NULL,MONP(5)+0x12,(uint16_t)theirs);w16(NULL,MONP(2)+0xc,0x1000);w16(NULL,MONP(5)+0xc,0x2000);
        locked=lk==0?0:lk==1?mine:theirs;blocked_mask=b==0?0:b<5?1u<<(b-1):0;frame(ATT,5);px_battle_trick_swap(0x6000,CTX,ATT,0x5000,&api);
        const int stop_early=(!f&&o)||(mine|theirs)==0;
        if(stop_early){ CHECK(records==0); ++cases; continue; }
        const int lock_hit=lk!=0&&locked!=0&&((lk==1&&mine!=0)||(lk==2&&theirs!=0));
        if(lock_hit||blocked_mask!=0) { CHECK(records==0); ++cases; continue; }
        CHECK(records==1&&mem[R(0)+4]==5);
        const char *e=log_;
        CHECK(strstr(e,"rec25/2 ")!=NULL&&strstr(e,"msg2aa/82 ")!=NULL);
        CHECK((strstr(e,"msg2ad/502 ")!=NULL)==(theirs!=0)&&(strstr(e,"msg2ad/2c2 ")!=NULL)==(mine!=0));
        ++cases; }
    /* Spite. */
    for(uint32_t s=0;s<2;++s)for(uint32_t p=0;p<7;++p){
        reset();mused=0x222;mslot=s?4:2;mpp=p==6?0x102:p;frame(ATT,5);px_battle_spite_drain(0x6000,CTX,ATT,0x5000,&api);
        const uint32_t pp=(mpp&0xff)>4?4:(mpp&0xff);
        if(s||pp==0) { CHECK(records==0); }
        else { char e[120];snprintf(e,sizeof e,"lu5 ms5/222 pp5/2 rec9/2 msg281/82 arg5 arg222 arg%x commit0 ",pp);CHECK(same(e));
            CHECK(mem[R(0)+6]==2&&mem[R(0)+5]==5&&mem[R(0)+4]==pp); }
        ++cases; }
    /* Rest. */
    for(uint32_t f=0;f<2;++f)for(uint32_t h=0;h<3;++h){
        reset();full=f;hp=h==0?0x10:h==1?0:0xfff0;turnw=0x77665544;frame(ATT,5);w32(NULL,R(1),0x1u);w32(NULL,R(2),0x2u);px_battle_rest_heal(0x6000,CTX,ATT,0x5000,&api);
        if(f) { CHECK(same("full2 ")&&key(0x53)==7); }
        else {
            CHECK(strstr(log_,"full2 recb/2 tw3 msg27e/142 arg2 commit0 rec4/2 pv2/e pv2/d commit1 rec22/2 commit2 ")!=NULL);
            CHECK(key(0x53)==0&&r16(NULL,R(0)+4)==2&&r32(NULL,R(0)+8)==0x77665544&&mem[R(0)+0xc]==1&&mem[R(0)+0x10]==1&&mem[R(0)+0xe]==1&&mem[R(0)+0xf]==ATT);
            CHECK(mem[R(1)+6]==ATT&&r16(NULL,R(1)+4)==0x30&&(r32(NULL,R(1))&0x1000000u)!=0&&r32(NULL,R(1))==(0x1u|0x1000000u)&&mem[R(2)+4]==ATT&&mem[R(2)+5]==4&&r32(NULL,R(2))==(0x2u|0x1000000u));
        }
        ++cases; }
    /* Mimic: the two last-move services (the second wins), and every refusal. */
    for(uint32_t u=0;u<9;++u)for(uint32_t r=0;r<5;++r){
        reset();const uint32_t moves[9]={0,0xa6,0xa5,0x1c0,0x123,0x123,0x123,0x123,0x1fc};mlast=0x77;mused=moves[u];mban=r==1;mblock=r==2;mslot=r==3?4:1;
        w16(NULL,0x6000+0x38,0x20);frame(ATT,5);px_battle_mimic_copy(0x6000,CTX,ATT,0x5000,&api);
        const int refused=u<4||r==1||r==2||r==3;
        if(refused) CHECK(records==0&&strstr(log_,"lm5 lu5 ")!=NULL);
        else { CHECK(records==2&&mem[R(0)+6]==0&&mem[R(0)+5]==1&&mem[R(0)+7]==1&&mem[R(0)+4]==ATT&&r16(NULL,R(0)+8)==moves[u]);
            char e[60];snprintf(e,sizeof e,"msg2b0/442 arg2 arg%x ",moves[u]);CHECK(strstr(log_,e)!=NULL); }
        ++cases; }
    reset();mlast=0x123;mused=0x123;w16(NULL,0x6000+0x38,0x20);mslot=3;frame(ATT,5);px_battle_mimic_copy(0x6000,CTX,ATT,0x5000,&api);CHECK(strstr(log_,"ms2/20 ")!=NULL&&records==2);++cases;
    /* Bestow. */
    for(uint32_t i=0;i<2;++i)for(uint32_t lk=0;lk<2;++lk)for(uint32_t th=0;th<2;++th)for(uint32_t b=0;b<4;++b){
        reset();w16(NULL,MONP(2)+0x12,(uint16_t)(i?0x111:0));w16(NULL,MONP(5)+0x12,(uint16_t)(th?0x222:0));w16(NULL,MONP(2)+0xc,0x1000);w16(NULL,MONP(5)+0xc,0x2000);
        locked=lk?0x111:0;blocked_mask=b==0?0:1u<<(b-1);frame(ATT,5);px_battle_bestow_give(0x6000,CTX,ATT,0x5000,&api);
        if(!i||lk||th||b) CHECK(records==0);
        else { CHECK(strstr(log_,"ib1000/111 ib2000/0 ib2000/111 rec21/2 commit0 rec21/2 msg45d/4c2 arg5 arg2 arg111 commit1 ")!=NULL);
            CHECK(records==2&&mem[R(0)+6]==ATT&&r16(NULL,R(0)+4)==0&&mem[R(1)+6]==5&&r16(NULL,R(1)+4)==0x111); }
        ++cases; }
    /* Psych Up. */
    for(uint32_t f=0;f<2;++f){ reset();psych_flag=f;w8(NULL,MONP(5)+0x155,0x9c);frame(ATT,5);px_battle_psych_up_copy(0x6000,CTX,ATT,0x5000,&api);
        CHECK(same(f?"rece/2 pv5/1 pv5/2 pv5/3 pv5/4 pv5/5 pv5/6 pv5/7 commit0 sc5/9 rec17/2 commit1 rec3/2 msg41d/842 arg2 arg5 commit2 ":"rece/2 pv5/1 pv5/2 pv5/3 pv5/4 pv5/5 pv5/6 pv5/7 commit0 sc5/9 rec18/2 commit1 rec3/2 msg41d/842 arg2 arg5 commit2 "));
        CHECK(mem[R(0)+4]==ATT&&mem[R(0)+0xc]==0x9c&&mem[R(1)+5]==ATT&&mem[R(1)+4]==9);
        for(uint32_t i=1;i<=7;++i) { CHECK(mem[R(0)+4+i]==((i*3+5)&0x7f)); }
        ++cases; }
    /* Guard Swap and Heart Swap. */
    reset();w8(NULL,MONP(2)+0x155,0x11);w8(NULL,MONP(5)+0x155,0x22);frame(ATT,5);px_battle_guard_swap(0x6000,CTX,ATT,0x5000,&api);
    CHECK(records==3&&mem[R(0)+4]==ATT&&mem[R(1)+4]==5&&mem[R(0)+0xc]==0x11&&mem[R(1)+0xc]==0x22);
    CHECK(mem[R(0)+6]==((2*3+5)&0x7f)&&mem[R(0)+8]==((4*3+5)&0x7f)&&mem[R(1)+6]==((2*3+2)&0x7f)&&mem[R(1)+8]==((4*3+2)&0x7f));
    CHECK(mem[R(0)+5]==((1*3+2)&0x7f)&&mem[R(1)+5]==((1*3+5)&0x7f)&&mem[R(0)+7]==((3*3+2)&0x7f)&&mem[R(1)+7]==((3*3+5)&0x7f));
    CHECK(mem[R(0)+9]==((5*3+2)&0x7f)&&mem[R(1)+9]==((5*3+5)&0x7f)&&mem[R(0)+0xb]==((7*3+2)&0x7f)&&mem[R(1)+0xb]==((7*3+5)&0x7f));
    CHECK(strstr(log_,"msg2a7/842 arg2 ")!=NULL);++cases;
    reset();w8(NULL,MONP(2)+0x155,0x11);w8(NULL,MONP(5)+0x155,0x22);frame(ATT,5);px_battle_heart_swap(0x6000,CTX,ATT,0x5000,&api);
    CHECK(records==3&&mem[R(0)+4]==ATT&&mem[R(1)+4]==5&&mem[R(0)+0xc]==0x11&&mem[R(1)+0xc]==0x22&&strstr(log_,"buf1c ")!=NULL);
    for(uint32_t i=1;i<=7;++i) { CHECK(mem[R(0)+4+i]==((i*3+5)&0x7f)&&mem[R(1)+4+i]==((i*3+2)&0x7f)&&r32(NULL,BUF+4*(i-1))==((i*3+2)&0x7fu)); }
    CHECK(strstr(log_,"msg2a1/842 arg2 ")!=NULL);++cases;
    /* Camouflage: every terrain and environment, and the current type. */
    {
        const uint8_t bt[8]={0,4,5,0xb,0xa,4,0xe,0xe},be[6]={0,0xb,0x11,0xc,0xa,0xe};
        for(uint32_t t=0;t<10;++t)for(uint32_t e=0;e<8;++e)for(uint32_t c=0;c<2;++c){
            reset();terr=t;env=e;frame(ATT,5);
            uint32_t ty=t<8?bt[t]:0u;if(e<6&&e!=0) ty=be[e];
            uint32_t w=ty&0x1f;w|=w<<5;w|=0x4800;types_cur=c?w:w+1;
            px_battle_camouflage_type(0x6000,CTX,ATT,0x5000,&api);
            if(c) CHECK(records==0);
            else { CHECK(records==1&&r16(NULL,R(0)+4)==w&&mem[R(0)+6]==ATT&&mem[R(0)+7]==0xa5&&strstr(log_,"tw102 ")!=NULL); }
            ++cases; }
    }
    /* Through the dispatcher on event 0xac: Protect's handler for battler 2, Rest's for battler 2. */
    {
        reset();
        for(unsigned i=0;i<2;++i){ w8(NULL,ENTRIES+0x10*i,0xac);w32(NULL,ENTRIES+0x10*i+4,i?1:0); }
        for(unsigned i=0;i<2;++i) {
            const uint32_t hn=NODES+0x40*i;
            w32(NULL,hn+8,ENTRIES+0x10*i);w32(NULL,hn+0x18,0x20000000u|1u<<16);w8(NULL,hn+0x3a,ATT);
            w32(NULL,hn,i?hn-0x40:0);w32(NULL,hn+4,i<1?hn+0x40:0);
        }
        w32(NULL,BATTLE+0x60,NODES);frame(ATT,5);px_battle_event_dispatch(CTX,0xac,&events);
        CHECK(records==6&&key(0x53)==0&&aborts==0);++cases;
    }
    printf("battle_moves_19: %u cases, all tests passed\n",cases);
    return 0;
}
