#include "battle_abilities_11.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, POS=3, ME=2, OTHER=5 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
struct mon { uint32_t hp,max_hp,status,pp[4]; };
static struct mon mons[8];
static uint32_t decided,turn_value,at_value,position_value;
static unsigned aborts,ncommit;static uint8_t commits[8][0x20];static char log_[1024];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static uint32_t value(void *u,uint32_t p,uint32_t k) { (void)u;notef("v%u/%x ",mon(p),k);CHECK(k==0xd||k==0xe);return k==0xd?mons[mon(p)].hp:mons[mon(p)].max_hp; }
static uint32_t status(void *u,uint32_t p) { (void)u;notef("st%u ",mon(p),0);return mons[mon(p)].status; }
static uint32_t side_of(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("pos%u ",id,0);return position_value; }
static const struct px_battle_ability_api base={&events,value,NULL,status,side_of,NULL,NULL};
static uint32_t battle_decided(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("dec ");return decided; }
static uint32_t turn(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("turn ");return turn_value; }
static uint32_t battler_at(void *u,uint32_t c,uint32_t p) { (void)u;CHECK(c==CTX&&p==POS);note("at ");return at_value; }
static uint32_t pp_missing(void *u,uint32_t p,uint32_t s) { (void)u;notef("pp%u/%u ",mon(p),s);CHECK(s<4);return mons[mon(p)].pp[s]; }
static void field_clear(void *u,uint32_t c,uint32_t f,uint32_t p) { (void)u;CHECK(c==CTX);notef("clear%x/%u ",f,p); }
static void remove_handler(void *u,uint32_t h) { (void)u;CHECK(h==HANDLER);note("remove "); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%x ",k,id);
    memset(mem+RECORD,0xa5,0x20);w32(NULL,RECORD,0x11);return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;notef("msg%x/%x ",id,(r-RECORD)*16+k); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;(void)r;notef("arg%x ",v,0); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD&&ncommit<8);memcpy(commits[ncommit++],mem+RECORD,0x20);note("commit "); }
static const struct px_battle_abilities_11_api api={&base,battle_decided,turn,battler_at,pp_missing,field_clear,remove_handler,
    action_record,message,message_arg,commit};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_abilities_11_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_field_wish,px_battle_field_healing_wish};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(mons,0,sizeof mons);memset(commits,0,sizeof commits);aborts=ncommit=0;log_[0]=0;
    decided=turn_value=at_value=position_value=0;
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<0x18;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*(i&7));
    w16(NULL,HANDLER+0x38,0x1203);   /* id byte 3 */
}
static void frame(uint32_t k2) { px_battle_event_push(&events);px_battle_event_set_id(0x02,k2,&events);log_[0]=0; }
static void run(fn f,uint32_t k2) { frame(k2);f(HANDLER,CTX,POS,WORK,&api);if(aborts)fprintf(stderr,"log %s\n",log_);CHECK(aborts==0); }
static uint32_t c16(unsigned n,unsigned o) { return (uint32_t)(commits[n][o]|commits[n][o+1]<<8); }
static uint32_t c32(unsigned n,unsigned o) { return c16(n,o)|c16(n,o+2)<<16; }
static void cat(char *w,size_t n,const char *f,uint32_t a,uint32_t b) { size_t l=strlen(w);snprintf(w+l,n-l,f,a,b); }
#define EXPECT(w) do { if(strcmp(log_,w)){fprintf(stderr,"got  %s\nwant %s\n",log_,w);CHECK(0);} } while (0)
#define TAIL "clear3/3 remove "
int main(void)
{
    unsigned cases=0;char want[1024];
    /* Wish: started, then a turn later half the maker's max HP for the battler at the position. */
    for(unsigned k=0;k<2;++k)for(uint32_t dec=0;dec<2;++dec)for(uint32_t started=0;started<2;++started)for(uint32_t t=0;t<3;++t)
    for(unsigned at=0;at<3;++at)for(uint32_t hp=0;hp<3;++hp) {
        static const uint32_t ats[3]={0x1f,ME,4};
        reset();decided=dec;turn_value=t+0x100;at_value=ats[at];w32(NULL,WORK,started);w32(NULL,WORK+4,0x101);w32(NULL,WORK+0x18,0x115);
        mons[ME].hp=hp;mons[ME].max_hp=2;mons[4].hp=hp;mons[4].max_hp=2;mons[5].max_hp=9;run(px_battle_field_wish,k?0x1f:ME);
        want[0]=0;
        if(k){strcat(want,"dec ");if(!dec){strcat(want,"turn ");
            if(!started)CHECK(r32(NULL,WORK)==1&&r32(NULL,WORK+4)==t+0x100);
            else if(0x101<t+0x100){strcat(want,"at ");const uint32_t tg=ats[at];
                if(tg!=0x1f){cat(want,sizeof want,"v%u/d v%u/e ",tg&0xff,tg&0xff);if(hp!=2)strcat(want,"rec4/15 v5/e msg2bc/82 arg15 commit ");}
                strcat(want,TAIL);
                if(ncommit)CHECK(commits[0][6]==(tg&0xff)&&c16(0,4)==4&&c32(0,0)==0x11);}}}
        EXPECT(want);++cases;
    }
    /* Future Sight: on its turn (signed), the attack on the battler at the position. */
    for(unsigned k=0;k<2;++k)for(uint32_t dec=0;dec<2;++dec)for(unsigned w=0;w<4;++w)for(unsigned at=0;at<4;++at)for(uint32_t hp=0;hp<2;++hp) {
        static const uint32_t ws[4]={0x105,4,0xffffffffu,6},ats[4]={0x1f,ME,6,4};
        reset();decided=dec;turn_value=0x305;at_value=ats[at];w32(NULL,WORK,ws[w]);w32(NULL,WORK+4,0x10123);w32(NULL,WORK+8,0x77);w32(NULL,WORK+0x18,0x106);
        mons[ME].hp=hp;mons[4].hp=hp;run(px_battle_field_future_sight,k?0x1f:ME);
        want[0]=0;
        if(k){strcat(want,"dec ");if(!dec){strcat(want,"turn ");
            if((int32_t)ws[w]<=5){strcat(want,"at ");const uint32_t tg=ats[at];
                if(tg!=0x1f&&tg!=6){cat(want,sizeof want,"v%u/d ",tg&0xff,0);
                    if(hp)cat(want,sizeof want,"rec3/6 msg43e/42 arg%x arg10123 commit rec28/6 commit ",tg,0);}
                strcat(want,TAIL);
                if(ncommit)CHECK(commits[1][4]==6&&commits[1][5]==(tg&0xff)&&commits[1][6]==0x77&&c16(1,8)==0x123&&c32(1,0)==0x11);}}}
        EXPECT(want);++cases;
    }
    /* The entry handler: the battler entering the position. */
    for(unsigned a=0;a<3;++a)for(unsigned b=0;b<3;++b) {
        static const uint32_t ids[3]={ME,OTHER,0x100+ME};
        reset();at_value=ids[b];w32(NULL,WORK,0x1234);run(px_battle_field_entry,ids[a]);
        const int on=(ids[a]&0xff)==ids[b];EXPECT(on?"at rec2b/1f commit " TAIL:"at ");
        if(on){CHECK(commits[0][4]==0x34&&commits[0][5]==(ids[a]&0xff)&&c32(0,0)==0x11);}
        ++cases;
    }
    /* Healing Wish and Lunar Dance: full restoration of the battler entering the position. */
    for(unsigned h=0;h<2;++h)for(unsigned pos=0;pos<2;++pos)for(uint32_t hp=0;hp<3;++hp)for(uint32_t st=0;st<2;++st)for(uint32_t moves=0;moves<3;++moves)
    for(unsigned ppm=0;ppm<4;++ppm) {
        reset();position_value=pos?POS:POS+1;mons[ME].hp=hp;mons[ME].max_hp=2;mons[ME].status=st;w8(NULL,MON+0x200+0x152,(uint8_t)moves);
        mons[ME].pp[0]=ppm&1?3:0;mons[ME].pp[1]=ppm&2?0x105:0;
        run(h?px_battle_field_lunar_dance:px_battle_field_healing_wish,0x100+ME);
        strcpy(want,"pos2 ");
        if(pos){strcat(want,"v2/d v2/e st2 ");int need=hp!=2||st;
            if(h)for(uint32_t i=0;i<moves;++i){cat(want,sizeof want,"pp2/%u ",i,0);if(mons[ME].pp[i])need=1;}
            if(need){cat(want,sizeof want,"rec38/2 msg%x/c2 arg2 commit ",h?0x2b6:0x2b9,0);
                strcat(want,"v2/d v2/e ");if(hp!=2)strcat(want,"rec4/2 v2/e v2/d commit ");
                strcat(want,"st2 ");if(st)strcat(want,"reca/2 commit ");
                if(h)for(uint32_t i=0;i<moves;++i){cat(want,sizeof want,"pp2/%u ",i,0);if(mons[ME].pp[i])strcat(want,"rec8/2 commit ");}
                CHECK(c16(0,4)==(h?0x2bc:0x2bb)&&commits[0][6]==POS&&commits[0][7]==0xa&&commits[0][0xb]==(h?1:0xa5)&&c32(0,0)==0x11);
                unsigned n=1;if(hp!=2){CHECK(commits[n][6]==ME&&c16(n,4)==2-hp);++n;}
                if(st){CHECK(commits[n][4]==0x26&&commits[n][5]==ME&&commits[n][0x11]==1);++n;}
                if(h)for(uint32_t i=0;i<moves;++i)if(mons[ME].pp[i]){CHECK(commits[n][5]==ME&&commits[n][6]==i&&commits[n][4]==(mons[ME].pp[i]&0xff));++n;}
                CHECK(ncommit==n);}
            strcat(want,TAIL);}
        EXPECT(want);++cases;
    }
    /* Through the dispatcher: Healing Wish on event 0x5e. */
    {
        reset();position_value=POS;mons[ME].hp=1;mons[ME].max_hp=2;
        const uint32_t entries=0x7c00;w8(NULL,entries,0x5e);w32(NULL,entries+4,1);
        w32(NULL,HANDLER+8,entries);w32(NULL,HANDLER+0x18,0x20000000u|1u<<16);w8(NULL,HANDLER+0x3a,POS);
        w32(NULL,BATTLE+0x60,HANDLER);frame(ME);px_battle_event_dispatch(CTX,0x5e,&events);
        EXPECT("pos2 v2/d v2/e st2 rec38/2 msg2b9/c2 arg2 commit v2/d v2/e rec4/2 v2/e v2/d commit st2 " TAIL);CHECK(aborts==0);++cases;
    }
    printf("battle_abilities_11: %u cases, all tests passed\n",cases);
    return 0;
}
