/* Replay calls captured from the running game through reconstructed routines.
 * Input lines (written by scripts/replay_live_calls.py):
 *   P species form record_hex12 table expected_record_hex12 expected_payload_hex64
 *   F selector payload_hex64 result
 *   G species form growth_record_hex412
 *   L species form growth result
 *   H species level base iv ev result
 *   S stat level base iv ev nature nature_row_hex5 result
 *   M move selector result
 *   X move pp_ups result
 * The personal table and growth curves come from the original archives; the
 * running-game state check shows they equal the game's loaded copies. Only
 * reconstructed routines compute the replayed results. */
#include "../reconstructed/pokemon_personal/pokemon_personal.h"
#include "../reconstructed/pokemon_growth/pokemon_growth.h"
#include "../reconstructed/pokemon_data/pokemon_data.h"
#include "../reconstructed/pokemon_stats/stats_math.h"
#include "../reconstructed/pokemon_moves/pokemon_moves.h"
#include "../reconstructed/pokemon_move_resource/pokemon_move_resource.h"
#include "../native/assets/garc.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"replay:%d: %s\n",__LINE__,#c);exit(2); } } while (0)
enum { SIZE=0x600000, LOCAL_TABLE=0x10000, RECORD=0x100, PAYLOAD=0x200, DESCRIPTOR=0x300, DESC_PAYLOAD=0x340,
       GROWTH=0x400, CACHE=0x600, SLOT=0x700, ARCHIVE_HANDLE=7, MOVE_HANDLE=9, MOVE_RECORD=0x800,
       MOVE_PAYLOAD=0x840, MOVE_CACHE=0x880, MOVE_SLOT=0x8c0, MOVE_SLOT_PAYLOAD=0x900, MOVES=0x100000 };
static uint8_t *memory;
static uint32_t table_base,table_bytes;
static uint8_t curves[8][404];
static uint32_t move_count;
static uint32_t map(uint32_t a)
{
    if(table_base&&a>=table_base&&a-table_base<table_bytes)return LOCAL_TABLE+(a-table_base);
    CHECK(a<SIZE);return a;
}
static uint8_t r8(void *u,uint32_t a) { (void)u;return memory[map(a)]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)(r8(u,a)|r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static void w8(void *u,uint32_t a,uint8_t v) { (void)u;memory[map(a)]=v; }
static void w16(void *u,uint32_t a,uint16_t v) { w8(u,a,(uint8_t)v);w8(u,a+1,(uint8_t)(v>>8)); }
static void w32(void *u,uint32_t a,uint32_t v) { w16(u,a,(uint16_t)v);w16(u,a+2,(uint16_t)(v>>16)); }
static struct px_pokemon_api api={.load8=r8,.load16=r16,.load32=r32,.store8=w8,.store16=w16,.store32=w32};
static void unhex(const char *s,uint8_t *out,size_t n) { for(size_t i=0;i<n;++i) { unsigned v;CHECK(sscanf(s+2*i,"%2x",&v)==1);out[i]=(uint8_t)v; } }
static uint8_t *member(const char *path,size_t index,size_t *size)
{
    struct px_garc *g=NULL;char error[256];CHECK(px_garc_open(path,&g,error,sizeof error));
    if(index==(size_t)-1)index=px_garc_count(g)-1;
    const struct px_garc_member *m=px_garc_member_at(g,index);uint8_t *b=NULL;
    CHECK(m&&px_garc_read(g,m->entry,m->subentry,1u<<24,&b,size,error,sizeof error));
    px_garc_close(g);return b;
}
/* Archive member read for growth curves (a/2/1/7). */
static void read_curve(void *u,uint32_t archive,uint32_t index,uint32_t destination)
{ (void)u;CHECK(archive==ARCHIVE_HANDLE&&index<8);for(uint32_t i=0;i<404;++i)w8(NULL,destination+i,curves[index][i]); }
/* A fresh one-slot growth cache and descriptor around the captured table. */
static void growth_state(void)
{
    w32(NULL,0x5d177c,table_base);w32(NULL,0x5d17a8,DESCRIPTOR);w32(NULL,DESCRIPTOR+8,DESC_PAYLOAD);
    w32(NULL,0x5d17ac,GROWTH);w32(NULL,GROWTH+4,0xffffffffu);w32(NULL,0x5d17b0,CACHE);
    w32(NULL,CACHE+4,CACHE+0x10);w8(NULL,CACHE+8,1);w8(NULL,CACHE+9,0);w32(NULL,CACHE+0x10,SLOT);w32(NULL,SLOT+4,0xfffffff0u);
    w32(NULL,0x5e52b0,ARCHIVE_HANDLE);
}
static const struct px_growth_api growth_api={&api,NULL,read_curve};
/* Move archive member read (a/2/1/2) and a fresh one-slot move cache. */
static void read_move(void *u,uint32_t archive,uint32_t index,uint32_t destination)
{ (void)u;CHECK(archive==MOVE_HANDLE&&index<move_count);for(uint32_t i=0;i<36;++i)w8(NULL,destination+i,memory[MOVES+36*index+i]); }
static const struct px_move_resource_api move_resource_api={&api,NULL,read_move};
static void move_state(void)
{
    w32(NULL,0x5e5338,MOVE_HANDLE);w32(NULL,0x5d188c,MOVE_RECORD);w32(NULL,MOVE_RECORD+8,MOVE_PAYLOAD);w16(NULL,MOVE_RECORD+4,0xffff);
    w32(NULL,0x5d1890,MOVE_CACHE);w32(NULL,MOVE_CACHE+4,MOVE_CACHE+0x10);w8(NULL,MOVE_CACHE+8,1);w8(NULL,MOVE_CACHE+9,0);
    w32(NULL,MOVE_CACHE+0x10,MOVE_SLOT);w32(NULL,MOVE_SLOT+8,MOVE_SLOT_PAYLOAD);w16(NULL,MOVE_SLOT+4,0xffff);
}
static uint32_t move_value(void *u,uint32_t move,uint32_t selector) { (void)u;move_state();return px_get_move_resource_value(move,selector,&move_resource_api); }
static void select_growth(void *u,uint32_t species,uint32_t form) { (void)u;px_select_growth_resource(species,form,&growth_api); }
int main(int argc,char **argv)
{
    if(argc!=4) { fprintf(stderr,"usage: replay_live_calls ROMFS TABLE_ADDRESS CALLS.txt\n");return 2; }
    memory=calloc(1,SIZE);CHECK(memory);char path[4096];size_t size;
    CHECK(snprintf(path,sizeof path,"%s/a/2/1/8",argv[1])<(int)sizeof path);
    uint8_t *flat=member(path,(size_t)-1,&size);CHECK(size%64==0&&LOCAL_TABLE+size<0x80000);
    memcpy(memory+LOCAL_TABLE,flat,size);free(flat);table_bytes=(uint32_t)size;
    /* The game's table address, or (0 when no record loads were captured) the local copy. */
    table_base=(uint32_t)strtoul(argv[2],NULL,0);
    if(table_base==0)table_base=LOCAL_TABLE;
    CHECK(snprintf(path,sizeof path,"%s/a/2/1/7",argv[1])<(int)sizeof path);
    for(size_t r=0;r<8;++r) { uint8_t *c=member(path,r,&size);CHECK(size==404);memcpy(curves[r],c,404);free(c); }
    CHECK(snprintf(path,sizeof path,"%s/a/2/1/2",argv[1])<(int)sizeof path);
    { struct px_garc *g=NULL;char error[256];CHECK(px_garc_open(path,&g,error,sizeof error));move_count=(uint32_t)px_garc_count(g);px_garc_close(g); }
    CHECK(MOVES+36*move_count<SIZE);
    for(uint32_t i=0;i<move_count;++i) { uint8_t *m=member(path,i,&size);CHECK(size>=36);memcpy(memory+MOVES+36*i,m,36);free(m); }
    FILE *in=fopen(argv[3],"r");CHECK(in);
    static char line[4096];uint64_t done[8]={0},bad[8]={0};
    while(fgets(line,sizeof line,in)) {
        if(line[0]=='P') {
            uint32_t species,form,table;char rec[32],exp_rec[32],exp_payload[160];
            CHECK(sscanf(line+2,"%" SCNu32 " %" SCNu32 " %24s %" SCNu32 " %24s %128s",&species,&form,rec,&table,exp_rec,exp_payload)==6);
            CHECK(table==table_base);
            uint8_t r[12],er[12],ep[64];unhex(rec,r,12);unhex(exp_rec,er,12);unhex(exp_payload,ep,64);
            w32(NULL,0x5d177c,table_base);memcpy(memory+RECORD,r,12);w32(NULL,RECORD+8,PAYLOAD);memset(memory+PAYLOAD,0xee,64);
            px_load_personal_record(RECORD,species,form,&api);
            uint8_t got[12];memcpy(got,memory+RECORD,12);memcpy(got+8,er+8,4); /* payload pointer stays the game's */
            const int ok=!memcmp(got,er,12)&&!memcmp(memory+PAYLOAD,ep,64);++done[0];bad[0]+=!ok;
            if(!ok)fprintf(stderr,"personal record %u/%u differs\n",species,form);
        } else if(line[0]=='F') {
            uint32_t selector,result;char payload[160];
            CHECK(sscanf(line+2,"%" SCNu32 " %128s %" SCNu32,&selector,payload,&result)==3);
            uint8_t p[64];unhex(payload,p,64);memcpy(memory+PAYLOAD,p,64);w32(NULL,RECORD+8,PAYLOAD);
            const uint32_t got=px_personal_resource_field(RECORD,selector,&api);++done[1];
            if(got!=result) { ++bad[1];fprintf(stderr,"field %u: %u vs game %u\n",selector,got,result); }
        } else if(line[0]=='G') {
            uint32_t species,form;static char grow[1024];uint8_t g[0x19c];
            CHECK(sscanf(line+2,"%" SCNu32 " %" SCNu32 " %830s",&species,&form,grow)==3);unhex(grow,g,0x19c);
            growth_state();select_growth(NULL,species,form);
            const int ok=!memcmp(g+4,memory+GROWTH+4,0x198);++done[2];bad[2]+=!ok;
            if(!ok)fprintf(stderr,"growth %u/%u differs\n",species,form);
        } else if(line[0]=='H') {
            uint32_t species,level,base,iv,ev,result;
            CHECK(sscanf(line+2,"%" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32,&species,&level,&base,&iv,&ev,&result)==6);
            const uint32_t got=px_pokemon_hp_value(species,level,base,iv,ev);++done[4];
            if(got!=result) { ++bad[4];fprintf(stderr,"hp %u/%u/%u/%u/%u: %u vs game %u\n",species,level,base,iv,ev,got,result); }
        } else if(line[0]=='S') {
            uint32_t stat,level,base,iv,ev,nature,result;char row[16];
            CHECK(sscanf(line+2,"%" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %15s %" SCNu32,&stat,&level,&base,&iv,&ev,&nature,row,&result)==8);
            if(row[0]!='-') { uint8_t r[5];unhex(row,r,5);for(uint32_t i=0;i<5;++i)w8(NULL,0x00570f6c+5*nature+i,r[i]); }
            const uint32_t got=px_pokemon_stat_value(level,base,iv,ev,nature,(enum px_stat)stat,&api);++done[5];
            if(got!=result) { ++bad[5];fprintf(stderr,"stat %u level %u base %u iv %u ev %u nature %u: %u vs game %u\n",stat,level,base,iv,ev,nature,got,result); }
        } else if(line[0]=='M') {
            uint32_t move,selector,result;CHECK(sscanf(line+2,"%" SCNu32 " %" SCNu32 " %" SCNu32,&move,&selector,&result)==3);
            const uint32_t got=move_value(NULL,move,selector);++done[6];
            if(got!=result) { ++bad[6];fprintf(stderr,"move %u selector %u: %u vs game %u\n",move,selector,got,result); }
        } else if(line[0]=='X') {
            uint32_t move,ups,result;CHECK(sscanf(line+2,"%" SCNu32 " %" SCNu32 " %" SCNu32,&move,&ups,&result)==3);
            const struct px_move_api moves={&api,NULL,move_value};
            const uint32_t got=px_pokemon_maximum_pp(move,ups,&moves);++done[7];
            if(got!=result) { ++bad[7];fprintf(stderr,"max pp move %u ups %u: %u vs game %u\n",move,ups,got,result); }
        } else if(line[0]=='L') {
            uint32_t species,form,growth,result;
            CHECK(sscanf(line+2,"%" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32,&species,&form,&growth,&result)==4);
            growth_state();const struct px_data_api data={&api,NULL,select_growth};
            const uint32_t got=px_pokemon_level_from_growth(species,form,growth,&data);++done[3];
            if(got!=result) { ++bad[3];fprintf(stderr,"level %u/%u growth %u: %u vs game %u\n",species,form,growth,got,result); }
        }
    }
    fclose(in);free(memory);
    static const char *names[8]={"load_personal_record","personal_field","select_growth","level_from_growth",
                                 "hp_value","stat_value","move_value","maximum_pp"};
    printf("{");
    for(unsigned i=0;i<8;++i)printf("%s\"%s\":[%" PRIu64 ",%" PRIu64 "]",i?",":"",names[i],done[i],done[i]-bad[i]);
    printf("}\n");
    for(unsigned i=4;i<8;++i)if(bad[i])return 1;
    return (bad[0]||bad[1]||bad[2]||bad[3])?1:0;
}
