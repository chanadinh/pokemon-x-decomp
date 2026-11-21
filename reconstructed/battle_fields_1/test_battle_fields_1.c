#include "battle_fields_1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, OUTER=0x3200, INNER=0x3300, TABLE=0x3400,
       RECORD=0x7000, BUFFER=0x7800, LAYERS=0x8000, WORK=0x9f00, HANDLER=0x9e00, MON=0xa000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
/* Answers: airborne, slot present, decided, has Poison/Fire, effectiveness, damaging, move flag, target class. */
static uint32_t airborne,present,decided,poison,fire,effectiveness,damaging,flag,target_class,maxhp[32];static unsigned aborts,records;
static char log_[1024];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[64];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static uint32_t who(uint32_t p) { CHECK(p>=MON&&p<MON+0x800&&!((p-MON)%0x20));return (p-MON)/0x20; }
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;CHECK(k==0xe);notef("ve/%u ",who(p),0);return maxhp[who(p)]; }
static const struct px_battle_ability_api base={.events=&events,.pokemon_value=pokemon_value};
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id)
{ (void)u;CHECK(c==CTX&&records<3);notef("rec%x/%x ",k,id);return RECORD+0x100*records++; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id)
{ (void)u;char t[64];snprintf(t,sizeof t,"msg%x@%x/%x ",id,r>=RECORD&&r<RECORD+0x300?(r-RECORD)&0xff:r,k);note(t); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;notef("arg%x@%x ",v,r>=RECORD&&r<RECORD+0x300?(r-RECORD)&0xff:r); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r>=RECORD);notef("commit%u ",(r-RECORD)>>8,0); }
static const struct px_battle_items_api items={&base,NULL,NULL,NULL,action_record,message,message_arg,commit,0,0};
static const struct px_battle_abilities_1_api abilities={.base=&base};
static uint32_t is_airborne(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("ab%u ",id,0);return airborne; }
static uint32_t slot_present(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("sp%u ",id,0);return present; }
static uint32_t battle_decided(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("dec ");return decided; }
static uint32_t side_of_position(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("sop%u ",id,0);return 0x30|id; }
static uint32_t has_type(void *u,uint32_t p,uint32_t t) { (void)u;notef("ht%u/%u ",t,who(p));return t==3?poison:fire; }
static uint32_t packed_types(void *u,uint32_t p) { (void)u;notef("pt/%u ",who(p),0);return 0x1234; }
static uint32_t eff(void *u,uint32_t t,uint32_t packed) { (void)u;CHECK(t==5&&packed==0x1234);note("ef ");return effectiveness; }
static uint32_t move_value(void *u,uint32_t m,uint32_t f) { (void)u;CHECK(f==0x1b);notef("mv%x ",m,0);return target_class; }
static uint32_t damaging_move(void *u,uint32_t m) { (void)u;notef("dm%x ",m,0);return damaging; }
static uint32_t move_flag(void *u,uint32_t m,uint32_t f) { (void)u;CHECK(f==3);notef("mf%x ",m,0);return flag; }
static const struct px_battle_fields_1_api api={&items,&abilities,LAYERS,is_airborne,slot_present,battle_decided,side_of_position,
    has_type,packed_types,eff,move_value,damaging_move,move_flag};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_fields_1_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_side_tailwind,px_battle_side_safeguard};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);airborne=0;present=1;decided=0;poison=0;fire=0;effectiveness=7;damaging=1;flag=1;target_class=5;aborts=0;records=0;log_[0]=0;
    for(unsigned i=0;i<32;++i)maxhp[i]=160;
    w32(NULL,CTX+4,OUTER);w32(NULL,OUTER+0x10,INNER);w8(NULL,INNER+1,0);
    w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<0x18;++i)w32(NULL,TABLE+0x84+4*i,MON+0x20*i);
    w16(NULL,HANDLER+0x38,0);
}
/* The keys a side handler reads; keys 0x41, 0x42, 0x46 and 0x53 are rewritable flags. */
struct keys { uint32_t k2,k3,k4,k12,k19,k1a,k1b,k1c,k1e,k21,k27,k36,k39,k40; };
static struct keys K;
static void frame(void)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x02,K.k2,&events);px_battle_event_set_id(0x03,K.k3,&events);px_battle_event_set_id(0x04,K.k4,&events);
    px_battle_event_set_value(0x12,K.k12,&events);px_battle_event_set_value(0x19,K.k19,&events);px_battle_event_set_value(0x1a,K.k1a,&events);
    px_battle_event_set_value(0x1b,K.k1b,&events);px_battle_event_set_value(0x1c,K.k1c,&events);px_battle_event_set_value(0x1e,K.k1e,&events);
    px_battle_event_set_value(0x21,K.k21,&events);px_battle_event_set_value(0x27,K.k27,&events);px_battle_event_set_value(0x39,K.k39,&events);
    px_battle_event_set_value(0x40,K.k40,&events);px_battle_event_set_multiplier(0x36,K.k36,0,0,&events);
    px_battle_event_set_value(0x41,0,&events);px_battle_event_set_value(0x42,0,&events);px_battle_event_set_value(0x46,0,&events);
    px_battle_event_set_value(0x53,0,&events);log_[0]=0;
}
static void lock(void)
{
    CHECK(px_battle_event_rewrite(0x41,0,&events)==1&&px_battle_event_rewrite(0x42,0,&events)==1&&px_battle_event_rewrite(0x46,0,&events)==1&&
          px_battle_event_rewrite(0x53,0,&events)==1);log_[0]=0;
}
static void defaults(void)
{
    memset(&K,0,sizeof K);K.k2=2;K.k3=13;K.k4=2;K.k12=0x21;K.k19=1;K.k1a=0;K.k1b=1;K.k1c=0;K.k1e=1;K.k21=0xffffffffu;K.k27=0x15;K.k36=0x1000;K.k39=7;K.k40=BUFFER;
}
static uint32_t key(uint32_t k) { return px_battle_event_get(k,&events); }
static unsigned cases;
static void expect(const char *s) { if(strcmp(log_,s)){fprintf(stderr,"got  [%s]\nwant [%s]\n",log_,s);} CHECK(!strcmp(log_,s));++cases; }
static uint32_t times(uint32_t v,uint32_t f) { return (uint32_t)((uint64_t)((int64_t)(int32_t)v*(int32_t)f+0x800)>>12); }
static void only(uint32_t at,const uint32_t *o,unsigned n)
{
    for(uint32_t k=0;k<0x100;++k){ int listed=0;for(unsigned i=0;i<n;++i)listed|=o[i]==k;if(!listed)CHECK(r8(NULL,at+k)==0); }
}
int main(void)
{
    /* The four helpers over every input that matters. */
    {
        for(uint32_t s=0;s<0x40;++s){ CHECK(px_battle_slot_side(s)==(s>=12));++cases; }
        for(uint32_t s=0;s<0x40;++s){ CHECK(px_battle_major_status(s)==(s<6));++cases; }
        CHECK(px_battle_slot_side(0xffffffff)==1&&px_battle_major_status(0xffffffff)==0);
        for(uint32_t m=0;m<256;++m){ reset();w8(NULL,INNER+1,(uint8_t)m);w8(NULL,INNER,0x77);w8(NULL,INNER+2,0x88);CHECK(px_battle_mode(CTX,&memory)==m);++cases; }
        for(uint32_t v=0;v<0x10000;v+=0x101){ reset();w16(NULL,BUFFER+2,(uint16_t)v);w16(NULL,BUFFER,0x1234);w16(NULL,BUFFER+4,0x5678);
            px_battle_message_flag(BUFFER,&memory);CHECK(r16(NULL,BUFFER+2)==(v|0x8000)&&r16(NULL,BUFFER)==0x1234&&r16(NULL,BUFFER+4)==0x5678);++cases; }
    }
    /* Reflect and Light Screen: side, category, critical hit, every battle mode. */
    {
        for(unsigned ls=0;ls<2;++ls)for(uint32_t k4=1;k4<26;++k4)for(uint32_t cat=0;cat<4;++cat)for(uint32_t crit=0;crit<2;++crit)for(uint32_t m=0;m<5;++m){
            reset();defaults();K.k4=k4;K.k1b=cat;K.k36=0x1555;w8(NULL,INNER+1,(uint8_t)m);frame();
            if(crit)CHECK(px_battle_event_rewrite(0x46,1,&events)==1);
            if(ls)px_battle_side_light_screen(HANDLER,CTX,0,WORK,&api); else px_battle_side_reflect(HANDLER,CTX,0,WORK,&api);
            const int hit=k4<12&&cat==(ls?2u:1u)&&!crit;
            CHECK(key(0x36)==(hit?times(0x1555,(m==0||m==3)?0x800:0xaac):0x1555)&&aborts==0);++cases;
        }
        reset();defaults();K.k4=0x10c;w8(NULL,INNER+1,1);frame();px_battle_side_reflect(HANDLER,CTX,1,WORK,&api);CHECK(key(0x36)==times(0x1000,0xaac));++cases;
    }
    /* Safeguard and Mist: the work word and the show handlers. */
    {
        for(uint32_t k4=1;k4<26;k4+=3)for(uint32_t k3=0;k3<3;++k3)for(uint32_t st=0;st<0x12;++st)for(unsigned locked=0;locked<2;++locked){
            reset();defaults();K.k4=k4;K.k3=k3==0?k4:k3==1?k4+0x100:20;K.k1e=st;if(k3==2)K.k3=20;frame();if(locked)lock();w32(NULL,WORK,0x55);
            px_battle_side_safeguard(HANDLER,CTX,0,WORK,&api);
            const int pass=k4<12&&K.k3!=k4&&(st<6||st==6||st==0xe);
            CHECK(r32(NULL,WORK)==(pass?(locked?0u:1u):0x55u)&&key(0x42)==(pass&&!locked)&&aborts==0);++cases;
        }
        for(uint32_t k4=0;k4<26;k4+=5)for(uint32_t w=0;w<3;++w){
            reset();defaults();K.k4=k4;frame();w32(NULL,WORK,w==2?0x80000000:w);px_battle_side_safeguard_show(HANDLER,CTX,0,WORK,&api);
            const uint32_t t=k4&0xff;char b[96];snprintf(b,sizeof b,"rec3/%x msg34a@4/2 arg%x@4 commit0 ",t,t);
            expect(w?b:"");CHECK(r32(NULL,WORK)==(w?0u:0u)||w==0);
        }
        const uint32_t drops[]={0,1,0x7fffffff,0x80000000,0xffffffff};
        for(uint32_t k2=1;k2<26;k2+=4)for(unsigned d=0;d<5;++d)for(uint32_t k3=0;k3<3;++k3)for(unsigned locked=0;locked<2;++locked){
            reset();defaults();K.k2=k2;K.k21=drops[d];K.k3=k3==0?k2:k3==1?k2+0x100:20;frame();if(locked)lock();w32(NULL,WORK,0x55);
            px_battle_side_mist(HANDLER,CTX,0,WORK,&api);
            const int pass=k2<12&&(int32_t)drops[d]<0&&K.k3!=k2;
            CHECK(r32(NULL,WORK)==(pass?(locked?0u:1u):0x55u));++cases;
        }
        const uint32_t moves[]={0,0x21,0x22,0x10021};
        for(unsigned m=0;m<4;++m)for(unsigned remembered=0;remembered<4;++remembered)for(uint32_t w=0;w<2;++w){
            reset();defaults();K.k2=0x105;K.k1a=moves[m];frame();w32(NULL,WORK,w);w32(NULL,WORK+4,moves[remembered]);
            px_battle_side_mist_show(HANDLER,CTX,0,WORK,&api);
            const int shown=w&&(moves[m]==0||moves[remembered]!=moves[m]);
            expect(shown?"rec3/5 msg34d@4/2 arg5@4 commit0 ":"");
            CHECK(r32(NULL,WORK)==0u||!w);CHECK(r32(NULL,WORK+4)==(shown?moves[m]:moves[remembered]));
            if(shown)CHECK(r16(NULL,RECORD+6)==0x8000);
        }
    }
    /* Tailwind, Swamp, Lucky Chant and the Rainbow handlers. */
    {
        for(uint32_t k=0;k<26;++k)for(uint32_t sd=0;sd<2;++sd){
            reset();defaults();K.k2=k;K.k3=k;K.k4=k;K.k27=0x9999;frame();
            px_battle_side_tailwind(HANDLER,CTX,sd,WORK,&api);CHECK(key(0x36)==(px_battle_slot_side(k)==sd?0x2000u:0x1000u));
            reset();defaults();K.k2=k+0x100;frame();px_battle_side_swamp(HANDLER,CTX,sd,WORK,&api);CHECK(key(0x36)==(px_battle_slot_side(k)==sd?0x400u:0x1000u));
            reset();defaults();K.k4=k;frame();px_battle_side_lucky_chant(HANDLER,CTX,sd,WORK,&api);CHECK(key(0x42)==(px_battle_slot_side(k)==sd));
            reset();defaults();K.k3=k;K.k27=0x9999;frame();px_battle_side_rainbow_75(HANDLER,CTX,sd,WORK,&api);CHECK(key(0x46)==(px_battle_slot_side(k)==sd));
            px_battle_side_rainbow_chance(HANDLER,CTX,sd,WORK,&api);
            CHECK(key(0x27)==(px_battle_slot_side(k)==sd?(0x9999u<<1)&0xffff:0x9999u));cases+=5;
        }
        const uint32_t chances[]={0,1,0x7fff,0x8000,0xffff,0x12345};
        for(unsigned c=0;c<6;++c){ reset();defaults();K.k3=2;K.k27=chances[c];frame();px_battle_side_rainbow_chance(HANDLER,CTX,0,WORK,&api);
            CHECK(key(0x27)==((chances[c]<<1)&0xffff));++cases; }
    }
    /* Spikes: the layers table, the side, airborne and a row that faults. */
    {
        for(uint32_t sd=0;sd<2;++sd)for(uint32_t layers=0;layers<5;++layers)for(uint32_t air=0;air<2;++air)for(uint32_t mx=0;mx<400;mx+=37)for(uint32_t hk=0;hk<3;++hk){
            const uint32_t row=hk==0?6:hk==1?0x11:0x106,b=sd?13:2;
            reset();defaults();airborne=air;maxhp[b]=mx;K.k2=b;frame();w16(NULL,HANDLER+0x38,(uint16_t)row);
            w8(NULL,LAYERS+(sd*17+6)*16+0xc,(uint8_t)layers);w8(NULL,LAYERS+(sd*17+6)*16+0xd,0x55);
            px_battle_side_spikes(HANDLER,CTX,sd,WORK,&api);
            char want[160];const int bad=(row&0xff)>=17;const uint32_t lay=bad?0:layers;
            const uint32_t div=lay==2?6:lay==3?4:8,hp=mx/div?mx/div:1;
            snprintf(want,sizeof want,"ab%u %srec6/1f ve/%u msg356@c/2 arg%x@c commit0 ",b,bad?"ABORT ":"",b,b);
            if(air){ snprintf(want,sizeof want,"ab%u ",b);expect(want);CHECK(records==0);continue; }
            expect(want);CHECK(r16(NULL,RECORD+4)==hp&&r8(NULL,RECORD+6)==b);{ const uint32_t o[]={4,5,6};only(RECORD,o,3); }
        }
        for(uint32_t k=0;k<26;++k){ reset();defaults();K.k2=k;frame();w16(NULL,HANDLER+0x38,6);px_battle_side_spikes(HANDLER,CTX,px_battle_slot_side(k)^1,WORK,&api);expect(""); }
        reset();defaults();frame();w16(NULL,HANDLER+0x38,6);px_battle_side_spikes(HANDLER,CTX,2,WORK,&api);expect("");
    }
    /* Toxic Spikes: Poison types absorb them; otherwise poison, badly from the second layer. */
    {
        for(uint32_t sd=0;sd<2;++sd)for(uint32_t layers=0;layers<4;++layers)for(uint32_t pt=0;pt<2;++pt)for(uint32_t air=0;air<2;++air)for(uint32_t hk=0;hk<2;++hk){
            const uint32_t b=sd?13:2,row=hk?0x11:7;
            reset();defaults();airborne=air;poison=pt;K.k2=b;frame();w16(NULL,HANDLER+0x38,(uint16_t)row);memset(mem+RECORD,0xaa,0x100);
            w8(NULL,LAYERS+(sd*17+7)*16+0xc,(uint8_t)layers);
            px_battle_side_toxic_spikes(HANDLER,CTX,sd,WORK,&api);
            char want[160];
            if(air){ snprintf(want,sizeof want,"ab%u ",b);expect(want);continue; }
            if(pt){ snprintf(want,sizeof want,"ab%u ht3/%u rec1a/%x commit0 ",b,b,b);expect(want);
                CHECK(r8(NULL,RECORD+4)==4&&r8(NULL,RECORD+5)==0x80&&r8(NULL,RECORD+6)==0&&r8(NULL,RECORD+7)==0&&r8(NULL,RECORD+8)==sd);
                CHECK(r8(NULL,RECORD+3)==0xaa&&r8(NULL,RECORD+9)==0xaa); }
            else { snprintf(want,sizeof want,"ab%u ht3/%u recb/%x %scommit0 ",b,b,b,hk?"ABORT ":"");expect(want);
                CHECK(r16(NULL,RECORD+4)==5&&r32(NULL,RECORD+8)==(!hk&&layers>1?0x79u:1u)&&r8(NULL,RECORD+0xf)==b); }
        }
    }
    /* Stealth Rock: every effectiveness code 0..20 against the divisor table. */
    {
        const uint32_t want_div[14]={8,0x100,0x100,0x80,0x40,0x20,0x10,8,4,2,1,1,1,1};
        for(uint32_t code=0;code<21;++code)for(uint32_t sd=0;sd<2;++sd)for(uint32_t mx=0;mx<600;mx+=41){
            const uint32_t b=sd?13:2;
            reset();defaults();effectiveness=code;maxhp[b]=mx;K.k2=b;frame();px_battle_side_stealth_rock(HANDLER,CTX,sd,WORK,&api);
            const uint32_t d=code<14?want_div[code]:8,hp=mx/d?mx/d:1;char want[160];
            snprintf(want,sizeof want,"pt/%u ef rec6/1f ve/%u msg359@c/2 arg%x@c commit0 ",b,b,b);
            expect(want);CHECK(r16(NULL,RECORD+4)==hp&&r8(NULL,RECORD+6)==b);
            reset();defaults();K.k2=b;frame();px_battle_side_stealth_rock(HANDLER,CTX,sd^1,WORK,&api);expect("");
        }
        reset();defaults();effectiveness=0xffffffff;frame();px_battle_side_stealth_rock(HANDLER,CTX,0,WORK,&api);CHECK(r16(NULL,RECORD+4)==20);++cases;
    }
    /* The guards: every early return, and a rewrite that does not take. */
    {
        for(uint32_t k4=1;k4<26;k4+=4)for(uint32_t k3=0;k3<2;++k3)for(uint32_t tc=0;tc<8;++tc)for(uint32_t dm=0;dm<2;++dm)for(unsigned locked=0;locked<2;++locked){
            reset();defaults();K.k4=k4;K.k3=k3?k4:20;K.k12=0x10021;target_class=tc|0x100;damaging=dm;frame();if(locked)lock();
            px_battle_side_wide_guard(HANDLER,CTX,0,WORK,&api);
            char want[128]="";const uint32_t d=k4&0xff;
            if(k4<12){ strcat(want,"mv21 dm21 ");
                if(dm&&(tc==4||tc==5)&&!locked){ char c[64];snprintf(c,sizeof c,"msg31d@7800/2 arg%x@7800 ",d);strcat(want,c); } }
            expect(want);CHECK(key(0x41)==(k4<12&&dm&&(tc==4||tc==5)&&!locked));
        }
        for(uint32_t k4=1;k4<26;k4+=4)for(uint32_t k3=0;k3<2;++k3)for(uint32_t pr=0;pr<4;++pr)for(uint32_t fl=0;fl<2;++fl)for(unsigned locked=0;locked<2;++locked){
            const uint32_t prios[]={0,1,0x7fffffff,0xfffffffe};
            reset();defaults();K.k4=k4;K.k3=k3?k4:20;K.k19=prios[pr];flag=fl;frame();if(locked)lock();
            px_battle_side_quick_guard(HANDLER,CTX,0,WORK,&api);
            char want[128]="";const int hit=k4<12&&!k3&&(int32_t)prios[pr]>=1&&fl&&!locked;
            if(k4<12&&!k3&&(int32_t)prios[pr]>=1){ strcat(want,"mf21 ");if(fl&&!locked){ char c[64];snprintf(c,sizeof c,"msg320@7800/2 arg%x@7800 ",k4&0xff);strcat(want,c); } }
            expect(want);CHECK(key(0x41)==(uint32_t)hit);
        }
        for(uint32_t k4=1;k4<26;k4+=4)for(uint32_t k3=0;k3<2;++k3)for(uint32_t cat=4;cat<10;++cat)for(uint32_t dm=0;dm<2;++dm){
            reset();defaults();K.k4=k4;K.k3=k3?k4:20;K.k1c=0x100|cat;damaging=dm;frame();
            px_battle_side_crafty_shield(HANDLER,CTX,0,WORK,&api);
            char want[128]="";const int hit=k4<12&&!dm&&!k3&&cat!=8&&cat!=6;
            if(k4<12){ strcat(want,"dm21 ");if(hit){ char c[64];snprintf(c,sizeof c,"msg323@7800/2 arg%x@7800 ",k4&0xff);strcat(want,c); } }
            expect(want);CHECK(key(0x41)==(uint32_t)hit);
        }
        for(uint32_t k4=1;k4<26;k4+=4)for(uint32_t dm=0;dm<2;++dm)for(unsigned locked=0;locked<2;++locked){
            reset();defaults();K.k4=k4;K.k12=0x10021;damaging=dm;frame();if(locked)lock();
            px_battle_side_mat_block(HANDLER,CTX,0,WORK,&api);
            char want[128]="";
            if(k4<12){ strcat(want,"dm21 ");if(dm&&!locked)strcat(want,"msgd5@7800/1 arg21@7800 "); }
            expect(want);CHECK(key(0x41)==(k4<12&&dm&&!locked)&&key(0x53)==(k4<12&&dm&&!locked));
        }
    }
    /* Sea of Fire and Sticky Web. */
    {
        for(uint32_t b=0;b<0x18;++b)for(uint32_t pres=0;pres<2;++pres)for(uint32_t dec=0;dec<2;++dec)for(uint32_t fr=0;fr<2;++fr)for(uint32_t sd=0;sd<2;++sd){
            reset();defaults();present=pres;decided=dec;fire=fr;K.k2=b|0x100;maxhp[b]=88;frame();
            px_battle_side_sea_of_fire(HANDLER,CTX,sd,WORK,&api);
            char want[200];char c[128];snprintf(want,sizeof want,"sp%u ",b);
            if(pres){ strcat(want,"dec ");
                if(!dec&&px_battle_slot_side(b)==sd){ snprintf(c,sizeof c,"ht9/%u ",b);strcat(want,c);
                    if(!fr){ snprintf(c,sizeof c,"rec38/%x sop%u commit0 rec6/1f ve/%u msg48a@c/2 arg%x@c commit1 ",b,b,b,b);strcat(want,c); } } }
            expect(want);
            if(pres&&!dec&&px_battle_slot_side(b)==sd&&!fr){
                CHECK(r16(NULL,RECORD+4)==0x27c&&r8(NULL,RECORD+6)==(0x30u|b)&&r8(NULL,RECORD+7)==0xa);
                CHECK(r16(NULL,RECORD+0x104)==11&&r8(NULL,RECORD+0x106)==b); }
        }
        reset();defaults();K.k2=0x1f;frame();px_battle_side_sea_of_fire(HANDLER,CTX,1,WORK,&api);expect("");
        for(uint32_t sd=0;sd<2;++sd)for(uint32_t air=0;air<2;++air){
            const uint32_t b=sd?13:2;
            reset();defaults();airborne=air;K.k2=b;frame();w8(NULL,RECORD+0x18,0x81);
            px_battle_side_sticky_web(HANDLER,CTX,sd,WORK,&api);
            char want[160];if(air){ snprintf(want,sizeof want,"ab%u ",b);expect(want);continue; }
            snprintf(want,sizeof want,"ab%u recd/1f msg4c6@1c/2 arg%x@1c commit0 ",b,b);expect(want);
            CHECK(r8(NULL,RECORD+0x16)==1&&r8(NULL,RECORD+4)==5&&r8(NULL,RECORD+0x17)==0xff&&r8(NULL,RECORD+0xc)==b&&r8(NULL,RECORD+0x18)==0x99);
        }
        reset();defaults();frame();px_battle_side_sticky_web(HANDLER,CTX,1,WORK,&api);expect("");
    }
    /* Through the dispatcher: Tailwind's speed on event 0x13, Safeguard on 0x6e. */
    {
        reset();defaults();K.k2=2;K.k4=2;K.k3=20;K.k1e=1;
        w8(NULL,0x7a00,0x13);w32(NULL,0x7a00+4,0);w8(NULL,0x7a00+8,0x6e);w32(NULL,0x7a00+12,1);
        w32(NULL,HANDLER+8,0x7a00);w32(NULL,HANDLER+0x18,0x20000000|1u<<16);w8(NULL,HANDLER+0x3a,0);w32(NULL,BATTLE+0x60,HANDLER);
        frame();px_battle_event_dispatch(CTX,0x13,&events);CHECK(key(0x36)==0x2000);++cases;
        w32(NULL,HANDLER+8,0x7a00+8);log_[0]=0;px_battle_event_dispatch(CTX,0x6e,&events);CHECK(r32(NULL,HANDLER+0x1c)==1&&key(0x42)==1&&aborts==0);++cases;
    }
    printf("battle_fields_1: %u cases, all tests passed\n",cases);
    return 0;
}
