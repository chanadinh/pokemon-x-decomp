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
 *   U kind mode object manager data extra entry_hex x4 exit_hex x4 nature_table_hex
 *   N species form record_hex12 payload_hex104
 * The personal table and growth curves come from the original archives; the
 * running-game state check shows they equal the game's loaded copies. Only
 * reconstructed routines compute the replayed results. */
#include "../reconstructed/pokemon_personal/pokemon_personal.h"
#include "../reconstructed/pokemon_growth/pokemon_growth.h"
#include "../reconstructed/pokemon_data/pokemon_data.h"
#include "../reconstructed/pokemon_stats/stats_math.h"
#include "../reconstructed/pokemon_moves/pokemon_moves.h"
#include "../reconstructed/pokemon_move_resource/pokemon_move_resource.h"
#include "../reconstructed/pokemon_stats/pokemon_stats.h"
#include "../reconstructed/pokemon_learnset/pokemon_learnset.h"
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
/* Captured heap regions (object, manager, data, auxiliary) keep their guest
 * addresses and are backed by slots in the high part of local memory. */
enum { REGION_BASE=0x500000, REGION_SLOT=0x400, REGIONS=4, LEARNSETS=0x200000 };
static uint32_t region_base[REGIONS],region_size[REGIONS];
static uint32_t map(uint32_t a)
{
    for(unsigned i=0;i<REGIONS;++i)
        if(region_size[i]&&a>=region_base[i]&&a-region_base[i]<region_size[i])return REGION_BASE+REGION_SLOT*i+(a-region_base[i]);
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
/* Personal services for the stat orchestrators (00167e2c / 00167e18). */
static void select_species(void *u,uint32_t species,uint32_t form) { (void)u;px_select_personal_resource(species,form,&api); }
static uint32_t personal_value(void *u,uint32_t selector) { (void)u;return px_current_personal_value(selector,&api); }
static const struct px_parameter_api parameters={.select_species=select_species,.personal_value=personal_value};
/* Learnset services: reconstructed personal record/field and members of a/2/1/4. */
enum { LEARNSET_HANDLE=11, LEARN_RECORD=0xa00, LEARN_BUFFER=0xa40, LEARN_CACHE=0xb00, LEARN_SLOT=0xb40,
       LEARN_SLOT_BUFFER=0xb80, LEARN_MEMBER=128 };
static uint32_t learnset_count;
static void learn_select(void *u,uint32_t record,uint32_t species,uint32_t form) { (void)u;px_load_personal_record(record,species,form,&api); }
static uint32_t learn_value(void *u,uint32_t record,uint32_t selector) { (void)u;return px_personal_resource_field(record,selector,&api); }
static void learn_read(void *u,uint32_t archive,uint32_t index,uint32_t destination)
{
    (void)u;CHECK(archive==LEARNSET_HANDLE&&index<learnset_count);
    const uint8_t *m=memory+LEARNSETS+LEARN_MEMBER*index;const uint32_t size=m[LEARN_MEMBER-1];
    for(uint32_t i=0;i<size;++i)w8(NULL,destination+i,m[i]);
}
static const struct px_learnset_api learnset_api={&api,NULL,learn_select,learn_value,learn_read};
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
    CHECK(snprintf(path,sizeof path,"%s/a/2/1/4",argv[1])<(int)sizeof path);
    { struct px_garc *g=NULL;char error[256];CHECK(px_garc_open(path,&g,error,sizeof error));learnset_count=(uint32_t)px_garc_count(g);px_garc_close(g); }
    CHECK(LEARNSETS+LEARN_MEMBER*learnset_count<MOVES+0x300000&&LEARNSETS+LEARN_MEMBER*learnset_count<REGION_BASE);
    for(uint32_t i=0;i<learnset_count;++i) {
        uint8_t *m=member(path,i,&size);CHECK(size<LEARN_MEMBER);
        memcpy(memory+LEARNSETS+LEARN_MEMBER*i,m,size);memory[LEARNSETS+LEARN_MEMBER*i+LEARN_MEMBER-1]=(uint8_t)size;free(m);
    }
    api.parameters=&parameters;
    FILE *in=fopen(argv[3],"r");CHECK(in);
    static char line[8192];uint64_t done[10]={0},bad[10]={0};
    while(fgets(line,sizeof line,in)) {
        if(line[0]=='R') {
            /* Read-only executable tables (block permutation, natures) at their addresses. */
            uint32_t address;static char hex[1024];CHECK(sscanf(line+2,"%" SCNu32 " %1023s",&address,hex)==2);
            const size_t n=strlen(hex)/2;uint8_t bytes[512];CHECK(n<=sizeof bytes);unhex(hex,bytes,n);
            for(size_t i=0;i<n;++i)w8(NULL,address+(uint32_t)i,bytes[i]);
        } else if(line[0]=='P') {
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
        } else if(line[0]=='U') {
            /* Object stat/HP update: replay on the entry snapshot, compare every exit byte. */
            uint32_t kind,mode,addr[4];static char in[4][512],out[4][512],natures[260];
            CHECK(sscanf(line+2,"%" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32
                         " %511s %511s %511s %511s %511s %511s %511s %511s %259s",&kind,&mode,&addr[0],&addr[1],&addr[2],&addr[3],
                         in[0],in[1],in[2],in[3],out[0],out[1],out[2],out[3],natures)==15);
            static const uint32_t sizes[4]={0x10,0x10,0xe8,0x1c};
            for(unsigned i=0;i<4;++i) {
                region_base[i]=addr[i];region_size[i]=in[i][0]=='-'||addr[i]==0?0:sizes[i];
                if(region_size[i])unhex(in[i],memory+REGION_BASE+REGION_SLOT*i,sizes[i]);
            }
            uint8_t nature_table[125];unhex(natures,nature_table,125);
            for(uint32_t i=0;i<125;++i)w8(NULL,0x00570f6c+i,nature_table[i]);
            growth_state();w32(NULL,0x5d17a4,RECORD);w32(NULL,RECORD+8,PAYLOAD);w16(NULL,RECORD+4,0xffff);
            const struct px_data_api data={&api,NULL,select_growth};
            if(getenv("REPLAY_DIFF")) {
                const uint32_t manager=addr[1];
                fprintf(stderr,"object %08x level %u species %u form %u nature %u",addr[0],px_pokemon_object_level(addr[0],&data),
                        px_read_pokemon_state_value(manager,PX_STATE_SPECIES,&api),px_read_pokemon_state_value(manager,PX_STATE_FORM,&api),
                        px_read_pokemon_nature(manager,&api));
                for(unsigned st=0;st<6;++st)fprintf(stderr," [%u:iv %u ev %u]",st,px_read_pokemon_iv(manager,(enum px_stat)st,&api),px_read_pokemon_ev(manager,(enum px_stat)st,&api));
                fprintf(stderr,"\n");
                for(unsigned i=0;i<4;++i)if(region_size[i])unhex(in[i],memory+REGION_BASE+REGION_SLOT*i,sizes[i]);
                growth_state();w32(NULL,0x5d17a4,RECORD);w32(NULL,RECORD+8,PAYLOAD);w16(NULL,RECORD+4,0xffff);
            }
            if(kind==5)px_update_pokemon_hp(addr[0],mode,&data);
            else px_update_pokemon_stat(addr[0],(enum px_state_stat)kind,&data);
            int ok=1;
            for(unsigned i=0;i<4;++i)if(region_size[i]) {
                uint8_t expected[0xe8];unhex(out[i],expected,sizes[i]);
                const uint8_t *got=memory+REGION_BASE+REGION_SLOT*i;
                if(memcmp(got,expected,sizes[i])) {
                    ok=0;
                    if(getenv("REPLAY_DIFF"))
                        for(uint32_t k=0;k<sizes[i];++k)if(got[k]!=expected[k])
                            fprintf(stderr,"  region %u +%02x: replay %02x game %02x\n",i,k,got[k],expected[k]);
                }
            }
            ++done[8];bad[8]+=!ok;
            if(!ok)fprintf(stderr,"update %u (mode %u) on object %08x differs\n",kind,mode,addr[0]);
            memset(region_size,0,sizeof region_size);
        } else if(line[0]=='N') {
            uint32_t species,form;char rec[32];static char payload[260];
            CHECK(sscanf(line+2,"%" SCNu32 " %" SCNu32 " %24s %259s",&species,&form,rec,payload)==4);
            uint8_t er[12],ep[104];unhex(rec,er,12);unhex(payload,ep,104);
            /* Fresh one-slot learnset state; members served from a/2/1/4. */
            w32(NULL,0x5d177c,table_base);w32(NULL,0x5d17e8,DESCRIPTOR);w32(NULL,DESCRIPTOR+8,DESC_PAYLOAD);
            w32(NULL,0x5d17ec,LEARNSET_HANDLE);w32(NULL,0x5d17b4,LEARN_RECORD);w16(NULL,LEARN_RECORD,0xffff);
            w32(NULL,LEARN_RECORD+4,LEARN_BUFFER);w32(NULL,0x5d17b8,LEARN_CACHE);w32(NULL,LEARN_CACHE+4,LEARN_CACHE+0x10);
            w32(NULL,LEARN_CACHE+8,1);w32(NULL,LEARN_CACHE+0x0c,0);w32(NULL,LEARN_CACHE+0x10,LEARN_SLOT);
            w16(NULL,LEARN_SLOT,0xffff);w32(NULL,LEARN_SLOT+4,LEARN_SLOT_BUFFER);
            px_select_pokemon_learnset(species,form,&learnset_api);
            const uint32_t count=r8(NULL,LEARN_RECORD+8);
            const int ok=r16(NULL,LEARN_RECORD)==(er[0]|er[1]<<8)&&r8(NULL,LEARN_RECORD+2)==er[2]&&count==er[8]&&
                         !memcmp(memory+LEARN_BUFFER,ep,4*count);
            ++done[9];bad[9]+=!ok;
            if(!ok)fprintf(stderr,"learnset %u/%u differs (count %u vs %u)\n",species,form,count,er[8]);
        } else if(line[0]=='L') {
            uint32_t species,form,growth,result;
            CHECK(sscanf(line+2,"%" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32,&species,&form,&growth,&result)==4);
            growth_state();const struct px_data_api data={&api,NULL,select_growth};
            const uint32_t got=px_pokemon_level_from_growth(species,form,growth,&data);++done[3];
            if(got!=result) { ++bad[3];fprintf(stderr,"level %u/%u growth %u: %u vs game %u\n",species,form,growth,got,result); }
        }
    }
    fclose(in);free(memory);
    static const char *names[10]={"load_personal_record","personal_field","select_growth","level_from_growth",
                                  "hp_value","stat_value","move_value","maximum_pp","object_update","select_learnset"};
    printf("{");
    for(unsigned i=0;i<10;++i)printf("%s\"%s\":[%" PRIu64 ",%" PRIu64 "]",i?",":"",names[i],done[i],done[i]-bad[i]);
    printf("}\n");
    for(unsigned i=4;i<10;++i)if(bad[i])return 1;
    return (bad[0]||bad[1]||bad[2]||bad[3])?1:0;
}
