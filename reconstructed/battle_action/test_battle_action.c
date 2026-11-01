#include "battle_action.h"
#include "../battle_insight/battle_insight.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, CTX=0x1000, STK=CTX+0x437c, TEMP=CTX+0x467a, STACK=0x9000, BATTLE=0xa000, POOL=0xa100, TABLE=0xb000,
       MON=0xc000, LIST=0xd000, CANDIDATES=0xd010, MSG=0xe000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};

/* Synthetic record sizes (not the original table): odd sizes exercise the rounding. */
static uint8_t sizes[2*PX_BATTLE_ACTION_KINDS];
static unsigned faults,executed;static char log_[2048];static uint32_t mutate_size,mutate_at;
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b,uint32_t c) { char t[64];snprintf(t,sizeof t,f,a,b,c);note(t); }
static uint32_t used(void) { return r32(NULL,STK)>>10&0x1ff; }
static void fault(void *u) { (void)u;++faults;note("FAULT "); }
/* Logs the record as executed: kind/id/size, then a message at +4 for kind 3. */
static void execute(void *u,uint32_t c,uint32_t r)
{
    (void)u;CHECK(c==CTX);++executed;
    const uint32_t h=r32(NULL,r);
    notef("x%x/%u/%x ",h&0xff,h>>8&0x1f,h>>13&0x3ff);
    if(r-STK-4+(h>>13&0x3ff)!=used())note("! ");   /* not the top record */
    if((h&0xff)==3){
        const uint32_t f=r16(NULL,r+6);notef("m%x/%u:",r16(NULL,r+4),f&0x7f,0);
        for(uint32_t i=0;i<(f>>7&0x7f);++i)notef("%x,",r32(NULL,r+8+4*i),0,0);
        note(" ");
    }
    if(mutate_at==executed)w32(NULL,r,(h&~UINT32_C(0x7fe000))|mutate_size<<13);
}
static const struct px_battle_action_api action={&memory,sizes,execute,fault};

static void reset(void)
{
    memset(mem,0,sizeof mem);faults=executed=0;log_[0]=0;mutate_at=0;
    for(unsigned i=0;i<PX_BATTLE_ACTION_KINDS;++i){sizes[2*i]=(uint8_t)i;sizes[2*i+1]=(uint8_t)(4*(i%5)+i%4);}
    sizes[2*1+1]=sizes[2*2+1]=4;sizes[2*3+1]=0x21;sizes[2*0xd+1]=0x19;
}

/* The insight handlers wired to the real services. */
struct mon { uint32_t ability,defense,special,moves[4],nmoves,item; };
static struct mon mons[8];static uint32_t opp[4],nopp,random_value;
static void abort_battle(void *u) { (void)u;CHECK(0); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,NULL,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static uint32_t value(void *u,uint32_t p,uint32_t k) { (void)u;const struct mon *m=&mons[mon(p)];if(k==0x10)return m->ability;if(k==9)return m->defense;CHECK(k==0xb);return m->special; }
static const struct px_battle_ability_api base={&events,value,NULL,NULL,NULL,NULL,NULL};
static uint32_t opponents(void *u,uint32_t c,uint32_t id,uint32_t out) { (void)u;CHECK(c==CTX&&id==2);for(uint32_t i=0;i<nopp;++i)w8(NULL,out+i,(uint8_t)opp[i]);return nopp; }
static uint32_t temp_buffer(void *u,uint32_t c,uint32_t n) { (void)u;return px_battle_temp_buffer(c,n,&action); }
static uint32_t random_below(void *u,uint32_t n) { (void)u;return random_value%n; }
static uint32_t move_at(void *u,uint32_t p,uint32_t s) { (void)u;CHECK(s<mons[mon(p)].nmoves);return mons[mon(p)].moves[s]; }
static uint32_t move_category(void *u,uint32_t m) { (void)u;return m!=0; }
static uint32_t move_power(void *u,uint32_t m) { (void)u;return m; }
static uint32_t move_kind(void *u,uint32_t m) { (void)u;(void)m;return 0; }
static void popup(void *u,uint32_t c,uint32_t p,uint32_t id) { (void)u;px_battle_ability_popup(c,p,id,&action); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;return px_battle_action_record(c,k,id,&action); }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;px_battle_message(r,k,id,&action); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;px_battle_message_arg(r,v,&action); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;px_battle_action_commit(c,r,&action); }
static const struct px_battle_insight_api insight={&base,opponents,NULL,NULL,temp_buffer,random_below,NULL,NULL,NULL,NULL,NULL,
    move_at,move_category,move_power,move_kind,NULL,NULL,NULL,NULL,NULL,popup,action_record,message,message_arg,commit,LIST,CANDIDATES};
static void battle(uint32_t entering)
{
    w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i){
        const uint32_t p=MON+0x100*i;
        w32(NULL,TABLE+0x84+4*i,p);w8(NULL,p+0x19,(uint8_t)i);w8(NULL,p+0x152,(uint8_t)mons[i].nmoves);w16(NULL,p+0x12,(uint16_t)mons[i].item);
    }
    px_battle_event_push(&events);px_battle_event_set_id(0x02,entering,&events);log_[0]=0;
}

int main(void)
{
    unsigned cases=0;
    /* Push: every kind, sizes rounded up to 4, zeroed, header bits, stack word. */
    for(uint32_t k=0;k<PX_BATTLE_ACTION_KINDS;++k)for(uint32_t id=0;id<0x40;id+=0x1d){
        reset();memset(mem+STK+4,0xa5,0x200);
        const uint32_t high=0xfff80000u|0x3a5u;w32(NULL,STK,high|(7u<<10));
        const uint32_t s=sizes[2*k+1],rounded=(s+3)&~3u;
        const uint32_t r=px_battle_action_record(CTX,k,id,&action);
        if(s==0){CHECK(r==0&&faults==1&&r32(NULL,STK)==(high|7u<<10));++cases;continue;}
        CHECK(r==STK+4+7&&faults==0);
        CHECK(r32(NULL,r)==(k|(id&0x1f)<<8|rounded<<13|0x4000000u));
        for(uint32_t i=4;i<rounded;++i)CHECK(mem[r+i]==0);
        CHECK(mem[r+rounded]==0xa5);
        CHECK(r32(NULL,STK)==(high|(7+rounded)<<10));++cases;
    }
    /* Kinds compare against the whole argument; the first matching entry wins; unknown kinds fault. */
    reset();CHECK(px_battle_action_record(CTX,0x103,0,&action)==0&&faults==1&&used()==0);++cases;
    reset();sizes[2*5]=3;CHECK(px_battle_action_record(CTX,3,0,&action)==STK+4&&used()==0x24);++cases;   /* entry 3 (0x21) before entry 5 */
    reset();sizes[2*3]=0x3f;sizes[2*0x3c]=3;sizes[2*0x3c+1]=9;CHECK(px_battle_action_record(CTX,3,0,&action)==STK+4&&used()==12);++cases;
    reset();sizes[2*0x3c]=0x77;sizes[2*0x3c+1]=1;CHECK(px_battle_action_record(CTX,0x77,0,&action)==STK+4&&used()==4);++cases;  /* the last entry is searched */
    reset();CHECK(px_battle_action_record(CTX,0x3d,0,&action)==0&&faults==1);++cases;
    /* Capacity: used + size may reach 0x1f4. */
    for(uint32_t u=0x1c0;u<=0x1f4;u+=4){
        reset();w32(NULL,STK,u<<10);
        const uint32_t r=px_battle_action_record(CTX,3,1,&action);
        if(u+0x24<=0x1f4)CHECK(r==STK+4+u&&used()==u+0x24&&faults==0);else CHECK(r==0&&faults==1&&used()==u);
        ++cases;
    }
    /* Commit: execute first (the record still on the stack), then pop the top. */
    reset();
    {
        const uint32_t a=px_battle_action_record(CTX,3,2,&action),b=px_battle_action_record(CTX,0xd,5,&action);
        CHECK(used()==0x24+0x1c);
        px_battle_action_commit(CTX,a,&action);CHECK(faults==1&&executed==1&&used()==0x40);++cases;   /* not the top */
        px_battle_action_commit(CTX,b,&action);CHECK(faults==1&&executed==2&&used()==0x24);++cases;
        px_battle_action_commit(CTX,a,&action);CHECK(faults==1&&executed==3&&used()==0);++cases;
        CHECK(strcmp(log_,"x3/2/24 ! m0/0: FAULT xd/5/1c x3/2/24 m0/0: ")==0);++cases;
    }
    /* Pop only touches bits 10-18. */
    reset();w32(NULL,STK,0xfff803ffu);
    {
        const uint32_t r=px_battle_action_record(CTX,1,3,&action);CHECK(r32(NULL,STK)==(0xfff803ffu|4u<<10));
        px_battle_action_commit(CTX,r,&action);CHECK(r32(NULL,STK)==0xfff803ffu&&faults==0);++cases;
    }
    /* The header is read after execute: a changed size breaks the pop. */
    for(uint32_t s=0;s<0x30;s+=4){
        reset();const uint32_t r=px_battle_action_record(CTX,3,0,&action);mutate_at=1;mutate_size=s;
        px_battle_action_commit(CTX,r,&action);
        if(s==0x24)CHECK(faults==0&&used()==0);else CHECK(faults==1&&used()==0x24);
        ++cases;
    }
    /* Pops across bit 8 of the count. */
    reset();w32(NULL,STK,0xf0u<<10);
    {
        const uint32_t r=px_battle_action_record(CTX,3,0,&action);CHECK(used()==0x114);
        px_battle_action_commit(CTX,r,&action);CHECK(used()==0xf0&&faults==0);++cases;
    }
    /* A size above the bytes in use faults, even when offset plus size matches (a record below the stack). */
    reset();w32(NULL,STK-0x20,0x34u<<13);w32(NULL,STK,0x10u<<10);px_battle_action_commit(CTX,STK-0x20,&action);CHECK(faults==1&&used()==0x10);++cases;
    reset();w32(NULL,STK+4,0x24u<<13);w32(NULL,STK,0x20u<<10);px_battle_action_commit(CTX,STK+4,&action);CHECK(faults==1&&used()==0x20);++cases;
    /* Popup: a header-only record of kind phase, executed and popped. */
    for(uint32_t phase=1;phase<=2;++phase)for(uint32_t id=0;id<6;++id){
        reset();px_battle_ability_popup(CTX,phase,id,&action);
        char want[32];snprintf(want,sizeof want,"x%x/%u/4 ",phase,id);
        CHECK(strcmp(log_,want)==0&&used()==0&&faults==0);++cases;
    }
    /* Message: id, kind masked to 7 bits, count cleared, bits 14-15 kept. */
    for(uint32_t flags=0;flags<0x10000;flags+=0x1111)for(uint32_t kind=0;kind<0x100;kind+=0x33){
        reset();w16(NULL,MSG+2,(uint16_t)flags);px_battle_message(MSG,kind,0x12345,&action);
        CHECK(r16(NULL,MSG)==0x2345&&r16(NULL,MSG+2)==((kind&0x7f)|(flags&0xc000)));++cases;
    }
    /* Arguments: eight stored in order, a ninth dropped; other bits kept. */
    for(uint32_t top=0;top<4;++top){
        reset();px_battle_message(MSG,0x45,7,&action);w16(NULL,MSG+2,(uint16_t)(r16(NULL,MSG+2)|top<<14));
        for(uint32_t i=0;i<9;++i)px_battle_message_arg(MSG,0x1000u+i,&action);
        CHECK(r16(NULL,MSG+2)==(0x45|8u<<7|top<<14));
        for(uint32_t i=0;i<8;++i)CHECK(r32(NULL,MSG+4+4*i)==0x1000u+i);
        CHECK(r32(NULL,MSG+0x24)==0);++cases;
    }
    reset();w16(NULL,MSG+2,0x3f80);px_battle_message_arg(MSG,9,&action);CHECK(r16(NULL,MSG+2)==0x3f80&&r32(NULL,MSG+4)==0);++cases;   /* count 0x7f */
    /* Temporary buffer. */
    for(uint32_t n=0;n<0x200;n+=0x13){reset();CHECK(px_battle_temp_buffer(CTX,n,&action)==TEMP&&faults==(n>=0x140));++cases;}
    reset();CHECK(px_battle_temp_buffer(CTX,0x13f,&action)==TEMP&&faults==0);CHECK(px_battle_temp_buffer(CTX,0x140,&action)==TEMP&&faults==1);++cases;

    /* Handlers on the real services. Frisk: popup, one report per held item, popup; the stack is empty after. */
    for(uint32_t held=0;held<8;++held){
        reset();memset(mons,0,sizeof mons);nopp=3;opp[0]=4;opp[1]=5;opp[2]=6;
        for(unsigned i=0;i<3;++i)mons[4+i].item=held>>i&1?0x100+i:0;
        battle(2);px_battle_frisk(0,CTX,2,0,&insight);
        char want[256]="";
        if(held){
            strcat(want,"x1/2/4 ");
            int first=1;
            for(unsigned i=0;i<3;++i)if(held>>i&1){
                char t[64];
                if(first)snprintf(t,sizeof t,"x3/2/24 m1b7/2:2,%x,%x, ",4+i,0x100+i);
                else snprintf(t,sizeof t,"x3/2/24 mfe/1:%x,%x, ",4+i,0x100+i);
                strcat(want,t);first=0;
            }
            strcat(want,"x2/2/4 ");
        }
        CHECK(strcmp(log_,want)==0&&used()==0&&faults==0);++cases;
    }
    /* Download: the stat record (kind 0xd, size 0x1c). */
    for(uint32_t d=0;d<3;++d){
        reset();memset(mons,0,sizeof mons);nopp=1;opp[0]=4;mons[4].defense=d;mons[4].special=1;
        battle(2);px_battle_download(0,CTX,2,0,&insight);
        CHECK(strcmp(log_,"xd/2/1c ")==0&&used()==0&&faults==0);++cases;
        /* The record was zeroed, then filled; it stays in memory after the pop. */
        CHECK(mem[STK+4+4]==(d<1?1:3)&&mem[STK+4+0xc]==2&&mem[STK+4+0x16]==1&&mem[STK+4+0x17]==1&&(r32(NULL,STK+4)&0x800000));++cases;
    }
    /* Forewarn: the strongest move from the real temporary buffer. */
    reset();memset(mons,0,sizeof mons);nopp=2;opp[0]=4;opp[1]=5;mons[4].nmoves=2;mons[4].moves[0]=0x30;mons[4].moves[1]=0x50;
    mons[5].nmoves=1;mons[5].moves[0]=0x50;random_value=1;
    battle(2);px_battle_forewarn(0,CTX,2,0,&insight);
    CHECK(strcmp(log_,"x1/2/4 x3/2/24 m1b1/2:5,50, x2/2/4 ")==0&&used()==0&&faults==0);
    CHECK(r8(NULL,TEMP)==4&&r16(NULL,TEMP+2)==0x50&&r8(NULL,TEMP+4)==5);++cases;
    printf("battle_action: %u cases, all tests passed\n",cases);
    return 0;
}
