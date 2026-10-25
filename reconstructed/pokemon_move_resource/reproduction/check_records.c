/* Optional original-data comparison with the existing independent native decoder. */
#include "../pokemon_move_resource.h"
#include "../../../native/assets/garc.h"
#include "../../../native/assets/pokedata.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"check_records:%d: %s\n",__LINE__,#c); exit(1); } } while (0)
struct memory { uint8_t bytes[128]; };
static uint8_t r8(void *u,uint32_t a) { struct memory *m=u; CHECK(a<sizeof m->bytes);return m->bytes[a]; }
static uint16_t r16(void *u,uint32_t a) { return (uint16_t)((uint32_t)r8(u,a)|(uint32_t)r8(u,a+1)<<8); }
static uint32_t r32(void *u,uint32_t a) { return (uint32_t)r16(u,a)|(uint32_t)r16(u,a+2)<<16; }
static uint32_t signed8(uint8_t v) { return (uint32_t)(v<128 ? (int32_t)v : (int32_t)v-256); }
int main(int argc,char **argv)
{
    if (argc!=2) { fprintf(stderr,"usage: check_records MOVE_GARC\n");return 2; }
    struct px_garc *g=NULL; char error[256];
    CHECK(px_garc_open(argv[1],&g,error,sizeof error));
    CHECK(px_garc_count(g)==618);
    struct memory m={0};m.bytes[8]=32;
    const struct px_pokemon_api api={.user=&m,.load8=r8,.load16=r16,.load32=r32};
    unsigned cases=0,always=0,negative=0;
    for (size_t index=0;index<px_garc_count(g);++index) {
        const struct px_garc_member *member=px_garc_member_at(g,index);
        uint8_t *raw=NULL;size_t size=0;struct px_move p;
        CHECK(member && px_garc_read(g,member->entry,member->subentry,128,&raw,&size,error,sizeof error));
        CHECK(size==36 && px_move_decode(raw,size,&p)==PX_POKEDATA_OK);
        memcpy(m.bytes+32,raw,36);free(raw);
        uint32_t expected[33]={p.type,p.quality,p.category,p.power,p.accuracy==101?100:p.accuracy,
            p.pp,signed8(p.priority),p.crit_stage,p.hit_max,p.hit_min,p.flinch,p.inflict,
            p.inflict_percent,p.inflict_duration,p.turn_min,p.turn_max,p.stat[0],p.stat[1],p.stat[2],
            (uint32_t)(int32_t)p.stage[0],(uint32_t)(int32_t)p.stage[1],(uint32_t)(int32_t)p.stage[2],
            p.stat_percent[0],p.stat_percent[1],p.stat_percent[2],p.recoil<128?p.recoil:0,
            p.healing<128?p.healing:0,p.target,p.effect,0,p.recoil>=128?256u-p.recoil:0,
            p.healing>=128?256u-p.healing:0,p.accuracy==101};
        for (unsigned selector=0;selector<33;++selector) {
            CHECK(px_move_resource_field(0,selector,&api)==expected[selector]);++cases;
        }
        always+=p.accuracy==101;
        negative+=p.priority>=128;
    }
    px_garc_close(g);
    printf("{\"move_records\":618,\"selector_cases\":%u,\"accuracy_sentinel_records\":%u,\"negative_priority_records\":%u,\"all_match\":true}\n",cases,always,negative);
    return 0;
}
