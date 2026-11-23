#include "battle_core_5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x20000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, HANDLER=0x2800, ENTRIES=0x2900, CTX=0x8000, TAB=0x8400,
       OBJ=0xa000, MON=0xc000, BUF=0xe000, NODES=0x3000, RANDOM_OBJ=0x4000, ITEM_OBJ=0x4100 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static unsigned aborts,checker_calls;static uint32_t random_value,seen_limit,seen_object,multiplier_key;
static uint32_t checker_result[8],checker_node[8],checker_owner[8],checker_id[8],checker_value[8],checker_event;
static void abort_battle(void *u) { (void)u;++aborts; }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api ev={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;(void)v;(void)w;CHECK(f==0x77&&h==HANDLER&&c==CTX&&px_battle_event_get(2,&ev)==9);
    if(multiplier_key){px_battle_event_multiply(0x36,multiplier_key,&ev);}
}
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;(void)p;(void)k;return 0; }
static const struct px_battle_ability_api base={.events=&ev,.pokemon_value=pokemon_value};
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id) { (void)u;(void)c;(void)k;(void)id;return 0; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id) { (void)u;(void)r;(void)k;(void)id; }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;(void)r;(void)v; }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;(void)c;(void)r; }
static void popup(void *u,uint32_t c,uint32_t p,uint32_t i) { (void)u;(void)c;(void)p;(void)i; }
static const struct px_battle_items_api items={&base,NULL,NULL,popup,action_record,message,message_arg,commit,0,0};
static const struct px_battle_core_api core={&items};
static uint32_t entries_fn(void *u,uint32_t f,uint32_t out) { (void)u;(void)f;(void)out;return 0; }
static const struct px_battle_handler_api handler={&memory,NULL,abort_battle,pokemon_value,NULL,entries_fn,BATTLE,POOL,0,0,0,0,0xf000};
static const struct px_battle_core_4_api core4={&core,&handler,0};
static uint32_t random_fn(void *u,uint32_t object,uint32_t limit) { (void)u;seen_object=object;seen_limit=limit;return random_value; }
static uint32_t item_fn(void *u,uint32_t object,uint32_t item,uint32_t param) { (void)u;return object*0x10000u+item*0x100u+param; }
static uint32_t checker_fn(void *u,uint32_t node,uint32_t context,uint32_t owner,uint32_t event,uint32_t id,uint32_t value)
{
    (void)u;CHECK(context==CTX&&checker_calls<8);checker_event=event;checker_node[checker_calls]=node;checker_owner[checker_calls]=owner;
    checker_id[checker_calls]=id;checker_value[checker_calls]=value;return checker_result[checker_calls++];
}
static const struct px_battle_core_5_api api={&core4,BATTLE,random_fn,item_fn,checker_fn};
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;checker_calls=0;random_value=0;multiplier_key=0;memset(checker_result,0,sizeof checker_result);
    w32(NULL,CTX+8,TAB);w32(NULL,CTX+0x1c,OBJ);w32(NULL,BATTLE+0x54,RANDOM_OBJ);w32(NULL,BATTLE+0x5c,ITEM_OBJ);
    for(unsigned i=0;i<0x18;++i){w32(NULL,TAB+0x84+4*i,MON+0x200*i);}
}
static uint32_t rnd(uint32_t *s) { *s=*s*1103515245u+12345u;return *s>>8; }
int main(void)
{
    unsigned cases=0;
    /* The random and item objects. */
    for(uint32_t limit=0;limit<5;++limit){reset();random_value=limit*7;CHECK(px_battle_random(limit,&api)==limit*7&&seen_limit==limit&&seen_object==RANDOM_OBJ);++cases;}
    for(uint32_t item=0;item<4;++item){reset();CHECK(px_battle_item_value(item,0x1ff,&api)==ITEM_OBJ*0x10000u+item*0x100u+0xff);++cases;}
    for(uint32_t percent=0;percent<4;++percent)for(uint32_t value=0;value<4;++value){reset();random_value=value;
        CHECK(px_battle_chance(percent,&api)==(value<percent)&&seen_limit==100);++cases;}
    /* Bounds-checked accessors. */
    for(uint32_t back=0;back<5;++back)for(uint32_t cursor=0;cursor<3;++cursor){reset();w8(NULL,CTX+0x205,(uint8_t)cursor);
        for(uint32_t i=0;i<3;++i){w8(NULL,CTX+0x202+i,(uint8_t)(10+i));}
        int32_t p=(int32_t)cursor-(int32_t)back;if(p<0){p+=3;}
        CHECK(px_battle_ring_length(CTX,back,&api)==(back<3?10u+(uint32_t)p:0u)&&aborts==(unsigned)(back>=3));++cases;}
    for(uint32_t group=0;group<6;++group){reset();for(uint32_t g=0;g<5;++g){w8(NULL,CTX+49*g,(uint8_t)(g+3));}
        CHECK(px_battle_group_count(CTX,group,&api)==(group<4?group+3:0)&&aborts==(unsigned)(group>=4));++cases;}
    for(uint32_t index=0;index<8;++index){reset();w8(NULL,CTX+0x152,4);for(uint32_t i=0;i<8;++i){w8(NULL,CTX+i*14+0x121,(uint8_t)(i&1?0x10:0x38));}
        CHECK(px_battle_entry_flag_121(CTX,index,&api)==(uint32_t)((index&1)==0)&&aborts==(unsigned)(index>=4));
        w8(NULL,CTX+index*14+0x121,0xf0);CHECK(px_battle_entry_flag_121(CTX,index,&api)==0);++cases;}
    for(uint32_t index=0;index<6;++index)for(uint32_t set=0;set<2;++set){reset();w32(NULL,CTX+4,BUF);
        if(index<4){w32(NULL,BUF+index*56+0x83c,set?0x55:0);}
        CHECK(px_battle_entry_83c(CTX,index,&api)==(uint32_t)(index<4&&set)&&aborts==(unsigned)(index>=4));++cases;}
    for(uint32_t index=0;index<8;++index){reset();w8(NULL,BUF+0x19,5);for(uint32_t i=0;i<8;++i){w8(NULL,BUF+i,(uint8_t)(0x40+i));}
        CHECK(px_battle_byte_or_6(BUF,index,&api)==(index<5?0x40u+index:6u));++cases;}
    for(uint32_t index=0;index<0x28;++index){reset();for(uint32_t i=0;i<0x28;++i){w32(NULL,MON+0x1c+4*i,0x1000+i);}
        CHECK(px_battle_condition_word(MON,index,&api)==0x1000+index&&aborts==(unsigned)(index>=0x26));++cases;}
    for(uint32_t index=0;index<12;++index){reset();for(uint32_t i=0;i<12;++i){w8(NULL,CTX+0x7ec+i,(uint8_t)(0x60+i));}
        CHECK(px_battle_byte_7ec(CTX,index,&api)==0x60+index&&aborts==(unsigned)(index>=10));++cases;}
    for(uint32_t v=0;v<0x400;++v){reset();w32(NULL,MON+0x30,v);CHECK(px_battle_status_is_0x78(MON,&api)==(uint32_t)((v&7)==1&&(v&0x1f8)==0x78));++cases;}
    for(uint32_t index=0;index<4;++index)for(uint32_t set=0;set<2;++set){reset();w32(NULL,BUF+index*56+0x838,set?9:0);w16(NULL,BUF+index*56+0x84c,0x1234);
        CHECK(px_battle_slot_half(BUF,index,&api)==(set?0u:0x1234u));++cases;}
    for(uint32_t cursor=0;cursor<8;++cursor)for(uint32_t member=0;member<5;++member){reset();w8(NULL,CTX+0x815,(uint8_t)cursor);for(uint32_t i=0;i<10;++i){w8(NULL,CTX+0x7ec+i,9);}
        const uint32_t next=(cursor+2)&3;if(member<4){w8(NULL,CTX+0x7ec+2*member,(uint8_t)next);}
        CHECK(px_battle_next_pair(CTX,&api)==(member<4?next:4u));++cases;}
    /* Struct initializers. */
    reset();for(uint32_t i=0;i<0x40;++i){w8(NULL,BUF+i,0xa5);}px_battle_clear_18(BUF,&api);
    for(uint32_t i=0;i<0x40;++i){CHECK(r8(NULL,BUF+i)==((i<0x1a)?0:0xa5));}
    for(uint32_t i=0;i<0x40;++i){w8(NULL,BUF+i,0xa5);}px_battle_clear_10(BUF,&api);
    for(uint32_t i=0;i<0x40;++i){CHECK(r8(NULL,BUF+i)==((i<10)?0:0xa5));}
    for(uint32_t v=0;v<0x300;v+=17){for(uint32_t i=0;i<0x40;++i){w8(NULL,BUF+i,0xa5);}px_battle_fill_10(BUF,v,&api);
        for(uint32_t i=0;i<0x40;++i){CHECK(r8(NULL,BUF+i)==((i<10)?(v&0xff):0xa5));}++cases;}
    for(uint32_t i=0;i<0x20;++i){w8(NULL,BUF+i,0xa5);}for(uint32_t i=0;i<0x20;++i){w8(NULL,BUF+0x20+i,(uint8_t)(0x30+i));}px_battle_copy_7(BUF,BUF+0x20,&api);
    for(uint32_t i=0;i<10;++i){CHECK(r8(NULL,BUF+i)==(i<7?0x30+i:0xa5));}++cases;
    /* The matching-entry search. */
    for(uint32_t seed=1;seed<400;++seed){reset();uint32_t s=seed;const uint32_t slot=rnd(&s)%0x1c,flag=rnd(&s)%2,second=rnd(&s)%2,count=rnd(&s)%4;
        const uint32_t group=px_battle_slot_group(slot,&handler);aborts=0;
        const uint32_t entry=BUF,list=0xd000+28*(group<4?group:0)+4;
        w8(NULL,entry+0x19,(uint8_t)slot);w8(NULL,entry+0x1b,(uint8_t)(flag?0x40:0));w32(NULL,entry,0x111);w32(NULL,entry+4,second?0x222:0);
        w8(NULL,list+0x18,(uint8_t)count);for(uint32_t i=0;i<4;++i){w32(NULL,list+4*i,0xd800+0x10*i);w32(NULL,0xd800+0x10*i,0x300+i);}
        const uint32_t key=second?0x222:0x111;uint32_t want=entry;
        if(flag){const uint32_t pick=rnd(&s)%5;if(pick<count){w32(NULL,0xd800+0x10*pick,key);want=0xd800+0x10*pick;}}
        CHECK(px_battle_find_matching_entry(0x99,0xd000,entry,&api)==want&&aborts==(unsigned)(flag&&group>=4?(slot>0x17?2:1):(flag&&slot>0x17?1:0)));++cases;}
    /* Unique adds. */
    for(uint32_t count=0;count<12;++count)for(uint32_t at=0;at<12;++at){reset();w8(NULL,BUF+0x6e,(uint8_t)count);w8(NULL,BUF+0x6f,5);
        for(uint32_t i=0;i<10;++i){w32(NULL,BUF+4*i,0x100+i);}w8(NULL,BUF+0x6e,(uint8_t)count);
        for(uint32_t i=0;i<10;++i){w8(NULL,BUF+0x50+i,0xee);w16(NULL,BUF+0x28+2*i,0xeeee);w16(NULL,BUF+0x3c+2*i,0xeeee);}
        const uint32_t value=at<10?0x100+at:0x999;px_battle_add_unique(BUF,value,&api);
        const int dup=count<10&&at<count;
        if(count>=10){CHECK(aborts==1&&r8(NULL,BUF+0x6e)==count);}
        else if(dup){CHECK(aborts==0&&r8(NULL,BUF+0x6e)==count);}
        else{CHECK(aborts==0&&r32(NULL,BUF+4*count)==value&&r8(NULL,BUF+0x50+count)==0&&r16(NULL,BUF+0x28+2*count)==0&&r16(NULL,BUF+0x3c+2*count)==0
            &&r8(NULL,BUF+0x6e)==count+1&&r8(NULL,BUF+0x6f)==(count+1>5?count+1:5));}
        ++cases;}
    for(uint32_t index=0;index<13;++index)for(uint32_t count=0;count<12;++count)for(uint32_t flag=0;flag<2;++flag)for(uint32_t at=0;at<3;++at){
        reset();w32(NULL,CTX+index*4+0x274,flag?7:0);w32(NULL,CTX+index*4+0x248,count);
        for(uint32_t i=0;i<10;++i){w32(NULL,CTX+index*40+0x90+4*i,0x50+i);}
        const uint32_t value=at==0?0x50u:at==1?0x51u:0x777u;
        const uint32_t got=px_battle_add_unique_slot(CTX,index,value,&api);
        uint32_t want=0;if(index<11&&flag&&count<10){want=1;for(uint32_t i=0;i<count;++i){if(0x50+i==value){want=0;}}}
        CHECK(got==want&&aborts==(unsigned)(index>=11?2:0));
        if(want){CHECK(r32(NULL,CTX+index*4+0x248)==count+1&&r32(NULL,CTX+index*40+0x90+4*count)==value);}
        else if(index<11){CHECK(r32(NULL,CTX+index*4+0x248)==count);}
        ++cases;}
    /* The scaled HP through the event. */
    for(uint32_t seed=0;seed<400;++seed){reset();uint32_t s=seed+3;const uint32_t id=rnd(&s)%0x1a,hp=(seed%3==0)?rnd(&s)%8:rnd(&s)%500,mult=(rnd(&s)%4)?(0x800+rnd(&s)%0x1800):0;
        multiplier_key=mult;w8(NULL,MON+0x19,9);w8(NULL,0x19,9);w16(NULL,0x150,(uint16_t)hp);for(unsigned i=0;i<0x18;++i){w16(NULL,MON+0x200*i+0x150,(uint16_t)hp);w8(NULL,MON+0x200*i+0x19,9);}
        w32(NULL,BATTLE+0x60,HANDLER);w32(NULL,HANDLER+8,ENTRIES);w8(NULL,ENTRIES,0x84);w32(NULL,ENTRIES+4,0x77);
        w32(NULL,HANDLER+0x18,0x20000000u|1u<<16);w8(NULL,HANDLER+0x10,5);w8(NULL,HANDLER+0x3b,0);
        const uint32_t used=mult?mult:0x1000;uint32_t prod=hp*used;uint32_t want=prod>>12;if((prod&0xfff)>0x800){++want;}if(!want){want=1;}
        CHECK(px_battle_scaled_hp(CTX,id,&api)==want&&aborts==(unsigned)(id>=0x18?2:0)&&r32(NULL,STACK)==0);++cases;}
    /* The blocked test. */
    for(uint32_t seed=0;seed<3000;++seed){reset();uint32_t s=seed+5;const uint32_t pick=rnd(&s)%6,slot=pick==5?0x1e:pick==4?0x1f:(rnd(&s)%4),owner=4+rnd(&s)%3;
        const uint32_t cond5c=rnd(&s)%2,cond68=rnd(&s)%2,bit167=rnd(&s)%2,flag290=rnd(&s)%2;
        w8(NULL,HANDLER+0x3b,(uint8_t)slot);w8(NULL,HANDLER+0x10,(uint8_t)owner);w16(NULL,HANDLER+0x38,0x1357);w8(NULL,HANDLER+0x3a,0x66);
        w32(NULL,OBJ+0x290,flag290?3:0);
        for(unsigned i=0;i<4;++i){w32(NULL,MON+0x200*i+0x5c,(cond5c&&(i&1))?0x104u:0x100u);w32(NULL,MON+0x200*i+0x68,cond68?0x104u:0x100u);w8(NULL,MON+0x200*i+0x167,(uint8_t)(bit167?2:0xfd));}
        const uint32_t count=rnd(&s)%4;uint32_t head=0,prev=0;
        for(uint32_t i=0;i<count;++i){const uint32_t n=NODES+0x40*i;w8(NULL,n+0x10,(uint8_t)((rnd(&s)%2)?4:5));w8(NULL,n+0x3b,(uint8_t)(rnd(&s)%4));
            w32(NULL,n+0xc,(rnd(&s)%4)?0x100u:0u);w32(NULL,n+0x18,(rnd(&s)%3)?0u:0x40000000u);if(prev){w32(NULL,prev+4,n);}else{head=n;}prev=n;
            checker_result[i]=rnd(&s)%2;}
        w32(NULL,BATTLE+0x60,head);
        /* The model. */
        const uint32_t battler=slot==0x1f?0:MON+0x200*slot;uint32_t want=0;int decided=0;
        if(owner==4){if(battler&&(r32(NULL,battler+0x5c)&7)){want=1;decided=1;}}
        else if(owner==5){if(flag290){want=1;decided=1;}else if(battler&&((r32(NULL,battler+0x68)&7)||(r8(NULL,battler+0x167)&2))){want=1;decided=1;}}
        unsigned expected_calls=0;uint32_t idx=0;
        for(uint32_t n=head;!decided&&n;n=r32(NULL,n+4),++idx){
            if(!r32(NULL,n+0xc))continue;
            if(r8(NULL,n+0x10)==4){const uint32_t b=MON+0x200*r8(NULL,n+0x3b);if(r32(NULL,b+0x5c)&7)continue;}
            if(r32(NULL,n+0x18)&0x40000000u)continue;
            if(checker_result[expected_calls++]){want=1;break;}
        }
        const unsigned abort_expected=0;(void)abort_expected;
        CHECK(px_battle_event_blocked(CTX,HANDLER,0x31,&api)==want);
        CHECK(aborts==(unsigned)(slot==0x1e?2:0));
        if(!(checker_calls==expected_calls||decided)){fprintf(stderr,"seed %u calls %u expected %u slot %u owner %u\n",seed,checker_calls,expected_calls,slot,owner);}
        CHECK(checker_calls==expected_calls||decided);
        for(unsigned i=0;i<checker_calls;++i){CHECK(checker_owner[i]==owner&&checker_id[i]==0x1357&&checker_value[i]==0x66&&checker_event==0x31);}
        ++cases;}
    printf("battle_core_5: %u cases, all tests passed\n",cases);
    return 0;
}
