#include "battle_typechange.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);exit(1); } } while (0)
enum { SIZE=0x10000, STACK=0x1000, BATTLE=0x2000, POOL=0x2100, CTX=0x3000, TABLE=0x3100, MON=0x4000, WORK=0x5000, HANDLER=0x6000 };
static uint8_t mem[SIZE];
static uint8_t r8(void *u,uint32_t a) { (void)u;CHECK(a<SIZE);return mem[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;CHECK(a<SIZE);mem[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static uint32_t weather_value,changed_code,changed_calls,usable,ghost,params[0x300];static unsigned aborts;static char log_[256];
static void note(const char *s) { strncat(log_,s,sizeof log_-strlen(log_)-1); }
static void abort_battle(void *u) { (void)u;++aborts;note("ABORT "); }
static void overflow(void *u) { (void)u;CHECK(0); }
static uint32_t blocked(void *u,uint32_t c,uint32_t h,uint32_t e) { (void)u;(void)c;(void)h;(void)e;return 0; }
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w);
static const struct px_battle_event_api events={&memory,NULL,abort_battle,overflow,blocked,call,STACK,BATTLE,POOL};
static const struct px_battle_ability_api base={&events,NULL,NULL,NULL,NULL,NULL,NULL};
static uint32_t weather(void *u,uint32_t c) { (void)u;CHECK(c==CTX);note("w ");return weather_value; }
static void type_changed(void *u,uint32_t c,uint32_t code) { (void)u;CHECK(c==CTX);note("tc ");changed_code=code;++changed_calls; }
static uint32_t item_usable(void *u,uint32_t c,uint32_t id) { (void)u;CHECK(c==CTX&&id==2);note("iu ");return usable; }
static uint32_t hidden_power_type(void *u,uint32_t d) { (void)u;return d%18; }
static uint32_t item_param(void *u,uint32_t item,uint32_t p) { (void)u;CHECK(item<0x300&&p==8);return params[item]; }
static uint32_t has_type(void *u,uint32_t p,uint32_t t) { (void)u;CHECK(p==MON+0x200&&t==7);return ghost; }
static const struct px_battle_typechange_api api={&base,weather,type_changed,item_usable,hidden_power_type,item_param,has_type};
static void call(void *u,uint32_t f,uint32_t h,uint32_t c,uint32_t v,uint32_t w)
{
    (void)u;
    static void (*const table[])(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_typechange_api*)={
        px_battle_normalize,px_battle_aerilate_type,px_battle_refrigerate_type};
    CHECK(f<3);table[f](h,c,v,w,&api);
}
static void reset(void)
{
    memset(mem,0,sizeof mem);memset(params,0,sizeof params);weather_value=0;changed_code=99;changed_calls=0;usable=1;ghost=0;
    aborts=0;log_[0]=0;w32(NULL,CTX+8,TABLE);for(unsigned i=0;i<8;++i)w32(NULL,TABLE+0x84+4*i,MON+0x100*i);
}
/* The move-type frame: user, type (a value entry, rewritable once), Curse form and Struggle flag. */
static void frame(uint32_t user,uint32_t type)
{
    px_battle_event_push(&events);
    px_battle_event_set_id(0x02,user,&events);px_battle_event_set_value(0x16,type,&events);
    px_battle_event_set_value(0x1c,0x55,&events);px_battle_event_set_value(0x4c,0,&events);log_[0]=0;
}
static uint32_t type(void) { return px_battle_event_get(0x16,&events); }
typedef void (*fn)(uint32_t,uint32_t,uint32_t,uint32_t,const struct px_battle_typechange_api*);
/* Independent plate table (item id -> type). */
static uint32_t plate(uint32_t item)
{
    static const struct { uint32_t item,type; } plates[]={{298,9},{299,10},{300,12},{301,11},{302,14},{303,1},{304,3},{305,4},
        {306,2},{307,13},{308,6},{309,5},{310,7},{311,15},{312,16},{313,8},{644,17}};
    for(unsigned i=0;i<sizeof plates/sizeof *plates;++i)if(plates[i].item==item)return plates[i].type;
    return 0;
}
int main(void)
{
    unsigned cases=0;
    /* No effect unless key 0x02 is the battler. */
    {
        const fn all[]={px_battle_normalize,px_battle_aerilate_type,px_battle_pixilate_type,px_battle_refrigerate_type,px_battle_weather_ball,
            px_battle_curse,px_battle_struggle,px_battle_hidden_power,px_battle_judgment,px_battle_natural_gift,px_battle_techno_blast};
        for(unsigned k=0;k<sizeof all/sizeof *all;++k) {
            reset();weather_value=1;w16(NULL,MON+0x312,300);frame(3,0);w32(NULL,WORK,7);all[k](HANDLER,CTX,2,WORK,&api);
            CHECK(type()==0&&px_battle_event_get(0x1c,&events)==0x55&&px_battle_event_get(0x4c,&events)==0&&changed_calls==0);
            CHECK(r32(NULL,WORK)==(k>=1&&k<=3?0u:7u));++cases;   /* the converters clear the work word first */
        }
    }
    /* Normalize: any type becomes Normal. */
    for(uint32_t t=0;t<0x14;++t) { reset();frame(2,t);px_battle_normalize(HANDLER,CTX,2,WORK,&api);CHECK(type()==0);++cases; }
    /* Converters: only Normal (full word), and the work word records the accepted rewrite. */
    {
        const fn conv[3]={px_battle_aerilate_type,px_battle_pixilate_type,px_battle_refrigerate_type};const uint32_t to[3]={2,17,14};
        for(unsigned k=0;k<3;++k)for(uint32_t t=0;t<0x14;++t)for(unsigned pre=0;pre<2;++pre) {
            reset();frame(2,t==0x13?0x100:t);if(pre)px_battle_event_rewrite(0x16,t==0x13?0x100:t,&events);
            conv[k](HANDLER,CTX,2,WORK,&api);
            const int on=t==0&&!pre;
            CHECK(type()==(on?to[k]:t==0x13?0x100u:t)&&r32(NULL,WORK)==(uint32_t)on);++cases;
        }
    }
    /* Weather Ball: the four weathers and the code passed on; other weather keeps the type's low byte. */
    for(uint32_t w=0;w<7;++w)for(uint32_t t=0;t<0x14;t+=5) {
        reset();weather_value=w;frame(2,t|0x200);px_battle_weather_ball(HANDLER,CTX,2,WORK,&api);
        const uint32_t types[5]={t,9,10,14,5},codes[5]={0,1,4,2,3};
        CHECK(type()==(w>=1&&w<=4?types[w]:t)&&changed_code==(w>=1&&w<=4?codes[w]:0)&&!strcmp(log_,"w tc "));++cases;
    }
    /* Curse: form 0 for a Ghost user, 7 otherwise. */
    for(uint32_t g=0;g<3;++g) { reset();ghost=g;frame(2,0);px_battle_curse(HANDLER,CTX,2,WORK,&api);CHECK(px_battle_event_get(0x1c,&events)==(g?0u:7u));++cases; }
    /* Struggle sets key 0x4c. */
    reset();frame(2,0);px_battle_struggle(HANDLER,CTX,2,WORK,&api);CHECK(px_battle_event_get(0x4c,&events)==1);++cases;
    /* Hidden Power: the type service of the battler's first word; an empty slot aborts and gives Normal. */
    for(uint32_t d=0;d<40;++d) { reset();w32(NULL,MON+0x200,d);frame(2,5);px_battle_hidden_power(HANDLER,CTX,2,WORK,&api);CHECK(type()==d%18);++cases; }
    reset();w32(NULL,TABLE+0x84+8,0);frame(2,5);px_battle_hidden_power(HANDLER,CTX,2,WORK,&api);CHECK(type()==0&&aborts==2);++cases;
    /* Judgment: every item id against the plate table; an unusable item changes nothing. */
    for(uint32_t item=0;item<0x300;++item)for(uint32_t u=0;u<2;++u) {
        reset();usable=u;w16(NULL,MON+0x212,(uint16_t)item);frame(2,5);px_battle_judgment(HANDLER,CTX,2,WORK,&api);
        CHECK(type()==(u?plate(item):5u)&&px_battle_plate_type(item)==plate(item));++cases;
    }
    CHECK(px_battle_plate_type(0xffffffffu)==0&&px_battle_plate_type(0x10000+298)==0);++cases;
    /* Natural Gift: parameter 8 of the held item. */
    for(uint32_t item=0;item<0x300;item+=7) { reset();params[item]=item%17;w16(NULL,MON+0x212,(uint16_t)item);frame(2,5);px_battle_natural_gift(HANDLER,CTX,2,WORK,&api);CHECK(type()==item%17);++cases; }
    /* Techno Blast: the four Drives, Normal otherwise, the code passed on; an unusable item changes nothing. */
    for(uint32_t item=100;item<130;++item)for(uint32_t u=0;u<2;++u) {
        reset();usable=u;w16(NULL,MON+0x212,(uint16_t)item);frame(2,5);px_battle_techno_blast(HANDLER,CTX,2,WORK,&api);
        const uint32_t t=item==116?10:item==117?12:item==118?9:item==119?14:0,c=item>=116&&item<=119?item-115:0;
        CHECK(type()==(u?t:5u)&&changed_calls==u&&(!u||changed_code==c));++cases;
    }
    /* Through the dispatcher: Normalize then Aerilate. Normalize's rewrite uses up the value entry, so
     * Aerilate's rewrite of the (now Normal) type is refused and its work word stays 0. */
    {
        reset();
        for(unsigned i=0;i<2;++i) {
            const uint32_t h=HANDLER+0x40*i,entries=0x7000+0x10*i;
            w8(NULL,entries,0x28);w32(NULL,entries+4,i);
            w32(NULL,h+8,entries);w32(NULL,h+0x18,0x20000000u|1u<<16);w8(NULL,h+0x3a,2);
            w32(NULL,h,i?h-0x40:0);w32(NULL,h+4,i<1?h+0x40:0);
        }
        w32(NULL,BATTLE+0x60,HANDLER);w32(NULL,HANDLER+0x40+0x1c,9);
        frame(2,6);px_battle_event_dispatch(CTX,0x28,&events);
        CHECK(type()==0&&r32(NULL,HANDLER+0x40+0x1c)==0&&aborts==0);++cases;
    }
    printf("battle_typechange: %u cases, all tests passed\n",cases);
    return 0;
}
