/* Compare reconstructed loading with the existing independent native decoder. */
#include "../message_resources.h"
#include "../../pokemon_names/pokemon_names.h"
#include "../../../native/assets/garc.h"
#include "../../../native/assets/gf_text.h"
#include "../../../native/platform/guest.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"check_messages:%d: %s\n",__LINE__,#c);exit(1); } } while (0)
enum { RESOURCE=0x100, STRING=0x200, LANG=0x5d113c, NAMES=0x5d1790,
       FILE_BASE=0x10000000, SECTION=0x20000000, DEST=0x30000000,
       TEMP=0x31000000, NAME_DEST=0x32000000, BUFFER_BYTES=131072 };
struct context {
    struct px_guest guest; struct px_message_api api;
    const uint8_t *raw;size_t size;uint32_t position,allocated;
    uint64_t reads,allocations;
};
static uint16_t u16(const uint8_t *p) { return (uint16_t)((uint32_t)p[0]|(uint32_t)p[1]<<8); }
static uint32_t u32(const uint8_t *p) { return (uint32_t)u16(p)|(uint32_t)u16(p+2)<<16; }
static void seek_cb(void *u,uint32_t archive,uint32_t member,uint32_t offset)
{ struct context *c=u;CHECK(archive==1&&member==1&&offset<=c->size);c->position=offset; }
static void read_cb(void *u,uint32_t archive,uint32_t size,uint32_t destination)
{
    struct context *c=u;CHECK(archive==1&&(uint64_t)c->position+size<=c->size);
    uint8_t *p=px_guest_span(&c->guest,destination,size?size:1);CHECK(p);
    memcpy(p,c->raw+c->position,size);c->position+=size;++c->reads;
}
static uint32_t alignment_cb(void *u,uint32_t heap) { (void)u;CHECK(heap==1);return 16; }
static uint32_t allocate_cb(void *u,uint32_t heap,uint32_t size,uint32_t alignment,uint32_t tail)
{
    struct context *c=u;CHECK(heap==1&&size<BUFFER_BYTES&&alignment==0xfffffff0&&tail==1);
    c->allocated=size;++c->allocations;return TEMP;
}
static void release_cb(void *u,uint32_t address)
{ struct context *c=u;CHECK(address==TEMP);memset(px_guest_span(&c->guest,TEMP,c->allocated?c->allocated:1),0xdd,c->allocated); }
static uint32_t name_load(void *u,uint32_t resource,uint32_t index,uint32_t string)
{ struct context *c=u;return px_load_message(resource,index,string,&c->api); }
static void initialize(struct context *c,const uint8_t *raw,size_t size,unsigned mode)
{
    memset(c,0,sizeof *c);c->raw=raw;c->size=size;px_guest_init(&c->guest);
    CHECK(size>0&&size<=UINT32_MAX);
    CHECK(px_guest_map(&c->guest,RESOURCE,40,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"resource"));
    CHECK(px_guest_map(&c->guest,STRING,16,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"string"));
    CHECK(px_guest_map(&c->guest,LANG,4,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"language"));
    CHECK(px_guest_map(&c->guest,NAMES,8,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"name globals"));
    CHECK(px_guest_map_view(&c->guest,FILE_BASE,(uint32_t)size,PX_GUEST_READ,(void *)raw,"original file"));
    CHECK(px_guest_map(&c->guest,SECTION,(uint32_t)size,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"section"));
    CHECK(px_guest_map(&c->guest,DEST,BUFFER_BYTES,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"decoded text"));
    CHECK(px_guest_map(&c->guest,TEMP,BUFFER_BYTES,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"temporary"));
    CHECK(px_guest_map(&c->guest,NAME_DEST,BUFFER_BYTES,PX_GUEST_READ|PX_GUEST_WRITE,NULL,0,"built name"));
    px_guest_store32(&c->guest,RESOURCE+4,1);px_guest_store32(&c->guest,RESOURCE+8,mode?1:0);
    px_guest_store8(&c->guest,RESOURCE+12,(uint8_t)(mode!=2));px_guest_store32(&c->guest,RESOURCE+16,1);
    px_guest_store32(&c->guest,RESOURCE+20,UINT32_MAX);px_guest_store32(&c->guest,RESOURCE+24,FILE_BASE);
    px_guest_store32(&c->guest,RESOURCE+28,SECTION);px_guest_store32(&c->guest,STRING+4,DEST);
    px_guest_store16(&c->guest,STRING+8,65535);px_guest_store8(&c->guest,STRING+12,1);
    px_guest_store32(&c->guest,NAMES,STRING);px_guest_store32(&c->guest,NAMES+4,RESOURCE);
}

int main(int argc,char **argv)
{
    if(argc!=2) { fprintf(stderr,"usage: check_messages ROMFS\n");return 2; }
    static const char *archives[]={"a/0/7/4","a/0/8/2"};
    uint64_t files=0,empty=0,lines=0,units=0,cases=0,name_cases=0,io_reads=0,allocations=0;
    uint32_t longest=0;
    for(unsigned a=0;a<2;++a) {
        char path[2048],error[256];struct px_garc *g=NULL;
        CHECK(snprintf(path,sizeof path,"%s/%s",argv[1],archives[a])<(int)sizeof path);
        CHECK(px_garc_open(path,&g,error,sizeof error));
        for(size_t file=0;file<px_garc_count(g);++file) {
            const struct px_garc_member *member=px_garc_member_at(g,file);uint8_t *raw=NULL;size_t size=0;
            CHECK(member&&px_garc_read(g,member->entry,member->subentry,1u<<24,&raw,&size,error,sizeof error));
            if(size==0) { ++empty;free(raw);continue; }
            struct px_gf_text text;
            if(px_gf_text_decode(raw,size,&text)!=PX_GF_TEXT_OK) {
                fprintf(stderr,"unsupported text member %s:%u:%u size=%zu\n",archives[a],member->entry,member->subentry,size);return 1;
            }
            const uint32_t section=u32(raw+12);++files;lines+=text.line_count;
            for(unsigned mode=0;mode<3;++mode) {
                struct context c;initialize(&c,raw,size,mode);
                const struct px_pokemon_api memory=px_guest_pokemon_api(&c.guest);
                c.api=(struct px_message_api){&memory,&c,seek_cb,read_cb,alignment_cb,allocate_cb,release_cb};
                for(uint32_t index=0;index<text.line_count;++index) {
                    const uint32_t n=text.length[index];const uint16_t *expected=text.units+text.first[index];
                    CHECK(n<65535);
                    const uint32_t metadata=u16(raw+section+4+index*8+6);
                    CHECK(px_load_message(RESOURCE,index,STRING,&c.api)==metadata);
                    for(uint32_t i=0;i<n;++i)CHECK(px_guest_load16(&c.guest,DEST+2*i)==expected[i]);
                    uint32_t length=0;
                    while(length<n&&expected[length]&&length<2048) {
                        if(expected[length]==0x10) {
                            CHECK(length+1<n);length+=(uint32_t)expected[length+1]+2;
                        } else ++length;
                    }
                    CHECK(px_guest_load16(&c.guest,STRING+10)==length);
                    CHECK(c.guest.fault==PX_FAULT_NONE);++cases;units+=n;
                    if(n>longest)longest=n;
                }
                if(a==0&&member->entry==80&&member->subentry==0) {
                    const struct px_name_api names={&memory,&c,name_load};
                    for(uint32_t species=1;species<=721;++species) {
                        const uint16_t *expected=text.units+text.first[species];
                        const uint32_t metadata=u16(raw+section+4+species*8+6);
                        memset(px_guest_span(&c.guest,NAME_DEST,BUFFER_BYTES),0xa5,BUFFER_BYTES);
                        CHECK(px_build_pokemon_name(NAME_DEST,species,&names)==metadata);
                        uint32_t i=0;
                        do { CHECK(i<text.length[species]);CHECK(px_guest_load16(&c.guest,NAME_DEST+2*i)==expected[i]); } while(expected[i++]!=0);
                        CHECK(px_guest_load16(&c.guest,NAME_DEST+2*i)==0xa5a5);++name_cases;
                    }
                }
                CHECK(c.guest.fault==PX_FAULT_NONE);io_reads+=c.reads;allocations+=c.allocations;px_guest_free(&c.guest);
            }
            px_gf_text_free(&text);free(raw);
        }
        px_garc_close(g);fprintf(stderr,"checked %s\n",archives[a]);
    }
    CHECK(name_cases==2163);
    printf("{\"text_files\":%" PRIu64 ",\"empty_members\":%" PRIu64 ",\"text_lines\":%" PRIu64
        ",\"load_cases\":%" PRIu64 ",\"decoded_units_compared\":%" PRIu64 ",\"longest_line_units\":%u"
        ",\"name_builder_cases\":%" PRIu64 ",\"archive_reads\":%" PRIu64 ",\"allocations\":%" PRIu64
        ",\"all_match\":true}\n",files,empty,lines,cases,units,longest,name_cases,io_reads,allocations);
    return 0;
}
