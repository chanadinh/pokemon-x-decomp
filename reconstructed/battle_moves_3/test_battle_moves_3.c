#include "battle_moves_3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, SCRATCH=0x7000, ENTRIES=0x7900, NODES=0x8000, HOOK=0x108baba4 };
enum { ATT=2, DEF=5, A=MON+0x200, D=MON+0x500 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static unsigned aborts;static char log_[2048];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[64];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static uint32_t values[8][0x20];
static unsigned slot(uint32_t p) { CHECK(p>=MON&&p<MON+0x800&&!(p&0xff));return (p-MON)>>8; }
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;CHECK(k<0x20);notef("v%u.%x ",slot(p),k);return values[slot(p)][k]; }
static uint32_t categories[0x400];
static uint32_t move_category(void *u,uint32_t m) { (void)u;CHECK(m<0x400);notef("mc%u ",m,0);return categories[m]; }
static const struct px_battle_ability_api base={&events,pokemon_value,move_category,NULL,NULL,NULL,NULL};
static uint32_t rnd,records,ally_mask,record_limit,record_calls,rec_index[400];
static uint8_t rec_data[8][8];
static uint32_t random_(void *u,uint32_t r) { (void)u;notef("rnd%u ",r,0);return rnd; }
static uint32_t damage_record(void *u,uint32_t p,uint32_t k,uint32_t i,uint32_t out)
{
    (void)u;CHECK(p==A&&k==0&&out==SCRATCH&&i<0x100);
    if(record_limit) { CHECK(record_calls<400);rec_index[record_calls++]=i;for(unsigned b=0;b<8;++b) w8(NULL,out+b,0);w8(NULL,out+5,9);return record_calls<=record_limit; }
    notef("dr%u ",i,0);if(i>=records) return 0;
    for(unsigned b=0;b<8;++b) w8(NULL,out+b,rec_data[i][b]);
    return 1;
}
static uint32_t is_ally(void *u,uint32_t id,uint32_t o) { (void)u;notef("al%u/%u ",id,o);return o<32&&(ally_mask>>o&1); }
static const struct px_battle_moves_3_api api={&base,random_,damage_record,is_ally,SCRATCH,HOOK};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_3_api*);
static const fn dispatched[]={px_battle_counter,px_battle_sonic_boom,px_battle_dragon_rage};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w) { (void)u;CHECK(f<3);dispatched[f](h,c,v,w,&api); }
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;log_[0]=0;memset(values,0,sizeof values);memset(categories,0,sizeof categories);memset(rec_data,0,sizeof rec_data);
    rnd=records=ally_mask=record_limit=record_calls=0;
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i){ w32(NULL,TABLE+0x84+4*i,MON+0x100*i);mem[MON+0x100*i+0x19]=(uint8_t)i; }
}
/* A frame with the kinds the events give: 0x38 scaled; 0x15, 0x16 ids; 0x17, 0x39, 0x42, 0x4c, 0x53 values. */
static uint32_t def_type=0,att_type=0;
static void frame(uint32_t attacker,uint32_t def)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x03,attacker,&events);px_battle_event_set_id(0x04,def,&events);
    px_battle_event_set_id(0x15,def_type,&events);px_battle_event_set_id(0x16,att_type,&events);px_battle_event_set_value(0x17,0x12,&events);
    px_battle_event_set_scaled(0x38,0,&events);px_battle_event_set_value(0x39,0xe,&events);px_battle_event_set_value(0x42,0,&events);
    px_battle_event_set_value(0x4c,0,&events);px_battle_event_set_value(0x53,0,&events);log_[0]=0;
}
static uint32_t key(uint32_t k) { return px_battle_event_get(k,&events); }
static int untouched(uint32_t except)
{
    const uint32_t keys[]={0x17,0x38,0x39,0x42,0x4c,0x53},start[]={0x12,0,0xe,0,0,0};
    for(unsigned i=0;i<6;++i) if(keys[i]!=except&&key(keys[i])!=start[i]) return 0;
    return 1;
}
static uint32_t run(fn f) { frame(ATT,DEF);f(HANDLER,CTX,ATT,WORK,&api);CHECK(aborts==0);CHECK(untouched(0x38));return key(0x38); }
static int same(const char *expected) { if(strcmp(log_,expected)){ fprintf(stderr,"log [%s] expected [%s]\n",log_,expected);return 0; } return 1; }
int main(void)
{
    unsigned cases=0;
    const fn all[]={px_battle_sonic_boom,px_battle_dragon_rage,px_battle_seismic_toss,px_battle_psywave,px_battle_super_fang,px_battle_endeavor,
        px_battle_final_gambit,px_battle_bide_damage,px_battle_counter,px_battle_mirror_coat,px_battle_metal_burst,px_battle_brick_break_mark,
        px_battle_brick_break_clear,px_battle_thunder_wave_check,px_battle_thousand_arrows_check,px_battle_freeze_dry,px_battle_flying_press,
        px_battle_thousand_arrows_immunity,px_battle_thousand_arrows_airborne,px_battle_chip_away_matchup};
    /* Nothing happens unless key 0x03 is the handler's battler. */
    for(unsigned k=0;k<sizeof all/sizeof *all;++k)for(uint32_t other=0;other<3;++other) {
        reset();def_type=10;att_type=4;w32(NULL,WORK,2);w32(NULL,WORK+4,9);records=1;rec_data[0][2]=5;values[ATT][0xd]=values[ATT][0xf]=9;
        w32(NULL,HANDLER+0xc,0x77);
        const uint32_t key3=other==0?3:other==1?0x102:ATT,id=other==2?0x102:ATT;
        frame(key3,DEF);all[k](HANDLER,CTX,id,WORK,&api);
        CHECK(log_[0]==0&&untouched(0)&&r32(NULL,WORK)==2&&r32(NULL,HANDLER+0xc)==0x77&&aborts==0);++cases;
        def_type=att_type=0;
    }
    /* Fixed amounts and the attacker's level. */
    reset();CHECK(run(px_battle_sonic_boom)==20&&log_[0]==0);++cases;
    reset();CHECK(run(px_battle_dragon_rage)==40&&log_[0]==0);++cases;
    for(uint32_t l=0;l<3;++l){ reset();const uint32_t lv[3]={1,100,0x1234};values[ATT][0xf]=lv[l];values[DEF][0xf]=7;
        CHECK(run(px_battle_seismic_toss)==(lv[l]&0xff)&&same("v2.f "));++cases; }
    /* Psywave: level * (random(101) + 50) / 100, 16 bits, at least 1. */
    for(uint32_t lv=0;lv<0x110;++lv)for(uint32_t r=0;r<104;++r) {
        reset();values[ATT][0xf]=lv|(lv&0x100?0xff00:0);rnd=r==103?0xffff:r;
        const uint32_t level=values[ATT][0xf]&0xff,e=level*((rnd+50)&0xffff)/100&0xffff;
        CHECK(run(px_battle_psywave)==(e?e:1));++cases;
    }
    reset();values[ATT][0xf]=50;rnd=40;CHECK(run(px_battle_psywave)==45&&same("v2.f rnd101 "));++cases;
    reset();values[ATT][0xf]=0xff;rnd=0x10000-50+0x10000;CHECK(run(px_battle_psywave)==1);++cases;   /* (r + 50) narrowed to 0 */
    /* Super Fang: half the defender's HP, signed toward zero, 16 bits, at least 1. */
    {
        const uint32_t hp[]={0,1,2,3,100,101,0x1ffff,0x20000,0x20001,0xffffffff,0xfffffffd,0x80000000,0x7fffffff};
        for(unsigned k=0;k<13;++k){ reset();values[DEF][0xd]=hp[k];values[ATT][0xd]=50;
            const uint32_t e=(uint32_t)((int32_t)hp[k]/2)&0xffff;CHECK(run(px_battle_super_fang)==(e?e:1)&&same("v5.d "));++cases; }
        reset();values[DEF][0xd]=40;frame(ATT,0x300+DEF);px_battle_super_fang(HANDLER,CTX,ATT,WORK,&api);CHECK(key(0x38)==20);++cases;
    }
    /* Endeavor: defender HP - attacker HP, 1 when negative. */
    for(uint32_t d=0;d<40;d+=3)for(uint32_t a=0;a<40;a+=3) {
        reset();values[DEF][0xd]=d;values[ATT][0xd]=a;CHECK(run(px_battle_endeavor)==(d>=a?d-a:1u)&&same("v5.d v2.d "));++cases;
    }
    reset();values[DEF][0xd]=0x80000000;values[ATT][0xd]=1;CHECK(run(px_battle_endeavor)==0x7fffffff);++cases;   /* signed difference */
    reset();values[DEF][0xd]=0x7fffffff;values[ATT][0xd]=0xffffffff;CHECK(run(px_battle_endeavor)==1);++cases;
    /* Final Gambit. */
    reset();values[ATT][0xd]=123;values[DEF][0xd]=7;CHECK(run(px_battle_final_gambit)==123&&same("v2.d "));++cases;
    /* Bide: from work word 0 = 2 (signed), twice word 1; then word 0 = 3. */
    {
        const uint32_t w0[]={0,1,2,3,7,0x80000000},w1[]={0,1,40,0x80000001};
        for(unsigned i=0;i<6;++i)for(unsigned j=0;j<4;++j) {
            reset();w32(NULL,WORK,w0[i]);w32(NULL,WORK+4,w1[j]);frame(ATT,DEF);px_battle_bide_damage(HANDLER,CTX,ATT,WORK,&api);
            const int on=(int32_t)w0[i]>=2;
            CHECK(key(0x38)==(on?w1[j]<<1:0)&&r32(NULL,WORK)==(on?3u:w0[i])&&r32(NULL,WORK+4)==w1[j]&&aborts==0);++cases;
        }
        /* The attacker is looked up (an empty slot aborts). */
        reset();w32(NULL,WORK,2);w32(NULL,TABLE+0x84+4*ATT,0);frame(ATT,DEF);px_battle_bide_damage(HANDLER,CTX,ATT,WORK,&api);CHECK(aborts==1);++cases;
    }
    /* Counter, Mirror Coat, Metal Burst: the first record not from an ally and of the category. */
    {
        const fn f[3]={px_battle_counter,px_battle_mirror_coat,px_battle_metal_burst};const uint32_t kind[3]={1,2,0},factor[3]={0x2000,0x2000,0x1800};
        for(unsigned k=0;k<3;++k)for(uint32_t n=0;n<4;++n)for(uint32_t pattern=0;pattern<64;++pattern) {
            reset();records=n;ally_mask=1u<<3;
            for(uint32_t i=0;i<n;++i){ const uint32_t p=pattern>>(2*i)&3;rec_data[i][5]=(uint8_t)(p==0?3:4+i);
                const uint16_t move=(uint16_t)(100+i);rec_data[i][0]=(uint8_t)move;rec_data[i][1]=(uint8_t)(move>>8);categories[move]=p==1?1:p==2?2:3;
                rec_data[i][2]=(uint8_t)(17+i*31);rec_data[i][3]=(uint8_t)i; }
            mem[A+0x19]=ATT;
            uint32_t hit=0xff;
            for(uint32_t i=0;i<n;++i){ const uint32_t p=pattern>>(2*i)&3;if(p==0) continue;if(kind[k]&&categories[100+i]!=kind[k]) continue;hit=i;break; }
            const uint32_t got=run(f[k]);
            CHECK(got==(hit==0xff?0u:px_battle_modify((uint32_t)(17+hit*31)|hit<<8,factor[k])));++cases;
        }
        /* Calls: the user's own id (+0x19) asks about allies; the category only when one is required. */
        reset();records=2;mem[A+0x19]=6;rec_data[0][5]=3;rec_data[1][5]=4;rec_data[1][0]=0x21;rec_data[1][1]=1;rec_data[1][2]=50;categories[0x121]=1;ally_mask=1u<<3;
        CHECK(run(px_battle_counter)==100&&same("dr0 al6/3 dr1 al6/4 mc289 "));++cases;
        reset();records=1;mem[A+0x19]=6;rec_data[0][5]=4;rec_data[0][2]=50;CHECK(run(px_battle_metal_burst)==75&&same("dr0 al6/4 "));++cases;
        reset();records=1;rec_data[0][5]=4;rec_data[0][2]=0;CHECK(run(px_battle_metal_burst)==0&&same("dr0 al2/4 "));++cases;   /* zero damage taken */
        reset();records=1;rec_data[0][5]=4;rec_data[0][2]=0xff;rec_data[0][3]=0xff;categories[0]=1;CHECK(run(px_battle_counter)==px_battle_modify(0xffff,0x2000));++cases;
        /* Record indexes wrap at 256. */
        reset();record_limit=300;ally_mask=1u<<9;CHECK(run(px_battle_metal_burst)==0&&record_calls==301);
        for(uint32_t i=0;i<301;++i) { CHECK(rec_index[i]==(i&0xff)); }
        ++cases;
        /* The routine with other arguments, including the unused work word. */
        reset();records=1;rec_data[0][5]=4;rec_data[0][2]=100;frame(ATT,DEF);px_battle_counter_damage(CTX,ATT,0xdead,0,0x800,&api);CHECK(key(0x38)==50);++cases;
        reset();records=1;rec_data[0][5]=4;rec_data[0][2]=100;frame(ATT,DEF);px_battle_counter_damage(CTX,3,WORK,0,0x800,&api);CHECK(key(0x38)==0&&log_[0]==0);++cases;
    }
    /* Brick Break: the hook in handler word +0xc, cleared after. */
    reset();w32(NULL,HANDLER+0xc,5);run(px_battle_brick_break_mark);CHECK(r32(NULL,HANDLER+0xc)==HOOK&&key(0x38)==0&&log_[0]==0);++cases;
    reset();w32(NULL,HANDLER+0xc,HOOK);run(px_battle_brick_break_clear);CHECK(r32(NULL,HANDLER+0xc)==0&&r32(NULL,HANDLER+8)==0&&r32(NULL,HANDLER+0x10)==0);++cases;
    reset();w32(NULL,HANDLER+8,1);w32(NULL,HANDLER+0x10,2);px_battle_set_handler_word(HANDLER,0xabcdef01,&memory);
    CHECK(r32(NULL,HANDLER+0xc)==0xabcdef01&&r32(NULL,HANDLER+8)==1&&r32(NULL,HANDLER+0x10)==2);px_battle_clear_handler_word(HANDLER,&memory);
    CHECK(r32(NULL,HANDLER+0xc)==0&&r32(NULL,HANDLER+8)==1&&r32(NULL,HANDLER+0x10)==2);++cases;
    /* Matchup keys. */
    {
        reset();frame(ATT,DEF);px_battle_thunder_wave_check(HANDLER,CTX,ATT,WORK,&api);CHECK(key(0x53)==1&&untouched(0x53));++cases;
        reset();frame(ATT,DEF);px_battle_thousand_arrows_check(HANDLER,CTX,ATT,WORK,&api);CHECK(key(0x53)==1&&untouched(0x53));++cases;
        reset();frame(ATT,DEF);px_battle_flying_press(HANDLER,CTX,ATT,WORK,&api);CHECK(key(0x17)==2&&untouched(0x17));++cases;
        reset();frame(ATT,DEF);px_battle_thousand_arrows_airborne(HANDLER,CTX,ATT,WORK,&api);CHECK(key(0x42)==1&&untouched(0x42));++cases;
        reset();frame(ATT,DEF);px_battle_chip_away_matchup(HANDLER,CTX,ATT,WORK,&api);CHECK(key(0x4c)==1&&untouched(0x4c));++cases;
        for(uint32_t t=0;t<20;++t) {
            reset();def_type=t==19?0x10a:t;att_type=10;frame(ATT,DEF);px_battle_freeze_dry(HANDLER,CTX,ATT,WORK,&api);
            CHECK(key(0x39)==(def_type==10?8u:0xeu)&&untouched(0x39));++cases;
            reset();att_type=t==19?0x104:t;def_type=4;frame(ATT,DEF);px_battle_thousand_arrows_immunity(HANDLER,CTX,ATT,WORK,&api);
            CHECK(key(0x39)==(att_type==4?7u:0xeu)&&untouched(0x39));++cases;
        }
        def_type=att_type=0;
        /* A value already rewritten in the frame stays. */
        reset();frame(ATT,DEF);px_battle_event_rewrite(0x42,5,&events);px_battle_thousand_arrows_airborne(HANDLER,CTX,ATT,WORK,&api);CHECK(key(0x42)==5);++cases;
    }
    /* Through the dispatcher on event 0x4e: Counter (no record), then Sonic Boom, then Dragon Rage for another battler.
     * Key 0x38 is scaled, so a later rewrite replaces an earlier one. */
    {
        reset();records=1;rec_data[0][5]=4;rec_data[0][2]=30;categories[0]=1;
        for(unsigned i=0;i<3;++i){ w8(NULL,ENTRIES+0x10*i,0x4e);w32(NULL,ENTRIES+0x10*i+4,i); }
        const uint8_t ids[3]={ATT,ATT,3};
        for(unsigned i=0;i<3;++i) {
            const uint32_t h=NODES+0x40*i;
            w32(NULL,h+8,ENTRIES+0x10*i);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ids[i]);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<2?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,NODES);frame(ATT,DEF);px_battle_event_dispatch(CTX,0x4e,&events);
        CHECK(key(0x38)==20&&same("dr0 al2/4 mc0 ")&&aborts==0);++cases;
    }
    printf("battle_moves_3: %u cases, all tests passed\n",cases);
    return 0;
}
