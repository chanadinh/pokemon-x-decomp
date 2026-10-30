/* Optional original-data sweep; no original resources are embedded. */
#include "../battle_types.h"
#include "../../../native/assets/pokedata.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"check_chart:%d: %s\n",__LINE__,#c);exit(1); } } while (0)
/* DllBattle segment 1 (rodata) is at analysis address 108ce000, file offset 0xce000. */
enum { RODATA=0x108ce000, RODATA_OFFSET=0xce000, RULES=0x108fca00 };
static uint8_t *cro;static size_t cro_size;static uint8_t rules[4];static unsigned aborts;
static uint8_t r8(void *u,uint32_t a)
{
    (void)u;
    if(a-RULES<sizeof rules)return rules[a-RULES];
    CHECK(a>=RODATA&&a-RODATA+RODATA_OFFSET<cro_size);return cro[a-RODATA+RODATA_OFFSET];
}
static uint16_t r16(void *u,uint32_t a) { (void)u;(void)a;CHECK(0);return 0; }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r8(u,a)|(uint32_t)r8(u,a+1)<<8|(uint32_t)r8(u,a+2)<<16|(uint32_t)r8(u,a+3)<<24; }
static void abort_battle(void *u) { (void)u;++aborts; }
int main(int argc,char **argv)
{
    CHECK(argc==2);FILE *f=fopen(argv[1],"rb");CHECK(f);
    CHECK(fseek(f,0,SEEK_END)==0);const long n=ftell(f);CHECK(n>0);rewind(f);cro_size=(size_t)n;
    cro=malloc(cro_size);CHECK(cro&&fread(cro,1,cro_size,f)==cro_size);fclose(f);
    uint8_t chart[PX_TYPE_COUNT][PX_TYPE_COUNT];
    CHECK(px_type_chart_decode(cro,cro_size,chart)==PX_POKEDATA_OK);
    const struct px_pokemon_api memory={.load8=r8,.load16=r16,.load32=r32};
    const struct px_battle_types_api api={&memory,NULL,abort_battle,PX_BATTLE_TYPE_CHART_STATIC,
                                          PX_BATTLE_EFFECTIVENESS_TABLE_STATIC,PX_BATTLE_RULES_OBJECT_STATIC};
    unsigned pairs=0,counts[2][14]={{0}};
    for(unsigned inverse=0;inverse<2;++inverse) {
        rules[1]=(uint8_t)inverse;
        for(uint32_t a=0;a<PX_TYPE_COUNT;++a)for(uint32_t d=0;d<PX_TYPE_COUNT;++d) {
            /* Native decoder value -> code, with Inverse Battle swapping weak/strong and immune -> strong. */
            const uint8_t v=chart[a][d];
            const uint32_t want=inverse?(v==4?7:v==8?6:8):(v==0?0:v==2?6:v==4?7:8);
            const uint32_t got=px_battle_type_effectiveness(a,d,&api);
            CHECK(got==want);++counts[inverse][got];++pairs;
        }
        CHECK(px_battle_type_effectiveness(PX_BATTLE_TYPELESS,0,&api)==7);
    }
    /* Well-known matchups (type order Normal..Fairy). */
    rules[1]=0;
    CHECK(px_battle_type_effectiveness(0,7,&api)==0);    /* Normal -> Ghost */
    CHECK(px_battle_type_effectiveness(9,11,&api)==8);   /* Fire -> Grass */
    CHECK(px_battle_type_effectiveness(10,9,&api)==8);   /* Water -> Fire */
    CHECK(px_battle_type_effectiveness(15,17,&api)==0);  /* Dragon -> Fairy */
    CHECK(px_battle_type_effectiveness(12,4,&api)==0);   /* Electric -> Ground */
    CHECK(px_battle_type_effectiveness(1,0,&api)==8);    /* Fighting -> Normal */
    /* The power table and every combination against the dual-type rule. */
    for(uint32_t i=0;i<14;++i)CHECK(r32(NULL,PX_BATTLE_EFFECTIVENESS_TABLE_STATIC+4*i)==(i?UINT32_C(1)<<(i-1):0));
    unsigned combinations=0;
    for(uint32_t a=0;a<14;++a)for(uint32_t b=0;b<14;++b) {
        const int sum=(int)a+(int)b-7;
        const uint32_t want=!a||!b?0:sum<1?1:sum>13?13:(uint32_t)sum;
        CHECK(px_battle_combine_effectiveness(a,b,&api)==want);++combinations;
    }
    CHECK(aborts==42);  /* clamped combinations: 21 below x1/64 and 21 above x64 */
    printf("{\n  \"type_pairs\": %u,\n  \"normal_codes\": {\"0\": %u, \"6\": %u, \"7\": %u, \"8\": %u},\n"
           "  \"inverse_codes\": {\"6\": %u, \"7\": %u, \"8\": %u},\n  \"combinations\": %u,\n  \"all_match\": true\n}\n",
           pairs,counts[0][0],counts[0][6],counts[0][7],counts[0][8],counts[1][6],counts[1][7],counts[1][8],combinations);
    free(cro);return 0;
}
