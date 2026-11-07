#include "battle_items_1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, CURES=0x7800, SCRATCH=0x7900, ENTRIES=0x7a00 };
enum { RUNNING=0x1000000, REENTRY=0x10000000, REGISTERED=0x20000000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
/* Battler state: HP, max HP, ability, status, conditions (bit n: condition n). */
static uint32_t hp,maxhp,ability,status_value,conditions,param;static unsigned aborts;static char log_[1024];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k)
{
    (void)u;CHECK(p==MON+0x200);notef("v%x ",k,0);
    if(k==0xd)return hp;
    if(k==0xe)return maxhp;
    CHECK(k==0x11);return ability;
}
static uint32_t pokemon_status(void *u,uint32_t p) { (void)u;CHECK(p==MON+0x200);note("st ");return status_value; }
static const struct px_battle_ability_api base={.events=&events,.pokemon_value=pokemon_value,.pokemon_status=pokemon_status};
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;CHECK(p==MON+0x200&&s<32);notef("hs%x ",s,0);return conditions>>s&1; }
static uint32_t item_param(void *u,uint32_t item,uint32_t k) { (void)u;CHECK(k==3);notef("ip%x ",item,0);return param; }
static void popup(void *u,uint32_t c,uint32_t p,uint32_t id) { (void)u;CHECK(c==CTX);notef("pop%u/%u ",p,id); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id)
{ (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);memset(mem+RECORD,0,0x60);return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;notef("msg%x/%x ",id,(r-RECORD)*16+k); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;CHECK(r==RECORD+8);notef("arg%x ",v,0); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD);note("commit "); }
static const struct px_battle_items_api api={&base,has_status,item_param,popup,action_record,message,message_arg,commit,CURES,SCRATCH};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_items_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_lum_check,px_battle_lum_use,px_battle_hp_berry_9c};
    CHECK(f<3);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);hp=10;maxhp=40;ability=0;status_value=0;conditions=0;param=10;aborts=0;log_[0]=0;
    w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
    w16(NULL,HANDLER+0x38,0x9e);w32(NULL,SCRATCH,0xdeadbeef);
}
static void frame(uint32_t id) { px_battle_event_push(&events);px_battle_event_set_id(0x02,id,&events);log_[0]=0; }
static unsigned cases;
static void expect(const char *s) { if(strcmp(log_,s)){fprintf(stderr,"got  [%s]\nwant [%s]\n",log_,s);} CHECK(!strcmp(log_,s));++cases; }
/* The reference for one cure: what the battler state says. */
static int needs(uint32_t cure)
{
    if(cure==0x26)return status_value!=0;
    if(cure==0x27)return status_value!=0||(conditions>>6&1);
    if(cure==0x28){ for(unsigned i=0;i<6;++i){ uint32_t c=r16(NULL,CURES+2*i);if(!c)return 0;if(r32(NULL,MON+0x200+0x1c+4*c)&7)return 1; } return 0; }
    return conditions>>cure&1;
}
static const char *probe(uint32_t cure)
{
    static char b[64];
    if(cure==0x26)return "st ";
    if(cure==0x27){ snprintf(b,sizeof b,"st %s",status_value?"":"hs6 ");return b; }
    if(cure==0x28)return "";
    snprintf(b,sizeof b,"hs%x ",cure);return b;
}
int main(void)
{
    /* 1087bfa0: the current frame's key, absent keys, keys 0 and 1, a full stack. */
    {
        reset();frame(2);px_battle_event_set_value(0x2f,0x1234,&events);px_battle_event_set_value(0x30,7,&events);
        CHECK(px_battle_event_find(0x2f,SCRATCH,&events)==1&&r32(NULL,SCRATCH)==0x1234);++cases;
        CHECK(px_battle_event_find(0x30,SCRATCH,&events)==1&&r32(NULL,SCRATCH)==7);++cases;
        CHECK(px_battle_event_find(0x02,SCRATCH,&events)==1&&r32(NULL,SCRATCH)==2);++cases;
        w32(NULL,SCRATCH,0x55);CHECK(px_battle_event_find(0x31,SCRATCH,&events)==0&&r32(NULL,SCRATCH)==0x55&&aborts==0);++cases;
        /* An outer frame's key is not seen. */
        px_battle_event_push(&events);px_battle_event_set_id(0x02,3,&events);
        CHECK(px_battle_event_find(0x2f,SCRATCH,&events)==0&&r32(NULL,SCRATCH)==0x55);++cases;
        CHECK(px_battle_event_find(0x02,SCRATCH,&events)==1&&r32(NULL,SCRATCH)==3);++cases;
        /* Key 0 faults, then matches the frame's first empty slot; key 1 faults and is absent. */
        aborts=0;w32(NULL,STACK+0xc4+4*(r32(NULL,STACK)+1),0x77);
        CHECK(px_battle_event_find(0,SCRATCH,&events)==1&&aborts==1&&r32(NULL,SCRATCH)==0x77);++cases;
        aborts=0;w32(NULL,SCRATCH,0x55);CHECK(px_battle_event_find(1,SCRATCH,&events)==0&&aborts==1&&r32(NULL,SCRATCH)==0x55);++cases;
        /* Key 1 still matches a marker after the abort. */
        aborts=0;w16(NULL,STACK+4+2*r32(NULL,STACK),1);
        CHECK(px_battle_event_find(1,SCRATCH,&events)==1&&aborts==1&&r32(NULL,SCRATCH)==3);++cases;
        /* No terminator up to the last slot; then a position at the end. */
        reset();for(unsigned i=0;i<0x60;++i){ w16(NULL,STACK+4+2*i,(uint16_t)(0x100+i));w32(NULL,STACK+0xc4+4*i,i); }
        w32(NULL,STACK,0x10);
        CHECK(px_battle_event_find(0x15f,SCRATCH,&events)==1&&r32(NULL,SCRATCH)==0x5f);++cases;
        CHECK(px_battle_event_find(0x105,SCRATCH,&events)==0);++cases;
        CHECK(px_battle_event_find(0x2f,SCRATCH,&events)==0);++cases;
        /* Slot 0x60 would read the first value word as a key. */
        w32(NULL,STACK+0xc4,0x15f);
        w32(NULL,STACK,0x60);CHECK(px_battle_event_find(0x15f,SCRATCH,&events)==0);++cases;
        w32(NULL,STACK,0x61);CHECK(px_battle_event_find(0x15f,SCRATCH,&events)==0);++cases;
        w32(NULL,STACK,0x5f);CHECK(px_battle_event_find(0x15f,SCRATCH,&events)==1);++cases;
        /* An entry after the terminator is not seen. */
        w32(NULL,STACK,0x10);w16(NULL,STACK+4+2*0x20,0);
        CHECK(px_battle_event_find(0x130,SCRATCH,&events)==0&&px_battle_event_find(0x121,SCRATCH,&events)==0);++cases;
        CHECK(px_battle_event_find(0x11f,SCRATCH,&events)==1&&r32(NULL,SCRATCH)==0x1f);++cases;
        CHECK(aborts==0);
    }
    /* 1084c96c: only a running handler is marked; other flags are kept. */
    {
        const uint32_t in[]={0,RUNNING,RUNNING|0x2000ffff,0x20000001,REENTRY,RUNNING|REENTRY,0xfeffffff,0xffffffff};
        for(unsigned i=0;i<8;++i){ reset();w32(NULL,HANDLER+0x18,in[i]);px_battle_handler_mark_reentry(HANDLER,&events);
            CHECK(r32(NULL,HANDLER+0x18)==((in[i]&RUNNING)?in[i]|REENTRY:in[i]));++cases; }
    }
    /* 1084c844: the six-entry list. */
    {
        reset();const uint16_t list[]={7,0x14,0x1e,0x1d,0x1c,0x25};
        for(unsigned i=0;i<6;++i){ w16(NULL,CURES+2*i,list[i]); }
        w16(NULL,CURES+12,9);
        CHECK(px_battle_listed_condition(MON+0x200,&api)==0);++cases;
        for(unsigned i=0;i<6;++i)for(uint32_t bits=0;bits<16;++bits){
            reset();for(unsigned j=0;j<6;++j)w16(NULL,CURES+2*j,list[j]);w16(NULL,CURES+12,9);
            w32(NULL,MON+0x200+0x1c+4*9,7);w32(NULL,MON+0x200+0x1c+4*list[i],bits);
            CHECK(px_battle_listed_condition(MON+0x200,&api)==((bits&7)?list[i]:0u)&&aborts==0);++cases; }
        /* A zero entry ends the list. */
        reset();w16(NULL,CURES,7);w16(NULL,CURES+2,0);w16(NULL,CURES+4,9);w32(NULL,MON+0x200+0x1c+4*9,1);
        CHECK(px_battle_listed_condition(MON+0x200,&api)==0);++cases;
        reset();CHECK(px_battle_listed_condition(MON+0x200,&api)==0);++cases;
        /* Entries from 0x26 fault and are read anyway. */
        reset();w16(NULL,CURES,0x26);w16(NULL,CURES+2,0x30);w32(NULL,MON+0x200+0x1c+4*0x30,4);
        CHECK(px_battle_listed_condition(MON+0x200,&api)==0x30&&aborts==2);++cases;
        reset();w16(NULL,CURES,0x25);w32(NULL,MON+0x200+0x1c+4*0x25,2);
        CHECK(px_battle_listed_condition(MON+0x200,&api)==0x25&&aborts==0);++cases;
    }
    /* 1084c730: every divisor, HP, max HP and three abilities. */
    {
        const uint32_t abilities[]={0x52,0x51,0x152};
        for(unsigned a=0;a<3;++a)for(uint32_t d=0;d<10;++d)for(uint32_t mx=0;mx<14;++mx)for(uint32_t h=0;h<16;++h){
            reset();ability=abilities[a];maxhp=mx;hp=h;
            uint32_t e=d;if(ability==0x52&&e>2)e>>=1;
            const int faulted=e==0;if(faulted)e=1;
            const uint32_t want=mx>1&&h<=mx/e;
            CHECK(px_battle_item_pinch(CTX,2,d,&api)==want&&aborts==(unsigned)faulted);
            char b[64];snprintf(b,sizeof b,"v11 %sve %s",faulted?"ABORT ":"",mx>1?"vd ve ":"");expect(b); }
    }
    /* 1084c198: key 0x02, the flag and key 0x2f bit 0. */
    {
        for(uint32_t flag=0;flag<3;++flag)for(uint32_t use=0;use<4;++use)for(uint32_t low=0;low<2;++low)for(uint32_t who=1;who<3;++who){
            reset();hp=low?5:30;frame(who);px_battle_event_set_value(0x2f,use,&events);
            w32(NULL,HANDLER+0x18,RUNNING);
            px_battle_hp_berry_check(HANDLER,CTX,2,2,flag,&api);
            const int pass=who==2&&(!flag||(use&1));
            char b[96];snprintf(b,sizeof b,"%s%s",pass?"v11 ve vd ve ":"",pass&&low?"pop0/2 ":"");expect(b);
            CHECK(r32(NULL,HANDLER+0x18)==(pass&&low?RUNNING|REENTRY:(uint32_t)RUNNING)&&aborts==0); }
        /* A missing key 0x2f faults only with the flag. */
        reset();hp=5;frame(2);px_battle_hp_berry_check(HANDLER,CTX,2,4,0,&api);expect("v11 ve vd ve pop0/2 ");
        reset();hp=5;frame(2);px_battle_hp_berry_check(HANDLER,CTX,2,4,1,&api);expect("ABORT ");CHECK(aborts==1);
        /* The divisor reaches the pinch test. */
        reset();hp=10;maxhp=40;frame(2);px_battle_hp_berry_check(HANDLER,CTX,2,4,0,&api);expect("v11 ve vd ve pop0/2 ");
        reset();hp=11;maxhp=40;frame(2);px_battle_hp_berry_check(HANDLER,CTX,2,4,0,&api);expect("v11 ve vd ve ");
        reset();hp=11;maxhp=40;ability=0x52;frame(2);px_battle_hp_berry_check(HANDLER,CTX,2,4,0,&api);expect("v11 ve vd ve pop0/2 ");
    }
    /* 1084cc48 and 1084c4c0 over every cure 0..0x28, statuses and conditions. */
    {
        const uint16_t list[]={7,0x14,0,9,9,9};
        const uint32_t conds[]={0,1u<<6,0xffffffffu,0x2,0x24,1u<<0x1f};
        for(uint32_t cure=0;cure<=0x28;++cure)for(unsigned c=0;c<6;++c)for(uint32_t st=0;st<2;++st)for(unsigned listed=0;listed<3;++listed){
            for(unsigned use=0;use<2;++use){
                reset();if(cure>=0x20&&cure<0x26)continue;
                conditions=conds[c];status_value=st?3:0;for(unsigned j=0;j<6;++j)w16(NULL,CURES+2*j,list[j]);
                if(listed)w32(NULL,MON+0x200+0x1c+4*(listed==1?7:0x14),listed);
                frame(2);w32(NULL,HANDLER+0x18,RUNNING);
                if(use)px_battle_cure_berry_use(HANDLER,CTX,2,cure,&api); else px_battle_cure_berry_check(HANDLER,CTX,2,cure,&api);
                const int n=needs(cure);char b[128];
                if(use)snprintf(b,sizeof b,"%s%s",probe(cure),n?"reca/2 commit ":"");
                else snprintf(b,sizeof b,"%s%s",probe(cure),n?"pop0/2 ":"");
                expect(b);
                CHECK(r32(NULL,HANDLER+0x18)==(!use&&n?RUNNING|REENTRY:(uint32_t)RUNNING));
                if(use&&n){ CHECK(r8(NULL,RECORD+4)==cure&&r8(NULL,RECORD+5)==2&&r8(NULL,RECORD+0x11)==1&&r8(NULL,RECORD+6)==0&&r8(NULL,RECORD+0x10)==0&&r32(NULL,RECORD)==0); }
                CHECK(aborts==0); } }
        /* Neither acts for another battler. */
        for(unsigned use=0;use<2;++use){ reset();status_value=1;conditions=~0u;frame(3);
            if(use)px_battle_cure_berry_use(HANDLER,CTX,2,0x27,&api); else px_battle_cure_berry_check(HANDLER,CTX,2,0x27,&api);expect(""); }
    }
    /* The check's key 0x2f: absent, bit 2 set or clear. */
    {
        for(uint32_t v=0;v<16;++v)for(unsigned present=0;present<2;++present){
            reset();conditions=1u<<1;frame(2);if(present)px_battle_event_set_value(0x2f,v|0x100,&events);
            px_battle_cure_berry_check(HANDLER,CTX,2,1,&api);
            const int pass=!present||(v&4);expect(pass?"hs1 pop0/2 ":"");
            CHECK(r32(NULL,SCRATCH)==(present?(v|0x100):0xdeadbeefu)); }
        /* The use does not consult it. */
        reset();conditions=1u<<1;frame(2);px_battle_event_set_value(0x2f,0,&events);px_battle_cure_berry_use(HANDLER,CTX,2,1,&api);
        expect("hs1 reca/2 commit ");
    }
    /* Each berry's handlers pass their cure. */
    {
        const fn check[]={px_battle_cheri_check,px_battle_chesto_check,px_battle_pecha_check,px_battle_rawst_check,px_battle_aspear_check,px_battle_persim_check,px_battle_lum_check};
        const fn use[]={px_battle_cheri_use,px_battle_chesto_use,px_battle_pecha_use,px_battle_rawst_use,px_battle_aspear_use,px_battle_persim_use,px_battle_lum_use};
        const uint32_t cure[]={1,2,5,4,3,6,0x27};
        for(unsigned i=0;i<7;++i)for(unsigned on=0;on<2;++on)for(unsigned id=0;id<2;++id){
            reset();conditions=on?~0u:0;frame(2);check[i](HANDLER,CTX,2+id,WORK,&api);
            char b[64];
            if(id){ expect(""); }
            else { if(i==6)snprintf(b,sizeof b,"st hs6 %s",on?"pop0/2 ":""); else snprintf(b,sizeof b,"hs%x %s",cure[i],on?"pop0/2 ":"");expect(b); }
            reset();conditions=on?~0u:0;frame(2);use[i](HANDLER,CTX,2+id,WORK,&api);
            if(id){ expect(""); }
            else { if(i==6)snprintf(b,sizeof b,"st hs6 %s",on?"reca/2 commit ":""); else snprintf(b,sizeof b,"hs%x %s",cure[i],on?"reca/2 commit ":"");expect(b);
                if(on)CHECK(r8(NULL,RECORD+4)==cure[i]); } }
    }
    /* The HP berries' checks: condition 0xf blocks them. */
    {
        for(unsigned blockedf=0;blockedf<2;++blockedf)for(unsigned low=0;low<2;++low)for(unsigned use=0;use<2;++use)for(uint32_t who=2;who<4;++who){
            char b[96];
            reset();conditions=blockedf?1u<<0xf:0;hp=low?20:21;frame(who);px_battle_event_set_value(0x2f,use,&events);
            px_battle_hp_berry_9c(HANDLER,CTX,2,WORK,&api);
            snprintf(b,sizeof b,"hs%x %s%s",0xf,!blockedf&&who==2&&use?"v11 ve vd ve ":"",!blockedf&&who==2&&use&&low?"pop0/2 ":"");expect(b);
            reset();conditions=blockedf?1u<<0xf:0;hp=low?20:21;frame(who);px_battle_event_set_value(0x2f,use,&events);
            px_battle_hp_berry_5e(HANDLER,CTX,2,WORK,&api);
            snprintf(b,sizeof b,"hs%x %s%s",0xf,!blockedf&&who==2?"v11 ve vd ve ":"",!blockedf&&who==2&&low?"pop0/2 ":"");expect(b);
            /* Event 0x61 ignores key 0x02 and key 0x2f. */
            reset();conditions=blockedf?1u<<0xf:0;hp=low?20:21;frame(who);w32(NULL,HANDLER+0x18,RUNNING);
            px_battle_hp_berry_61(HANDLER,CTX,2,WORK,&api);
            snprintf(b,sizeof b,"hs%x %s%s",0xf,!blockedf?"v11 ve vd ve ":"",!blockedf&&low?"pop0/2 ":"");expect(b);
            CHECK(r32(NULL,HANDLER+0x18)==(!blockedf&&low?RUNNING|REENTRY:(uint32_t)RUNNING));
        }
        /* Gluttony does not change a divisor of 2. */
        reset();ability=0x52;hp=21;frame(2);px_battle_hp_berry_61(HANDLER,CTX,2,WORK,&api);expect("hsf v11 ve vd ve ");
        reset();maxhp=1;hp=0;frame(2);px_battle_hp_berry_61(HANDLER,CTX,2,WORK,&api);expect("hsf v11 ve ");
    }
    /* Oran Berry and Berry Juice: the item's amount. */
    {
        const uint32_t amounts[]={10,20,0,0xffff,0x1000a,0xffffffff};
        for(unsigned i=0;i<6;++i)for(uint32_t who=2;who<4;++who){
            reset();param=amounts[i];frame(who);w16(NULL,HANDLER+0x38,(uint16_t)(0x2b+i));
            px_battle_oran_use(HANDLER,CTX,2,WORK,&api);
            if(who!=2){ expect("");continue; }
            char b[96];snprintf(b,sizeof b,"rec4/2 ip%x msg392/82 arg2 arg%x commit ",0x2b+i,0x2b+i);expect(b);
            CHECK(r16(NULL,RECORD+4)==(amounts[i]&0xffff)&&r8(NULL,RECORD+6)==2&&r8(NULL,RECORD+7)==0); }
        reset();frame(5);px_battle_oran_use(HANDLER,CTX,5,WORK,&api);expect("rec4/5 ip9e msg392/82 arg5 arg9e commit ");CHECK(r8(NULL,RECORD+6)==5);
    }
    /* Sitrus Berry: percent (low byte of the parameter) of max HP. */
    {
        const uint32_t maxima[]={0,1,3,40,99,100,101,255,714,999,0xffff,0x28f5c28,0x28f5c29,0x7fffffff,0xffffffff};
        const uint32_t percents[]={0,1,25,50,99,100,255,0x119,0x1ff,0xffffff00};
        for(unsigned i=0;i<15;++i)for(unsigned j=0;j<10;++j){
            reset();maxhp=maxima[i];param=percents[j];frame(2);px_battle_sitrus_use(HANDLER,CTX,2,WORK,&api);
            expect("ve ip9e rec4/2 msg392/82 arg2 arg9e commit ");
            const uint32_t product=maxima[i]*(percents[j]&0xff);
            CHECK(r16(NULL,RECORD+4)==((product/100)&0xffff)&&r8(NULL,RECORD+6)==2&&r8(NULL,RECORD+7)==0); }
        for(uint32_t x=0;x<200000;x+=7){ reset();maxhp=x;param=100;frame(2);px_battle_sitrus_use(HANDLER,CTX,2,WORK,&api);CHECK(r16(NULL,RECORD+4)==(x&0xffff)); }
        ++cases;
        reset();frame(3);px_battle_sitrus_use(HANDLER,CTX,2,WORK,&api);expect("");
    }
    /* Through the dispatcher: Lum Berry's check on event 0x9c keeps its handler running. */
    {
        for(unsigned on=0;on<2;++on){
            reset();status_value=on;
            w8(NULL,ENTRIES,0x9c);w32(NULL,ENTRIES+4,0);w8(NULL,ENTRIES+8,0x7b);w32(NULL,ENTRIES+12,1);
            w32(NULL,HANDLER+8,ENTRIES);w32(NULL,HANDLER+0x18,REGISTERED|2u<<16);w8(NULL,HANDLER+0x3a,2);
            w32(NULL,BATTLE+0x60,HANDLER);frame(2);px_battle_event_dispatch(CTX,0x9c,&events);
            expect(on?"st pop0/2 ":"st hs6 ");
            CHECK(r32(NULL,HANDLER+0x18)==(REGISTERED|2u<<16|(on?(uint32_t)RUNNING:0u))&&aborts==0);
            /* A running handler is not entered again; once idle, event 0x7b uses the berry. */
            log_[0]=0;px_battle_event_dispatch(CTX,0x7b,&events);
            expect(on?"":"st hs6 ");
            w32(NULL,HANDLER+0x18,REGISTERED|2u<<16);status_value=1;log_[0]=0;px_battle_event_dispatch(CTX,0x7b,&events);
            expect("st reca/2 commit ");CHECK(r8(NULL,RECORD+4)==0x27);
        }
        /* An HP berry and the frame's use flags. */
        reset();hp=1;w8(NULL,ENTRIES,0x9c);w32(NULL,ENTRIES+4,2);
        w32(NULL,HANDLER+8,ENTRIES);w32(NULL,HANDLER+0x18,REGISTERED|1u<<16);w8(NULL,HANDLER+0x3a,2);
        w32(NULL,BATTLE+0x60,HANDLER);frame(2);px_battle_event_set_value(0x2f,1,&events);px_battle_event_dispatch(CTX,0x9c,&events);
        expect("hsf v11 ve vd ve pop0/2 ");CHECK(r32(NULL,HANDLER+0x18)==(REGISTERED|1u<<16|(uint32_t)RUNNING));
    }
    printf("battle_items_1: %u cases, all tests passed\n",cases);
    return 0;
}
