#include "battle_immunity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, SCRATCH=0x7800 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static uint32_t kinds[0x300],stages[0x300][4],stage_count[0x300],flags[0x300],damaging[0x300],powder[0x300];
static uint32_t ally,statuses[8],mega,reflect_blocked,records;static unsigned aborts;static char log_[1024];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static const struct px_battle_ability_api base={&events,NULL,NULL,NULL,NULL,NULL,NULL};
static uint32_t move_kind(void *u,uint32_t m) { (void)u;CHECK(m<0x300);note("k ");return kinds[m]; }
static uint32_t stat_change_count(void *u,uint32_t m) { (void)u;note("n ");return stage_count[m]; }
static void stat_change(void *u,uint32_t m,uint32_t i,uint32_t out) { (void)u;CHECK(out==SCRATCH&&i<4);notef("s%u ",i,0);w32(NULL,out,stages[m][i]); }
static uint32_t move_flag(void *u,uint32_t m,uint32_t f) { (void)u;CHECK(m<0x300&&f<32);return flags[m]>>f&1; }
static uint32_t damaging_move(void *u,uint32_t m) { (void)u;CHECK(m<0x300);return damaging[m]; }
static uint32_t powder_move(void *u,uint32_t m) { (void)u;CHECK(m<0x300);return powder[m]; }
static uint32_t is_ally(void *u,uint32_t id,uint32_t o) { (void)u;CHECK(id<8&&o<8);return ally>>(id*8+o)&1; }
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;return statuses[mon(p)]==s; }
static uint32_t mega_gengar(void *u,uint32_t p) { (void)u;(void)p;return mega; }
static uint32_t reflection_blocked(void *u,uint32_t p) { (void)u;(void)p;return reflect_blocked; }
static void reflect(void *u,uint32_t c,uint32_t id,uint32_t a) { (void)u;CHECK(c==CTX);notef("reflect%u/%u ",id,a); }
static void heal(void *u,uint32_t c,uint32_t id,uint32_t d) { (void)u;CHECK(c==CTX);notef("heal%u/%u ",id,d); }
static void raise_stat(void *u,uint32_t c,uint32_t id,uint32_t s) { (void)u;CHECK(c==CTX);notef("raise%u/%u ",id,s); }
static void raise_stat5(void *u,uint32_t c,uint32_t id,uint32_t s,uint32_t n) { CHECK(n==1);raise_stat(u,c,id,s); }
static void popup(void *u,uint32_t c,uint32_t p,uint32_t id) { (void)u;CHECK(c==CTX);notef("pop%u/%u ",p,id); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);return RECORD+0x10*records++; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;CHECK(k==2);notef("msg%x@%x ",id,r); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;notef("arg%x@%x ",v,r); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX);notef("commit%x ",r,0); }
static const struct px_battle_immunity_api api={&base,move_kind,stat_change_count,stat_change,move_flag,damaging_move,powder_move,is_ally,
    has_status,mega_gengar,reflection_blocked,reflect,heal,raise_stat5,popup,action_record,message,message_arg,commit,SCRATCH};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static void (*const table[])(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_immunity_api*)={
        px_battle_water_absorb,px_battle_storm_drain};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(kinds,0,sizeof kinds);memset(stages,0,sizeof stages);memset(stage_count,0,sizeof stage_count);
    memset(flags,0,sizeof flags);memset(damaging,0,sizeof damaging);memset(powder,0,sizeof powder);memset(statuses,0,sizeof statuses);
    ally=mega=reflect_blocked=records=0;aborts=0;log_[0]=0;w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
static void frame(uint32_t attacker,uint32_t defender,uint32_t move,uint32_t type,uint32_t code)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x03,attacker,&events);px_battle_event_set_id(0x04,defender,&events);
    px_battle_event_set_id(0x12,move,&events);px_battle_event_set_id(0x16,type,&events);px_battle_event_set_value(0x39,code,&events);
    px_battle_event_set_value(0x41,0,&events);px_battle_event_set_value(0x4f,0,&events);px_battle_event_set_value(0x53,0,&events);log_[0]=0;
}
static uint32_t blocked_key(void) { return px_battle_event_get(0x41,&events); }
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_immunity_api*);
static int contains(const char *s) { return strstr(log_,s)!=NULL; }
int main(void)
{
    unsigned cases=0;
    /* The absorb test: defender, not the attacker, the type (full word), and the status-move rule. */
    for(uint32_t side=0;side<3;++side)for(uint32_t t=0;t<0x14;++t)for(uint32_t kind=0;kind<4;++kind)for(uint32_t pattern=0;pattern<7;++pattern) {
        reset();kinds[7]=kind;
        const int32_t sets[7][3]={{0,0,0},{1,0,0},{-1,0,0},{1,2,0},{1,-2,0},{2,1,-1},{0,1,0}};const uint32_t counts[7]={0,1,1,2,2,3,2};
        stage_count[7]=counts[pattern];for(unsigned i=0;i<3;++i)stages[7][i]=(uint32_t)sets[pattern][i];
        frame(side==1?2:3,side==2?5:2,7,t,7);
        const uint32_t got=px_battle_absorbs(CTX,2,10,&api);
        int negative=0;for(unsigned i=0;i<counts[pattern];++i)negative|=sets[pattern][i]<0;
        const int want=side==0&&t==10&&(kind!=2||counts[pattern]==0||negative);
        CHECK(got==(uint32_t)want&&blocked_key()==(uint32_t)want);++cases;
    }
    reset();kinds[7]=2;stage_count[7]=3;stages[7][0]=1;stages[7][1]=(uint32_t)-1;stages[7][2]=(uint32_t)-1;frame(3,2,7,10,7);
    px_battle_absorbs(CTX,2,10,&api);CHECK(!strcmp(log_,"k n s0 s1 "));++cases;   /* stops at the first lowering change */
    reset();frame(3,2,7,10,7);px_battle_event_rewrite(0x41,1,&events);CHECK(px_battle_absorbs(CTX,2,10,&api)==0);++cases;  /* already blocked */
    /* Healers and boosters: the type and the follow-up. */
    {
        const fn f[6]={px_battle_water_absorb,px_battle_volt_absorb,px_battle_dry_skin_absorb,px_battle_motor_drive,px_battle_lightning_rod,px_battle_storm_drain};
        const uint32_t types[6]={10,12,10,12,12,10};const char *after[6]={"heal2/4 ","heal2/4 ","heal2/4 ","raise2/5 ","raise2/3 ","raise2/3 "};
        for(unsigned k=0;k<6;++k)for(uint32_t t=8;t<14;++t) {
            reset();frame(3,2,7,t,7);f[k](HANDLER,CTX,2,WORK,&api);
            CHECK(blocked_key()==(t==types[k])&&contains(after[k])==(t==types[k]));++cases;
        }
    }
    /* Flash Fire: first time sets state bit 13 through a record; already active only reports. */
    for(uint32_t active=0;active<2;++active)for(uint32_t t=8;t<11;++t) {
        reset();if(active)mem[MON+0x200+0x169+1]=0x20;frame(3,2,7,t,7);px_battle_flash_fire(HANDLER,CTX,2,WORK,&api);
        if(t!=9) { CHECK(log_[0]=='k'||log_[0]==0);CHECK(!contains("pop"));++cases;continue; }
        if(active) CHECK(!strcmp(log_,"k pop1/2 rec3/2 msgd2@7004 arg2@7004 commit7000 pop2/2 "));
        else { CHECK(!strcmp(log_,"k pop1/2 rec3/2 msg1ab@7004 arg2@7004 commit7000 rec17/2 commit7010 pop2/2 "));CHECK(mem[0x7015]==2&&mem[0x7014]==0xd); }
        ++cases;
    }
    /* Wonder Guard: damaging moves other than Struggle that are not super effective. */
    for(uint32_t m=0;m<2;++m)for(uint32_t code=0;code<12;++code)for(uint32_t side=0;side<2;++side) {
        reset();damaging[100]=1;const uint32_t mv=m?165:100;damaging[165]=1;frame(side?2:3,2,mv,7,code|0x100);
        px_battle_wonder_guard(HANDLER,CTX,2,WORK,&api);
        const int on=!side&&!m&&code<=7;CHECK(blocked_key()==(uint32_t)on);
        if(on)CHECK(!strcmp(log_,"pop1/2 rec3/2 msgd2@7004 arg2@7004 commit7000 pop2/2 "));
        ++cases;
    }
    reset();frame(3,2,101,7,7);px_battle_wonder_guard(HANDLER,CTX,2,WORK,&api);CHECK(blocked_key()==0);++cases;   /* status move */
    /* Soundproof, Overcoat, Oblivious and Sticky Hold. */
    for(uint32_t m=0;m<0x200;++m) {
        reset();flags[m&0xff]=(m&7)==3?0x100:0;powder[m&0xff]=(m%5)==1;frame(3,2,m,7,7);
        px_battle_soundproof(HANDLER,CTX,2,WORK,&api);CHECK(blocked_key()==((m&7)==3&&m<0x100));
        reset();powder[m&0xff]=(m%5)==1;frame(3,2,m&0xff,7,7);px_battle_overcoat(HANDLER,CTX,2,WORK,&api);CHECK(blocked_key()==((m%5)==1));
        reset();frame(3,2,m,7,7);px_battle_oblivious(HANDLER,CTX,2,WORK,&api);CHECK(blocked_key()==(m==445||m==269));
        reset();frame(3,2,m,7,7);px_battle_sticky_hold(HANDLER,CTX,2,WORK,&api);CHECK(blocked_key()==(m==271||m==415));
        if(m==271)CHECK(r32(NULL,RECORD)==0x800000&&!strcmp(log_,"rec3/2 msgd2@7004 arg2@7004 commit7000 "));
        ++cases;
    }
    /* Telepathy: an ally's damaging move (not its own). */
    for(uint32_t a=0;a<4;++a)for(uint32_t al=0;al<2;++al)for(uint32_t dmg=0;dmg<2;++dmg) {
        reset();if(al)ally=1u<<(2*8+a);damaging[7]=dmg;frame(a|0x100,2,7,7,7);px_battle_telepathy(HANDLER,CTX,2,WORK,&api);
        const int on=al&&a!=2&&dmg;CHECK(blocked_key()==(uint32_t)on);if(on)CHECK(contains("msg1d5@7004")&&r32(NULL,RECORD)==0x800000);++cases;
    }
    /* Safety Goggles: powder moves, the item and move in the message. */
    for(uint32_t p=0;p<2;++p) {
        reset();powder[9]=p;w16(NULL,HANDLER+0x38,650);frame(3,2,0x10009,7,7);px_battle_safety_goggles(HANDLER,CTX,2,WORK,&api);
        CHECK(blocked_key()==p);if(p)CHECK(!strcmp(log_,"rec3/2 msg4ae@7004 arg2@7004 arg28a@7004 arg10009@7004 commit7000 "));++cases;
    }
    /* Magic Coat and Magic Bounce: reflectable moves, not already reflected, not blocked for the battler. */
    {
        const fn f[2]={px_battle_magic_coat,px_battle_magic_bounce};
        for(unsigned k=0;k<2;++k)for(uint32_t refl=0;refl<2;++refl)for(uint32_t prior=0;prior<2;++prior)for(uint32_t blockd=0;blockd<2;++blockd) {
            reset();flags[7]=refl?0x10:0;reflect_blocked=blockd;frame(0x305,2,7,7,7);if(prior)px_battle_event_rewrite(0x4f,1,&events);
            f[k](HANDLER,CTX,2,WORK,&api);
            const int on=refl&&!prior&&!blockd;
            CHECK(blocked_key()==(uint32_t)on&&px_battle_event_get(0x53,&events)==(uint32_t)on&&contains("reflect2/5 ")==on);++cases;
        }
    }
    /* Telekinesis: Diglett, Dugtrio, Mega Gengar and two statuses. */
    for(uint32_t sp=48;sp<100;++sp)for(uint32_t mg=0;mg<2;++mg)for(uint32_t st=0x14;st<0x21;st+=1) {
        reset();w16(NULL,MON+0x400+0xc,(uint16_t)sp);mega=mg;statuses[4]=st;frame(2,0x204,7,7,7);px_battle_telekinesis(HANDLER,CTX,2,WORK,&api);
        const int on=sp==50||sp==51||(sp==94&&mg)||st==0x1f||st==0x15;CHECK(blocked_key()==(uint32_t)on);++cases;
    }
    reset();w16(NULL,MON+0x400+0xc,50);frame(3,4,7,7,7);px_battle_telekinesis(HANDLER,CTX,2,WORK,&api);CHECK(blocked_key()==0);++cases;
    /* Through the dispatcher: Water Absorb and Storm Drain both see a Water move; only the first blocks. */
    {
        reset();
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7c00+0x10*i;
            w8(NULL,entries,0x31);w32(NULL,entries+4,i);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,2);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);frame(3,2,7,10,7);px_battle_event_dispatch(CTX,0x31,&events);
        CHECK(blocked_key()==1&&contains("heal2/4 ")&&!contains("raise")&&aborts==0);++cases;
    }
    printf("battle_immunity: %u cases, all tests passed\n",cases);
    return 0;
}
