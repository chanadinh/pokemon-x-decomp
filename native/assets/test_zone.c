#include "zone.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void put16(uint8_t *p, unsigned v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); }
static void put32(uint8_t *p, uint32_t v) { put16(p, v & 0xffff); put16(p + 2, v >> 16); }

/* One furniture record, two NPCs, three warps (type 0, type 0 at negative
 * coordinates, type 1), one type-1 and one type-2 trigger, then script bytes. */
static size_t build(uint8_t *d)
{
    memset(d, 0, 512);
    const uint32_t length = 8 + 0x14 + 2 * 0x30 + 3 * 0x18 + 2 * 0x18;
    put32(d, length);
    d[4] = 1; d[5] = 2; d[6] = 3; d[7] = 1; put32(d + 8, 1);
    uint8_t *p = d + 12 + 0x14;
    for (unsigned i = 0; i < 2; ++i, p += 0x30) {
        put16(p, 10 + i); put16(p + 2, 47); put16(p + 10, 3 + i); put16(p + 12, i);
        put16(p + 0x28, 10 + i); put16(p + 0x2a, 4);
        const float height = i ? 9.5f : 0;
        uint32_t bits; memcpy(&bits, &height, 4); put32(p + 0x2c, bits);
    }
    /* Warp 0: zone 4 warp 1, face right, x 63 (tile 3), y 279 (tile 15), 1x2. */
    put16(p, 4); put16(p + 2, 1); p[4] = 3; p[5] = 5; put16(p + 6, 0);
    put16(p + 8, 63); put16(p + 10, (unsigned)-9 & 0xffff); put16(p + 12, 279); put16(p + 14, 1); put16(p + 16, 2);
    p += 0x18;
    /* Warp 1: x -9 (tile -1), y 9 (tile 0), 2x1, receive only. */
    put16(p, 7); put16(p + 2, 0); p[4] = 1; put16(p + 8, (unsigned)-9 & 0xffff); put16(p + 12, 9);
    put16(p + 14, 2); put16(p + 16, 1); put16(p + 18, 2);
    p += 0x18;
    /* Warp 2: coordinate type 1, kept raw. */
    put16(p, 9); put16(p + 6, 1); put16(p + 8, 0xbef6); put16(p + 14, 0xffe4);
    p += 0x18 + 2 * 0x18;
    memcpy(p, "SCRIPT", 6);
    return (size_t)(p - d) + 6;
}

int main(void)
{
    uint8_t d[512];
    const size_t n = build(d);
    struct px_zone_entities e;
    assert(px_zone_entities_decode(d, n, &e) == PX_ZONE_OK);
    assert(e.furniture_count == 1 && e.npc_count == 2 && e.warp_count == 3 && e.trigger1_count == 1 &&
           e.trigger2_count == 1 && e.length == 8 + 0x14 + 0x60 + 0x48 + 0x30);
    assert(e.npcs[1].id == 11 && e.npcs[1].model == 47 && e.npcs[1].script == 4 && e.npcs[1].face == 1);
    assert(e.npcs[1].tile_x == 11 && e.npcs[1].tile_y == 4 && e.npcs[1].height == 9.5f);
    const struct px_zone_warp *w = e.warps;
    assert(w[0].supported && w[0].target_zone == 4 && w[0].target_warp == 1 && w[0].face == 3 &&
           w[0].transition == 5 && w[0].height == -9 && w[0].tile_x == 3 && w[0].tile_y == 15 &&
           w[0].width == 1 && w[0].length == 2);
    assert(w[1].supported && w[1].tile_x == -1 && w[1].tile_y == 0 && w[1].directionality == 2);
    assert(!w[2].supported && w[2].coordinate_type == 1 && w[2].target_zone == 9 && w[2].raw[8] == 0xf6);
    assert(px_zone_warp_at(&e, 3, 15) == 0 && px_zone_warp_at(&e, 3, 16) == 0 && px_zone_warp_at(&e, 3, 17) == -1);
    assert(px_zone_warp_at(&e, 4, 15) == -1 && px_zone_warp_at(&e, 0, 0) == 1 && px_zone_warp_at(&e, 1, 0) == -1);
    assert(px_zone_warp_at(NULL, 3, 15) == -1);
    px_zone_entities_free(&e);
    assert(e.npcs == NULL && e.warps == NULL);
    /* Rejections. */
    uint8_t c[512];
    assert(px_zone_entities_decode(NULL, n, &e) == PX_ZONE_INVALID_ARGUMENT);
    assert(px_zone_entities_decode(d, n, NULL) == PX_ZONE_INVALID_ARGUMENT);
    assert(px_zone_entities_decode(d, 11, &e) == PX_ZONE_TRUNCATED);
    assert(px_zone_entities_decode(d, 4 + 8 + 0x14 + 0x60 + 0x48 + 0x30 - 1, &e) == PX_ZONE_TRUNCATED);
    memcpy(c, d, n); c[0] += 4;                         /* length disagrees with the counts */
    assert(px_zone_entities_decode(c, n, &e) == PX_ZONE_INVALID && e.warps == NULL);
    memcpy(c, d, n); put32(c + 8, 5000);                /* type-2 trigger cap */
    assert(px_zone_entities_decode(c, n, &e) == PX_ZONE_INVALID);
    const size_t warp0 = 12 + 0x14 + 0x60;
    memcpy(c, d, n); put16(c + warp0 + 14, 0);           /* zero width */
    assert(px_zone_entities_decode(c, n, &e) == PX_ZONE_INVALID);
    memcpy(c, d, n); put16(c + warp0 + 18, 3);           /* unknown directionality */
    assert(px_zone_entities_decode(c, n, &e) == PX_ZONE_INVALID);
    memcpy(c, d, n); put32(c + 12 + 0x14 + 0x2c, 0x7fc00000); /* NaN height */
    assert(px_zone_entities_decode(c, n, &e) == PX_ZONE_INVALID);
    /* Registry. */
    uint8_t r[48] = {0};
    put16(r, 47); put16(r + 0x14, 513); put16(r + 0x18, 48); put16(r + 0x18 + 0x14, 7);
    struct px_npc_registry reg;
    assert(px_npc_registry_decode(r, 48, &reg) == PX_ZONE_OK && reg.count == 2);
    assert(px_npc_registry_find(&reg, 47)->model == 513 && px_npc_registry_find(&reg, 48)->model == 7);
    assert(!px_npc_registry_find(&reg, 49) && !px_npc_registry_find(NULL, 47));
    px_npc_registry_free(&reg);
    assert(px_npc_registry_decode(r, 47, &reg) == PX_ZONE_TRUNCATED && reg.entries == NULL);
    assert(px_npc_registry_decode(r, 0, &reg) == PX_ZONE_OK && reg.count == 0);
    px_npc_registry_free(&reg);
    assert(px_npc_registry_decode(r, 48, NULL) == PX_ZONE_INVALID_ARGUMENT);
    assert(!strcmp(px_zone_result_string(PX_ZONE_INVALID), "invalid zone data"));
    /* Region props: two records and padding. */
    uint8_t g[128] = {0};
    put32(g, 2);
    const float first[9] = {1, 1, 1, 0, 90, 0, 8145, 144, 10755}, second[9] = {2, .5f, 1, 0, 0, 0, -1, 0, 3};
    put32(g + 4, 5); memcpy(g + 8, first, sizeof first); put32(g + 44, 9);
    put32(g + 48, 10002); memcpy(g + 52, second, sizeof second);
    struct px_gr_props props;
    assert(px_gr_props_decode(g, sizeof g, &props) == PX_ZONE_OK && props.count == 2);
    assert(props.props[0].uid == 5 && props.props[0].rotation[1] == 90 && props.props[0].position[2] == 10755 &&
           props.props[0].unknown == 9);
    assert(props.props[1].uid == 10002 && props.props[1].scale[1] == .5f && props.props[1].position[0] == -1);
    px_gr_props_free(&props);
    assert(props.props == NULL && props.count == 0);
    assert(px_gr_props_decode(g, 4 + 2 * 0x2c - 1, &props) == PX_ZONE_TRUNCATED && !props.props);
    assert(px_gr_props_decode(g, 3, &props) == PX_ZONE_TRUNCATED);
    put32(g, 5000);
    assert(px_gr_props_decode(g, sizeof g, &props) == PX_ZONE_INVALID);
    put32(g, 0);
    assert(px_gr_props_decode(g, 4, &props) == PX_ZONE_OK && props.count == 0);
    px_gr_props_free(&props);
    assert(px_gr_props_decode(NULL, 4, &props) == PX_ZONE_INVALID_ARGUMENT);
    /* Prop registry: 80-byte entries, padding after them. */
    uint8_t a[4 + 2 * 80 + 12] = {0};
    put32(a, 2);
    put16(a + 4, 5); put16(a + 6, 5); a[8] = 1;
    put16(a + 84, 10002); put16(a + 86, 24); a[89] = 2; a[84 + 79] = 0x5a;
    struct px_prop_registry pr;
    assert(px_prop_registry_decode(a, sizeof a, &pr) == PX_ZONE_OK && pr.count == 2);
    assert(px_prop_registry_find(&pr, 5)->model == 5 && px_prop_registry_find(&pr, 5)->event_script[0] == 1);
    assert(px_prop_registry_find(&pr, 10002)->model == 24 && px_prop_registry_find(&pr, 10002)->event_script[1] == 2 &&
           px_prop_registry_find(&pr, 10002)->raw[79] == 0x5a);
    assert(!px_prop_registry_find(&pr, 6) && !px_prop_registry_find(NULL, 5));
    px_prop_registry_free(&pr);
    assert(px_prop_registry_decode(a, 4 + 2 * 80 - 1, &pr) == PX_ZONE_TRUNCATED && !pr.entries);
    put32(a, 4097);
    assert(px_prop_registry_decode(a, sizeof a, &pr) == PX_ZONE_INVALID);
    assert(px_prop_registry_decode(a, sizeof a, NULL) == PX_ZONE_INVALID_ARGUMENT);
    puts("zone entity, NPC registry, prop and prop registry tests passed");
    return 0;
}
