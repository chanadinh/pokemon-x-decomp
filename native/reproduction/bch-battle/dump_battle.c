/* Local validation bridge for verify_battle.py.
 *
 *   dump_battle tables ROMFS          one JSON line per personal entry, move,
 *                                     learnset, then the type chart, as
 *                                     decoded by assets/pokedata.c
 *   dump_battle encounters FILE...    one JSON line per ZO section-3 file
 *   dump_battle compute ROMFS CODEBIN reads cases from stdin, one per line:
 *       stats SPECIES LEVEL NATURE IV0..IV5
 *       damage A_SPECIES A_LEVEL A_NATURE D_SPECIES D_LEVEL D_NATURE MOVE CRIT ROLL A_STAGE D_STAGE
 *     (IVs 31 for damage cases; A_STAGE/D_STAGE the attacking and defending
 *     stat stages) and prints the stats or "damage EFFECTIVENESS" per case.
 * Output may contain local asset data; keep it in ignored outputs/. */
#include "../../assets/garc.h"
#include "../../assets/pokedata.h"
#include "../../game/battle.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t *member(const char *romfs, const char *archive, uint32_t entry, size_t *size, uint32_t *count)
{
    char path[1024], error[256];
    snprintf(path, sizeof path, "%s/%s", romfs, archive);
    struct px_garc *g = NULL;
    if (!px_garc_open(path, &g, error, sizeof error)) { fprintf(stderr, "%s\n", error); exit(1); }
    if (count) *count = (uint32_t)px_garc_count(g);
    uint8_t *d = NULL;
    if (entry != UINT32_MAX && !px_garc_read(g, entry, 0, 1u << 24, &d, size, error, sizeof error)) {
        fprintf(stderr, "%s\n", error); exit(1);
    }
    px_garc_close(g);
    return d;
}

static uint8_t *file(const char *path, size_t *size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    const long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *d = malloc(n > 0 ? (size_t)n : 1);
    *size = d && fread(d, 1, (size_t)n, f) == (size_t)n ? (size_t)n : 0;
    fclose(f);
    return d;
}

static struct px_personal personal[2048];
static struct px_move moves[1024];
static uint8_t chart[PX_TYPE_COUNT][PX_TYPE_COUNT];
static uint32_t personal_count, move_count;

static void load(const char *romfs, int print)
{
    uint32_t count;
    size_t size;
    member(romfs, "a/2/1/8", UINT32_MAX, NULL, &count);
    personal_count = count - 1;
    for (uint32_t i = 0; i < personal_count && i < 2048; ++i) {
        uint8_t *d = member(romfs, "a/2/1/8", i, &size, NULL);
        const int r = px_personal_decode(d, size, &personal[i]);
        free(d);
        const struct px_personal *p = &personal[i];
        if (print)
            printf("{\"personal\":%u,\"result\":%d,\"base\":[%u,%u,%u,%u,%u,%u],\"types\":[%u,%u],\"growth\":%u,"
                   "\"catch\":%u,\"abilities\":[%u,%u,%u],\"base_exp\":%u}\n", i, r, p->base[0], p->base[1], p->base[2],
                   p->base[3], p->base[4], p->base[5], p->types[0], p->types[1], p->growth, p->catch_rate,
                   p->abilities[0], p->abilities[1], p->abilities[2], p->base_exp);
    }
    member(romfs, "a/2/1/2", UINT32_MAX, NULL, &move_count);
    for (uint32_t i = 0; i < move_count && i < 1024; ++i) {
        uint8_t *d = member(romfs, "a/2/1/2", i, &size, NULL);
        const int r = px_move_decode(d, size, &moves[i]);
        free(d);
        const struct px_move *m = &moves[i];
        if (print)
            printf("{\"move\":%u,\"result\":%d,\"fields\":[%u,%u,%u,%u,%u,%u,%u,%u],\"stat\":[%u,%u,%u],"
                   "\"stage\":[%d,%d,%d],\"percent\":[%u,%u,%u]}\n", i, r, m->type, m->category, m->power,
                   m->accuracy, m->pp, m->priority, m->crit_stage, m->target, m->stat[0], m->stat[1], m->stat[2],
                   m->stage[0], m->stage[1], m->stage[2], m->stat_percent[0], m->stat_percent[1],
                   m->stat_percent[2]);
    }
    uint8_t *cro = NULL;
    char path[1024];
    snprintf(path, sizeof path, "%s/DllBattle.cro", romfs);
    cro = file(path, &size);
    const int r = px_type_chart_decode(cro, cro ? size : 0, chart);
    free(cro);
    if (print) {
        uint32_t learn_count;
        member(romfs, "a/2/1/4", UINT32_MAX, NULL, &learn_count);
        static struct px_learnset l;
        for (uint32_t i = 0; i < learn_count; ++i) {
            uint8_t *d = member(romfs, "a/2/1/4", i, &size, NULL);
            const int lr = px_learnset_decode(d, size, &l);
            free(d);
            printf("{\"learnset\":%u,\"result\":%d,\"pairs\":[", i, lr);
            for (uint32_t k = 0; k < l.count; ++k) printf("%s[%u,%d]", k ? "," : "", l.move[k], l.level[k]);
            uint16_t at[4];
            printf("],\"at5\":[");
            px_learnset_moves_at(&l, 5, at);
            for (unsigned k = 0; k < 4; ++k) printf("%s%u", k ? "," : "", at[k]);
            printf("],\"at50\":[");
            px_learnset_moves_at(&l, 50, at);
            for (unsigned k = 0; k < 4; ++k) printf("%s%u", k ? "," : "", at[k]);
            printf("]}\n");
        }
        printf("{\"chart_result\":%d,\"chart\":[", r);
        for (unsigned i = 0; i < PX_TYPE_COUNT * PX_TYPE_COUNT; ++i)
            printf("%s%u", i ? "," : "", chart[i / PX_TYPE_COUNT][i % PX_TYPE_COUNT]);
        printf("]}\n");
    }
}

int main(int argc, char **argv)
{
    if (argc >= 3 && !strcmp(argv[1], "tables")) { load(argv[2], 1); return 0; }
    if (argc >= 3 && !strcmp(argv[1], "encounters")) {
        for (int i = 2; i < argc; ++i) {
            size_t size = 0;
            uint8_t *d = file(argv[i], &size);
            struct px_encounter_table t;
            const int r = px_encounter_table_decode(d, size, &t);
            free(d);
            printf("{\"result\":%d,\"slots\":[", r);
            for (unsigned k = 0; k < PX_SLOT_COUNT; ++k)
                printf("%s[%u,%u,%u,%u]", k ? "," : "", t.slots[k].species, t.slots[k].form, t.slots[k].min_level,
                       t.slots[k].max_level);
            printf("]}\n");
        }
        return 0;
    }
    if (argc < 4 || strcmp(argv[1], "compute")) {
        fprintf(stderr, "usage: %s tables ROMFS | encounters FILE... | compute ROMFS CODEBIN\n", argv[0]);
        return 2;
    }
    load(argv[2], 0);
    size_t size = 0;
    uint8_t *code = file(argv[3], &size);
    if (!code || size < 0x470f6c + 125) return 1;
    const struct px_battle_data data = {personal, personal_count, moves, move_count,
                                        (const uint8_t (*)[PX_TYPE_COUNT])chart, code + 0x470f6c};
    char line[256];
    const uint16_t none[4] = {0, 0, 0, 0};
    while (fgets(line, sizeof line, stdin)) {
        unsigned v[16] = {0};
        int sa = 0, sd = 0;
        if (!strncmp(line, "stats ", 6) &&
            sscanf(line + 6, "%u %u %u %u %u %u %u %u %u", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &v[6], &v[7],
                   &v[8]) == 9) {
            const uint8_t iv[6] = {(uint8_t)v[3], (uint8_t)v[4], (uint8_t)v[5], (uint8_t)v[6], (uint8_t)v[7], (uint8_t)v[8]};
            struct px_battler b;
            if (!px_battler_make(&data, (uint16_t)v[0], (uint8_t)v[1], iv, (uint8_t)v[2], none, &b)) { puts("-"); continue; }
            printf("%u %u %u %u %u %u\n", b.stats[0], b.stats[1], b.stats[2], b.stats[3], b.stats[4], b.stats[5]);
        } else if (!strncmp(line, "damage ", 7) &&
                   sscanf(line + 7, "%u %u %u %u %u %u %u %u %u %d %d", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5],
                          &v[6], &v[7], &v[8], &sa, &sd) == 11) {
            const uint8_t iv[6] = {31, 31, 31, 31, 31, 31};
            struct px_battler a, d;
            if (!px_battler_make(&data, (uint16_t)v[0], (uint8_t)v[1], iv, (uint8_t)v[2], none, &a) ||
                !px_battler_make(&data, (uint16_t)v[3], (uint8_t)v[4], iv, (uint8_t)v[5], none, &d) || v[6] >= move_count) {
                puts("-"); continue;
            }
            for (unsigned k = 1; k < 6; ++k) { a.stages[k] = (int8_t)sa; d.stages[k] = (int8_t)sd; }
            uint32_t e;
            const uint32_t damage = px_battle_damage(&data, &a, &d, (uint16_t)v[6], (int)v[7], v[8], &e);
            printf("%" PRIu32 " %" PRIu32 "\n", damage, e);
        } else puts("?");
    }
    free(code);
    return 0;
}
