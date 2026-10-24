/* Local validation bridge for verify_zone.py.
 *
 *   dump_zone entities FILE...   one JSON line per ZO section-1 file:
 *                                px_zone_entities_decode result, counts,
 *                                NPCs and warps (raw warp bytes as hex)
 *   dump_zone registry FILE...   one JSON line per a/1/4/9 member
 * Output may contain local asset data; keep it in ignored outputs/. */
#include "../../assets/zone.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t *read_file(const char *path, size_t *size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    uint8_t *data = NULL;
    if (!fseek(f, 0, SEEK_END)) {
        const long n = ftell(f);
        if (n >= 0 && !fseek(f, 0, SEEK_SET) && (data = malloc(n ? (size_t)n : 1)) &&
            fread(data, 1, (size_t)n, f) != (size_t)n) { free(data); data = NULL; }
        *size = (size_t)n;
    }
    fclose(f);
    return data;
}
int main(int argc, char **argv)
{
    if (argc < 3 || (strcmp(argv[1], "entities") && strcmp(argv[1], "registry"))) {
        fprintf(stderr, "usage: %s entities|registry FILE...\n", argv[0]);
        return 2;
    }
    for (int i = 2; i < argc; ++i) {
        size_t size = 0;
        uint8_t *data = read_file(argv[i], &size);
        if (!data) return 1;
        if (!strcmp(argv[1], "registry")) {
            struct px_npc_registry r;
            const enum px_zone_result result = px_npc_registry_decode(data, size, &r);
            printf("{\"result\":%d,\"entries\":[", (int)result);
            for (uint32_t k = 0; k < r.count; ++k)
                printf("%s[%u,%u]", k ? "," : "", r.entries[k].id, r.entries[k].model);
            printf("]}\n");
            px_npc_registry_free(&r);
        } else {
            struct px_zone_entities e;
            const enum px_zone_result result = px_zone_entities_decode(data, size, &e);
            printf("{\"result\":%d,\"length\":%" PRIu32 ",\"counts\":[%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32
                   ",%" PRIu32 "],\"npcs\":[", (int)result, e.length, e.furniture_count, e.npc_count,
                   e.warp_count, e.trigger1_count, e.trigger2_count);
            for (uint32_t k = 0; k < e.npc_count; ++k) {
                const struct px_zone_npc *n = &e.npcs[k];
                uint32_t bits; memcpy(&bits, &n->height, 4);
                printf("%s[%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%" PRIu32 "]", k ? "," : "", n->id, n->model, n->move1,
                       n->move2, n->spawn_flag, n->script, n->face, n->sight, n->tile_x, n->tile_y, bits);
            }
            printf("],\"warps\":[");
            for (uint32_t k = 0; k < e.warp_count; ++k) {
                const struct px_zone_warp *w = &e.warps[k];
                printf("%s{\"supported\":%d,\"fields\":[%u,%u,%u,%u,%u,%d,%d,%d,%d,%d,%u,%" PRId32 ",%" PRId32
                       "],\"raw\":\"", k ? "," : "", w->supported, w->target_zone, w->target_warp, w->face,
                       w->transition, w->coordinate_type, w->x, w->height, w->y, w->width, w->length,
                       w->directionality, w->tile_x, w->tile_y);
                for (unsigned j = 0; j < sizeof w->raw; ++j) printf("%02x", w->raw[j]);
                printf("\"}");
            }
            printf("]}\n");
            px_zone_entities_free(&e);
        }
        free(data);
    }
    return 0;
}
