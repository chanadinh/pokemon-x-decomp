#include "battle_items_2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, CURES=0x7800, SCRATCH=0x7900, ENTRIES=0x7a00, STATS=0x7b00, BUFFER=0x7c00 };
enum { RUNNING=0x1000000, REENTRY=0x10000000, REGISTERED=0x20000000, P=MON+0x200 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
/* Battler state: HP, max HP, ability, conditions (bit n: condition n), item parameter 3,
 * flavor answer, stats that can rise (bit n: stat n), random answer. */
static uint32_t hp,maxhp,ability,conditions,param,taste,risers,roll;static unsigned aborts,records;static char log_[1024];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k)
{
    (void)u;CHECK(p==P);notef("v%x ",k,0);
    if(k==0xd)return hp;
    if(k==0xe)return maxhp;
    CHECK(k==0x11);return ability;
}
static uint32_t pokemon_status(void *u,uint32_t p) { (void)u;(void)p;CHECK(0);return 0; }
static const struct px_battle_ability_api base={.events=&events,.pokemon_value=pokemon_value,.pokemon_status=pokemon_status};
static uint32_t has_status(void *u,uint32_t p,uint32_t s) { (void)u;CHECK(p==P&&s<32);notef("hs%x ",s,0);return conditions>>s&1; }
static uint32_t item_param(void *u,uint32_t item,uint32_t k) { (void)u;CHECK(k==3);notef("ip%x ",item,0);return param; }
static void popup(void *u,uint32_t c,uint32_t p,uint32_t id) { (void)u;CHECK(c==CTX);notef("pop%u/%u ",p,id); }
/* Records are handed out one after another, 0x100 bytes apart. */
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id)
{ (void)u;CHECK(c==CTX&&records<4);notef("rec%x/%u ",k,id);return RECORD+0x100*records++; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id)
{ (void)u;CHECK(r>RECORD&&r<RECORD+0x400);char t[48];snprintf(t,sizeof t,"msg%x@%x/%x ",id,(r-RECORD)&0xff,k);note(t); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;CHECK(r>RECORD&&r<RECORD+0x400);notef("arg%x@%x ",v,(r-RECORD)&0xff); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r>=RECORD&&r<RECORD+0x400&&!(r&0xff));notef("commit%u ",(r-RECORD)>>8,0); }
static const struct px_battle_items_api items={&base,has_status,item_param,popup,action_record,message,message_arg,commit,CURES,SCRATCH};
static const struct px_battle_abilities_1_api abilities={.base=&base};
static uint32_t flavor_taste(void *u,uint32_t data,uint32_t flavor) { (void)u;CHECK(data==0x5eed1234);notef("ft%u ",flavor,0);return taste; }
static void condition_for(void *u,uint32_t s,uint32_t p,uint32_t out) { (void)u;CHECK(p==P);notef("cf%x@%x ",s,(out-RECORD)&0xff);w32(NULL,out,0xc0de0000|s); }
static uint32_t temp_buffer(void *u,uint32_t c,uint32_t n) { (void)u;CHECK(c==CTX);notef("tb%x ",n,0);return BUFFER; }
static uint32_t stage_can_change(void *u,uint32_t p,uint32_t s,int32_t d) { (void)u;CHECK(p==P&&s<8);notef("sc%u%+d ",s,(uint32_t)d);return risers>>s&1; }
static uint32_t random_below(void *u,uint32_t n) { (void)u;notef("rnd%u ",n,0);CHECK(roll<n);return roll; }
static const struct px_battle_items_2_api api={&items,&abilities,flavor_taste,condition_for,temp_buffer,stage_can_change,random_below,STATS};
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_items_2_api*);
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static const fn table[]={px_battle_figy_check_9c,px_battle_figy_use,px_battle_figy_use_7c,px_battle_stat_check_61,px_battle_starf_use};
    CHECK(f<5);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);hp=10;maxhp=40;ability=0;conditions=0;param=8;taste=0;risers=0;roll=0;aborts=0;records=0;log_[0]=0;
    w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
    w32(NULL,P,0x5eed1234);
    w16(NULL,HANDLER+0x38,0x9e);
    for(unsigned i=0;i<5;++i)w8(NULL,STATS+i,(uint8_t)(i+1));
    memset(mem+BUFFER,0xee,8);
}
static void frame(uint32_t id) { px_battle_event_push(&events);px_battle_event_set_id(0x02,id,&events);log_[0]=0; }
static unsigned cases;
static void expect(const char *s) { if(strcmp(log_,s)){fprintf(stderr,"got  [%s]\nwant [%s]\n",log_,s);} CHECK(!strcmp(log_,s));++cases; }
/* The first batch's pinch reference: HP at most max HP / divisor. */
static int pinch(uint32_t d,int *faulted)
{
    uint32_t e=d;if(ability==0x52&&e>2)e>>=1;
    *faulted=e==0;if(e==0)e=1;
    return maxhp>1&&hp<=maxhp/e;
}
static const char *pinch_log(int faulted)
{
    static char b[48];snprintf(b,sizeof b,"v11 %sve %s",faulted?"ABORT ":"",maxhp>1?"vd ve ":"");return b;
}
/* Every byte of a record outside the listed offsets is untouched (zero). */
static void only(uint32_t record,const uint32_t *offsets,unsigned n)
{
    for(uint32_t k=0;k<0x100;++k){ int listed=0;for(unsigned i=0;i<n;++i)listed|=offsets[i]==k;if(!listed)CHECK(r8(NULL,record+k)==0); }
}
int main(void)
{
    /* The Figy family's checks: work word 0, condition 0xf, key 0x02, key 0x2f and the pinch. */
    {
        const uint32_t works[]={0,1,0x80000000};
        for(unsigned wk=0;wk<3;++wk)for(unsigned blockedf=0;blockedf<2;++blockedf)for(unsigned low=0;low<3;++low)
        for(uint32_t use=0;use<4;++use)for(uint32_t who=2;who<4;++who)for(unsigned glut=0;glut<2;++glut)for(unsigned ev=0;ev<3;++ev){
            reset();ability=glut?0x52:0;conditions=blockedf?1u<<0xf|1:~(1u<<0xf);hp=low==0?20:low==1?21:0;
            w32(NULL,WORK,works[wk]);w32(NULL,HANDLER+0x18,RUNNING);frame(who);px_battle_event_set_value(0x2f,use,&events);
            static const fn h[]={px_battle_figy_check_9c,px_battle_figy_check_5e,px_battle_figy_check_61};
            h[ev](HANDLER,CTX,2,WORK,&api);
            char b[128]="";int f=0;const int ok=pinch(2,&f);
            if(works[wk]==0){
                strcat(b,"hsf ");
                const int reach=!blockedf&&(ev==2||(who==2&&(ev==1||(use&1))));
                if(reach){ strcat(b,pinch_log(f));if(ok)strcat(b,"pop0/2 "); }
            }
            expect(b);
            const int asked=works[wk]==0&&!blockedf&&(ev==2||(who==2&&(ev==1||(use&1))))&&ok;
            CHECK(r32(NULL,HANDLER+0x18)==(asked?RUNNING|REENTRY:(uint32_t)RUNNING)&&r32(NULL,WORK)==works[wk]&&aborts==0&&records==0);
        }
        /* The divisor is 2 whatever the parameter. */
        reset();param=0;hp=21;frame(2);px_battle_figy_check_61(HANDLER,CTX,2,WORK,&api);expect("hsf v11 ve vd ve ");
        reset();param=0;hp=20;frame(2);px_battle_figy_check_5e(HANDLER,CTX,2,WORK,&api);expect("hsf v11 ve vd ve pop0/2 ");
    }
    /* 1084c5e0: work word 0, key 0x02, the share of max HP and the flavor. */
    {
        const uint32_t maxima[]={0,1,7,8,9,40,714,0xffff,0x10000,0x7ffff,0xffffffff};
        const uint32_t divisors[]={1,2,3,8,9,0x10,0xffff,0x80000000,0xffffffff};
        for(unsigned mx=0;mx<11;++mx)for(unsigned d=0;d<9;++d)for(uint32_t t=0;t<4;++t)for(uint32_t flavor=0;flavor<6;++flavor){
            reset();maxhp=maxima[mx];param=divisors[d];taste=t;frame(2);
            px_battle_figy_berry_use(HANDLER,CTX,2,WORK,flavor,&api);
            char b[192];
            snprintf(b,sizeof b,"rec4/2 ip9e ve msg392@8/2 arg2@8 arg9e@8 commit0 ft%u %s",flavor,t==2?"recb/2 cf6@8 commit1 ":"");expect(b);
            const uint32_t share=maxima[mx]/divisors[d];
            CHECK(r16(NULL,RECORD+4)==((share?share:1)&0xffff)&&r8(NULL,RECORD+6)==2);
            { const uint32_t o[]={4,5,6};only(RECORD,o,3); }
            if(t==2){ CHECK(r16(NULL,RECORD+0x104)==6&&r32(NULL,RECORD+0x108)==0xc0de0006&&r8(NULL,RECORD+0x10f)==2);
                const uint32_t o[]={0x4,0x5,0x8,0x9,0xa,0xb,0xf};only(RECORD+0x100,o,7); }
            else CHECK(records==1);
            CHECK(r32(NULL,WORK)==1&&aborts==0);
        }
        /* Already used, or another battler: nothing, and the work word is kept. */
        const uint32_t works[]={1,2,0x100,0x80000000};
        for(unsigned i=0;i<4;++i){ reset();w32(NULL,WORK,works[i]);frame(2);px_battle_figy_berry_use(HANDLER,CTX,2,WORK,0,&api);
            expect("");CHECK(r32(NULL,WORK)==works[i]&&records==0); }
        for(uint32_t who=0;who<8;++who){ if(who==2)continue;reset();frame(who);px_battle_figy_berry_use(HANDLER,CTX,2,WORK,0,&api);
            expect("");CHECK(r32(NULL,WORK)==0&&records==0); }
        /* Another battler id and item. */
        reset();maxhp=80;frame(5);w32(NULL,MON+0x500,0x5eed1234);w16(NULL,HANDLER+0x38,0xa3);taste=2;
        {   /* pokemon_value and has-status checks expect battler 2's data: give battler 5 the same. */
            w32(NULL,TABLE+0x84+4*5,P);
            px_battle_figy_berry_use(HANDLER,CTX,5,WORK,4,&api);
            expect("rec4/5 ipa3 ve msg392@8/2 arg5@8 arga3@8 commit0 ft4 recb/5 cf6@8 commit1 ");
            CHECK(r16(NULL,RECORD+4)==10&&r8(NULL,RECORD+6)==5&&r8(NULL,RECORD+0x10f)==5); }
    }
    /* The five Figy-family berries: their flavor, and event 0x7c's work word 6. */
    {
        const fn use[]={px_battle_figy_use,px_battle_wiki_use,px_battle_mago_use,px_battle_aguav_use,px_battle_iapapa_use};
        const fn use7c[]={px_battle_figy_use_7c,px_battle_wiki_use_7c,px_battle_mago_use_7c,px_battle_aguav_use_7c,px_battle_iapapa_use_7c};
        const uint32_t sixth[]={0,1,0x100,0x80000000};
        for(unsigned i=0;i<5;++i)for(unsigned s=0;s<4;++s)for(unsigned used=0;used<2;++used)for(uint32_t who=2;who<4;++who){
            char b[160];
            snprintf(b,sizeof b,"rec4/2 ip9e ve msg392@8/2 arg2@8 arg9e@8 commit0 ft%u recb/2 cf6@8 commit1 ",i);
            const char *want=used||who!=2?"":b;
            reset();taste=2;w32(NULL,WORK,used);w32(NULL,WORK+0x18,sixth[s]);frame(who);use[i](HANDLER,CTX,2,WORK,&api);
            expect(want);CHECK(r32(NULL,WORK)==(used||who==2?1u:0u)&&r32(NULL,WORK+0x18)==sixth[s]);
            reset();taste=2;w32(NULL,WORK,used);w32(NULL,WORK+0x18,sixth[s]);frame(who);use7c[i](HANDLER,CTX,2,WORK,&api);
            expect(sixth[s]?want:"");CHECK(r32(NULL,WORK)==(used||(who==2&&sixth[s])?1u:0u));
        }
    }
    /* The stat berries' checks: key 0x02, key 0x2f, parameter 3 as the divisor. */
    {
        const uint32_t divisors[]={0,1,2,3,4,5,8,0x100};
        for(unsigned d=0;d<8;++d)for(uint32_t mx=0;mx<10;++mx)for(uint32_t h=0;h<11;++h)for(unsigned glut=0;glut<2;++glut)
        for(uint32_t use=0;use<2;++use)for(uint32_t who=2;who<4;++who)for(unsigned ev=0;ev<3;++ev){
            reset();ability=glut?0x52:0;maxhp=mx;hp=h;param=divisors[d];conditions=~0u;
            w32(NULL,HANDLER+0x18,RUNNING);frame(who);px_battle_event_set_value(0x2f,use,&events);
            static const fn c[]={px_battle_stat_check_9c,px_battle_stat_check_5e,px_battle_stat_check_61};
            c[ev](HANDLER,CTX,2,WORK,&api);
            int f=0;const int ok=pinch(divisors[d],&f);
            const int reach=ev==2||(who==2&&(ev==1||(use&1)));
            char b[128]="";
            if(ev==2||who==2){ strcat(b,"ip9e ");if(reach){ strcat(b,pinch_log(f));if(ok)strcat(b,"pop0/2 "); } }
            expect(b);
            CHECK(r32(NULL,HANDLER+0x18)==(reach&&ok?RUNNING|REENTRY:(uint32_t)RUNNING)&&aborts==(unsigned)(reach&&f)&&records==0);
        }
    }
    /* Liechi, Ganlon, Salac, Petaya and Apicot: +1 in their stat. */
    {
        const fn use[]={px_battle_liechi_use,px_battle_ganlon_use,px_battle_salac_use,px_battle_petaya_use,px_battle_apicot_use};
        const uint32_t stat[]={1,2,5,3,4};
        for(unsigned i=0;i<5;++i)for(uint32_t who=0;who<6;++who){
            reset();frame(who);use[i](HANDLER,CTX,2,WORK,&api);
            if(who!=2){ expect("");CHECK(records==0);continue; }
            expect("recd/2 commit0 ");
            CHECK(r8(NULL,RECORD+4)==stat[i]&&r8(NULL,RECORD+0xc)==2&&r8(NULL,RECORD+0x16)==1&&r8(NULL,RECORD+0x17)==1);
            const uint32_t o[]={4,0xc,0x16,0x17};only(RECORD,o,4);
        }
        reset();frame(6);w32(NULL,TABLE+0x84+4*6,P);px_battle_salac_use(HANDLER,CTX,6,WORK,&api);expect("recd/6 commit0 ");CHECK(r8(NULL,RECORD+0xc)==6);
    }
    /* Lansat: state 9 (bit 1 of the byte at +0x16a) and key 0x02. */
    {
        for(uint32_t bits=0;bits<256;++bits)for(uint32_t who=2;who<4;++who)for(unsigned around=0;around<2;++around){
            reset();w8(NULL,P+0x16a,(uint8_t)bits);if(around){ w8(NULL,P+0x169,0xff);w8(NULL,P+0x16b,0xff); }
            frame(who);px_battle_lansat_use(HANDLER,CTX,2,WORK,&api);
            if(who!=2||(bits&2)){ expect("");CHECK(records==0);continue; }
            expect("rec17/2 commit0 rec3/2 msg3ef@4/2 arg2@4 arg9e@4 commit1 ");
            CHECK(r8(NULL,RECORD+4)==9&&r8(NULL,RECORD+5)==2);
            const uint32_t o[]={4,5};only(RECORD,o,2);only(RECORD+0x100,o,0);
        }
        reset();w16(NULL,HANDLER+0x38,0xce);frame(4);w32(NULL,TABLE+0x84+4*4,P);px_battle_lansat_use(HANDLER,CTX,4,WORK,&api);
        expect("rec17/4 commit0 rec3/4 msg3ef@4/2 arg4@4 argce@4 commit1 ");CHECK(r8(NULL,RECORD+5)==4);
    }
    /* Starf: every set of stats that can rise, every roll, key 0x02 after the roll. */
    {
        for(uint32_t set=0;set<64;++set)for(uint32_t who=2;who<4;++who)for(unsigned order=0;order<2;++order){
            uint8_t table[5];uint32_t allowed[5],n=0;
            for(unsigned i=0;i<5;++i){ table[i]=(uint8_t)(order?5-i:i+1); }
            if(order){ table[0]=6;table[4]=0; }   /* any byte is passed on */
            for(unsigned i=0;i<5;++i)if(set>>table[i]&1)allowed[n++]=table[i];
            for(uint32_t r=0;r<(n?n:1);++r){
                reset();for(unsigned i=0;i<5;++i)w8(NULL,STATS+i,table[i]);risers=set;roll=r;frame(who);
                px_battle_starf_use(HANDLER,CTX,2,WORK,&api);
                char b[192];snprintf(b,sizeof b,"tb5 sc%u+1 sc%u+1 ",table[0],table[1]);
                char c[64];snprintf(c,sizeof c,"sc%u+1 sc%u+1 sc%u+1 ",table[2],table[3],table[4]);strcat(b,c);
                if(n){ snprintf(c,sizeof c,"rnd%u ",n);strcat(b,c);if(who==2)strcat(b,"recd/2 commit0 "); }
                expect(b);
                for(unsigned i=0;i<n;++i)CHECK(r8(NULL,BUFFER+i)==allowed[i]);
                for(unsigned i=n;i<8;++i)CHECK(r8(NULL,BUFFER+i)==0xee);
                if(n&&who==2){ CHECK(r8(NULL,RECORD+4)==allowed[r]&&r8(NULL,RECORD+0xc)==2&&r8(NULL,RECORD+0x16)==1&&r8(NULL,RECORD+0x17)==2);
                    const uint32_t o[]={4,0xc,0x16,0x17};only(RECORD,o,4); }
                else CHECK(records==0);
            }
        }
    }
    /* Through the dispatcher: a Figy check keeps its handler running, the 0x7b use sets the work
     * word, and the check then gives up; Starf on event 0x7b. */
    {
        reset();hp=20;
        w8(NULL,ENTRIES,0x9c);w32(NULL,ENTRIES+4,0);w8(NULL,ENTRIES+8,0x7b);w32(NULL,ENTRIES+12,1);w8(NULL,ENTRIES+16,0x7c);w32(NULL,ENTRIES+20,2);
        w32(NULL,HANDLER+8,ENTRIES);w32(NULL,HANDLER+0x18,REGISTERED|3u<<16);w8(NULL,HANDLER+0x3a,2);
        w32(NULL,BATTLE+0x60,HANDLER);frame(2);px_battle_event_set_value(0x2f,1,&events);px_battle_event_dispatch(CTX,0x9c,&events);
        expect("hsf v11 ve vd ve pop0/2 ");CHECK(r32(NULL,HANDLER+0x18)==(REGISTERED|3u<<16|(uint32_t)RUNNING));
        w32(NULL,HANDLER+0x18,REGISTERED|3u<<16);log_[0]=0;px_battle_event_dispatch(CTX,0x7c,&events);expect("");
        log_[0]=0;px_battle_event_dispatch(CTX,0x7b,&events);
        expect("rec4/2 ip9e ve msg392@8/2 arg2@8 arg9e@8 commit0 ft0 ");CHECK(r32(NULL,HANDLER+0x1c)==1&&r16(NULL,RECORD+4)==5);
        log_[0]=0;px_battle_event_dispatch(CTX,0x9c,&events);expect("");
        reset();risers=1u<<3;
        w8(NULL,ENTRIES,0x7b);w32(NULL,ENTRIES+4,4);w8(NULL,ENTRIES+8,0x61);w32(NULL,ENTRIES+12,3);
        w32(NULL,HANDLER+8,ENTRIES);w32(NULL,HANDLER+0x18,REGISTERED|2u<<16);w8(NULL,HANDLER+0x3a,2);
        w32(NULL,BATTLE+0x60,HANDLER);frame(2);param=4;
        px_battle_event_dispatch(CTX,0x61,&events);expect("ip9e v11 ve vd ve pop0/2 ");
        w32(NULL,HANDLER+0x18,REGISTERED|2u<<16);log_[0]=0;px_battle_event_dispatch(CTX,0x7b,&events);
        expect("tb5 sc1+1 sc2+1 sc3+1 sc4+1 sc5+1 rnd1 recd/2 commit0 ");CHECK(r8(NULL,RECORD+4)==3&&aborts==0);
    }
    printf("battle_items_2: %u cases, all tests passed\n",cases);
    return 0;
}
