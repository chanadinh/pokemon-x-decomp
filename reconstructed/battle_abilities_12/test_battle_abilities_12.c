#include "battle_abilities_12.h"
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
static uint32_t hp[8],toxic,illusion,damaging,at_value;
static unsigned aborts,ncommit;static uint8_t commits[4][0x20];static char log_[1024];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static uint32_t value(void *u,uint32_t p,uint32_t k) { (void)u;notef("v%u/%x ",mon(p),k);CHECK(k==0xd);return hp[mon(p)]; }
static uint32_t side_of(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("pos%u ",id,0);return 0x10+id; }
static const struct px_battle_ability_api base={&events,value,NULL,NULL,side_of,NULL,NULL};
static uint32_t battle_mode(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("mode ");return 7; }
static uint32_t facing(void *u,uint32_t m,uint32_t p) { (void)u;notef("face%u/%x ",m,p);return p+0x20; }
static uint32_t battler_at(void *u,uint32_t c,uint32_t p) { (void)u;CHECK(c==CTX);notef("at%x ",p,0);return at_value; }
static uint32_t badly_poisoned(void *u,uint32_t c) { (void)u;notef("tox%x ",c,0);return toxic; }
static void condition_for(void *u,uint32_t s,uint32_t p,uint32_t out) { (void)u;CHECK(out==RECORD+8);notef("cf%u/%u ",s,mon(p));w32(NULL,out,0xcf00+s); }
static uint32_t illusion_active(void *u,uint32_t p) { (void)u;notef("ill%u ",mon(p),0);return illusion; }
static uint32_t damaging_move(void *u,uint32_t m) { (void)u;notef("dmg%x ",m,0);return damaging; }
static void popup(void *u,uint32_t c,uint32_t p,uint32_t id) { (void)u;CHECK(c==CTX);notef("pop%u/%u ",p,id); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);
    memset(mem+RECORD,0xa5,0x20);w32(NULL,RECORD,0x11);w8(NULL,RECORD+0x18,0xff);return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;notef("msg%x/%x ",id,(r-RECORD)*16+k); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;(void)r;notef("arg%x ",v,0); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD&&ncommit<4);memcpy(commits[ncommit++],mem+RECORD,0x20);note("commit "); }
static const struct px_battle_abilities_12_api api={&base,battle_mode,facing,battler_at,badly_poisoned,condition_for,illusion_active,damaging_move,
    popup,action_record,message,message_arg,commit};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_abilities_12_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_truant_loaf,px_battle_truant_message};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(hp,0,sizeof hp);memset(commits,0,sizeof commits);aborts=ncommit=0;log_[0]=0;
    toxic=illusion=damaging=at_value=0;
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);w16(NULL,HANDLER+0x38,0x95);
}
struct keys { uint32_t k2,k3,k4,k10,move,k16,k1e,k1f,k23,k47; };
static void frame(struct keys k)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x02,k.k2,&events);px_battle_event_set_id(0x03,k.k3,&events);px_battle_event_set_id(0x04,k.k4,&events);
    px_battle_event_set_value(0x10,k.k10,&events);px_battle_event_set_value(0x12,k.move,&events);px_battle_event_set_value(0x16,k.k16,&events);
    px_battle_event_set_value(0x1e,k.k1e,&events);px_battle_event_set_value(0x1f,k.k1f,&events);px_battle_event_set_value(0x23,k.k23,&events);
    px_battle_event_set_value(0x47,k.k47,&events);px_battle_event_set_value(0x36,0x1000,&events);px_battle_event_set_value(0x57,0,&events);
    log_[0]=0;
}
static uint32_t get(uint32_t key) { return px_battle_event_get(key,&events); }
static void run(fn f,struct keys k) { frame(k);f(HANDLER,CTX,ME,WORK,&api);if(aborts)fprintf(stderr,"log %s\n",log_);CHECK(aborts==0); }
static uint32_t c16(unsigned n,unsigned o) { return (uint32_t)(commits[n][o]|commits[n][o+1]<<8); }
static uint32_t c32(unsigned n,unsigned o) { return c16(n,o)|c16(n,o+2)<<16; }
static void cat(char *w,size_t n,const char *f,uint32_t a,uint32_t b) { size_t l=strlen(w);snprintf(w+l,n-l,f,a,b); }
#define EXPECT(w) do { if(strcmp(log_,w)){fprintf(stderr,"got  %s\nwant %s\n",log_,w);CHECK(0);} } while (0)
int main(void)
{
    unsigned cases=0;char want[512];
    static const uint32_t ids[3]={ME,OTHER,0x100+ME};
    const struct keys BASE={OTHER,OTHER,OTHER,0x95,0x10021,0,0,0,0,0};
    /* Imposter: transform into the battler facing its position, both alive. */
    for(unsigned a=0;a<3;++a)for(uint32_t mine=0;mine<2;++mine)for(uint32_t theirs=0;theirs<2;++theirs) {
        reset();struct keys k=BASE;k.k2=ids[a];at_value=6;hp[ME]=mine;hp[6]=theirs?0x10000:0;run(px_battle_imposter,k);
        want[0]=0;if(ids[a]==ME){strcat(want,"pos2 mode face7/12 at32 v2/d ");if(mine){strcat(want,"v6/d ");if(theirs)strcat(want,"rec34/2 msg284/82 arg2 arg6 commit ");}}
        EXPECT(want);if(ncommit){CHECK(commits[0][4]==6&&c32(0,0)==0x800011);}
        ++cases;
    }
    /* Truant: loafs every other turn (reason 0x13), then its message. */
    for(unsigned a=0;a<3;++a)for(uint32_t w=0;w<3;++w)for(unsigned pre=0;pre<2;++pre) {
        reset();struct keys k=BASE;k.k2=ids[a];w32(NULL,WORK,w);w32(NULL,WORK+4,7);frame(k);if(pre)px_battle_event_rewrite(0x23,0,&events);log_[0]=0;
        px_battle_truant_loaf(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0&&log_[0]==0);
        const int me=ids[a]==ME;
        CHECK(r32(NULL,WORK)==(me?(w==0?1u:0u):w)&&r32(NULL,WORK+4)==(me&&w?(pre?0u:1u):7u)&&get(0x23)==(me&&w&&!pre?0x13u:0u));
        reset();w32(NULL,WORK+4,w);run(px_battle_truant_message,k);
        EXPECT(me&&w?"rec3/2 msg1bd/42 arg2 commit ":"");CHECK(r32(NULL,WORK+4)==(me&&w?0u:w));if(ncommit)CHECK(c32(0,0)==0x800011);cases+=2;
    }
    /* Synchronize: poison, paralysis or burn passed back, bad poison kept. */
    for(unsigned a=0;a<3;++a)for(unsigned s=0;s<6;++s)for(unsigned src=0;src<4;++src)for(uint32_t tx=0;tx<2;++tx) {
        static const uint32_t sts[6]={5,1,4,2,0x105,0x101},srcs[4]={OTHER,0x1f,ME,0x105};
        reset();toxic=tx;struct keys k=BASE;k.k4=ids[a];k.k1e=sts[s];k.k3=srcs[src];k.k1f=0x7701;run(px_battle_synchronize,k);
        const uint32_t st=sts[s]&0xff,sr=srcs[src]&0xff;want[0]=0;
        if(ids[a]==ME&&(st==5||st==1||st==4)&&sr!=0x1f&&sr!=ME){strcat(want,"pop1/2 recb/2 ");
            const int keep=st==5&&(strcat(want,"tox7701 "),tx);
            if(!keep)cat(want,sizeof want,"cf%u/2 ",st,0);
            strcat(want,"commit pop2/2 ");
            CHECK(commits[0][0xf]==sr&&c16(0,4)==st&&c32(0,8)==(keep?0x7701u:0xcf00+st)&&commits[0][0xc]==1&&c32(0,0)==0x11);}
        EXPECT(want);++cases;
    }
    /* Aura Break: key 0x57 for every battler. */
    reset();run(px_battle_aura_break_reverse,BASE);CHECK(log_[0]==0&&get(0x57)==1);++cases;
    /* Protean: the move's type below 18. */
    for(unsigned a=0;a<3;++a)for(uint32_t t=0;t<0x14;++t)for(unsigned hi=0;hi<2;++hi) {
        reset();struct keys k=BASE;k.k3=ids[a];k.k16=t|(hi?0x300:0);run(px_battle_protean,k);
        const int on=ids[a]==ME&&t<0x12;EXPECT(on?"rec13/2 commit ":"");
        if(on){CHECK(commits[0][6]==ME&&c16(0,4)==(t|t<<5|0x4800)&&c32(0,0)==0x800011);}
        ++cases;
    }
    /* Stance Change: Aegislash's shield and blade forms. */
    for(unsigned a=0;a<3;++a)for(unsigned sp=0;sp<2;++sp)for(uint32_t f=0;f<3;++f)for(unsigned mv=0;mv<3;++mv)for(uint32_t dm=0;dm<2;++dm) {
        static const uint32_t moves[3]={0x1024c,0x24c,0x21};
        reset();damaging=dm;w16(NULL,MON+0x200+0xc,(uint16_t)(sp?0x2a9:0x2a8));w8(NULL,MON+0x200+0x153,(uint8_t)f);
        struct keys k=BASE;k.k3=ids[a];k.move=moves[mv];run(px_battle_stance_change,k);
        want[0]=0;uint32_t next=f;
        if(ids[a]==ME&&sp){if((moves[mv]&0xffff)==0x24c)next=0;else{cat(want,sizeof want,"dmg%x ",moves[mv]&0xffff,0);if(dm)next=1;}
            if(next!=f)cat(want,sizeof want,"rec3a/2 msg%x/81 commit ",next?0xfc:0xfd,0);}
        EXPECT(want);if(ncommit){CHECK(commits[0][4]==ME&&commits[0][5]==next&&c32(0,0)==0x800011);}
        ++cases;
    }
    /* Illusion breaks on leaving, on a hit (not suppressed) or losing the ability. */
    for(unsigned a=0;a<3;++a)for(uint32_t ill=0;ill<2;++ill)for(unsigned b=0;b<2;++b) {
        reset();illusion=ill;struct keys k=BASE;k.k2=ids[a];k.k4=ids[a];k.k47=b?0x100:0;k.k10=b?0x95:0x96;
        const char *brk=ill?"ill2 rec35/2 msg1de/82 arg2 commit ":"ill2 ";const int me=ids[a]==ME;
        run(px_battle_illusion_leave,k);EXPECT(me?brk:"");if(ncommit)CHECK(commits[0][4]==ME&&c32(0,0)==0x11);
        reset();illusion=ill;run(px_battle_illusion_hit,k);EXPECT(me&&!b?brk:"");
        reset();illusion=ill;run(px_battle_illusion_lost,k);EXPECT(me&&!b?brk:"");cases+=3;
    }
    /* Steadfast: Speed +1 when flinching. */
    for(unsigned a=0;a<3;++a)for(unsigned r=0;r<3;++r) {
        static const uint32_t reasons[3]={6,7,0x106};
        reset();struct keys k=BASE;k.k2=ids[a];k.k23=reasons[r];run(px_battle_steadfast,k);
        const int on=ids[a]==ME&&reasons[r]==6;EXPECT(on?"recd/2 commit ":"");
        if(on){CHECK(commits[0][4]==5&&commits[0][0x17]==1&&commits[0][0x18]==0xfd&&commits[0][0x16]==1&&commits[0][0xc]==ME&&c32(0,0)==0x800011);}
        ++cases;
    }
    /* Mega Launcher: Heal Pulse (505). */
    for(unsigned a=0;a<3;++a)for(unsigned mv=0;mv<3;++mv) {
        static const uint32_t moves[3]={0x101f9,0x1f9,0x1fa};
        reset();struct keys k=BASE;k.k3=ids[a];k.move=moves[mv];run(px_battle_mega_launcher,k);
        CHECK(log_[0]==0&&get(0x36)==(ids[a]==ME&&(moves[mv]&0xffff)==0x1f9?0xc00u:0x1000u));++cases;
    }
    /* Through the dispatcher: Truant loafs on event 0x1e, then its message on 0x21. */
    {
        reset();
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7c00+0x10*i;
            w8(NULL,entries,i?0x21:0x1e);w32(NULL,entries+4,i);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ME);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);w32(NULL,HANDLER+0x1c,1);struct keys k=BASE;k.k2=ME;frame(k);px_battle_event_dispatch(CTX,0x1e,&events);
        CHECK(aborts==0&&get(0x23)==0x13&&r32(NULL,HANDLER+0x1c+4)==1&&r32(NULL,HANDLER+0x1c)==0);
        w32(NULL,HANDLER+0x40+0x1c+4,1);frame(k);px_battle_event_dispatch(CTX,0x21,&events);
        EXPECT("rec3/2 msg1bd/42 arg2 commit ");CHECK(aborts==0&&r32(NULL,HANDLER+0x40+0x1c+4)==0);cases+=2;
    }
    printf("battle_abilities_12: %u cases, all tests passed\n",cases);
    return 0;
}
