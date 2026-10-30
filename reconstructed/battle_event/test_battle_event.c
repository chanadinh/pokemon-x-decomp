#include "battle_event.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x20000, STACK=0x1000, BATTLE=0x2000, FREE=0x3000, NODES=0x4000, ENTRY=0x8000, CTX=0x77 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static unsigned aborts,overflows,blocked_calls;static uint32_t block_result;static char log_[4096];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;++overflows;note("OVERFLOW "); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;CHECK(c==CTX);char b[32];snprintf(b,sizeof b,"B%x:%x ",h,e);note(b);++blocked_calls;return block_result; }
/* Scripted handler actions, by function id. */
struct action { uint32_t set_on,set_bits,clear_on,clear_bits;int relink;uint32_t relink_to;int nested_event; };
static struct action actions[16];
static const struct px_battle_event_api api;
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;CHECK(c==CTX&&w==h+0x1c&&f<16);char b[48];snprintf(b,sizeof b,"C%x@%x/%x/d%x ",f,h,v,r32(NULL,BATTLE+0x68));note(b);
    const struct action *a=&actions[f];
    if(a->set_on)w32(NULL,a->set_on+0x18,r32(NULL,a->set_on+0x18)|a->set_bits);
    if(a->clear_on)w32(NULL,a->clear_on+0x18,r32(NULL,a->clear_on+0x18)&~a->clear_bits);
    if(a->relink)w32(NULL,h+4,a->relink_to);
    if(a->nested_event) { px_battle_event_dispatch(CTX,(uint32_t)a->nested_event,&api);snprintf(b,sizeof b,"F%x ",r32(NULL,h+0x18));note(b); }
}
static const struct px_battle_event_api api={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,FREE};
static void reset(void) { memset(mem,0,sizeof mem);aborts=overflows=blocked_calls=0;block_result=0;log_[0]=0;memset(actions,0,sizeof actions); }
static unsigned calls(void) { unsigned n=0;for(const char *p=log_;*p;++p)n+=*p=='@';return n; }
static uint32_t key(uint32_t i) { return r16(NULL,STACK+4+2*i); }
/* Independent frame model for valid sequences. */
struct frame { uint32_t keys[96],values[96],kinds[96],mins[96],maxs[96];unsigned n; };
static struct frame frames[64];static unsigned depth;
static uint32_t lcg=12345;
static uint32_t rnd(uint32_t n) { lcg=lcg*1103515245u+12345u;return (lcg>>16)%n; }
static unsigned used(void) { unsigned t=0;for(unsigned i=0;i<depth;++i)t+=frames[i].n+1;return t; }
static void check_layout(void)
{
    /* Frames lie one after another: marker, then entries. */
    unsigned p=0;
    for(unsigned f=0;f<depth;++f) {
        CHECK(key(p)==1);++p;
        for(unsigned i=0;i<frames[f].n;++i,++p) {
            CHECK(key(p)==frames[f].keys[i]&&r32(NULL,STACK+0xc4+4*p)==frames[f].values[i]&&r8(NULL,STACK+0x544+p)==frames[f].kinds[i]);
            CHECK(r32(NULL,STACK+0x3c4+4*p)==frames[f].mins[i]&&r32(NULL,STACK+0x244+4*p)==frames[f].maxs[i]);
        }
        if(f+1==depth)CHECK(r32(NULL,STACK)==p-frames[f].n);
    }
    for(unsigned i=p;i<96;++i)CHECK(key(i)==0);
    if(depth==0)CHECK(r32(NULL,STACK)==0);
}
/* Handler list helpers. */
static uint32_t node(unsigned i) { return NODES+0x40*i; }
static void make_handler(unsigned i,uint32_t flags,uint8_t owner,uint8_t value,unsigned entries,const uint8_t *events,const uint32_t *functions)
{
    const uint32_t h=node(i),e=ENTRY+0x100*i;
    w32(NULL,h+8,e);w8(NULL,h+0x10,owner);w32(NULL,h+0x18,flags|(uint32_t)entries<<16);w8(NULL,h+0x3a,value);
    for(unsigned k=0;k<entries;++k)w8(NULL,e+8*k,events[k]),w32(NULL,e+8*k+4,functions[k]);
}
static void link_handlers(const unsigned *order,unsigned n)
{
    w32(NULL,BATTLE+0x60,n?node(order[0]):0);
    for(unsigned k=0;k<n;++k){w32(NULL,node(order[k]),k?node(order[k-1]):0);w32(NULL,node(order[k])+4,k+1<n?node(order[k+1]):0);}
}
#define REG 0x20000000u
static int expect_eligible(uint32_t f,uint32_t event,uint32_t owner,uint8_t node_owner,uint32_t d)
{
    const int running=(f>>24)&1,reentry=(f>>28)&1,off=((f>>25)&1)|((f>>30)&1),reg=(f>>29)&1,acc=(f>>26)&1;
    const uint32_t level=f&0xffff;
    return (!running||reentry)&&!off&&(owner==7||node_owner==owner)&&(level==0||d>level)&&reg&&(event!=0x7c||acc);
}
int main(void)
{
    unsigned cases=0;
    /* Exact push/set/get/pop layout. */
    reset();px_battle_event_push(&api);CHECK(r32(NULL,STACK)==1&&key(0)==1);
    px_battle_event_set_id(3,0x11,&api);px_battle_event_set_value(0x39,0xe,&api);px_battle_event_set_scaled(0x34,0x99,&api);
    px_battle_event_set_multiplier(0x36,0x1000,0x19a,0x20000,&api);
    CHECK(key(1)==3&&key(2)==0x39&&key(3)==0x34&&key(4)==0x36&&r32(NULL,STACK)==1);
    CHECK(r8(NULL,STACK+0x545)==4&&r8(NULL,STACK+0x546)==1&&r8(NULL,STACK+0x547)==0&&r8(NULL,STACK+0x548)==3);
    CHECK(r32(NULL,STACK+0xc4+16)==0x1000&&r32(NULL,STACK+0x3c4+16)==0x19a&&r32(NULL,STACK+0x244+16)==0x20000);
    CHECK(px_battle_event_get(0x34,&api)==0x99&&px_battle_event_get(3,&api)==0x11&&aborts==0);
    px_battle_event_pop(&api);CHECK(r32(NULL,STACK)==0);for(unsigned i=0;i<6;++i)CHECK(key(i)==0);++cases;
    /* Random valid sequences against the frame model. */
    for(unsigned run=0;run<400;++run) {
        reset();depth=0;memset(frames,0,sizeof frames);
        for(unsigned step=0;step<200;++step) {
            const uint32_t op=rnd(10);
            if(op<2&&used()<90) { px_battle_event_push(&api);frames[depth++].n=0; }
            else if(op<4&&depth>0) { px_battle_event_pop(&api);--depth; }
            else if(op<8&&depth>0&&used()<95) {
                struct frame *f=&frames[depth-1];uint32_t k;int dup;
                do { k=2+rnd(0x60);dup=0;for(unsigned i=0;i<f->n;++i)dup|=f->keys[i]==k; } while(dup);
                const uint32_t v=rnd(0x10000)*0x10001u,kind=rnd(4);
                f->keys[f->n]=k;f->values[f->n]=v;f->mins[f->n]=0;f->maxs[f->n]=0;
                switch(kind) {
                case 0: px_battle_event_set_scaled(k,v,&api);f->kinds[f->n]=0;break;
                case 1: px_battle_event_set_value(k,v,&api);f->kinds[f->n]=1;break;
                case 2: px_battle_event_set_id(k,v,&api);f->kinds[f->n]=4;break;
                default: f->mins[f->n]=v^1;f->maxs[f->n]=v^2;px_battle_event_set_multiplier(k,v,v^1,v^2,&api);f->kinds[f->n]=3;break;
                }
                ++f->n;
            } else if(depth>0&&frames[depth-1].n>0) {
                const struct frame *f=&frames[depth-1];const unsigned i=rnd(f->n);
                CHECK(px_battle_event_get(f->keys[i],&api)==f->values[i]);
            }
            CHECK(aborts==0&&overflows==0);check_layout();
        }
        ++cases;
    }
    /* Only the current frame is visible; outer values return after the pop. */
    reset();px_battle_event_push(&api);px_battle_event_set_value(5,50,&api);px_battle_event_push(&api);px_battle_event_set_value(6,60,&api);
    CHECK(px_battle_event_get(6,&api)==60&&aborts==0);CHECK(px_battle_event_get(5,&api)==0&&aborts==1);
    px_battle_event_pop(&api);CHECK(r32(NULL,STACK)==1&&px_battle_event_get(5,&api)==50&&aborts==1&&key(2)==0&&key(3)==0);++cases;
    /* Reserved keys abort but are still used. */
    for(uint32_t k=0;k<2;++k) {
        reset();px_battle_event_push(&api);px_battle_event_set_value(k,9,&api);CHECK(aborts==1&&key(1)==k&&r32(NULL,STACK+0xc8)==9);
        /* Key 0 matches the first empty slot (value 0) before the empty test; key 1 runs off the frame. */
        reset();px_battle_event_push(&api);px_battle_event_set_value(7,70,&api);CHECK(px_battle_event_get(k,&api)==0&&aborts==(k?2u:1u));++cases;
    }
    /* Key 1 written as an entry, then read back: get returns it (key 1 aborts first). */
    reset();px_battle_event_push(&api);px_battle_event_set_id(1,0x42,&api);CHECK(px_battle_event_get(1,&api)==0x42&&aborts==2);++cases;
    /* A duplicate key aborts and overwrites the existing entry. */
    reset();px_battle_event_push(&api);px_battle_event_set_value(9,1,&api);px_battle_event_set_value(8,2,&api);px_battle_event_set_scaled(9,3,&api);
    CHECK(aborts==1&&key(1)==9&&key(3)==0&&r32(NULL,STACK+0xc8)==3&&r8(NULL,STACK+0x545)==0&&px_battle_event_get(9,&api)==3);++cases;
    /* A full stack aborts and writes slot 0 (over the first marker). */
    reset();px_battle_event_push(&api);for(uint32_t k=2;k<2+95;++k)px_battle_event_set_value(k,k,&api);CHECK(aborts==0&&key(95)==96);
    px_battle_event_set_multiplier(0x200,5,6,7,&api);CHECK(aborts==1&&key(0)==0x200&&r32(NULL,STACK+0xc4)==5&&r32(NULL,STACK+0x3c4)==6&&r32(NULL,STACK+0x244)==7);++cases;
    /* get past the end of a full frame aborts and returns 0. */
    reset();px_battle_event_push(&api);for(uint32_t k=2;k<2+95;++k)px_battle_event_set_value(k,k,&api);
    CHECK(px_battle_event_get(0x300,&api)==0&&aborts==1&&px_battle_event_get(96,&api)==96);++cases;
    /* Push skips the current entries and stops one short of the end. */
    reset();w32(NULL,STACK,0x5e);px_battle_event_push(&api);CHECK(overflows==0&&key(0x5e)==1&&r32(NULL,STACK)==0x5f);++cases;
    reset();w32(NULL,STACK,0x5f);px_battle_event_push(&api);CHECK(overflows==1&&key(0x5f)==0&&r32(NULL,STACK)==0x5f);++cases;
    reset();w32(NULL,STACK,0x5d);w16(NULL,STACK+4+2*0x5d,7);px_battle_event_push(&api);CHECK(overflows==0&&r32(NULL,STACK)==0x5f&&key(0x5e)==1);++cases;
    reset();w32(NULL,STACK,0x5d);w16(NULL,STACK+4+2*0x5d,7);w16(NULL,STACK+4+2*0x5e,8);px_battle_event_push(&api);
    CHECK(overflows==1&&r32(NULL,STACK)==0x5f);++cases;
    reset();w32(NULL,STACK,0x5e);for(unsigned i=0x5e;i<0x60;++i)w16(NULL,STACK+4+2*i,7);px_battle_event_push(&api);
    CHECK(overflows==1&&r32(NULL,STACK)==0x60);++cases;   /* the position is stored past the end */
    reset();w32(NULL,STACK,0x70);px_battle_event_push(&api);CHECK(overflows==1&&r32(NULL,STACK)==0x70);++cases;
    /* Pop at position zero aborts and changes nothing. */
    reset();w16(NULL,STACK+4,1);px_battle_event_pop(&api);CHECK(aborts==1&&key(0)==1&&r32(NULL,STACK)==0);++cases;
    /* Pop with no earlier marker returns to zero; markers inside a frame are found. */
    reset();w32(NULL,STACK,3);w16(NULL,STACK+4,5);w16(NULL,STACK+6,6);w16(NULL,STACK+8,1);w16(NULL,STACK+10,9);
    px_battle_event_pop(&api);CHECK(r32(NULL,STACK)==0&&key(2)==0&&key(3)==0&&key(0)==5);++cases;
    reset();w32(NULL,STACK,4);w16(NULL,STACK+4,1);w16(NULL,STACK+6,6);w16(NULL,STACK+8,7);w16(NULL,STACK+10,1);w16(NULL,STACK+12,9);w16(NULL,STACK+14,9);
    px_battle_event_pop(&api);CHECK(r32(NULL,STACK)==1&&key(3)==0&&key(4)==0&&key(5)==0&&key(2)==7);++cases;
    /* Handlers: every flag combination for one handler and one matching entry. */
    const uint32_t bits[8]={0x1000000,0x2000000,0x4000000,0x10000000,0x20000000,0x40000000,0,0};
    for(unsigned mask=0;mask<64;++mask)for(unsigned ev=0;ev<2;++ev)for(unsigned own=0;own<3;++own)for(unsigned lv=0;lv<3;++lv)for(unsigned d=0;d<3;++d) {
        reset();uint32_t f=lv;for(unsigned b=0;b<6;++b)if(mask>>b&1)f|=bits[b];
        const uint8_t events[1]={(uint8_t)(ev?0x7c:0x44)};const uint32_t fns[1]={1};
        make_handler(0,f,2,0x5a,1,events,fns);const unsigned order[1]={0};link_handlers(order,1);
        w32(NULL,BATTLE+0x68,d);
        const uint32_t owner=own==0?7:own==1?2:3;
        px_battle_event_run_handlers(CTX,events[0],owner,0,&api);
        const int want=expect_eligible(f,events[0],owner,2,d);
        CHECK((strstr(log_,"C1@")!=NULL)==want);
        if(want) { char b[48];snprintf(b,sizeof b,"C1@%x/5a/d%x ",node(0),d);CHECK(!strcmp(log_,b)); }
        /* Running clears its own running bit (or the re-entry bit if set). */
        const uint32_t after=r32(NULL,node(0)+0x18);
        CHECK(after==(want?((f&0x10000000)?((f|0x1000000)&~0x10000000u):(f&~0x1000000u)):f)+(1u<<16));
        ++cases;
    }
    /* Entry search: count from the flags, first matching event only, blocked check. */
    for(unsigned n=0;n<5;++n)for(unsigned at=0;at<5;++at)for(unsigned chk=0;chk<2;++chk)for(unsigned blk=0;blk<2;++blk) {
        reset();uint8_t events[5]={0x10,0x11,0x12,0x13,0x14};const uint32_t fns[5]={1,2,3,4,5};
        events[at]=0x30;if(at+1<5)events[at+1]=0x30;
        make_handler(0,REG,0,0,n,events,fns);const unsigned order[1]={0};link_handlers(order,1);block_result=blk;
        px_battle_event_run_handlers(CTX,0x30,7,chk,&api);
        const int found=at<n;
        CHECK(blocked_calls==(unsigned)(found&&chk));
        if(found&&!(chk&&blk)) { char b[32];snprintf(b,sizeof b,"C%x@",at+1);CHECK(strstr(log_,b)&&calls()==1); }
        else CHECK(strstr(log_,"C")==NULL);
        ++cases;
    }
    /* The entry count is the full byte at bits 16..23. */
    for(unsigned n=8;n<=0xff;n+=0x77) {
        reset();uint8_t events[0xff];uint32_t fns[0xff];
        for(unsigned i=0;i<n;++i)events[i]=(uint8_t)(0x80+i%0x70),fns[i]=i%16;
        events[n-1]=0x30;fns[n-1]=9;
        make_handler(0,REG,0,0,n,events,fns);const unsigned order[1]={0};link_handlers(order,1);
        px_battle_event_run_handlers(CTX,0x30,7,0,&api);CHECK(calls()==1&&strstr(log_,"C9@"));++cases;
    }
    /* Order, arguments and the visit mark over three handlers. */
    {
        reset();const uint8_t e[1]={0x44};const uint32_t f1[1]={1},f2[1]={2},f3[1]={3};
        make_handler(0,REG,0,1,1,e,f1);make_handler(1,REG,0,2,1,e,f2);make_handler(2,REG,0,3,1,e,f3);
        const unsigned order[3]={2,0,1};link_handlers(order,3);
        px_battle_event_dispatch(CTX,0x44,&api);
        char b[160];snprintf(b,sizeof b,"B%x:44 C3@%x/3/d1 B%x:44 C1@%x/1/d1 B%x:44 C2@%x/2/d1 ",node(2),node(2),node(0),node(0),node(1),node(1));
        CHECK(!strcmp(log_,b)&&r32(NULL,BATTLE+0x68)==0);
        for(unsigned i=0;i<3;++i) { CHECK(r32(NULL,node(i)+0x18)==(REG|1u<<16)); }
        ++cases;
    }
    /* A visited successor that stays marked is resumed; an unmarked one continues through the link. */
    {
        reset();const uint8_t e[1]={0x44};const uint32_t f1[1]={1},f2[1]={2},f3[1]={3};
        make_handler(0,REG,0,0,1,e,f1);make_handler(1,REG,0,0,1,e,f2);make_handler(2,REG,0,0,1,e,f3);
        const unsigned order[3]={0,1,2};link_handlers(order,3);
        actions[1].relink=1;actions[1].relink_to=node(2);   /* handler 0 relinks past 1, but 1 is still marked */
        px_battle_event_run_handlers(CTX,0x44,7,0,&api);
        char b[96];snprintf(b,sizeof b,"C1@%x/0/d0 C2@%x/0/d0 C3@%x/0/d0 ",node(0),node(1),node(2));CHECK(!strcmp(log_,b));++cases;
        /* Clearing the mark makes the walk follow the new link. */
        reset();make_handler(0,REG,0,0,1,e,f1);make_handler(1,REG,0,0,1,e,f2);make_handler(2,REG,0,0,1,e,f3);link_handlers(order,3);
        actions[1].relink=1;actions[1].relink_to=node(2);actions[1].clear_on=node(1);actions[1].clear_bits=0x80000000u;
        px_battle_event_run_handlers(CTX,0x44,7,0,&api);
        snprintf(b,sizeof b,"C1@%x/0/d0 C3@%x/0/d0 ",node(0),node(2));CHECK(!strcmp(log_,b));++cases;
    }
    /* An unregistered successor is not resumed: the walk follows the link from the current handler. */
    {
        reset();const uint8_t e[1]={0x44};const uint32_t f1[1]={1},f2[1]={2},f3[1]={3};
        make_handler(0,REG,0,0,1,e,f1);make_handler(1,0,0,0,1,e,f2);make_handler(2,REG,0,0,1,e,f3);
        const unsigned order[3]={0,1,2};link_handlers(order,3);
        px_battle_event_run_handlers(CTX,0x44,7,0,&api);
        char b[96];snprintf(b,sizeof b,"C1@%x/0/d0 C3@%x/0/d0 ",node(0),node(2));CHECK(!strcmp(log_,b));
        CHECK(r32(NULL,node(1)+0x18)==(0x80000000u|1u<<16)&&r32(NULL,node(2)+0x18)==(REG|1u<<16));++cases;
    }
    /* Removal requested by the running handler: unlink, clear and free (head, middle, tail). */
    for(unsigned who=0;who<3;++who) {
        reset();const uint8_t e[1]={0x44};const uint32_t f1[1]={1},f2[1]={2},f3[1]={3};
        make_handler(0,REG,0,0x33,1,e,f1);make_handler(1,REG,0,0x33,1,e,f2);make_handler(2,REG,0,0x33,1,e,f3);
        const unsigned order[3]={0,1,2};link_handlers(order,3);w32(NULL,BATTLE+0x64,4);
        actions[who+1].set_on=node(who);actions[who+1].set_bits=0x8000000;
        px_battle_event_run_handlers(CTX,0x44,7,0,&api);
        for(uint32_t o=0;o<0x3c;o+=4) { CHECK(r32(NULL,node(who)+o)==0); }
        CHECK(r32(NULL,BATTLE+0x64)==3&&r32(NULL,FREE+12)==node(who)&&aborts==0);
        unsigned rest[2],n=0;for(unsigned i=0;i<3;++i)if(i!=who)rest[n++]=i;
        CHECK(r32(NULL,BATTLE+0x60)==node(rest[0])&&r32(NULL,node(rest[0]))==0&&r32(NULL,node(rest[0])+4)==node(rest[1]));
        CHECK(r32(NULL,node(rest[1]))==node(rest[0])&&r32(NULL,node(rest[1])+4)==0);
        /* Removing the last of three ends the walk; earlier removals continue with the marked successor. */
        CHECK(calls()==3);++cases;
    }
    /* No free slot: abort, but the handler stays unlinked and is not cleared. */
    {
        reset();const uint8_t e[1]={0x44};const uint32_t f1[1]={1};make_handler(0,REG,0,0,1,e,f1);const unsigned order[1]={0};link_handlers(order,1);
        actions[1].set_on=node(0);actions[1].set_bits=0x8000000;
        px_battle_event_run_handlers(CTX,0x44,7,0,&api);
        CHECK(aborts==1&&r32(NULL,BATTLE+0x60)==0&&r32(NULL,node(0)+8)==ENTRY&&(r32(NULL,node(0)+0x18)&0x8000000));++cases;
    }
    /* A removal request on an unregistered handler is ignored. */
    {
        reset();const uint8_t e[1]={0x44};const uint32_t f1[1]={1};make_handler(0,REG,0,0,1,e,f1);const unsigned order[1]={0};link_handlers(order,1);
        actions[1].clear_on=node(0);actions[1].clear_bits=REG;actions[1].set_on=node(0);actions[1].set_bits=0x8000000;w32(NULL,BATTLE+0x64,1);
        px_battle_event_run_handlers(CTX,0x44,7,0,&api);CHECK(r32(NULL,BATTLE+0x60)==node(0)&&r32(NULL,BATTLE+0x64)==1);++cases;
    }
    /* Removal requested in a nested (re-entered) call is deferred to the outer call. */
    {
        reset();const uint8_t e[2]={0x44,0x45};const uint32_t f[2]={1,2};make_handler(0,REG,0,0,2,e,f);
        const unsigned order[1]={0};link_handlers(order,1);w32(NULL,BATTLE+0x64,2);
        actions[1].set_on=node(0);actions[1].set_bits=0x10000000;actions[1].nested_event=0x45;
        actions[2].set_on=node(0);actions[2].set_bits=0x8000000;
        px_battle_event_dispatch(CTX,0x44,&api);
        /* Inner call clears re-entry and sees running: it records the request; the outer call then frees it. */
        char b[128];snprintf(b,sizeof b,"B%x:44 C1@%x/0/d1 B%x:45 C2@%x/0/d2 F29020000 ",node(0),node(0),node(0),node(0));CHECK(!strcmp(log_,b));
        CHECK(r32(NULL,BATTLE+0x60)==0&&r32(NULL,BATTLE+0x64)==1&&r32(NULL,FREE+4)==node(0)&&r32(NULL,node(0)+0x18)==0);++cases;
    }
    /* A running handler is not re-entered without the re-entry flag. */
    {
        reset();const uint8_t e[2]={0x44,0x45};const uint32_t f[2]={1,2};make_handler(0,REG,0,0,2,e,f);
        const unsigned order[1]={0};link_handlers(order,1);actions[1].nested_event=0x45;
        px_battle_event_dispatch(CTX,0x44,&api);CHECK(strstr(log_,"C2@")==NULL&&strstr(log_,"C1@"));++cases;
    }
    /* Removal with an unregistered successor: a handler that relinks to a registered one continues there. */
    {
        reset();const uint8_t e[1]={0x44};const uint32_t f1[1]={1},f2[1]={2},f3[1]={3};
        make_handler(0,REG,0,0,1,e,f1);make_handler(1,0,0,0,1,e,f2);make_handler(2,REG,0,0,1,e,f3);
        const unsigned order[3]={0,1,2};link_handlers(order,3);w32(NULL,BATTLE+0x64,1);
        actions[1].set_on=node(0);actions[1].set_bits=0x8000000;actions[1].relink=1;actions[1].relink_to=node(2);
        px_battle_event_run_handlers(CTX,0x44,7,0,&api);
        /* Handler 0 is freed; its successor search found handler 2 and marked it; unlinking used the new link. */
        CHECK(r32(NULL,FREE)==node(0)&&r32(NULL,BATTLE+0x60)==node(2));
        char b[64];snprintf(b,sizeof b,"C1@%x/0/d0 C3@%x/0/d0 ",node(0),node(2));CHECK(!strcmp(log_,b));++cases;
        /* Relinking to nothing ends the search and the walk. */
        reset();make_handler(0,REG,0,0,1,e,f1);make_handler(1,0,0,0,1,e,f2);make_handler(2,REG,0,0,1,e,f3);link_handlers(order,3);w32(NULL,BATTLE+0x64,1);
        actions[1].set_on=node(0);actions[1].set_bits=0x8000000;actions[1].relink=1;actions[1].relink_to=0;
        px_battle_event_run_handlers(CTX,0x44,7,0,&api);
        CHECK(calls()==1&&r32(NULL,FREE)==node(0)&&r32(NULL,BATTLE+0x60)==0&&r32(NULL,node(1)+0x18)==(0x80000000u|1u<<16));++cases;
    }
    /* Dispatch: depth counting and clearing registration depths only at the outermost level. */
    {
        reset();const uint8_t e[2]={0x44,0x45};const uint32_t f[2]={1,2};
        make_handler(0,REG|0x10000000,0,0,2,e,f);make_handler(1,REG|2,0,0,1,e+1,f+1);
        const unsigned order[2]={0,1};link_handlers(order,2);
        actions[1].nested_event=0x45;actions[1].set_on=node(0);actions[1].set_bits=0x10000000;
        px_battle_event_dispatch(CTX,0x44,&api);
        /* Handler 1 (registered at depth 2) does not run at depth 2 inside the nested dispatch. */
        CHECK(strstr(log_,"C2@")!=NULL);
        char b[32];snprintf(b,sizeof b,"C2@%x",node(1));CHECK(strstr(log_,b)==NULL);
        CHECK(r32(NULL,BATTLE+0x68)==0&&r16(NULL,node(1)+0x18)==0&&r16(NULL,node(0)+0x18)==0);++cases;
        reset();make_handler(0,REG|5,0,0,1,e,f);link_handlers(order,1);w32(NULL,BATTLE+0x68,3);
        px_battle_event_dispatch(CTX,0x44,&api);CHECK(r32(NULL,BATTLE+0x68)==3&&r16(NULL,node(0)+0x18)==5&&strstr(log_,"C1@")==NULL);++cases;
        reset();make_handler(0,REG|5,0,0,1,e,f);link_handlers(order,1);w32(NULL,BATTLE+0x68,5);
        px_battle_event_dispatch(CTX,0x44,&api);CHECK(r32(NULL,BATTLE+0x68)==5&&strstr(log_,"C1@"));++cases;
        reset();px_battle_event_dispatch(CTX,0x44,&api);CHECK(r32(NULL,BATTLE+0x68)==0&&log_[0]==0);++cases;
        reset();make_handler(0,REG|5,0,0,1,e,f);link_handlers(order,1);w32(NULL,BATTLE+0x68,1);
        px_battle_event_dispatch(CTX,0x44,&api);CHECK(r32(NULL,BATTLE+0x68)==1&&r16(NULL,node(0)+0x18)==5);++cases;
    }
    printf("battle_event: %u cases, all tests passed\n",cases);
    return 0;
}
