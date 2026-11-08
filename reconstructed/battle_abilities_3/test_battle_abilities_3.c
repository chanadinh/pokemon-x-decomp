#include "battle_abilities_3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, LIST=0x7400, ACTION=0x7410, BUFFER=0x7800, ME=2 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
/* Battler i at MON + 0x100 i: species +0xc, held +0x12, consumed +0x14, id +0x19. */
struct mon { uint32_t max_hp,status,battle_flag,turn_flag,asleep; };
static struct mon mons[8];
static uint32_t list[8],nlist,list_count,chance_value,forced_value,random_value,up_mask,down_mask,weather_value,berry_value,locked,
    queued,action_word;
static unsigned aborts,ncommit;static uint8_t commits[8][0x20];static char log_[2048];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static uint32_t value(void *u,uint32_t p,uint32_t k) { (void)u;notef("v%u/%x ",mon(p),k);CHECK(k==0xe);return mons[mon(p)].max_hp; }
static uint32_t status(void *u,uint32_t p) { (void)u;notef("st%u ",mon(p),0);return mons[mon(p)].status; }
static uint32_t side_of(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("pos%u ",id,0);return 0x31; }
static uint32_t side_battlers(void *u,uint32_t c,uint32_t s,uint32_t out) { (void)u;CHECK(c==CTX);notef("list%x ",s,0);
    for(uint32_t i=0;i<nlist;++i){w8(NULL,out+i,(uint8_t)list[i]);}return list_count; }
static const struct px_battle_ability_api base={&events,value,NULL,status,side_of,side_battlers,NULL};
static uint32_t battle_flag(void *u,uint32_t p,uint32_t f) { (void)u;notef("bf%u/%u ",mon(p),f);return mons[mon(p)].battle_flag; }
static uint32_t turn_flag(void *u,uint32_t p,uint32_t f) { (void)u;notef("tf%u/%u ",mon(p),f);return mons[mon(p)].turn_flag; }
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;notef("hs%u/%u ",mon(p),s);return s==2&&mons[mon(p)].asleep; }
static uint32_t chance(void *u,uint32_t p) { (void)u;notef("ch%u ",p,0);return chance_value; }
static uint32_t forced(void *u,uint32_t c,uint32_t f) { (void)u;CHECK(c==CTX);notef("fo%u ",f,0);return forced_value; }
static uint32_t random_below(void *u,uint32_t n) { (void)u;notef("rnd%u ",n,0);return n?random_value%n:0; }
static uint32_t can_change(void *u,uint32_t p,uint32_t s,int32_t d) { (void)u;char t[32];snprintf(t,sizeof t,"can%u/%u/%d ",mon(p),s,(int)d);note(t);
    CHECK(s>=1&&s<=7&&(d==1||d==-1));return ((d==1?up_mask:down_mask)>>(s-1))&1; }
static uint32_t temp_buffer(void *u,uint32_t c,uint32_t n) { (void)u;CHECK(c==CTX);notef("buf%x ",n,0);return BUFFER; }
static uint32_t opposing(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("opp%u ",id,0);return 0x30; }
static uint32_t weather(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("w ");return weather_value; }
static uint32_t is_berry(void *u,uint32_t i) { (void)u;notef("berry%x ",i,0);return berry_value; }
static uint32_t item_locked(void *u,uint32_t c,uint32_t s,uint32_t i) { (void)u;CHECK(c==CTX);notef("lock%x/%x ",s,i);return locked; }
static uint32_t queued_action(void *u,uint32_t c,uint32_t id,uint32_t out) { (void)u;CHECK(c==CTX&&out==ACTION);notef("qa%u ",id,0);
    if(queued){w32(NULL,out,action_word);}
    return queued; }
static void popup(void *u,uint32_t c,uint32_t p,uint32_t id) { (void)u;CHECK(c==CTX);notef("pop%u/%u ",p,id); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);
    memset(mem+RECORD,0xa5,0x20);w32(NULL,RECORD,0x11);return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;notef("msg%x/%x ",id,(r-RECORD)*16+k); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;(void)r;notef("arg%x ",v,0); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD&&ncommit<8);memcpy(commits[ncommit++],mem+RECORD,0x20);note("commit "); }
static const struct px_battle_abilities_3_api api={&base,battle_flag,turn_flag,has_status,chance,forced,random_below,can_change,temp_buffer,
    opposing,weather,is_berry,item_locked,queued_action,popup,action_record,message,message_arg,commit,LIST,ACTION};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_abilities_3_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_speed_boost,px_battle_levitate_turn};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(mons,0,sizeof mons);memset(commits,0,sizeof commits);
    nlist=list_count=chance_value=forced_value=random_value=up_mask=down_mask=weather_value=berry_value=locked=queued=action_word=0;
    aborts=ncommit=0;log_[0]=0;w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i){w32(NULL,TABLE+0x84+4*i,MON+0x100*i);w8(NULL,MON+0x100*i+0x19,(uint8_t)i);w16(NULL,MON+0x100*i+0xc,(uint16_t)(0x180+i));}
}
static void frame(uint32_t who) { px_battle_event_push(&events);px_battle_event_set_id(0x02,who,&events);log_[0]=0; }
static void run(fn f,uint32_t who) { frame(who);f(HANDLER,CTX,ME,WORK,&api);CHECK(aborts==0); }
static uint32_t c16(unsigned n,unsigned o) { return (uint32_t)(commits[n][o]|commits[n][o+1]<<8); }
static uint32_t c32(unsigned n,unsigned o) { return c16(n,o)|c16(n,o+2)<<16; }
static void cat(char *w,size_t n,const char *f,uint32_t a,uint32_t b) { size_t l=strlen(w);snprintf(w+l,n-l,f,a,b); }
static void stage_ok(unsigned n,uint32_t stat,uint32_t stages,uint32_t flags)
{
    CHECK(commits[n][0x16]==1&&commits[n][0xc]==ME&&commits[n][0x17]==stages&&commits[n][4]==stat&&c32(n,0)==flags&&commits[n][0x18]==0xa5);
}
static void cure_ok(unsigned n,uint32_t target) { CHECK(commits[n][4]==0x26&&commits[n][5]==target&&commits[n][0x11]==1&&c32(n,0)==0x2800011&&commits[n][0x10]==0xa5); }
int main(void)
{
    unsigned cases=0;char want[2048];
    /* Speed Boost: Speed +1 once battle flag 0 is set. */
    for(unsigned bits=0;bits<4;++bits) {
        reset();mons[ME].battle_flag=bits>>1&1?0x80:0;run(px_battle_speed_boost,bits&1?ME:3);
        want[0]=0;if(bits&1){strcat(want,"bf2/0 ");if(bits&2)strcat(want,"recd/2 commit ");}
        CHECK(!strcmp(log_,want));if(ncommit)stage_ok(0,5,1,0x800011);++cases;
    }
    /* Moody over every raisable and lowerable set and three rolls. */
    for(uint32_t um=0;um<128;++um)for(uint32_t dm=0;dm<128;++dm)for(uint32_t rv=0;rv<3;++rv) {
        reset();up_mask=um;down_mask=dm;random_value=rv*17+rv;run(px_battle_moody,ME);
        uint32_t ups[7],downs[7],nu=0,nd=0;
        strcpy(want,"bufb6 ");
        for(uint32_t s=1;s<=7;++s){cat(want,sizeof want,"can2/%u/1 can2/%u/-1 ",s,s);if(um>>(s-1)&1)ups[nu++]=s;if(dm>>(s-1)&1)downs[nd++]=s;}
        uint32_t up=0,down=0;
        if(!nu){if(nd){cat(want,sizeof want,"rnd%u ",nd,0);down=downs[random_value%nd];}}
        else if(!nd){cat(want,sizeof want,"rnd%u ",nu,0);up=ups[random_value%nu];}
        else{uint32_t pu[64],pd[64],np=0;
            for(uint32_t i=0;i<nu;++i)for(uint32_t j=0;j<nd;++j)if(ups[i]!=downs[j]){pu[np]=ups[i];pd[np]=downs[j];++np;}
            cat(want,sizeof want,"rnd%u ",np,0);if(np){up=pu[random_value%np];down=pd[random_value%np];}}
        strcat(want,"pop1/2 ");if(up)strcat(want,"recd/2 commit ");if(down)strcat(want,"recd/2 commit ");strcat(want,"pop2/2 ");
        if(strcmp(log_,want))fprintf(stderr,"got  %s\nwant %s\n",log_,want);
        CHECK(!strcmp(log_,want));
        unsigned k=0;if(up)stage_ok(k++,up,2,0x11);if(down)stage_ok(k++,down,0xff,0x11);CHECK(ncommit==k);++cases;
    }
    reset();up_mask=down_mask=0x7f;run(px_battle_moody,3);CHECK(log_[0]==0);++cases;
    /* Harvest: a consumed berry and no held item; sun, or 50% (or forced). */
    for(unsigned bits=0;bits<128;++bits) {
        reset();w16(NULL,MON+0x214,bits&2?0x9b:0);berry_value=bits>>2&1;w16(NULL,MON+0x212,bits&8?0x20:0);
        weather_value=bits&16?1:0x101;chance_value=bits>>5&1;forced_value=bits>>6&1;run(px_battle_harvest,bits&1?ME:3);
        want[0]=0;
        if(bits&1&&bits&2){strcat(want,"berry9b ");
            if(berry_value&&!(bits&8)){strcat(want,"w ");
                int on=bits&16;if(!on){strcat(want,"ch50 ");on=chance_value;if(!on){strcat(want,"fo1 ");on=forced_value;}}
                if(on)strcat(want,"rec21/2 msg1db/c2 arg2 arg9b commit ");}}
        CHECK(!strcmp(log_,want));
        if(ncommit)CHECK(c16(0,4)==0x9b&&commits[0][6]==ME&&commits[0][7]==1&&c32(0,0)==0x800011);
        ++cases;
    }
    /* Pickup: a random opponent's item consumed this turn, unless held or locked. */
    for(unsigned bits=0;bits<512;++bits)for(uint32_t rv=0;rv<3;++rv) {
        reset();nlist=3;list[0]=4;list[1]=5;list[2]=6;list_count=bits&2?3:0;
        for(unsigned i=0;i<3;++i){mons[4+i].turn_flag=bits>>(2+i)&1;w16(NULL,MON+0x100*(4+i)+0x14,(uint16_t)(bits>>(5+i)&1?0x30+i:0));}
        w16(NULL,MON+0x212,bits&0x100?0x44:0);w8(NULL,BUFFER+0xd,0x77);locked=rv==2;random_value=rv;run(px_battle_pickup,bits&1?ME:3);
        want[0]=0;
        if(bits&1&&!(bits&0x100)){strcat(want,"opp2 bufe list230 ");
            if(list_count){uint32_t cand[3],nc=0;
                for(unsigned i=0;i<3;++i){cat(want,sizeof want,"tf%u/8 ",4+i,0);if(mons[4+i].turn_flag&&(bits>>(5+i)&1))cand[nc++]=4+i;}
                if(nc){cat(want,sizeof want,"rnd%u ",nc,0);const uint32_t from=cand[rv%nc];cat(want,sizeof want,"lock182/%x ",0x30+from-4,0);
                    if(!locked)cat(want,sizeof want,"rec21/2 msg1ea/c2 arg2 arg%x commit ",0x30+from-4,0);
                    if(ncommit)CHECK(commits[0][9]==from&&commits[0][6]==ME&&commits[0][8]==1&&c16(0,4)==0x30+from-4&&c32(0,0)==0x800011);}}}
        CHECK(!strcmp(log_,want));
        if(bits&1&&!(bits&0x100))CHECK(mem[BUFFER+0xc]==list_count&&mem[BUFFER+0xd]==0);
        ++cases;
    }
    /* Pickup: the whole count word decides, the stored byte bounds the loop. */
    reset();nlist=0;list_count=0x100;run(px_battle_pickup,ME);CHECK(!strcmp(log_,"opp2 bufe list230 "));++cases;
    /* Pickup: a candidate whose item vanished. */
    {
        reset();nlist=1;list[0]=4;list_count=1;mons[4].turn_flag=1;w16(NULL,MON+0x414,0x30);w8(NULL,MON+0x419,7);run(px_battle_pickup,ME);
        CHECK(!strcmp(log_,"opp2 bufe list230 tf4/8 rnd1 "));++cases;
    }
    /* Bad Dreams: every sleeping opponent loses 1/8, one popup. */
    for(unsigned bits=0;bits<32;++bits) {
        reset();nlist=3;list[0]=4;list[1]=5;list[2]=6;list_count=bits&2?3:1;
        for(unsigned i=0;i<3;++i){mons[4+i].asleep=bits>>(2+i)&1;mons[4+i].max_hp=40*(i+1)+i;}
        run(px_battle_bad_dreams,bits&1?ME:3);
        want[0]=0;unsigned k=0;
        if(bits&1){strcat(want,"opp2 list130 ");
            for(unsigned i=0;i<list_count;++i){cat(want,sizeof want,"hs%u/2 ",4+i,0);
                if(mons[4+i].asleep){if(!k)strcat(want,"pop1/2 ");cat(want,sizeof want,"rec6/2 v%u/e msg1e4/c2 arg%x commit ",4+i,4+i);
                    CHECK(commits[k][6]==4+i&&c16(k,4)==mons[4+i].max_hp/8&&c32(k,0)==0x11);++k;}}
            if(k)strcat(want,"pop2/2 ");}
        CHECK(!strcmp(log_,want)&&ncommit==k);++cases;
    }
    /* Slow Start: five turns with a queued action not of kind 3, then the message and the work words. */
    for(uint32_t turns=0;turns<8;++turns)for(unsigned bits=0;bits<32;++bits) {
        static const uint32_t kinds[4]={0x13,3,0x23,0};
        reset();w32(NULL,WORK+4,turns);w32(NULL,WORK+8,bits&2?1:0);w32(NULL,WORK+0xc,7);queued=bits>>2&1;action_word=kinds[bits>>3&3];
        run(px_battle_slow_start_turn,bits&1?ME:3);
        want[0]=0;uint32_t after=turns;int done=0;
        if(bits&1&&!(bits&2)){
            if(turns<5){strcat(want,"qa2 ");if(queued&&(action_word&0xf)!=3){after=turns+1;done=after>=5;}}else done=1;
            if(done)strcat(want,"rec3/2 msg1f3/42 arg2 commit ");}
        CHECK(!strcmp(log_,want)&&r32(NULL,WORK+4)==after&&r32(NULL,WORK+8)==(done?1u:(bits&2?1u:0u))&&r32(NULL,WORK+0xc)==(done?0u:7u));
        ++cases;
    }
    /* Shed Skin: a status cured 33% (or forced). */
    for(unsigned bits=0;bits<16;++bits) {
        reset();mons[ME].status=bits&2?4:0;chance_value=bits>>2&1;forced_value=bits>>3&1;run(px_battle_shed_skin,bits&1?ME:3);
        want[0]=0;
        if(bits&1){strcat(want,"st2 ");if(bits&2){strcat(want,"ch33 ");if(!chance_value)strcat(want,"fo1 ");if(chance_value||forced_value)strcat(want,"reca/2 commit ");}}
        CHECK(!strcmp(log_,want));if(ncommit)cure_ok(0,ME);++cases;
    }
    /* Healer: each other ally with a status, 30% each. */
    for(unsigned bits=0;bits<128;++bits) {
        reset();nlist=3;list[0]=ME;list[1]=3;list[2]=1;list_count=bits&2?3:2;mons[ME].status=1;
        mons[3].status=bits>>2&1;mons[1].status=bits>>3&1?6:0;chance_value=bits>>4&1;forced_value=bits>>5&1;
        if(bits>>6&1){list[0]=3;list[1]=ME;}
        run(px_battle_healer,bits&1?ME:3);
        want[0]=0;unsigned k=0;
        if(bits&1){strcat(want,"pos2 list431 ");
            for(unsigned i=0;i<list_count;++i){if(list[i]==ME)continue;cat(want,sizeof want,"st%u ",list[i],0);if(!mons[list[i]].status)continue;
                strcat(want,"ch30 ");if(!chance_value)strcat(want,"fo1 ");if(!chance_value&&!forced_value)continue;
                strcat(want,"reca/2 commit ");cure_ok(k++,list[i]);}}
        CHECK(!strcmp(log_,want)&&ncommit==k);
        ++cases;
    }
    /* Levitate (event 0x7f): the work word is cleared for its battler. */
    for(unsigned who=0;who<2;++who){reset();w32(NULL,WORK,0x55);run(px_battle_levitate_turn,who?ME:3);CHECK(r32(NULL,WORK)==(who?0u:0x55u)&&log_[0]==0);++cases;}
    /* Through the dispatcher: Speed Boost on event 0x80 and Levitate on 0x7f. */
    {
        reset();mons[ME].battle_flag=1;
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7c00+0x10*i;
            w8(NULL,entries,i?0x7f:0x80);w32(NULL,entries+4,i);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,ME);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);w32(NULL,HANDLER+0x40+0x1c,9);frame(ME);px_battle_event_dispatch(CTX,0x80,&events);
        CHECK(!strcmp(log_,"bf2/0 recd/2 commit ")&&aborts==0&&r32(NULL,HANDLER+0x40+0x1c)==9);
        frame(ME);px_battle_event_dispatch(CTX,0x7f,&events);CHECK(log_[0]==0&&r32(NULL,HANDLER+0x40+0x1c)==0&&aborts==0);cases+=2;
    }
    printf("battle_abilities_3: %u cases, all tests passed\n",cases);
    return 0;
}
