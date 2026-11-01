#include "battle_power.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000, HANDLER=0x6000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static uint32_t gender[8],statuses[8],flags[0x400],recoil[0x400],sheer[0x400],excluded[0x400],biting[0x400],last,weather_value;
static unsigned aborts;static char log_[512];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static uint32_t value(void *u,uint32_t p,uint32_t k) { (void)u;CHECK(k==0x12);char b[8];snprintf(b,sizeof b,"g%u ",mon(p));note(b);return gender[mon(p)]; }
static const struct px_battle_ability_api base={&events,value,NULL,NULL,NULL,NULL,NULL};
static uint32_t move_flag(void *u,uint32_t m,uint32_t f) { (void)u;CHECK(m<0x400&&f<32);note("mf ");return flags[m]>>f&1; }
static uint32_t move_value(void *u,uint32_t m,uint32_t f) { (void)u;CHECK(m<0x400&&f==0x1e);note("mv ");return recoil[m]; }
static uint32_t sheer_move(void *u,uint32_t m) { (void)u;CHECK(m<0x400);return sheer[m]; }
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;note("hs ");return statuses[mon(p)]==s; }
static uint32_t analytic_excluded(void *u,uint32_t m) { (void)u;CHECK(m<0x400);note("ex ");return excluded[m]; }
static uint32_t moved_last(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);note("ml ");return last==id; }
static uint32_t weather(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("w ");return weather_value; }
static uint32_t biting_move(void *u,uint32_t m) { (void)u;CHECK(m<0x400);return biting[m]; }
static const struct px_battle_power_api api={&base,move_flag,move_value,sheer_move,has_status,analytic_excluded,moved_last,weather,biting_move};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static void (*const table[])(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_power_api*)={
        px_battle_technician,px_battle_dark_aura,px_battle_fairy_aura,px_battle_iron_fist};
    CHECK(f<4);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(gender,0,sizeof gender);memset(statuses,0,sizeof statuses);memset(flags,0,sizeof flags);
    memset(recoil,0,sizeof recoil);memset(sheer,0,sizeof sheer);memset(excluded,0,sizeof excluded);memset(biting,0,sizeof biting);
    last=99;weather_value=0;aborts=0;log_[0]=0;w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
/* The power frame: attacker, defender, move id, type, category byte, power, multiplier, aura keys. */
static void frame(uint32_t attacker,uint32_t defender,uint32_t move,uint32_t type,uint32_t category,uint32_t power,uint32_t aura_break)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x03,attacker,&events);px_battle_event_set_id(0x04,defender,&events);
    px_battle_event_set_id(0x12,move,&events);px_battle_event_set_id(0x16,type,&events);px_battle_event_set_id(0x1b,category,&events);
    px_battle_event_set_value(0x31,power,&events);px_battle_event_set_multiplier(0x32,0x1000,0,0,&events);
    px_battle_event_set_value(0x57,aura_break,&events);px_battle_event_set_value(0x58,0,&events);
    log_[0]=0;
}
static uint32_t mult(void) { return px_battle_event_get(0x32,&events); }
static uint32_t ref(uint32_t f) { return (uint32_t)(((uint64_t)0x1000*f+0x800)>>12); }
typedef void (*handler_fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_power_api*);
int main(void)
{
    unsigned cases=0;
    /* Attacker-side handlers: no effect unless key 0x03 is the battler. */
    {
        const handler_fn attackers[]={px_battle_rivalry,px_battle_technician,px_battle_iron_fist,px_battle_reckless,px_battle_sheer_force,
            px_battle_toxic_boost,px_battle_flare_boost,px_battle_analytic,px_battle_sand_force,px_battle_strong_jaw,px_battle_tough_claws,
            px_battle_aerilate,px_battle_pixilate,px_battle_refrigerate,px_battle_mega_launcher};
        for(unsigned k=0;k<sizeof attackers/sizeof *attackers;++k) {
            reset();for(unsigned i=0;i<0x400;++i)flags[i]=~0u,recoil[i]=sheer[i]=biting[i]=1;last=2;weather_value=4;
            frame(3,1,0x160,2,1,10,0);w32(NULL,WORK,1);attackers[k](HANDLER,CTX,2,WORK,&api);
            CHECK(mult()==0x1000&&log_[0]==0&&aborts==0);++cases;
        }
        const handler_fn defenders[]={px_battle_heatproof,px_battle_dry_skin_power};
        for(unsigned k=0;k<2;++k) { reset();frame(2,3,5,9,1,10,0);defenders[k](HANDLER,CTX,2,WORK,&api);CHECK(mult()==0x1000);++cases; }
    }
    /* Rivalry: same gender x1.25, opposite x0.75, none when either is genderless; low byte of key 0x04. */
    for(uint32_t g1=0;g1<4;++g1)for(uint32_t g2=0;g2<4;++g2) {
        reset();gender[2]=g1|0x100;gender[5]=g2;frame(2,0x305,5,9,1,10,0);px_battle_rivalry(HANDLER,CTX,2,WORK,&api);
        const uint32_t a=g1&0xff,b=g2&0xff;
        CHECK(mult()==(a==2||b==2?0x1000u:a==b?ref(0x1400):ref(0xc00))&&!strcmp(log_,"g2 g5 "));++cases;
    }
    /* Technician: power at most 60. */
    for(uint32_t power=0;power<200;++power) { reset();frame(2,1,5,9,1,power,0);px_battle_technician(HANDLER,CTX,2,WORK,&api);CHECK(mult()==(power<=60?ref(0x1800):0x1000u));++cases; }
    reset();frame(2,1,5,9,1,0x8000003c,0);px_battle_technician(HANDLER,CTX,2,WORK,&api);CHECK(mult()==0x1000);++cases;  /* unsigned */
    /* Iron Fist and Tough Claws: move flags 7 and 0 of the 16-bit move id. */
    for(uint32_t f=0;f<4;++f) {
        reset();flags[0x123]=f==1?0x80:f==2?0x1:f==3?0x81:0;frame(2,1,0x10123,9,1,50,0);
        px_battle_iron_fist(HANDLER,CTX,2,WORK,&api);CHECK(mult()==((f&1)?ref(0x1333):0x1000u));
        reset();flags[0x123]=f==1?0x80:f==2?0x1:f==3?0x81:0;frame(2,1,0x10123,9,1,50,0);
        px_battle_tough_claws(HANDLER,CTX,2,WORK,&api);CHECK(mult()==((f&2)?ref(0x14cd):0x1000u));++cases;
    }
    /* Reckless: recoil moves, Jump Kick and High Jump Kick; recoil asked first. */
    for(uint32_t m=0;m<0x200;++m)for(uint32_t r=0;r<2;++r) {
        reset();recoil[m]=r;frame(2,1,m,9,1,50,0);px_battle_reckless(HANDLER,CTX,2,WORK,&api);
        CHECK(mult()==(r||m==26||m==136?ref(0x1333):0x1000u)&&!strcmp(log_,"mv "));++cases;
    }
    /* Heatproof and Dry Skin: the defender against Fire (full word). */
    for(uint32_t t=0;t<0x20;++t)for(unsigned wide=0;wide<2;++wide) {
        const uint32_t type=t|(wide?0x100u:0);
        reset();frame(1,2,5,type,1,50,0);px_battle_heatproof(HANDLER,CTX,2,WORK,&api);CHECK(mult()==(type==9?ref(0x800):0x1000u));
        reset();frame(1,2,5,type,1,50,0);px_battle_dry_skin_power(HANDLER,CTX,2,WORK,&api);CHECK(mult()==(type==9?ref(0x1400):0x1000u));++cases;
    }
    /* Sheer Force and Strong Jaw: move predicates. */
    for(uint32_t s=0;s<2;++s) {
        reset();sheer[7]=s;frame(2,1,7,9,1,50,0);px_battle_sheer_force(HANDLER,CTX,2,WORK,&api);CHECK(mult()==(s?ref(0x14cd):0x1000u));
        reset();biting[7]=s;frame(2,1,7,9,1,50,0);px_battle_strong_jaw(HANDLER,CTX,2,WORK,&api);CHECK(mult()==(s?ref(0x1800):0x1000u));++cases;
    }
    /* Toxic Boost (poison, physical) and Flare Boost (burn, special). */
    for(uint32_t st=0;st<7;++st)for(uint32_t cat=0;cat<4;++cat) {
        reset();statuses[2]=st;frame(2,1,7,9,cat,50,0);px_battle_toxic_boost(HANDLER,CTX,2,WORK,&api);
        CHECK(mult()==(st==5&&cat==1?ref(0x1800):0x1000u)&&!strncmp(log_,"hs ",3));
        reset();statuses[2]=st;frame(2,1,7,9,cat,50,0);px_battle_flare_boost(HANDLER,CTX,2,WORK,&api);
        CHECK(mult()==(st==4&&cat==2?ref(0x1800):0x1000u));++cases;
    }
    /* Analytic: an excluded move is never boosted; otherwise moving last. */
    for(uint32_t ex=0;ex<2;++ex)for(uint32_t l=0;l<2;++l) {
        reset();excluded[7]=ex;last=l?2:4;frame(2,1,7,9,1,50,0);px_battle_analytic(HANDLER,CTX,2,WORK,&api);
        CHECK(mult()==(!ex&&l?ref(0x14cd):0x1000u)&&!strcmp(log_,ex?"ex ":"ex ml "));++cases;
    }
    /* Sand Force: sandstorm, Ground/Rock/Steel (low byte); the weather is asked first. */
    for(uint32_t w=0;w<6;++w)for(uint32_t t=0;t<0x14;++t) {
        reset();weather_value=w;frame(2,1,7,t|0x200,1,50,0);px_battle_sand_force(HANDLER,CTX,2,WORK,&api);
        CHECK(mult()==(w==4&&(t==4||t==5||t==8)?ref(0x14cd):0x1000u)&&!strncmp(log_,"w ",2));++cases;
    }
    /* Auras: any battler, the full type word, only once per frame, Aura Break. */
    for(unsigned k=0;k<2;++k)for(uint32_t t=0;t<0x14;++t)for(uint32_t ab=0;ab<2;++ab) {
        const handler_fn fn=k?px_battle_fairy_aura:px_battle_dark_aura;const uint32_t type=k?17:16;
        reset();frame(7,6,7,t,1,50,ab);fn(HANDLER,CTX,2,WORK,&api);
        const uint32_t once=t==type?(ab?ref(0xc00):ref(0x1548)):0x1000u;CHECK(mult()==once);
        fn(HANDLER,CTX,3,WORK,&api);CHECK(mult()==once&&aborts==0);   /* second aura: rewrite refused */
        CHECK(px_battle_event_get(0x58,&events)==(t==type));++cases;
    }
    reset();frame(7,6,7,16,1,50,0);px_battle_event_rewrite(0x58,5,&events);px_battle_dark_aura(HANDLER,CTX,2,WORK,&api);CHECK(mult()==0x1000);++cases;
    /* Aerilate, Pixilate, Refrigerate: converted type (full word) and the work word. */
    {
        const handler_fn fns[3]={px_battle_aerilate,px_battle_pixilate,px_battle_refrigerate};const uint32_t types[3]={2,17,14};
        for(unsigned k=0;k<3;++k)for(uint32_t t=0;t<0x14;++t)for(uint32_t wv=0;wv<3;++wv) {
            reset();w32(NULL,WORK,wv==2?0x80000000u:wv);frame(2,1,7,t,1,50,0);fns[k](HANDLER,CTX,2,WORK,&api);
            CHECK(mult()==(t==types[k]&&wv?ref(0x14cd):0x1000u));++cases;
        }
        reset();w32(NULL,WORK,1);frame(2,1,7,0x102,1,50,0);px_battle_aerilate(HANDLER,CTX,2,WORK,&api);CHECK(mult()==0x1000);++cases;
    }
    /* Mega Launcher: four pulse/aura moves of the 16-bit id. */
    for(uint32_t m=0;m<0x400;++m) {
        reset();frame(2,1,m|0x30000,9,1,50,0);px_battle_mega_launcher(HANDLER,CTX,2,WORK,&api);
        CHECK(mult()==(m==352||m==399||m==406||m==396?ref(0x1800):0x1000u));++cases;
    }
    /* Through the dispatcher: Technician, Dark Aura and Fairy Aura handlers on event 0x3d. */
    {
        reset();flags[5]=0x80;
        for(unsigned i=0;i<4;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7000+0x10*i;
            w8(NULL,entries,0x3d);w32(NULL,entries+4,i==3?0:i==0?3:i);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,2);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<3?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);
        frame(2,1,5,16,1,40,0);px_battle_event_dispatch(CTX,0x3d,&events);
        /* Iron Fist x1.2, Dark Aura x1.33 (Dark move), Fairy Aura skipped, Technician x1.5. */
        const uint32_t a=ref(0x1333),b=(uint32_t)(((uint64_t)a*0x1548+0x800)>>12),c=(uint32_t)(((uint64_t)b*0x1800+0x800)>>12);
        CHECK(mult()==c&&aborts==0);++cases;
    }
    printf("battle_power: %u cases, all tests passed\n",cases);
    return 0;
}
