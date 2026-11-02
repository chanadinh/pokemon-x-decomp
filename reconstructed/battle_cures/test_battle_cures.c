#include "battle_cures.h"
#include "../battle_action/battle_action.h"
#include "../battle_ability/battle_ability.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n  log: %s\n",__FILE__,__LINE__,#c,log_);exit(1); } } while (0)
enum { SIZE=0x10000, CTX=0x1000, WORD=CTX+0x437c, TEXT=CTX+0x433c, TABLE=0x6000, MON=0x7000, REC=0xa000, LISTW=0x9000, FIELD=0x9800,
       STACK=0xb000, BATTLE=0xc000, POOL=0xc100, SCRATCH=0xc800, FRAMES=0xc900, CURES=0xca00, HANDLER=0xd000, ENTRIES=0xd400, STATE=0x5555 };
static uint8_t mem[SIZE];static char log_[8192];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }

static uint32_t final_field[3],final_set,empty_after;
static uint32_t hp[8],status_seq[8],status_n,noposition,block_a6,side_n;static unsigned faults,routed;
static struct { uint32_t address,value; } rets[24];static unsigned nrets;
static void ret(uint32_t a,uint32_t v) { rets[nrets].address=a;rets[nrets++].value=v; }
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static const struct px_battle_cures_api cures;
static uint32_t call(void *u,uint32_t a,const uint32_t *args,unsigned n)
{
    (void)u;
    if(a==PX_BATTLE_POKEMON_VALUE){CHECK(n==2&&args[1]==0xd);return hp[mon(args[0])];}
    if(a==0x10820904){++routed;return px_battle_kind_cure(args[0],args[1],args[2],&cures);}
    if(a==0x1081f36c){++routed;return px_battle_kind_set_item(args[0],args[1],&cures);}
    char t[32];snprintf(t,sizeof t,"%x(",a);note(t);
    for(unsigned i=0;i<n;++i){snprintf(t,sizeof t,i?",%x":"%x",args[i]);note(t);}
    note(") ");
    if(a==PX_BATTLE_POSITION&&n==2&&args[1]<32)return noposition>>args[1]&1?0xa:args[1]&0xf;
    if(a==0x1086d8ac)return status_n?status_seq[--status_n]:0;   /* the current status, consumed by cures */
    if(a==0x108695c0){w32(NULL,args[3]?args[3]:0,0);if(args[3])w32(NULL,args[3],0x55);if(args[2]<0x26)w32(NULL,args[1]+0x1c+4*args[2],0);return 0;}
    if(a==0x108707d4)return args[1]<6?MON+0x100*args[1]:0;
    if(a==0x108672c0){for(uint32_t i=0;i<side_n;++i)w8(NULL,args[2]+i,(uint8_t)i);return side_n;}
    if(a==0x108225dc){for(unsigned i=0;i<3;++i)final_field[i]=r32(NULL,FIELD+4*i);if(final_set)w8(NULL,FIELD+5,(uint8_t)(r8(NULL,FIELD+5)|1));return 0;}
    if(a==0x10863604&&empty_after){w8(NULL,LISTW+0x6e,0);w8(NULL,LISTW+0x71,(uint8_t)(empty_after==1));w8(NULL,LISTW+0x6f,0);return 0;}
    for(unsigned i=0;i<nrets;++i)if(rets[i].address==a)return rets[i].value;
    if(a==0x1086b284)return r32(NULL,args[0]);
    return 0;
}
static void fault(void *u) { (void)u;++faults;note("FAULT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static const struct px_battle_event_api events;
static void handler(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;(void)h;(void)c;(void)v;(void)w;char t[64];
    switch(f){
    case 0:snprintf(t,sizeof t,"ev9c[%x,%x] ",px_battle_event_get(2,&events),px_battle_event_get(0x2f,&events));w32(NULL,WORD,0xbad);break;
    case 1:snprintf(t,sizeof t,"eva6[%x,%x] ",px_battle_event_get(2,&events),px_battle_event_get(0x2e,&events));if(block_a6)px_battle_event_rewrite(0x42,1,&events);break;
    case 2:snprintf(t,sizeof t,"eva7[%x] ",px_battle_event_get(2,&events));break;
    default:snprintf(t,sizeof t,"ev9b[%x,%x,%x] ",px_battle_event_get(2,&events),px_battle_event_get(0x2e,&events),px_battle_event_get(0x52,&events));break;
    }
    note(t);
}
static const struct px_battle_event_api events={&memory,NULL,fault,overflow,blocked,handler,STACK,BATTLE,POOL};
static const struct px_battle_process_api process={&events,call,SCRATCH};
static const struct px_battle_cures_api cures={&process,CURES,FRAMES};
static void reset(void)
{
    memset(mem,0,sizeof mem);log_[0]=0;nrets=0;faults=0;routed=0;noposition=0;block_a6=0;status_n=0;side_n=0;final_set=0;empty_after=0;
    w32(NULL,CTX+8,TABLE);w32(NULL,CTX+0xc,STATE);w32(NULL,CTX+0xdb4,FIELD);w32(NULL,CTX+0x1268,LISTW);
    for(unsigned i=0;i<6;++i){const uint32_t p=MON+0x100*i;w32(NULL,TABLE+0x84+4*i,p);w8(NULL,p+0x19,(uint8_t)i);w32(NULL,p,0x900+i);hp[i]=5;}
    static const uint16_t list[8]={7,0xc,0xd,0xf,0x17,0xb,0x76,0xd6};
    for(unsigned i=0;i<8;++i)w16(NULL,CURES+2*i,list[i]);
    static const uint8_t ev[4]={0x9c,0xa6,0xa7,0x9b};
    for(unsigned i=0;i<4;++i){
        const uint32_t h=HANDLER+0x40*i,e=ENTRIES+0x10*i;
        w8(NULL,e,ev[i]);w32(NULL,e+4,i);
        w32(NULL,h+8,e);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x10,7);
        w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<3?h+0x40:0);
    }
    w32(NULL,BATTLE+0x60,HANDLER);
}
#define OK(s) (strcmp(log_,(s))==0)
static void cond(uint32_t id,uint32_t c) { w32(NULL,MON+0x100*id+0x1c+4*c,1); }
static uint8_t sizes[2*PX_BATTLE_ACTION_KINDS];
static void execute(void *u,uint32_t c,uint32_t r) { (void)u;px_battle_action_execute(c,r,&process); }
static const struct px_battle_action_api action={&memory,sizes,execute,fault};

int main(void)
{
    unsigned cases=0;
    /* Kind 0xa: a single condition, the current status, status then 6, the list. */
    for(uint32_t v=0;v<64;++v){
        reset();const uint32_t mode=(uint32_t[]){5,0x26,0x27,0x28}[v&3];
        w32(NULL,REC,0xau|1u<<8|(v&4?0x800000u:0));w8(NULL,REC+4,(uint8_t)mode);w8(NULL,REC+5,2);w8(NULL,REC+0x11,1);w8(NULL,REC+0x12,(uint8_t)(v>>3&1));
        if(v&16){status_seq[0]=0x13;status_n=1;}
        if(v&32){cond(2,5);cond(2,6);cond(2,0xd);cond(2,0x17);}
        w16(NULL,MON+0x212,0x44);ret(0x10882188,1);w16(NULL,TEXT+2,0x8085);w32(NULL,TEXT+0x20,9);
        const uint32_t r=px_battle_kind_cure(CTX,REC,0x2a,&cures);
        /* Expected conditions in order. */
        uint32_t want[8],n=0;
        if(mode==5){if(v&32)want[n++]=5;}
        else if(mode==0x26){if(v&16)want[n++]=0x13;}
        else if(mode==0x27){if(v&16){want[n++]=0x13;if(v&32)want[n++]=6;}else if(v&32)want[n++]=6;}
        else{if(v&32){want[n++]=0xd;want[n++]=0x17;}}
        char exp[2000]="",t[200];
        if(v&4)strcat(exp,"1086c22c(5555,66,1) ");
        strcat(exp,"108007cc(902,2) ");
        /* Each cure re-reads the status; one more read ends the loop. */
        for(uint32_t i=0;i<n;++i){
            strcat(exp,"1086d8ac(7200) ");
            snprintf(t,sizeof t,"108695c0(1000,7200,%x,c900) ",want[i]);strcat(exp,t);
            if(v>>3&1)strcat(exp,"1086bbe8(1000,a014) ");
            else{snprintf(t,sizeof t,"10882188(%x,55,7200,2a,%x) 1086bbe8(1000,%x) ",want[i],TEXT,TEXT);strcat(exp,t);}
            if(want[i]==0x13)strcat(exp,"1086b284(537c) ev9c[2,7] ");
        }
        strcat(exp,"1086d8ac(7200) ");
        if(v&4)strcat(exp,"1086c22c(5555,67,1) ");
        CHECK(OK(exp)&&r==(n>0));
        if(n&&!(v>>3&1))CHECK(r32(NULL,TEXT)==0&&r32(NULL,TEXT+0x20)==0);
        ++cases;
    }
    /* Kind 0xa: two targets, one excluded; a condition at the list's end; bounds. */
    reset();w8(NULL,REC+4,0x28);w8(NULL,REC+5,1);w8(NULL,REC+6,2);w8(NULL,REC+0x11,2);w8(NULL,REC+0x12,1);ret(0x108007cc,0);cond(1,0xb);cond(2,7);hp[1]=0;
    CHECK(px_battle_kind_cure(CTX,REC,0,&cures)==1&&strstr(log_,"108695c0(1000,7200,7,c900) ")&&!strstr(log_,"7100,b"));++cases;
    reset();w8(NULL,REC+4,0x28);w8(NULL,REC+5,1);w8(NULL,REC+0x11,1);w8(NULL,REC+0x12,1);cond(1,0xb);CHECK(px_battle_kind_cure(CTX,REC,0,&cures)==1&&strstr(log_,"108695c0(1000,7100,b,c900) "));++cases;
    reset();w8(NULL,REC+4,0x28);w8(NULL,REC+5,1);w8(NULL,REC+0x11,1);w16(NULL,CURES+4,0);cond(1,0xf);CHECK(px_battle_kind_cure(CTX,REC,0,&cures)==0);++cases;   /* a zero ends the list before 0xf */
    reset();w8(NULL,REC+4,0x30);w8(NULL,REC+5,1);w8(NULL,REC+0x11,1);CHECK(px_battle_kind_cure(CTX,REC,0,&cures)==0&&faults==1);++cases;
    reset();w8(NULL,REC+4,0x28);w8(NULL,REC+5,1);w8(NULL,REC+0x11,1);w16(NULL,CURES,0x26);CHECK(px_battle_kind_cure(CTX,REC,0,&cures)==0&&faults==1);++cases;
    reset();w8(NULL,REC+4,5);w8(NULL,REC+5,1);w8(NULL,REC+0x11,1);cond(1,5);ret(0x10882188,0);CHECK(px_battle_kind_cure(CTX,REC,0,&cures)==1&&!strstr(log_,"1086bbe8"));++cases;
    /* Kind 0x21: set an item. */
    for(uint32_t v=0;v<128;++v){
        reset();block_a6=v&1;w32(NULL,REC,0x21u|(v&2?3u:1u)<<8|(v&4?0x800000u:0));w16(NULL,REC+4,(uint16_t)(v&8?0:0x31));w8(NULL,REC+6,3);
        w8(NULL,REC+7,(uint8_t)(v>>4&1));w8(NULL,REC+8,(uint8_t)(v>>5&1));w8(NULL,REC+9,4);w8(NULL,REC+0xa,(uint8_t)(v>>6&1));
        w16(NULL,MON+0x312,0x22);w16(NULL,MON+0x314,0x99);w16(NULL,MON+0x414,0x99);
        ret(0x1086b140,0);
        const uint32_t r=px_battle_kind_set_item(CTX,REC,&cures);
        const uint32_t item=v&8?0:0x31,hid=v&2?3:1;
        char exp[1200]="",t[200];
        if(hid!=3){snprintf(t,sizeof t,"1086b284(537c) eva6[3,%x] ",item);strcat(exp,t);if(v&1){strcat(exp,"1086b284(537c) eva7[3] ");CHECK(OK(exp)&&r==0);++cases;continue;}}
        if(v&4){snprintf(t,sizeof t,"1086c22c(5555,66,%x) ",hid);strcat(exp,t);}
        strcat(exp,"1086bbe8(1000,a00c) ");
        if(v>>4&1)strcat(exp,"1086c22c(5555,2e,3) ");
        snprintf(t,sizeof t,"1086b140(1000,7300,%x,0) ",item);strcat(exp,t);
        if(item==0&&(v>>6&1))strcat(exp,"1086b284(537c) ev9b[3,22,1] ");
        if(v&4){snprintf(t,sizeof t,"1086c22c(5555,67,%x) ",hid);strcat(exp,t);}
        if(v>>5&1)strcat(exp,"1086c22c(5555,2e,4) ");
        strcat(exp,"1086b284(537c) ev9c[3,f] ");   /* the stub leaves the old item in place */
        CHECK(OK(exp)&&r==1&&r16(NULL,MON+0x314)==(v>>4&1?0:0x99)&&r16(NULL,MON+0x414)==(v>>5&1?0:0x99)&&r32(NULL,WORD)==0);++cases;
    }
    /* Kind 0x25: swap items. */
    for(uint32_t v=0;v<16;++v){
        reset();block_a6=v&1;w32(NULL,REC,0x25u|2u<<8|(v&2?0x800000u:0));w8(NULL,REC+4,4);
        w16(NULL,MON+0x412,(uint16_t)(v&4?0x11:0));w16(NULL,MON+0x212,(uint16_t)(v&8?0x22:0));
        const uint32_t r=px_battle_kind_swap_items(CTX,REC,&cures);
        char exp[800],t[100];snprintf(exp,sizeof exp,"1086b284(537c) eva6[4,%x] ",v&8?0x22:0);
        if(v&1){strcat(exp,"1086b284(537c) eva7[4] ");CHECK(OK(exp)&&r==0);++cases;continue;}
        if(v&2)strcat(exp,"1086c22c(5555,66,2) ");
        strcat(exp,"1086bbe8(1000,a008) 1086bbe8(1000,a02c) 1086bbe8(1000,a050) ");
        if(v&2)strcat(exp,"1086c22c(5555,67,2) ");
        snprintf(t,sizeof t,"1086b140(1000,7200,%x,0) 1086b140(1000,7400,%x,0) ",v&4?0x11:0,v&8?0x22:0);strcat(exp,t);
        if(v&8)strcat(exp,"1086b284(537c) ev9c[2,7] ");
        if(v&4)strcat(exp,"1086b284(537c) ev9c[4,7] ");
        CHECK(OK(exp)&&r==1);++cases;
    }
    /* Kind 0x28: use a move; the final phase sees the prepared field and sets the result bit. */
    for(uint32_t v=0;v<32;++v){
        reset();w8(NULL,REC+4,1);w8(NULL,REC+5,2);w8(NULL,REC+6,0x5a);w16(NULL,REC+8,0x123);
        w8(NULL,LISTW+0x6e,(uint8_t)(v&1?0:2));w32(NULL,LISTW,MON+0x300);w32(NULL,LISTW+4,MON+0x400);w8(NULL,LISTW+0x6f,(uint8_t)(v&2?1:0));
        if(v&4){hp[4]=0;}
        if(v&8){noposition|=1u<<3;}
        final_set=v>>4&1;
        w32(NULL,FIELD,0x11223344);w32(NULL,FIELD+4,0x55667788);w32(NULL,FIELD+8,0x99aabbcc);
        const uint32_t r=px_battle_kind_use_move(CTX,REC,&cures);
        char exp[1600]="10869c5c(1000,123,7100,c920) 10869870(0) 10869870(9000) 108696b0(9000,7200) ";
        if(v&1){strcat(exp,"1086b5e0(5555,69,4a,ffff0000) ");CHECK(OK(exp)&&r==0&&r32(NULL,FIELD)==0x11223344);++cases;continue;}
        strcat(exp,"1086c2cc(220c,3) ");
        if(v&8)strcat(exp,"10869784(9000,7300) ");
        if(v&4)strcat(exp,"10869784(9000,7400) ");else strcat(exp,"1086c2cc(220c,4) ");
        strcat(exp,"1082399c(1000,c920,7100) 10826800(1000,c920,7100,9000,5360) 10863790(1000,c920,7100,9000,5360) 10863604(1000,c920,7100,9000) ");
        strcat(exp,"1086c2cc(220c,1) ");
        if(v&2)strcat(exp,"1086c2cc(220c,3) ");
        strcat(exp,"108225dc(1000,c920,7100,9000,5360,1) ");
        CHECK(OK(exp)&&r==(v>>4&1)&&r8(NULL,LISTW+0x71)==1&&r8(NULL,LISTW+0x70)==2);
        /* As the final phase saw it: +0 u16 0, +2 the field byte, +3 the first target's position (or 10), +4 1, +5 low bits cleared, +8 0. */
        CHECK(final_field[0]==(0x5au<<16|(uint32_t)((v&2)&&!(v&8)?3:10)<<24));
        CHECK(final_field[1]==0x55667401u);
        CHECK(final_field[2]==0);
        /* Restored afterwards. */
        CHECK(r32(NULL,FIELD)==0x11223344&&r32(NULL,FIELD+4)==0x55667788&&r32(NULL,FIELD+8)==0x99aabbcc);++cases;
    }
    /* Kind 0x28: the list empties during the phases. */
    reset();w8(NULL,REC+4,1);w8(NULL,REC+5,2);w8(NULL,LISTW+0x6e,1);w32(NULL,LISTW,MON+0x300);empty_after=1;
    CHECK(px_battle_kind_use_move(CTX,REC,&cures)==0&&!strstr(log_,"108225dc")&&!strstr(log_,"1086b5e0"));++cases;
    /* Kind 0x28: both flags cleared during the phases, no targets left: still runs the final phase. */
    reset();w8(NULL,REC+4,1);w8(NULL,REC+5,2);w8(NULL,LISTW+0x6e,1);w32(NULL,LISTW,MON+0x300);empty_after=2;
    px_battle_kind_use_move(CTX,REC,&cures);CHECK(strstr(log_,"108225dc")!=NULL);++cases;
    /* Kind 0x36: cure the side. */
    for(uint32_t v=0;v<64;++v){
        reset();side_n=3;w32(NULL,REC,0x36u|1u<<8);hp[2]=v&1?0:5;
        for(unsigned i=0;i<3;++i){const uint32_t p=MON+0x100*i;w8(NULL,p+0x169,(uint8_t)(v&2?0x88:0x80));w32(NULL,p+0xa0,v&4?1:0);w32(NULL,p+0x84,v&8?2:0);
            w32(NULL,p+0x94,v&16?4:0);w32(NULL,p+0x9c,v&32?1:0);w8(NULL,p+0x171,(uint8_t)(i==0?3:i==1?0x19:0));}   /* 0x19: outside 1..0x18 */
        w8(NULL,CTX+0x108c,2);w32(NULL,CTX+0x10bc,MON+0x500);w32(NULL,CTX+0x10cc,MON);
        ret(0x10873164,4);ret(0x108651d8,0);
        CHECK(px_battle_kind_cure_side(CTX,REC,&cures)==1);
        char exp[4000],t[300];snprintf(exp,sizeof exp,"10873164(0,6000,1) 108672c0(0,804,c918) 108707d4(6000,0) 108707d4(6000,1) 108707d4(6000,2) ");
        for(unsigned i=0;i<3;++i){
            if(i==2&&(v&1))continue;
            const uint32_t p=MON+0x100*i;int cured=0;
            if(v&2){snprintf(t,sizeof t,"1086c22c(5555,1c,%x,3) ",i);strcat(exp,t);
                if(v&8){snprintf(t,sizeof t,"108695c0(1000,%x,1a,0) ",p);strcat(exp,t);}
                snprintf(t,sizeof t,"1086c22c(5555,38,%x,0) ",i);strcat(exp,t);
                if(i==0){snprintf(t,sizeof t,"1086844c(1000,%x,0,1) ",p);strcat(exp,t);}
                cured=1;}
            snprintf(t,sizeof t,"108651d8(1000,%x,1) ",p);strcat(exp,t);
            if(v&16){snprintf(t,sizeof t,"108695c0(1000,%x,1e,0) ",p);strcat(exp,t);cured=1;}
            if(v&32){snprintf(t,sizeof t,"108695c0(1000,%x,20,0) ",p);strcat(exp,t);cured=1;}
            if(cured){snprintf(t,sizeof t,"1086b5e0(5555,6a,441,%x,ffff0000) ",i);strcat(exp,t);}
            CHECK(mem[p+0x169]==0x80);
        }
        CHECK(OK(exp));
        /* Battler 0's entry (the second, +0x10cc) is marked unless +0xa0 is set. */
        CHECK(mem[CTX+0x10cc+0xd]==((v&2)&&!(v&4)?1:0)&&mem[CTX+0x10bc+0xd]==0);++cases;
    }
    /* End to end: kinds 0xa and 0x21 through battle_action and the processor (0xa with the low bits). */
    {
        reset();w32(NULL,WORD,0);
        for(unsigned i=0;i<PX_BATTLE_ACTION_KINDS;++i){sizes[2*i]=(uint8_t)i;sizes[2*i+1]=0x40;}
        uint32_t r=px_battle_action_record(CTX,0xa,1,&action);w8(NULL,r+4,7);w8(NULL,r+5,2);w8(NULL,r+0x11,1);w8(NULL,r+0x12,1);cond(2,7);
        px_battle_action_commit(CTX,r,&action);
        r=px_battle_action_record(CTX,0x21,2,&action);w16(NULL,r+4,0x31);w8(NULL,r+6,2);w32(NULL,r,r32(NULL,r)|0x1000000u);
        px_battle_action_commit(CTX,r,&action);
        CHECK(routed==2&&strstr(log_,"108695c0(1000,7200,7,c900) 1086bbe8(1000,5394) ")&&strstr(log_,"1086b140(1000,7200,31,0) ")&&faults==0);
        CHECK((r32(NULL,WORD)&0x70000000u)==0x70000000u&&(r32(NULL,WORD)>>10&0x1ff)==0);++cases;
    }
    printf("battle_cures: %u cases, all tests passed\n",cases);
    return 0;
}
