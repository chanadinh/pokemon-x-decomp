#include "battle_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x20000, BATTLE=0x1000, POOL=0x1100, NODES=0x2000, ABILITY=0x8000, ITEM=0x8800, MOVE=0x9000, FIELD=0xa000,
       SCRATCH=0xb000, POKEMON=0xc000, WORDS=0xd000, TABLES=0xe000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static unsigned aborts;static uint32_t ability_value,speed_value;static char log_[2048];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;CHECK(p==POKEMON&&k==0x10);note("V ");return ability_value; }
static uint32_t raw_value(void *u,uint32_t p,uint32_t k) { (void)u;CHECK(p==POKEMON&&k==0xc);note("R ");return speed_value; }
/* Table functions: id f returns TABLES + 0x40 f and stores word f * 0x10001 masked by the scenario. */
static uint32_t count_word[64];
static uint32_t entries(void *u,uint32_t f,uint32_t out)
{
    (void)u;CHECK(out==SCRATCH&&f>=0x100&&f<0x140);char b[16];snprintf(b,sizeof b,"E%x ",f);note(b);
    w32(NULL,out,count_word[f-0x100]);return f==0x13f?0:TABLES+0x40*(f-0x100);
}
static const struct px_battle_handler_api api={&memory,NULL,abort_battle,pokemon_value,raw_value,entries,BATTLE,POOL,
                                               ABILITY,ITEM,MOVE,FIELD,SCRATCH};
static uint32_t node(unsigned i) { return NODES+0x40*i; }
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;log_[0]=0;ability_value=speed_value=0;memset(count_word,0,sizeof count_word);
    for(unsigned i=0;i<PX_BATTLE_HANDLER_POOL;++i)w32(NULL,POOL+4*i,node(i));
}
static uint32_t head(void) { return r32(NULL,BATTLE+0x60); }
static uint32_t used(void) { return r32(NULL,BATTLE+0x64); }
/* Walks the list checking both links; returns the count. */
static unsigned walk(uint32_t *out,unsigned max)
{
    unsigned n=0;uint32_t prev=0;
    for(uint32_t h=head();h;prev=h,h=r32(NULL,h+4)) { CHECK(n<max&&r32(NULL,h)==prev);out[n++]=h; }
    return n;
}
static void make_registered(unsigned i,uint8_t owner,uint8_t value,uint16_t id,uint32_t flags)
{
    w8(NULL,node(i)+0x10,owner);w8(NULL,node(i)+0x3a,value);w16(NULL,node(i)+0x38,id);w32(NULL,node(i)+0x18,flags);
}
static void link(const unsigned *order,unsigned n)
{
    w32(NULL,BATTLE+0x60,n?node(order[0]):0);
    for(unsigned k=0;k<n;++k){w32(NULL,node(order[k]),k?node(order[k-1]):0);w32(NULL,node(order[k])+4,k+1<n?node(order[k+1]):0);}
}
#define REG 0x20000000u
int main(void)
{
    unsigned cases=0;
    /* Exact fields for every owner, with stale handler contents. */
    for(uint32_t owner=0;owner<8;++owner)for(uint32_t depth=0;depth<3;++depth) {
        reset();for(unsigned o=0;o<0x3c;++o) { w8(NULL,node(0)+o,0xa5); }
        w32(NULL,BATTLE+0x68,0x10000|depth);
        const uint32_t h=px_battle_handler_register(owner,0x1234+owner,3,0x2345,0x5a,0x7777,0x1c3,&api);
        CHECK(h==node(0)&&used()==1&&head()==h&&aborts==0);
        CHECK(r32(NULL,h)==0&&r32(NULL,h+4)==0&&r32(NULL,h+8)==0x7777&&r32(NULL,h+0xc)==0&&r8(NULL,h+0x10)==owner);
        CHECK(r32(NULL,h+0x14)==(4u<<24|0x2345)&&r32(NULL,h+0x18)==(REG|0xc3u<<16|depth)&&r16(NULL,h+0x38)==0x1234+owner);
        CHECK(r8(NULL,h+0x3a)==0x5a&&r8(NULL,h+0x3b)==(owner==0||owner==4||owner==5?0x5a:0x1f));
        for(unsigned o=0x1c;o<=0x34;o+=4) { CHECK(r32(NULL,h+o)==0); }
        CHECK(r16(NULL,h+0x10+1)==0xa5a5&&r8(NULL,h+0x13)==0xa5);   /* untouched bytes */
        ++cases;
    }
    /* Bounds abort but continue; priorities above 7 wrap the key. */
    reset();CHECK(px_battle_handler_register(0,1,0xff,0xffffff,0,0,0,&api)==node(0)&&aborts==2);
    CHECK(r32(NULL,node(0)+0x14)==((7u-0xffu)<<24|0xffffff));++cases;
    reset();CHECK(px_battle_handler_register(0,1,0xfe,0xfffffe,0,0,0,&api)==node(0)&&aborts==0);++cases;
    /* Pool exhausted, and an empty pool slot (the count still advances). */
    reset();w32(NULL,BATTLE+0x64,0x9d);CHECK(px_battle_handler_register(0,1,0,0,0,0,0,&api)==0&&aborts==1&&used()==0x9d);++cases;
    reset();w32(NULL,POOL+4*3,0);w32(NULL,BATTLE+0x64,3);CHECK(px_battle_handler_register(0,1,0,0,0,0,0,&api)==0&&aborts==1&&used()==4);++cases;
    /* Sorted insertion: random keys against an independent model (equal keys keep insertion order). */
    for(unsigned run=0;run<300;++run) {
        reset();uint32_t model[0x9d];uint32_t keys[0x9d];unsigned n=0;uint32_t seed=run*2654435761u+1;
        for(unsigned i=0;i<0x9d;++i) {
            seed=seed*1103515245u+12345u;const uint32_t prio=(seed>>16)%8,sub=(seed>>8)%(run%3?4:0x100);
            const uint32_t h=px_battle_handler_register(4,i,prio,sub,i,0,0,&api);CHECK(h==node(i));
            const uint32_t key=sub|(7u-prio)<<24;unsigned at=0;while(at<n&&keys[at]>=key)++at;
            memmove(model+at+1,model+at,(n-at)*4);memmove(keys+at+1,keys+at,(n-at)*4);model[at]=h;keys[at]=key;++n;
        }
        uint32_t got[0x9d];CHECK(walk(got,0x9d)==n&&!memcmp(got,model,n*4)&&aborts==0);
        CHECK(px_battle_handler_register(4,0,0,0,0,0,0,&api)==0&&aborts==1);++cases;
    }
    /* Removal: null, unregistered, running, then head, middle and tail; the pool takes it back. */
    reset();px_battle_handler_remove(0,&api);CHECK(aborts==0&&log_[0]==0);++cases;
    for(unsigned who=0;who<3;++who) {
        reset();for(unsigned i=0;i<3;++i)px_battle_handler_register(4,i,3,10-i,i,0x55,1,&api);
        w8(NULL,node(who)+0x3a,0x77);
        px_battle_handler_remove(node(who),&api);
        uint32_t got[3];const unsigned n=walk(got,3);CHECK(n==2&&used()==2&&r32(NULL,POOL+8)==node(who));
        for(unsigned o=0;o<0x3c;o+=4) { CHECK(r32(NULL,node(who)+o)==0); }
        unsigned k=0;for(unsigned i=0;i<3;++i)if(i!=who)CHECK(got[k++]==node(i));++cases;
        /* A new registration reuses it. */
        CHECK(px_battle_handler_register(4,9,3,0,9,0,0,&api)==node(who));++cases;
    }
    reset();px_battle_handler_register(4,1,3,0,1,0,1,&api);w32(NULL,node(0)+0x18,r32(NULL,node(0)+0x18)|0x1000000);
    px_battle_handler_remove(node(0),&api);CHECK(head()==node(0)&&(r32(NULL,node(0)+0x18)&0x8000000)&&used()==1);++cases;
    reset();px_battle_handler_register(4,1,3,0,1,0,1,&api);w32(NULL,node(0)+0x18,r32(NULL,node(0)+0x18)&~REG);
    px_battle_handler_remove(node(0),&api);CHECK(head()==node(0)&&used()==1&&r32(NULL,node(0)+0x18)==1u<<16);++cases;
    reset();px_battle_handler_register(4,1,3,0,1,0x55,1,&api);w32(NULL,BATTLE+0x64,0);
    px_battle_handler_remove(node(0),&api);CHECK(aborts==1&&head()==0&&r32(NULL,node(0)+8)==0x55);++cases;
    /* Remove every owner-6 handler; running ones keep only the request. */
    for(unsigned mask=0;mask<64;++mask) {
        reset();unsigned owner6=0,running6=0;
        for(unsigned i=0;i<6;++i)px_battle_handler_register(mask>>i&1?6:4+(i&1),i,3,20-i,i,0,1,&api);
        w32(NULL,node(2)+0x18,r32(NULL,node(2)+0x18)|0x1000000);
        for(unsigned i=0;i<6;++i) { if(mask>>i&1){++owner6;if(i==2)++running6;} }
        px_battle_handler_remove_owner6(&api);
        uint32_t got[6];const unsigned n=walk(got,6);CHECK(n==6-owner6+running6&&used()==6-owner6+running6);
        for(unsigned k=0;k<n;++k) { CHECK(r8(NULL,got[k]+0x10)!=6||got[k]==node(2)); }
        CHECK(!(mask>>2&1)||(r32(NULL,node(2)+0x18)&0x8000000));++cases;
    }
    reset();px_battle_handler_register(6,1,3,0,1,0,1,&api);w32(NULL,BATTLE+0x64,0);px_battle_handler_remove_owner6(&api);
    CHECK(aborts==1&&head()==0);++cases;
    /* find / find_next: owner and value bytes, registration (find only) and removal requests. */
    {
        reset();const unsigned order[6]={0,1,2,3,4,5};
        make_registered(0,0,3,1,REG);make_registered(1,0,4,2,REG);make_registered(2,0,3,3,REG|0x8000000);
        make_registered(3,0,3,4,0);make_registered(4,1,3,5,REG);make_registered(5,0,3,6,REG);link(order,6);
        CHECK(px_battle_handler_find(0,3,&api)==node(0));
        CHECK(px_battle_handler_find_next(node(0),&api)==node(3));   /* unregistered, still found */
        CHECK(px_battle_handler_find_next(node(3),&api)==node(5)&&px_battle_handler_find_next(node(5),&api)==0);
        CHECK(px_battle_handler_find(0,4,&api)==node(1)&&px_battle_handler_find(1,3,&api)==node(4)&&px_battle_handler_find(2,3,&api)==0);
        CHECK(px_battle_handler_find(0x100,3,&api)==0&&px_battle_handler_find(0,0x103,&api)==0&&px_battle_handler_find_next(0,&api)==0);
        w32(NULL,node(0)+0x18,0);CHECK(px_battle_handler_find(0,3,&api)==node(5));++cases;
    }
    /* Work words: 0..6, and the abort that still writes. */
    for(uint32_t i=0;i<9;++i) { reset();px_battle_handler_set_work(node(1),i,0xabc0+i,&api);CHECK(r32(NULL,node(1)+0x1c+4*i)==0xabc0+i&&aborts==(i>=7));++cases; }
    /* Abilities: byte ids against a 16-bit ability, default and explicit priority, call order. */
    for(unsigned row=0;row<0xb6;++row)w8(NULL,ABILITY+8*row,(uint8_t)(row+1)),w32(NULL,ABILITY+8*row+4,0x100+row%60);
    {
        uint8_t saved[0xb6*8];memcpy(saved,mem+ABILITY,sizeof saved);
        for(uint32_t ability=0;ability<0x200;ability+=7)for(unsigned explicit_prio=0;explicit_prio<2;++explicit_prio) {
            reset();memcpy(mem+ABILITY,saved,sizeof saved);ability_value=0xabcd0000|ability;speed_value=0x99990000|0x123;w8(NULL,POKEMON+0x19,2);
            const unsigned row=ability-1;
            count_word[row%60]=(explicit_prio?2u<<16:0)|3u;
            const uint32_t h=px_battle_register_ability(POKEMON,&api);
            if(ability>=1&&ability<=0xb6) {
                char b[32];snprintf(b,sizeof b,"V R E%x ",0x100+row%60);CHECK(!strcmp(log_,b)&&h==node(0));
                CHECK(r8(NULL,h+0x10)==4&&r16(NULL,h+0x38)==ability&&r8(NULL,h+0x3a)==2&&r8(NULL,h+0x1a)==3);
                CHECK(r32(NULL,h+0x14)==((7u-(explicit_prio?2u:5u))<<24|0x123)&&r32(NULL,h+8)==TABLES+0x40*(row%60));
            } else CHECK(h==0&&!strcmp(log_,"V ")&&used()==0);
            ++cases;
        }
        /* Priority half above a byte is narrowed. */
        reset();memcpy(mem+ABILITY,saved,sizeof saved);ability_value=5;count_word[4]=0x1020004;
        const uint32_t h=px_battle_register_ability(POKEMON,&api);CHECK(r32(NULL,h+0x14)>>24==((7u-0x2u)&0xff)&&aborts==0);++cases;
    }
    /* Items: none, word ids, fixed priority 6. */
    {
        reset();for(unsigned row=0;row<0xb6;++row)w32(NULL,ITEM+8*row,0x10000*(row&1)+row*3+1),w32(NULL,ITEM+8*row+4,0x100+row%60);
        w32(NULL,ITEM+8*0xb5,0);   /* terminator row, as in the original table */
        uint8_t saved[0xb6*8];memcpy(saved,mem+ITEM,sizeof saved);
        for(uint32_t item=0;item<0x240;++item) {
            reset();memcpy(mem+ITEM,saved,sizeof saved);w16(NULL,POKEMON+0x12,(uint16_t)item);w8(NULL,POKEMON+0x19,5);speed_value=0x77;
            for(unsigned k=0;k<60;++k) { count_word[k]=0x50000|k; }
            const uint32_t h=px_battle_register_item(POKEMON,&api);
            int row=-1;for(unsigned r=0;r<0xb6;++r)if(r32(NULL,ITEM+8*r)==item){row=(int)r;break;}
            if(item!=0&&row>=0) {
                CHECK(h==node(0)&&r8(NULL,h+0x10)==5&&r16(NULL,h+0x38)==item&&r32(NULL,h+0x14)==(1u<<24|0x77)&&r8(NULL,h+0x1a)==row%60);
                CHECK(r8(NULL,h+0x3b)==5&&!strncmp(log_,"R E",3));
            } else CHECK(h==0&&log_[0]==0);
            ++cases;
        }
    }
    /* Moves: duplicates, nine other move handlers, a missing table and the fixed priority. */
    {
        for(unsigned others=0;others<11;++others)for(unsigned dup=0;dup<2;++dup) {
            reset();for(unsigned row=0;row<0x128;++row)w16(NULL,MOVE+8*row,(uint16_t)(row+1)),w32(NULL,MOVE+8*row+4,0x100+row%63);
            w8(NULL,POKEMON+0x19,1);
            for(unsigned k=0;k<others;++k)px_battle_handler_register(0,0x200+k,0,0,1,0,0,&api);
            px_battle_handler_register(0,0x300,0,0,2,0,0,&api);   /* another battler */
            if(dup)px_battle_handler_register(0,50,0,0,1,0,0,&api);
            count_word[49%63]=0x2;log_[0]=0;
            const uint32_t before=used(),got=px_battle_register_move(POKEMON,50,0x31,&api);
            const int want=!dup&&others<=8;
            if(dup&&others>8)CHECK(got==0);
            CHECK(got==(uint32_t)want&&used()==before+(unsigned)want);
            if(want){uint32_t h=r32(NULL,POOL+4*before);CHECK(r32(NULL,h+0x14)==(7u<<24|0x31)&&r16(NULL,h+0x38)==50&&r8(NULL,h+0x1a)==2);}
            ++cases;
        }
        reset();w16(NULL,MOVE,7);w32(NULL,MOVE+4,0x13f);CHECK(px_battle_register_move(POKEMON,7,0,&api)==0&&used()==0&&strstr(log_,"E13f"));++cases;
        reset();w16(NULL,MOVE,7);w32(NULL,MOVE+4,0x101);CHECK(px_battle_register_move(POKEMON,8,0,&api)==0&&log_[0]==0);++cases;
    }
    /* Field: kind bound, duplicates for the same value, work words. */
    {
        for(uint32_t kind=0;kind<7;++kind)for(unsigned dup=0;dup<2;++dup)for(uint32_t words=0;words<9;words+=4) {
            reset();for(unsigned row=0;row<5;++row)w8(NULL,FIELD+8*row,(uint8_t)row),w32(NULL,FIELD+8*row+4,0x110+row);
            for(unsigned i=0;i<8;++i) { w32(NULL,WORDS+4*i,0x1000+i); }
            count_word[0x10+kind%5]=0x30001;
            if(dup)px_battle_handler_register(1,kind,1,0,9,0,0,&api);
            px_battle_handler_register(1,kind,1,0,8,0,0,&api);    /* other value */
            const uint32_t before=used(),h=px_battle_register_field(kind,9,0xfeed,WORDS,words,&api);
            if(kind<5&&!dup) {
                CHECK(h==r32(NULL,POOL+4*before));CHECK(r8(NULL,h+0x10)==1&&(words==8||r16(NULL,h+0x38)==kind));
                CHECK(r32(NULL,h+0x14)==6u<<24);CHECK(words==8||r8(NULL,h+0x3b)==0x1f);
                for(unsigned i=0;i<words&&i<7;++i)CHECK(r32(NULL,h+0x1c+4*i)==(i==6?0xfeedu:0x1000+i));
                CHECK(r32(NULL,h+0x1c+24)==0xfeed&&aborts==(words>7?words-7:0));
                if(words==8)CHECK(r32(NULL,h+0x38)==0x1007);   /* index 7 overwrites the id and value bytes */
            } else { CHECK(h==0&&used()==before);CHECK(aborts==(kind>=5)); }
            ++cases;
        }
    }
    printf("battle_handler: %u cases, all tests passed\n",cases);
    return 0;
}
