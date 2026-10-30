/* Optional original-data sweep; no original resources are embedded. */
#include "../pokemon_personal.h"
#include "../../pokemon_learnset/pokemon_learnset.h"
#include "../../../native/assets/garc.h"
#include "../../../native/assets/pokedata.h"
#include "../../../native/platform/guest.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"check_personal:%d: %s\n",__LINE__,#c);exit(1); } } while (0)
enum { RECORD=0x100, SECOND=0x120, LEARNSET=0x140, PAYLOAD=0x1000,
       SECOND_PAYLOAD=0x1100, ENTRIES=0x2000, TABLE=0x10000000, GLOBALS=0x5d1700 };
struct context { struct px_guest guest;struct px_pokemon_api memory;struct px_garc *learnsets;uint32_t last_member;uint64_t loads; };
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
static uint32_t u16(const uint8_t *p) { return (uint32_t)p[0]|(uint32_t)p[1]<<8; }
static uint32_t u32(const uint8_t *p) { return u16(p)|(u16(p+2)<<16); }
static uint32_t expected(const uint8_t *raw,const struct px_personal *p,unsigned selector)
{
    if(selector<6)return p->base[selector];
    if(selector<8)return p->types[selector-6];
    if(selector>=10&&selector<=15)return u16(raw+10)/(1u<<(2*(selector-10)))%4;
    if(selector>=17&&selector<=19)return u16(raw+12+2*(selector-17));
    if(selector>=26&&selector<=28)return p->abilities[selector-26];
    if(selector>=39&&selector<=43)return u32(raw+40+4*(selector-39));
    switch(selector) {
    case 8:return p->catch_rate;case 9:return p->base_exp;case 16:return u16(raw+10)/4096%2;
    case 20:return p->gender;case 21:return raw[19];case 22:return raw[20];case 23:return p->growth;
    case 24:return raw[22];case 25:return raw[23];case 29:return p->escape_rate;
    case 30:return p->form_index;case 31:return u16(raw+30);case 32:return p->form_count;
    case 33:return raw[33]%64;case 34:return raw[33]/64%2;case 35:return raw[33]/128;
    case 36:return p->evolution_stage;case 37:return u16(raw+36);case 38:return u16(raw+38);
    case 44:return u16(raw+60);case 45:return raw[62];default:CHECK(0);return 0;
    }
}
static void select_cb(void *u,uint32_t record,uint32_t species,uint32_t form)
{ struct context *c=u;px_load_personal_record(record,species,form,&c->memory); }
static uint32_t field_cb(void *u,uint32_t record,uint32_t selector)
{ struct context *c=u;return px_personal_resource_field(record,selector,&c->memory); }
static void read_cb(void *u,uint32_t handle,uint32_t index,uint32_t destination)
{
    struct context *c=u;CHECK(handle==1&&destination==ENTRIES);size_t size;uint8_t *raw=member(c->learnsets,index,&size);
    CHECK(size<=104);uint8_t *to=px_guest_span(&c->guest,destination,104);CHECK(to);memset(to,0xff,104);memcpy(to,raw,size);
    c->last_member=index;++c->loads;free(raw);
}
int main(int argc,char **argv)
{
    if(argc!=2) { fprintf(stderr,"usage: check_personal ROMFS\n");return 2; }
    struct px_garc *g=archive(argv[1],"a/2/1/8");const size_t count=px_garc_count(g)-1;
    uint8_t *table=malloc(count*64);struct px_personal *personal=calloc(count,sizeof *personal);CHECK(table&&personal&&count>721);
    for(size_t i=0;i<count;++i) { size_t n;uint8_t *raw=member(g,i,&n);CHECK(n==64);memcpy(table+64*i,raw,64);CHECK(px_personal_decode(raw,n,personal+i)==PX_POKEDATA_OK);free(raw); }
    size_t flat_size;uint8_t *flat=member(g,count,&flat_size);CHECK(flat_size==count*64&&!memcmp(flat,table,flat_size));free(flat);px_garc_close(g);
    struct context c={0};px_guest_init(&c.guest);
    CHECK(px_guest_map(&c.guest,RECORD,0x200,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"records"));
    CHECK(px_guest_map(&c.guest,PAYLOAD,0x200,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"personal payloads"));
    CHECK(px_guest_map(&c.guest,ENTRIES,104,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"learnset entries"));
    CHECK(px_guest_map(&c.guest,GLOBALS,0x100,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"globals"));
    CHECK(px_guest_map_view(&c.guest,TABLE,(uint32_t)(count*64),PX_GUEST_READ,table,"original personal"));
    c.memory=px_guest_pokemon_api(&c.guest);c.learnsets=archive(argv[1],"a/2/1/4");
    px_guest_store32(&c.guest,0x5d177c,TABLE);px_guest_store32(&c.guest,0x5d17a4,RECORD);
    px_guest_store32(&c.guest,RECORD+8,PAYLOAD);px_guest_store32(&c.guest,SECOND+8,SECOND_PAYLOAD);
    px_guest_store32(&c.guest,LEARNSET+4,ENTRIES);px_guest_store32(&c.guest,0x5d17e8,SECOND);px_guest_store32(&c.guest,0x5d17ec,1);
    uint64_t field_cases=0,selections=0,learnset_cases=0;
    for(uint32_t i=0;i<count;++i) {
        memcpy(px_guest_span(&c.guest,PAYLOAD,64),table+64*i,64);
        for(unsigned s=0;s<46;++s) { CHECK(px_current_personal_value(s,&c.memory)==expected(table+64*i,personal+i,s));++field_cases; }
    }
    const struct px_learnset_api api={&c.memory,&c,select_cb,field_cb,read_cb};
    for(uint32_t species=0;species<=721;++species) {
        const struct px_personal *p=personal+species;const unsigned forms=p->form_count?p->form_count:1;
        for(uint32_t form=0;form<=forms;++form) {
            const uint32_t index=form&&p->form_index&&form<p->form_count?p->form_index+form-1:species;CHECK(index<count);
            px_select_personal_resource(species,form,&c.memory);
            CHECK(!memcmp(px_guest_span(&c.guest,PAYLOAD,64),table+64*index,64));
            CHECK(px_guest_load16(&c.guest,RECORD+4)==species&&px_guest_load8(&c.guest,RECORD+6)==form);++selections;
            px_guest_store16(&c.guest,LEARNSET,0xffff);
            px_load_learnset_record(LEARNSET,species,form,&api);CHECK(c.last_member==index);
            size_t size;uint8_t *raw=member(c.learnsets,index,&size);struct px_learnset reference;
            CHECK(px_learnset_decode(raw,size,&reference)==PX_POKEDATA_OK);free(raw);
            CHECK(px_learnset_record_count(LEARNSET,&c.memory)==reference.count);
            for(unsigned k=0;k<reference.count;++k) {
                CHECK(px_learnset_record_move(LEARNSET,k,&c.memory)==reference.move[k]);
                CHECK(px_learnset_record_level(LEARNSET,k,&c.memory)==(uint16_t)reference.level[k]);++learnset_cases;
            }
        }
    }
    CHECK(c.guest.fault==PX_FAULT_NONE);
    printf("{\"personal_records\":%zu,\"flat_table_bytes\":%zu,\"flat_table_matches_members\":true,\"selector_cases\":%" PRIu64
        ",\"species_form_boundary_selections\":%" PRIu64 ",\"learnset_entry_cases\":%" PRIu64 ",\"learnset_loads\":%" PRIu64 ",\"all_match\":true}\n",
        count,flat_size,field_cases,selections,learnset_cases,c.loads);
    px_garc_close(c.learnsets);px_guest_free(&c.guest);free(table);free(personal);return 0;
}
