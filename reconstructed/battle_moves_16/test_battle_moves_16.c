#include "battle_moves_16.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, SCRATCH=0x7800, ENTRIES=0x7900, COUNTS=0x7a00, PARTY=0x7b00, NODES=0x8000 };
enum { ATT=2 };
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
static void notef(const char *f,uint32_t a,uint32_t b,uint32_t c) { char t[64];snprintf(t,sizeof t,f,a,b,c);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned slot(uint32_t p) { CHECK(p>=MON&&p<MON+0x800&&!(p&0xff));return (p-MON)>>8; }
/* Mock state. */
static uint32_t records,chance_v,full,weather_v,nohp,ally_mask,hits,hit_target[4],hit_move[4],hit_sel[4],selector_v[32],target_v[32],chosen,
    pp,blocked_v,slot_v,item_p,mode,carried,rnd,used[4],party_n,status_v[8],battle_ok[8],pos,sideb_n,sideb[8],has_status_v,has_type_v,ability_v,
    held,switch_v,redirect,cat_v;
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;notef("val%u/%x ",slot(p),k,0);return ability_v; }
static uint32_t move_category(void *u,uint32_t m) { (void)u;notef("cat%x ",m,0,0);return m==0x111?1u:m==0x222?2u:cat_v; }
static uint32_t pokemon_status(void *u,uint32_t p) { (void)u;return status_v[(p-0x9000)>>8]; }
static uint32_t side_of(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("side%u ",id,0,0);return 0x20+id; }
static uint32_t side_battlers(void *u,uint32_t c,uint32_t s,uint32_t out) { (void)u;CHECK(c==CTX);notef("sb%x ",s,0,0);
    for(uint32_t i=0;i<sideb_n;++i) { w8(NULL,out+i,(uint8_t)sideb[i]); }
    return sideb_n; }
static const struct px_battle_ability_api base={&events,pokemon_value,move_category,pokemon_status,side_of,side_battlers,NULL};
static const struct px_battle_handler_api handlers={&memory,NULL,abort_battle,NULL,NULL,NULL,BATTLE,POOL,0,0,0,0,COUNTS};
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX&&records<12);notef("rec%x/%u ",k,id,0);return RECORD+0x40*records++; }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX);notef("commit%u ",(r-RECORD)/0x40,0,0); }
static void message(void *u,uint32_t at,uint32_t k,uint32_t m) { (void)u;notef("msg%x/%x ",m,(at-RECORD)*16+k,0); }
static void message_arg(void *u,uint32_t at,uint32_t v) { (void)u;notef("arg%x/%x ",v,at-RECORD,0); }
static uint32_t chance(void *u,uint32_t pc) { (void)u;notef("chance%u ",pc,0,0);return chance_v; }
static void effect_flag(void *u,uint32_t c,uint32_t v) { (void)u;CHECK(c==CTX);notef("flag%u ",v,0,0); }
static uint32_t full_hp(void *u,uint32_t p) { (void)u;notef("full%u ",slot(p),0,0);return full; }
static uint32_t hp_fraction(void *u,uint32_t p,uint32_t d) { (void)u;notef("hp%u/%u ",slot(p),d,0);return 0x12345; }
static uint32_t weather(void *u,uint32_t c) { (void)u;CHECK(c==CTX);return weather_v; }
static uint32_t no_hp(void *u,uint32_t p) { (void)u;notef("nohp%u ",slot(p),0,0);return nohp; }
static uint32_t is_ally(void *u,uint32_t id,uint32_t other) { (void)u;notef("ally%u/%u ",id,other,0);return ally_mask>>other&1; }
static uint32_t damage_record(void *u,uint32_t p,uint32_t kind,uint32_t index,uint32_t out) { (void)u;(void)p;notef("dr%u/%u ",kind,index,0);
    if(kind==0) { if(index>=hits) return 0;w16(NULL,out,(uint16_t)hit_move[index]);w8(NULL,out+5,(uint8_t)hit_target[index]);w8(NULL,out+6,(uint8_t)hit_sel[index]);return 1; }
    w8(NULL,out+5,(uint8_t)hit_target[kind]);return 1; }
static uint32_t opposing_selector(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX&&id<32);notef("os%u ",id,0,0);return selector_v[id]; }
static uint32_t target_for(void *u,uint32_t c,uint32_t s) { (void)u;CHECK(c==CTX&&s<32);notef("tf%u ",s,0,0);return target_v[s]; }
static uint32_t choose_target(void *u,uint32_t c,uint32_t id,uint32_t m) { (void)u;CHECK(c==CTX);notef("ct%u/%u ",id,m,0);return chosen; }
static uint32_t move_slot(void *u,uint32_t p,uint32_t m) { (void)u;notef("ms%u/%x ",slot(p),m,0);return slot_v; }
static uint32_t move_pp(void *u,uint32_t p,uint32_t s) { (void)u;notef("pp%u/%u ",slot(p),s,0);return pp; }
static uint32_t disable_blocked(void *u,uint32_t m) { (void)u;notef("db%x ",m,0,0);return blocked_v; }
static uint32_t item_param(void *u,uint32_t i,uint32_t p) { (void)u;notef("ip%x/%u ",i,p,0);return item_p; }
static uint32_t counter(void *u,uint32_t p,uint32_t i) { (void)u;notef("cnt%u/%u ",slot(p),i,0);return carried; }
static uint32_t battle_mode(void *u,uint32_t c) { (void)u;CHECK(c==CTX);return mode; }
static uint32_t random_(void *u,uint32_t n) { (void)u;notef("rnd%u ",n,0,0);return rnd; }
static uint32_t slot_used(void *u,uint32_t p,uint32_t i) { (void)u;(void)p;notef("su%u ",i,0,0);return used[i]; }
static uint32_t party(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("party%u ",id,0,0);return PARTY; }
static uint32_t party_member(void *u,uint32_t pa,uint32_t i) { (void)u;CHECK(pa==PARTY&&i<6);return 0x9000+0x100*i; }
static uint32_t can_battle(void *u,uint32_t p) { (void)u;return battle_ok[(p-0x9000)>>8]; }
static uint32_t position(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("pos%u ",id,0,0);return pos; }
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;notef("st%u/%x ",slot(p),s,0);return has_status_v; }
static uint32_t has_type(void *u,uint32_t p,uint32_t t) { (void)u;notef("ty%u/%x ",slot(p),t,0);return has_type_v; }
static uint32_t held_item(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("held%u ",id,0,0);return held; }
static uint32_t switch_pending(void *u,uint32_t c) { (void)u;CHECK(c==CTX);return switch_v; }
static uint32_t redirect_blocked(void *u,uint32_t c,uint32_t a,uint32_t id,uint32_t m) { (void)u;CHECK(c==CTX);notef("rb%u/%u/%x ",a,id,m);return redirect; }
static const struct px_battle_moves_16_api api={&base,&handlers,action_record,commit,message,message_arg,chance,effect_flag,full_hp,hp_fraction,
    weather,no_hp,is_ally,damage_record,opposing_selector,target_for,choose_target,move_slot,move_pp,disable_blocked,item_param,counter,
    battle_mode,random_,slot_used,party,party_member,can_battle,position,has_status,has_type,held_item,switch_pending,redirect_blocked,SCRATCH};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_moves_16_api*);
static const fn dispatched[]={px_battle_fixed_flag,px_battle_support_ready};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w) { (void)u;CHECK(f<2);dispatched[f](h,c,v,w,&api); }
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;log_[0]=0;
    records=chance_v=full=weather_v=nohp=ally_mask=hits=chosen=pp=blocked_v=slot_v=item_p=mode=carried=rnd=party_n=pos=sideb_n=has_status_v=
        has_type_v=ability_v=held=switch_v=redirect=cat_v=0;
    memset(hit_target,0,sizeof hit_target);memset(hit_move,0,sizeof hit_move);memset(hit_sel,0,sizeof hit_sel);memset(selector_v,0,sizeof selector_v);
    memset(target_v,0,sizeof target_v);memset(used,0,sizeof used);memset(status_v,0,sizeof status_v);memset(battle_ok,0,sizeof battle_ok);memset(sideb,0,sizeof sideb);
    for(unsigned i=0;i<12;++i){ memset(mem+RECORD+0x40*i,0xa5,0x40); }
    memset(mem+SCRATCH,0xcc,0x10);
    w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i){ w32(NULL,TABLE+0x84+4*i,MON+0x100*i);w8(NULL,MON+0x100*i+0x19,(uint8_t)(0x10+i)); }
    for(unsigned i=0;i<PX_BATTLE_HANDLER_POOL;++i)w32(NULL,POOL+4*i,NODES+0x40*i);
    w8(NULL,PARTY+0x18,0);
}
/* Keys: 0x03 attacker, 0x04 defender, 0x02 battler (ids); 0x12, 0x14, 0x2b, 0x2c, 0x33, 0x43, 0x47, 0x48, 0x53 values. */
static uint32_t move_v=0x99,used_v=0x321,flag_v=0,flag2_v=0,bonus_v=0;
static void frame(uint32_t attacker,uint32_t defender)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x03,attacker,&events);px_battle_event_set_id(0x04,defender,&events);
    px_battle_event_set_id(0x02,attacker,&events);
    px_battle_event_set_value(0x12,move_v,&events);px_battle_event_set_value(0x14,used_v,&events);px_battle_event_set_value(0x47,flag_v,&events);
    px_battle_event_set_value(0x48,flag2_v,&events);px_battle_event_set_value(0x33,bonus_v,&events);px_battle_event_set_value(0x53,0,&events);
    px_battle_event_set_value(0x2b,0,&events);px_battle_event_set_value(0x2c,0,&events);px_battle_event_set_value(0x43,0,&events);log_[0]=0;
}
/* Event 0x2a/0x2b frames: key 0x04 is a value. */
static void frame_value(uint32_t attacker)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x03,attacker,&events);px_battle_event_set_value(0x04,0x77,&events);
    px_battle_event_set_value(0x12,move_v,&events);log_[0]=0;
}
static uint32_t key(uint32_t k) { return px_battle_event_get(k,&events); }
static int same(const char *expected) { if(strcmp(log_,expected)){ fprintf(stderr,"log [%s] expected [%s]\n",log_,expected);return 0; } return 1; }
#define R(n) (RECORD+0x40*(n))
#define W(i) r32(NULL,WORK+4*(i))
int main(void)
{
    unsigned cases=0;
    /* Nothing for another attacker (key 0x03, or 0x04 / 0x02 as each tests). */
    {
        const fn by3[]={px_battle_counter_target,px_battle_mirror_coat_target,px_battle_metal_burst_target,px_battle_bide_target,
                        px_battle_present_flag,px_battle_present_effect,px_battle_weather_accuracy,px_battle_beat_up,px_battle_fixed_flag,
                        px_battle_support_ready,px_battle_move_message_499,px_battle_chip_allies,px_battle_fling_effect};
        for(unsigned k=0;k<13;++k)for(uint32_t o=0;o<2;++o){ reset();w32(NULL,WORK,5);w32(NULL,WORK+0x18,1);mode=1;hits=1;weather_v=1;
            frame(o?0x102:3,5);by3[k](HANDLER,CTX,ATT,WORK,&api);
            CHECK(strstr(log_,"rec")==NULL&&key(0x53)==0&&key(0x2c)==0&&key(0x43)==0&&aborts==0&&W(0)==5);++cases; }
        const fn by4[]={px_battle_rage_boost,px_battle_bide_damage,px_battle_grudge_drain};
        for(unsigned k=0;k<3;++k)for(uint32_t o=0;o<2;++o){ reset();w32(NULL,WORK+0x18,1);nohp=1;
            frame(3,o?0x102:5);by4[k](HANDLER,CTX,ATT,WORK,&api);CHECK(strstr(log_,"rec")==NULL&&W(1)==0&&aborts==0);++cases; }
        reset();w32(NULL,WORK,1);frame(3,3);px_battle_pledge_ready(HANDLER,CTX,ATT,WORK,&api);CHECK(key(0x53)==0);++cases;
    }
    /* Counter, Mirror Coat, Metal Burst: the first qualifying hit; none; selector paths. */
    {
        const fn f[3]={px_battle_counter_target,px_battle_mirror_coat_target,px_battle_metal_burst_target};
        const uint32_t cats[3]={1,2,0};
        for(unsigned k=0;k<3;++k){
            /* no hits */
            reset();frame_value(ATT);f[k](HANDLER,CTX,ATT,WORK,&api);CHECK(same("dr0/0 ")&&key(0x04)==0x77);++cases;
            /* allies first, wrong category second, then a match (any category for Metal Burst) */
            reset();hits=3;hit_target[0]=6;hit_target[1]=7;hit_target[2]=8;hit_move[0]=0x111;hit_move[1]=cats[k]==1?0x222:0x111;hit_move[2]=cats[k]==2?0x222:0x111;
            ally_mask=1u<<6;selector_v[7]=selector_v[8]=0;frame_value(ATT);f[k](HANDLER,CTX,ATT,WORK,&api);CHECK(aborts==0);
            if(k==2) CHECK(same("dr0/0 ally18/6 dr0/1 ally18/7 os7 ")&&key(0x04)==7);
            else if(k==0) CHECK(same("dr0/0 ally18/6 dr0/1 ally18/7 cat222 dr0/2 ally18/8 cat111 os8 ")&&key(0x04)==8);
            else CHECK(same("dr0/0 ally18/6 dr0/1 ally18/7 cat111 dr0/2 ally18/8 cat222 os8 ")&&key(0x04)==8);
            ++cases;
        }
        /* An unreachable hitter (selector 0xa): its stored selector, then the chosen one, then itself. */
        for(uint32_t sel=0;sel<2;++sel)for(uint32_t t1=0;t1<2;++t1)for(uint32_t ch=0;ch<3;++ch)for(uint32_t t2=0;t2<2;++t2){
            reset();hits=1;hit_target[0]=6;hit_sel[0]=sel?3:0xa;selector_v[6]=0xa;target_v[3]=t1?9:0x1f;chosen=ch==0?0xa:4;target_v[4]=t2?12:0x1f;
            frame_value(ATT);px_battle_metal_burst_target(HANDLER,CTX,ATT,WORK,&api);
            uint32_t want=6;char e[200]="dr0/0 ally18/6 os6 ";
            int done=0;
            if(sel){ strcat(e,"tf3 ");
                if(t1) { want=9;done=1; } }
            if(!done){ strcat(e,"ct2/1 ");
                if(ch!=0){ strcat(e,"tf4 ");
                    if(t2) { want=12;done=1; } } }
            (void)done;
            CHECK(same(e)&&key(0x04)==want&&aborts==0);++cases;
        }
    }
    /* Bide's target: released early, a kind with a record, selector 0xa picks a random battler. */
    for(uint32_t w=0;w<4;++w)for(uint32_t u=0;u<8;++u)for(uint32_t sel=0;sel<2;++sel)for(uint32_t n=0;n<2;++n){
        reset();hits=1;w32(NULL,WORK,w==0?0:w==1?1:w==2?2:0x80000000u);used[0]=u&1;used[1]=u>>1&1;used[2]=u>>2&1;hit_target[0]=5;hit_target[1]=6;hit_target[2]=7;
        selector_v[5]=selector_v[6]=selector_v[7]=sel?0xa:0;sideb_n=n?3:0;sideb[0]=10;sideb[1]=11;sideb[2]=12;rnd=0x102;frame_value(ATT);
        px_battle_bide_target(HANDLER,CTX,ATT,WORK,&api);CHECK(aborts==0);
        if(w==0||w==1||w==3){ CHECK(log_[0]==0&&key(0x04)==0x77&&W(2)==0); ++cases; continue; }
        uint32_t first=3;for(uint32_t k=0;k<3;++k){ if(used[k]){ first=k;break; } }
        uint32_t want=first==3?0x1f:5+first;
        if(first<3&&sel) { want=n?(uint32_t)(10+(rnd&0xff)):0x1fu;
            if(n&&(rnd&0xff)>=3) { want=0xcc; } }
        CHECK(key(0x04)==want&&W(2)==want);
        CHECK(strstr(log_,"rnd3 ")!=NULL?(first<3&&sel&&n):!(first<3&&sel&&n));
        if(first<3&&sel&&n) { CHECK(strstr(log_,"side2 sb122 ")!=NULL); }
        ++cases;
    }
    /* Sky Drop's target. */
    for(uint32_t m=0;m<5;++m)for(uint32_t c=0;c<0x1b;++c){
        reset();mode=m;carried=c==0x1a?0x105:c;frame_value(ATT);px_battle_sky_drop_target(HANDLER,CTX,ATT,WORK,&api);
        const uint32_t cand=(carried-1)&0xff;const int ok=m!=0&&m!=3&&carried!=0&&cand<0x18;
        CHECK(key(0x04)==(ok?cand:0x77u));++cases;
    }
    /* Follow Me / Rage Powder: every refusal in turn, and the redirect. */
    for(uint32_t r=0;r<12;++r)for(uint32_t h=0;h<2;++h){
        reset();mode=1;w16(NULL,HANDLER+0x38,h?0x1dc:0x1db);move_v=0x10000+0x300;
        ally_mask=r==1?1u<<5:0;
        if(r==2) { move_v=0x1fb; }
        has_status_v=r==3;switch_v=r==4;has_type_v=r==5;ability_v=r==6?0x8e:0;
        if(r==7) { held=0x28a; }
        redirect=r==8;
        if(r==9) { mode=0; }
        if(r==10) { mode=3; }
        frame_value(5);px_battle_follow_me_target(HANDLER,CTX,ATT,WORK,&api);CHECK(aborts==0);
        int redirects=1;
        if(r==1||r==2||r==3||r==4||r==8||r==9||r==10) redirects=0;
        if(h&&(r==5||r==6||r==7)) redirects=0;
        CHECK(key(0x04)==(redirects?(uint32_t)ATT:0x77u));
        if(r==11&&!h) { CHECK(strstr(log_,"rb5/2/300 ")!=NULL); }
        if(r==11) { CHECK(strstr(log_,"st2/21 ")!=NULL&&(!h||strstr(log_,"ty5/b ")!=NULL)); }
        ++cases;
    }
    move_v=0x99;
    /* Present: chance, the stored flag, and the effect flag. */
    for(uint32_t c=0;c<2;++c)for(uint32_t w=0;w<3;++w){
        reset();chance_v=c;w32(NULL,WORK,w==0?0:w==1?1:0x100);frame(3,5);px_battle_present_flag(HANDLER,CTX,3,WORK,&api);
        if(c) CHECK(same("chance20 flag1 ")&&key(0x53)==1&&W(0)==1);
        else CHECK(same(w?"chance20 flag1 ":"chance20 flag0 ")&&key(0x53)==0&&W(0)==(w==0?0u:w==1?1u:0x100u));
        ++cases;
    }
    reset();chance_v=1;frame(3,5);px_battle_event_rewrite(0x53,9,&events);w32(NULL,WORK,0);px_battle_present_flag(HANDLER,CTX,3,WORK,&api);
    CHECK(same("chance20 flag0 ")&&W(0)==0&&key(0x53)==9);++cases;   /* a rewrite that does not take stores 0 */
    for(uint32_t w=0;w<2;++w)for(uint32_t f=0;f<2;++f){
        reset();full=f;w32(NULL,WORK,w?0x100:0);frame(3,0x105);px_battle_present_effect(HANDLER,CTX,3,WORK,&api);
        if(!w) CHECK(log_[0]==0);
        else if(f) CHECK(same("full5 rec3/3 msgd2/42 arg5/4 commit0 "));
        else { CHECK(same("full5 rec4/3 hp5/4 msg183/82 arg5/8 commit0 ")&&mem[R(0)+6]==5&&r16(NULL,R(0)+4)==0x2345&&mem[R(0)+7]==0xa5); }
        ++cases;
    }
    /* Weather-dependent accuracy. */
    for(uint32_t w=0;w<4;++w){ reset();weather_v=w;frame(3,5);px_battle_weather_accuracy(HANDLER,CTX,3,WORK,&api);CHECK(key(0x2c)==(w==1?0x32u:0u));++cases; }
    /* Beat Up: party sizes, members that qualify and the round skips. */
    for(uint32_t n=0;n<7;++n)for(uint32_t m=0;m<64;m+=3){
        reset();w8(NULL,PARTY+0x18,(uint8_t)n);w32(NULL,WORK,5);
        uint32_t qual=0;
        for(unsigned i=0;i<6;++i){ w8(NULL,0x9000+0x100*i+0x19,(uint8_t)(m>>i&1?3:0x10+i));battle_ok[i]=(m*7+i)%3!=0;status_v[i]=(m+i)%4==0; }
        frame(3,5);px_battle_beat_up(HANDLER,0x3000,3,WORK,&api);(void)qual;
        /* recompute */
        uint32_t counted=0;
        for(uint32_t round=0;round<6;++round){ uint32_t skip=round;
            if(n<=skip||n==0) { continue; }
            for(uint32_t i=0;i<n;++i){ int take=(m>>i&1)!=0;if(!take&&battle_ok[i]) take=status_v[i]==0;
                if(!take) { continue; }
                if(skip) { --skip;continue; }
                ++counted;break; } }
        if(counted==0) { counted=1; }
        CHECK(key(0x2b)==counted&&W(0)==0&&aborts==0);++cases;
    }
    /* Move 167, 215/312 and the pledges. */
    reset();frame(3,3);px_battle_fixed_flag(HANDLER,CTX,3,WORK,&api);CHECK(key(0x43)==1&&log_[0]==0);++cases;
    reset();frame(3,3);px_battle_support_ready(HANDLER,CTX,3,WORK,&api);CHECK(key(0x53)==1&&log_[0]==0);++cases;
    for(uint32_t w=0;w<2;++w){ reset();w32(NULL,WORK,w?0x100:0);frame(3,3);px_battle_pledge_ready(HANDLER,CTX,3,WORK,&api);CHECK(key(0x53)==(w?1u:0u));++cases; }
    reset();w32(NULL,WORK,1);px_battle_event_push(&events);px_battle_event_set_id(0x02,5,&events);px_battle_event_set_id(0x03,3,&events);
    px_battle_event_set_value(0x53,0,&events);px_battle_pledge_ready(HANDLER,CTX,3,WORK,&api);CHECK(key(0x53)==0);++cases;
    /* Move 499. */
    for(uint32_t f=0;f<2;++f){ reset();flag_v=f;frame(3,0x105);px_battle_move_message_499(HANDLER,CTX,3,WORK,&api);flag_v=0;
        if(f) CHECK(log_[0]==0);else CHECK(same("recf/3 commit0 rec3/3 msgc3/442 arg5/44 commit1 ")&&mem[R(0)+4]==1&&mem[R(0)+5]==5&&mem[R(0)+6]==0xa5); ++cases; }
    /* Rage. */
    for(uint32_t f=0;f<2;++f)for(uint32_t h=0;h<2;++h){
        reset();flag_v=f;nohp=h;for(unsigned i=0;i<12;++i) { w8(NULL,R(i)+0x18,(uint8_t)(0x13*i+7)); }frame(3,ATT);px_battle_rage_boost(HANDLER,CTX,ATT,WORK,&api);flag_v=0;
        if(f) CHECK(log_[0]==0);
        else if(h) CHECK(same("nohp2 "));
        else { CHECK(same("nohp2 recd/2 msg214/1c2 arg2/1c commit0 ")&&mem[R(0)+0x18]==((7&~2)|5)&&mem[R(0)+0xc]==ATT&&mem[R(0)+4]==1&&mem[R(0)+0x16]==1&&mem[R(0)+0x17]==1); }
        ++cases;
    }
    /* Bide's damage counter. */
    for(uint32_t w=0;w<2;++w)for(uint32_t f=0;f<2;++f){
        reset();flag_v=f;bonus_v=0xfffffff0u;w32(NULL,WORK+4,0x20);w32(NULL,WORK+0x18,w?0x100:0);frame(3,ATT);px_battle_bide_damage(HANDLER,CTX,ATT,WORK,&api);flag_v=0;bonus_v=0;
        CHECK(W(1)==((!f&&w)?0x10u:0x20u));++cases;
    }
    /* Move 481: the target's side positions. */
    for(uint32_t p=0;p<3;++p)for(uint32_t n=0;n<5;++n){
        reset();pos=p==0?0xa:p==1?3:0x104;sideb_n=n;sideb[0]=5;sideb[1]=6;sideb[2]=7;sideb[3]=5;sideb[4]=3;frame(3,5);px_battle_chip_allies(HANDLER,CTX,3,WORK,&api);
        if(p==0) { CHECK(same("pos5 ")); ++cases; continue; }
        unsigned recs=0;for(uint32_t i=0;i<n;++i){ if(sideb[i]!=5) ++recs; }
        CHECK(records==recs);CHECK(strstr(log_,"sb903 ")!=NULL||p==2);
        if(n>1) { CHECK(mem[R(0)+6]==6&&r16(NULL,R(0)+4)==0x2345&&mem[R(0)+7]==(0xa5|1));
            CHECK(strstr(log_,"hp6/16 ")!=NULL&&strstr(log_,"msg457/")!=NULL); }
        ++cases;
    }
    /* Grudge. */
    for(uint32_t b=0;b<2;++b)for(uint32_t h=0;h<2;++h)for(uint32_t s=0;s<5;s+=4){
        reset();blocked_v=b;nohp=h;slot_v=s;pp=0x105;used_v=0x10321;frame(4,ATT);px_battle_event_rewrite(0x53,0,&events);
        const uint32_t hndl=px_battle_handler_register(0,288,1,0,ATT,ENTRIES,1,&handlers);log_[0]=0;
        px_battle_grudge_drain(hndl,CTX,ATT,hndl+0x1c,&api);used_v=0x321;
        if(b||!h) CHECK(strstr(log_,"rec")==NULL&&r32(NULL,hndl+0x18)!=0);
        else if(s==4) CHECK(strstr(log_,"rec")==NULL&&r32(NULL,hndl+0x18)!=0);
        else { CHECK(strstr(log_,"rec9/2 ")!=NULL&&mem[R(0)+5]==4&&mem[R(0)+6]==0&&mem[R(0)+4]==(uint8_t)(0u-0x105)&&mem[R(0)+7]==(0xa5|2)&&r32(NULL,hndl+0x18)==0);
            CHECK(strstr(log_,"msg27b/82 arg4/8 arg321/8 commit0 ")!=NULL); }
        ++cases;
    }
    /* Fling. */
    for(uint32_t i=0;i<2;++i)for(uint32_t f=0;f<3;++f)for(uint32_t p=0;p<2;++p){
        reset();w16(NULL,MON+0x200+0x12,i?0x1234:0);flag_v=f==1;flag2_v=f==2;item_p=p;frame(ATT,0x105);px_battle_fling_effect(HANDLER,CTX,ATT,WORK,&api);flag_v=flag2_v=0;
        if(!i){ CHECK(log_[0]==0&&W(0)==0); ++cases; continue; }
        CHECK(W(0)==1&&mem[R(0)+4]==1&&mem[R(0)+5]==1);
        if(f||!p) CHECK(records==1);
        else CHECK(same("rec24/2 commit0 ip1234/5 rec23/2 commit1 ")&&mem[R(1)+4]==5&&r16(NULL,R(1)+6)==0x1234&&mem[R(1)+5]==1);
        ++cases;
    }
    /* Through the dispatcher on event 0x42 (Heal Bell) and event 0x3a (move 167). */
    {
        reset();
        for(unsigned i=0;i<2;++i){ w8(NULL,ENTRIES+0x10*i,0x42);w32(NULL,ENTRIES+0x10*i+4,i); }
        const uint8_t ids[2]={ATT,ATT};
        for(unsigned i=0;i<2;++i) {
            const uint32_t hn=NODES+0x40*i;
            w32(NULL,hn+8,ENTRIES+0x10*i);w32(NULL,hn+0x18,0x20000000u|1u<<16);w8(NULL,hn+0x3a,ids[i]);
            w32(NULL,hn,i?hn-0x40:0);w32(NULL,hn+4,i<1?hn+0x40:0);
        }
        w32(NULL,BATTLE+0x60,NODES);frame(ATT,ATT);px_battle_event_dispatch(CTX,0x42,&events);
        CHECK(key(0x43)==1&&key(0x53)==1&&aborts==0);++cases;
    }
    printf("battle_moves_16: %u cases, all tests passed\n",cases);
    return 0;
}
