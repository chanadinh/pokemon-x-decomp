#include "battle_items_11.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x5100, MON=0x8000,
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
/* Answers: opposing selector, can switch, decided, charging, stage room. */
static uint32_t selector,switchable,decided,charging,room;static unsigned aborts,records;static char log_[512];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static const struct px_battle_ability_api base={.events=&events};
static void popup(void *u,uint32_t c,uint32_t p,uint32_t id) { (void)u;CHECK(c==CTX);notef("pop%u/%u ",p,id); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id)
{ (void)u;CHECK(c==CTX&&records<2);notef("rec%x/%x ",k,id);return RECORD+0x100*records++; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;char t[48];snprintf(t,sizeof t,"msg%x@%x/%x ",id,(r-RECORD)&0xff,k);note(t); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;notef("arg%x@%x ",v,(r-RECORD)&0xff); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r>=RECORD&&!(r&0xff));notef("commit%u ",(r-RECORD)>>8,0); }
static const struct px_battle_items_api items={&base,NULL,NULL,popup,action_record,message,message_arg,commit,CURES,SCRATCH};
static uint32_t opposing_selector(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("os%u ",id,0);return selector; }
static uint32_t can_switch(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("cs%u ",id,0);return switchable; }
static uint32_t battle_decided(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("dec ");return decided; }
static uint32_t is_charging(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("ch%u ",id,0);return charging; }
static uint32_t stage_can_change(void *u,uint32_t p,uint32_t s,int32_t d) { (void)u;CHECK(p==P&&d==1);notef("sc%u ",s,0);return room; }
static const struct px_battle_items_11_api api={&items,opposing_selector,can_switch,battle_decided,is_charging,stage_can_change};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_items_11_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_eject_button_check,px_battle_eject_button_use};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);selector=1;switchable=1;decided=0;charging=0;room=1;aborts=0;records=0;log_[0]=0;
    w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
    w16(NULL,HANDLER+0x38,0x21e);w32(NULL,HANDLER+0x18,RUNNING);
}
/* The battlers hit: n of them from `list`; key 0x02, 0x03, 0x4e and 0x1b. */
static void frame(uint32_t n,const uint32_t *list,uint32_t k2,uint32_t k3,uint32_t k4e,uint32_t k1b)
{
    px_battle_event_push(&events);px_battle_event_set_value(0x05,n,&events);
    for(uint32_t i=0;i<n;++i)px_battle_event_set_value(6+i,list[i],&events);
    px_battle_event_set_id(0x02,k2,&events);px_battle_event_set_id(0x03,k3,&events);
    px_battle_event_set_value(0x4e,k4e,&events);px_battle_event_set_value(0x1b,k1b,&events);log_[0]=0;
}
static int listed(uint32_t n,const uint32_t *list) { for(uint32_t i=0;i<n;++i)if(list[i]==2)return 1;return 0; }
static unsigned cases;
static void expect(const char *s) { if(strcmp(log_,s)){fprintf(stderr,"got  [%s]\nwant [%s]\n",log_,s);} CHECK(!strcmp(log_,s));++cases; }
/* Lists of battlers hit: none, the holder alone, others, the holder later, the holder in a high byte. */
static const uint32_t lists[][4]={{0},{2},{1},{3,4},{1,3,2},{5,2,2,1},{0x102},{1,4,5,6}};
static const uint32_t counts[]={0,1,1,2,3,4,1,4};
int main(void)
{
    /* 1084c000 and 1084c020 over every value of context +0x1093, with the bytes around it set. */
    {
        for(uint32_t v=0;v<256;++v){
            reset();w8(NULL,CTX+0x1093,(uint8_t)v);w8(NULL,CTX+0x1092,0x55);w8(NULL,CTX+0x1094,0x66);
            CHECK(px_battle_switch_pending(CTX,&memory)==(v&1));++cases;
            CHECK(px_battle_claim_eject(CTX,&memory)==!(v&8)&&r8(NULL,CTX+0x1093)==(v|8));++cases;
            CHECK(px_battle_claim_eject(CTX,&memory)==0&&r8(NULL,CTX+0x1093)==(v|8));++cases;
            CHECK(r8(NULL,CTX+0x1092)==0x55&&r8(NULL,CTX+0x1094)==0x66);
        }
    }
    /* Red Card: the list, the selector, key 0x4e, the attacker's replacement, decided. */
    {
        for(unsigned l=0;l<8;++l)for(uint32_t sel=0;sel<2;++sel)for(uint32_t k4e=0;k4e<2;++k4e)for(uint32_t sw=0;sw<2;++sw)for(uint32_t d=0;d<2;++d){
            reset();selector=sel?0xa:1;switchable=sw;decided=d;memset(mem+RECORD+0x100,0,8);w8(NULL,RECORD+0x107,0xe5);
            frame(counts[l],lists[l],7,0x105,k4e,0);px_battle_red_card(HANDLER,CTX,2,WORK,&api);
            char b[192]="";
            if(listed(counts[l],lists[l])){ strcat(b,"os5 ");
                if(!sel&&!k4e){ strcat(b,"cs5 ");
                    if(sw){ strcat(b,"dec ");
                        if(!d)strcat(b,"rec24/2 msg1a1@8/2 arg2@8 arg5@8 commit0 rec2f/2 commit1 "); } } }
            expect(b);
            if(strstr(b,"rec2f")){ CHECK(r16(NULL,RECORD+0x104)==0x2a7&&r8(NULL,RECORD+0x106)==5&&r8(NULL,RECORD+0x107)==0x15); }
        }
    }
    /* Eject Button: the list, then every early return in order. */
    {
        for(unsigned l=0;l<8;++l)for(uint32_t k4e=0;k4e<2;++k4e)for(uint32_t ch=0;ch<2;++ch)for(uint32_t d=0;d<2;++d)
        for(uint32_t pend=0;pend<2;++pend)for(uint32_t sw=0;sw<2;++sw)for(uint32_t claimed=0;claimed<2;++claimed){
            reset();charging=ch;decided=d;switchable=sw;w8(NULL,CTX+0x1093,(uint8_t)(pend|claimed<<3|0x40));
            frame(counts[l],lists[l],7,7,k4e,0);px_battle_eject_button_check(HANDLER,CTX,2,WORK,&api);
            char b[96]="";int ask=0;
            if(listed(counts[l],lists[l])&&!k4e){ strcat(b,"ch2 ");
                if(!ch){ strcat(b,"dec ");
                    if(!d&&!pend){ strcat(b,"cs2 ");
                        if(sw&&!claimed){ strcat(b,"pop0/2 ");ask=1; } } } }
            expect(b);
            CHECK(r32(NULL,HANDLER+0x18)==(ask?RUNNING|REENTRY:(uint32_t)RUNNING));
            const int reached_claim=listed(counts[l],lists[l])&&!k4e&&!ch&&!d&&!pend&&sw;
            CHECK(r8(NULL,CTX+0x1093)==((pend|claimed<<3|0x40)|(reached_claim?8:0)));
        }
        for(uint32_t holder=1;holder<4;++holder)for(uint32_t sw=0;sw<2;++sw){
            reset();switchable=sw;frame(0,lists[0],holder,7,0,0);px_battle_eject_button_use(HANDLER,CTX,2,WORK,&api);
            char b[96]="";if(holder==2){ strcat(b,"cs2 ");if(sw)strcat(b,"rec3/2 msg19e@4/2 arg2@4 commit0 rec2a/2 commit1 "); }
            expect(b);
            if(holder==2&&sw){ CHECK(r8(NULL,RECORD+0x14c)==2);for(uint32_t k=0;k<0x100;++k)if(k!=0x4c)CHECK(r8(NULL,RECORD+0x100+k)==0); }
        }
    }
    /* Kee and Maranga: the list, the category, the stage. */
    {
        for(unsigned berry=0;berry<2;++berry)for(unsigned l=0;l<8;++l)for(uint32_t cat=0;cat<4;++cat)for(uint32_t r=0;r<2;++r){
            reset();room=r;frame(counts[l],lists[l],7,7,0,cat);
            if(berry)px_battle_maranga_check(HANDLER,CTX,2,WORK,&api); else px_battle_kee_check(HANDLER,CTX,2,WORK,&api);
            const uint32_t stat=berry?4:2;char b[48]="";
            if(listed(counts[l],lists[l])&&cat==berry+1u){ char c[16];snprintf(c,sizeof c,"sc%u ",stat);strcat(b,c);if(r)strcat(b,"pop0/2 "); }
            expect(b);
        }
        for(unsigned berry=0;berry<2;++berry)for(uint32_t holder=1;holder<4;++holder)for(uint32_t r=0;r<2;++r){
            reset();room=r;frame(0,lists[0],holder,7,0,0);
            if(berry)px_battle_maranga_use(HANDLER,CTX,2,WORK,&api); else px_battle_kee_use(HANDLER,CTX,2,WORK,&api);
            const uint32_t stat=berry?4:2;char b[48]="";
            if(holder==2){ char c[16];snprintf(c,sizeof c,"sc%u ",stat);strcat(b,c);if(r)strcat(b,"recd/2 commit0 "); }
            expect(b);
            if(holder==2&&r){ CHECK(r8(NULL,RECORD+4)==stat&&r8(NULL,RECORD+0xc)==2&&r8(NULL,RECORD+0x16)==1&&r8(NULL,RECORD+0x17)==1);
                for(uint32_t k=0;k<0x100;++k)if(k!=4&&k!=0xc&&k!=0x16&&k!=0x17)CHECK(r8(NULL,RECORD+k)==0); }
        }
    }
    /* Through the dispatcher: Eject Button on 0x8c, then its use on 0x7b. */
    {
        reset();w32(NULL,HANDLER+0x18,REGISTERED|1u<<16);
        w8(NULL,ENTRIES,0x8c);w32(NULL,ENTRIES+4,0);w8(NULL,ENTRIES+8,0x7b);w32(NULL,ENTRIES+12,1);
        w32(NULL,HANDLER+8,ENTRIES);w8(NULL,HANDLER+0x3a,2);w32(NULL,BATTLE+0x60,HANDLER);
        const uint32_t hit[]={3,2};frame(2,hit,2,3,0,1);px_battle_event_dispatch(CTX,0x8c,&events);
        expect("ch2 dec cs2 pop0/2 ");CHECK(r8(NULL,CTX+0x1093)==8);
        w32(NULL,HANDLER+0x18,REGISTERED|1u<<16);w32(NULL,HANDLER+8,ENTRIES+8);log_[0]=0;px_battle_event_dispatch(CTX,0x7b,&events);
        expect("cs2 rec3/2 msg19e@4/2 arg2@4 commit0 rec2a/2 commit1 ");CHECK(aborts==0);
    }
    printf("battle_items_11: %u cases, all tests passed\n",cases);
    return 0;
}
