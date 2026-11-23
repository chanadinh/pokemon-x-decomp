#include "battle_core_4.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x20000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, HANDLER=0x2800, ENTRIES=0x2900, CTX=0x8000, TAB=0x8400,
       OBJ=0xa000, MON=0xc000, RECORD=0xb000, BUF=0xe000, IDS=0xe800, STATE=0xe900 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static unsigned aborts,records;static char log_[600];
static int h53,h42;static uint32_t v53,v42,ability_of;
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void notef(const char *f,uint32_t a,uint32_t b) { char t[64];snprintf(t,sizeof t,f,a,b);note(t); }
static void abort_battle(void *u) { (void)u;++aborts; }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api ev={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;(void)v;(void)w;CHECK(f==0x77&&h==HANDLER&&c==CTX&&px_battle_event_get(2,&ev)==7);
    if(h53)CHECK(px_battle_event_rewrite(0x53,v53,&ev)==1);
    if(h42)CHECK(px_battle_event_rewrite(0x42,v42,&ev)==1);
}
static uint32_t pokemon_value(void *u,uint32_t p,uint32_t k) { (void)u;(void)p;CHECK(k==0x11);return ability_of; }
static const struct px_battle_ability_api base={.events=&ev,.pokemon_value=pokemon_value};
static uint32_t action_record(void *u,uint32_t c,uint32_t k,uint32_t id)
{ (void)u;CHECK(c==CTX&&records<3);notef("rec%x/%x ",k,id);return RECORD+0x100*records++; }
static void message(void *u,uint32_t r,uint32_t k,uint32_t id)
{ (void)u;char t[64];snprintf(t,sizeof t,"msg%x@%x/%x ",id,(r-RECORD)&0xff,k);note(t); }
static void message_arg(void *u,uint32_t r,uint32_t v) { (void)u;notef("arg%x@%x ",v,(r-RECORD)&0xff); }
static void commit(void *u,uint32_t c,uint32_t r) { (void)u;CHECK(c==CTX&&r>=RECORD);notef("commit%u ",(r-RECORD)>>8,0); }
static void popup(void *u,uint32_t c,uint32_t phase,uint32_t id) { (void)u;CHECK(c==CTX);notef("popup%u/%x ",phase,id); }
static const struct px_battle_items_api items={&base,NULL,NULL,popup,action_record,message,message_arg,commit,0,0};
static const struct px_battle_core_api core={&items};
static uint32_t entries_fn(void *u,uint32_t f,uint32_t out) { (void)u;(void)f;(void)out;return 0; }
static uint32_t pokemon_value2(void *u,uint32_t p,uint32_t k) { return pokemon_value(u,p,k); }
static const struct px_battle_handler_api handler={&memory,NULL,abort_battle,pokemon_value2,NULL,entries_fn,BATTLE,POOL,0,0,0,0,0xf000};
static const struct px_battle_core_4_api api={&core,&handler,IDS};
static void reset(void)
{
    memset(mem,0,sizeof mem);aborts=0;records=0;log_[0]=0;h53=h42=0;v53=v42=0;ability_of=0;
    w32(NULL,CTX+8,TAB);w32(NULL,CTX+0x1c,OBJ);
    for(unsigned i=0;i<0x18;++i)w32(NULL,TAB+0x84+4*i,MON+0x200*i);
    static const uint8_t ids[4]={6,0xd,0x42,0x2b};for(unsigned i=0;i<4;++i)w8(NULL,IDS+i,ids[i]);
}
static uint32_t rnd(uint32_t *s) { *s=*s*1103515245u+12345u;return *s>>8; }
static void expect(const char *s) { if(strcmp(log_,s)){fprintf(stderr,"got  [%s]\nwant [%s]\n",log_,s);} CHECK(!strcmp(log_,s)); }
int main(void)
{
    unsigned cases=0;
    /* The grounded check against a model of the event keys (a key rewrites once). */
    for(uint32_t seed=0;seed<3000;++seed){
        reset();uint32_t s=seed+11;const uint32_t flag=rnd(&s)%2,t1=rnd(&s)%4,t2=rnd(&s)%4,cond=rnd(&s)%16;
        h53=(int)(rnd(&s)%2);h42=(int)(rnd(&s)%2);{static const uint32_t vals[5]={0,1,2,0x10,0x100};v53=vals[rnd(&s)%5];v42=vals[rnd(&s)%5];}
        w8(NULL,MON+0x19,7);w8(NULL,MON+0x100,(uint8_t)t1);w8(NULL,MON+0x101,(uint8_t)t2);w8(NULL,MON+0x102,0x12);
        w32(NULL,MON+0x70,(cond&1)?0x104:0x100);w32(NULL,MON+0x98,(cond&2)?0x104:0x100);w32(NULL,MON+0x9c,(cond&4)?0x104:0x100);w32(NULL,MON+0x94,(cond&8)?0x104:0x100);
        w32(NULL,BATTLE+0x60,HANDLER);w32(NULL,HANDLER+8,ENTRIES);w8(NULL,ENTRIES,0x12);w32(NULL,ENTRIES+4,0x77);
        w32(NULL,HANDLER+0x18,0x20000000u|1u<<16);w8(NULL,HANDLER+0x10,5);w8(NULL,HANDLER+0x3b,0);
        const uint32_t flying=flag?(t1==2||t2==2):0;
        uint32_t k53=flying,k42=0;int r53=0,r42=0;
        if(h53){k53=v53;r53=1;}if(h42){k42=v42;r42=1;}
        if(cond&1){if(!r42){k42=1;r42=1;}}
        if(cond&2){if(!r42){k42=1;r42=1;}}
        if((cond&4)||(cond&8)){if(!r53){k53=1;r53=1;}}
        const uint32_t want=((k42&0xff)==0&&(k53&0xff)!=0);
        CHECK(px_battle_grounded_check(CTX,MON,flag,&api)==want&&aborts==0&&r32(NULL,STACK)==0);++cases;
    }
    /* Airborne: the context flag, the battler lookup and its aborts. */
    for(uint32_t id=0;id<0x1a;++id)for(uint32_t blocked_flag=0;blocked_flag<2;++blocked_flag)for(uint32_t missing=0;missing<2;++missing){
        reset();w32(NULL,OBJ+0x27c,blocked_flag);w32(NULL,BATTLE+0x60,0);
        if(id<0x18&&missing)w32(NULL,TAB+0x84+4*id,0);
        const uint32_t battler=(id<0x18&&!missing)?MON+0x200*id:0;
        if(battler){w8(NULL,battler+0x100,0x2);w8(NULL,battler+0x101,0);w8(NULL,battler+0x102,0x12);}
        const uint32_t got=px_battle_is_airborne(CTX,id,&api);
        if(id<0x18){if(blocked_flag){CHECK(got==0);}else if(battler){CHECK(got==1);}}
        CHECK(aborts==(unsigned)((id>=0x18?1:0)+((id>=0x18||(missing&&!battler))?1:0)));++cases;}
    /* Exempt: the id table and bits at battler +0x169. */
    for(uint32_t mask=0;mask<5;++mask){reset();static const uint8_t ids[4]={6,0xd,0x42,0x2b};
        if(mask)w8(NULL,MON+0x169+(ids[mask-1]>>3),(uint8_t)(1u<<(ids[mask-1]&7)));
        CHECK(px_battle_is_exempt(CTX,0,&api)==(mask!=0)&&aborts==0);++cases;}
    /* Item usable: ability 0x67, the battler word and the context flag. */
    for(uint32_t mask=0;mask<16;++mask){reset();ability_of=(mask&1)?0x67:0x11;w32(NULL,MON+0x68,(mask&2)?0x104:0x100);w32(NULL,OBJ+0x290,(mask&4)?5:0);
        CHECK(px_battle_item_usable(CTX,0,&api)==(mask&7?0u:1u));++cases;}
    /* Flagged condition: byte +0x171 and the word +0xa0. */
    for(uint32_t v=0;v<0x40;++v)for(uint32_t word=0;word<3;++word){reset();w8(NULL,MON+0x171,(uint8_t)v);w32(NULL,MON+0xa0,word==1?0x104:word==2?0x101:0x100);
        const uint32_t want=(v!=0&&((v-1)&0xff)<0x18)?1u:(word!=0);CHECK(px_battle_has_flagged_condition(CTX,0,&api)==want);++cases;}
    /* Group table and the ring copy. */
    for(uint32_t slot=0;slot<0x1c;++slot){reset();const uint32_t g=px_battle_slot_group(slot,&handler);aborts=0;
        const uint32_t got=px_battle_group_table(CTX,slot,&api);CHECK(got==TAB+g*28+4&&aborts==(unsigned)((slot>0x17?1:0)));++cases;}
    for(uint32_t back=0;back<4;++back)for(uint32_t skip=0;skip<5;++skip)for(uint32_t cursor=0;cursor<3;++cursor){
        reset();w8(NULL,CTX+0x205,(uint8_t)cursor);for(uint32_t i=0;i<3;++i)w8(NULL,CTX+0x202+i,(uint8_t)(i+2));
        for(uint32_t i=0;i<0x90;++i)w8(NULL,CTX+0x172+i,(uint8_t)i);
        for(uint32_t i=0;i<8;++i)w8(NULL,BUF+i,0xa5);
        const uint32_t got=px_battle_ring_copy(CTX,back,skip,BUF,&memory);
        int32_t p=(int32_t)cursor-(int32_t)back;if(p<0)p+=3;
        const uint32_t length=back<3?(uint32_t)(p+2):0;
        const uint32_t want=(back<3&&length>skip)?1u:0u;
        CHECK(got==want);
        if(want){const uint32_t e=(length-skip-1)&0xff;for(uint32_t i=0;i<8;++i){CHECK(r8(NULL,BUF+i)==r8(NULL,CTX+0x172+(uint32_t)p*48+e*8+i));}}
        else{for(uint32_t i=0;i<8;++i){CHECK(r8(NULL,BUF+i)==0xa5);}}
        ++cases;}
    /* The group flag marker. */
    for(uint32_t mode=0;mode<4;++mode)for(uint32_t index=0;index<4;++index)for(uint32_t same=0;same<2;++same)for(uint32_t slot=0;slot<0x18;slot+=5){
        reset();w32(NULL,0xc800+4,0xc900);w32(NULL,0xc800,0xca00);w32(NULL,0xc900+0x10,0xcb00);w8(NULL,0xcb00,(uint8_t)mode);
        w32(NULL,0xca00+0x10,0xcc00);const uint32_t g=px_battle_slot_group(slot,&handler);aborts=0;w8(NULL,0xc900+0x815,(uint8_t)(same?g:g+1));
        px_battle_mark_group_flag(0xc800,slot,index,&api);
        const int ok=(mode<=1)&&same;unsigned wantabort=(slot>0x17?1u:0u)+((ok&&index>=2)?1u:0u);
        CHECK(aborts==wantabort);
        for(uint32_t k=0;k<2;++k){CHECK(r8(NULL,0xcc00+0xa50+k)==((ok&&index<2&&index==k)?1u:0u));}++cases;}
    /* Small accessors. */
    for(uint32_t index=0;index<0x28;++index){reset();for(uint32_t i=0;i<0x28;++i)w32(NULL,MON+0x1c+4*i,(i&1)?0x104u:0x100u);
        CHECK(px_battle_has_condition(MON,index,&handler)==(index&1)&&aborts==(unsigned)(index>=0x26));++cases;}
    for(uint32_t index=0;index<8;++index){reset();w8(NULL,CTX+0x152,4);for(uint32_t i=0;i<8;++i){w8(NULL,CTX+i*14+0x11e,(uint8_t)(i*3+250));w8(NULL,CTX+i*14+0x11f,(uint8_t)(i*5+3));}
        CHECK(px_battle_range_width(CTX,index,&handler)==(uint32_t)((uint8_t)(index*5+3)-(uint8_t)(index*3+250))%256&&aborts==(unsigned)(index>=4));++cases;}
    for(uint32_t n=0;n<5;++n){reset();for(uint32_t i=0;i<5;++i)w32(NULL,BUF+4*i,0x700+i);w8(NULL,BUF+0x18,(uint8_t)n);
        for(uint32_t i=0;i<6;++i){CHECK(px_battle_array_word_b(BUF,i,&memory)==(i<n?0x700+i:0));}++cases;}
    for(uint32_t v=0;v<0x300;v+=7){reset();w8(NULL,BUF+6,0xee);px_battle_fill_6(BUF,v,&memory);const uint32_t b=v&0xff;
        for(uint32_t i=0;i<6;++i){CHECK(r8(NULL,BUF+i)==b);}CHECK(r8(NULL,BUF+6)==0xee);++cases;}
    for(uint32_t a=0;a<0x80;a+=3)for(uint32_t b=0;b<0x300;b+=37){CHECK(px_battle_pack_7(a,b)==((((a<<3&0x1f8u)|2u)&~0x1fffe00u)|b<<9));++cases;}
    /* The record builders. */
    for(uint32_t key=0;key<2;++key){reset();w32(NULL,BATTLE+0x60,0);
        px_battle_event_push(&ev);px_battle_event_set_id(3,key?5:6,&ev);
        for(uint32_t i=0;i<4;++i){w8(NULL,MON+0x200*5+0x16d+i,(uint8_t)(0x30+i));}
        w32(NULL,RECORD+0x100,0xffffffffu);
        px_battle_report_slot(CTX,5,&api);px_battle_event_pop(&ev);
        if(key){expect("rec15/5 commit0 rec27/5 commit1 rec3/5 msg205@4/2 arg5@4 commit2 ");
            CHECK(r8(NULL,RECORD+4)==7&&r8(NULL,RECORD+5)==5&&r8(NULL,RECORD+0x104)==5&&r8(NULL,RECORD+0x105)==3&&r8(NULL,RECORD+0x106)==0x34&&r32(NULL,RECORD+0x100)==0xfeffffffu);}
        else{expect("");}
        ++cases;}
    for(uint32_t mask=0;mask<32;++mask){reset();w32(NULL,BATTLE+0x60,0);
        px_battle_event_push(&ev);px_battle_event_set_id(2,(mask&1)?9:8,&ev);px_battle_event_set_value(0x1a,(mask&2)?4:0,&ev);
        w32(NULL,STATE,(mask&4)?1:0);w32(NULL,STATE+4,(mask&8)?4:7);w32(NULL,RECORD+4,0);
        px_battle_popup_message(CTX,9,0x40,STATE,0x1234,&api);px_battle_event_pop(&ev);
        const int match=mask&1,active=mask&4,prev=(mask&2)?4:0,same=(mask&8)?(prev==4):0;
        if(!match||!active){CHECK(log_[0]==0&&r32(NULL,STATE)==(uint32_t)((mask&4)?1:0));}
        else if(prev&&same){CHECK(log_[0]==0&&r32(NULL,STATE)==0&&r32(NULL,STATE+4)==4);}
        else{expect("popup1/40 rec3/40 msg1234@4/2 arg9@4 commit0 popup2/40 ");CHECK(r32(NULL,STATE)==0&&r32(NULL,STATE+4)==(uint32_t)prev);
            CHECK((r16(NULL,RECORD+6)&0x8000)!=0);}
        ++cases;}
    printf("battle_core_4: %u cases, all tests passed\n",cases);
    return 0;
}
