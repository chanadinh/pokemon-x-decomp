#include "message_resources.h"
#include "../pokemon_names/pokemon_names.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c); exit(1); } } while (0)
enum { SIZE=0x600000, RESOURCE=0x100, STRING=0x200, HEAP=0x300,
       SECTION=0x4000, SECTION2=0x5000, SECTION3=0x6000, SOURCE=0x10000,
       TEMP=0x40000, DEST=0x80000, DEST2=0xb0000, FILE_BASE=0x100000,
       LANG=0x5d113c, NAME_GLOBAL=0x5d1790, NAME_DEST=0xe0000 };
struct event { char kind; uint32_t a,b,c,d; };
struct fixture {
    uint8_t *bytes; struct px_pokemon_api memory; struct px_message_api api;
    struct event events[32]; unsigned event_count,seeks,reads,aligns,allocs,frees;
    unsigned fail,mutation,language_reads,source_reads,track_source;
    uint32_t position;
};
static uint8_t r8(void *u,uint32_t a) { struct fixture *f=u;CHECK(a<SIZE);return f->bytes[a]; }
static uint16_t r16(void *u,uint32_t a)
{ struct fixture *f=u;if (f->track_source && a>=SOURCE && a<SOURCE+0x20000) ++f->source_reads;return (uint16_t)((uint32_t)r8(u,a)|(uint32_t)r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a)
{ struct fixture *f=u;if(a==LANG)++f->language_reads;return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { struct fixture *f=u;CHECK(a<SIZE);f->bytes[a]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static void event(struct fixture *f,char kind,uint32_t a,uint32_t b,uint32_t c,uint32_t d)
{ CHECK(f->event_count<32);f->events[f->event_count++]=(struct event){kind,a,b,c,d}; }
static uint16_t key_at(uint32_t index,unsigned unit)
{
    const uint32_t k=((index+3u)*0x2983u)&65535u,shift=(unit*3u)%16u;
    return (uint16_t)((k<<shift)|(k>>((16u-shift)%16u)));
}
static uint32_t section_offset(unsigned language) { return 0x1000u+language*0x100u; }
static void seek_cb(void *u,uint32_t archive,uint32_t member,uint32_t offset)
{
    struct fixture *f=u;event(f,'S',archive,member,offset,0);++f->seeks;f->position=offset;
    if (f->mutation==1) {
        w32(f,RESOURCE+28,f->seeks==1?SECTION2:SECTION3);
        w32(f,RESOURCE+8,0xa1);w32(f,RESOURCE+16,0xb1);
        if (f->seeks==2) w32(f,SECTION3,r32(f,FILE_BASE+offset));
    }
    if (f->mutation==2 && f->seeks==1) w32(f,RESOURCE+8,0xa1);
}
static void read_cb(void *u,uint32_t archive,uint32_t size,uint32_t destination)
{
    struct fixture *f=u;event(f,'R',archive,size,destination,0);++f->reads;
    CHECK(FILE_BASE+f->position+size<SIZE && destination+size<SIZE);
    memmove(f->bytes+destination,f->bytes+FILE_BASE+f->position,size);f->position+=size;
    if (f->mutation==2 && f->reads==1) w32(f,STRING+4,DEST2);
    if (f->mutation==4 && f->reads==2) { w16(f,RESOURCE+36,2);w16(f,RESOURCE+38,0x4321); }
    if (f->mutation==6 && f->reads==1) { w8(f,RESOURCE+12,1);w32(f,RESOURCE+28,FILE_BASE+section_offset(0)); }
}
static uint32_t align_cb(void *u,uint32_t heap)
{ struct fixture *f=u;event(f,'A',heap,0,0,0);++f->aligns;if(f->mutation==3)w32(f,RESOURCE+4,HEAP+16);return 16; }
static uint32_t allocate_cb(void *u,uint32_t heap,uint32_t size,uint32_t alignment,uint32_t tail)
{
    struct fixture *f=u;event(f,'M',heap,size,alignment,tail);++f->allocs;
    if(f->mutation==3)w32(f,RESOURCE+20,1);
    return f->fail?0:TEMP;
}
static void release_cb(void *u,uint32_t buffer)
{
    struct fixture *f=u;event(f,'F',buffer,0,0,0);++f->frees;CHECK(buffer==TEMP);
    memset(f->bytes+TEMP,0xdd,32);if(f->mutation==5)w16(f,RESOURCE+38,0xcafe);
}

static unsigned line_units(unsigned index,uint32_t language,uint16_t out[8])
{
    static const uint16_t lines[4][8]={{'L',0,0},{0x10,2,0xff00,0,0x2640,0},{0,'Z',0},{0xd83d,0xde00,0xe08d,0}};
    static const unsigned lengths[4]={3,6,3,4};
    memcpy(out,lines[index],sizeof lines[index]);if(index==0)out[1]=(uint16_t)('0'+language%10);
    return lengths[index];
}
static void reset(struct fixture *f,unsigned cached,unsigned archive,unsigned plaintext)
{
    memset(f->bytes,0xa5,SIZE);f->event_count=f->seeks=f->reads=f->aligns=f->allocs=f->frees=0;
    f->fail=f->mutation=f->language_reads=f->source_reads=f->track_source=0;
    f->memory=(struct px_pokemon_api){.user=f,.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
    f->api=(struct px_message_api){&f->memory,f,seek_cb,read_cb,align_cb,allocate_cb,release_cb};
    w32(f,RESOURCE+4,HEAP);w32(f,RESOURCE+8,archive);w8(f,RESOURCE+12,(uint8_t)cached);
    w32(f,RESOURCE+16,0xb0);w32(f,RESOURCE+20,UINT32_MAX);
    w32(f,RESOURCE+24,FILE_BASE);w32(f,RESOURCE+28,SECTION);
    w32(f,STRING+4,DEST);w16(f,STRING+8,128);w16(f,STRING+10,3);w8(f,STRING+12,1);
    w16(f,DEST,'O');w16(f,DEST+2,'l');w16(f,DEST+4,'d');w16(f,DEST+6,0);
    w32(f,LANG,0);w16(f,FILE_BASE,2);w16(f,FILE_BASE+2,4);w32(f,FILE_BASE+8,plaintext);
    const unsigned languages[]={0,1,2,3,255};
    for(unsigned k=0;k<5;++k) {
        const unsigned language=languages[k];const uint32_t offset=section_offset(language);
        w32(f,FILE_BASE+12+language*4,offset);uint32_t payload=36;
        for(unsigned index=0;index<4;++index) {
            uint16_t units[8];const unsigned n=line_units(index,language,units);
            const uint32_t entry=FILE_BASE+offset+4+index*8;
            w32(f,entry,payload);w16(f,entry+4,(uint16_t)n);w16(f,entry+6,(uint16_t)(0x9000+language*4+index));
            for(unsigned i=0;i<n;++i)w16(f,FILE_BASE+offset+payload+2*i,
                (uint16_t)(units[i]^(plaintext?0:key_at(index,i))));
            payload+=n*2;
        }
        w32(f,FILE_BASE+offset,payload);
    }
}
static void decoding(struct fixture *f)
{
    unsigned cases=0;
    for(unsigned plain=0;plain<2;++plain) {
        reset(f,1,0,plain);w16(f,STRING+8,3);
        for(unsigned value=0;value<=65535;++value) {
            const uint32_t index=value*0x1f123bb5u+5u;
            w16(f,SOURCE,(uint16_t)(value^(plain?0:key_at(index,0))));
            w16(f,SOURCE+2,plain?0:key_at(index,1));
            w16(f,DEST+6,0xa5a5);w16(f,STRING+10,123);
            px_decode_message(RESOURCE,SOURCE,2,index,STRING,&f->memory);
            CHECK(r16(f,DEST)==value && r16(f,DEST+2)==0 && r16(f,DEST+4)==0);
            CHECK(r16(f,STRING+10)==(value==0x10?2:(value?1:0)) && r16(f,DEST+6)==0xa5a5);++cases;
        }
        for(unsigned capacity=0;capacity<=8;++capacity)for(unsigned count=0;count<=8;++count)
            for(unsigned zero=0;zero<2;++zero) {
                w16(f,STRING+8,(uint16_t)capacity);w16(f,STRING+10,7);memset(f->bytes+DEST,0xa5,24);
                for(unsigned i=0;i<count;++i)w16(f,SOURCE+2*i,(uint16_t)((zero&&i+1==count?0:'A'+i)^(plain?0:key_at(7,i))));
                px_decode_message(RESOURCE,SOURCE,count,7,STRING,&f->memory);
                const unsigned ok=count<capacity || (count==capacity&&count&&zero);
                CHECK(r16(f,STRING+10)==(ok?count-(zero&&count):0));
                CHECK(r16(f,DEST)==(ok&&count&&!(count==1&&zero)?'A':0));
                CHECK(r16(f,DEST+2*(capacity?capacity:1))==0xa5a5);++cases;
            }
        w8(f,STRING+12,0);w16(f,STRING+10,77);w16(f,DEST,0x4567);f->track_source=1;f->source_reads=0;
        px_decode_message(RESOURCE,SOURCE,1,0,STRING,&f->memory);
        CHECK(f->source_reads==1 && r16(f,DEST)==0x4567 && r16(f,STRING+10)==77);
        f->source_reads=0;px_decode_message(RESOURCE,SOURCE,0,0,STRING,&f->memory);CHECK(f->source_reads==0);
        f->track_source=0;w8(f,STRING+12,0x80);w16(f,STRING+8,3);
        w16(f,SOURCE,plain?'X':(uint16_t)('X'^key_at(UINT32_MAX,0)));
        px_decode_message(RESOURCE,SOURCE,1,UINT32_MAX,STRING,&f->memory);CHECK(r16(f,DEST)=='X' && r16(f,STRING+10)==1);
    }
    printf("decode: %u unit/capacity cases, inactive reads, full-width keys and active byte 0x80\n",cases);
    reset(f,1,0,1);w16(f,STRING+8,65535);
    for(unsigned count=65534;count<=65536;++count)for(unsigned terminated=0;terminated<2;++terminated) {
        for(unsigned i=0;i<count;++i)w16(f,SOURCE+2*i,(uint16_t)(terminated&&i+1==count?0:'A'));
        memset(f->bytes+DEST,0xa5,131074);
        px_decode_message(RESOURCE,SOURCE,count,0,STRING,&f->memory);
        const unsigned fits=count<65535||(count==65535&&terminated);
        CHECK(r16(f,STRING+10)==(fits?2048:0));
        CHECK(r16(f,DEST)==(fits?'A':0) && r16(f,DEST+131070)==0xa5a5);
    }
    w8(f,STRING+12,0);f->track_source=1;f->source_reads=0;
    px_decode_message(RESOURCE,SOURCE,UINT32_MAX,0,STRING,&f->memory);CHECK(f->source_reads==1);
    reset(f,1,0,0x80000000u);w16(f,SOURCE,'P');
    px_decode_message(RESOURCE,SOURCE,1,0,STRING,&f->memory);CHECK(r16(f,DEST)=='P');
    puts("decode: six maximum-capacity cases; inactive UINT32_MAX count; nonzero plaintext mode word");
}

static void lengths(struct fixture *f)
{
    reset(f,1,0,1);
    w16(f,SOURCE,0x10);
    for(unsigned count=0;count<=65535;++count) {
        w16(f,SOURCE+2,(uint16_t)count);
        if(count+2<2048)w16(f,SOURCE+2*(count+2),0);
        CHECK(px_game_string_length(SOURCE,&f->memory)==count+2);
    }
    const unsigned prefixes[]={0,1,2046,2047};
    const unsigned counts[]={0,1,2,2044,2045,2046,65534,65535};
    for(unsigned p=0;p<4;++p)for(unsigned c=0;c<8;++c) {
        const unsigned prefix=prefixes[p],span=counts[c]+2;
        for(unsigned i=0;i<prefix;++i)w16(f,SOURCE+2*i,'A');
        w16(f,SOURCE+2*prefix,0x10);w16(f,SOURCE+2*(prefix+1),(uint16_t)counts[c]);
        if(prefix+span<2048)w16(f,SOURCE+2*(prefix+span),0);
        CHECK(px_game_string_length(SOURCE,&f->memory)==prefix+span);
    }
    for(unsigned n=0;n<=2050;++n) {
        for(unsigned i=0;i<n;++i)w16(f,SOURCE+2*i,'A');
        w16(f,SOURCE+2*n,0);
        CHECK(px_game_string_length(SOURCE,&f->memory)==(n<2048?n:2048));
    }
    const uint16_t adjacent[]={0x10,1,0,0x10,2,0,0x9999,0};
    for(unsigned i=0;i<8;++i)w16(f,SOURCE+2*i,adjacent[i]);
    CHECK(px_game_string_length(SOURCE,&f->memory)==7);
    /* Decode stores the low halfword of the helper's full-width result.
     * The guard is reached before the enormous declared token is traversed. */
    for(unsigned i=0;i<2047;++i)w16(f,SOURCE+2*i,'A');
    w16(f,SOURCE+4094,0x10);w16(f,SOURCE+4096,65535);
    w16(f,STRING+8,65535);
    px_decode_message(RESOURCE,SOURCE,2049,0,STRING,&f->memory);
    CHECK(px_game_string_length(DEST,&f->memory)==67584 && r16(f,STRING+10)==2048);
    puts("length: 65536 token counts, 32 guard/token boundaries, 2051 plain lengths, adjacent tokens and stored-length wrapping");
}

static void loading(struct fixture *f)
{
    unsigned cases=0;
    for(unsigned mode=0;mode<3;++mode)for(unsigned plain=0;plain<2;++plain)
        for(unsigned language=0;language<2;++language)for(unsigned index=0;index<4;++index) {
            reset(f,mode!=2,mode==0?0:0xa0,plain);w32(f,LANG,language);
            CHECK(px_load_message(RESOURCE,index,STRING,&f->api)==0x9000+language*4+index);
            uint16_t expected[8];const unsigned n=line_units(index,language,expected);
            for(unsigned i=0;i<n;++i)CHECK(r16(f,DEST+2*i)==expected[i]);
            const unsigned lengths[]={2,5,0,3};CHECK(r16(f,STRING+10)==lengths[index]);
            CHECK(f->reads==(mode==0?0:2) && f->seeks==(mode==0?0:2));
            CHECK(f->allocs==(mode==2) && f->frees==(mode==2));
            if(mode==2) {
                CHECK(f->events[0].kind=='S' && f->events[0].c==section_offset(language)+4+index*8);
                CHECK(f->events[2].kind=='A' && f->events[3].kind=='M');
                CHECK(f->events[3].a==HEAP && f->events[3].b==n*2 && f->events[3].c==0xfffffff0 && f->events[3].d==1);
                CHECK(f->events[6].kind=='F');
            }
            const unsigned old_reads=f->reads;
            CHECK(px_load_message(RESOURCE,index,STRING,&f->api)==0x9000+language*4+index);
            CHECK(f->reads==old_reads+(mode==2?2:0));++cases;
        }
    const uint32_t selectors[]={0,1,2,3,255,256,257,UINT32_MAX};
    const unsigned sections[]={0,1,2,3,255,256,257,65535};
    for(unsigned s=0;s<8;++s)for(unsigned n=0;n<8;++n) {
        reset(f,1,0,0);w16(f,FILE_BASE,(uint16_t)sections[n]);w32(f,LANG,selectors[s]);
        const uint32_t selected=(selectors[s]<sections[n]?selectors[s]:0)&255u;
        CHECK(px_load_message(RESOURCE,0,STRING,&f->api)==0x9000+selected*4);
        CHECK(r32(f,RESOURCE+20)==selected && r16(f,DEST+2)=='0'+selected%10);++cases;
    }
    for(unsigned active=0;active<2;++active)for(unsigned i=0;i<3;++i) {
        const uint32_t index=i==0?4:i==1?65536:UINT32_MAX;
        reset(f,0,0xa0,0);w8(f,STRING+12,(uint8_t)active);
        CHECK(px_load_message(RESOURCE,index,STRING,&f->api)==0);
        CHECK(f->event_count==0 && f->language_reads==0);
        CHECK(r16(f,DEST)==(active?0:'O') && r16(f,STRING+10)==(active?0:3));++cases;
    }
    reset(f,0,0xa0,0);f->fail=1;
    CHECK(px_load_message(RESOURCE,0,STRING,&f->api)==0x9000);
    CHECK(r16(f,DEST)=='O' && r16(f,STRING+10)==3 && f->allocs==1 && f->frees==0 && f->reads==1);
    reset(f,0,0xa0,0);w8(f,STRING+12,0);
    CHECK(px_load_message(RESOURCE,0,STRING,&f->api)==0x9000);
    CHECK(r16(f,DEST)=='O' && r16(f,STRING+10)==3 && f->allocs==1 && f->frees==1);
    reset(f,0,0xa0,0);w16(f,FILE_BASE+section_offset(0)+8,0);
    CHECK(px_load_message(RESOURCE,0,STRING,&f->api)==0x9000);
    CHECK(f->events[3].b==0 && f->events[5].b==0 && f->frees==1);
    CHECK(r16(f,DEST)==0 && r16(f,STRING+10)==0);
    reset(f,1,0,0);w16(f,FILE_BASE+2,0);
    CHECK(px_load_message(RESOURCE,0,STRING,&f->api)==0 && f->language_reads==0);
    printf("loader: %u cached/streamed, language-width and invalid-index cases; reuse and allocation failure\n",cases);
}

static void mutations(struct fixture *f)
{
    reset(f,1,0xa0,0);f->mutation=1;
    CHECK(px_load_message(RESOURCE,0,STRING,&f->api)==0x9000 && r16(f,DEST)=='L');
    CHECK(f->events[0].a==0xa0 && f->events[2].a==0xa0 && f->events[2].b==0xb0);
    CHECK(f->events[1].a==0xa0 && f->events[1].c==SECTION2);
    CHECK(f->events[3].a==0xa0 && f->events[3].c==SECTION3 && f->events[3].b==68);
    reset(f,0,0xa0,0);f->mutation=2;
    CHECK(px_load_message(RESOURCE,0,STRING,&f->api)==0x9000 && r16(f,DEST2)=='L');
    CHECK(f->events[1].a==0xa1 && r16(f,DEST)=='O');
    reset(f,0,0xa0,0);f->mutation=3;
    CHECK(px_load_message(RESOURCE,0,STRING,&f->api)==0x9000 && r16(f,DEST+2)=='1');
    CHECK(f->events[3].a==HEAP && r32(f,RESOURCE+4)==HEAP+16);
    CHECK(f->events[4].c==section_offset(1)+36);
    reset(f,0,0xa0,0);f->mutation=4;
    CHECK(px_load_message(RESOURCE,0,STRING,&f->api)==0x4321 && r16(f,STRING+10)==2);
    CHECK(f->events[3].b==6 && f->events[5].b==6);
    reset(f,0,0xa0,0);f->mutation=5;
    CHECK(px_load_message(RESOURCE,0,STRING,&f->api)==0xcafe && f->frees==1);
    reset(f,0,0xa0,0);f->mutation=6;
    CHECK(px_load_message(RESOURCE,0,STRING,&f->api)==0x9000 && r16(f,DEST)=='L');
    CHECK(f->seeks==1 && f->reads==1 && f->allocs==0 && f->frees==0);
    puts("mutation: cached buffers, captured archive/heap, reloaded handles/language/entry and post-free metadata");
}

static uint32_t name_load(void *u,uint32_t resource,uint32_t index,uint32_t string)
{ struct fixture *f=u;return px_load_message(resource,index,string,&f->api); }
static void names(struct fixture *f)
{
    for(unsigned mode=0;mode<3;++mode) {
        reset(f,mode!=2,mode==0?0:0xa0,0);w32(f,NAME_GLOBAL,STRING);w32(f,NAME_GLOBAL+4,RESOURCE);
        const struct px_name_api api={&f->memory,f,name_load};
        CHECK(px_build_pokemon_name(NAME_DEST,0,&api)==0x9000);
        CHECK(r16(f,NAME_DEST)=='L' && r16(f,NAME_DEST+2)=='0' && r16(f,NAME_DEST+4)==0);
        CHECK(px_build_pokemon_name(NAME_DEST,1,&api)==0x9001);
        uint16_t expected[8];const unsigned n=line_units(1,0,expected);
        for(unsigned i=0;i<n;++i)CHECK(r16(f,NAME_DEST+2*i)==expected[i]);
        if(mode==2) { CHECK(f->frees==2);f->fail=1; }
        CHECK(px_build_pokemon_name(NAME_DEST,724,&api)==0 && r16(f,NAME_DEST)==0x10);
    }
    puts("integration: reconstructed name builder and control-token copy use the message service in all three modes");
}
int main(void)
{
    struct fixture f={0};f.bytes=malloc(SIZE);CHECK(f.bytes);
    decoding(&f);lengths(&f);loading(&f);mutations(&f);names(&f);free(f.bytes);
    puts("message_resources: all tests passed");return 0;
}
