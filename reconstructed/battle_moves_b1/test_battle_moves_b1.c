#include "battle_moves_b1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, FRAME=0x7400, ME=2, OTHER=5 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static uint32_t max_hp[8],contact_value,partner_found,partner_move,partner_id;
static unsigned aborts,ncommit;static uint8_t commits[4][0x20];static char log_[512];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static uint32_t value(void *u,uint32_t p,uint32_t k) { (void)u;notef("v%u/%x ",mon(p),k);CHECK(k==0xe);return max_hp[mon(p)]; }
static const struct px_battle_ability_api base={&events,value,NULL,NULL,NULL,NULL,NULL};
static uint32_t move_flag(void *u,uint32_t m,uint32_t f) { (void)u;notef("mf%x/%u ",m,f);return contact_value; }
static uint32_t pledge_partner(void *u,uint32_t p,uint32_t po,uint32_t mo) { (void)u;CHECK(po==FRAME+4&&mo==FRAME);notef("partner%u ",mon(p),0);
    if(partner_found){w8(NULL,po,(uint8_t)partner_id);w16(NULL,mo,(uint16_t)partner_move);}
    return partner_found; }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);
    memset(mem+RECORD,0xa5,0x20);w32(NULL,RECORD,0x11);w8(NULL,RECORD+7,0xff);w8(NULL,RECORD+0x18,0x41);return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;notef("msg%x/%x ",id,(r-RECORD)*16+k); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;(void)r;notef("arg%x ",v,0); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD&&ncommit<4);memcpy(commits[ncommit++],mem+RECORD,0x20);note("commit "); }
static const struct px_battle_moves_b1_api api={&base,move_flag,pledge_partner,action_record,message,message_arg,commit,FRAME};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_b1_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_spiky_shield_hit,px_battle_kings_shield_hit};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(max_hp,0,sizeof max_hp);memset(commits,0,sizeof commits);aborts=ncommit=0;log_[0]=0;
    contact_value=partner_found=partner_move=partner_id=0;
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
static void frame(uint32_t k3,uint32_t k4,uint32_t type)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x03,k3,&events);px_battle_event_set_id(0x04,k4,&events);
    px_battle_event_set_value(0x12,0x10088,&events);px_battle_event_set_value(0x16,type,&events);
    px_battle_event_set_value(0x53,0,&events);px_battle_event_set_value(0x59,0x77,&events);log_[0]=0;
}
static uint32_t get(uint32_t key) { return px_battle_event_get(key,&events); }
static void run(fn f,uint32_t k3,uint32_t k4,uint32_t type) { frame(k3,k4,type);f(HANDLER,CTX,ME,WORK,&api);if(aborts)fprintf(stderr,"log %s\n",log_);CHECK(aborts==0); }
static uint32_t c16(unsigned n,unsigned o) { return (uint32_t)(commits[n][o]|commits[n][o+1]<<8); }
static uint32_t c32(unsigned n,unsigned o) { return c16(n,o)|c16(n,o+2)<<16; }
#define EXPECT(w) do { if(strcmp(log_,w)){fprintf(stderr,"got  %s\nwant %s\n",log_,w);CHECK(0);} } while (0)
int main(void)
{
    unsigned cases=0;
    static const uint32_t ids[3]={ME,OTHER,0x100+ME};
    /* Final Gambit: the user faints once key 0x53 is rewritten. */
    for(unsigned a=0;a<3;++a)for(unsigned pre=0;pre<2;++pre) {
        reset();frame(ids[a],OTHER,0);if(pre)px_battle_event_rewrite(0x53,0,&events);log_[0]=0;
        px_battle_final_gambit_faint(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0);
        const int on=ids[a]==ME&&!pre;EXPECT(on?"rec12/2 commit ":"");CHECK(get(0x53)==(ids[a]==ME&&!pre?1u:0u));
        if(on){CHECK(commits[0][4]==ME&&c32(0,0)==0x11&&commits[0][5]==0xa5);}
        ++cases;
    }
    /* Whirlwind and Roar: the message key 0x59 by the handler's move. */
    for(unsigned a=0;a<3;++a)for(unsigned mv=0;mv<3;++mv) {
        static const uint32_t moves[3]={0x2e,0x12,0x12e};
        reset();w16(NULL,HANDLER+0x38,(uint16_t)moves[mv]);run(px_battle_roar_message,ids[a],OTHER,0);
        CHECK(log_[0]==0&&get(0x59)==(ids[a]==ME?(moves[mv]==0x2e?0x2c9u:0x2cau):0x77u));++cases;
    }
    /* Spiky Shield and King's Shield: contact moves into the protected battler. */
    for(unsigned a=0;a<3;++a)for(uint32_t c=0;c<2;++c)for(uint32_t mx=0;mx<20;mx+=3) {
        reset();contact_value=c;max_hp[OTHER]=mx*7;run(px_battle_spiky_shield_hit,0x100+OTHER,ids[a],0);
        const int on=ids[a]==ME&&c;
        if(ids[a]!=ME)EXPECT("");else if(!c)EXPECT("mf88/0 ");else EXPECT("mf88/0 rec6/2 v5/e msg4c0/c2 arg5 commit ");
        if(on){CHECK(commits[0][6]==OTHER&&c16(0,4)==(mx*7/8?mx*7/8:1)&&commits[0][7]==0xfe&&c32(0,0)==0x11);}
        reset();contact_value=c;run(px_battle_kings_shield_hit,0x100+OTHER,ids[a],0);
        if(ids[a]!=ME)EXPECT("");else if(!c)EXPECT("mf88/0 ");else EXPECT("mf88/0 recd/2 commit ");
        if(on){CHECK(commits[0][4]==1&&commits[0][0x17]==0xfe&&commits[0][0x16]==1&&commits[0][0x18]==0x57&&commits[0][0xc]==OTHER&&c32(0,0)==0x11);}
        cases+=2;
    }
    /* The Pledges: every pair of moves, with and without a partner. */
    static const uint32_t partners[7]={0x205,0x206,0x207,0x208,0x209,0x306,0x407};
    for(unsigned a=0;a<3;++a)for(unsigned found=0;found<2;++found)for(uint32_t mine=0x205;mine<=0x209;++mine)for(unsigned pi=0;pi<7;++pi) {
        const uint32_t theirs=partners[pi];
        reset();w16(NULL,FRAME,0x207);w8(NULL,FRAME+4,4);w16(NULL,HANDLER+0x38,(uint16_t)mine);partner_found=found;partner_move=theirs;partner_id=3;w32(NULL,WORK,9);w32(NULL,WORK+4,9);w32(NULL,WORK+8,9);
        run(px_battle_pledge_combine,ids[a],OTHER,0x10c);
        uint32_t combo=0;
        if((mine==0x206&&theirs==0x207)||(theirs==0x206&&mine==0x207))combo=1;
        else if((mine==0x208&&theirs==0x207)||(theirs==0x208&&mine==0x207))combo=2;
        else if((mine==0x206&&theirs==0x208)||(theirs==0x206&&mine==0x208))combo=3;
        EXPECT(ids[a]==ME?"partner2 ":"");
        const int on=ids[a]==ME&&found&&combo;static const uint32_t types[4]={0,0xa,9,0xb};
        CHECK(get(0x16)==(on?types[combo]:0x10cu));
        CHECK(r32(NULL,WORK)==(on?1u:9u)&&r32(NULL,WORK+4)==(on?combo:9u)&&r32(NULL,WORK+8)==(on?3u:9u));
        ++cases;
    }
    /* Through the dispatcher: Spiky Shield then King's Shield on event 0xb1. */
    {
        reset();contact_value=1;max_hp[OTHER]=80;
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7c00+0x10*i;
            w8(NULL,entries,0xb1);w32(NULL,entries+4,i);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ME);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);frame(OTHER,ME,0);px_battle_event_dispatch(CTX,0xb1,&events);
        EXPECT("mf88/0 rec6/2 v5/e msg4c0/c2 arg5 commit mf88/0 recd/2 commit ");CHECK(aborts==0&&c16(0,4)==10);++cases;
    }
    printf("battle_moves_b1: %u cases, all tests passed\n",cases);
    return 0;
}
