#ifndef PX_ZONE_H
#define PX_ZONE_H

#include <stddef.h>
#include <stdint.h>

/* XY zone entities (ZO section 1) and NPC registries (a/1/4/9 members), as
 * read by pk3DS 6daaca9 (OWSEStructs.cs ZoneEntities, EntityNPC, EntityWarp)
 * and CTRMap 4b780a7 (ZoneEntities.java, NPCRegistry.java). Neither layout
 * is traced in the game's code here.
 *
 * Entities: u32 length L (bytes after this word), u8 counts of furniture,
 * NPCs, warps and type-1 triggers, u32 count of type-2 triggers; then records
 * of 0x14, 0x30, 0x18, 0x18 and 0x18 bytes in that order. L must equal 8 plus
 * the records' size; the zone script that follows is not read.
 *
 * Warp (0x18): destination zone u16, destination warp index u16, face
 * direction u8, transition u8, coordinate type u16, then for type 0: x s16,
 * height s16, plane y s16, width and height in tiles s16, directionality u16
 * (0 both ways, 1 send only, 2 receive only), two unused s16. x and y are
 * world units; the first covered tile is floor(x / 18), floor(y / 18) and the
 * rectangle extends width tiles in +x and height tiles in +y. Type 1 (used
 * by outdoor zones) does not fit that layout: its fields are kept raw and the
 * warp is marked unsupported.
 *
 * NPC (0x30): id, registry model id, two move permissions, spawn flag, script,
 * face direction, sight range (u16 each), then fields kept raw, tile x u16 at
 * 0x28, tile y u16 at 0x2a, height float at 0x2c.
 *
 * Face directions (CTRMap): 0 up (-Z), 1 down (+Z), 2 left (-X), 3 right (+X).
 *
 * Registry entry (0x18): id u16 at 0, a/0/2/1 model entry u16 at 0x14. */

enum px_zone_result {
    PX_ZONE_OK = 0, PX_ZONE_INVALID_ARGUMENT, PX_ZONE_TRUNCATED, PX_ZONE_INVALID, PX_ZONE_NO_MEMORY
};

struct px_zone_warp {
    uint16_t target_zone, target_warp, coordinate_type, directionality;
    uint8_t face, transition;
    int16_t x, height, y, width, length; /* length: tiles in +y */
    int32_t tile_x, tile_y;
    int supported; /* coordinate type 0 */
    uint8_t raw[0x18];
};
struct px_zone_npc {
    uint16_t id, model, move1, move2, spawn_flag, script, face, sight;
    uint16_t tile_x, tile_y;
    float height;
    uint8_t raw[0x30];
};
struct px_zone_entities {
    uint32_t length;
    uint32_t furniture_count, npc_count, warp_count, trigger1_count, trigger2_count;
    struct px_zone_npc *npcs;
    struct px_zone_warp *warps;
};
struct px_npc_registry_entry { uint16_t id, model; };
struct px_npc_registry { uint32_t count; struct px_npc_registry_entry *entries; };

/* Caps: 255 records of each byte-counted kind, 4096 type-2 triggers. Output
 * owns its arrays; on failure it is zeroed. */
enum px_zone_result px_zone_entities_decode(const uint8_t *data, size_t size, struct px_zone_entities *out);
void px_zone_entities_free(struct px_zone_entities *entities);
/* Index of the first supported warp whose rectangle covers the tile, or -1. */
int32_t px_zone_warp_at(const struct px_zone_entities *entities, int32_t tile_x, int32_t tile_y);
/* Registry: whole 0x18-byte entries; a trailing partial entry is rejected. */
enum px_zone_result px_npc_registry_decode(const uint8_t *data, size_t size, struct px_npc_registry *out);
void px_npc_registry_free(struct px_npc_registry *registry);
const struct px_npc_registry_entry *px_npc_registry_find(const struct px_npc_registry *registry, uint16_t id);
const char *px_zone_result_string(enum px_zone_result result);

#endif
