#include "battle_abilities_2.h"
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
static uint32_t max_hp[8],weather_value,category_value,status_value;
static unsigned aborts,ncommit;static uint8_t commits[2][0x20];static char log_[512];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static uint32_t value(void *u,uint32_t p,uint32_t k) { (void)u;notef("v%u/%x ",mon(p),k);CHECK(k==0xe);return max_hp[mon(p)]; }
static uint32_t move_category(void *u,uint32_t m) { (void)u;notef("cat%x ",m,0);return category_value; }
static uint32_t pokemon_status(void *u,uint32_t p) { (void)u;notef("st%u ",mon(p),0);return status_value; }
static const struct px_battle_ability_api base={&events,value,move_category,pokemon_status,NULL,NULL,NULL};
static uint32_t weather(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("w ");return weather_value; }
static void popup(void *u,uint32_t c,uint32_t p,uint32_t id) { (void)u;CHECK(c==CTX);notef("pop%u/%u ",p,id); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);
    memset(mem+RECORD,0xa5,0x20);w32(NULL,RECORD,0x11);return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;notef("msg%x/%x ",id,(r-RECORD)*16+k); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD&&ncommit<2);memcpy(commits[ncommit++],mem+RECORD,0x20);note("commit "); }
static const struct px_battle_abilities_2_api api={&base,weather,popup,action_record,message,commit};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_abilities_2_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_overcoat,px_battle_ice_body};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(commits,0,sizeof commits);aborts=ncommit=0;log_[0]=0;w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i){w32(NULL,TABLE+0x84+4*i,MON+0x100*i);max_hp[i]=0;}
}
struct keys { uint32_t k2,k3,k4,k33,k3a; };
static void frame(struct keys k)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x02,k.k2,&events);px_battle_event_set_id(0x03,k.k3,&events);px_battle_event_set_id(0x04,k.k4,&events);
    px_battle_event_set_value(0x12,0x10077,&events);px_battle_event_set_value(0x33,k.k33,&events);px_battle_event_set_value(0x3a,k.k3a,&events);
    px_battle_event_set_multiplier(0x36,0x1000,0,0x100000,&events);px_battle_event_set_value(0x42,0,&events);log_[0]=0;
}
static uint32_t c16(unsigned n,unsigned o) { return (uint32_t)(commits[n][o]|commits[n][o+1]<<8); }
static uint32_t c32(unsigned n,unsigned o) { return c16(n,o)|c16(n,o+2)<<16; }
static uint32_t part(uint32_t max,uint32_t d) { return max/d?max/d:1; }
enum { ICE, RAIN, DRY, SOLAR_W, OVERCOAT, SAND_I, SNOW_I, SWIFT, CHLORO, SANDRUSH, SANDVEIL, SNOWCLOAK, SOLAR, HYDRATION, LEAF, AIRLOCK, SUPPRESS, COUNT };
static const fn handlers[COUNT]={px_battle_ice_body,px_battle_rain_dish,px_battle_dry_skin_weather,px_battle_solar_power_weather,px_battle_overcoat,
    px_battle_sand_immunity,px_battle_snow_cloak_weather,px_battle_swift_swim,px_battle_chlorophyll,px_battle_sand_rush,px_battle_sand_veil,
    px_battle_snow_cloak,px_battle_solar_power,px_battle_hydration,px_battle_leaf_guard,px_battle_air_lock,px_battle_air_lock_suppress};
/* Expected outcome: log, multiplier, key 0x42, and the record checks. */
struct outcome { char log[160]; uint32_t mult,cancelled; };
static struct outcome model(unsigned h,struct keys k,uint32_t max)
{
    struct outcome o={"",0x1000,0};char *L=o.log;const size_t n=sizeof o.log;(void)max;
    switch(h){
    case ICE: case RAIN:
        if(k.k3a!=(h==ICE?3u:2u)||k.k2!=ME)break;
        snprintf(L,n,"rec4/2 v2/e commit ");o.cancelled=1;break;
    case DRY:
        if(k.k2!=ME)break;
        if((k.k3a&0xff)==1)snprintf(L,n,"pop1/2 rec6/2 v2/e commit pop2/2 ");
        else if((k.k3a&0xff)==2)snprintf(L,n,"rec4/2 v2/e commit ");
        break;
    case SOLAR_W:
        if(k.k2!=ME||k.k3a!=1)break;
        snprintf(L,n,"v2/e pop1/2 rec6/2 commit pop2/2 ");break;
    case OVERCOAT: case SAND_I: case SNOW_I:
        if(k.k2!=ME||(int32_t)k.k33<=0)break;
        if(h==SAND_I&&k.k3a!=4)break;
        if(h==SNOW_I&&k.k3a!=3)break;
        o.cancelled=1;break;
    case SWIFT: case CHLORO: case SANDRUSH: case SANDVEIL: case SNOWCLOAK: {
        static const uint32_t wanted[5]={2,1,4,4,3};const unsigned i=h-SWIFT;
        if((i<3?k.k2:k.k4)!=ME)break;
        snprintf(L,n,"w ");
        if(weather_value==wanted[i])o.mult=i<3?0x2000:0xccd;
        break; }
    case SOLAR:
        if(k.k3!=ME)break;
        snprintf(L,n,"w ");if(weather_value!=1)break;
        snprintf(L,n,"w cat77 ");if(category_value==2)o.mult=0x1800;break;
    case HYDRATION:
        if(k.k2!=ME)break;
        snprintf(L,n,"w ");if(weather_value!=2)break;
        snprintf(L,n,"w st2 ");if(status_value)snprintf(L,n,"w st2 pop1/2 reca/2 commit pop2/2 ");break;
    case LEAF:
        if(k.k2!=ME)break;
        snprintf(L,n,"w ");if(weather_value==1)o.cancelled=1;break;
    case AIRLOCK:
        if(k.k2!=ME)break;
        snprintf(L,n,"rec1e/2 msg61/81 commit ");break;
    case SUPPRESS: o.cancelled=1;break;
    }
    return o;
}
static void check_record(unsigned h,struct keys k,uint32_t max)
{
    if(!ncommit)return;
    CHECK(ncommit==1);
    switch(h){
    case ICE: case RAIN: CHECK(commits[0][6]==ME&&c16(0,4)==part(max,16)&&c32(0,0)==0x800011&&commits[0][7]==0xa5);break;
    case DRY: CHECK(commits[0][6]==ME&&c16(0,4)==part(max,8)&&c32(0,0)==((k.k3a&0xff)==1?0x11u:0x800011u));break;
    case SOLAR_W: CHECK(commits[0][6]==ME&&c16(0,4)==part(max,8)&&c32(0,0)==0x11);break;
    case HYDRATION: CHECK(commits[0][4]==0x26&&commits[0][5]==ME&&commits[0][0x11]==1&&c32(0,0)==0x2000011&&commits[0][0x10]==0xa5);break;
    case AIRLOCK: CHECK(commits[0][4]==0&&commits[0][6]==1&&commits[0][5]==0xa5&&c32(0,0)==0x800011);break;
    default: CHECK(0);
    }
}
int main(void)
{
    unsigned cases=0;
    static const uint32_t skies[7]={0,1,2,3,4,0x101,0x102},damage[3]={0xffffffffu,0,1},maxes[3]={7,16,0x1234};
    for(unsigned h=0;h<COUNT;++h)for(unsigned who=0;who<8;++who)for(unsigned s=0;s<7;++s)for(uint32_t w=0;w<5;++w)
    for(unsigned d=0;d<3;++d)for(unsigned c=0;c<3;++c)for(uint32_t st=0;st<2;++st) {
        const struct keys k={who&1?ME:OTHER,who&2?ME:OTHER,who&4?ME:OTHER,damage[d],skies[s]};
        reset();max_hp[ME]=maxes[c];weather_value=w;category_value=c;status_value=st;frame(k);
        handlers[h](HANDLER,CTX,ME,WORK,&api);
        const struct outcome o=model(h,k,maxes[c]);
        if(strcmp(log_,o.log)){fprintf(stderr,"handler %u: got '%s' want '%s'\n",h,log_,o.log);CHECK(0);}
        CHECK(aborts==0&&px_battle_event_get(0x36,&events)==o.mult&&px_battle_event_get(0x42,&events)==o.cancelled);
        check_record(h,k,maxes[c]);
        ++cases;
    }
    /* Through the dispatcher: Overcoat then Ice Body on event 0x88 in hail. */
    {
        reset();max_hp[ME]=0x40;
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7c00+0x10*i;
            w8(NULL,entries,0x88);w32(NULL,entries+4,i);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ME);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);frame((struct keys){ME,OTHER,OTHER,1,3});px_battle_event_dispatch(CTX,0x88,&events);
        CHECK(!strcmp(log_,"rec4/2 v2/e commit ")&&aborts==0&&c16(0,4)==4);++cases;
    }
    printf("battle_abilities_2: %u cases, all tests passed\n",cases);
    return 0;
}
