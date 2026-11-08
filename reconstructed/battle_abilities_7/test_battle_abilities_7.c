#include "battle_abilities_7.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, LIST=0x7400, ME=2, OTHER=5 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
/* Battler i: effective ability, Steel, airborne, HP and max HP. */
struct mon { uint32_t ability,steel,airborne,hp,max_hp; };
static struct mon mons[8];
static uint32_t ally_value,category_value,kind_value,pressure_value,list[4],nlist,list_count;
static unsigned aborts,ncommit;static uint8_t commits[4][0x20];static char log_[1024];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static uint32_t value(void *u,uint32_t p,uint32_t k) { (void)u;notef("v%u/%x ",mon(p),k);const struct mon *m=&mons[mon(p)];
    switch(k){case 0x11:return m->ability;case 0xd:return m->hp;case 0xe:return m->max_hp;}CHECK(0);return 0; }
static uint32_t move_category(void *u,uint32_t m) { (void)u;notef("cat%x ",m,0);return category_value; }
static uint32_t side_battlers(void *u,uint32_t c,uint32_t s,uint32_t out) { (void)u;CHECK(c==CTX&&out==LIST);notef("list%x ",s,0);
    for(uint32_t i=0;i<nlist;++i){w8(NULL,out+i,(uint8_t)list[i]);}return list_count; }
static const struct px_battle_ability_api base={&events,value,move_category,NULL,NULL,side_battlers,NULL};
static uint32_t is_ally(void *u,uint32_t id,uint32_t o) { (void)u;notef("ally%u/%u ",id,o);return ally_value; }
static uint32_t opposing(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("opp%u ",id,0);return 0x30; }
static uint32_t has_type(void *u,uint32_t p,uint32_t t) { (void)u;notef("ty%u/%x ",mon(p),t);return t==8&&mons[mon(p)].steel; }
static uint32_t airborne(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX&&id<8);notef("air%u ",id,0);return mons[id].airborne; }
static uint32_t move_kind(void *u,uint32_t m) { (void)u;notef("kind%x ",m,0);return kind_value; }
static uint32_t pressure_move(void *u,uint32_t m) { (void)u;notef("pm%x ",m,0);return pressure_value; }
static void popup(void *u,uint32_t c,uint32_t p,uint32_t id) { (void)u;CHECK(c==CTX);notef("pop%u/%u ",p,id); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);
    memset(mem+RECORD,0xa5,0x20);w32(NULL,RECORD,0x11);return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;notef("msg%x/%x ",id,(r-RECORD)*16+k); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;(void)r;notef("arg%x ",v,0); }
static void message_extra(void *u,uint32_t r,uint32_t v) { (void)u;notef("extra%x/%x ",v,r-RECORD); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD&&ncommit<4);memcpy(commits[ncommit++],mem+RECORD,0x20);note("commit "); }
static const struct px_battle_abilities_7_api api={&base,is_ally,opposing,has_type,airborne,move_kind,pressure_move,popup,action_record,
    message,message_arg,message_extra,commit,LIST};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_abilities_7_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_levitate_check,px_battle_levitate_message};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(mons,0,sizeof mons);memset(commits,0,sizeof commits);aborts=ncommit=0;log_[0]=0;
    ally_value=category_value=kind_value=pressure_value=nlist=list_count=0;
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
struct keys { uint32_t k2,k3,k4,k5,targets[4],k16,k19,k21,k53; };
static void frame(struct keys k)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x02,k.k2,&events);px_battle_event_set_id(0x03,k.k3,&events);px_battle_event_set_id(0x04,k.k4,&events);
    px_battle_event_set_value(0x05,k.k5,&events);for(unsigned i=0;i<4;++i)px_battle_event_set_id(0x06+i,k.targets[i],&events);
    px_battle_event_set_value(0x12,0x10044,&events);px_battle_event_set_value(0x16,k.k16,&events);px_battle_event_set_value(0x19,k.k19,&events);
    px_battle_event_set_value(0x21,k.k21,&events);px_battle_event_set_value(0x53,k.k53,&events);
    px_battle_event_set_value(0x42,0,&events);px_battle_event_set_value(0x3b,0,&events);log_[0]=0;
}
static uint32_t get(uint32_t key) { return px_battle_event_get(key,&events); }
static void run(fn f,struct keys k) { frame(k);f(HANDLER,CTX,ME,WORK,&api);if(aborts)fprintf(stderr,"log %s\n",log_);CHECK(aborts==0); }
static void cat(char *w,size_t n,const char *f,uint32_t a,uint32_t b) { size_t l=strlen(w);snprintf(w+l,n-l,f,a,b); }
static uint32_t c32(unsigned n,unsigned o) { return (uint32_t)(commits[n][o]|commits[n][o+1]<<8|commits[n][o+2]<<16|(uint32_t)commits[n][o+3]<<24); }
#define EXPECT(w) do { if(strcmp(log_,w)){fprintf(stderr,"got  %s\nwant %s\n",log_,w);CHECK(0);} } while (0)
int main(void)
{
    unsigned cases=0;char want[1024];
    const struct keys BASE={0x104,OTHER,OTHER,0,{0,0,0,0},0,3,4,0};
    /* The trapping handlers over three listed opponents' abilities, types and footing. */
    for(unsigned h=0;h<3;++h)for(unsigned ally=0;ally<2;++ally)for(uint32_t count=0;count<4;++count)for(unsigned mask=0;mask<8;++mask) {
        static const fn f[3]={px_battle_shadow_tag,px_battle_arena_trap,px_battle_magnet_pull};
        reset();ally_value=ally;nlist=3;list[0]=4;list[1]=5;list[2]=6;list_count=count;
        for(unsigned i=0;i<3;++i){const int b=mask>>i&1;mons[4+i].ability=b?0x17:0x117;mons[4+i].steel=b;mons[4+i].airborne=!b;}
        run(f[h],BASE);
        strcpy(want,"ally4/2 ");uint32_t set=0;
        if(!ally){strcat(want,"opp2 list130 ");
            if(h==0){set=1;for(unsigned i=0;i<count;++i){cat(want,sizeof want,"v%u/11 ",4+i,0);if(mask>>i&1){set=0;break;}}}
            if(h==1){for(unsigned i=0;i<count;++i){cat(want,sizeof want,"air%u ",4+i,0);if(mask>>i&1){set=1;break;}}}
            if(h==2){for(unsigned i=0;i<count;++i){cat(want,sizeof want,"ty%u/8 ",4+i,0);if(mask>>i&1)set=1;}}}
        EXPECT(want);CHECK(get(0x42)==set);++cases;
    }
    /* Run Away: an ally's escape always works, and its message. */
    for(unsigned ally=0;ally<2;++ally)for(unsigned pre=0;pre<2;++pre) {
        reset();ally_value=ally;struct keys k=BASE;frame(k);if(pre)px_battle_event_rewrite(0x53,0,&events);log_[0]=0;
        px_battle_run_away(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0);EXPECT("ally4/2 ");CHECK(get(0x53)==(ally&&!pre?1u:0u));
        reset();ally_value=ally;frame(k);if(pre)px_battle_event_rewrite(0x53,0,&events);log_[0]=0;
        px_battle_run_away_message(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0);
        strcpy(want,"ally4/2 ");if(ally&&!pre)strcat(want,"rec3/2 msg4b/41 extrad0024/4 commit ");EXPECT(want);
        if(ncommit){CHECK(c32(0,0)==0x800011);}
        cases+=2;
    }
    /* Suction Cups, Levitate, Sturdy's one-hit KO block: battler, key already rewritten, work word. */
    for(unsigned bits=0;bits<16;++bits) {
        const int me=bits&1,pre=bits>>1&1;struct keys k=BASE;k.k4=me?ME:OTHER;k.k2=me?ME:OTHER;k.k53=bits>>2&1?0x10:0;
        reset();frame(k);if(pre)px_battle_event_rewrite(0x42,0,&events);log_[0]=0;px_battle_suction_cups(HANDLER,CTX,ME,WORK,&api);
        EXPECT(me&&!pre?"rec3/2 msg1c6/42 arg2 commit ":"");if(ncommit)CHECK(c32(0,0)==0x800011);
        reset();frame(k);if(pre)px_battle_event_rewrite(0x42,0,&events);log_[0]=0;px_battle_sturdy_ohko(HANDLER,CTX,ME,WORK,&api);
        EXPECT(me&&!pre?"pop1/2 rec3/2 msgd2/42 arg2 commit pop2/2 ":"");if(ncommit)CHECK(c32(0,0)==0x11);
        reset();w32(NULL,WORK,0x99);frame(k);if(pre)px_battle_event_rewrite(0x53,k.k53,&events);log_[0]=0;
        px_battle_levitate_check(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0&&log_[0]==0);
        const int lev=me&&k.k53==0;CHECK(r32(NULL,WORK)==(lev?(pre?0u:1u):0x99u));
        reset();w32(NULL,WORK,bits>>3&1);k.k2=me?ME:OTHER;run(px_battle_levitate_message,k);
        EXPECT(me&&(bits>>3&1)?"rec3/2 msgd2/42 arg2 commit ":"");CHECK(r32(NULL,WORK)==(me&&(bits>>3&1)?0u:(bits>>3&1)));
        reset();w32(NULL,WORK,bits>>3&1);run(px_battle_sturdy_message,k);
        EXPECT(me&&(bits>>3&1)?"pop1/2 rec3/2 msg202/42 arg2 commit pop2/2 ":"");CHECK(r32(NULL,WORK)==(me&&(bits>>3&1)?0u:(bits>>3&1)));
        if(ncommit)CHECK(c32(0,0)==0x11);
        cases+=5;
    }
    /* Sturdy at full HP survives (reason 4); the work word holds the rewrite result. */
    for(unsigned me=0;me<2;++me)for(uint32_t hp=0;hp<3;++hp)for(unsigned pre=0;pre<2;++pre) {
        reset();mons[ME].hp=hp;mons[ME].max_hp=2;w32(NULL,WORK,0x99);struct keys k=BASE;k.k4=me?ME:OTHER;frame(k);
        if(pre){px_battle_event_rewrite(0x3b,0,&events);}
        log_[0]=0;px_battle_sturdy_endure(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0);
        EXPECT(me?"v2/d v2/e ":"");
        CHECK(r32(NULL,WORK)==(me?(hp==2&&!pre?1u:0u):0x99u)&&get(0x3b)==(me&&hp==2&&!pre?4u:0u));++cases;
    }
    /* Pressure: one more PP when targeted, for field moves, or for listed moves. */
    for(unsigned ally=0;ally<2;++ally)for(uint32_t n=0;n<5;++n)for(unsigned mask=0;mask<16;++mask)for(unsigned kp=0;kp<4;++kp) {
        reset();ally_value=ally;kind_value=kp&1?0xa:0x10a;pressure_value=kp>>1;struct keys k=BASE;k.k3=0x105;k.k5=n;k.k21=mask*0x11+0xf0;
        for(unsigned i=0;i<4;++i)k.targets[i]=mask>>i&1?ME:7;
        run(px_battle_pressure_pp,k);
        strcpy(want,"ally5/2 ");uint32_t more=0;
        if(!ally){int found=0;for(unsigned i=0;i<n&&i<4;++i)if(mask>>i&1){found=1;break;}
            if(found)more=1;else{strcat(want,"kind44 ");if(kind_value==0xa)more=1;else{strcat(want,"pm44 ");more=pressure_value;}}}
        EXPECT(want);CHECK(get(0x21)==(more?(k.k21+1)&0xff:k.k21));++cases;
    }
    /* Prankster and Gale Wings: priority + 1 (the whole word). */
    for(unsigned me=0;me<2;++me)for(uint32_t c=0;c<3;++c)for(unsigned t=0;t<3;++t) {
        static const uint32_t types[3]={2,0x102,3};
        reset();category_value=c==2?0x100:c;struct keys k=BASE;k.k3=me?ME:OTHER;k.k16=types[t];k.k19=0x1ff;
        run(px_battle_prankster,k);EXPECT(me?"cat44 ":"");CHECK(get(0x19)==(me&&category_value==0?0x200u:0x1ffu));
        reset();run(px_battle_gale_wings,k);CHECK(log_[0]==0&&get(0x19)==(me&&k.k16==2?0x200u:0x1ffu));cases+=2;
    }
    /* Through the dispatcher: Levitate on event 0x12, then its message on 0x1b. */
    {
        reset();
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7c00+0x10*i;
            w8(NULL,entries,i?0x1b:0x12);w32(NULL,entries+4,i);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ME);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);struct keys k=BASE;k.k2=ME;frame(k);px_battle_event_dispatch(CTX,0x12,&events);
        CHECK(log_[0]==0&&aborts==0&&get(0x53)==1&&r32(NULL,HANDLER+0x1c)==1);
        w32(NULL,HANDLER+0x40+0x1c,1);frame(k);px_battle_event_dispatch(CTX,0x1b,&events);
        EXPECT("rec3/2 msgd2/42 arg2 commit ");CHECK(aborts==0&&r32(NULL,HANDLER+0x40+0x1c)==0);cases+=2;
    }
    printf("battle_abilities_7: %u cases, all tests passed\n",cases);
    return 0;
}
