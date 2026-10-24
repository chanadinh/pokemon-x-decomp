#include "zone.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

enum { FURNITURE = 0x14, NPC = 0x30, WARP = 0x18, TRIGGER = 0x18, REGISTRY = 0x18,
       MAX_TRIGGER2 = 4096, MAX_REGISTRY = 65536 };

static uint16_t u16(const uint8_t *p) { return (uint16_t)(p[0] | p[1] << 8); }
static int16_t s16(const uint8_t *p) { uint16_t v = u16(p); int16_t s; memcpy(&s, &v, sizeof s); return s; }
static uint32_t u32(const uint8_t *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}
static int32_t tile(int32_t world) { return world >= 0 ? world / 18 : -((-world + 17) / 18); }

enum px_zone_result px_zone_entities_decode(const uint8_t *data, size_t size, struct px_zone_entities *out)
{
    if (!out) return PX_ZONE_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    if (!data) return PX_ZONE_INVALID_ARGUMENT;
    if (size < 12) return PX_ZONE_TRUNCATED;
    const uint32_t length = u32(data), trigger2 = u32(data + 8);
    if (trigger2 > MAX_TRIGGER2) return PX_ZONE_INVALID;
    const uint64_t expected = 8 + (uint64_t)data[4] * FURNITURE + (uint64_t)data[5] * NPC +
                              (uint64_t)data[6] * WARP + (uint64_t)data[7] * TRIGGER + (uint64_t)trigger2 * TRIGGER;
    if (length != expected) return PX_ZONE_INVALID;
    if ((uint64_t)length + 4 > size) return PX_ZONE_TRUNCATED;
    out->length = length;
    out->furniture_count = data[4]; out->npc_count = data[5]; out->warp_count = data[6];
    out->trigger1_count = data[7]; out->trigger2_count = trigger2;
    out->npcs = calloc(out->npc_count ? out->npc_count : 1, sizeof *out->npcs);
    out->warps = calloc(out->warp_count ? out->warp_count : 1, sizeof *out->warps);
    if (!out->npcs || !out->warps) { px_zone_entities_free(out); return PX_ZONE_NO_MEMORY; }
    const uint8_t *p = data + 12 + (size_t)out->furniture_count * FURNITURE;
    for (uint32_t i = 0; i < out->npc_count; ++i, p += NPC) {
        struct px_zone_npc *n = &out->npcs[i];
        memcpy(n->raw, p, NPC);
        n->id = u16(p); n->model = u16(p + 2); n->move1 = u16(p + 4); n->move2 = u16(p + 6);
        n->spawn_flag = u16(p + 8); n->script = u16(p + 10); n->face = u16(p + 12); n->sight = u16(p + 14);
        n->tile_x = u16(p + 0x28); n->tile_y = u16(p + 0x2a);
        uint32_t bits = u32(p + 0x2c);
        memcpy(&n->height, &bits, sizeof n->height);
        if (!isfinite(n->height)) { px_zone_entities_free(out); return PX_ZONE_INVALID; }
    }
    for (uint32_t i = 0; i < out->warp_count; ++i, p += WARP) {
        struct px_zone_warp *w = &out->warps[i];
        memcpy(w->raw, p, WARP);
        w->target_zone = u16(p); w->target_warp = u16(p + 2); w->face = p[4]; w->transition = p[5];
        w->coordinate_type = u16(p + 6);
        if (w->coordinate_type != 0) continue;
        w->x = s16(p + 8); w->height = s16(p + 10); w->y = s16(p + 12);
        w->width = s16(p + 14); w->length = s16(p + 16); w->directionality = u16(p + 18);
        if (w->width <= 0 || w->length <= 0 || w->directionality > 2 || w->face > 3) {
            px_zone_entities_free(out); return PX_ZONE_INVALID;
        }
        w->supported = 1;
        w->tile_x = tile(w->x); w->tile_y = tile(w->y);
    }
    return PX_ZONE_OK;
}

void px_zone_entities_free(struct px_zone_entities *e)
{
    if (!e) return;
    free(e->npcs); free(e->warps);
    memset(e, 0, sizeof *e);
}

int32_t px_zone_warp_at(const struct px_zone_entities *e, int32_t x, int32_t y)
{
    for (uint32_t i = 0; e && i < e->warp_count; ++i) {
        const struct px_zone_warp *w = &e->warps[i];
        if (w->supported && x >= w->tile_x && x < w->tile_x + w->width && y >= w->tile_y && y < w->tile_y + w->length)
            return (int32_t)i;
    }
    return -1;
}

enum px_zone_result px_npc_registry_decode(const uint8_t *data, size_t size, struct px_npc_registry *out)
{
    if (!out) return PX_ZONE_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    if (!data && size) return PX_ZONE_INVALID_ARGUMENT;
    if (size % REGISTRY) return PX_ZONE_TRUNCATED;
    if (size / REGISTRY > MAX_REGISTRY) return PX_ZONE_INVALID;
    out->count = (uint32_t)(size / REGISTRY);
    out->entries = calloc(out->count ? out->count : 1, sizeof *out->entries);
    if (!out->entries) { out->count = 0; return PX_ZONE_NO_MEMORY; }
    for (uint32_t i = 0; i < out->count; ++i) {
        out->entries[i].id = u16(data + (size_t)i * REGISTRY);
        out->entries[i].model = u16(data + (size_t)i * REGISTRY + 0x14);
    }
    return PX_ZONE_OK;
}

void px_npc_registry_free(struct px_npc_registry *r)
{
    if (!r) return;
    free(r->entries);
    memset(r, 0, sizeof *r);
}

const struct px_npc_registry_entry *px_npc_registry_find(const struct px_npc_registry *r, uint16_t id)
{
    for (uint32_t i = 0; r && i < r->count; ++i) if (r->entries[i].id == id) return &r->entries[i];
    return NULL;
}

const char *px_zone_result_string(enum px_zone_result result)
{
    switch (result) {
    case PX_ZONE_OK: return "ok";
    case PX_ZONE_INVALID_ARGUMENT: return "invalid argument";
    case PX_ZONE_TRUNCATED: return "truncated zone data";
    case PX_ZONE_INVALID: return "invalid zone data";
    case PX_ZONE_NO_MEMORY: return "out of memory";
    }
    return "unknown zone result";
}
