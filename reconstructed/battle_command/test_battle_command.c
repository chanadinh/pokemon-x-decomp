#include "battle_command.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x4000, QUEUE=0x1000, DATA=QUEUE+8, INFO=0x100, BUFFER=0x200, ARGS=0x300 };
static uint8_t mem[SIZE*2];
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
static const struct px_battle_command_api api={&memory,fault,overflow,INFO,BUFFER};
/* The queue is placed so a full 6000-byte queue still fits in memory. */
static void reset(void) { memset(mem,0,sizeof mem);faults=overflows=0; }
static uint32_t used(void) { return r32(NULL,QUEUE); }
/* Encode format info with arguments a[], compare the bytes after the id. */
static void encode(uint32_t info,const uint32_t *a,unsigned n) { reset();for(unsigned i=0;i<n;++i)w32(NULL,ARGS+4*i,a[i]);px_battle_command_encode(QUEUE,0x1234,info,ARGS,&api); }
static int bytes(const uint8_t *want,unsigned n)
{
    if(used()!=2+n||mem[DATA]!=0x12||mem[DATA+1]!=0x34)return 0;
    return memcmp(mem+DATA+2,want,n)==0;
}
#define EXPECT(...) do { static const uint8_t w_[]={__VA_ARGS__}; CHECK(bytes(w_,sizeof w_)&&faults==0&&overflows==0); ++cases; } while (0)

int main(void)
{
    unsigned cases=0;
    /* Writers: big-endian, one overflow check per value at its own limit. */
    reset();px_battle_queue_u16(QUEUE,0xabcd,&api);px_battle_queue_u24(QUEUE,0x123456,&api);px_battle_queue_u32(QUEUE,0x89abcdef,&api);
    CHECK(used()==9&&!memcmp(mem+DATA,"\xab\xcd\x12\x34\x56\x89\xab\xcd\xef",9)&&overflows==0);++cases;
    static const struct { void (*f)(uint32_t,uint32_t,const struct px_battle_command_api*); uint32_t limit,bytes; } writers[3]={
        {px_battle_queue_u16,0x176f,2},{px_battle_queue_u24,0x176e,3},{px_battle_queue_u32,0x176d,4}};
    for(unsigned k=0;k<3;++k)for(uint32_t at=writers[k].limit-1;at<=writers[k].limit+1;++at){
        reset();w32(NULL,QUEUE,at);writers[k].f(QUEUE,0xffffffff,&api);
        CHECK(overflows==(at>=writers[k].limit)&&used()==at+writers[k].bytes&&mem[DATA+at+writers[k].bytes-1]==0xff);++cases;
    }
    /* Inline bytes overflow from 0x1770 on (format 1). */
    for(uint32_t at=0x176c;at<=0x1771;++at){reset();w32(NULL,QUEUE,at);w32(NULL,ARGS,0x5a);px_battle_command_encode(QUEUE,1,1,ARGS,&api);
        CHECK(overflows==(unsigned)((at>=0x176f)+(at+2>=0x1770))&&mem[DATA+at+2]==0x5a);++cases;}
    /* Packers. */
    CHECK(px_battle_pack2(0x1f,7,5,3,&api)==0xff&&px_battle_pack2(3,2,5,3,&api)==0x1a&&px_battle_pack2(9,5,4,4,&api)==0x95&&faults==0);++cases;
    CHECK(px_battle_pack3(2,1,2,3,5,5,5,&api)==(1u<<10|2u<<5|3)&&px_battle_pack3(3,0x1f,0x1f,0x3fff,5,5,0xe,&api)==0xffffff&&faults==0);++cases;
    reset();px_battle_pack2(0x20,0,5,3,&api);CHECK(faults==1);++cases;   /* field too wide */
    reset();px_battle_pack2(0,8,5,3,&api);CHECK(faults==1);++cases;
    reset();CHECK(px_battle_pack2(0x3f,7,6,3,&api)==0xff&&faults==1);++cases;      /* widths over 8: still one byte */
    reset();px_battle_pack2(0,0,0,3,&api);CHECK(faults==1);++cases;
    reset();px_battle_pack2(0,0,3,0,&api);CHECK(faults==1);++cases;
    reset();CHECK(px_battle_pack2(-1,0,5,3,&api)==0xf8&&faults==0);++cases;   /* signed: -1 passes the check, then masks */
    reset();px_battle_pack3(2,0,0,0,5,5,7,&api);CHECK(faults==1);++cases;     /* 7 + 10 > 16 */
    reset();px_battle_pack3(2,0,0,0,5,5,6,&api);CHECK(faults==0);++cases;
    reset();px_battle_pack3(2,0x20,0,0,5,5,5,&api);CHECK(faults==1);++cases;
    reset();px_battle_pack3(2,0,0x20,0,5,5,5,&api);CHECK(faults==1);++cases;
    reset();px_battle_pack3(2,0,0,0x20,5,5,5,&api);CHECK(faults==1);++cases;
    reset();px_battle_pack3(2,0,0,0,0,5,5,&api);CHECK(faults==1);++cases;
    reset();px_battle_pack3(2,0,0,0,5,0,5,&api);CHECK(faults==1);++cases;
    reset();px_battle_pack3(2,0,0,0,5,5,0,&api);CHECK(faults==1);++cases;     /* zero width; 0 passes the mask */
    /* Every format. */
    {const uint32_t a[]={0x1aa};encode(0x01,a,1);EXPECT(0xaa);}
    {const uint32_t a[]={0x11,0x22};encode(0x02,a,2);EXPECT(0x11,0x22);}
    {const uint32_t a[]={3,2,0x77};encode(0x03,a,3);EXPECT(0x1a,0x77);}
    {const uint32_t a[]={3,2,0x77,0x1234};encode(0x04,a,4);EXPECT(0x1a,0x77,0x12,0x34);}
    {const uint32_t a[]={1,2,3,0xbeef,0x10002};encode(0x05,a,5);EXPECT(0x04,0x43,0xbe,0xef,0x00,0x02);}
    {const uint32_t a[]={1,2,3,4,5,6};encode(0x06,a,6);EXPECT(0x04,0x43,0x10,0xa6);}
    for(uint32_t n=7;n<=9;++n){const uint32_t a[]={1,2,3,4,5,6,7,8,9};encode(n,a,9);CHECK(used()==2+n);for(uint32_t i=0;i<n;++i)CHECK(mem[DATA+2+i]==i+1);++cases;}
    {const uint32_t a[]={0x12345};encode(0x11,a,1);EXPECT(0x23,0x45);}
    {const uint32_t a[]={0x9,0x1234};encode(0x12,a,2);EXPECT(0x09,0x12,0x34);}
    {const uint32_t a[]={3,2,0x1234};encode(0x13,a,3);EXPECT(0x1a,0x12,0x34);}
    {const uint32_t a[]={3,2,4,5};encode(0x14,a,4);EXPECT(0x1a,0x25);}
    {const uint32_t a[]={3,2,0x7f,1,0xcafe};encode(0x15,a,5);EXPECT(0x00,0x1a,0x00,0xff,0xca,0xfe);}
    {const uint32_t a[]={5,2,1,0,1,0x4321};encode(0x16,a,6);EXPECT(0xb5,0x43,0x21);}
    {const uint32_t a[]={0xdeadbeef};encode(0x21,a,1);EXPECT(0xde,0xad,0xbe,0xef);}
    {const uint32_t a[]={7,0x01020304};encode(0x22,a,2);EXPECT(0x07,0x01,0x02,0x03,0x04);}
    {const uint32_t a[]={1,2,3};encode(0x23,a,3);EXPECT(0x04,0x43);}
    {const uint32_t a[]={1,2,3,9};encode(0x24,a,4);EXPECT(0x08,0x80,0x03,0x09);}
    {const uint32_t a[]={5,6,1,1,0x55aa};encode(0x25,a,5);EXPECT(0xbb,0x55,0xaa);}
    {const uint32_t a[]={1,2,3,4,0x1111,0x2222};encode(0x26,a,6);EXPECT(0x00,0x88,0x64,0x11,0x11,0x22,0x22);}
    {encode(0x30,NULL,0);CHECK(used()==2&&faults==0);++cases;}
    {const uint32_t a[]={9,5};encode(0x32,a,2);EXPECT(0x95);}
    {const uint32_t a[]={1,2,3};encode(0x33,a,3);EXPECT(0x08,0x80,0x03);}
    {const uint32_t a[]={1,2,3,0xabcd};encode(0x34,a,4);EXPECT(0x08,0x80,0x03,0xab,0xcd);}
    {const uint32_t a[]={1,2,3,0x7f,1};encode(0x35,a,5);EXPECT(0x08,0x80,0x03,0xff);}
    {const uint32_t a[]={1,2,3,4,5,6};encode(0x36,a,6);EXPECT(0x08,0x83,0x00,0x04,0x00,0x05,0x00,0x06);}
    {const uint32_t a[]={3,2};encode(0x42,a,2);EXPECT(0x1a);}
    {const uint32_t a[]={1,2,0x0304};encode(0x43,a,3);EXPECT(0x01,0x02,0x03,0x04);}
    {const uint32_t a[]={1,2,3,0x0405};encode(0x44,a,4);EXPECT(0x08,0x83,0x04,0x05);}
    {const uint32_t a[]={1,2,0x03040506};encode(0x53,a,3);EXPECT(0x01,0x02,0x03,0x04,0x05,0x06);}
    {const uint32_t a[]={1,2,0x0304,0x0506};encode(0x54,a,4);EXPECT(0x01,0x02,0x03,0x04,0x05,0x06);}
    /* Field checks of the bit formats. */
    {const uint32_t a[]={8,0,0,0,0,0};encode(0x16,a,6);CHECK(faults==1);++cases;}
    {const uint32_t a[]={0,4,0,0,0,0};encode(0x16,a,6);CHECK(faults==1);++cases;}
    {const uint32_t a[]={0,0,2,2,2,0};encode(0x16,a,6);CHECK(faults==3);++cases;}
    {const uint32_t a[]={8,8,2,2,0};encode(0x25,a,5);CHECK(faults==4);++cases;}
    {const uint32_t a[]={0x20,0,0,0x20,0,0};encode(0x26,a,6);CHECK(faults==2);++cases;}
    {const uint32_t a[]={0xffffffff,0,0,0,0,0};encode(0x16,a,6);CHECK(faults==0&&mem[DATA+2]==0xe0);++cases;}   /* signed compare */
    /* Format 0 and formats without a case fault after the id. */
    for(uint32_t f=0;f<0x100;++f){
        encode(f,NULL,0);
        const int known=px_battle_command_format_args(f)>=0;
        if(!known)CHECK(faults==1&&used()==2);
        ++cases;
    }
    {unsigned n=0;for(uint32_t f=0;f<0x100;++f)n+=px_battle_command_format_args(f)>=0;CHECK(n==32);++cases;}
    /* The writer: ids below 0x6d, info bytes, the argument buffer. */
    reset();w8(NULL,INFO+0x22,0x26);
    {const uint32_t a[]={1,2,3,4,0x1111,0x2222};px_battle_command(QUEUE,0x122,a,6,&api);}   /* the id is a byte */
    CHECK(used()==2+7&&mem[DATA]==0&&mem[DATA+1]==0x22&&r32(NULL,BUFFER+0x14)==0x2222&&faults==0);++cases;
    reset();w8(NULL,INFO+0x10,0x05);{const uint32_t a[]={1,2,3};px_battle_command(QUEUE,0x10,a,3,&api);}
    CHECK(r32(NULL,BUFFER+0xc)==0&&r32(NULL,BUFFER+0x10)==0&&used()==8);++cases;   /* missing arguments read as 0 */
    reset();w8(NULL,INFO+0x40,0x09);{const uint32_t a[]={1,2,3,4,5,6,7,8,9};px_battle_command(QUEUE,0x40,a,9,&api);}
    CHECK(used()==11&&mem[DATA+10]==9&&r32(NULL,BUFFER+0x20)==9);++cases;   /* nine arguments */
    reset();px_battle_command(QUEUE,0x6d,NULL,0,&api);CHECK(faults==1&&used()==0);++cases;
    reset();px_battle_command(QUEUE,0x6c,NULL,0,&api);CHECK(faults==1&&used()==2);++cases;   /* info 0 */
    reset();w8(NULL,INFO+3,0x30);w32(NULL,BUFFER,0x99);px_battle_command(QUEUE,3,NULL,0,&api);CHECK(used()==2&&r32(NULL,BUFFER)==0x99);++cases;
    printf("battle_command: %u cases, all tests passed\n",cases);
    return 0;
}
