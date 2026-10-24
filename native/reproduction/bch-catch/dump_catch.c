/* Local validation bridge for verify_catch.py.
 *
 *   dump_catch ROMFS     loads personal (a/2/1/8), growth (a/2/1/7) and
 *                        learnset (a/2/1/4) data as the viewer does, prints
 *                        one JSON line per growth table, then reads cases
 *                        from stdin, one per line:
 *       level SPECIES EXP              -> the level for that experience
 *       gain SPECIES LEVEL             -> experience for defeating it
 *       catch SPECIES LEVEL HP BALL10  -> the shake threshold (0..65536)
 *       start SPECIES LEVEL            -> the experience a new battler has
 * Output may contain local asset data; keep it in ignored outputs/. */
#include "../../assets/garc.h"
#include "../../assets/pokedata.h"
#include "../../game/battle.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct px_personal personal[2048];
static struct px_learnset learnsets[2048];
static uint32_t growth[8][101];

static struct px_garc *open_archive(const char *romfs, const char *name, uint32_t *count)
{
    char path[1024], error[256];
    snprintf(path, sizeof path, "%s/%s", romfs, name);
    struct px_garc *g = NULL;
    if (!px_garc_open(path, &g, error, sizeof error)) { fprintf(stderr, "%s\n", error); exit(1); }
    *count = (uint32_t)px_garc_count(g);
    return g;
}

static uint8_t *read_member(struct px_garc *g, uint32_t i, size_t *size)
{
    char error[256];
    uint8_t *d = NULL;
    if (!px_garc_read(g, i, 0, 1u << 24, &d, size, error, sizeof error)) { fprintf(stderr, "%s\n", error); exit(1); }
    return d;
}

int main(int argc, char **argv)
{
    if (argc != 2) { fprintf(stderr, "usage: %s ROMFS < cases\n", argv[0]); return 2; }
    uint32_t personal_count, growth_count, learnset_count;
    size_t size;
    struct px_garc *g = open_archive(argv[1], "a/2/1/8", &personal_count);
    if (--personal_count > 2048) return 1;
    for (uint32_t i = 0; i < personal_count; ++i) {
        uint8_t *d = read_member(g, i, &size);
        if (px_personal_decode(d, size, &personal[i]) != PX_POKEDATA_OK) return 1;
        free(d);
    }
    px_garc_close(g);
    g = open_archive(argv[1], "a/2/1/7", &growth_count);
    if (growth_count > 8) growth_count = 8;
    for (uint32_t i = 0; i < growth_count; ++i) {
        uint8_t *d = read_member(g, i, &size);
        if (size < 404) return 1;
        printf("{\"growth\":%u,\"size\":%zu,\"exp\":[", i, size);
        for (unsigned l = 0; l <= 100; ++l) {
            growth[i][l] = (uint32_t)d[4 * l] | (uint32_t)d[4 * l + 1] << 8 | (uint32_t)d[4 * l + 2] << 16 |
                           (uint32_t)d[4 * l + 3] << 24;
            printf("%s%u", l ? "," : "", growth[i][l]);
        }
        printf("]}\n");
        free(d);
    }
    px_garc_close(g);
    g = open_archive(argv[1], "a/2/1/4", &learnset_count);
    if (learnset_count > 2048) return 1;
    for (uint32_t i = 0; i < learnset_count; ++i) {
        uint8_t *d = read_member(g, i, &size);
        if (px_learnset_decode(d, size, &learnsets[i]) != PX_POKEDATA_OK) return 1;
        free(d);
    }
    px_garc_close(g);
    static struct px_move moves[1];
    static uint8_t chart[PX_TYPE_COUNT][PX_TYPE_COUNT];
    const struct px_battle_data data = {personal, personal_count, moves, 1, (const uint8_t (*)[PX_TYPE_COUNT])chart,
                                        NULL, (const uint32_t (*)[101])growth, growth_count, learnsets,
                                        learnset_count};
    const uint8_t iv[6] = {31, 31, 31, 31, 31, 31};
    const uint16_t none[4] = {0, 0, 0, 0};
    char line[256];
    while (fgets(line, sizeof line, stdin)) {
        unsigned v[4] = {0};
        struct px_battler b;
        if (sscanf(line, "level %u %u", &v[0], &v[1]) == 2) {
            printf("%u\n", px_battle_level_for(&data, (uint16_t)v[0], v[1]));
        } else if (sscanf(line, "gain %u %u", &v[0], &v[1]) == 2) {
            if (!px_battler_make(&data, (uint16_t)v[0], (uint8_t)v[1], iv, 0, none, &b)) { puts("-"); continue; }
            printf("%u\n", px_battle_exp_gain(&data, &b));
        } else if (sscanf(line, "catch %u %u %u %u", &v[0], &v[1], &v[2], &v[3]) == 4) {
            if (!px_battler_make(&data, (uint16_t)v[0], (uint8_t)v[1], iv, 0, none, &b) || v[2] > b.stats[0]) {
                puts("-"); continue;
            }
            b.hp = (uint16_t)v[2];
            printf("%u %u\n", b.stats[0], px_battle_catch_threshold(&data, &b, v[3]));
        } else if (sscanf(line, "start %u %u", &v[0], &v[1]) == 2) {
            if (!px_battler_make(&data, (uint16_t)v[0], (uint8_t)v[1], iv, 0, none, &b)) { puts("-"); continue; }
            printf("%u\n", b.exp);
        } else puts("?");
    }
    return 0;
}
