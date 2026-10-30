/* Optional original-data sweep; no original resources are embedded. */
#define _FILE_OFFSET_BITS 64
#define _POSIX_C_SOURCE 200809L
#include "../pokemon_resources.h"
#include "../../pokemon_personal/pokemon_personal.h"
#include "../../pokemon_growth/pokemon_growth.h"
#include "../../pokemon_learnset/pokemon_learnset.h"
#include "../../pokemon_data/pokemon_data.h"
#include "../../pokemon_stats/stats_math.h"
#include "../../../native/assets/garc.h"
#include "../../../native/assets/pokedata.h"
#include "../../../native/platform/guest.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"check_resources:%d: %s\n",__LINE__,#c);exit(1); } } while (0)
enum { HEAP=0x40, SCRATCH=0x100, CONFIG=0x200, ARENA=0x10000000, ARENA_SIZE=64u<<20, HANDLE=0x1000 };
/* Configuration words 0..4 and 6 select these archives in this sweep. Words 0, 1, 2
 * and 6 follow from the data each consumer reads; words 3 and 4 are stand-ins. */
static const char *const paths[]={"a/2/1/8","a/2/1/4","a/2/1/7","a/2/1/5","a/2/1/6","a/2/1/9"};
struct context { struct px_guest guest;struct px_pokemon_api memory;struct px_archive_api io;struct px_archive_index_api index;
                 struct px_resource_api api;uint8_t *files[6];uint64_t sizes[6];uint32_t next,blocks,messages; };
static uint8_t *load(const char *romfs,const char *name,uint64_t *size)
{
    char path[4096];CHECK(snprintf(path,sizeof path,"%s/%s",romfs,name)<(int)sizeof path);FILE *f=fopen(path,"rb");CHECK(f);
    CHECK(fseeko(f,0,SEEK_END)==0);*size=(uint64_t)ftello(f);rewind(f);uint8_t *b=malloc(*size);CHECK(b&&fread(b,1,*size,f)==*size);fclose(f);return b;
}
static uint32_t bump(struct context *c,uint32_t size) { const uint32_t p=c->next;c->next+=(size+15)&~15u;CHECK(c->next-ARENA<=ARENA_SIZE);return p; }
static uint32_t object_allocate(void *u,uint32_t size,uint32_t heap) { CHECK(heap==HEAP);return bump(u,size); }
static unsigned file_index(uint32_t handle) { CHECK(handle>=HANDLE&&handle<HANDLE+12&&handle%2==0);return (handle-HANDLE)/2; }
static void lock(void *u,uint32_t s) { (void)u;(void)s; }
static uint32_t is_open(void *u,uint32_t o) { (void)u;(void)o;return 1; }
static uint32_t get_size(void *u,uint32_t h,uint64_t *size) { struct context *c=u;*size=c->sizes[file_index(h)];return 0; }
static uint32_t file_read(void *u,uint32_t h,uint64_t position,uint32_t buffer,uint32_t size,uint32_t *count)
{
    struct context *c=u;const unsigned i=file_index(h);uint32_t n=size;CHECK(position<=c->sizes[i]);
    if(position+n>c->sizes[i])n=(uint32_t)(c->sizes[i]-position);
    uint8_t *to=px_guest_span(&c->guest,buffer,n?n:1);CHECK(to);memcpy(to,c->files[i]+position,n);*count=n;return 0;
}
static void fatal(void *u,uint32_t r,uint32_t s) { (void)u;fprintf(stderr,"fatal %08x at %08x\n",r,s);exit(1); }
static void panic(void *u,uint32_t h,uint32_t m) { (void)u;(void)h;(void)m;CHECK(0); }
static void stream_info(void *u,uint32_t a,uint32_t i,uint32_t info[3]) { struct context *c=u;px_archive_member_info_streamed(a,i,info,&c->index); }
static uint32_t try_enter(void *u,uint32_t s) { (void)u;(void)s;return 1; }
static void signal_event(void *u,uint32_t e) { struct context *c=u;px_guest_store32(&c->guest,e,1); }
static uint32_t temporary_heap(void *u,uint32_t h) { (void)u;CHECK(h==HEAP);return HEAP; }
static uint32_t alignment(void *u,uint32_t h) { (void)u;CHECK(h==HEAP);return 4; }
static uint32_t heap_allocate(void *u,uint32_t h,uint32_t size,uint32_t a,uint32_t f) { (void)a;(void)f;CHECK(h==HEAP);return bump(u,size); }
static void release(void *u,uint32_t p) { (void)u;(void)p; }
static uint32_t construct(void *u,uint32_t object,uint32_t heap,uint32_t id,uint32_t flags)
{
    struct context *c=u;CHECK(heap==HEAP&&flags==1&&id<6);
    px_guest_store32(&c->guest,object+4,HEAP);px_guest_store32(&c->guest,object+0x18,HANDLE+2*id);
    for(uint32_t o=0x1c;o<0x2c;o+=4)px_guest_store32(&c->guest,object+o,0);
    px_guest_store16(&c->guest,object+0x34,0);px_guest_store32(&c->guest,object+0x3c,0);px_guest_store32(&c->guest,object+0x4c,0xfffffffeu);
    return object;
}
static uint32_t allocate_block(void *u,uint32_t heap,uint32_t size) { struct context *c=u;CHECK(heap==HEAP);++c->blocks;return bump(c,size); }
static uint32_t construct_message(void *u,uint32_t o,uint32_t a,uint32_t m,uint32_t h,uint32_t mode,uint32_t f)
{ struct context *c=u;(void)a;(void)m;(void)h;(void)mode;(void)f;++c->messages;return o; }
static void select_personal(void *u,uint32_t record,uint32_t species,uint32_t form) { struct context *c=u;px_load_personal_record(record,species,form,&c->memory); }
static uint32_t personal_value(void *u,uint32_t record,uint32_t selector) { struct context *c=u;return px_personal_resource_field(record,selector,&c->memory); }
static void read_member(void *u,uint32_t archive,uint32_t index,uint32_t destination) { struct context *c=u;(void)px_read_archive_member(archive,index,destination,&c->io); }
static void select_growth(void *u,uint32_t species,uint32_t form)
{ struct context *c=u;const struct px_growth_api g={&c->memory,c,read_member};px_select_growth_resource(species,form,&g); }
static uint32_t u32(const uint8_t *p) { return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24; }
int main(int argc,char **argv)
{
    if(argc!=2) { fprintf(stderr,"usage: check_resources ROMFS\n");return 2; }
    struct context c={0};c.next=ARENA;px_guest_init(&c.guest);
    for(unsigned i=0;i<6;++i)c.files[i]=load(argv[1],paths[i],&c.sizes[i]);
    CHECK(px_guest_map(&c.guest,0,0x1000,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"scratch/config"));
    CHECK(px_guest_map(&c.guest,0x5d1000,0x1000,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"resource cells"));
    CHECK(px_guest_map(&c.guest,0x5e5000,0x1000,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"archive cells"));
    CHECK(px_guest_map(&c.guest,ARENA,ARENA_SIZE,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"heap"));
    /* The guest is the context's first member, so its user pointer is the context. */
    c.memory=px_guest_pokemon_api(&c.guest);CHECK(c.memory.user==(void *)&c);c.memory.allocate=object_allocate;
    c.io=(struct px_archive_api){&c.memory,&c,lock,lock,is_open,get_size,file_read,fatal,stream_info,panic};
    c.index=(struct px_archive_index_api){&c.io,&c,try_enter,signal_event,temporary_heap,alignment,heap_allocate,release,construct,SCRATCH};
    c.api=(struct px_resource_api){&c.index,&c,allocate_block,construct_message};
    const uint32_t words[21]={0,1,2,3,4,0,5,3,8,4,2,2,0,0,0,0,0,9,0,1,2};
    px_guest_store32(&c.guest,PX_RESOURCE_CONFIG_CELL,CONFIG);
    for(uint32_t i=0;i<21;++i)px_guest_store32(&c.guest,CONFIG+4*i,words[i]);
    px_initialize_pokemon_resources(HEAP,&c.api);
    CHECK(c.blocks==2&&c.messages==3);
    /* Personal table: the loaded block equals every independently decoded row. */
    struct px_garc *g=NULL;char error[256];char path[4096];
    CHECK(snprintf(path,sizeof path,"%s/a/2/1/8",argv[1])<(int)sizeof path&&px_garc_open(path,&g,error,sizeof error));
    const size_t count=px_garc_count(g)-1;struct px_personal *personal=calloc(count,sizeof *personal);uint8_t *rows=malloc(64*count);CHECK(personal&&rows);
    for(size_t i=0;i<count;++i) { uint8_t *raw=NULL;size_t n=0;const struct px_garc_member *m=px_garc_member_at(g,i);
        CHECK(px_garc_read(g,m->entry,m->subentry,64,&raw,&n,error,sizeof error)&&n==64&&px_personal_decode(raw,n,personal+i)==PX_POKEDATA_OK);memcpy(rows+64*i,raw,64);free(raw); }
    px_garc_close(g);
    const uint32_t table=px_guest_load32(&c.guest,0x5d177c);
    CHECK(!memcmp(px_guest_span(&c.guest,table,(uint32_t)(64*count)),rows,64*count));
    /* Tail block: the final a/2/1/9 member. */
    const uint8_t *tail=c.files[5];const uint32_t entries=tail[0x24]|(uint32_t)tail[0x25]<<8;
    const uint8_t *fatb=tail+0x1c+u32(tail+0x20);const uint8_t *rec=fatb+12+u32(tail+0x1c+12+4*(entries-1));
    const uint32_t tail_size=u32(rec+8)-u32(rec+4);
    CHECK(tail_size==0x5a4&&!memcmp(px_guest_span(&c.guest,px_guest_load32(&c.guest,PX_RESOURCE_BASE+0x44),tail_size),tail+u32(tail+0x10)+u32(rec+4),tail_size));
    /* Services from the booted state: personal fields, growth levels and learnsets. */
    struct px_garc *curves=NULL,*learnsets=NULL;
    CHECK(snprintf(path,sizeof path,"%s/a/2/1/7",argv[1])<(int)sizeof path&&px_garc_open(path,&curves,error,sizeof error));
    CHECK(snprintf(path,sizeof path,"%s/a/2/1/4",argv[1])<(int)sizeof path&&px_garc_open(path,&learnsets,error,sizeof error));
    uint8_t curve[8][404];
    for(unsigned r=0;r<8;++r) { uint8_t *raw=NULL;size_t n=0;const struct px_garc_member *m=px_garc_member_at(curves,r);
        CHECK(px_garc_read(curves,m->entry,m->subentry,404,&raw,&n,error,sizeof error)&&n==404);memcpy(curve[r],raw,404);free(raw); }
    const struct px_data_api data={&c.memory,&c,select_growth};
    const struct px_learnset_api api={&c.memory,&c,select_personal,personal_value,read_member};
    uint64_t selections=0,fields=0,levels=0,entries_checked=0;
    for(uint32_t species=1;species<=721;++species) {
        const struct px_personal *p=personal+species;const unsigned forms=p->form_count?p->form_count:1;
        for(uint32_t form=0;form<forms;++form) {
            const uint32_t index=form&&p->form_index&&form<p->form_count?p->form_index+form-1:species;
            px_select_personal_resource(species,form,&c.memory);++selections;
            CHECK(px_current_personal_value(0x17,&c.memory)==personal[index].growth);
            for(unsigned s=0;s<6;++s) { CHECK(px_current_personal_value(s,&c.memory)==personal[index].base[s]);++fields; }
            const uint8_t *cv=curve[personal[index].growth];
            for(uint32_t level=1;level<=100;level+=9) { CHECK(px_pokemon_level_from_growth(species,form,u32(cv+4*level),&data)==level);++levels; }
            px_select_pokemon_learnset(species,form,&api);const uint32_t record=px_guest_load32(&c.guest,0x5d17b4);
            uint8_t *raw=NULL;size_t n=0;const struct px_garc_member *m=px_garc_member_at(learnsets,index);
            CHECK(px_garc_read(learnsets,m->entry,m->subentry,4096,&raw,&n,error,sizeof error));
            struct px_learnset reference;CHECK(px_learnset_decode(raw,n,&reference)==PX_POKEDATA_OK);free(raw);
            CHECK(px_learnset_record_count(record,&c.memory)==reference.count);
            for(unsigned k=0;k<reference.count;++k) {
                CHECK(px_learnset_record_move(record,k,&c.memory)==reference.move[k]);
                CHECK(px_learnset_record_level(record,k,&c.memory)==(uint16_t)reference.level[k]);++entries_checked;
            }
        }
    }
    CHECK(c.guest.fault==PX_FAULT_NONE);
    printf("{\"personal_rows_loaded\":%zu,\"personal_table_bytes\":%zu,\"tail_member_bytes\":%u,\"species_form_selections\":%" PRIu64
           ",\"personal_fields\":%" PRIu64 ",\"growth_levels\":%" PRIu64 ",\"learnset_entries\":%" PRIu64 ",\"heap_bytes\":%u,\"all_match\":true}\n",
           count,64*count,tail_size,selections,fields,levels,entries_checked,c.next-ARENA);
    px_garc_close(curves);px_garc_close(learnsets);free(personal);free(rows);
    for(unsigned i=0;i<6;++i)free(c.files[i]);
    px_guest_free(&c.guest);
    return 0;
}
