/* Local validation bridge for verify_town.py.
 *
 *   dump_town props FILE...      one JSON line per GR section-3 file:
 *                                px_gr_props_decode result and every record
 *                                (floats as their 32-bit patterns)
 *   dump_town registry FILE...   one JSON line per AD section-0 file:
 *                                px_prop_registry_decode result, reference,
 *                                model and event-script bytes
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

static uint32_t bits(float value)
{
    uint32_t out;
    memcpy(&out, &value, sizeof out);
    return out;
}

int main(int argc, char **argv)
{
    if (argc < 3 || (strcmp(argv[1], "props") && strcmp(argv[1], "registry"))) {
        fprintf(stderr, "usage: %s props|registry FILE...\n", argv[0]);
        return 2;
    }
    for (int i = 2; i < argc; ++i) {
        size_t size = 0;
        uint8_t *data = read_file(argv[i], &size);
        if (!data) return 1;
        if (!strcmp(argv[1], "registry")) {
            struct px_prop_registry r;
            const enum px_zone_result result = px_prop_registry_decode(data, size, &r);
            printf("{\"result\":%d,\"entries\":[", (int)result);
            for (uint32_t k = 0; k < r.count; ++k)
                printf("%s[%u,%u,%u,%u]", k ? "," : "", r.entries[k].reference, r.entries[k].model,
                       r.entries[k].event_script[0], r.entries[k].event_script[1]);
            printf("]}\n");
            px_prop_registry_free(&r);
        } else {
            struct px_gr_props p;
            const enum px_zone_result result = px_gr_props_decode(data, size, &p);
            printf("{\"result\":%d,\"props\":[", (int)result);
            for (uint32_t k = 0; k < p.count; ++k) {
                const struct px_gr_prop *g = &p.props[k];
                printf("%s[%" PRIu32, k ? "," : "", g->uid);
                for (unsigned c = 0; c < 3; ++c) printf(",%" PRIu32, bits(g->scale[c]));
                for (unsigned c = 0; c < 3; ++c) printf(",%" PRIu32, bits(g->rotation[c]));
                for (unsigned c = 0; c < 3; ++c) printf(",%" PRIu32, bits(g->position[c]));
                printf(",%" PRIu32 "]", g->unknown);
            }
            printf("]}\n");
            px_gr_props_free(&p);
        }
        free(data);
    }
    return 0;
}
