#include "battle_moves_1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, SCRATCH=0x7000, PARTY=0x7100, PM=0x7200, COUNTS=0x7800, ENTRIES=0x7900, NODES=0x8000 };
enum { ATT=2, DEF=5, A=MON+0x200, D=MON+0x500 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static unsigned aborts;static char log_[4096];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[64];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
/* Battler slots: MON + 0x100 i -> i, party members PM + 0x40 j -> 8 + j. */
static uint32_t values[16][0x20],statuses[16],able[16];
static unsigned slot(uint32_t p)
{
    if(p>=MON&&p<MON+0x800&&!(p&0xff)) return (p-MON)>>8;
    CHECK(p>=PM&&p<PM+0x200&&!(p&0x3f));return 8+((p-PM)>>6);
}
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;CHECK(k<0x20);notef("v%u.%x ",slot(p),k);return values[slot(p)][k]; }
static uint32_t pokemon_status(void *u,uint32_t p) { (void)u;notef("st%u ",slot(p),0);return statuses[slot(p)]; }
static const struct px_battle_ability_api base={&events,pokemon_value,NULL,pokemon_status,NULL,NULL,NULL};
static const struct px_battle_handler_api handlers={&memory,NULL,abort_battle,NULL,NULL,NULL,BATTLE,POOL,0,0,0,0,COUNTS};
static uint32_t cond,friend_,records,record_calls,record_limit,rec_index[400],speeds[16],weights[0x100],slot_value,pp_value,turn_value,
                used_mask,users,percent,fixed,rnd,last,streak,stock,item_factor;
static uint8_t rec_data[8][8];
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;notef("hs%u/%u ",slot(p),s);return cond; }
static uint32_t turn_flag(void *u,uint32_t p,uint32_t f) { (void)u;notef("tf%u/%u ",slot(p),f);return cond; }
static uint32_t friendship(void *u,uint32_t core) { (void)u;notef("fr%x ",core,0);return friend_; }
static uint32_t damage_record(void *u,uint32_t p,uint32_t k,uint32_t i,uint32_t out)
{
    (void)u;CHECK(p==A&&k==0&&out==SCRATCH&&i<0x100);
    if(record_limit) { CHECK(record_calls<400);rec_index[record_calls++]=i;return record_calls<=record_limit; }
    notef("dr%u ",i,0);if(i>=records) return 0;
    for(unsigned b=0;b<8;++b) w8(NULL,out+b,rec_data[i][b]);
    return 1;
}
static uint32_t speed(void *u,uint32_t c,uint32_t p,uint32_t f) { (void)u;CHECK(c==CTX&&f==0);notef("sp%u ",slot(p),0);return speeds[slot(p)]; }
static uint32_t weight(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX&&id<0x100);notef("wt%x ",id,0);return weights[id]; }
static uint32_t move_slot(void *u,uint32_t p,uint32_t m) { (void)u;notef("ms%u/%u ",slot(p),m);return slot_value; }
static uint32_t move_pp(void *u,uint32_t p,uint32_t s) { (void)u;notef("pp%u/%u ",slot(p),s);return pp_value; }
static uint32_t turn(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("turn ");return turn_value; }
static uint32_t move_used(void *u,uint32_t h,uint32_t m,uint32_t t) { (void)u;CHECK(h==CTX+0x20);notef("mu%u/%u ",m,t);return t<32&&(used_mask>>t&1); }
static uint32_t move_users(void *u,uint32_t h,uint32_t m,uint32_t t) { (void)u;CHECK(h==CTX+0x20);notef("mn%u/%u ",m,t);return users; }
static uint32_t target_switching(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("sw ");return cond; }
static uint32_t hp_percent(void *u,uint32_t p) { (void)u;notef("hp%u ",slot(p),0);return percent; }
static uint32_t fixed_multiply(void *u,uint32_t v,uint32_t f) { (void)u;notef("fx%u/%u ",v,f);return fixed; }
static uint32_t random_(void *u,uint32_t r) { (void)u;notef("rnd%u ",r,0);return rnd; }
static uint32_t last_move(void *u,uint32_t p) { (void)u;notef("lm%u ",slot(p),0);return last; }
static uint32_t use_streak(void *u,uint32_t p) { (void)u;notef("us%u ",slot(p),0);return streak; }
static uint32_t stockpile(void *u,uint32_t p,uint32_t i) { (void)u;notef("sk%u/%u ",slot(p),i);return stock; }
static uint32_t weather(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("w ");return cond; }
static uint32_t party(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("pa%u ",id,0);return PARTY; }
static uint32_t zero_member;   /* index whose member lookup returns 0 (0xff none) */
static uint32_t party_member(void *u,uint32_t p,uint32_t i) { (void)u;CHECK(p==PARTY&&i<0x100);notef("pm%u ",i,0);
    return i==zero_member?0:i<mem[PARTY+0x18]?PM+0x40*i:0; }
static uint32_t can_battle(void *u,uint32_t p) { (void)u;notef("cb%u ",slot(p),0);return able[slot(p)]; }
static uint32_t item_param(void *u,uint32_t item,uint32_t p) { (void)u;notef("ip%u/%u ",item,p);return item*item_factor+p; }
static const struct px_battle_moves_1_api api={&base,&handlers,has_status,turn_flag,friendship,damage_record,speed,weight,move_slot,move_pp,
    turn,move_used,move_users,target_switching,hp_percent,fixed_multiply,random_,last_move,use_streak,stockpile,weather,party,
    party_member,can_battle,item_param,SCRATCH};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_1_api*);
static const fn dispatched[]={px_battle_rollout,px_battle_triple_kick,px_battle_hex};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;CHECK(f<3);dispatched[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;log_[0]=0;memset(values,0,sizeof values);memset(statuses,0,sizeof statuses);memset(able,0,sizeof able);
    memset(speeds,0,sizeof speeds);memset(weights,0,sizeof weights);memset(rec_data,0,sizeof rec_data);
    cond=friend_=records=record_calls=record_limit=slot_value=pp_value=turn_value=used_mask=users=percent=fixed=rnd=last=streak=stock=0;
    item_factor=3;zero_member=0xff;
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
    for(unsigned i=0;i<PX_BATTLE_HANDLER_POOL;++i)w32(NULL,POOL+4*i,NODES+0x40*i);
}
/* A frame: attacker key 0x03, defender key 0x04, power key 0x31. */
static void frame(uint32_t attacker,uint32_t def,uint32_t power)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x03,attacker,&events);px_battle_event_set_id(0x04,def,&events);
    px_battle_event_set_value(0x31,power,&events);log_[0]=0;
}
static uint32_t power(void) { return px_battle_event_get(0x31,&events); }
static uint32_t run(fn f,uint32_t p) { log_[0]=0;frame(ATT,DEF,p);log_[0]=0;f(HANDLER,CTX,ATT,WORK,&api);CHECK(aborts==0);return power(); }
static int same(const char *expected) { if(strcmp(log_,expected)){ fprintf(stderr,"log [%s] expected [%s]\n",log_,expected);return 0; } return 1; }
int main(void)
{
    unsigned cases=0;
    const fn all[]={px_battle_smelling_salts,px_battle_wake_up_slap,px_battle_hex,px_battle_assurance,px_battle_payback,px_battle_gust,
        px_battle_acrobatics,px_battle_pursuit,px_battle_weather_ball,px_battle_revenge,px_battle_round,px_battle_return,px_battle_frustration,
        px_battle_punishment,px_battle_stored_power,px_battle_flail,px_battle_wring_out,px_battle_gyro_ball,px_battle_electro_ball,
        px_battle_low_kick,px_battle_heavy_slam,px_battle_trump_card,px_battle_spit_up,px_battle_natural_gift,px_battle_fling,
        px_battle_beat_up,px_battle_echoed_voice,px_battle_fury_cutter,px_battle_rollout,px_battle_triple_kick,px_battle_magnitude,
        px_battle_present,px_battle_pledge};
    /* Nothing happens unless key 0x03 is the handler's battler (also for an id differing above the low byte). */
    for(unsigned k=0;k<sizeof all/sizeof *all;++k)for(uint32_t other=0;other<3;++other) {
        reset();cond=1;w32(NULL,WORK,1);w32(NULL,WORK+4,9);
        const uint32_t key3=other==0?3:other==1?0x102:ATT;const uint32_t id=other==2?0x102:ATT;
        frame(key3,DEF,77);all[k](HANDLER,CTX,id,WORK,&api);
        CHECK(log_[0]==0&&power()==77&&r32(NULL,WORK)==1&&r32(NULL,WORK+4)==9&&aborts==0);++cases;
    }
    /* Doubled power on a defender's condition. */
    {
        const fn f[]={px_battle_smelling_salts,px_battle_wake_up_slap,px_battle_assurance,px_battle_payback,px_battle_pursuit,px_battle_weather_ball};
        const char *calls[]={"hs5/1 ","hs5/2 ","tf5/2 ","tf5/1 ","sw ","w "};
        const uint32_t conds[]={0,1,2,0x100,0x80000000};
        for(unsigned k=0;k<6;++k)for(unsigned c=0;c<5;++c)for(uint32_t p=0;p<2;++p) {
            reset();cond=conds[c];const uint32_t base_power=p?0x80000001:60;
            CHECK(run(f[k],base_power)==(cond?base_power<<1:base_power)&&same(calls[k]));++cases;
        }
        for(unsigned c=0;c<5;++c){ reset();statuses[DEF]=conds[c];statuses[ATT]=!conds[c];CHECK(run(px_battle_hex,50)==(conds[c]?100u:50u)&&same("st5 "));++cases; }
        /* The defender key is narrowed to its low byte. */
        reset();statuses[DEF]=1;frame(ATT,0x300+DEF,50);px_battle_hex(HANDLER,CTX,ATT,WORK,&api);CHECK(power()==100&&aborts==0&&same("st5 "));++cases;
        /* Gust and Twister: state bit 3 (+0x169) of the defender only. */
        for(int bit=-1;bit<24;++bit) {
            reset();if(bit>=0)mem[D+0x169+bit/8]=(uint8_t)(1u<<(bit%8));mem[A+0x169]=8;
            CHECK(run(px_battle_gust,40)==(bit==3?80u:40u)&&log_[0]==0);++cases;
        }
        /* Acrobatics: the attacker's held item (+0x12). */
        const uint16_t items[]={0,1,0x100,0xffff};
        for(unsigned i=0;i<4;++i){ reset();w16(NULL,A+0x12,items[i]);w16(NULL,D+0x12,(uint16_t)!items[i]);w16(NULL,A+0x10,0xffff);w16(NULL,A+0x14,0xffff);
            CHECK(run(px_battle_acrobatics,55)==(items[i]?55u:110u));++cases; }
    }
    /* Revenge and Avalanche: a damage record from the defender with damage. */
    {
        reset();CHECK(run(px_battle_revenge,60)==60&&same("dr0 "));++cases;
        for(uint32_t n=1;n<5;++n)for(uint32_t at=0;at<=n;++at)for(uint32_t dmg=0;dmg<3;++dmg) {
            reset();records=n;
            for(uint32_t i=0;i<n;++i) rec_data[i][5]=(uint8_t)(i==at?DEF:i==0?ATT:DEF+1);
            if(at<n){ const uint16_t d=dmg==0?0:dmg==1?1:0x100;rec_data[at][2]=(uint8_t)d;rec_data[at][3]=(uint8_t)(d>>8); }
            for(uint32_t i=0;i<n;++i)if(i!=at){ rec_data[i][2]=0x40; }
            const int hit=at<n&&dmg!=0;
            CHECK(run(px_battle_revenge,60)==(hit?120u:60u));
            char b[64]="";for(uint32_t i=0;i<=(hit?at:n);++i){ char t[8];snprintf(t,sizeof t,"dr%u ",i);strcat(b,t); }
            CHECK(same(b));++cases;
        }
        /* The defender key is narrowed to its low byte; the other record bytes are ignored. */
        reset();records=1;rec_data[0][5]=DEF;rec_data[0][2]=1;rec_data[0][4]=DEF;rec_data[0][6]=0xff;
        frame(ATT,0x300+DEF,60);px_battle_revenge(HANDLER,CTX,ATT,WORK,&api);CHECK(power()==120);++cases;
        reset();records=1;rec_data[0][4]=DEF;rec_data[0][6]=DEF;rec_data[0][0]=1;rec_data[0][1]=1;rec_data[0][3]=0;CHECK(run(px_battle_revenge,60)==60);++cases;
        reset();records=1;rec_data[0][5]=DEF;rec_data[0][3]=1;CHECK(run(px_battle_revenge,60)==120);++cases;   /* damage high byte */
        /* Record indexes wrap at 256. */
        reset();record_limit=300;CHECK(run(px_battle_revenge,60)==60&&record_calls==301);
        for(uint32_t i=0;i<301;++i) { CHECK(rec_index[i]==(i&0xff)); }
        ++cases;
    }
    /* Round: more than one user this turn. */
    for(uint32_t n=0;n<4;++n){ reset();users=n==3?0x80000000:n;turn_value=7;w16(NULL,HANDLER+0x38,496);w16(NULL,HANDLER+0x3a,0xffff);
        CHECK(run(px_battle_round,60)==(users>1?120u:60u)&&same("turn mn496/7 "));++cases; }
    /* Return and Frustration: friendship * 10 / 25, at least 1. */
    for(uint32_t f=0;f<0x140;++f)for(unsigned k=0;k<2;++k) {
        reset();friend_=f;w32(NULL,A,0x1234567);
        const uint32_t base_value=k?(uint32_t)(0xffu-f):f,expected=base_value*10u/25u;
        CHECK(run(k?px_battle_frustration:px_battle_return,9)==(expected?expected:1)&&same("fr1234567 "));++cases;
    }
    reset();friend_=0x199999a0;CHECK(run(px_battle_return,9)==2);++cases;   /* product wraps: 0x40 / 25 */
    reset();friend_=0x1999999a;CHECK(run(px_battle_frustration,9)==(uint32_t)((0xffu-0x1999999au)*10u)/25u);++cases;
    /* Punishment and Stored Power: raised stages. */
    {
        for(unsigned k=0;k<2;++k)for(uint32_t stat=1;stat<8;++stat)for(uint32_t stage=0;stage<13;++stage) {
            reset();for(unsigned s=1;s<8;++s){ values[DEF][s]=6;values[ATT][s]=6; }
            values[k?ATT:DEF][stat]=stage;values[k?DEF:ATT][stat]=12;
            const uint32_t raised=stage>6?stage-6:0;
            const uint32_t got=run(k?px_battle_stored_power:px_battle_punishment,70);
            CHECK(got==(k?(raised?70+20*raised:70):60+20*raised));++cases;
        }
        reset();for(unsigned s=1;s<8;++s){ values[DEF][s]=12; }CHECK(run(px_battle_punishment,70)==200);++cases;   /* capped */
        reset();for(unsigned s=1;s<8;++s){ values[DEF][s]=6; }values[DEF][1]=12;values[DEF][2]=12;CHECK(run(px_battle_punishment,70)==200);++cases;
        reset();for(unsigned s=1;s<8;++s){ values[DEF][s]=6; }values[DEF][1]=12;values[DEF][2]=11;CHECK(run(px_battle_punishment,70)==200);++cases;
        reset();for(unsigned s=1;s<8;++s){ values[DEF][s]=6; }values[DEF][1]=12;values[DEF][2]=7;CHECK(run(px_battle_punishment,70)==200);++cases;
        reset();for(unsigned s=1;s<8;++s){ values[DEF][s]=6; }values[DEF][1]=12;values[DEF][2]=6;values[DEF][7]=9;CHECK(run(px_battle_punishment,70)==200);++cases;
        reset();for(unsigned s=1;s<8;++s){ values[DEF][s]=7; }CHECK(run(px_battle_punishment,70)==200);++cases;
        reset();for(unsigned s=1;s<8;++s){ values[DEF][s]=6; }values[DEF][3]=9;values[DEF][4]=8;values[DEF][6]=7;CHECK(run(px_battle_punishment,70)==60+20*6);++cases;
        reset();for(unsigned s=1;s<8;++s){ values[ATT][s]=12; }CHECK(run(px_battle_stored_power,20)==20+20*42);++cases;
        reset();for(unsigned s=1;s<8;++s){ values[ATT][s]=s; }CHECK(run(px_battle_stored_power,20)==20+20*1);++cases;
        /* Signed tests: Punishment compares the stage with 6, Stored Power the stage minus 6 with 0. */
        reset();for(unsigned s=1;s<8;++s){ values[DEF][s]=6;values[ATT][s]=6; }values[DEF][2]=0x80000005;CHECK(run(px_battle_punishment,70)==60);++cases;
        reset();for(unsigned s=1;s<8;++s){ values[DEF][s]=6;values[ATT][s]=6; }values[DEF][2]=0x0cccccd3;CHECK(run(px_battle_punishment,70)==64);++cases;
        reset();for(unsigned s=1;s<8;++s){ values[ATT][s]=6; }values[ATT][4]=0x80000005;CHECK(run(px_battle_stored_power,20)==20u+0x7fffffffu*20u);++cases;
        reset();for(unsigned s=1;s<8;++s){ values[ATT][s]=6; }values[ATT][4]=0x80000006;CHECK(run(px_battle_stored_power,20)==20);++cases;
        reset();for(unsigned s=1;s<8;++s){ values[ATT][s]=6; }values[ATT][4]=0xffffffff;CHECK(run(px_battle_stored_power,20)==20);++cases;
        /* With none raised the power value is not rewritten, so it can still be rewritten once. */
        for(unsigned k=0;k<2;++k){ reset();for(unsigned s=1;s<8;++s){ values[ATT][s]=k?7:6; }run(px_battle_stored_power,20);
            px_battle_event_rewrite(0x31,999,&events);CHECK(power()==(k?20u+20u*7u:999u));++cases; }
        /* Stat orders (tables 108d2c3f and 108d2c4d) and calls. */
        reset();CHECK(run(px_battle_punishment,70)==60&&same("v5.1 v5.2 v5.3 v5.4 v5.5 v5.6 v5.7 "));++cases;
        reset();values[ATT][5]=8;CHECK(run(px_battle_stored_power,70)==110&&same("v2.1 v2.2 v2.5 v2.3 v2.4 v2.6 v2.7 "));++cases;
    }
    /* Flail and Reversal: 48 * HP / max HP. */
    {
        const uint32_t maxima[]={1,2,3,47,48,49,100,255,714};
        for(unsigned k=0;k<9;++k)for(uint32_t hp=0;hp<=maxima[k];++hp) {
            reset();values[ATT][0xd]=hp;values[ATT][0xe]=maxima[k];
            uint32_t r=hp*48/maxima[k];if(!r) r=1;
            const uint32_t expected=r<=1?200:r<=4?150:r<=9?100:r<=16?80:r<=32?40:20;
            CHECK(run(px_battle_flail,7)==expected);++cases;
        }
        reset();values[ATT][0xd]=100;values[ATT][0xe]=10;CHECK(run(px_battle_flail,7)==20&&same("v2.e v2.d "));++cases;   /* above the table */
        reset();values[ATT][0xd]=0xffffffff;values[ATT][0xe]=1;CHECK(run(px_battle_flail,7)==20);++cases;   /* -48: unsigned table compare */
        reset();values[ATT][0xd]=3;values[ATT][0xe]=0xfffffffe;CHECK(run(px_battle_flail,7)==20);++cases;  /* signed: 144 / -2 */
        reset();values[ATT][0xd]=1;values[ATT][0xe]=0xffffffff;CHECK(run(px_battle_flail,7)==20);++cases;   /* 48 / -1 */
    }
    /* Wring Out and Crush Grip. */
    for(uint32_t x=0;x<1000;x+=7)for(unsigned big=0;big<2;++big) {
        reset();percent=x*3+1;fixed=big?0xfffffff0-x:x;
        const uint32_t e=fixed/100;CHECK(run(px_battle_wring_out,7)==(e?e:1));
        char b[64];snprintf(b,sizeof b,"hp5 fx120/%u ",percent);CHECK(same(b));++cases;
    }
    /* Gyro Ball: 25 * defender / attacker + 1, at most 150. */
    for(uint32_t a=1;a<64;++a)for(uint32_t d=0;d<64;++d) {
        reset();speeds[ATT]=a;speeds[DEF]=d;const uint32_t e=25*d/a+1;
        CHECK(run(px_battle_gyro_ball,7)==(e>150?150:e));++cases;
    }
    reset();speeds[ATT]=1;speeds[DEF]=0x0a3d70a4;CHECK(run(px_battle_gyro_ball,7)==(uint32_t)(0x0a3d70a4u*25u)+1u&&same("sp2 sp5 "));++cases;  /* wraps */
    reset();speeds[ATT]=0xfffffffe;speeds[DEF]=10;CHECK(run(px_battle_gyro_ball,7)==1);++cases;  /* unsigned: 250 / 0xfffffffe */
    /* Electro Ball: attacker / defender, signed. */
    for(int32_t a=-9;a<70;++a)for(int32_t d=-6;d<20;++d) {
        if(!d) continue;
        reset();speeds[ATT]=(uint32_t)a;speeds[DEF]=(uint32_t)d;const int32_t q=a/d;
        CHECK(run(px_battle_electro_ball,7)==(q>=4?150u:q>=3?120u:q>=2?80u:q>=1?60u:40u));++cases;
    }
    reset();speeds[ATT]=10;speeds[DEF]=3;CHECK(run(px_battle_electro_ball,7)==120&&same("sp2 sp5 "));++cases;
    /* Low Kick and Grass Knot: the defender's weight, by the id at +0x19. */
    {
        const uint32_t w[]={0,99,100,249,250,499,500,999,1000,1999,2000,65535,0xffffffff},e[]={20,20,40,40,60,60,80,80,100,100,120,120,120};
        for(unsigned k=0;k<13;++k){ reset();mem[D+0x19]=0x45;weights[0x45]=w[k];weights[DEF]=0;CHECK(run(px_battle_low_kick,7)==e[k]&&same("wt45 "));++cases; }
    }
    /* Heavy Slam and Heat Crash: attacker / defender weight, signed. */
    for(int32_t a=-20;a<130;++a)for(int32_t d=-7;d<31;++d) {
        if(!d) continue;
        reset();mem[D+0x19]=0x46;weights[ATT]=(uint32_t)a;weights[0x46]=(uint32_t)d;const int32_t q=a/d;
        CHECK(run(px_battle_heavy_slam,7)==(q>=5?120u:q==4?100u:q==3?80u:q==2?60u:40u));
        CHECK(same("wt2 wt46 "));++cases;
    }
    /* Heavy Slam looks the attacker up first: an attacker that cannot be found aborts. */
    reset();w32(NULL,TABLE+0x84+4*ATT,0);mem[D+0x19]=0x46;weights[ATT]=10;weights[0x46]=1;frame(ATT,DEF,7);
    px_battle_heavy_slam(HANDLER,CTX,ATT,WORK,&api);CHECK(aborts==1&&!strncmp(log_,"ABORT ",6));++cases;
    /* Trump Card: power by remaining PP (table 108d49ac). */
    {
        const uint32_t table[]={200,80,60,50,40};
        for(uint32_t s=0;s<6;++s)for(uint32_t pp=0;pp<10;++pp) {
            reset();slot_value=s==5?0x104:s;pp_value=pp==8?0x103:pp==9?0x1ff:pp;w16(NULL,HANDLER+0x38,376);w16(NULL,HANDLER+0x3a,0xffff);
            const uint32_t q=pp_value&0xff;
            const uint32_t got=run(px_battle_trump_card,7);
            if(s==4){ CHECK(got==40&&same("ms2/376 "));++cases;continue; }
            CHECK(got==table[q>=5?4:q]);char b[48];snprintf(b,sizeof b,"ms2/376 pp2/%u ",slot_value);CHECK(same(b));++cases;
        }
    }
    /* Spit Up: 100 per stockpile, a signed 16-bit product. */
    {
        const uint32_t s[]={0,1,2,3,0xffff,0x18000,0x17fff};const uint32_t e[]={0,100,200,300,(uint32_t)-100,(uint32_t)(-32768*100),32767*100};
        for(unsigned k=0;k<7;++k){ reset();stock=s[k];CHECK(run(px_battle_spit_up,7)==e[k]&&same("sk2/0 "));++cases; }
    }
    /* Natural Gift and Fling: held item parameters 7 and 6. */
    for(uint32_t item=0;item<3;++item)for(unsigned k=0;k<2;++k) {
        reset();const uint16_t id=(uint16_t)(item==2?0xffff:149+item);w16(NULL,A+0x12,id);w16(NULL,D+0x12,1);
        CHECK(run(k?px_battle_fling:px_battle_natural_gift,7)==id*3u+(k?6u:7u));
        char b[32];snprintf(b,sizeof b,"ip%u/%u ",id,k?6:7);CHECK(same(b));++cases;
    }
    /* Beat Up: work word 0 eligible members are skipped; base Attack / 10 + 5. */
    {
        /* member kinds: 0 the user, 1 able, 2 fainted, 3 statused, 4 fainted and statused */
        for(uint32_t count=0;count<7;++count)for(uint32_t pattern=0;pattern<15625;pattern+=(count>3?1:(count>1?5:25)))for(uint32_t w=0;w<7;++w) {
            reset();mem[PARTY+0x18]=(uint8_t)count;w32(NULL,WORK,w==6?0x105:w);values[ATT][0x14]=1000;
            uint32_t kinds[6],p=pattern;for(unsigned i=0;i<6;++i){ kinds[i]=p%5;p/=5; }
            for(unsigned i=0;i<6;++i){ mem[PM+0x40*i+0x19]=(uint8_t)(kinds[i]==0?ATT:ATT+1+i);able[8+i]=kinds[i]!=2&&kinds[i]!=4;
                statuses[8+i]=kinds[i]>=3?(i+1)*0x10:0;values[8+i][0x14]=17+20*i; }
            uint32_t skip=r32(NULL,WORK)&0xff,chosen=0xff;
            if(count>skip)for(uint32_t i=0;i<count;++i){ const int ok=kinds[i]==0||kinds[i]==1;if(!ok) continue;if(skip){ --skip;continue; }chosen=i;break; }
            const uint32_t next=r32(NULL,WORK)+1;
            const uint32_t got=run(px_battle_beat_up,7);
            CHECK(got==(chosen==0xff?1000u/10u+5u:(17u+20u*chosen)/10u+5u));
            CHECK(r32(NULL,WORK)==(next>=6?0:next));++cases;
        }
        /* Calls: the user is taken without checks; others are checked for battling, then status. */
        reset();mem[PARTY+0x18]=3;w32(NULL,WORK,1);mem[PM+0x19]=ATT;mem[PM+0x59]=7;mem[PM+0x99]=8;able[9]=1;statuses[9]=1;able[10]=1;values[10][0x14]=99;
        CHECK(run(px_battle_beat_up,7)==14&&same("pa2 pm0 pm1 cb9 st9 pm2 cb10 st10 v10.14 ")&&r32(NULL,WORK)==2);++cases;
        reset();mem[PARTY+0x18]=2;w32(NULL,WORK,5);values[ATT][0x14]=40;
        CHECK(run(px_battle_beat_up,7)==9&&same("pa2 v2.14 ")&&r32(NULL,WORK)==0);++cases;   /* count <= skip: the user */
        /* A member lookup returning 0 that reads as the user: the user's battler is used. */
        reset();mem[PARTY+0x18]=3;zero_member=1;mem[0x19]=ATT;mem[PM+0x19]=9;values[ATT][0x14]=60;values[8][0x14]=200;w32(NULL,WORK,0);
        CHECK(run(px_battle_beat_up,7)==11&&same("pa2 pm0 cb8 pm1 v2.14 "));++cases;
        reset();mem[PARTY+0x18]=3;zero_member=1;mem[0x19]=ATT;mem[PM+0x19]=9;able[8]=1;values[ATT][0x14]=60;values[8][0x14]=200;w32(NULL,WORK,0);
        CHECK(run(px_battle_beat_up,7)==25&&same("pa2 pm0 cb8 st8 v8.14 "));++cases;
        reset();mem[PARTY+0x18]=3;zero_member=1;mem[0x19]=ATT;mem[PM+0x99]=ATT;values[ATT][0x14]=60;values[10][0x14]=300;w32(NULL,WORK,1);
        CHECK(run(px_battle_beat_up,7)==35&&same("pa2 pm0 cb8 pm1 pm2 v10.14 "));++cases;   /* the 0 member counts as skipped */
    }
    /* Echoed Voice: consecutive earlier turns using the move. */
    for(uint32_t t=0;t<8;++t)for(uint32_t mask=0;mask<256;++mask) {
        reset();turn_value=t;used_mask=mask;w16(NULL,HANDLER+0x38,497);w16(NULL,HANDLER+0x3a,0xffff);
        uint32_t n=0;for(int32_t i=(int32_t)t-1;i>=0&&(mask>>i&1);--i) ++n;
        CHECK(run(px_battle_echoed_voice,7)==(n==0?40u:n==1?80u:n==2?120u:n==3?160u:200u));++cases;
    }
    reset();turn_value=3;used_mask=6;w16(NULL,HANDLER+0x38,497);CHECK(run(px_battle_echoed_voice,7)==120&&same("turn mu497/2 mu497/1 mu497/0 "));++cases;
    reset();turn_value=0x80000001;used_mask=~0u;CHECK(run(px_battle_echoed_voice,7)==40&&same("turn "));++cases;   /* negative turn - 1 */
    reset();turn_value=0;used_mask=~0u;CHECK(run(px_battle_echoed_voice,7)==40&&same("turn "));++cases;
    /* Fury Cutter: doubled per consecutive use, at most 160, only after the same move. */
    {
        const uint32_t bases[]={10,40,41,80,81,160,161,0x80000000,0};
        for(unsigned b=0;b<9;++b)for(uint32_t n=0;n<7;++n)for(uint32_t same_move=0;same_move<3;++same_move) {
            reset();streak=n;w16(NULL,HANDLER+0x38,210);w16(NULL,HANDLER+0x3a,0xffff);last=same_move==0?210:same_move==1?211:0x100d2;
            uint32_t e=bases[b];
            if(same_move==0)for(uint32_t i=0;i<n;++i){ e<<=1;if(e>160){ e=160;break; } }
            CHECK(run(px_battle_fury_cutter,bases[b])==e);
            CHECK(same(same_move==0?"lm2 us2 ":"lm2 "));++cases;
        }
    }
    /* Rollout and Ice Ball. */
    {
        for(uint32_t hits=0;hits<40;++hits)for(uint32_t curl=0;curl<3;++curl) {
            reset();w32(NULL,WORK,hits);w32(NULL,WORK+4,0x7fffffff);mem[A+0x169]=(uint8_t)(curl==1?0x80:curl==2?0x7f:0);mem[D+0x169]=0x80;
            uint32_t factor=1;for(uint32_t i=0;i<hits;++i) factor<<=1;if(curl==1) factor<<=1;
            CHECK(run(px_battle_rollout,30)==30u*factor&&r32(NULL,WORK)==hits+1);++cases;
        }
        /* Removal of the move's handlers of this battler once the count reaches the limit. */
        const uint32_t limits[]={0,4,5,6,0xffffffff,0x80000000};
        for(unsigned l=0;l<6;++l) {
            reset();
            uint32_t h[6];const uint32_t owners[6]={0,0,0,4,0,0},vals[6]={ATT,ATT,3,ATT,ATT,ATT},ids[6]={205,33,205,205,205,0x1cd};
            for(unsigned i=0;i<6;++i) h[i]=px_battle_handler_register(owners[i],ids[i],1,i,vals[i],ENTRIES,1,&handlers);
            w32(NULL,WORK,4);w32(NULL,WORK+4,limits[l]);w16(NULL,HANDLER+0x38,205);
            CHECK(run(px_battle_rollout,30)==480&&r32(NULL,WORK)==5);
            const int removed=(int32_t)limits[l]<=5;
            for(unsigned i=0;i<6;++i){ const int gone=removed&&(i==0||i==4);CHECK((r32(NULL,h[i]+0x18)==0)==gone); }
            CHECK(r32(NULL,BATTLE+0x64)==(removed?4u:6u));++cases;
        }
    }
    /* Triple Kick, Magnitude, Present and the pledges. */
    for(uint32_t w=0;w<6;++w){ reset();w32(NULL,WORK,w);CHECK(run(px_battle_triple_kick,7)==10*(w+1)&&r32(NULL,WORK)==w+1);++cases; }
    reset();w32(NULL,WORK,0x1999999a);CHECK(run(px_battle_triple_kick,7)==0x1999999bu*10u);++cases;
    {
        const uint32_t w[]={0,10,30,150,0x80000000};
        for(unsigned k=0;k<5;++k){ reset();w32(NULL,WORK,w[k]);CHECK(run(px_battle_magnitude,7)==(w[k]?w[k]:10)&&r32(NULL,WORK)==(w[k]?w[k]:10));++cases; }
        for(uint32_t r=0;r<82;++r){ reset();rnd=r==81?0xffffffff:r;CHECK(run(px_battle_present,7)==(rnd<40?40u:rnd<70?80u:120u)&&same("rnd80 "));++cases; }
        for(unsigned k=0;k<5;++k){ reset();w32(NULL,WORK,w[k]);CHECK(run(px_battle_pledge,7)==(w[k]?150u:7u)&&log_[0]==0);++cases; }
    }
    /* Through the dispatcher: Rollout reaching its limit while running (removal deferred),
     * Triple Kick for the same battler and Hex for another on event 0x3c. */
    {
        reset();
        for(unsigned i=0;i<3;++i){ w8(NULL,ENTRIES+0x10*i,0x3c);w32(NULL,ENTRIES+0x10*i+4,i); }
        const uint32_t roll=px_battle_handler_register(0,205,1,3,ATT,ENTRIES,1,&handlers);
        const uint32_t kick=px_battle_handler_register(0,167,1,2,ATT,ENTRIES+0x10,1,&handlers);
        const uint32_t hex=px_battle_handler_register(0,506,1,1,3,ENTRIES+0x20,1,&handlers);
        w32(NULL,roll+0x1c,2);w32(NULL,roll+0x20,3);statuses[DEF]=1;
        frame(ATT,DEF,30);px_battle_event_dispatch(CTX,0x3c,&events);
        /* Rollout rewrites first (30 * 4); the power value is then fixed, so Triple Kick's rewrite does not take.
         * Rollout's removal waits until it has run, then its node returns to the pool (cleared). */
        CHECK(power()==120&&r32(NULL,kick+0x1c)==1&&aborts==0&&log_[0]==0);
        CHECK(r32(NULL,BATTLE+0x64)==2&&r32(NULL,hex+0x18)!=0&&r32(NULL,roll+0x18)==0&&r32(NULL,roll+0x1c)==0);
        CHECK(r32(NULL,POOL+4*2)==roll&&r32(NULL,BATTLE+0x60)==kick);++cases;
        /* The same with a limit not reached: Rollout stays and counts. */
        reset();
        for(unsigned i=0;i<3;++i){ w8(NULL,ENTRIES+0x10*i,0x3c);w32(NULL,ENTRIES+0x10*i+4,i); }
        const uint32_t roll2=px_battle_handler_register(0,205,1,3,ATT,ENTRIES,1,&handlers);
        w32(NULL,roll2+0x1c,2);w32(NULL,roll2+0x20,4);
        frame(ATT,DEF,30);px_battle_event_dispatch(CTX,0x3c,&events);
        CHECK(power()==120&&r32(NULL,roll2+0x1c)==3&&r32(NULL,roll2+0x18)==0x20010000u&&r32(NULL,BATTLE+0x64)==1&&aborts==0);++cases;
    }
    printf("battle_moves_1: %u cases, all tests passed\n",cases);
    return 0;
}
