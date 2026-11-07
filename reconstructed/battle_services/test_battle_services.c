#include "battle_services.h"
#include "../battle_ability/battle_ability.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n  log: %s\n",__FILE__,__LINE__,#c,log_);exit(1); } } while (0)
enum { SIZE=0x10000, CTX=0x1000, WORD=CTX+0x437c, SETUP=0x6800, MON=0x7000, FIELD=0x9000, OUT=0x9800,
       STACK=0xb000, BATTLE=0xc000, POOL=0xc100, SCRATCH=0xc800, HANDLER=0xd000, ENTRIES=0xd400, STATE=0x5555 };
static uint8_t mem[SIZE];static char log_[4096];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static unsigned faults;
static struct { uint32_t address,value; } rets[16];static unsigned nrets;
static void ret(uint32_t a,uint32_t v) { rets[nrets].address=a;rets[nrets++].value=v; }
static uint32_t call(void *u,uint32_t a,const uint32_t *args,unsigned n)
{
    (void)u;char t[32];snprintf(t,sizeof t,"%x(",a);note(t);
    for(unsigned i=0;i<n;++i){snprintf(t,sizeof t,i?",%x":"%x",args[i]);note(t);}
    note(") ");
    for(unsigned i=0;i<nrets;++i)if(rets[i].address==a)return rets[i].value;
    if(a==0x1086b284)return r32(NULL,args[0]);
    return 0;
}
static void fault(void *u) { (void)u;++faults;note("FAULT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
/* Handlers: 0 event 0x63 (sets key 0x21 to adjust), 1 event 0x64 (key 0x42), 2 event 0x65, 3 event 0x66,
 * 4 event 0x6e (keys 0x42, 0x44), 5 event 0x70. */
static int32_t adjust;static uint32_t block64,block6e,show6e;
static const struct px_battle_event_api events;
static void handler(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;(void)h;(void)c;(void)v;(void)w;char t[80];
    const struct px_battle_event_api *e=&events;
    switch(f){
    case 0:snprintf(t,sizeof t,"ev63[%x,%x,%x,%x] ",px_battle_event_get(2,e),px_battle_event_get(3,e),px_battle_event_get(0x20,e),px_battle_event_get(0x21,e));
        if(adjust){px_battle_event_rewrite(0x21,(uint32_t)adjust,e);}
        break;
    case 1:snprintf(t,sizeof t,"ev64[%x,%x,%x,%x,%x] ",px_battle_event_get(2,e),px_battle_event_get(3,e),px_battle_event_get(0x20,e),
                    px_battle_event_get(0x21,e),px_battle_event_get(0x1a,e));if(block64)px_battle_event_rewrite(0x42,1,e);break;
    case 2:snprintf(t,sizeof t,"ev65[%x,%x] ",px_battle_event_get(2,e),px_battle_event_get(0x1a,e));w32(NULL,WORD,0xbad);break;
    case 3:snprintf(t,sizeof t,"ev66[%x,%x,%x,%x] ",px_battle_event_get(3,e),px_battle_event_get(2,e),px_battle_event_get(0x20,e),px_battle_event_get(0x21,e));w32(NULL,WORD,0xbad);break;
    case 4:snprintf(t,sizeof t,"ev6e[%x,%x,%x,%x] ",px_battle_event_get(3,e),px_battle_event_get(4,e),px_battle_event_get(0x1e,e),px_battle_event_get(0x44,e));
        w32(NULL,WORD,0xbad);
        if(block6e){px_battle_event_rewrite(0x42,1,e);}
        if(!show6e){px_battle_event_rewrite(0x44,0,e);}
        break;
    default:snprintf(t,sizeof t,"ev70[%x,%x,%x] ",px_battle_event_get(4,e),px_battle_event_get(3,e),px_battle_event_get(0x1e,e));break;
    }
    note(t);
}
static const struct px_battle_event_api events={&memory,NULL,fault,overflow,blocked,handler,STACK,BATTLE,POOL};
static const struct px_battle_process_api api={&events,call,SCRATCH};
static void reset(void)
{
    memset(mem,0,sizeof mem);log_[0]=0;nrets=0;faults=0;adjust=0;block64=block6e=0;show6e=1;
    w32(NULL,CTX+4,SETUP);w32(NULL,CTX+0xc,STATE);
    for(unsigned i=0;i<6;++i){w8(NULL,MON+0x100*i+0x19,(uint8_t)(0x10+i));w32(NULL,MON+0x100*i,0x900+i);}
    static const uint8_t ev[6]={0x63,0x64,0x65,0x66,0x6e,0x70};
    for(unsigned i=0;i<6;++i){
        const uint32_t h=HANDLER+0x40*i,e=ENTRIES+0x10*i;
        w8(NULL,e,ev[i]);w32(NULL,e+4,i);
        w32(NULL,h+8,e);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x10,7);
        w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<5?h+0x40:0);
    }
    w32(NULL,BATTLE+0x60,HANDLER);
}
#define OK(s) (strcmp(log_,(s))==0)
#define P(i) (MON+0x100*(i))

int main(void)
{
    unsigned cases=0;
    /* Stages: every stat and stage, both directions. */
    for(uint32_t stat=0;stat<9;++stat)for(int32_t stage=-1;stage<=13;++stage)for(int32_t d=-1;d<=1;++d){
        reset();if(stat>=1&&stat<=7)w8(NULL,P(1)+0x103+stat,(uint8_t)stage);
        const uint32_t r=px_battle_stage_can_change(P(1),stat,d,&api);
        const int32_t s=(stat>=1&&stat<=7)?stage:0;
        CHECK(r==(uint32_t)(d>0?s<12:s>0)&&faults==(stat<1||stat>7));++cases;
    }
    for(uint32_t stat=0;stat<9;++stat)for(uint32_t v=0;v<15;++v){
        reset();px_battle_set_stage(P(1),stat,v,&api);
        const int ok=stat>=1&&stat<=7&&v<=12;
        CHECK(faults==!ok);if(ok)CHECK(mem[P(1)+0x103+stat]==v);++cases;
    }
    /* Values: keys 8-0xc, 9 and 0xb exchanged under the field effect. */
    for(uint32_t f=0;f<3;++f)for(uint32_t key=6;key<0xe;++key){
        reset();if(f){w32(NULL,P(1)+0x210,FIELD);w32(NULL,FIELD+0x28c,f-1);}
        const uint32_t k=px_battle_value_key(P(1),key,&api);
        const uint32_t want=f==2&&key==9?0xb:f==2&&key==0xb?9:key;
        CHECK(k==want);
        px_battle_set_value(P(1),key,0x1234,&api);
        if(want>=8&&want<=0xc)CHECK(r16(NULL,P(1)+0xf6+2*(want-8))==0x1234&&faults==0);else CHECK(faults==1);
        ++cases;
    }
    /* HP loss: the setup bit, saturation, commands 1 and 2. */
    for(uint32_t v=0;v<16;++v){
        reset();ret(0x108007e4,v&1);w32(NULL,SETUP+0x10,0x77);w16(NULL,P(2)+0x10,30);
        const uint32_t amount=v&2?40:(v&4?0x10005:5);
        px_battle_lose_hp(CTX,P(2),amount,v>>3&1,&api);
        if(v&1){CHECK(OK("108007e4(77,4) ")&&r16(NULL,P(2)+0x10)==30);++cases;continue;}
        char want[160];
        if(amount==40)snprintf(want,sizeof want,"108007e4(77,4) 108003fc(902) 1086c22c(5555,%u,12,28) ",v>>3&1?2:1);
        else snprintf(want,sizeof want,"108007e4(77,4) 1086c22c(5555,%u,12,5) ",v>>3&1?2:1);
        CHECK(OK(want)&&r16(NULL,P(2)+0x10)==(amount==40?0:25));++cases;
    }
    reset();w16(NULL,P(2)+0x10,5);px_battle_lose_hp(CTX,P(2),5,0,&api);CHECK(r16(NULL,P(2)+0x10)==0&&strstr(log_,"108003fc"));++cases;   /* exactly to 0 */
    /* HP change: restore with cap and 16-bit wrap, loss, report. */
    for(uint32_t v=0;v<16;++v){
        reset();w16(NULL,P(3)+0x10,50);w16(NULL,P(3)+0xe,100);if(v&4)ret(PX_BATTLE_POSITION,0xa);
        const int32_t amount=(v&3)==0?30:(v&3)==1?80:(v&3)==2?-20:0;
        px_battle_change_hp(CTX,P(3),amount,v>>3&1,&api);
        char want[200]="";
        if(amount>0){char t[64];snprintf(t,sizeof t,"1086c22c(5555,3,13,%x) ",(uint32_t)amount);strcat(want,t);}
        if(amount<0)strcat(want,"108007e4(0,4) 1086c22c(5555,1,13,14) ");
        if(v>>3&1){strcat(want,"1086c2cc(220c,13) ");if(!(v&4))strcat(want,"1086c22c(5555,48,13) ");}
        CHECK(OK(want));
        CHECK(r16(NULL,P(3)+0x10)==(uint32_t)((v&3)==0?80:(v&3)==1?100:(v&3)==2?30:50));++cases;
    }
    reset();w16(NULL,P(3)+0x10,0x20);w16(NULL,P(3)+0xe,0x100);px_battle_change_hp(CTX,P(3),0x1fff0,0,&api);CHECK(r16(NULL,P(3)+0x10)==0x10);++cases;
    /* Cures: none, major statuses, others; the out word and its bound. */
    for(uint32_t c=0;c<0x28;++c)for(uint32_t o=0;o<2;++o){
        reset();for(uint32_t i=0;i<0x26;++i){w32(NULL,P(1)+0x1c+4*i,0x100+i);w8(NULL,P(1)+0xb4+i,(uint8_t)(0x40+i));}
        px_battle_cure_condition(CTX,P(1),c,o?OUT:0,&api);
        if(c==0){CHECK(OK("")&&r32(NULL,P(1)+0x20)==0x101);++cases;continue;}
        if(o)CHECK(r32(NULL,OUT)==(c<0x26?0x100+c:r32(NULL,P(1)+0x1c+4*c))&&faults==(c>=0x26));
        if(c<6){
            for(uint32_t i=0;i<0x26;++i){const int cleared=(i>=1&&i<=5)||i==9;CHECK(r32(NULL,P(1)+0x1c+4*i)==(cleared?0:0x100+i));
                CHECK(mem[P(1)+0xb4+i]==(cleared?0:0x40+i));}
            CHECK(strstr(log_,"1086c22c(5555,11,11) 1086c2cc(220c,11) 1086c22c(5555,3c,11,0) ")!=NULL);
        }else{
            char t[96];snprintf(t,sizeof t,"1086fd8c(7100,%x) 1086c22c(5555,12,11,%x) ",c,c);CHECK(strstr(log_,t)!=NULL&&r32(NULL,P(1)+0x20)==0x101);
        }
        ++cases;
    }
    reset();ret(PX_BATTLE_POSITION,0xa);px_battle_cure_condition(CTX,P(1),1,0,&api);CHECK(OK("1086c22c(5555,11,11) 1086c2cc(220c,11) "));++cases;
    /* Status blocked: a reason (reported or not), event 0x6e, the announcement. */
    for(uint32_t v=0;v<32;++v){
        reset();ret(0x1082008c,v&1?7:0);block6e=v>>1&1;show6e=v>>2&1;const int8_t rep=(int8_t)(v&8?-1:0);
        const uint32_t src=v&16?0:P(2);mem[0x19]=0xee;w32(NULL,WORD,0x4321);
        const uint32_t r=px_battle_status_blocked(CTX,P(3),src,4,0xabc,9,rep,&api);
        char want[400];
        if(v&1){snprintf(want,sizeof want,"1082008c(1000,7300,4,abc,9) %s",rep?"1081cd58(1000,7300,7,4) ":"");CHECK(OK(want)&&r==1);++cases;continue;}
        snprintf(want,sizeof want,"1082008c(1000,7300,4,abc,9) 1086b284(537c) ev6e[%x,13,4,%x] ",src?0x12:0x1f,rep?0xffffffffu:0);
        const int shown=(v>>2&1)&&rep;
        if(block6e&&shown){char t[64];snprintf(t,sizeof t,"ev70[13,%x,4] ",src?0x12:0xee);strcat(want,t);}
        CHECK(OK(want)&&r==(v>>1&1)&&mem[CTX+0x1093]==(block6e&&shown?0x10:0)&&r32(NULL,WORD)==0x4321);++cases;
    }
    /* The stage applier. */
    for(uint32_t v=0;v<512;++v){
        reset();ret(0x10865c4c,v&1);
        const int stuck=v>>1&1;w8(NULL,P(4)+0x104,(uint8_t)(stuck?12:6));
        if(v&4)w16(NULL,P(4)+0x208,1);
        struct px_battle_stage_change ch={.delta=v&8?-1:1,.source_key=0x1f,.low=0x2a,.word=0x77,.report=v>>4&1,.quiet=1,.message=v&32?0xa01c:0,.forced=v>>6&1};
        if(stuck&&(v&8))w8(NULL,P(4)+0x104,0);
        block64=v>>7&1;adjust=v&256?2:0;
        const uint32_t source=v&256?0x14:0x11;
        w32(NULL,WORD,0x4321);
        const uint32_t r=px_battle_apply_stage(CTX,source,P(4),1,&ch,&api);
        if(v&1){CHECK(r==0&&OK("10865c4c(1000) "));++cases;continue;}
        const int32_t delta=adjust?2:ch.delta;
        char want[600],t[200];snprintf(want,sizeof want,"10865c4c(1000) ev63[14,1f,1,%x] ",(uint32_t)ch.delta);
        const int32_t stage=stuck?(ch.delta<0?0:12):6;
        if(delta>0?stage>=12:stage<=0){
            if(ch.report){snprintf(t,sizeof t,"1086b5e0(5555,6a,%x,14,1,ffff0000) ",delta>0?0x99:0xae);strcat(want,t);}
            CHECK(OK(want)&&r==0&&mem[CTX+0x1093]==(ch.report?0x10:0));++cases;continue;
        }
        if((v&4)&&!ch.forced&&source!=0x14){
            if(ch.report)strcat(want,"1086b5e0(5555,6a,d2,14,ffff0000) ");
            CHECK(OK(want)&&r==0);++cases;continue;
        }
        snprintf(t,sizeof t,"ev64[14,%x,1,%x,77] ",source,(uint32_t)delta);strcat(want,t);
        if(block64){
            if(ch.report)strcat(want,"1086b284(537c) ev65[14,77] ");
            CHECK(OK(want)&&r==0&&mem[CTX+0x1093]==(ch.report?0x10:0)&&r32(NULL,WORD)==0x4321);++cases;continue;
        }
        if(ch.message)strcat(want,"1086bbe8(1000,a01c) ");
        snprintf(t,sizeof t,"1081c0a0(1000,7400,1,%x,2a,1) 1086b284(537c) ev66[%x,14,1,%x] ",(uint32_t)delta,source,(uint32_t)delta);strcat(want,t);
        CHECK(OK(want)&&r==1&&r32(NULL,WORD)==0x4321);++cases;
    }
    printf("battle_services: %u cases, all tests passed\n",cases);
    return 0;
}
