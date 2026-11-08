#include "battle_moves_10.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, SCRATCH=0x7800, ENTRIES=0x7900, NODES=0x8000 };
enum { ATT=2, A=MON+0x200 };
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
static uint32_t values[9][0x20];static uint64_t status_mask[9];
static unsigned slot(uint32_t p) { if(!p) return 8;   /* an aborted lookup */
    CHECK(p>=MON&&p<MON+0x800&&!(p&0xff));return (p-MON)>>8; }
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;CHECK(k<0x20);notef("v%u.%x ",slot(p),k);return values[slot(p)][k]; }
static uint32_t categories[0x400];
static uint32_t move_category(void *u,uint32_t m) { (void)u;CHECK(m<0x400);notef("mc%u ",m,0);return categories[m]; }
static const struct px_battle_ability_api base={&events,pokemon_value,move_category,NULL,NULL,NULL,NULL};
static uint32_t records,record_limit,record_calls,rec_index[400],ally_mask,moves[8],used_mask,counters[8],rnd,last,full,forbid,format,allowed,
                fairy,present,typed_v,target_class,flag_v,param6,param7,usable,locked,records_made;
static uint8_t rec_data[8][8];
static uint32_t damage_record(void *u,uint32_t p,uint32_t k,uint32_t i,uint32_t out)
{
    (void)u;CHECK(p==A&&k==0&&out==SCRATCH&&i<0x100);
    if(record_limit) { CHECK(record_calls<400);rec_index[record_calls++]=i;for(unsigned b=0;b<8;++b) w8(NULL,out+b,0);w8(NULL,out+5,9);return record_calls<=record_limit; }
    notef("dr%u ",i,0);if(i>=records) return 0;
    for(unsigned b=0;b<8;++b) w8(NULL,out+b,rec_data[i][b]);
    return 1;
}
static uint32_t is_ally(void *u,uint32_t id,uint32_t o) { (void)u;notef("al%u/%u ",id,o);return o<32&&(ally_mask>>o&1); }
static uint32_t move_at(void *u,uint32_t p,uint32_t s) { (void)u;CHECK(s<8);notef("ma%u/%u ",slot(p),s);return moves[s]; }
static uint32_t move_used(void *u,uint32_t p,uint32_t s) { (void)u;CHECK(s<8);notef("mu%u/%u ",slot(p),s);return used_mask>>s&1; }
static uint32_t counter(void *u,uint32_t p,uint32_t i) { (void)u;CHECK(i<8);notef("cnt%u/%u ",slot(p),i);return counters[i]; }
static uint32_t random_(void *u,uint32_t r) { (void)u;notef("rnd%u ",r,0);return rnd; }
static uint32_t moves_last(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("last%u ",id,0);return last; }
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;CHECK(s<64);notef("hs%u/%x ",slot(p),s);return (uint32_t)(status_mask[slot(p)]>>s&1); }
static uint32_t full_hp(void *u,uint32_t p) { (void)u;notef("full%u ",slot(p),0);return full; }
static uint32_t escape_forbidden(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("forbid ");return forbid; }
static uint32_t battle_format(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("fmt ");return format; }
static uint32_t escape_allowed(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("esc%u ",id,0);return allowed; }
static uint32_t field_active(void *u,uint32_t c,uint32_t k) { (void)u;CHECK(c==CTX);notef("fa%u ",k,0);return fairy; }
static uint32_t slot_present(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("sp%u ",id,0);return present; }
static uint32_t typed(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("ty%u ",id,0);return typed_v; }
static uint32_t move_target(void *u,uint32_t m) { (void)u;notef("mt%u ",m,0);return target_class; }
static uint32_t move_flag(void *u,uint32_t m,uint32_t f) { (void)u;notef("mf%u/%u ",m,f);return flag_v; }
static void bounce(void *u,uint32_t c,uint32_t id,uint32_t o) { (void)u;CHECK(c==CTX);notef("bounce%u/%u ",id,o); }
static uint32_t item_param(void *u,uint32_t item,uint32_t p) { (void)u;notef("ip%u/%u ",item,p);return p==6?param6:param7; }
static uint32_t item_usable(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("iu%u ",id,0);return usable; }
static uint32_t item_locked(void *u,uint32_t c,uint32_t sp,uint32_t item) { (void)u;CHECK(c==CTX);notef("il%u/%u ",sp,item);return locked; }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);return RECORD+0x40*records_made++; }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX);notef("commit%u ",(r-RECORD)/0x40,0); }
static const struct px_battle_moves_10_api api={&base,damage_record,is_ally,move_at,move_used,counter,random_,moves_last,has_status,full_hp,
    escape_forbidden,battle_format,escape_allowed,field_active,slot_present,typed,move_target,move_flag,bounce,item_param,item_usable,item_locked,
    action_record,commit,SCRATCH};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_10_api*);
static const fn dispatched[]={px_battle_fake_out_ready,px_battle_spit_up_ready};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w) { (void)u;CHECK(f<2);dispatched[f](h,c,v,w,&api); }
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;log_[0]=0;memset(values,0,sizeof values);memset(status_mask,0,sizeof status_mask);memset(categories,0,sizeof categories);
    memset(rec_data,0,sizeof rec_data);memset(moves,0,sizeof moves);memset(counters,0,sizeof counters);
    records=record_limit=record_calls=ally_mask=used_mask=rnd=last=full=forbid=format=allowed=fairy=present=typed_v=target_class=flag_v=0;
    param6=param7=usable=locked=records_made=0;
    for(unsigned i=0;i<4;++i){ memset(mem+RECORD+0x40*i,0xa5,0x40); }
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i){ w32(NULL,TABLE+0x84+4*i,MON+0x100*i);mem[MON+0x100*i+0x19]=(uint8_t)i; }
}
/* Keys: 0x02 battler (id), 0x12 move, 0x23 result (values). */
static uint32_t start_result=0,move_key=0;
static void frame(uint32_t battler)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x02,battler,&events);px_battle_event_set_value(0x12,move_key,&events);
    px_battle_event_set_value(0x23,start_result,&events);log_[0]=0;
}
static uint32_t res(void) { return px_battle_event_get(0x23,&events); }
static uint32_t run_as(fn f,uint32_t battler,uint32_t id) { frame(battler);f(HANDLER,CTX,id,WORK,&api);CHECK(aborts==0);return res(); }
static uint32_t run(fn f) { return run_as(f,ATT,ATT); }
static int same(const char *expected) { if(strcmp(log_,expected)){ fprintf(stderr,"log [%s] expected [%s]\n",log_,expected);return 0; } return 1; }
int main(void)
{
    unsigned cases=0;
    /* Handlers that test key 0x02 do nothing for another battler. */
    {
        const fn keyed[]={px_battle_counter_ready,px_battle_mirror_coat_ready,px_battle_metal_burst_ready,px_battle_last_resort_ready,px_battle_bide_ready,
            px_battle_protect_ready,px_battle_rest_ready,px_battle_spit_up_ready,px_battle_snore_ready,px_battle_sky_drop_ready,px_battle_stockpile_ready,
            px_battle_fling_ready,px_battle_magnet_rise_ready,px_battle_mat_block_ready,px_battle_natural_gift_ready,px_battle_fake_out_ready};
        for(unsigned k=0;k<sizeof keyed/sizeof *keyed;++k)for(uint32_t other=0;other<2;++other) {
            reset();w32(NULL,WORK,5);counters[4]=3;last=1;CHECK(run_as(keyed[k],other?0x102:3,ATT)==0&&log_[0]==0&&r32(NULL,WORK)==5);++cases;
        }
    }
    /* Counter, Mirror Coat, Metal Burst: fail without a fitting damage record. */
    {
        const fn f[3]={px_battle_counter_ready,px_battle_mirror_coat_ready,px_battle_metal_burst_ready};const uint32_t kind[3]={1,2,0};
        for(unsigned k=0;k<3;++k)for(uint32_t n=0;n<4;++n)for(uint32_t pattern=0;pattern<64;++pattern) {
            reset();records=n;ally_mask=1u<<3;mem[A+0x19]=ATT;
            uint32_t ok=0;
            for(uint32_t i=0;i<n;++i){ const uint32_t p=pattern>>(2*i)&3;rec_data[i][5]=(uint8_t)(p==0?3:4+i);rec_data[i][0]=(uint8_t)(100+i);
                categories[100+i]=p==1?1:p==2?2:3;if(p!=0&&(!kind[k]||categories[100+i]==kind[k])) ok=1; }
            CHECK(run(f[k])==(ok?0u:0x1bu));++cases;
        }
        reset();records=2;mem[A+0x19]=6;rec_data[0][5]=3;rec_data[1][5]=4;rec_data[1][0]=0x21;rec_data[1][1]=1;categories[0x121]=1;ally_mask=1u<<3;
        CHECK(run(px_battle_counter_ready)==0&&same("dr0 al6/3 dr1 al6/4 mc289 "));++cases;
        reset();records=1;rec_data[0][5]=4;CHECK(run(px_battle_metal_burst_ready)==0&&same("dr0 al2/4 "));++cases;   /* no damage needed */
        reset();record_limit=300;ally_mask=1u<<9;CHECK(run(px_battle_metal_burst_ready)==0x1b&&record_calls==301);
        for(uint32_t i=0;i<301;++i) { CHECK(rec_index[i]==(i&0xff)); }
        ++cases;
        reset();frame(ATT);px_battle_counter_check(CTX,ATT,0xbeef,0,&api);CHECK(res()==0x1b&&same("dr0 "));++cases;
    }
    /* Last Resort over move lists. */
    for(uint32_t n=0;n<6;++n)for(uint32_t where=0;where<=n;++where)for(uint32_t used=0;used<64;used+=(n>3?3:1)) {
        reset();mem[A+0x152]=(uint8_t)n;for(uint32_t i=0;i<n;++i) moves[i]=i==where?387:10+i;used_mask=used;
        uint32_t cnt=0;for(uint32_t i=0;i<n;++i) if(i!=where&&(used>>i&1)) ++cnt;
        const int ok=where<n&&n>=2&&cnt>=n-1;
        CHECK(run(px_battle_last_resort_ready)==(ok?0u:0x1bu));++cases;
    }
    reset();mem[A+0x152]=3;moves[0]=387;moves[1]=0x10183;used_mask=6;CHECK(run(px_battle_last_resort_ready)==0&&same("ma2/0 ma2/1 mu2/1 ma2/2 mu2/2 "));++cases;
    /* Bide. */
    {
        const uint32_t w0[]={0,1,2,3,0x80000000},w1[]={0,5},w2[]={0x1f,3,0x11f};
        for(unsigned a=0;a<5;++a)for(unsigned b=0;b<2;++b)for(unsigned c=0;c<3;++c) {
            reset();w32(NULL,WORK,w0[a]);w32(NULL,WORK+4,w1[b]);w32(NULL,WORK+8,w2[c]);
            const int on=(int32_t)w0[a]>=2;const int fail=on&&(w1[b]==0||w2[c]==0x1f);
            CHECK(run(px_battle_bide_ready)==(fail?0x1bu:0u)&&r32(NULL,WORK)==(on?3u:w0[a]));++cases;
        }
    }
    /* Protect family: streak odds (table 108d4b38) and moving last. */
    {
        const uint32_t odds[7]={1,3,9,27,81,243,729};
        for(uint32_t s=0;s<10;++s)for(uint32_t r=0;r<2;++r)for(uint32_t l=0;l<2;++l) {
            reset();counters[3]=s==9?0x100:s;rnd=r;last=l;
            const uint32_t got=run(px_battle_protect_ready);
            const int roll_fail=counters[3]!=0&&r;
            CHECK(got==(roll_fail||l?0x1bu:0u));
            char b[96];
            if(counters[3]==0) snprintf(b,sizeof b,"cnt2/3 last2 ");
            else if(r) snprintf(b,sizeof b,"cnt2/3 rnd%u ",odds[counters[3]>=7?6:counters[3]]);
            else snprintf(b,sizeof b,"cnt2/3 rnd%u last2 ",odds[counters[3]>=7?6:counters[3]]);
            CHECK(same(b));++cases;
        }
    }
    /* Rest. */
    {
        const uint32_t ab[]={0,15,72,0x1000f,0x48+0x10000};
        for(uint32_t pre=0;pre<3;++pre)for(uint32_t sl=0;sl<2;++sl)for(uint32_t hb=0;hb<2;++hb)for(uint32_t fu=0;fu<2;++fu)for(unsigned a=0;a<5;++a) {
            reset();start_result=pre==0?0:pre==1?0x100:5;status_mask[ATT]=(sl?1ull<<2:0)|(hb?1ull<<0xf:0);full=fu;values[ATT][0x11]=ab[a];
            const uint32_t got=run(px_battle_rest_ready);
            uint32_t e=start_result;
            if((start_result&0xff)==0){ if(sl) e=0x1b;else if(hb) e=0xd;else if(fu) e=0xe;else if((ab[a]&0xffff)==15||(ab[a]&0xffff)==72) e=0xf; }
            CHECK(got==e);++cases;
        }
        start_result=0;
    }
    /* Spit Up, Swallow and Stockpile. */
    for(uint32_t c=0;c<6;++c){ reset();counters[0]=c==5?0x100:c;CHECK(run(px_battle_spit_up_ready)==(counters[0]?0u:0x1bu)&&same("cnt2/0 "));++cases;
        reset();counters[0]=c==5?0x100:c;CHECK(run(px_battle_stockpile_ready)==(counters[0]>=3?0x1bu:0u)&&same("cnt2/0 "));++cases; }
    /* Snore and Sleep Talk. */
    for(uint32_t pre=0;pre<4;++pre)for(uint32_t sl=0;sl<2;++sl) {
        const uint32_t starts[4]={0,2,0x102,0x1b};reset();start_result=starts[pre];status_mask[ATT]=sl?1ull<<2:0;
        const uint32_t got=run(px_battle_snore_ready);
        CHECK(got==((starts[pre]&0xff)==2?0u:sl?starts[pre]:0x1bu));++cases;
    }
    start_result=0;
    /* Teleport (no key test). */
    for(uint32_t fb=0;fb<2;++fb)for(uint32_t fm=0;fm<3;++fm)for(uint32_t al=0;al<2;++al)for(uint32_t tr=0;tr<4;++tr)for(uint32_t fl=0;fl<2;++fl) {
        reset();forbid=fb;format=fm;allowed=al;status_mask[ATT]=(tr&1?1ull<<8:0)|(tr&2?1ull<<0x16:0);fairy=fl;
        const uint32_t got=run_as(px_battle_teleport_ready,3,ATT);
        const int fail=fb||fm||(!al&&(tr||fl));
        CHECK(got==(fail?0x1bu:0u));++cases;
    }
    reset();forbid=1;run(px_battle_teleport_ready);CHECK(same("forbid esc2 hs2/8 hs2/16 fa9 "));++cases;
    /* Sky Drop: the carried battler must be present. */
    for(uint32_t c=0;c<0x1b;++c)for(uint32_t p=0;p<2;++p) {
        reset();counters[4]=c==0x1a?0x105:c;present=p;const uint32_t got=run(px_battle_sky_drop_ready);
        const uint32_t t=(counters[4]-1)&0xff;const int fail=counters[4]!=0&&t<0x18&&!p;
        CHECK(got==(fail?0x1bu:0u));++cases;
    }
    reset();counters[4]=6;CHECK(run(px_battle_sky_drop_ready)==0x1b&&same("cnt2/4 sp5 "));++cases;
    /* Magic Coat: own use, and bouncing another battler's move. */
    for(uint32_t l=0;l<2;++l){ reset();last=l;CHECK(run(px_battle_magic_coat_ready)==(l?0x1bu:0u)&&same("last2 "));++cases; }
    reset();last=1;frame(0x102);px_battle_magic_coat_ready(HANDLER,CTX,ATT,WORK,&api);CHECK(res()==0x1b);++cases;   /* key 0x02 narrowed */
    for(uint32_t pattern=0;pattern<64;++pattern) {
        reset();move_key=0x10077;ally_mask=pattern&1?1u<<5:0;typed_v=pattern>>1&1;if(pattern&4) ally_mask|=1u<<ATT;
        target_class=pattern&8?0xb:0xa;flag_v=pattern>>4&1;start_result=pattern&32?7:0;
        frame(5);if(pattern&32) px_battle_event_rewrite(0x23,7,&events);log_[0]=0;
        px_battle_magic_coat_ready(HANDLER,CTX,ATT,WORK,&api);
        const int go=!(pattern&1)&&!(pattern&2)&&!(pattern&4)&&(pattern&8)&&(pattern&16);
        CHECK(res()==(go&&!(pattern&32)?0x19u:start_result));
        CHECK((strstr(log_,"bounce2/5")!=NULL)==(go&&!(pattern&32)));++cases;
    }
    move_key=0;start_result=0;
    reset();move_key=0x10077;target_class=0xb;flag_v=1;frame(5);px_battle_magic_coat_ready(HANDLER,CTX,ATT,WORK,&api);
    CHECK(same("al2/5 ty2 al5/2 mt119 mf119/4 bounce2/5 "));++cases;
    /* Guards (no key test): moving last fails and resets the streak. */
    for(uint32_t l=0;l<2;++l)for(uint32_t pre=0;pre<2;++pre) {
        reset();last=l;frame(3);if(pre) px_battle_event_rewrite(0x23,9,&events);log_[0]=0;px_battle_guard_ready(HANDLER,CTX,ATT,WORK,&api);
        if(!l){ CHECK(same("last2 ")&&res()==(pre?9u:0u));++cases;continue; }
        if(pre){ CHECK(same("last2 ")&&res()==9);++cases;continue; }
        CHECK(same("last2 rec27/2 commit0 ")&&res()==0x1b&&mem[RECORD+4]==ATT&&mem[RECORD+5]==3&&mem[RECORD+6]==0&&mem[RECORD+7]==0xa5);++cases;
    }
    /* Fling. */
    for(uint32_t it=0;it<2;++it)for(uint32_t p6=0;p6<2;++p6)for(uint32_t us=0;us<2;++us)for(uint32_t lk=0;lk<2;++lk) {
        reset();w16(NULL,A+0x12,(uint16_t)(it?0x81:0));w16(NULL,A+0xc,0x1a5);param6=p6;usable=us;locked=lk;
        const int ok=it&&p6&&us&&!lk;CHECK(run(px_battle_fling_ready)==(ok?0u:0x1bu));++cases;
    }
    reset();w16(NULL,A+0x12,0x81);w16(NULL,A+0xc,0x1a5);param6=1;usable=1;CHECK(run(px_battle_fling_ready)==0&&same("ip129/6 iu2 il421/129 "));++cases;
    /* Magnet Rise. */
    for(uint32_t s=0;s<4;++s){ reset();status_mask[ATT]=(s&1?1ull<<0x15:0)|(s&2?1ull<<0x1f:0);CHECK(run(px_battle_magnet_rise_ready)==(s?0x1bu:0u));++cases; }
    /* Mat Block and Fake Out: the first turn only (state bit 0). */
    for(int bit=-1;bit<10;++bit)for(uint32_t l=0;l<2;++l) {
        reset();if(bit>=0) mem[A+0x169+bit/8]=(uint8_t)(1u<<(bit%8));last=l;
        CHECK(run(px_battle_mat_block_ready)==(bit==0||l?0x1bu:0u));++cases;
        reset();if(bit>=0) mem[A+0x169+bit/8]=(uint8_t)(1u<<(bit%8));CHECK(run(px_battle_fake_out_ready)==(bit==0?0x1bu:0u));++cases;
    }
    reset();mem[A+0x169]=1;CHECK(run(px_battle_mat_block_ready)==0x1b&&same("last2 "));++cases;
    /* Natural Gift. */
    for(uint32_t us=0;us<2;++us)for(uint32_t it=0;it<2;++it)for(uint32_t p7=0;p7<2;++p7) {
        reset();usable=us;w16(NULL,A+0x12,(uint16_t)(it?0x99:0));param7=p7;
        CHECK(run(px_battle_natural_gift_ready)==(us&&it&&p7?0u:0x1bu));CHECK(same(us?(it?"iu2 ip153/7 ":"iu2 ip0/7 "):"iu2 "));++cases;
    }
    /* Follow Me and Rage Powder. */
    for(uint32_t fm=0;fm<5;++fm)for(uint32_t pre=0;pre<2;++pre) {
        reset();format=fm;start_result=pre?0x100:0;const uint32_t got=run(px_battle_follow_me_ready);
        CHECK(got==((fm==0||fm==3)&&!pre?0x1bu:start_result));++cases;
    }
    start_result=0;
    reset();format=1;CHECK(run(px_battle_follow_me_ready)==0&&same("fmt fmt "));++cases;
    reset();format=0;frame(3);px_battle_follow_me_ready(HANDLER,CTX,ATT,WORK,&api);CHECK(res()==0&&same("fmt "));++cases;
    /* Through the dispatcher on event 0x1f: Fake Out for battler 2 (not its first turn), Spit Up for battler 3. */
    {
        reset();mem[A+0x169]=1;
        for(unsigned i=0;i<2;++i){ w8(NULL,ENTRIES+0x10*i,0x1f);w32(NULL,ENTRIES+0x10*i+4,i); }
        const uint8_t ids[2]={ATT,3};
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=NODES+0x40*i;
            w32(NULL,h+8,ENTRIES+0x10*i);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ids[i]);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,NODES);frame(ATT);px_battle_event_dispatch(CTX,0x1f,&events);
        CHECK(res()==0x1b&&log_[0]==0&&aborts==0);++cases;
    }
    printf("battle_moves_10: %u cases, all tests passed\n",cases);
    return 0;
}
