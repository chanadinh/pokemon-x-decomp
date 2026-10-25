#include "save.h"

#include <stdio.h>
#include <string.h>

#include "../../reconstructed/pokemon_data/pokemon_data.h"
#include "../platform/guest.h"

/* The reconstructed codec reaches memory through 32-bit guest addresses:
 * the guest maps the manager, one record and its extension in the heap, and
 * the block-order table at its game address 0x00570eec (rodata, read-only).
 * Any other access is a fault, reported as an error. */
enum { MANAGER = PX_GUEST_HEAP, DATA = PX_GUEST_HEAP + 0x100, EXTRA = PX_GUEST_HEAP + 0x200, TABLE = 0x00570eec };
struct space {
    uint8_t manager[0x10], data[0xe8], extra[0x1c];
    struct px_guest guest;
};
static void space_init(struct space *s, const uint8_t table[128])
{
    memset(s->manager, 0, sizeof s->manager); memset(s->data, 0, sizeof s->data); memset(s->extra, 0, sizeof s->extra);
    px_guest_init(&s->guest);
    px_guest_map_view(&s->guest, MANAGER, sizeof s->manager, PX_GUEST_READ | PX_GUEST_WRITE, s->manager, "manager");
    px_guest_map_view(&s->guest, DATA, sizeof s->data, PX_GUEST_READ | PX_GUEST_WRITE, s->data, "record");
    px_guest_map_view(&s->guest, EXTRA, sizeof s->extra, PX_GUEST_READ | PX_GUEST_WRITE, s->extra, "extension");
    px_guest_map_view(&s->guest, TABLE, 128, PX_GUEST_READ, (void *)table, "block order");
}
#define load8(s, a) px_guest_load8(&(s)->guest, (a))
#define load16(s, a) px_guest_load16(&(s)->guest, (a))
#define load32(s, a) px_guest_load32(&(s)->guest, (a))
#define store8(s, a, v) px_guest_store8(&(s)->guest, (a), (v))
#define store16(s, a, v) px_guest_store16(&(s)->guest, (a), (v))
#define store32(s, a, v) px_guest_store32(&(s)->guest, (a), (v))
static void put16(uint8_t *p, uint32_t v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); }
static void put32(uint8_t *p, uint32_t v) { for (unsigned k = 0; k < 4; ++k) p[k] = (uint8_t)(v >> (8 * k)); }
static uint32_t get16(const uint8_t *p) { return (uint32_t)p[0] | (uint32_t)p[1] << 8; }
static uint32_t get32(const uint8_t *p) { return get16(p) | get16(p + 2) << 16; }

/* A key from the record's contents (FNV-1a), never 0. Not the game's. */
static uint32_t derive_key(const struct px_battler *b, uint32_t slot, uint32_t trainer)
{
    uint32_t h = 2166136261u;
    const uint32_t words[] = {b->species, b->level, b->nature, b->iv[0] | b->iv[1] << 5 | b->iv[2] << 10 |
                              (uint32_t)b->iv[3] << 15 | (uint32_t)b->iv[4] << 20 | (uint32_t)b->iv[5] << 25,
                              b->exp, slot, trainer};
    for (size_t i = 0; i < sizeof words / sizeof words[0]; ++i)
        for (unsigned k = 0; k < 4; ++k) { h ^= (words[i] >> (8 * k)) & 255; h *= 16777619u; }
    return h ? h : 1;
}

static int fail(char *error, size_t size, const char *text)
{
    if (error && size) snprintf(error, size, "%s", text);
    return 0;
}

int px_save_encode(struct px_save *save, const struct px_battle_data *d, const uint8_t table[128],
                   uint8_t out[PX_SAVE_SIZE], char *error, size_t error_size)
{
    if (!save || !d || !table || !out) return fail(error, error_size, "invalid save arguments");
    if (!save->party_count || save->party_count > 6) return fail(error, error_size, "party of 1..6 required");
    memset(out, 0, PX_SAVE_SIZE);
    memcpy(out, "PXSV", 4);
    put16(out + 4, 1); put16(out + 6, save->party_count);
    put32(out + 8, save->zone); put32(out + 12, (uint32_t)save->world_x); put32(out + 16, (uint32_t)save->world_z);
    out[20] = save->facing & 3;
    put32(out + 24, save->balls); put32(out + 28, save->rng); put32(out + 32, save->trainer);
    for (uint32_t i = 0; i < save->party_count; ++i) {
        struct px_battler *b = &save->party[i];
        if (!b->species || b->species >= d->personal_count) return fail(error, error_size, "party species out of range");
        if (!b->key) b->key = derive_key(b, i, save->trainer);
        struct space s;
        space_init(&s, table);
        const struct px_pokemon_api a = px_guest_pokemon_api(&s.guest);
        /* Bind (an initial encode of the empty record), then the setters in
         * the order the population routine uses them. */
        px_bind_pokemon_data(MANAGER, DATA, EXTRA, &a);
        px_set_pokemon_field(MANAGER, PX_DATA_KEY, b->key, &a);
        px_set_pokemon_field(MANAGER, PX_DATA_WORD_10, save->trainer, &a);
        px_set_pokemon_field(MANAGER, PX_DATA_SPECIES, b->species, &a);
        px_set_pokemon_field(MANAGER, PX_DATA_EXTRA_LEVEL, b->level, &a);
        px_set_pokemon_field(MANAGER, PX_DATA_NATURE, b->nature, &a);
        px_set_pokemon_field(MANAGER, PX_DATA_STAT_24, b->iv[0], &a);
        px_set_pokemon_field(MANAGER, PX_DATA_STAT_26, b->iv[1], &a);
        px_set_pokemon_field(MANAGER, PX_DATA_STAT_28, b->iv[2], &a);
        px_set_pokemon_field(MANAGER, PX_DATA_STAT_2A, b->iv[4], &a);
        px_set_pokemon_field(MANAGER, PX_DATA_STAT_2C, b->iv[5], &a);
        px_set_pokemon_field(MANAGER, PX_DATA_STAT_2E, b->iv[3], &a);
        px_set_pokemon_field(MANAGER, PX_DATA_ABILITY, d->personal[b->species].abilities[0], &a);
        px_set_pokemon_field(MANAGER, PX_DATA_ABILITY_FLAG_0, 1, &a);
        px_set_pokemon_field(MANAGER, PX_DATA_GROWTH, b->exp, &a);
        /* Moves, PP and the party values: decoded, written, encoded again. */
        px_decode_pokemon_manager(MANAGER, &a);
        const uint32_t block_b = px_pokemon_block(MANAGER, 1, 1, &a);
        for (unsigned k = 0; k < 4; ++k) {
            store16(&s, block_b + 0x1a + 2 * k, b->moves[k]);
            store8(&s, block_b + 0x22 + k, b->pp[k]);
        }
        store16(&s, EXTRA + 8, b->hp);
        for (unsigned k = 0; k < 6; ++k) store16(&s, EXTRA + 0x0a + 2 * k, b->stats[k]);
        px_encode_pokemon_manager(MANAGER, &a);
        if (s.guest.fault) return fail(error, error_size, "codec accessed memory outside the record");
        memcpy(out + PX_SAVE_HEADER + i * PX_SAVE_SLOT, s.data, sizeof s.data);
        memcpy(out + PX_SAVE_HEADER + i * PX_SAVE_SLOT + sizeof s.data, s.extra, sizeof s.extra);
    }
    return 1;
}

int px_save_decode(const uint8_t *in, size_t size, const struct px_battle_data *d, const uint8_t table[128],
                   struct px_save *save, char *error, size_t error_size)
{
    if (!in || !d || !table || !save) return fail(error, error_size, "invalid save arguments");
    memset(save, 0, sizeof *save);
    if (size != PX_SAVE_SIZE || memcmp(in, "PXSV", 4)) return fail(error, error_size, "not a viewer save file");
    if (get16(in + 4) != 1) return fail(error, error_size, "unsupported save version");
    save->party_count = get16(in + 6);
    if (!save->party_count || save->party_count > 6) return fail(error, error_size, "party of 1..6 required");
    save->zone = get32(in + 8);
    save->world_x = (int32_t)get32(in + 12); save->world_z = (int32_t)get32(in + 16);
    if (in[20] > 3 || in[21] || in[22] || in[23]) return fail(error, error_size, "invalid facing");
    save->facing = in[20];
    save->balls = get32(in + 24); save->rng = get32(in + 28); save->trainer = get32(in + 32);
    for (uint32_t i = 0; i < save->party_count; ++i) {
        struct space s;
        space_init(&s, table);
        memcpy(s.data, in + PX_SAVE_HEADER + i * PX_SAVE_SLOT, sizeof s.data);
        memcpy(s.extra, in + PX_SAVE_HEADER + i * PX_SAVE_SLOT + sizeof s.data, sizeof s.extra);
        const struct px_pokemon_api a = px_guest_pokemon_api(&s.guest);
        /* An encoded record bound as it is stored: manager +8 data, +4 the
         * extension, +0x0c set (encoded), then decode (which checks). */
        store32(&s, MANAGER + 8, DATA); store32(&s, MANAGER + 4, EXTRA);
        store8(&s, MANAGER + 0x0c, 1); store8(&s, MANAGER + 0x0d, 0);
        px_decode_pokemon_manager(MANAGER, &a);
        if (load16(&s, DATA + 4) & 4) return fail(error, error_size, "a party record fails its checksum");
        const uint32_t block_a = px_pokemon_block(MANAGER, 0, 0, &a), block_b = px_pokemon_block(MANAGER, 1, 0, &a);
        uint16_t moves[4];
        uint8_t iv[6];
        const uint32_t ivs = load32(&s, block_b + 0x34);
        iv[0] = ivs & 31; iv[1] = (ivs >> 5) & 31; iv[2] = (ivs >> 10) & 31;
        iv[3] = (ivs >> 15) & 31; iv[4] = (ivs >> 20) & 31; iv[5] = (ivs >> 25) & 31;
        for (unsigned k = 0; k < 4; ++k) moves[k] = load16(&s, block_b + 0x1a + 2 * k);
        struct px_battler *b = &save->party[i];
        const uint16_t species = load16(&s, block_a);
        if (!px_battler_make(d, species, s.extra[4], iv, (uint8_t)load8(&s, block_a + 0x14), moves, b) ||
            s.guest.fault) return fail(error, error_size, "a party record does not form a Pokémon");
        b->exp = load32(&s, block_a + 8);
        b->key = load32(&s, DATA);
        for (unsigned k = 0; k < 4; ++k) {
            const uint8_t pp = load8(&s, block_b + 0x22 + k);
            if (pp > b->pp[k]) return fail(error, error_size, "a party record holds more PP than its move");
            b->pp[k] = pp;
        }
        for (unsigned k = 0; k < 6; ++k)
            if (load16(&s, EXTRA + 0x0a + 2 * k) != b->stats[k])
                return fail(error, error_size, "a party record's stats disagree with its species, level and IVs");
        const uint16_t hp = load16(&s, EXTRA + 8);
        if (hp > b->stats[0]) return fail(error, error_size, "a party record holds more HP than its maximum");
        b->hp = hp;
    }
    return 1;
}
