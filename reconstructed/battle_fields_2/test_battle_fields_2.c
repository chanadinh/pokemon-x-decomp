#include "battle_fields_2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3400,
       RECORD=0x7000, HANDLER=0x9e00, WORK=0x9f00, MON=0xa000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static uint32_t airborne,exempt,maxhp[32];static unsigned aborts,records;static char log_[512];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[64];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static uint32_t who(uint32_t p) { CHECK(p>=MON&&p<MON+0x3000&&!((p-MON)%0x200));return (p-MON)/0x200; }
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;CHECK(k==0xe);notef("ve/%u ",who(p),0);return maxhp[who(p)]; }
static const struct px_battle_ability_api base={.events=&events,.pokemon_value=pokemon_value};
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id)
{ (void)u;CHECK(c==CTX&&records<3);notef("rec%x/%x ",k,id);return RECORD+0x100*records++; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id)
{ (void)u;char t[64];snprintf(t,sizeof t,"msg%x@%x/%x ",id,(r-RECORD)&0xff,k);note(t); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;notef("arg%x@%x ",v,(r-RECORD)&0xff); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r>=RECORD);notef("commit%u ",(r-RECORD)>>8,0); }
static const struct px_battle_items_api items={&base,NULL,NULL,NULL,action_record,message,message_arg,commit,0,0};
static const struct px_battle_abilities_1_api abilities={.base=&base};
static uint32_t is_airborne(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("ab%u ",id,0);return airborne; }
static uint32_t is_exempt(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("ex%u ",id,0);return exempt; }
static const struct px_battle_fields_2_api api={&items,&abilities,is_airborne,is_exempt};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_fields_2_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_grassy_power,px_battle_grassy_heal};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);airborne=0;exempt=0;aborts=0;records=0;log_[0]=0;
    for(unsigned i=0;i<32;++i)maxhp[i]=160;
    w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<0x18;++i)w32(NULL,TABLE+0x84+4*i,MON+0x200*i);
}
struct keys { uint32_t k2,k3,k4,k12,k16,k1e,k32,k36,k3e; };
static struct keys K;
static void defaults(void) { memset(&K,0,sizeof K);K.k2=2;K.k3=3;K.k4=4;K.k12=0x21;K.k16=0x15;K.k1e=1;K.k32=0x1000;K.k36=0x1000;K.k3e=5; }
static void frame(void)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x02,K.k2,&events);px_battle_event_set_id(0x03,K.k3,&events);px_battle_event_set_id(0x04,K.k4,&events);
    px_battle_event_set_value(0x12,K.k12,&events);px_battle_event_set_value(0x16,K.k16,&events);px_battle_event_set_value(0x1e,K.k1e,&events);
    px_battle_event_set_value(0x3e,K.k3e,&events);px_battle_event_set_multiplier(0x32,K.k32,0,0,&events);
    px_battle_event_set_multiplier(0x36,K.k36,0,0,&events);px_battle_event_set_value(0x42,0,&events);
    px_battle_event_set_value(0x4b,0,&events);px_battle_event_set_value(0x4c,0,&events);log_[0]=0;
}
static void lock(void)
{
    CHECK(px_battle_event_rewrite(0x42,0,&events)==1&&px_battle_event_rewrite(0x4b,0,&events)==1&&px_battle_event_rewrite(0x4c,0,&events)==1);log_[0]=0;
}
static uint32_t key(uint32_t k) { return px_battle_event_get(k,&events); }
static uint32_t times(uint32_t v,uint32_t f) { return (uint32_t)((uint64_t)((int64_t)(int32_t)v*(int32_t)f+0x800)>>12); }
static unsigned cases;
static void expect(const char *s) { if(strcmp(log_,s)){fprintf(stderr,"got  [%s]\nwant [%s]\n",log_,s);} CHECK(!strcmp(log_,s));++cases; }
static void only(uint32_t at,const uint32_t *o,unsigned n)
{
    for(uint32_t k=0;k<0x100;++k){ int listed=0;for(unsigned i=0;i<n;++i)listed|=o[i]==k;if(!listed)CHECK(r8(NULL,at+k)==0); }
}
int main(void)
{
    /* The regular rows: the no-ops touch nothing, the others one key each. */
    {
        reset();defaults();frame();px_battle_position_noop_4f(HANDLER,CTX,0,WORK,&api);px_battle_position_noop(HANDLER,CTX,0,WORK,&api);expect("");
        CHECK(key(0x32)==0x1000&&key(0x36)==0x1000&&key(0x42)==0&&records==0);
        for(unsigned locked=0;locked<2;++locked){
            reset();defaults();frame();if(locked)lock();px_battle_position_flag_4b(HANDLER,CTX,0,WORK,&api);CHECK(key(0x4b)==!locked);++cases;
        }
        const uint32_t starts[]={0x1000,0x1001,3,0x7fffffff};
        for(unsigned s=0;s<4;++s){
            reset();defaults();K.k36=starts[s];frame();px_battle_position_accuracy(HANDLER,CTX,0,WORK,&api);CHECK(key(0x36)==times(starts[s],0x1ab8)&&key(0x32)==0x1000);++cases;
        }
        for(uint32_t t=0;t<0x14;++t)for(unsigned locked=0;locked<2;++locked){
            reset();defaults();K.k16=t;frame();if(locked)lock();px_battle_position_ground(HANDLER,CTX,0,WORK,&api);CHECK(key(0x4c)==(t==4&&!locked));++cases;
            reset();defaults();K.k16=t;K.k32=0x1555;frame();px_battle_position_water_sport(HANDLER,CTX,0,WORK,&api);
            CHECK(key(0x32)==(t==9?times(0x1555,0x548):0x1555u));
            reset();defaults();K.k16=t;K.k32=0x1555;frame();px_battle_position_mud_sport(HANDLER,CTX,0,WORK,&api);
            CHECK(key(0x32)==(t==0xc?times(0x1555,0x548):0x1555u));
            reset();defaults();K.k16=t|0x100;frame();px_battle_position_ground(HANDLER,CTX,0,WORK,&api);CHECK(key(0x4c)==0);
            reset();defaults();K.k16=t;frame();if(locked)CHECK(px_battle_event_rewrite(0x16,t,&events)==1);log_[0]=0;px_battle_position_ion_deluge(HANDLER,CTX,0,WORK,&api);
            CHECK(key(0x16)==(t==0&&!locked?0xcu:t));cases+=4;
        }
        const uint32_t counts[]={0,1,5,0xff,0x7fffffff,0xffffffff};
        for(unsigned c=0;c<6;++c){ reset();defaults();K.k3e=counts[c];frame();px_battle_position_count_3e(HANDLER,CTX,0,WORK,&api);
            CHECK(key(0x3e)==counts[c]+1);++cases; }
    }
    /* The terrain powers: type, grounded attacker or defender, and the three moves. */
    {
        const uint32_t moves[]={0x21,0x59,0xde,0x20b,0x10059,0x20a,0x20c,0x58};
        for(uint32_t t=0;t<0x14;++t)for(uint32_t air=0;air<2;++air)for(uint32_t ex=0;ex<2;++ex)for(unsigned m=0;m<8;++m){
            reset();defaults();airborne=air;exempt=ex;K.k16=t;K.k12=moves[m];K.k32=0x1555;frame();px_battle_grassy_power(HANDLER,CTX,0,WORK,&api);
            char want[128]="";uint32_t v=0x1555;const int g=!air&&!ex;
            if(t==0xb){ strcat(want,"ab3 ");if(!air)strcat(want,"ex3 ");if(g)v=times(v,0x1800); }
            const uint32_t mv=moves[m]&0xffff;
            if(mv==0x59||mv==0xde||mv==0x20b){ strcat(want,"ab4 ");if(!air)strcat(want,"ex4 ");if(g)v=times(v,0x800); }
            expect(want);CHECK(key(0x32)==v);
            reset();defaults();airborne=air;exempt=ex;K.k16=t;K.k32=0x1555;frame();px_battle_misty_power(HANDLER,CTX,0,WORK,&api);
            strcpy(want,"");v=0x1555;if(t==0xf){ strcat(want,"ab4 ");if(!air)strcat(want,"ex4 ");if(g)v=times(v,0x800); }
            expect(want);CHECK(key(0x32)==v);
            reset();defaults();airborne=air;exempt=ex;K.k16=t;K.k32=0x1555;frame();px_battle_electric_power(HANDLER,CTX,0,WORK,&api);
            strcpy(want,"");v=0x1555;if(t==0xc){ strcat(want,"ab3 ");if(!air)strcat(want,"ex3 ");if(g)v=times(v,0x1800); }
            expect(want);CHECK(key(0x32)==v);
        }
    }
    /* Grassy Terrain's end-of-turn heal: slot 0x1f, the two exemptions and every max HP. */
    {
        for(uint32_t b=0;b<0x18;++b)for(uint32_t air=0;air<2;++air)for(uint32_t ex=0;ex<2;++ex){
            reset();defaults();airborne=air;exempt=ex;K.k2=b|0x100;maxhp[b]=88;frame();px_battle_grassy_heal(HANDLER,CTX,0,WORK,&api);
            char want[128];snprintf(want,sizeof want,"ab%u ",b);if(!air){ char c[16];snprintf(c,sizeof c,"ex%u ",b);strcat(want,c); }
            if(!air&&!ex){ char c[96];snprintf(c,sizeof c,"rec4/%x ve/%u msg183@8/2 arg%x@8 commit0 ",b,b,b);strcat(want,c); }
            expect(want);
            if(!air&&!ex){ CHECK(r16(NULL,RECORD+4)==5&&r8(NULL,RECORD+6)==b);const uint32_t o[]={4,5,6};only(RECORD,o,3); }
        }
        reset();defaults();K.k2=0x1f;frame();px_battle_grassy_heal(HANDLER,CTX,0,WORK,&api);expect("");
        for(uint32_t mx=0;mx<=2000;++mx){ reset();defaults();K.k2=5;maxhp[5]=mx;frame();px_battle_grassy_heal(HANDLER,CTX,0,WORK,&api);
            CHECK(r16(NULL,RECORD+4)==(mx/16?mx/16:1));++cases; }
    }
    /* The blocks: every status, a rewrite that does not take, and sleep falling back to the second test. */
    {
        for(uint32_t st=0;st<0x12;++st)for(uint32_t air=0;air<2;++air)for(uint32_t ex=0;ex<2;++ex)for(unsigned locked=0;locked<2;++locked){
            reset();defaults();airborne=air;exempt=ex;K.k1e=st|0x10000;frame();if(locked)lock();px_battle_misty_block(HANDLER,CTX,0,WORK,&api);
            char want[96];snprintf(want,sizeof want,"ab4 ");if(!air)strcat(want,"ex4 ");
            const int hit=!air&&!ex&&st<6&&!locked;
            if(hit)strcat(want,"rec17/4 commit0 ");
            expect(want);CHECK(key(0x42)==(uint32_t)(!air&&!ex&&st<6&&!locked));
            if(hit){ CHECK(r8(NULL,RECORD+4)==0x11&&r8(NULL,RECORD+5)==4);const uint32_t o[]={4,5};only(RECORD,o,2); }
            reset();defaults();airborne=air;exempt=ex;K.k1e=st|0x10000;frame();if(locked)lock();px_battle_electric_block(HANDLER,CTX,0,WORK,&api);
            snprintf(want,sizeof want,"ab4 ");if(!air)strcat(want,"ex4 ");
            const int ehit=!air&&!ex&&(st==2||st==0xe)&&!locked;
            if(ehit)strcat(want,"rec17/4 commit0 ");
            expect(want);CHECK(key(0x42)==(uint32_t)ehit);
            if(ehit){ CHECK(r8(NULL,RECORD+4)==0x10&&r8(NULL,RECORD+5)==4); }
        }
        reset();defaults();K.k4=0x104;frame();px_battle_misty_block(HANDLER,CTX,0,WORK,&api);expect("ab4 ex4 rec17/4 commit0 ");
    }
    /* The shows: the state bit of the battler, and the first record committed twice. */
    {
        for(uint32_t bit=0;bit<2;++bit)for(uint32_t b=0;b<8;++b){
            const uint32_t flags[]={0x11,0x10};
            for(unsigned e=0;e<2;++e){
                reset();defaults();K.k4=b;frame();
                w8(NULL,MON+0x200*b+0x169+(flags[e]>>3),(uint8_t)(bit?1u<<(flags[e]&7):0));
                w8(NULL,MON+0x200*b+0x169+((flags[e]>>3)^1),0xff&~(bit?1u<<(flags[e]&7):0)&0);
                if(e)px_battle_electric_show(HANDLER,CTX,0,WORK,&api); else px_battle_misty_show(HANDLER,CTX,0,WORK,&api);
                char want[160]="";
                if(bit){ snprintf(want,sizeof want,"rec18/%x commit0 rec3/%x msg%x@4/2 arg%x@4 commit0 ",b,b,e?0x4b7:0x4c9,b); }
                expect(want);
                if(bit){ CHECK(r8(NULL,RECORD+4)==flags[e]&&r8(NULL,RECORD+5)==b);const uint32_t o[]={4,5};only(RECORD,o,2); }
            }
        }
        /* Other bits of the state bytes do not count. */
        reset();defaults();K.k4=2;frame();w8(NULL,MON+0x400+0x169+2,0xfd);w8(NULL,MON+0x400+0x169+1,0xff);px_battle_misty_show(HANDLER,CTX,0,WORK,&api);
        expect("");
    }
    /* Through the dispatcher: Grassy Terrain's power on 0x3d and heal on 0x7f. */
    {
        reset();defaults();K.k16=0xb;K.k2=5;K.k3=2;
        w8(NULL,0x7a00,0x3d);w32(NULL,0x7a00+4,0);w8(NULL,0x7a00+8,0x7f);w32(NULL,0x7a00+12,1);
        w32(NULL,HANDLER+8,0x7a00);w32(NULL,HANDLER+0x18,0x20000000|1u<<16);w8(NULL,HANDLER+0x3a,0);w32(NULL,BATTLE+0x60,HANDLER);
        frame();px_battle_event_dispatch(CTX,0x3d,&events);CHECK(key(0x32)==times(0x1000,0x1800));++cases;
        w32(NULL,HANDLER+8,0x7a00+8);log_[0]=0;px_battle_event_dispatch(CTX,0x7f,&events);
        expect("ab5 ex5 rec4/5 ve/5 msg183@8/2 arg5@8 commit0 ");CHECK(aborts==0);
    }
    printf("battle_fields_2: %u cases, all tests passed\n",cases);
    return 0;
}
