#include "battle_core_2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x20000, BATTLE=0x1000, POOL=0x1100, NODES=0x2000, LAYERS=0x5000, CTX=0x8000, BUF=0xb000, LOG=0xc000, ARR=0xd000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static unsigned aborts;
static void abort_battle(void *u) { (void)u;++aborts; }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t entries(void *u,uint32_t f,uint32_t out) { (void)u;(void)f;(void)out;return 0; }
static uint32_t position_entries(void *u,uint32_t f,uint32_t out,uint32_t p) { (void)u;(void)f;(void)out;(void)p;return 0; }
static const struct px_battle_handler_api handler_api={&memory,NULL,abort_battle,NULL,NULL,entries,BATTLE,POOL,0,0,0,0,0xe000};
static const struct px_battle_tables_api tables={&handler_api,LAYERS,0,0,overflow,position_entries};
static uint32_t node(unsigned i) { return NODES+0x40*i; }
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;
    for(unsigned i=0;i<PX_BATTLE_HANDLER_POOL;++i)w32(NULL,POOL+4*i,node(i));
}
static uint32_t rnd(uint32_t *s) { *s=*s*1103515245u+12345u;return *s>>8; }
int main(void)
{
    unsigned cases=0;
    /* Pure helpers. */
    for(uint32_t v=0;v<0x400;++v){CHECK(px_battle_is_kind1_0x78(v)==((v&7)==1&&(v&0x1f8)==0x78));++cases;}
    CHECK(px_battle_is_kind1_0x78(0xfffffe79u)==1&&px_battle_is_kind1_0x78(0x79)==1&&px_battle_is_kind1_0x78(0x7a)==0);
    for(uint32_t a=0;a<0x80;a+=3)for(uint32_t b=0;b<0x80;b+=5){CHECK(px_battle_pack_4(a,b)==(((a<<9)&0x7e00u)|((b<<3)&0x1f8u)|4u));++cases;}
    for(uint32_t flag=0;flag<4;++flag)for(uint32_t v=0;v<0x200;++v){CHECK(px_battle_double_plus(flag,v)==(((flag&1)?1u+2u*v:2u*v)&0xffu));++cases;}
    /* Four halfwords. */
    for(uint32_t pick=0;pick<5;++pick){reset();static const uint32_t off[4]={0x11c,0x12a,0x138,0x146};
        for(unsigned i=0;i<4;++i)w16(NULL,CTX+off[i],(uint16_t)(0x100+i));
        if(pick<4)w16(NULL,CTX+off[pick],0x55);
        CHECK(px_battle_four_halves_match(CTX,0x55,&memory)==(pick<4)&&px_battle_four_halves_match(CTX,0x56,&memory)==0);++cases;}
    /* Group slots. */
    for(uint32_t group=0;group<6;++group)for(uint32_t index=0;index<8;++index){
        reset();for(uint32_t g=0;g<5;++g){w8(NULL,CTX+49*g,(uint8_t)(g+2));for(uint32_t i=0;i<8;++i)w8(NULL,CTX+49*g+0x19+i,(uint8_t)(0x40+g*8+i));}
        const uint32_t want=(group<4&&index<group+2u)?0x40u+group*8+index:0x1fu;
        CHECK(px_battle_group_slot(CTX,group,index,&memory)==want);++cases;}
    /* The default byte: pointer, length and inline byte. */
    for(uint32_t seed=0;seed<200;++seed){reset();uint32_t s=seed+1;const uint32_t ptr=(rnd(&s)%2)?BUF:0,len=rnd(&s)%6,index=rnd(&s)%6;
        w32(NULL,CTX+0x1244,ptr);w32(NULL,CTX+0x1248,len);w8(NULL,CTX+0x124c,0x77);for(uint32_t i=0;i<6;++i)w8(NULL,BUF+i,(uint8_t)(0x10+i));
        CHECK(px_battle_default_byte(CTX,index,&memory)==((ptr&&len>index)?0x10u+index:0x77u));++cases;}
    /* Entries at +0x120c and the id table. */
    for(uint32_t value=0;value<6;++value)for(uint32_t slot=0;slot<11;++slot){
        reset();if(slot<10){w8(NULL,CTX+0x120c+3*slot,1);w8(NULL,CTX+0x120c+3*slot+2,(uint8_t)value);}
        CHECK(px_battle_has_entry_120c(CTX,value,&memory)==(slot<10));++cases;}
    for(uint32_t mask=0;mask<5;++mask){reset();static const uint8_t ids[4]={6,0xd,0x42,0x2b};for(unsigned i=0;i<4;++i)w8(NULL,BUF+i,ids[i]);
        if(mask)w8(NULL,CTX+0x169+(ids[mask-1]>>3),(uint8_t)(1u<<(ids[mask-1]&7)));
        CHECK(px_battle_has_set_id(CTX,BUF,&memory)==(mask!=0));++cases;}
    /* A halfword in an array: every length and position. */
    for(uint32_t count=0;count<9;++count)for(uint32_t pos=0;pos<10;++pos){
        reset();for(uint32_t i=0;i<10;++i)w16(NULL,ARR+2*i,(uint16_t)(0x100+i));w16(NULL,ARR+2*pos,0x33);
        CHECK(px_battle_half_in_array(0x33,ARR,count,&memory)==(pos<count));++cases;}
    /* A word in an array: index of the first match, or -1. */
    for(uint32_t count=0;count<8;++count)for(uint32_t pos=0;pos<9;++pos){
        reset();for(uint32_t i=0;i<6;++i)w32(NULL,ARR+4*i,0x100+i);w8(NULL,ARR+0x18,(uint8_t)count);w32(NULL,ARR+4*(pos%6),0x33);
        CHECK(px_battle_find_word_index(ARR,0x33,&memory)==((pos%6)<count?(pos%6):0xffffffffu));++cases;}
    /* The history ring. */
    {static const uint32_t heads[]={0,1,2,5,398,399};
     for(unsigned h=0;h<sizeof heads/sizeof *heads;++h)for(unsigned gap=0;gap<3;++gap){
        reset();w32(NULL,BUF,heads[h]);
        for(uint32_t i=0;i<400;++i){w16(NULL,BUF+i*8+8,1);w32(NULL,BUF+i*8+4,2);}
        if(gap==1)for(uint32_t k=1;k<4;++k)w16(NULL,BUF+((heads[h]+400-k)%400)*8+8,0);
        if(gap==2){w32(NULL,BUF+((heads[h]+400-2)%400)*8+4,9);}
        const uint32_t want=gap==0||gap==2?1:0;  /* the first entry back matches in gap 0; gap 1 stops at once; gap 2 matches the entry before the odd one */
        CHECK(px_battle_log_contains(BUF,1,2,&memory)==want);
        CHECK(px_battle_log_contains(BUF,1,9,&memory)==(gap==2)&&px_battle_log_contains(BUF,2,2,&memory)==0&&px_battle_log_contains(BUF,1,3,&memory)==0);++cases;}}
    for(uint32_t seed=1;seed<150;++seed){reset();uint32_t s=seed;const uint32_t head=rnd(&s)%400;w32(NULL,BUF,head);
        for(uint32_t i=0;i<400;++i){w16(NULL,BUF+i*8+8,(uint16_t)(rnd(&s)%3));w32(NULL,BUF+i*8+4,rnd(&s)%3);}
        uint32_t want=0,idx=head;for(;;){idx=idx?idx-1:399;if(idx==head)break;const uint32_t k=r16(NULL,BUF+idx*8+8);if(!k)break;if(k==1&&r32(NULL,BUF+idx*8+4)==2){want=1;break;}}
        CHECK(px_battle_log_contains(BUF,1,2,&memory)==want);++cases;}
    /* Side records. */
    for(uint32_t side=0;side<2;++side)for(uint32_t id=0;id<17;++id)for(uint32_t set=0;set<2;++set){
        reset();const uint32_t rec=LAYERS+(side*17+id)*16;w32(NULL,rec,set?node(3):0);
        CHECK(px_battle_side_record_set(0x1234,side,id,&tables)==(uint32_t)set);
        w32(NULL,node(3)+0x18,0x2000007u);CHECK(px_battle_side_flag_clear(side,id,&tables)==(uint32_t)set);
        CHECK(r32(NULL,node(3)+0x18)==(set?0x7u:0x2000007u));
        CHECK(px_battle_side_flag_clear(side,id,&tables)==0);
        w32(NULL,node(3)+0x18,0x5u);CHECK(px_battle_side_flag_set(side,id,&tables)==(uint32_t)set);
        CHECK(r32(NULL,node(3)+0x18)==(set?0x2000005u:0x5u));CHECK(px_battle_side_flag_set(side,id,&tables)==0);++cases;}
    /* Field rewrites. */
    for(uint32_t seed=1;seed<600;++seed){reset();uint32_t s=seed;const uint32_t w=rnd(&s)*2654435761u^rnd(&s),v=rnd(&s);
        w32(NULL,BUF,w);px_battle_set_field_3(BUF,v,&memory);
        const uint32_t k=w&7;uint32_t e=w;if((k==2||k==4)&&((w<<23)>>26)<8)e=(w&~0x1f8u)|((v<<3)&0x1f8u);
        CHECK(r32(NULL,BUF)==e&&r32(NULL,BUF+4)==0);
        w32(NULL,BUF+8,w);px_battle_set_field_9(BUF+8,v,&memory);
        e=w;if(k>=1&&k<=3)e=(w&~0x1fffe00u)|v<<9;else if(k==4)e=(w&0x80007fffu)|v<<15;
        CHECK(r32(NULL,BUF+8)==e);++cases;}
    /* Clears. */
    for(uint32_t index=0;index<4;++index){reset();for(uint32_t i=0;i<0x300;i+=4)w32(NULL,ARR+i,0xa5a5a5a5u);
        px_battle_clear_slot_tables(ARR,index,&memory);
        for(uint32_t i=0;i<0x300;i+=4){const uint32_t a=i;uint32_t want=0xa5a5a5a5u;
            if(a==index*4+0xc||a==index*4+0x38||a==index*4+0x64||a==index*4+0x248||a==index*4+0x274)want=0;
            if(a>=40*index+0x90&&a<40*index+0x90+40){want=0x1f;}
            CHECK(r32(NULL,ARR+a)==want);}
        ++cases;}
    for(uint32_t first=0;first<9;++first){reset();for(uint32_t i=0;i<0x100;++i)w8(NULL,ARR+i,0xa5);
        px_battle_clear_entries(ARR,first,&memory);
        uint8_t want[0x100];memset(want,0xa5,sizeof want);
        if(first>=6){for(unsigned k=0;k<4;++k)want[first*4+0x1c+k]=0;want[first+0xb4]=0;}
        else{for(uint32_t i=1;i<6;++i){for(unsigned k=0;k<4;++k)want[i*4+0x1c+k]=0;want[i+0xb4]=0;if(i==2){for(unsigned k=0;k<4;++k)want[0x40+k]=0;want[0xbd]=0;}}}
        for(uint32_t i=0;i<0x100;++i){CHECK(r8(NULL,ARR+i)==want[i]);}
        ++cases;}
    reset();for(uint32_t i=0;i<0x100;++i)w8(NULL,ARR+i,0xa5);px_battle_clear_tables_10(ARR,&memory);
    for(uint32_t i=0;i<0x100;++i){uint8_t want=0xa5;
        if(i<0x28||(i>=0x28&&i<0x28+20)||(i>=0x3c&&i<0x3c+20)||(i>=0x50&&i<0x5a)||(i>=0x5a&&i<0x5a+20)||(i>=0x6e&&i<=0x71))want=0;
        CHECK(r8(NULL,ARR+i)==want);}++cases;
    /* The log push: below ten entries, and full. */
    for(uint32_t count=0;count<12;++count)for(uint32_t slot=0;slot<11;++slot){
        reset();w32(NULL,CTX+0xdc0,LOG);w8(NULL,LOG,(uint8_t)count);for(uint32_t i=0;i<0x30;++i)w8(NULL,LOG+1+i,0xee);w8(NULL,LOG,(uint8_t)count);
        if(slot<10){w8(NULL,CTX+0x120c+3*slot,1);w8(NULL,CTX+0x120c+3*slot+2,0x61);}
        px_battle_push_log(CTX,0x31,0x61,&memory);
        if(count>=10){CHECK(r8(NULL,LOG)==count&&r8(NULL,LOG+1+count)==0xee&&r8(NULL,LOG+0x15+count)==0xee);}
        else{CHECK(r8(NULL,LOG)==count+1&&r8(NULL,LOG+1+count)==0x31&&r8(NULL,LOG+0x15+count)==0x61&&r8(NULL,LOG+0xb+count)==(slot<10?slot:10));}
        ++cases;}
    /* Removing a battler's move handlers. */
    for(uint32_t mask=0;mask<64;++mask){
        reset();uint32_t handlers[6];
        for(unsigned i=0;i<6;++i){const uint32_t owner=(mask>>i&1)?0u:4u,value=(i&1)?7u:3u;
            handlers[i]=px_battle_handler_register(owner,0x100+i,3,i,value,0,0,&handler_api);}
        w8(NULL,CTX+0x19,3);
        px_battle_remove_move_handlers(CTX,&handler_api);
        unsigned left=0;for(unsigned i=0;i<6;++i)if(px_battle_handler_find((mask>>i&1)?0u:4u,(i&1)?7u:3u,&handler_api)){++left;}
        (void)handlers;
        unsigned expect=0;for(unsigned i=0;i<6;++i){const int removed=((mask>>i&1)&&!(i&1));if(!removed)++expect;}
        unsigned live=0;for(uint32_t h=r32(NULL,BATTLE+0x60);h;h=r32(NULL,h+4))++live;
        CHECK(live==expect&&px_battle_handler_find(0,3,&handler_api)==0);(void)left;++cases;}
    printf("battle_core_2: %u cases, all tests passed\n",cases);
    return 0;
}
