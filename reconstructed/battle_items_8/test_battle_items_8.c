#include "battle_items_8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000,
       HANDLER=0x6000, WORK=0x601c, RECORD=0x7000, CURES=0x7800, SCRATCH=0x7900, ENTRIES=0x7a00 };
enum { RUNNING=0x1000000, REENTRY=0x10000000, REGISTERED=0x20000000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
/* Per battler: HP, max HP. Answers: status 0xf, item parameter, contact, stage room, gravity, excluded. */
static uint32_t hp[8],maxhp[8],blockedf,param,contact,room,gravity,excluded;static unsigned aborts,records;static char log_[512];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static uint32_t who(uint32_t p) { CHECK(p>=MON&&p<MON+0x800&&!(p&0xff));return (p-MON)>>8; }
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k)
{ (void)u;notef("v%x/%u ",k,who(p));CHECK(k==0xd||k==0xe);return k==0xd?hp[who(p)]:maxhp[who(p)]; }
static const struct px_battle_ability_api base={.events=&events,.pokemon_value=pokemon_value};
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;notef("hs%x/%u ",s,who(p));return blockedf; }
static uint32_t item_param(void *u,uint32_t item,uint32_t k) { (void)u;CHECK(k==3);notef("ip%x ",item,0);return param; }
static void popup(void *u,uint32_t c,uint32_t p,uint32_t id) { (void)u;CHECK(c==CTX);notef("pop%u/%u ",p,id); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id)
{ (void)u;CHECK(c==CTX&&records<2);notef("rec%x/%x ",k,id);return RECORD+0x100*records++; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;char t[48];snprintf(t,sizeof t,"msg%x@%x/%x ",id,(r-RECORD)&0xff,k);note(t); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;notef("arg%x@%x ",v,(r-RECORD)&0xff); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r>=RECORD&&!(r&0xff));notef("commit%u ",(r-RECORD)>>8,0); }
static const struct px_battle_items_api items={&base,has_status,item_param,popup,action_record,message,message_arg,commit,CURES,SCRATCH};
static const struct px_battle_abilities_1_api abilities={.base=&base};
static uint32_t move_flag(void *u,uint32_t m,uint32_t f) { (void)u;notef("mf%x/%u ",m,f);return contact; }
static uint32_t stage_can_change(void *u,uint32_t p,uint32_t s,int32_t d) { (void)u;notef("sc%u/%u ",s,who(p));CHECK(d==1);return room; }
static uint32_t field_active(void *u,uint32_t c,uint32_t k) { (void)u;CHECK(c==CTX);notef("fa%u ",k,0);return gravity; }
static uint32_t move_excluded(void *u,uint32_t m) { (void)u;notef("mx%x ",m,0);return excluded; }
static void note_berry(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);notef("nb%u ",id,0); }
static const struct px_battle_items_8_api api={&items,&abilities,move_flag,stage_can_change,field_active,move_excluded,note_berry};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_items_8_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_jaboca_check,px_battle_jaboca_rowap_use};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);
    for(unsigned i=0;i<8;++i){ hp[i]=50;maxhp[i]=80; }
    blockedf=0;param=8;contact=1;room=1;gravity=0;excluded=0;aborts=0;records=0;log_[0]=0;
    w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
    w16(NULL,HANDLER+0x38,0x21c);w32(NULL,HANDLER+0x18,RUNNING);
}
/* key 0x02, 0x03, 0x04, 0x47, move, type, effectiveness, category. */
static void frame(uint32_t k2,uint32_t k3,uint32_t k4,uint32_t k47,uint32_t move,uint32_t type,uint32_t eff,uint32_t cat)
{
    px_battle_event_push(&events);px_battle_event_set_id(0x02,k2,&events);px_battle_event_set_id(0x03,k3,&events);
    px_battle_event_set_id(0x04,k4,&events);px_battle_event_set_value(0x47,k47,&events);px_battle_event_set_value(0x12,move,&events);
    px_battle_event_set_value(0x16,type,&events);px_battle_event_set_value(0x39,eff,&events);px_battle_event_set_value(0x1b,cat,&events);log_[0]=0;
}
static void only(uint32_t at,const uint32_t *o,unsigned n)
{
    for(uint32_t k=0;k<0x100;++k){ int listed=0;for(unsigned i=0;i<n;++i)listed|=o[i]==k;if(!listed)CHECK(r8(NULL,at+k)==0); }
}
static uint32_t share(uint32_t m,uint32_t d) { const uint32_t s=m/d;return s?s:1; }
static unsigned cases;
static void expect(const char *s) { if(strcmp(log_,s)){fprintf(stderr,"got  [%s]\nwant [%s]\n",log_,s);} CHECK(!strcmp(log_,s));++cases; }
int main(void)
{
    /* Rocky Helmet: the holder hit, key 0x47, contact; the attacker's share by parameter 3's low byte. */
    {
        const uint32_t params[]={6,8,0x106,0xffffff03};const uint32_t maxima[]={0,5,6,80,600};
        for(uint32_t h4=2;h4<4;++h4)for(uint32_t k47=0;k47<2;++k47)for(uint32_t c=0;c<2;++c)for(unsigned k=0;k<4;++k)for(unsigned mx=0;mx<5;++mx)for(unsigned mv=0;mv<2;++mv){
            const uint32_t moves[]={0x21,0x10021};
            reset();param=params[k];maxhp[5]=maxima[mx];contact=c;frame(7,0x105,h4,k47,moves[mv],0,7,1);
            px_battle_rocky_helmet(HANDLER,CTX,2,WORK,&api);
            char b[128]="";
            if(h4==2&&!k47){ strcat(b,"mf21/0 ");if(c)strcat(b,"rec6/2 ip21c ve/5 msg1a8@c/2 arg5@c commit0 "); }
            expect(b);
            if(h4==2&&!k47&&c){ CHECK(r8(NULL,RECORD+6)==5&&r16(NULL,RECORD+4)==(share(maxima[mx],params[k]&0xff)&0xffff));const uint32_t o[]={4,5,6};only(RECORD,o,3); }
            else CHECK(records==0);
        }
    }
    /* Air Balloon: the announcement (gravity), floating (key 0x53) and popping. */
    {
        for(uint32_t holder=1;holder<4;++holder)for(uint32_t g=0;g<2;++g){
            reset();gravity=g;frame(holder,7,7,0,0,0,7,0);px_battle_air_balloon_announce(HANDLER,CTX,2,WORK,&api);
            expect(holder!=2?"":g?"fa2 ":"fa2 rec3/2 msg198@4/2 arg2@4 commit0 ");
            if(holder==2&&!g){ const uint32_t o[]={0};only(RECORD,o,0); }
            reset();frame(holder,7,7,0,0,0,7,0);px_battle_event_set_value(0x53,0,&events);px_battle_air_balloon_float(HANDLER,CTX,2,WORK,&api);
            CHECK(px_battle_event_get(0x53,&events)==(holder==2));++cases;
            reset();memset(mem+RECORD,0xaa,0x10);frame(7,7,holder,1,0,0,7,0);px_battle_air_balloon_pop(HANDLER,CTX,2,WORK,&api);
            expect(holder!=2?"":"rec21/2 msg19b@c/2 arg2@c commit0 ");
            if(holder==2)CHECK(r16(NULL,RECORD+4)==0&&r8(NULL,RECORD+6)==2&&r8(NULL,RECORD+0xa)==1);
        }
    }
    /* The stat-item checks: holder, key 0x47, the type word, the stage answer. */
    {
        static const fn h[]={px_battle_absorb_bulb_check,px_battle_cell_battery_check,px_battle_snowball_check,px_battle_luminous_moss_check};
        const uint32_t type[]={0xa,0xc,0xe,0xa};const uint32_t stat[]={3,1,1,4};
        for(unsigned i=0;i<4;++i)for(uint32_t t=0;t<0x13;++t)for(uint32_t h4=2;h4<4;++h4)for(uint32_t k47=0;k47<2;++k47)for(uint32_t r=0;r<2;++r){
            reset();room=r;frame(7,7,h4,k47,0x21,t,7,1);h[i](HANDLER,CTX,2,WORK,&api);
            char b[64]="";const int reach=h4==2&&!k47&&t==type[i];
            if(reach){ snprintf(b,sizeof b,"sc%u/2 %s",stat[i],r?"pop0/2 ":""); }
            expect(b);CHECK(r32(NULL,HANDLER+0x18)==(reach&&r?RUNNING|REENTRY:(uint32_t)RUNNING));
        }
        reset();frame(7,7,2,0,0x21,0x10a,7,1);px_battle_absorb_bulb_check(HANDLER,CTX,2,WORK,&api);expect("");
    }
    /* The stat uses and Weakness Policy's two records. */
    {
        static const fn h[]={px_battle_absorb_bulb_use,px_battle_attack_raise_use,px_battle_luminous_moss_use};
        const uint32_t stat[]={3,1,4};
        for(unsigned i=0;i<3;++i)for(uint32_t holder=0;holder<5;++holder){
            reset();frame(holder,7,7,0,0,0,7,0);h[i](HANDLER,CTX,2,WORK,&api);
            if(holder!=2){ expect("");continue; }
            expect("recd/2 commit0 ");CHECK(r8(NULL,RECORD+4)==stat[i]&&r8(NULL,RECORD+0xc)==2&&r8(NULL,RECORD+0x16)==1&&r8(NULL,RECORD+0x17)==1);
            const uint32_t o[]={4,0xc,0x16,0x17};only(RECORD,o,4);
        }
        for(uint32_t holder=0;holder<5;++holder){
            reset();frame(holder,7,7,0,0,0,7,0);px_battle_weakness_policy_use(HANDLER,CTX,2,WORK,&api);
            if(holder!=2){ expect("");continue; }
            expect("recd/2 commit0 recd/2 commit1 ");
            CHECK(r8(NULL,RECORD+4)==1&&r8(NULL,RECORD+0x104)==3&&r8(NULL,RECORD+0x17)==2&&r8(NULL,RECORD+0x117)==2);
            const uint32_t o[]={4,0xc,0x16,0x17};only(RECORD,o,4);only(RECORD+0x100,o,4);
        }
    }
    /* Weakness Policy, Jaboca and Rowap checks: the attacker + 1 into work word 0 and a kind-0 record. */
    {
        const uint32_t effs[]={0,7,8,0x10,0xffffffff,0x80000000};
        for(unsigned e=0;e<6;++e)for(uint32_t h4=2;h4<4;++h4)for(uint32_t k47=0;k47<2;++k47){
            reset();w32(NULL,RECORD+4,0x100);frame(7,0x104,h4,k47,0x21,0,effs[e],1);px_battle_weakness_policy_check(HANDLER,CTX,2,WORK,&api);
            const int hit=h4==2&&!k47&&(int32_t)effs[e]>7;
            expect(hit?"rec0/2 commit0 ":"");
            CHECK(r32(NULL,WORK)==(hit?0x105u:0u)&&r32(NULL,RECORD+4)==(hit?0x102u:0x100u)&&r32(NULL,HANDLER+0x18)==(hit?RUNNING|REENTRY:(uint32_t)RUNNING));
        }
        for(unsigned berry=0;berry<2;++berry)for(uint32_t cat=0;cat<4;++cat)for(uint32_t x=0;x<2;++x)for(uint32_t h4=2;h4<4;++h4)for(uint32_t k47=0;k47<2;++k47){
            reset();excluded=x;frame(7,3,h4,k47,0x10021,0,7,cat);
            if(berry)px_battle_rowap_check(HANDLER,CTX,2,WORK,&api); else px_battle_jaboca_check(HANDLER,CTX,2,WORK,&api);
            const int reach=h4==2&&!k47;const int hit=reach&&!x&&cat==berry+1;
            char b[48]="";if(reach){ strcat(b,"mx21 ");if(hit)strcat(b,"rec0/2 commit0 "); }
            expect(b);CHECK(r32(NULL,WORK)==(hit?4u:0u));
        }
    }
    /* The resist berries' event 0x53: a positive work word, the holder. */
    {
        const uint32_t works[]={0,1,2,0x7fffffff,0x80000000,0xffffffff};
        for(unsigned w=0;w<6;++w)for(uint32_t h4=2;h4<4;++h4){
            reset();w32(NULL,WORK,works[w]);frame(7,7,h4,1,0,0,7,0);px_battle_resist_berry_eaten(HANDLER,CTX,2,WORK,&api);
            const int hit=(int32_t)works[w]>0&&h4==2;
            expect(hit?"nb2 rec24/2 msgdb@8/2 arg2@8 arg21c@8 commit0 ":"");
            CHECK(r32(NULL,WORK)==(hit?0u:works[w]));
            if(hit){ CHECK(r8(NULL,RECORD+4)==1);const uint32_t o[]={4};only(RECORD,o,1); }
        }
    }
    /* Enigma Berry: the check and the restore. */
    {
        const uint32_t effs[]={7,8,0xffffffff};
        for(unsigned e=0;e<3;++e)for(uint32_t h4=2;h4<4;++h4)for(uint32_t k47=0;k47<2;++k47)for(uint32_t b=0;b<2;++b){
            reset();blockedf=b;frame(7,7,h4,k47,0,0,effs[e],0);px_battle_enigma_check(HANDLER,CTX,2,WORK,&api);
            const int reach=h4==2&&!k47&&(int32_t)effs[e]>7;
            char s[48]="";if(reach){ strcat(s,"hsf/2 ");if(!b)strcat(s,"pop0/2 "); }
            expect(s);
        }
        const uint32_t params[]={4,0x104,1};
        for(unsigned k=0;k<3;++k)for(uint32_t mx=0;mx<200;mx+=13)for(uint32_t holder=2;holder<4;++holder){
            reset();param=params[k];maxhp[2]=mx;frame(holder,7,7,0,0,0,7,0);px_battle_enigma_use(HANDLER,CTX,2,WORK,&api);
            if(holder!=2){ expect("");continue; }
            expect("ip21c rec4/2 ve/2 commit0 ");CHECK(r16(NULL,RECORD+4)==share(mx,params[k]&0xff)&&r8(NULL,RECORD+6)==2);
            const uint32_t o[]={4,5,6};only(RECORD,o,3);
        }
    }
    /* Jaboca and Rowap's use: the remembered attacker, fainted or not. */
    {
        const uint32_t works[]={0,1,4,6,0x105};
        for(unsigned w=0;w<5;++w)for(uint32_t f=0;f<2;++f)for(uint32_t holder=2;holder<4;++holder){
            reset();w32(NULL,WORK,works[w]);const uint32_t t=(works[w]-1)&0xff;if(works[w]&&t<8)hp[t]=f?0:30;maxhp[t<8?t:0]=96;
            frame(holder,7,7,0,0,0,7,0);px_battle_jaboca_rowap_use(HANDLER,CTX,2,WORK,&api);
            char b[160]="";
            if(holder==2&&works[w]){ char c[64];snprintf(c,sizeof c,"vd/%u ",t);strcat(b,c);
                if(!f){ snprintf(c,sizeof c,"ip21c rec6/2 ve/%u msg40d@c/2 arg%x@c arg2@c arg21c@c commit0 ",t,t);strcat(b,c); } }
            expect(b);
            if(holder==2&&works[w]&&!f){ CHECK(r8(NULL,RECORD+6)==t&&r16(NULL,RECORD+4)==12);const uint32_t o[]={4,5,6};only(RECORD,o,3); }
        }
    }
    /* Sweeps: max HP 0..2,000 and parameters 1..40 for the three HP shares. */
    {
        for(uint32_t d=1;d<=40;++d)for(uint32_t mx=0;mx<=2000;mx+=3){
            reset();param=d;maxhp[3]=mx;frame(7,3,2,0,0x21,0,7,1);px_battle_rocky_helmet(HANDLER,CTX,2,WORK,&api);
            CHECK(r16(NULL,RECORD+4)==share(mx,d));++cases;
            reset();param=d;maxhp[2]=mx;frame(2,7,7,0,0,0,7,0);px_battle_enigma_use(HANDLER,CTX,2,WORK,&api);
            CHECK(r16(NULL,RECORD+4)==share(mx,d));++cases;
            reset();param=d;maxhp[4]=mx;w32(NULL,WORK,5);frame(2,7,7,0,0,0,7,0);px_battle_jaboca_rowap_use(HANDLER,CTX,2,WORK,&api);
            CHECK(r16(NULL,RECORD+4)==share(mx,d)&&r8(NULL,RECORD+6)==4);++cases;
        }
    }
    /* Through the dispatcher: Jaboca's check on event 0x54 remembers the attacker, its use on 0x7b. */
    {
        reset();w32(NULL,HANDLER+0x18,REGISTERED|1u<<16);
        w8(NULL,ENTRIES,0x54);w32(NULL,ENTRIES+4,0);w8(NULL,ENTRIES+8,0x7b);w32(NULL,ENTRIES+12,1);
        w32(NULL,HANDLER+8,ENTRIES);w8(NULL,HANDLER+0x3a,2);w32(NULL,BATTLE+0x60,HANDLER);
        frame(2,5,2,0,0x21,0,7,1);px_battle_event_dispatch(CTX,0x54,&events);expect("mx21 rec0/2 commit0 ");
        CHECK(r32(NULL,WORK)==6&&r32(NULL,HANDLER+0x18)==(REGISTERED|1u<<16|(uint32_t)RUNNING));
        records=0;w32(NULL,HANDLER+0x18,REGISTERED|1u<<16);w32(NULL,HANDLER+8,ENTRIES+8);log_[0]=0;px_battle_event_dispatch(CTX,0x7b,&events);
        expect("vd/5 ip21c rec6/2 ve/5 msg40d@c/2 arg5@c arg2@c arg21c@c commit0 ");CHECK(r16(NULL,RECORD+4)==10&&aborts==0);
    }
    printf("battle_items_8: %u cases, all tests passed\n",cases);
    return 0;
}
