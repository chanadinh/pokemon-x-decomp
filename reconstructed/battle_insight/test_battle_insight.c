#include "battle_insight.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000,
       HANDLER=0x6000, RECORD=0x7000, LIST=0x7400, CANDIDATES=0x7410, BUFFER=0x7800 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
/* Battler i at MON + 0x100 i: id byte, item, moves; values by key. */
struct mon { uint32_t ability,defense,special,moves[4],nmoves,item,packed; };
static struct mon mons[8];static uint32_t opp[4],nopp,random_value,random_calls,untraceable_mask,pos_value,mode,pos_ok;static uint64_t ally_mask;
static uint32_t cat[0x200],power[0x200],kind[0x200],damaging[0x200],type[0x200];static unsigned aborts;static char log_[1024];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[48];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static unsigned mon(uint32_t p) { CHECK(p>=MON&&(p-MON)%0x100==0&&(p-MON)/0x100<8);return (p-MON)/0x100; }
static uint32_t value(void *u,uint32_t p,uint32_t k) { (void)u;const struct mon *m=&mons[mon(p)];notef("v%u/%x ",mon(p),k);
    if(k==0x10)return m->ability;
    if(k==9)return m->defense;
    CHECK(k==0xb);return m->special; }
static const struct px_battle_ability_api base={&events,value,NULL,NULL,NULL,NULL,NULL};
static uint32_t opponents(void *u,uint32_t c,uint32_t id,uint32_t out) { (void)u;CHECK(c==CTX&&id==2);for(uint32_t i=0;i<nopp;++i){w8(NULL,out+i,(uint8_t)opp[i]);}return nopp; }
static uint32_t opposing_selector(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX&&(id==2||id==0x1f));note("sel ");return 0x40; }
static uint32_t side_battlers(void *u,uint32_t c,uint32_t s,uint32_t out) { (void)u;CHECK(c==CTX&&s==0x140&&out==LIST);for(uint32_t i=0;i<nopp;++i){w8(NULL,out+i,(uint8_t)opp[i]);}return nopp; }
static uint32_t temp_buffer(void *u,uint32_t c,uint32_t n) { (void)u;CHECK(c==CTX&&n==0x14);return BUFFER; }
static uint32_t random_below(void *u,uint32_t n) { (void)u;++random_calls;notef("rnd%u ",n,0);return random_value%n; }
static uint32_t untraceable(void *u,uint32_t a) { (void)u;return a<32&&(untraceable_mask>>a&1); }
static uint32_t is_ally(void *u,uint32_t id,uint32_t o) { (void)u;CHECK(id<8&&o<8);return (uint32_t)(ally_mask>>(id*8+o)&1); }
static uint32_t position(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX);(void)id;note("pos ");return pos_value; }
static uint32_t battle_mode(void *u,uint32_t c) { (void)u;CHECK(c==CTX);return mode; }
static uint32_t position_in(void *u,uint32_t s,uint32_t p) { (void)u;CHECK(s==0x40&&p==pos_value);return pos_ok; }
static uint32_t move_at(void *u,uint32_t p,uint32_t s) { (void)u;CHECK(s<mons[mon(p)].nmoves);return mons[mon(p)].moves[s]; }
static uint32_t move_category(void *u,uint32_t m) { (void)u;CHECK(m<0x200);return cat[m]; }
static uint32_t move_power(void *u,uint32_t m) { (void)u;return power[m]; }
static uint32_t move_kind(void *u,uint32_t m) { (void)u;return kind[m]; }
static uint32_t damaging_move(void *u,uint32_t m) { (void)u;return damaging[m]; }
static uint32_t move_type(void *u,uint32_t m) { (void)u;return type[m]; }
static uint32_t hp_type(void *u,uint32_t d) { (void)u;return d; }
static uint32_t packed_types(void *u,uint32_t p) { (void)u;return mons[mon(p)].packed; }
static uint32_t effectiveness(void *u,uint32_t t,uint32_t packed) { (void)u;return t==packed?8:7; }
static void popup(void *u,uint32_t c,uint32_t p,uint32_t id) { (void)u;CHECK(c==CTX);notef("pop%u/%u ",p,id); }
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;CHECK(c==CTX);notef("rec%x/%u ",k,id);return RECORD; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;notef("msg%x/%x ",id,(r-RECORD)*16+k); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;(void)r;notef("arg%x ",v,0); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r==RECORD);note("commit "); }
static const struct px_battle_insight_api api={&base,opponents,opposing_selector,side_battlers,temp_buffer,random_below,untraceable,is_ally,
    position,battle_mode,position_in,move_at,move_category,move_power,move_kind,damaging_move,move_type,hp_type,packed_types,effectiveness,
    popup,action_record,message,message_arg,commit,LIST,CANDIDATES};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static void (*const table[])(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_insight_api*)={px_battle_download,px_battle_frisk};
    CHECK(f<2);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(mons,0,sizeof mons);memset(cat,0,sizeof cat);memset(power,0,sizeof power);memset(kind,0,sizeof kind);
    memset(damaging,0,sizeof damaging);memset(type,0,sizeof type);nopp=0;random_value=random_calls=untraceable_mask=ally_mask=0;pos_value=1;mode=1;pos_ok=1;
    aborts=0;log_[0]=0;w32(NULL,CTX+8,TABLE);
    for(unsigned i=0;i<8;++i){w32(NULL,TABLE+0x84+4*i,MON+0x100*i);w8(NULL,MON+0x100*i+0x19,(uint8_t)i);}
}
static void sync(void)
{
    for(unsigned i=0;i<8;++i){w8(NULL,MON+0x100*i+0x152,(uint8_t)mons[i].nmoves);w16(NULL,MON+0x100*i+0x12,(uint16_t)mons[i].item);w32(NULL,MON+0x100*i,i*3);}
}
static void frame(uint32_t entering) { sync();px_battle_event_push(&events);px_battle_event_set_id(0x02,entering,&events);log_[0]=0; }
int main(void)
{
    unsigned cases=0;
    /* Download: Attack when the opponents' summed Defense is lower, else Sp. Atk; 16-bit sums. */
    for(uint32_t d=0;d<6;++d)for(uint32_t s=0;s<6;++s)for(uint32_t n=0;n<3;++n) {
        reset();nopp=n;opp[0]=4;opp[1]=5;mons[4].defense=d*10;mons[4].special=s*10;mons[5].defense=7;mons[5].special=7;w32(NULL,RECORD,0x11);
        frame(2);px_battle_download(HANDLER,CTX,2,WORK,&api);
        if(!n){CHECK(log_[0]==0);++cases;continue;}
        const uint32_t sd=d*10+(n>1?7:0),ss=s*10+(n>1?7:0);
        CHECK(mem[RECORD+4]==(sd<ss?1:3)&&mem[RECORD+0xc]==2&&mem[RECORD+0x16]==1&&mem[RECORD+0x17]==1&&r32(NULL,RECORD)==0x800011);++cases;
    }
    reset();nopp=2;opp[0]=4;opp[1]=5;mons[4].defense=0xffff;mons[5].defense=2;mons[4].special=0;mons[5].special=2;frame(2);
    px_battle_download(HANDLER,CTX,2,WORK,&api);CHECK(mem[RECORD+4]==1);++cases;   /* 0xffff + 2 wraps to 1 < 2 */
    reset();nopp=1;opp[0]=4;frame(3);px_battle_download(HANDLER,CTX,2,WORK,&api);CHECK(log_[0]==0);++cases;
    /* Forewarn: the power rule, ties, the random pick and the message. */
    {
        reset();cat[10]=0;cat[11]=1;power[11]=90;cat[12]=1;power[12]=1;kind[12]=9;cat[0x44]=1;power[0x44]=1;cat[0xf3]=1;power[0xf3]=1;
        cat[0x170]=1;power[0x170]=1;cat[13]=1;power[13]=1;cat[14]=1;power[14]=0x1ff;
        CHECK(px_battle_forewarn_power(10,&api)==1&&px_battle_forewarn_power(11,&api)==90&&px_battle_forewarn_power(12,&api)==150);
        CHECK(px_battle_forewarn_power(0x44,&api)==120&&px_battle_forewarn_power(0xf3,&api)==120&&px_battle_forewarn_power(0x170,&api)==120);
        CHECK(px_battle_forewarn_power(13,&api)==80&&px_battle_forewarn_power(14,&api)==0xff);++cases;
        for(uint32_t rv=0;rv<4;++rv) {
            reset();cat[11]=1;power[11]=90;cat[12]=1;power[12]=1;kind[12]=9;cat[20]=1;power[20]=150;cat[21]=0;
            nopp=2;opp[0]=4;opp[1]=5;mons[4].nmoves=2;mons[4].moves[0]=11;mons[4].moves[1]=12;mons[5].nmoves=3;mons[5].moves[0]=21;mons[5].moves[1]=20;mons[5].moves[2]=11;
            random_value=rv;frame(2);px_battle_forewarn(HANDLER,CTX,2,WORK,&api);
            /* Best 150: (4, move 12) and (5, move 20). */
            const uint32_t who=rv%2?5:4,mv=rv%2?20:12;char b[96];snprintf(b,sizeof b,"rnd2 pop1/2 rec3/2 msg1b1/42 arg%x arg%x commit pop2/2 ",who,mv);
            CHECK(!strcmp(log_,b));++cases;
        }
        reset();nopp=1;opp[0]=4;mons[4].nmoves=0;frame(2);px_battle_forewarn(HANDLER,CTX,2,WORK,&api);CHECK(log_[0]==0&&random_calls==0);++cases;
        reset();cat[11]=1;power[11]=50;nopp=1;opp[0]=4;mons[4].nmoves=1;mons[4].moves[0]=11;frame(2);px_battle_forewarn(HANDLER,CTX,2,WORK,&api);CHECK(random_calls==1);++cases;
    }
    /* Anticipation: one-hit KO or a super-effective damaging move (Hidden Power by its holder's type). */
    for(uint32_t variant=0;variant<6;++variant) {
        reset();nopp=2;opp[0]=4;opp[1]=5;mons[2].packed=6;mons[4].nmoves=1;mons[4].moves[0]=30;mons[5].nmoves=1;mons[5].moves[0]=31;
        damaging[30]=damaging[31]=1;type[30]=type[31]=5;
        if(variant==1)kind[31]=9;
        if(variant==2)type[31]=6;
        if(variant==3){mons[5].moves[0]=0xed;damaging[0xed]=1;w32(NULL,MON+0x500,6);}  /* Hidden Power, data word 6 */
        if(variant==4){mons[5].moves[0]=0xed;damaging[0xed]=1;w32(NULL,MON+0x500,5);}
        if(variant==5){type[31]=6;damaging[31]=0;}
        sync();px_battle_event_push(&events);px_battle_event_set_id(0x02,2,&events);log_[0]=0;
        if(variant>=3)w32(NULL,MON+0x500,variant==3?6:5);
        px_battle_anticipation(HANDLER,CTX,2,WORK,&api);
        const int on=variant==1||variant==2||variant==3;
        CHECK(!strcmp(log_,on?"pop1/2 rec3/2 msg1b4/42 arg2 commit pop2/2 ":""));++cases;
    }
    /* Frisk: every held item, the first message naming this battler. */
    for(uint32_t mask=0;mask<8;++mask) {
        reset();nopp=3;opp[0]=4;opp[1]=5;opp[2]=6;for(unsigned i=0;i<3;++i)mons[4+i].item=mask>>i&1?0x100+i:0;
        frame(2);px_battle_frisk(HANDLER,CTX,2,WORK,&api);
        char want[256]="";unsigned k=0;
        if(mask){strcat(want,"pop1/2 ");
            for(unsigned i=0;i<3;++i)if(mask>>i&1){char b[96];
                if(k==0) { snprintf(b,sizeof b,"rec3/2 msg1b7/42 arg2 arg%x arg%x commit ",4+i,0x100+i); }
                else { snprintf(b,sizeof b,"rec3/2 msgfe/41 arg%x arg%x commit ",4+i,0x100+i); }
                strcat(want,b);++k;}
            strcat(want,"pop2/2 ");}
        CHECK(!strcmp(log_,want));
        for(unsigned i=0;i<3;++i) { CHECK(r16(NULL,BUFFER+2*i)==mons[4+i].item); }
        ++cases;
    }
    /* Trace on entry: a random traceable opponent; none waits (work word 1). */
    for(uint32_t mask=0;mask<4;++mask)for(uint32_t rv=0;rv<2;++rv) {
        reset();nopp=2;opp[0]=4;opp[1]=5;mons[4].ability=10;mons[5].ability=11;untraceable_mask=(mask&1?1u<<10:0)|(mask&2?1u<<11:0);random_value=rv;
        w32(NULL,RECORD,0x11);frame(2);px_battle_trace(HANDLER,CTX,2,WORK,&api);
        const unsigned found=(mask&1?0:1)+(mask&2?0:1);
        if(!found){CHECK(r32(NULL,WORK)==1&&!strstr(log_,"rec"));++cases;continue;}
        uint32_t src=found==1?(mask&1?5:4):(rv?5:4);
        CHECK(random_calls==(found>1)&&mem[RECORD+6]==2&&r16(NULL,RECORD+4)==mons[src].ability&&mem[RECORD+8]==1&&r32(NULL,RECORD)==0x800011);
        char b[64];snprintf(b,sizeof b,"msg17d/c2 arg%x arg%x commit ",src,mons[src].ability);CHECK(strstr(log_,b)&&r32(NULL,WORK)==0);++cases;
    }
    reset();nopp=1;opp[0]=4;mons[4].ability=0;frame(2);px_battle_trace(HANDLER,CTX,2,WORK,&api);CHECK(!strstr(log_,"rec")&&r32(NULL,WORK)==0);++cases;  /* ability 0 */
    reset();nopp=1;opp[0]=4;mons[4].ability=9;frame(0x1f);px_battle_trace(HANDLER,CTX,0x1f,WORK,&api);CHECK(!strstr(log_,"rec"));++cases;   /* id 0x1f */
    /* Trace pending: an opponent entering in a faced position. */
    for(uint32_t pend=0;pend<3;++pend)for(uint32_t al=0;al<2;++al)for(uint32_t pos=0;pos<2;++pos)for(uint32_t md=1;md<3;++md)for(uint32_t ok=0;ok<2;++ok)for(uint32_t un=0;un<2;++un) {
        reset();w32(NULL,WORK,pend);if(al)ally_mask=1ull<<(5*8+2);pos_value=pos?0xa:3;mode=md;pos_ok=ok;mons[5].ability=12;if(un)untraceable_mask=1u<<12;
        frame(0x105);px_battle_trace(HANDLER,CTX,2,WORK,&api);
        const int on=pend==1&&!al&&!pos&&(md!=2||ok)&&!un;
        CHECK((strstr(log_,"rec20")!=NULL)==on);if(on)CHECK(strstr(log_,"arg5 argc ")!=NULL);++cases;
    }
    /* Through the dispatcher: Download then Frisk on event 0x94. */
    {
        reset();nopp=1;opp[0]=4;mons[4].defense=5;mons[4].special=9;mons[4].item=0x55;
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7c00+0x10*i;
            w8(NULL,entries,0x94);w32(NULL,entries+4,i);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,2);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);frame(2);px_battle_event_dispatch(CTX,0x94,&events);
        CHECK(mem[RECORD+4]==1&&strstr(log_,"msg1b7/42 arg2 arg4 arg55 ")&&aborts==0);++cases;
    }
    printf("battle_insight: %u cases, all tests passed\n",cases);
    return 0;
}
