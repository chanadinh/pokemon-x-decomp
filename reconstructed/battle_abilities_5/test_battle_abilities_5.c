#include "battle_abilities_5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, SCRATCH=0x7400, ME=2, OTHER=5 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static uint32_t kind_value,stat_count,stat_sign,flag_value,ally_value,reflect_block,redirect_block,fixed,ball,bounce_block,grass,flash;
static unsigned aborts,ncommit,k4_plain;static uint8_t commits[4][0x20];static char log_[1024];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static const struct px_battle_ability_api base={&events,NULL,NULL,NULL,NULL,NULL,NULL};
static uint32_t move_kind(void *u,uint32_t m) { (void)u;notef("kind%x ",m,0);return kind_value; }
static uint32_t stat_change_count(void *u,uint32_t m) { (void)u;notef("scc%x ",m,0);return stat_count; }
static void stat_change(void *u,uint32_t m,uint32_t i,uint32_t out) { (void)u;(void)m;CHECK(out==SCRATCH);notef("sc%u ",i,0);w32(NULL,out,stat_sign?0xffffffffu:1); }
static uint32_t move_flag(void *u,uint32_t m,uint32_t f) { (void)u;notef("mf%x/%u ",m,f);return flag_value; }
static uint32_t is_ally(void *u,uint32_t id,uint32_t o) { (void)u;notef("ally%u/%u ",id,o);return ally_value; }
static uint32_t reflection_blocked(void *u,uint32_t p) { (void)u;notef("rb%u ",mon(p),0);return reflect_block; }
static void reflect(void *u,uint32_t c,uint32_t id,uint32_t a) { (void)u;CHECK(c==CTX);notef("reflect%u/%u ",id,a); }
static void raise_stat(void *u,uint32_t c,uint32_t id,uint32_t s,uint32_t n) { (void)u;CHECK(c==CTX);char t[32];snprintf(t,sizeof t,"raise%u/%u/%u ",id,s,n);note(t); }
static void popup(void *u,uint32_t c,uint32_t p,uint32_t id) { (void)u;CHECK(c==CTX);notef("pop%u/%u ",p,id); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);
    memset(mem+RECORD,0xa5,0x20);w32(NULL,RECORD,0x11);return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;notef("msg%x/%x ",id,(r-RECORD)*16+k); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;(void)r;notef("arg%x ",v,0); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD&&ncommit<4);memcpy(commits[ncommit++],mem+RECORD,0x20);note("commit "); }
static const struct px_battle_immunity_api immunity={&base,move_kind,stat_change_count,stat_change,move_flag,NULL,NULL,is_ally,NULL,NULL,
    reflection_blocked,reflect,NULL,raise_stat,popup,action_record,message,message_arg,commit,SCRATCH};
static uint32_t redirect_blocked(void *u,uint32_t c,uint32_t a,uint32_t id,uint32_t m) { (void)u;CHECK(c==CTX&&id==ME);notef("rdb%u/%x ",a,m);return redirect_block; }
static uint32_t fixed_target(void *u,uint32_t m) { (void)u;notef("fix%x ",m,0);return fixed; }
static uint32_t ball_move(void *u,uint32_t m) { (void)u;notef("ball%x ",m,0);return ball; }
static uint32_t bounce_blocked(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("bb%u ",id,0);return bounce_block; }
static uint32_t has_type(void *u,uint32_t p,uint32_t t) { (void)u;notef("ty%u/%x ",mon(p),t);return t==0xb&&grass; }
static uint32_t battle_flag(void *u,uint32_t p,uint32_t f) { (void)u;notef("bf%u/%u ",mon(p),f);return f==0xd&&flash; }
static const struct px_battle_abilities_5_api api={&immunity,redirect_blocked,fixed_target,ball_move,bounce_blocked,has_type,battle_flag};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_abilities_5_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_lightning_rod_redirect,px_battle_redirect_message};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(commits,0,sizeof commits);aborts=ncommit=0;log_[0]=0;
    k4_plain=0;kind_value=stat_count=stat_sign=flag_value=ally_value=reflect_block=redirect_block=fixed=ball=bounce_block=grass=flash=0;
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
/* The frame: ids, values; key 0x36 a multiplier; the rewritable keys plain values. */
struct keys { uint32_t k2,k3,k4,k5,targets[4],move,type,k21,k23; };
static void frame(struct keys k)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x02,k.k2,&events);px_battle_event_set_id(0x03,k.k3,&events);if(k4_plain)px_battle_event_set_value(0x04,k.k4,&events);else px_battle_event_set_id(0x04,k.k4,&events);
    px_battle_event_set_value(0x05,k.k5,&events);for(unsigned i=0;i<4;++i)px_battle_event_set_id(0x06+i,k.targets[i],&events);
    px_battle_event_set_value(0x12,k.move,&events);px_battle_event_set_value(0x16,k.type,&events);
    px_battle_event_set_value(0x21,k.k21,&events);px_battle_event_set_value(0x23,k.k23,&events);
    px_battle_event_set_value(0x41,0,&events);px_battle_event_set_value(0x42,0,&events);px_battle_event_set_value(0x53,0,&events);
    px_battle_event_set_multiplier(0x36,0x1000,0,0x100000,&events);log_[0]=0;
}
static uint32_t get(uint32_t key) { return px_battle_event_get(key,&events); }
static uint32_t c16(unsigned n,unsigned o) { return (uint32_t)(commits[n][o]|commits[n][o+1]<<8); }
static uint32_t c32(unsigned n,unsigned o) { return c16(n,o)|c16(n,o+2)<<16; }
static void run(fn f,struct keys k) { frame(k);f(HANDLER,CTX,ME,WORK,&api);if(aborts)fprintf(stderr,"log %s\n",log_);
    CHECK(aborts==0); }
static void cat(char *w,size_t n,const char *f,uint32_t a,uint32_t b) { size_t l=strlen(w);snprintf(w+l,n-l,f,a,b); }
#define EXPECT(w) do { if(strcmp(log_,w)){fprintf(stderr,"got  %s\nwant %s\n",log_,w);CHECK(0);} } while (0)
int main(void)
{
    unsigned cases=0;char want[1024];
    const struct keys BASE={ME,OTHER,OTHER,0,{0,0,0,0},0x10021,0,0,0};
    /* Lightning Rod / Storm Drain: draw a move of their type; the work word says whether the target changed. */
    for(unsigned h=0;h<2;++h)for(unsigned bits=0;bits<256;++bits) {
        static const uint32_t types[4]={0xc,0xa,0x10c,0x10a};static const uint32_t moves[2]={0x10021,0x101fb};
        reset();struct keys k=BASE;k.k3=bits&1?(bits&0x80?0x100+ME:ME):(bits&0x80?0x100+OTHER:OTHER);k.type=types[bits>>1&3];k.move=moves[bits>>3&1];redirect_block=bits>>4&1;fixed=bits>>5&1;
        k.k4=bits>>6&1?ME:3;w32(NULL,WORK,0x77);k4_plain=1;
        if(bits>>7&1){k.k4=bits>>6&1?0x100+ME:0x103;}   /* high bits: ids are compared as bytes */
        run(h?px_battle_storm_drain_redirect:px_battle_lightning_rod_redirect,k);
        const uint32_t type=h?0xa:0xc,move=k.move&0xffff;uint32_t drawn=0;want[0]=0;
        if((k.k3&0xff)!=ME&&k.type==type){cat(want,sizeof want,"rdb%u/%x ",OTHER,move);
            if(!redirect_block){cat(want,sizeof want,"fix%x ",move,0);if(!fixed&&move!=0x1fb)drawn=(k.k4&0xff)!=ME;}}
        EXPECT(want);
        CHECK(r32(NULL,WORK)==drawn);
        const int touched=(k.k3&0xff)!=ME&&k.type==type&&!redirect_block&&!fixed&&move!=0x1fb;
        CHECK(get(0x04)==(touched?(uint32_t)ME:k.k4));
        ++cases;
    }
    /* The redirect message: once, if this battler is among the targets; the work word is cleared. */
    for(uint32_t n=0;n<5;++n)for(unsigned mask=0;mask<16;++mask)for(unsigned w=0;w<2;++w) {
        reset();struct keys k=BASE;k.k5=n;for(unsigned i=0;i<4;++i)k.targets[i]=mask>>i&1?ME:6;w32(NULL,WORK,w?3:0);
        run(px_battle_redirect_message,k);
        int hit=0;for(unsigned i=0;i<n&&i<4;++i)if(mask>>i&1)hit=1;
        if(n==0)hit=0;
        EXPECT(w&&hit?"rec3/2 msg1c0/42 arg2 commit ":"");
        CHECK(r32(NULL,WORK)==0);if(ncommit)CHECK(c32(0,0)==0x800011);++cases;
    }
    /* Sap Sipper through the shared absorb test. */
    for(unsigned bits=0;bits<64;++bits) {
        reset();struct keys k=BASE;k.k4=bits&1?ME:OTHER;k.k3=bits&2?ME:OTHER;k.type=bits&4?0xb:0x10b;kind_value=bits&8?2:1;stat_count=bits>>4&1;stat_sign=bits>>5&1;
        run(px_battle_sap_sipper,k);
        want[0]=0;int on=0;
        if(k.k4==ME&&k.k3!=ME&&k.type==0xb){strcat(want,"kind21 ");on=1;
            if(kind_value==2){strcat(want,"scc21 ");if(stat_count){strcat(want,"sc0 ");on=stat_sign;}}}
        if(on)strcat(want,"raise2/1/1 ");
        EXPECT(want);CHECK(get(0x41)==(uint32_t)on);++cases;
    }
    /* Flash Fire: leaving clears its state bit; Fire moves x1.5 while active. */
    for(unsigned bits=0;bits<16;++bits) {
        reset();struct keys k=BASE;k.k2=bits&1?ME:OTHER;k.k3=bits&2?ME:OTHER;flash=bits>>2&1;k.type=bits&8?9:0x109;
        run(px_battle_flash_fire_leave,k);
        want[0]=0;if(k.k2==ME){strcat(want,"bf2/13 ");if(flash)strcat(want,"rec18/2 commit ");}
        EXPECT(want);if(ncommit)CHECK(commits[0][5]==ME&&commits[0][4]==0xd&&c32(0,0)==0x11);
        reset();flash=bits>>2&1;run(px_battle_flash_fire_power,k);
        want[0]=0;if(k.k3==ME)strcat(want,"bf2/13 ");
        EXPECT(want);CHECK(get(0x36)==(k.k3==ME&&flash&&k.type==9?0x1800u:0x1000u));cases+=2;
    }
    /* Bulletproof: ball moves at it are blocked once. */
    for(unsigned bits=0;bits<8;++bits) {
        reset();struct keys k=BASE;k.k4=bits&1?ME:OTHER;ball=bits>>1&1;frame(k);
        if(bits&4)px_battle_event_rewrite(0x41,0,&events);   /* a value already rewritten cannot change again */
        log_[0]=0;px_battle_bulletproof(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0);
        want[0]=0;if(k.k4==ME){strcat(want,"ball21 ");if(ball&&!(bits&4))strcat(want,"pop1/2 rec3/2 msgd2/42 arg2 commit pop2/2 ");}
        EXPECT(want);++cases;
    }
    /* Damp: Explosion and Self-Destruct fail with reason 0x13, then the message names the user and the move. */
    for(unsigned bits=0;bits<32;++bits) {
        static const uint32_t moves[4]={0x10099,0x20078,0x10077,0x99};static const uint32_t reasons[4]={0,0x100,1,0x101};
        reset();struct keys k=BASE;k.move=moves[bits&3];k.k23=reasons[bits>>2&3];w32(NULL,WORK,9);w32(NULL,WORK+4,0x44);frame(k);
        if(bits&16)px_battle_event_rewrite(0x23,0,&events);
        log_[0]=0;px_battle_damp_check(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0&&log_[0]==0);
        const uint32_t move=k.move&0xffff,reason=bits&16?0:k.k23;const int fails=(move==0x99||move==0x78)&&(reason&0xff)==0;
        CHECK(r32(NULL,WORK)==(uint32_t)(fails&&!(bits&16))&&r32(NULL,WORK+4)==(fails?move:0x44));
        CHECK(get(0x23)==(fails&&!(bits&16)?0x13u:reason));
        const uint32_t was=r32(NULL,WORK);log_[0]=0;k.k2=0x106;frame(k);px_battle_damp_message(HANDLER,CTX,ME,WORK,&api);
        want[0]=0;if(was)cat(want,sizeof want,"rec3/2 msg38f/42 arg6 arg%x commit ",move,0);
        EXPECT(want);CHECK(r32(NULL,WORK)==0);if(ncommit)CHECK(c32(0,0)==0x800011);cases+=2;
    }
    /* Magic Bounce: reflect reflectable kind-11 moves from non-allies. */
    for(unsigned bits=0;bits<64;++bits) {
        reset();struct keys k=BASE;k.k2=0x105;bounce_block=bits&1;ally_value=bits>>1&1;kind_value=bits&4?0xb:0x10b;flag_value=bits>>3&1;frame(k);
        if(bits&16)px_battle_event_rewrite(0x23,0,&events);
        log_[0]=0;px_battle_magic_bounce_reflect(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0);
        want[0]=0;strcat(want,"bb2 ");
        if(!bounce_block){strcat(want,"ally5/2 ");if(!ally_value){strcat(want,"kind21 ");if(kind_value==0xb){strcat(want,"mf21/4 ");
            if(flag_value&&!(bits&16))strcat(want,"reflect2/5 ");}}}
        EXPECT(want);++cases;
    }
    for(unsigned bits=0;bits<8;++bits) {
        reset();struct keys k=BASE;k.k2=bits&1?ME:OTHER;reflect_block=bits>>1&1;frame(k);
        if(bits&4)px_battle_event_rewrite(0x53,0,&events);
        log_[0]=0;px_battle_magic_bounce_message(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0);
        want[0]=0;if(k.k2==ME&&!(bits&4)){strcat(want,"pop1/2 rb2 ");if(!reflect_block)strcat(want,"rec3/2 msg2fc/42 arg2 arg10021 commit ");strcat(want,"pop2/2 ");}
        EXPECT(want);CHECK(get(0x53)==(k.k2==ME&&!(bits&4)?1u:0u));++cases;
    }
    /* The veils and Insomnia: key 0x42 set. */
    for(unsigned bits=0;bits<16;++bits) {
        reset();struct keys k=BASE;k.k2=bits&1?ME:0x104;ally_value=bits>>1&1;grass=bits>>2&1;
        run(px_battle_sweet_veil,k);snprintf(want,sizeof want,"ally2/%u ",k.k2&0xff);EXPECT(want);CHECK(get(0x42)==ally_value);
        reset();ally_value=bits>>1&1;grass=bits>>2&1;run(px_battle_flower_veil,k);
        snprintf(want,sizeof want,"ally2/%u ",k.k2&0xff);if(ally_value)cat(want,sizeof want,"ty%u/b ",k.k2&0xff,0);
        EXPECT(want);CHECK(get(0x42)==(uint32_t)(ally_value&&grass));
        reset();run(px_battle_insomnia_block,k);CHECK(log_[0]==0&&get(0x42)==(k.k2==ME));cases+=3;
    }
    /* Liquid Ooze: the drain is cancelled and dealt to the attacker. */
    for(unsigned bits=0;bits<8;++bits) {
        static const uint32_t drains[4]={0,0x30,0x10000,0x1ffff};
        reset();struct keys k=BASE;k.k4=bits&1?ME:OTHER;k.k21=drains[bits>>1&3];k.k3=bits&4?0x106:3;
        run(px_battle_liquid_ooze,k);
        const uint32_t d=k.k21&0xffff;want[0]=0;
        if(k.k4==ME&&d)cat(want,sizeof want,"rec6/2 msg1c9/c2 arg%x commit ",k.k3&0xff,0);
        EXPECT(want);CHECK(get(0x21)==(k.k4==ME?0u:k.k21));
        if(ncommit)CHECK(c16(0,4)==d&&commits[0][6]==(k.k3&0xff)&&c32(0,0)==0x800011);
        ++cases;
    }
    /* Through the dispatcher: Lightning Rod on 0x2b, then its message on 0x24. */
    {
        reset();
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7c00+0x10*i;
            w8(NULL,entries,i?0x24:0x2b);w32(NULL,entries+4,i);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ME);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);struct keys k=BASE;k.type=0xc;k.k4=3;k4_plain=1;frame(k);px_battle_event_dispatch(CTX,0x2b,&events);
        CHECK(!strcmp(log_,"rdb5/21 fix21 ")&&aborts==0&&get(0x04)==ME&&r32(NULL,HANDLER+0x1c)==1);
        /* The message handler has its own work word; give it the flag as event 0x2b would through the shared handler. */
        w32(NULL,HANDLER+0x40+0x1c,1);k.k5=1;k.targets[0]=ME;frame(k);px_battle_event_dispatch(CTX,0x24,&events);
        CHECK(!strcmp(log_,"rec3/2 msg1c0/42 arg2 commit ")&&aborts==0&&r32(NULL,HANDLER+0x40+0x1c)==0);cases+=2;
    }
    printf("battle_abilities_5: %u cases, all tests passed\n",cases);
    return 0;
}
