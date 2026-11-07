#include "battle_abilities_1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, SCRATCH=0x7400, ME=2, FOE=4 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
/* Battler i at MON + 0x100 i; values by key. */
struct mon { uint32_t hp,max_hp,ability,effective,gender,item; int grass; };
static struct mon mons[8];
static uint32_t contact_value,random_value,chance_value,forced_value,decided,ally,can_mask;static int32_t rooms[2];static unsigned nroom;
static unsigned aborts,ncommit;static uint8_t commits[4][0x30];static char log_[1024];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static uint32_t value(void *u,uint32_t p,uint32_t k) { (void)u;const struct mon *m=&mons[mon(p)];notef("v%u/%x ",mon(p),k);
    switch(k){case 0xd:return m->hp;case 0xe:return m->max_hp;case 0x10:return m->ability;case 0x11:return m->effective;case 0x12:return m->gender;}
    CHECK(0);return 0; }
static const struct px_battle_ability_api base={&events,value,NULL,NULL,NULL,NULL,NULL};
static uint32_t move_flag(void *u,uint32_t m,uint32_t f) { (void)u;notef("mf%x/%u ",m,f);return contact_value; }
static uint32_t has_type(void *u,uint32_t p,uint32_t t) { (void)u;notef("ty%u/%x ",mon(p),t);return t==0xb&&mons[mon(p)].grass; }
static uint32_t held_item(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX&&id<8);notef("it%u ",id,0);return mons[id].item; }
static uint32_t random_below(void *u,uint32_t n) { (void)u;notef("rnd%u ",n,0);CHECK(random_value<n);return random_value; }
static uint32_t chance(void *u,uint32_t p) { (void)u;notef("ch%u ",p,0);return chance_value; }
static uint32_t forced(void *u,uint32_t c,uint32_t f) { (void)u;CHECK(c==CTX);notef("fo%u ",f,0);return forced_value; }
static uint32_t condition(void *u,uint32_t s) { (void)u;notef("cd%x ",s,0);return 0xc000+s; }
static void condition_for(void *u,uint32_t s,uint32_t p,uint32_t out) { (void)u;CHECK(out==SCRATCH);notef("cf%x/%u ",s,mon(p));w32(NULL,out,0xcf00+mon(p)); }
static uint32_t battle_decided(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("dec ");return decided; }
static uint32_t is_ally(void *u,uint32_t id,uint32_t o) { (void)u;notef("ally%u/%u ",id,o);return ally; }
static int32_t stage_room(void *u,uint32_t p,uint32_t s) { (void)u;notef("room%u/%u ",mon(p),s);CHECK(nroom<2);return rooms[nroom++]; }
static uint32_t can_change(void *u,uint32_t p,uint32_t s,int32_t d) { (void)u;char t[32];snprintf(t,sizeof t,"can%u/%u/%d ",mon(p),s,(int)d);note(t);
    return s==2&&d==-1?can_mask&1:s==5&&d==1?can_mask>>1&1:0; }
static void popup(void *u,uint32_t c,uint32_t p,uint32_t id) { (void)u;CHECK(c==CTX);notef("pop%u/%u ",p,id); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);
    /* A record holding stale bytes, with a header word and flag bytes already set. */
    memset(mem+RECORD,0xa5,0x30);w32(NULL,RECORD,0x11);w8(NULL,RECORD+7,0x40);w8(NULL,RECORD+0x18,0x40);return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;notef("msg%x/%x ",id,(r-RECORD)*16+k); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;(void)r;notef("arg%x ",v,0); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD&&ncommit<4);memcpy(commits[ncommit++],mem+RECORD,0x30);note("commit "); }
static const struct px_battle_abilities_1_api api={&base,move_flag,has_type,held_item,random_below,chance,forced,condition,condition_for,
    battle_decided,is_ally,stage_room,can_change,popup,action_record,message,message_arg,commit,SCRATCH};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_abilities_1_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_static,px_battle_rough_skin};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(mons,0,sizeof mons);memset(commits,0,sizeof commits);
    contact_value=1;random_value=chance_value=forced_value=decided=ally=can_mask=0;nroom=0;rooms[0]=rooms[1]=0;aborts=ncommit=0;log_[0]=0;
    w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i){w32(NULL,TABLE+0x84+4*i,MON+0x100*i);mons[i].hp=mons[i].max_hp=100+i;}
}
/* Keys of event 0x53. */
struct keys { uint32_t attacker,defender,move,type,category,critical,k47,k48; };
static void frame(struct keys k)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x03,k.attacker,&events);px_battle_event_set_id(0x04,k.defender,&events);
    px_battle_event_set_value(0x12,k.move,&events);px_battle_event_set_value(0x16,k.type,&events);
    px_battle_event_set_value(0x1b,k.category,&events);px_battle_event_set_value(0x46,k.critical,&events);
    px_battle_event_set_value(0x47,k.k47,&events);px_battle_event_set_value(0x48,k.k48,&events);log_[0]=0;
}
static const struct keys HIT={FOE,ME,0x10021,0x10,1,0,0,0};   /* move 0x21 in the low half */
static void run(fn f,struct keys k) { frame(k);f(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0); }
static uint32_t c16(unsigned n,unsigned o) { return (uint32_t)(commits[n][o]|commits[n][o+1]<<8); }
static uint32_t c32(unsigned n,unsigned o) { return c16(n,o)|c16(n,o+2)<<16; }
/* Expected log of the contact-status helper for the current settings. */
static void contact_expect(char *out,size_t n,struct keys k,uint32_t status)
{
    out[0]=0;
    if(k.defender!=ME||k.k47)return;
    size_t at=(size_t)snprintf(out,n,"mf21/0 ");
    if(!contact_value)return;
    at+=(size_t)snprintf(out+at,n-at,"ch30 ");
    if(!chance_value){at+=(size_t)snprintf(out+at,n-at,"fo1 ");if(!forced_value)return;}
    snprintf(out+at,n-at,"recb/%u commit ",(unsigned)ME);(void)status;
}
static void contact_record(uint32_t status,uint32_t cond)
{
    CHECK(ncommit==1&&c16(0,4)==status&&c32(0,8)==cond&&commits[0][0xc]==0&&commits[0][0xf]==FOE&&c32(0,0)==0x800011);
}
int main(void)
{
    unsigned cases=0;char want[256],pre[64];
    /* Static, Poison Point, Flame Body: the condition first, then 30% on contact. */
    {
        const fn f[3]={px_battle_static,px_battle_poison_point,px_battle_flame_body};const uint32_t st[3]={1,5,4};
        for(unsigned h=0;h<3;++h)for(unsigned bits=0;bits<32;++bits) {
            reset();struct keys k=HIT;k.defender=bits&1?ME:3;k.k47=bits>>1&1;contact_value=bits>>2&1;chance_value=bits>>3&1;forced_value=bits>>4&1;
            run(f[h],k);snprintf(pre,sizeof pre,"cd%x ",st[h]);contact_expect(want+strlen(pre),sizeof want-strlen(pre),k,st[h]);memcpy(want,pre,strlen(pre));
            CHECK(!strcmp(log_,want));
            if(ncommit)contact_record(st[h],0xc000+st[h]);
            ++cases;
        }
    }
    /* Effect Spore: immunities, then the roll: above 20 poison, above 10 paralysis, else sleep. */
    for(unsigned bits=0;bits<32;++bits)for(uint32_t roll=0;roll<30;++roll) {
        reset();struct keys k=HIT;k.defender=bits&1?ME:3;k.k47=bits>>1&1;mons[FOE].grass=bits>>2&1;
        mons[FOE].effective=bits>>3&1?0x8e:0x8f;mons[FOE].ability=0x8e;mons[FOE].item=bits>>4&1?0x28a:0x28b;random_value=roll;chance_value=1;
        if(roll%7&&bits!=1)continue;  /* every roll on the open path; three rolls elsewhere */
        run(px_battle_effect_spore,k);
        const uint32_t s=roll>20?5:roll>10?1:2;want[0]=0;
        if(k.defender==ME&&!k.k47){strcat(want,"ty4/b ");
            if(!mons[FOE].grass){strcat(want,"v4/11 ");
                if(mons[FOE].effective!=0x8e){strcat(want,"it4 ");
                    if(mons[FOE].item!=0x28a){char t[64];snprintf(t,sizeof t,"rnd30 cd%x mf21/0 ch30 recb/2 commit ",s);strcat(want,t);}}}}
        CHECK(!strcmp(log_,want));
        if(ncommit)contact_record(s,0xc000+s);
        ++cases;
    }
    /* Random boundaries exactly. */
    {
        const uint32_t rolls[6]={0,10,11,20,21,29},want_s[6]={2,2,1,1,5,5};
        for(unsigned i=0;i<6;++i){reset();random_value=rolls[i];chance_value=1;run(px_battle_effect_spore,HIT);contact_record(want_s[i],0xc000+want_s[i]);++cases;}
    }
    /* Cute Charm: opposite genders only, the condition from 1084c438. */
    for(unsigned bits=0;bits<8;++bits)for(uint32_t g=0;g<9;++g) {
        reset();struct keys k=HIT;k.defender=bits&1?ME:3;k.k47=bits>>1&1;decided=bits>>2&1;mons[ME].gender=g/3|0x100;mons[FOE].gender=g%3|0x200;chance_value=1;
        run(px_battle_cute_charm,k);
        if(k.defender!=ME||k.k47){CHECK(log_[0]==0);++cases;continue;}
        if(decided){CHECK(!strcmp(log_,"dec "));++cases;continue;}
        const int on=g/3!=2&&g%3!=2&&g/3!=g%3;
        snprintf(want,sizeof want,"dec v2/12 v4/12 %s",on?"cf7/2 mf21/0 ch30 recb/2 commit ":"");
        CHECK(!strcmp(log_,want));
        if(on)contact_record(7,0xcf02);
        ++cases;
    }
    /* Poison Touch: the attacker's contact move poisons the defender (30%). */
    for(unsigned bits=0;bits<64;++bits) {
        reset();struct keys k=HIT;k.attacker=bits&1?ME:3;k.defender=FOE;k.k47=bits>>1&1;k.k48=bits>>2&1;contact_value=bits>>3&1;chance_value=bits>>4&1;forced_value=bits>>5&1;
        run(px_battle_poison_touch,k);
        want[0]=0;
        if(k.attacker==ME&&!k.k47&&!k.k48){strcat(want,"mf21/0 ");
            if(contact_value){strcat(want,"ch30 ");if(!chance_value)strcat(want,"fo1 ");
                if(chance_value||forced_value)strcat(want,"recb/2 cd5 msg1d8/142 arg4 commit ");}}
        CHECK(!strcmp(log_,want));
        if(ncommit)CHECK(c16(0,4)==5&&c32(0,8)==0xc005&&commits[0][0xf]==FOE&&c32(0,0)==0x800011);
        ++cases;
    }
    /* Rough Skin / Iron Barbs: 1/8 of the attacker's max HP; Aftermath: 1/4 when this battler fainted. */
    for(unsigned bits=0;bits<16;++bits)for(uint32_t max=0;max<20;++max) {
        reset();struct keys k=HIT;k.defender=bits&1?ME:3;k.k47=bits>>1&1;contact_value=bits>>2&1;mons[FOE].hp=bits>>3&1?0:5;mons[FOE].max_hp=max*7;
        run(px_battle_rough_skin,k);want[0]=0;
        if(k.defender==ME&&!k.k47){strcat(want,"mf21/0 ");
            if(contact_value){strcat(want,"v4/d ");if(mons[FOE].hp)strcat(want,"rec6/2 v4/e msg1ae/c2 arg4 commit ");}}
        CHECK(!strcmp(log_,want));
        if(ncommit)CHECK(commits[0][6]==FOE&&c16(0,4)==(max*7/8?max*7/8:1)&&commits[0][7]==0x40&&c32(0,0)==0x800011);
        ++cases;
        reset();mons[ME].hp=bits>>3&1?0:9;mons[FOE].max_hp=max*7;contact_value=bits>>2&1;mons[FOE].hp=bits>>1&1?0:5;
        run(px_battle_aftermath,k);want[0]=0;
        if(k.defender==ME){strcat(want,"v2/d ");
            if(!mons[ME].hp){strcat(want,"mf21/0 ");if(contact_value)strcat(want,"rec6/2 v4/e msg192/c2 arg4 commit ");}}
        CHECK(!strcmp(log_,want));
        if(ncommit)CHECK(commits[0][6]==FOE&&c16(0,4)==(max*7/4?max*7/4:1)&&commits[0][7]==0x41&&c32(0,0)==0x800011);
        ++cases;
    }
    /* Helpers directly. */
    for(uint32_t max=0;max<40;++max)for(uint32_t d=1;d<9;++d){reset();mons[3].max_hp=max;CHECK(px_battle_hp_share(MON+0x300,d,&api)==(max/d?max/d:1));++cases;}
    reset();mons[3].hp=0;CHECK(px_battle_fainted(MON+0x300,&api)==1);mons[3].hp=1;CHECK(px_battle_fainted(MON+0x300,&api)==0);
    mons[3].hp=0x10000;CHECK(px_battle_fainted(MON+0x300,&api)==0);++cases;
    /* Mummy: the attacker's ability becomes Mummy; flagged unless an ally. */
    for(unsigned bits=0;bits<64;++bits) {
        reset();struct keys k=HIT;k.defender=bits&1?ME:3;k.k47=bits>>1&1;decided=bits>>2&1;contact_value=bits>>3&1;
        mons[FOE].ability=bits>>4&1?0x98:0x97;mons[FOE].effective=0x98;ally=bits>>5&1;
        run(px_battle_mummy,k);want[0]=0;
        if(k.defender==ME&&!k.k47){strcat(want,"dec ");
            if(!decided){strcat(want,"mf21/0 ");
                if(contact_value){strcat(want,"v4/10 ");if(mons[FOE].ability!=0x98)strcat(want,"rec20/2 msg1cf/c2 arg4 ally2/4 commit ");}}}
        CHECK(!strcmp(log_,want));
        if(ncommit)CHECK(commits[0][6]==FOE&&c16(0,4)==0x98&&c32(0,0)==(ally?0x11u:0x800011u));
        ++cases;
    }
    /* Gooey: the attacker's Speed -1 (flags 0x12). */
    for(unsigned bits=0;bits<16;++bits) {
        reset();struct keys k=HIT;k.defender=bits&1?ME:3;k.k47=bits>>1&1;contact_value=bits>>2&1;mons[FOE].hp=bits>>3&1?0:1;
        run(px_battle_gooey,k);want[0]=0;
        if(k.defender==ME&&!k.k47){strcat(want,"mf21/0 ");if(contact_value){strcat(want,"v4/d ");if(mons[FOE].hp)strcat(want,"recd/2 commit ");}}
        CHECK(!strcmp(log_,want));
        if(ncommit)CHECK(commits[0][4]==5&&commits[0][0xc]==FOE&&commits[0][0x16]==1&&commits[0][0x17]==0xff&&commits[0][0x18]==0x52&&c32(0,0)==0x800011);
        ++cases;
    }
    /* Rattled (Dark, Bug, Ghost: Speed +1) and Justified (Dark: Attack +1) over every type byte and a high bit. */
    for(unsigned bits=0;bits<4;++bits)for(uint32_t t=0;t<0x200;++t) {
        if(t>=0x20&&t<0x100)continue;
        reset();struct keys k=HIT;k.defender=bits&1?ME:3;k.k47=bits>>1&1;k.type=t;
        run(px_battle_rattled,k);const uint32_t b=t&0xff;int on=k.defender==ME&&!k.k47&&(b==16||b==6||b==7);
        CHECK(!strcmp(log_,on?"recd/2 commit ":""));
        if(on)CHECK(commits[0][4]==5&&commits[0][0xc]==ME&&commits[0][0x16]==1&&commits[0][0x17]==1&&commits[0][0x18]==0x40&&c32(0,0)==0x800011);
        reset();run(px_battle_justified,k);on=k.defender==ME&&!k.k47&&t==16;
        CHECK(!strcmp(log_,on?"recd/2 commit ":""));
        if(on)CHECK(commits[0][4]==1&&commits[0][0xc]==ME&&commits[0][0x16]==1&&commits[0][0x17]==1&&commits[0][0x18]==0x40&&c32(0,0)==0x800011);
        cases+=2;
    }
    /* Anger Point: a critical hit raises Attack by every stage left. */
    for(unsigned bits=0;bits<8;++bits)for(int room=-2;room<8;++room) {
        reset();struct keys k=HIT;k.defender=bits&1?ME:3;k.k47=bits>>1&1;k.critical=bits>>2&1?0x100:0;rooms[0]=room;rooms[1]=room+0x40;
        run(px_battle_anger_point,k);want[0]=0;
        if(k.defender==ME&&!k.k47&&k.critical){strcat(want,"room2/1 ");if(room>0)strcat(want,"recd/2 room2/1 msg1e1/1c2 arg2 commit ");}
        CHECK(!strcmp(log_,want));
        if(ncommit)CHECK(commits[0][4]==1&&commits[0][0xc]==ME&&commits[0][0x16]==1&&commits[0][0x17]==(uint8_t)(room+0x40)&&commits[0][0x18]==0x43&&c32(0,0)==0x800011);
        ++cases;
    }
    /* Weak Armor: a physical hit, Defense -1 then Speed +1, unflagged. */
    for(unsigned bits=0;bits<128;++bits) {
        static const uint32_t categories[4]={1,2,0x101,0};
        reset();struct keys k=HIT;k.defender=bits&1?ME:3;k.category=categories[bits>>1&3];k.k47=bits>>3&1;can_mask=bits>>4&3;mons[ME].hp=bits>>6&1?0:3;
        run(px_battle_weak_armor,k);want[0]=0;
        if(k.defender==ME&&k.category==1&&!k.k47){strcat(want,"can2/2/-1 ");if(!(can_mask&1))strcat(want,"can2/5/1 ");
            if(can_mask){strcat(want,"v2/d ");if(mons[ME].hp)strcat(want,"pop1/2 recd/2 commit recd/2 commit pop2/2 ");}}
        CHECK(!strcmp(log_,want));
        if(ncommit){CHECK(ncommit==2&&commits[0][4]==2&&commits[0][0x17]==0xff&&commits[1][4]==5&&commits[1][0x17]==1);
            for(unsigned i=0;i<2;++i)CHECK(commits[i][0xc]==ME&&commits[i][0x16]==1&&commits[i][0x18]==0x42&&c32(i,0)==0x11);}
        ++cases;
    }
    /* Through the dispatcher: Static and Rough Skin on event 0x53. */
    {
        reset();chance_value=1;
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7c00+0x10*i;
            w8(NULL,entries,0x53);w32(NULL,entries+4,i);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ME);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);frame(HIT);px_battle_event_dispatch(CTX,0x53,&events);
        CHECK(!strcmp(log_,"cd1 mf21/0 ch30 recb/2 commit mf21/0 v4/d rec6/2 v4/e msg1ae/c2 arg4 commit ")&&aborts==0);++cases;
    }
    printf("battle_abilities_1: %u cases, all tests passed\n",cases);
    return 0;
}
