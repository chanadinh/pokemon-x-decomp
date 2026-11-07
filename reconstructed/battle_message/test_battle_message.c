#include "battle_message.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { CTX=0x100, QUEUE=0x1000, DATA=QUEUE+8, MSG=0x400, ARGS=0x500 };
static uint8_t mem[0x8000];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<sizeof mem);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<sizeof mem);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static unsigned faults,overflows;
static void fault(void *u) { (void)u;++faults; }
static void overflow(void *u) { (void)u;++overflows; }
static const struct px_battle_command_api api={&memory,fault,overflow,0,0};
static void reset(void) { memset(mem,0,sizeof mem);faults=overflows=0;w32(NULL,CTX+0xc,QUEUE); }
static uint32_t used(void) { return r32(NULL,QUEUE); }
/* The queue as expected: group, id, then words. */
static int queued(uint32_t group,uint32_t id,const uint32_t *words,unsigned n)
{
    uint8_t want[64];unsigned k=0;
    want[k++]=0;want[k++]=(uint8_t)group;want[k++]=(uint8_t)(id>>8);want[k++]=(uint8_t)id;
    for(unsigned i=0;i<n;++i)for(int b=3;b>=0;--b)want[k++]=(uint8_t)(words[i]>>(8*b));
    return used()==k&&memcmp(mem+DATA,want,k)==0;
}
#define END PX_BATTLE_MESSAGE_END
static void message(uint32_t id,uint32_t flags,const uint32_t *args,unsigned n,uint32_t extra)
{
    w16(NULL,MSG,(uint16_t)id);w16(NULL,MSG+2,(uint16_t)flags);
    for(unsigned i=0;i<8;++i)w32(NULL,MSG+4+4*i,i<n?args[i]:0xdead0000u+i);
    w32(NULL,MSG+0x20,extra);
}

int main(void)
{
    unsigned cases=0;
    /* The command: words through the terminator; group 0x6b's first word ahead of the list. */
    reset();{const uint32_t a[]={1,2,END,9};px_battle_message_command(QUEUE,0x6a,0x1234,a,4,&api);const uint32_t w[]={1,2,END};CHECK(queued(0x6a,0x1234,w,3));}++cases;
    reset();{const uint32_t a[]={END,7,END};px_battle_message_command(QUEUE,0x6b,0x55,a,3,&api);const uint32_t w[]={END,7,END};CHECK(queued(0x6b,0x55,w,3));}++cases;   /* first word even when it is the terminator */
    reset();{const uint32_t a[]={END,7,END};px_battle_message_command(QUEUE,0x6c,0x55,a,3,&api);const uint32_t w[]={END};CHECK(queued(0x6c,0x55,w,1));}++cases;
    reset();{const uint32_t a[]={3};px_battle_message_command(QUEUE,0x169,0x12345,a,1,&api);const uint32_t w[]={3};CHECK(queued(0x69,0x2345,w,1));}++cases;   /* byte group, u16 id, no terminator */
    /* Overflow: each word is checked once at 0x176d. */
    for(uint32_t at=0x1766;at<=0x176e;++at){
        reset();w32(NULL,QUEUE,at);const uint32_t a[]={END};px_battle_message_command(QUEUE,0x69,1,a,1,&api);
        CHECK(overflows==(unsigned)((at>=0x176f)+(at+2>=0x176f)+(at+4>=0x176d))&&used()==at+8);++cases;
    }
    /* Plain messages: 0-4 arguments (others send none). */
    for(uint32_t n=0;n<7;++n){
        reset();for(uint32_t i=0;i<6;++i)w32(NULL,ARGS+4*i,0x100+i);
        px_battle_message_plain(CTX,0x321,n,ARGS,&api);
        uint32_t w[6];unsigned k=0;if(n>=1&&n<=4)for(uint32_t i=0;i<n;++i)w[k++]=0x100+i;w[k++]=END;
        CHECK(queued(0x69,0x321,w,k));++cases;
    }
    /* Shown messages: every kind, count and flag. */
    for(uint32_t ext=0;ext<2;++ext)for(uint32_t kind=0;kind<4;++kind)for(uint32_t n=0;n<7;++n){
        reset();const uint32_t a[]={0x11,0x22,0x33,0x44,0x55,0x66};
        message(0x777,kind|n<<7|(ext?0x4000u:0)|0x8000u,a,6,0x9999);
        const uint32_t r=px_battle_show_message(CTX,MSG,&api);
        if(kind!=1&&kind!=2){CHECK(r==0&&used()==0);++cases;continue;}
        uint32_t w[8];unsigned k=0;
        if(ext)w[k++]=0x9999;
        if(n>=1&&n<=4)for(uint32_t i=0;i<n;++i)w[k++]=a[i];
        w[k++]=END;
        const uint32_t group=ext?(kind==1?0x6b:0x6c):(kind==1?0x69:0x6a);
        CHECK(r==1&&queued(group,0x777,w,k));++cases;
    }
    /* Kind bits only (bit 7 up is the count). */
    reset();message(1,0x81,NULL,0,0);w32(NULL,MSG+4,5);CHECK(px_battle_show_message(CTX,MSG,&api)==1&&queued(0x69,1,(const uint32_t[]){5,END},2));++cases;
    reset();message(1,0x3,NULL,0,0);CHECK(px_battle_show_message(CTX,MSG,&api)==0);++cases;
    /* An extra word equal to the terminator ends group 0x6c at once but not 0x6b. */
    reset();message(2,0x4000|1<<7|2,(const uint32_t[]){7},1,END);px_battle_show_message(CTX,MSG,&api);CHECK(queued(0x6c,2,(const uint32_t[]){END},1));++cases;
    reset();message(2,0x4000|1<<7|1,(const uint32_t[]){7},1,END);px_battle_show_message(CTX,MSG,&api);CHECK(queued(0x6b,2,(const uint32_t[]){END,7,END},3));++cases;
    CHECK(faults==0);
    printf("battle_message: %u cases, all tests passed\n",cases);
    return 0;
}
