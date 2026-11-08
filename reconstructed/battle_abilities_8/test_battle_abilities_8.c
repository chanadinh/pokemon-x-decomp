#include "battle_abilities_8.h"
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
/* Battler i: HP, max HP, turn flags, types, disabled. */
struct mon { uint32_t hp,max_hp,flags,typemask,disabled; };
static struct mon mons[8];
static uint32_t ally_value,steal_value,contact_value,locked_mask,berry_value,decided,disable_block,chance_value,forced_value;
static unsigned aborts,ncommit;static uint8_t commits[4][0x20];static char log_[1024];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static uint32_t value(void *u,uint32_t p,uint32_t k) { (void)u;notef("v%u/%x ",mon(p),k);CHECK(k==0xd||k==0xe);return k==0xd?mons[mon(p)].hp:mons[mon(p)].max_hp; }
static const struct px_battle_ability_api base={&events,value,NULL,NULL,NULL,NULL,NULL};
static uint32_t is_ally(void *u,uint32_t id,uint32_t o) { (void)u;notef("ally%u/%u ",id,o);return ally_value; }
static uint32_t steal_blocked(void *u,uint32_t c,uint32_t t,uint32_t v) { (void)u;CHECK(c==CTX);notef("sb%u/%u ",t,v);return steal_value; }
static uint32_t move_flag(void *u,uint32_t m,uint32_t f) { (void)u;notef("mf%x/%u ",m,f);return contact_value; }
static uint32_t turn_flag(void *u,uint32_t p,uint32_t f) { (void)u;notef("tf%u/%x ",mon(p),f);CHECK(f<32);return mons[mon(p)].flags>>f&1; }
static uint32_t item_locked(void *u,uint32_t c,uint32_t s,uint32_t i) { (void)u;CHECK(c==CTX);notef("lock%x/%x ",s,i);return locked_mask>>(s&0xf)&1; }
static uint32_t is_berry(void *u,uint32_t i) { (void)u;notef("berry%x ",i,0);return berry_value; }
static uint32_t battle_decided(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("dec ");return decided; }
static uint32_t has_type(void *u,uint32_t p,uint32_t t) { (void)u;notef("ty%u/%x ",mon(p),t);return t<32&&(mons[mon(p)].typemask>>t&1); }
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;notef("hs%u/%x ",mon(p),s);return s==0xd&&mons[mon(p)].disabled; }
static uint32_t disable_blocked(void *u,uint32_t m) { (void)u;notef("db%x ",m,0);return disable_block; }
static uint32_t disable_condition(void *u,uint32_t t,uint32_t m) { (void)u;notef("dc%u/%x ",t,m);return 0xd000+t*0x100+(m&0xff); }
static uint32_t chance(void *u,uint32_t p) { (void)u;notef("ch%u ",p,0);return chance_value; }
static uint32_t forced(void *u,uint32_t c,uint32_t f) { (void)u;CHECK(c==CTX);notef("fo%u ",f,0);return forced_value; }
static void popup(void *u,uint32_t c,uint32_t p,uint32_t id) { (void)u;CHECK(c==CTX);notef("pop%u/%u ",p,id); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);
    memset(mem+RECORD,0xa5,0x20);w32(NULL,RECORD,0x11);return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;notef("msg%x/%x ",id,(r-RECORD)*16+k); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;(void)r;notef("arg%x ",v,0); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD&&ncommit<4);memcpy(commits[ncommit++],mem+RECORD,0x20);note("commit "); }
static const struct px_battle_abilities_8_api api={&base,is_ally,steal_blocked,move_flag,turn_flag,item_locked,is_berry,battle_decided,has_type,
    has_status,disable_blocked,disable_condition,chance,forced,popup,action_record,message,message_arg,commit};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_abilities_8_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_sticky_hold_check,px_battle_sticky_hold_message};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(mons,0,sizeof mons);memset(commits,0,sizeof commits);aborts=ncommit=0;log_[0]=0;
    ally_value=steal_value=contact_value=locked_mask=berry_value=decided=disable_block=chance_value=forced_value=0;
    w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i){w32(NULL,TABLE+0x84+4*i,MON+0x100*i);w16(NULL,MON+0x100*i+0xc,(uint16_t)(0x180+i));}
}
static void hold(unsigned i,uint32_t item) { w16(NULL,MON+0x100*i+0x12,(uint16_t)item); }
struct keys { uint32_t k2,k3,k4,k5,targets[4],move,k14,k16,k2e,k47,k52; };
static void frame(struct keys k)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x02,k.k2,&events);px_battle_event_set_id(0x03,k.k3,&events);px_battle_event_set_id(0x04,k.k4,&events);
    px_battle_event_set_value(0x05,k.k5,&events);for(unsigned i=0;i<4;++i)px_battle_event_set_id(0x06+i,k.targets[i],&events);
    px_battle_event_set_value(0x12,k.move,&events);px_battle_event_set_value(0x14,k.k14,&events);px_battle_event_set_value(0x16,k.k16,&events);
    px_battle_event_set_value(0x2e,k.k2e,&events);px_battle_event_set_value(0x47,k.k47,&events);px_battle_event_set_value(0x52,k.k52,&events);
    px_battle_event_set_value(0x42,0,&events);log_[0]=0;
}
static uint32_t get(uint32_t key) { return px_battle_event_get(key,&events); }
static void run(fn f,struct keys k) { frame(k);f(HANDLER,CTX,ME,WORK,&api);if(aborts)fprintf(stderr,"log %s\n",log_);CHECK(aborts==0); }
static void cat(char *w,size_t n,const char *f,uint32_t a,uint32_t b) { size_t l=strlen(w);snprintf(w+l,n-l,f,a,b); }
static uint32_t c16(unsigned n,unsigned o) { return (uint32_t)(commits[n][o]|commits[n][o+1]<<8); }
static uint32_t c32(unsigned n,unsigned o) { return c16(n,o)|c16(n,o+2)<<16; }
#define EXPECT(w) do { if(strcmp(log_,w)){fprintf(stderr,"got  %s\nwant %s\n",log_,w);CHECK(0);} } while (0)
int main(void)
{
    unsigned cases=0;char want[1024];
    const struct keys BASE={OTHER,OTHER,OTHER,0,{0,0,0,0},0x10021,0,0,0,0,0};
    /* Sticky Hold: the work word holds key 0x42's rewrite, then the message. */
    for(unsigned bits=0;bits<16;++bits) {
        static const uint32_t items[2]={0,0x10000};
        struct keys k=BASE;k.k2=bits&1?ME:OTHER;k.k2e=items[bits>>1&1];
        reset();w32(NULL,WORK,0x99);frame(k);if(bits&4)px_battle_event_rewrite(0x42,0,&events);log_[0]=0;
        px_battle_sticky_hold_check(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0&&log_[0]==0);
        const int on=(bits&1)&&k.k2e==0;CHECK(r32(NULL,WORK)==(on?(bits&4?0u:1u):0x99u)&&get(0x42)==(uint32_t)(on&&!(bits&4)));
        reset();w32(NULL,WORK,bits>>3&1);run(px_battle_sticky_hold_message,k);
        const int say=(bits&1)&&(bits>>3&1);EXPECT(say?"rec3/2 msg1ed/42 arg2 commit ":"");CHECK(r32(NULL,WORK)==(say?0u:(bits>>3&1)));
        if(ncommit)CHECK(c32(0,0)==0x800011);
        cases+=2;
    }
    /* Pickpocket over the target list, steal block, contact and both hands. */
    for(uint32_t n=0;n<4;++n)for(unsigned mask=0;mask<8;++mask)for(unsigned bits=0;bits<16;++bits) {
        reset();struct keys k=BASE;k.k5=n;k.k3=0x105;for(unsigned i=0;i<4;++i)k.targets[i]=mask>>i&1?ME:6;
        steal_value=bits&1;contact_value=bits>>1&1;hold(ME,bits&4?0x130:0);hold(5,bits&8?0x31:0);run(px_battle_pickpocket,k);
        int found=0;for(unsigned i=0;i<n&&i<4;++i)if(mask>>i&1)found=1;
        want[0]=0;
        if(found){strcat(want,"sb2/5 ");if(!steal_value){strcat(want,"mf21/0 ");
            if(contact_value&&!(bits&4)&&(bits&8))strcat(want,"rec25/2 msg1cc/82 arg5 arg31 commit ");}}
        EXPECT(want);if(ncommit)CHECK(commits[0][4]==5&&c32(0,0)==0x800011);++cases;
    }
    /* Magician (event 0x8a): the work word notes a held item. */
    for(unsigned bits=0;bits<4;++bits) {
        reset();struct keys k=BASE;k.k3=bits&1?ME:OTHER;hold(ME,bits&2?0x100:0);w32(NULL,WORK,0x99);run(px_battle_magician_held,k);
        CHECK(log_[0]==0&&r32(NULL,WORK)==(bits&1?(bits&2?1u:0u):0x99u));++cases;
    }
    /* Symbiosis: pass its item to an ally that has none. */
    for(unsigned bits=0;bits<256;++bits) {
        reset();struct keys k=BASE;k.k2=0x105;ally_value=bits&1;hold(ME,bits&2?0x44:0);hold(5,bits&4?0x45:0);
        mons[ME].flags=(bits>>3&1)<<0xf|(bits>>4&1)<<0x11;locked_mask=(bits>>5&1)<<5|(bits>>6&1)<<2;run(px_battle_symbiosis,k);
        strcpy(want,"ally2/5 ");
        if(ally_value&&(bits&2)&&!(bits&4)){strcat(want,"tf2/f ");if(!(bits>>3&1)){strcat(want,"tf2/11 ");if(!(bits>>4&1)){strcat(want,"lock185/44 ");
            if(!(bits>>5&1)){strcat(want,"lock182/44 ");if(!(bits>>6&1))strcat(want,"rec25/2 msg4c3/82 arg2 arg44 arg5 commit ");}}}}
        EXPECT(want);if(ncommit)CHECK(commits[0][4]==5&&c32(0,0)==0x800011);++cases;
    }
    /* Cheek Pouch: eating a berry also heals a third; the message names the handler's ability. */
    for(unsigned bits=0;bits<16;++bits)for(uint32_t mx=0;mx<8;++mx) {
        reset();struct keys k=BASE;k.k2=bits&1?ME:OTHER;k.k52=bits&2?0x100:0;k.k2e=bits&4?0x10099:0x99;berry_value=bits>>3&1;mons[ME].max_hp=mx*5;
        w16(NULL,HANDLER+0x38,0xa7);run(px_battle_cheek_pouch,k);
        want[0]=0;if((bits&1)&&k.k52){strcat(want,"berry99 ");if(berry_value)strcat(want,"rec4/2 v2/e msg183/82 arg2 arga7 commit ");}
        EXPECT(want);if(ncommit)CHECK(commits[0][6]==ME&&c16(0,4)==(mx*5/3?mx*5/3:1)&&c32(0,0)==0x800011);++cases;
    }
    /* Moxie: Attack + the number of fainted targets. */
    for(unsigned bits=0;bits<4;++bits)for(uint32_t n=0;n<5;++n)for(unsigned mask=0;mask<16;++mask) {
        reset();decided=bits&1;struct keys k=BASE;k.k3=bits&2?ME:OTHER;k.k5=n;for(unsigned i=0;i<4;++i){k.targets[i]=0x104+i;mons[4+i].hp=mask>>i&1?0:9;}
        run(px_battle_moxie,k);
        strcpy(want,"dec ");uint32_t knocked=0;
        if(!decided&&k.k3==ME){for(unsigned i=0;i<n;++i){cat(want,sizeof want,"v%u/d ",4+i,0);if(mask>>i&1)++knocked;}if(knocked)strcat(want,"recd/2 commit ");}
        EXPECT(want);
        if(ncommit)CHECK(commits[0][0xc]==ME&&commits[0][0x16]==1&&commits[0][4]==1&&commits[0][0x17]==knocked&&c32(0,0)==0x800011&&commits[0][0x18]==0xa5);
        ++cases;
    }
    /* Color Change: a target takes the move's type. */
    for(uint32_t n=0;n<3;++n)for(unsigned bits=0;bits<16;++bits)for(unsigned t=0;t<5;++t) {
        static const uint32_t types[5]={0xa,0x12,0x10a,0x1f,0x32};
        reset();struct keys k=BASE;k.k5=n;k.targets[0]=6;k.targets[1]=ME;k.k16=types[t];decided=bits&1;mons[ME].hp=bits&2?0:1;
        mons[ME].typemask=bits&4&&(types[t]&0xff)<32?1u<<(types[t]&0xff):0;run(px_battle_color_change,k);
        want[0]=0;
        if(n>=2){strcat(want,"dec ");if(!decided){strcat(want,"v2/d ");if(mons[ME].hp&&(types[t]&0xff)!=0x12){cat(want,sizeof want,"ty2/%x ",types[t]&0xff,0);
            if(!(mons[ME].typemask))strcat(want,"pop1/2 rec13/2 commit pop2/2 ");}}}
        EXPECT(want);
        if(ncommit){const uint32_t ty=types[t]&0x1f;CHECK(commits[0][6]==ME&&c16(0,4)==(ty|ty<<5|0x4800)&&c32(0,0)==0x11);}
        ++cases;
    }
    /* Cursed Body: the attacker's last move is disabled, 30%. */
    for(unsigned bits=0;bits<1024;++bits) {
        static const uint32_t moves[4]={0x10021,0,0xa5,0x100a5};
        reset();struct keys k=BASE;k.k4=bits&1?ME:OTHER;k.k47=bits>>1&1;decided=bits>>2&1;k.k3=0x105;mons[5].disabled=bits>>3&1;
        k.k14=moves[bits>>4&3];disable_block=bits>>6&1;chance_value=bits>>7&1;forced_value=bits>>8&1;mons[5].flags=(bits>>9&1)<<1;
        run(px_battle_cursed_body,k);
        const uint32_t mv=k.k14&0xffff;want[0]=0;
        if(k.k4==ME&&!k.k47){strcat(want,"dec ");if(!decided){strcat(want,"hs5/d ");if(!mons[5].disabled&&mv&&mv!=0xa5){cat(want,sizeof want,"db%x ",mv,0);
            if(!disable_block){strcat(want,"ch30 ");if(!chance_value)strcat(want,"fo1 ");
                if(chance_value||forced_value){const uint32_t turns=bits>>9&1?5:4;cat(want,sizeof want,"tf5/1 rec%x/%u ",0xb,ME);cat(want,sizeof want,"dc%u/%x commit ",turns,mv);
                    }}}}}
        EXPECT(want);
        if(ncommit){const uint32_t turns=bits>>9&1?5:4;CHECK(c16(0,4)==0xd&&c32(0,8)==0xd000+turns*0x100+(mv&0xff)&&commits[0][0xf]==5&&c32(0,0)==0x800011);}
        ++cases;
    }
    /* Through the dispatcher: Sticky Hold on 0xa6, then its message on 0xa7. */
    {
        reset();
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7c00+0x10*i;
            w8(NULL,entries,i?0xa7:0xa6);w32(NULL,entries+4,i);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ME);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);struct keys k=BASE;k.k2=ME;frame(k);px_battle_event_dispatch(CTX,0xa6,&events);
        CHECK(log_[0]==0&&aborts==0&&get(0x42)==1&&r32(NULL,HANDLER+0x1c)==1);
        w32(NULL,HANDLER+0x40+0x1c,1);frame(k);px_battle_event_dispatch(CTX,0xa7,&events);
        EXPECT("rec3/2 msg1ed/42 arg2 commit ");CHECK(aborts==0&&r32(NULL,HANDLER+0x40+0x1c)==0);cases+=2;
    }
    printf("battle_abilities_8: %u cases, all tests passed\n",cases);
    return 0;
}
