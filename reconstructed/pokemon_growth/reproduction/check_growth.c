/* Optional original-data sweep; no original resources are embedded. */
#include "../pokemon_growth.h"
#include "../../pokemon_data/pokemon_data.h"
#include "../../pokemon_stats/stats_math.h"
#include "../../../native/assets/garc.h"
#include "../../../native/assets/pokedata.h"
#include "../../../native/platform/guest.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"check_growth:%d: %s\n",__LINE__,#c);exit(1); } } while (0)
enum { DESCRIPTOR=0x100, PAYLOAD=0x200, RECORD=0x1000, SLOTS=0x2000, SLOT_STRIDE=0x200, SLOT_COUNT=3,
       CACHE=0x3000, CACHE_TABLE=0x3100, TABLE=0x10000000, GLOBALS=0x5d1700, ARCHIVE_PAGE=0x5e5000, HANDLE=7 };
struct context { struct px_guest guest;struct px_pokemon_api memory;struct px_growth_api api;uint8_t (*curves)[404];size_t curve_count;uint64_t loads; };
static struct px_garc *archive(const char *romfs,const char *name)
{
    char path[2048],error[256];struct px_garc *g=NULL;CHECK(snprintf(path,sizeof path,"%s/%s",romfs,name)<(int)sizeof path);
    CHECK(px_garc_open(path,&g,error,sizeof error));return g;
}
static uint8_t *member(struct px_garc *g,size_t index,size_t *size)
{
    const struct px_garc_member *m=px_garc_member_at(g,index);uint8_t *bytes=NULL;char error[256];CHECK(m);
    CHECK(px_garc_read(g,m->entry,m->subentry,1u<<24,&bytes,size,error,sizeof error));return bytes;
}
static uint32_t u32(const uint8_t *p) { return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24; }
static void read_cb(void *u,uint32_t handle,uint32_t index,uint32_t destination)
{
    struct context *c=u;CHECK(handle==HANDLE&&destination==RECORD+8&&index<c->curve_count);
    uint8_t *to=px_guest_span(&c->guest,destination,404);CHECK(to);memcpy(to,c->curves[index],404);++c->loads;
}
static void select_cb(void *u,uint32_t species,uint32_t form) { struct context *c=u;px_select_growth_resource(species,form,&c->api); }
static uint32_t reference_level(const uint8_t *curve,uint32_t growth)
{
    uint32_t level=1;while(level<100&&u32(curve+4*(level+1))<=growth)++level;return level;
}
int main(int argc,char **argv)
{
    if(argc!=2) { fprintf(stderr,"usage: check_growth ROMFS\n");return 2; }
    struct px_garc *g=archive(argv[1],"a/2/1/8");const size_t count=px_garc_count(g)-1;
    uint8_t *table=malloc(count*64);struct px_personal *personal=calloc(count,sizeof *personal);CHECK(table&&personal&&count>721);
    for(size_t i=0;i<count;++i) { size_t n;uint8_t *raw=member(g,i,&n);CHECK(n==64);memcpy(table+64*i,raw,64);CHECK(px_personal_decode(raw,n,personal+i)==PX_POKEDATA_OK);free(raw); }
    px_garc_close(g);
    struct context c={0};g=archive(argv[1],"a/2/1/7");c.curve_count=px_garc_count(g);c.curves=calloc(c.curve_count,404);CHECK(c.curves);
    for(size_t i=0;i<c.curve_count;++i) {
        size_t n;uint8_t *raw=member(g,i,&n);CHECK(n==404);memcpy(c.curves[i],raw,404);free(raw);
        for(unsigned l=1;l<=100;++l)CHECK(u32(c.curves[i]+4*l)>=u32(c.curves[i]+4*(l-1)));
    }
    px_garc_close(g);
    px_guest_init(&c.guest);
    CHECK(px_guest_map(&c.guest,DESCRIPTOR,0x4000,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"records"));
    CHECK(px_guest_map(&c.guest,GLOBALS,0x100,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"globals"));
    CHECK(px_guest_map(&c.guest,ARCHIVE_PAGE,0x1000,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"archive cell"));
    CHECK(px_guest_map_view(&c.guest,TABLE,(uint32_t)(count*64),PX_GUEST_READ,table,"original personal"));
    c.memory=px_guest_pokemon_api(&c.guest);c.api=(struct px_growth_api){&c.memory,&c,read_cb};
    px_guest_store32(&c.guest,0x5d177c,TABLE);px_guest_store32(&c.guest,0x5d17a8,DESCRIPTOR);px_guest_store32(&c.guest,DESCRIPTOR+8,PAYLOAD);
    px_guest_store32(&c.guest,0x5d17ac,RECORD);px_guest_store32(&c.guest,0x5d17b0,CACHE);px_guest_store32(&c.guest,0x5e52b0,HANDLE);
    px_guest_store32(&c.guest,RECORD+4,UINT32_MAX);px_guest_store32(&c.guest,CACHE+4,CACHE_TABLE);
    px_guest_store8(&c.guest,CACHE+8,SLOT_COUNT);px_guest_store8(&c.guest,CACHE+9,0);
    for(unsigned i=0;i<SLOT_COUNT;++i) { px_guest_store32(&c.guest,CACHE_TABLE+4*i,SLOTS+SLOT_STRIDE*i);px_guest_store32(&c.guest,SLOTS+SLOT_STRIDE*i+4,UINT32_MAX); }
    const struct px_data_api data={&c.memory,&c,select_cb};
    uint64_t selections=0,threshold_cases=0,level_cases=0;
    for(uint32_t species=0;species<=721;++species) {
        const struct px_personal *p=personal+species;const unsigned forms=p->form_count?p->form_count:1;
        for(uint32_t form=0;form<=forms;++form) {
            const uint32_t index=form&&p->form_index&&form<p->form_count?p->form_index+form-1:species;CHECK(index<count);
            const uint32_t rate=personal[index].growth;CHECK(rate<c.curve_count);
            px_select_growth_resource(species,form,&c.api);++selections;
            CHECK(px_guest_load32(&c.guest,RECORD+4)==rate);
            CHECK(!memcmp(px_guest_span(&c.guest,RECORD+8,404),c.curves[rate],404));
            for(uint32_t level=0;level<=101;++level,++threshold_cases)
                CHECK(px_pokemon_growth_value(level,&data)==u32(c.curves[rate]+4*(level>100?100:level)));
            for(uint32_t level=1;level<=100;++level) {
                const uint32_t at=u32(c.curves[rate]+4*level);const uint32_t probes[3]={at,at?at-1:0,at+1};
                for(unsigned k=0;k<3;++k,++level_cases)
                    CHECK(px_pokemon_level_from_growth(species,form,probes[k],&data)==reference_level(c.curves[rate],probes[k]));
            }
        }
    }
    CHECK(c.guest.fault==PX_FAULT_NONE);
    printf("{\"personal_records\":%zu,\"growth_curves\":%zu,\"growth_curve_bytes\":404,\"species_form_selections\":%" PRIu64
        ",\"threshold_cases\":%" PRIu64 ",\"level_from_growth_cases\":%" PRIu64 ",\"archive_loads\":%" PRIu64 ",\"cache_slots\":%d,\"all_match\":true}\n",
        count,c.curve_count,selections,threshold_cases,level_cases,c.loads,SLOT_COUNT);
    px_guest_free(&c.guest);free(c.curves);free(table);free(personal);return 0;
}
