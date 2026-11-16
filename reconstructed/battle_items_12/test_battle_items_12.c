#include "battle_items_12.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000,
       HANDLER=0x6000, WORK=0x601c, RECORD=0x7000, CURES=0x7800, SCRATCH=0x7900, ENTRIES=0x7a00, P=MON+0x200 };
enum { RUNNING=0x1000000, REENTRY=0x10000000, REGISTERED=0x20000000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
/* State: HP, max HP, ability, parameter, PP per slot (8 slots), restorable mask. */
static uint32_t hp,maxhp,ability,param,pp[8],restorable;static unsigned aborts,records;static char log_[512];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k)
{ (void)u;CHECK(p==P);notef("v%x ",k,0);if(k==0xd)return hp;if(k==0xe)return maxhp;CHECK(k==0x11);return ability; }
static const struct px_battle_ability_api base={.events=&events,.pokemon_value=pokemon_value};
static uint32_t item_param(void *u,uint32_t item,uint32_t k) { (void)u;CHECK(k==3);notef("ip%x ",item,0);return param; }
static void popup(void *u,uint32_t c,uint32_t p,uint32_t id) { (void)u;CHECK(c==CTX);notef("pop%u/%u ",p,id); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id)
{ (void)u;CHECK(c==CTX&&records<1);notef("rec%x/%x ",k,id);++records;return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;char t[48];snprintf(t,sizeof t,"msg%x@%x/%x ",id,r-RECORD,k);note(t); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;notef("arg%x@%x ",v,r-RECORD); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD);note("commit "); }
static const struct px_battle_items_api items={&base,NULL,item_param,popup,action_record,message,message_arg,commit,CURES,SCRATCH};
static uint32_t move_pp(void *u,uint32_t p,uint32_t s) { (void)u;CHECK(p==P&&s<8);notef("pp%u ",s,0);return pp[s]; }
static uint32_t pp_restorable(void *u,uint32_t p,uint32_t s) { (void)u;CHECK(p==P&&s<8);notef("pr%u ",s,0);return restorable>>s&1; }
static uint32_t slot_move(void *u,uint32_t p,uint32_t s) { (void)u;CHECK(p==P);notef("sm%u ",s,0);return 0x100+s; }
static uint32_t condition_word(void *u,uint32_t v) { (void)u;notef("cw%x ",v,0);return 0xc0de0000|v; }
static const struct px_battle_items_12_api api={&items,move_pp,pp_restorable,slot_move,condition_word};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_items_12_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_leppa_check_04,px_battle_leppa_eat};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);hp=20;maxhp=80;ability=0;param=4;restorable=0;aborts=0;records=0;log_[0]=0;
    for(unsigned i=0;i<8;++i)pp[i]=5;
    w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
    w16(NULL,HANDLER+0x38,0x9a);w32(NULL,HANDLER+0x18,RUNNING);
}
/* Four moves 0x21, 0x22, 0x23, 0x24, `count` of them, the last move used. */
static void moves(uint32_t count,uint32_t last)
{
    for(unsigned i=0;i<4;++i)w16(NULL,P+0x11c+0xe*i,(uint16_t)(0x21+i));
    w8(NULL,P+0x152,(uint8_t)count);w16(NULL,P+0x162,(uint16_t)last);
}
static void frame(uint32_t k2,uint32_t k0c,uint32_t k2f)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x02,k2,&events);px_battle_event_set_value(0x0c,k0c,&events);
    px_battle_event_set_value(0x2f,k2f,&events);px_battle_event_set_value(0x53,0,&events);log_[0]=0;
}
/* Reference: the first slot with no PP below the count (4 when none, or when it is slot 4). */
static uint32_t ref_empty(uint32_t count) { for(uint32_t i=0;i<count;++i)if(!pp[i])return i;return 4; }
static uint32_t ref_slot(uint32_t last) { for(uint32_t s=0;s<4;++s)if(last&&last==0x21+s)return s;return 4; }
static unsigned cases;
static void expect(const char *s) { if(strcmp(log_,s)){fprintf(stderr,"got  [%s]\nwant [%s]\n",log_,s);} CHECK(!strcmp(log_,s));++cases; }
/* PP layouts: all full, slot 0, 2 or 3 empty, two empty, slot 4 empty (beyond four moves). */
static const uint32_t layouts[][6]={{5,5,5,5,5,5},{0,5,5,5,5,5},{5,5,0,5,5,5},{5,5,5,0,5,5},{5,0,0,5,5,5},{5,5,5,5,0,5}};
static void set_pp(unsigned l) { for(unsigned i=0;i<6;++i)pp[i]=layouts[l][i]; }
int main(void)
{
    /* 1084c278 and 1084c224: every slot, move 0, absent moves and the halfwords around them. */
    {
        reset();moves(4,0x23);CHECK(px_battle_last_move(P,&memory)==0x23);++cases;
        for(uint32_t mv=0;mv<0x30;++mv){ CHECK(px_battle_move_slot(P,mv,&memory)==ref_slot(mv));++cases; }
        w16(NULL,P+0x11c+0xe,0x21);CHECK(px_battle_move_slot(P,0x21,&memory)==0);++cases;
        w16(NULL,P+0x11c,0);CHECK(px_battle_move_slot(P,0,&memory)==4&&px_battle_move_slot(P,0x21,&memory)==1);++cases;
    }
    /* Leppa's last-move checks (events 0x04 and 0x57). */
    {
        const uint32_t lasts[]={0,0x21,0x23,0x24,0x30};
        for(unsigned l=0;l<6;++l)for(unsigned k=0;k<5;++k)for(uint32_t holder=1;holder<4;++holder){
            const uint32_t s=ref_slot(lasts[k]);
            reset();set_pp(l);moves(4,lasts[k]);frame(holder,0,0);px_battle_leppa_check_04(HANDLER,CTX,2,WORK,&api);
            char b[64]="";const int ask=holder==2&&s!=4&&!pp[s];
            if(holder==2&&s!=4){ char c[16];snprintf(c,sizeof c,"pp%u ",s);strcat(b,c);if(ask)strcat(b,"pop0/2 "); }
            expect(b);CHECK(r32(NULL,HANDLER+0x18)==(ask?RUNNING|REENTRY:(uint32_t)RUNNING));
            reset();set_pp(l);moves(4,lasts[k]);frame(holder,0,0);px_battle_leppa_check_57(HANDLER,CTX,2,WORK,&api);
            b[0]=0;if(holder==2&&s!=4){ char c[16];snprintf(c,sizeof c,"pp%u ",s);strcat(b,c); }
            expect(b);CHECK(px_battle_event_get(0x53,&events)==(ask?1u:0u));
        }
    }
    /* Leppa's any-slot checks (events 0x5e/0xa9, 0x61, 0x9c) over layouts and move counts. */
    {
        static const fn h[]={px_battle_leppa_check_5e,px_battle_leppa_check_61,px_battle_leppa_check_9c};
        for(unsigned i=0;i<3;++i)for(unsigned l=0;l<6;++l)for(uint32_t count=0;count<=6;++count)for(uint32_t holder=1;holder<4;++holder)for(uint32_t f2f=0;f2f<4;f2f+=2){
            reset();set_pp(l);moves(count,0);frame(holder,0,f2f);h[i](HANDLER,CTX,2,WORK,&api);
            const int reach=i==1||(holder==2&&(i==0||(f2f&2)));
            char b[96]="";
            if(reach){ uint32_t e=0;while(e<count&&pp[e])++e;const uint32_t stop=e<count?e+1:count;
                for(uint32_t s=0;s<stop;++s){ char c[16];snprintf(c,sizeof c,"pp%u ",s);strcat(b,c); }
                if(e<count&&e!=4)strcat(b,"pop0/2 "); }
            expect(b);
        }
    }
    /* 108a9e1c: the last move's slot first, then any empty slot, then (with `any`) a restorable one. */
    {
        const uint32_t lasts[]={0,0x22,0x24};
        for(unsigned l=0;l<6;++l)for(unsigned k=0;k<3;++k)for(uint32_t count=0;count<=6;count+=2)for(uint32_t any=0;any<2;++any)for(uint32_t mask=0;mask<64;mask+=7){
            reset();set_pp(l);restorable=mask;moves(count,lasts[k]);frame(2,0,0);
            const uint32_t got=px_battle_leppa_use(HANDLER,CTX,2,any,&api);
            uint32_t slot=4;const uint32_t s=ref_slot(lasts[k]);
            if(s!=4&&!pp[s])slot=s;
            if(slot==4)slot=ref_empty(count);
            if(slot==4&&any){ uint32_t i=0;while(i<count&&!(mask>>i&1))++i;if(i<count&&i!=4)slot=i; }
            CHECK(got==(slot!=4));
            if(slot!=4){ CHECK(r8(NULL,RECORD+4)==0xa&&r8(NULL,RECORD+5)==2&&r8(NULL,RECORD+6)==slot&&r8(NULL,RECORD+7)==1);
                char want[96];snprintf(want,sizeof want,"rec8/2 msg395@8/2 arg2@8 arg9a@8 sm%u arg%x@8 commit ",slot,0x100+slot);
                CHECK(strstr(log_,want)!=NULL); }
            else CHECK(records==0);
            ++cases;
        }
        /* +7 keeps its other bits; the exact call sequence for one case. */
        reset();pp[1]=0;moves(4,0x22);w8(NULL,RECORD+7,0x80);frame(2,0,0);CHECK(px_battle_leppa_use(HANDLER,CTX,2,0,&api)==1);
        expect("pp1 rec8/2 msg395@8/2 arg2@8 arg9a@8 sm1 arg101@8 commit ");CHECK(r8(NULL,RECORD+7)==0x81);
        reset();restorable=4;moves(4,0x22);frame(2,0,0);CHECK(px_battle_leppa_use(HANDLER,CTX,2,1,&api)==1);
        expect("pp1 pp0 pp1 pp2 pp3 pr0 pr1 pr2 rec8/2 msg395@8/2 arg2@8 arg9a@8 sm2 arg102@8 commit ");
        reset();restorable=0x10;moves(6,0);frame(2,0,0);CHECK(px_battle_leppa_use(HANDLER,CTX,2,1,&api)==0);
        expect("pp0 pp1 pp2 pp3 pp4 pp5 pr0 pr1 pr2 pr3 pr4 ");
    }
    /* Leppa's uses: event 0x7b tests key 0x02; event 0x7c tests work word 6 only and allows any slot. */
    {
        for(uint32_t holder=1;holder<4;++holder)for(uint32_t sx=0;sx<2;++sx){
            reset();restorable=1;moves(4,0);frame(holder,0,0);px_battle_leppa_eat(HANDLER,CTX,2,WORK,&api);
            expect(holder==2?"pp0 pp1 pp2 pp3 ":"");
            reset();restorable=1;moves(4,0);w32(NULL,WORK+0x18,sx);frame(holder,0,0);px_battle_leppa_eat_7c(HANDLER,CTX,2,WORK,&api);
            expect(sx?"pp0 pp1 pp2 pp3 pr0 rec8/2 msg395@8/2 arg2@8 arg9a@8 sm0 arg100@8 commit ":"");
        }
    }
    /* Micle's checks: key 0x0c, the holder, key 0x2f bit 0, and the pinch with parameter 3's low byte. */
    {
        static const fn h[]={px_battle_micle_check_02,px_battle_micle_check_5e,px_battle_micle_check_61,px_battle_micle_check_9c};
        const uint32_t params[]={4,2,0x104};
        for(unsigned i=0;i<4;++i)for(uint32_t holder=1;holder<4;++holder)for(uint32_t k0c=5;k0c<8;++k0c)for(uint32_t f2f=0;f2f<2;++f2f)
        for(unsigned k=0;k<3;++k)for(uint32_t h_=19;h_<=41;h_+=11){
            reset();param=params[k];hp=h_;frame(holder,k0c,f2f);h[i](HANDLER,CTX,2,WORK,&api);
            const int reach=(i==0?(k0c!=6&&holder==2):i==1?holder==2:i==2?1:(holder==2&&f2f));
            const int pinch=h_<=80/(params[k]&0xff);
            char b[96]="";
            if(i==3){ if(holder==2){ strcat(b,"ip9a ");if(f2f){ strcat(b,"v11 ve vd ve ");if(pinch)strcat(b,"pop0/2 "); } } }
            else if(reach){ strcat(b,"ip9a v11 ve vd ve ");if(pinch)strcat(b,"pop0/2 "); }
            expect(b);
        }
    }
    /* Micle's uses: kind 0xb with status 0x22 and condition word 0x78. */
    {
        for(uint32_t holder=1;holder<4;++holder)for(uint32_t sx=0;sx<2;++sx){
            const char *want=holder==2?"recb/2 cw78 msg40a@14/2 arg2@14 arg9a@14 commit ":"";
            reset();frame(holder,0,0);px_battle_micle_use(HANDLER,CTX,2,WORK,&api);expect(want);
            if(holder==2){ CHECK(r8(NULL,RECORD+0xf)==2&&r16(NULL,RECORD+4)==0x22&&r32(NULL,RECORD+8)==0xc0de0078);
                for(uint32_t k=0;k<0x100;++k)if(k!=4&&k!=5&&k!=0xf&&(k<8||k>0xb))CHECK(r8(NULL,RECORD+k)==0); }
            reset();w32(NULL,WORK+0x18,sx);frame(holder,0,0);px_battle_micle_use_7c(HANDLER,CTX,2,WORK,&api);expect(sx?want:"");
        }
    }
    /* Through the dispatcher: Leppa's check on event 0x04, then its use on 0x7b. */
    {
        reset();pp[2]=0;moves(4,0x23);w32(NULL,HANDLER+0x18,REGISTERED|1u<<16);
        w8(NULL,ENTRIES,0x04);w32(NULL,ENTRIES+4,0);w8(NULL,ENTRIES+8,0x7b);w32(NULL,ENTRIES+12,1);
        w32(NULL,HANDLER+8,ENTRIES);w8(NULL,HANDLER+0x3a,2);w32(NULL,BATTLE+0x60,HANDLER);
        frame(2,0,0);px_battle_event_dispatch(CTX,0x04,&events);expect("pp2 pop0/2 ");
        w32(NULL,HANDLER+0x18,REGISTERED|1u<<16);w32(NULL,HANDLER+8,ENTRIES+8);log_[0]=0;px_battle_event_dispatch(CTX,0x7b,&events);
        expect("pp2 rec8/2 msg395@8/2 arg2@8 arg9a@8 sm2 arg102@8 commit ");CHECK(r8(NULL,RECORD+6)==2&&aborts==0);
    }
    printf("battle_items_12: %u cases, all tests passed\n",cases);
    return 0;
}
