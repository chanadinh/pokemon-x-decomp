#include "save.h"

#include <stdio.h>
#include <string.h>

#include "../../reconstructed/pokemon_data/pokemon_data.h"

/* The reconstructed codec reaches memory through logical ARM addresses; this
 * adapter maps the manager, one record, its extension and the block-order
 * table. Any other access is an error. */
enum { MANAGER = 0x10000000, DATA = 0x10000100, EXTRA = 0x10000200, TABLE = 0x00570eec };
struct space {
    uint8_t manager[0x10], data[0xe8], extra[0x1c];
    const uint8_t *table;
    int fault;
};
static uint8_t *at(struct space *s, uint32_t address, uint32_t width)
{
    if (address >= MANAGER && address + width <= MANAGER + sizeof s->manager) return s->manager + (address - MANAGER);
    if (address >= DATA && address + width <= DATA + sizeof s->data) return s->data + (address - DATA);
    if (address >= EXTRA && address + width <= EXTRA + sizeof s->extra) return s->extra + (address - EXTRA);
    s->fault = 1;
    return NULL;
}
static uint8_t load8(void *user, uint32_t address)
{
    struct space *s = user;
    if (address >= TABLE && address < TABLE + 128) return s->table[address - TABLE];
    const uint8_t *p = at(s, address, 1);
    return p ? p[0] : 0;
}
static uint16_t load16(void *user, uint32_t address)
{
    const uint8_t *p = at(user, address, 2);
    return p ? (uint16_t)(p[0] | p[1] << 8) : 0;
}
static uint32_t load32(void *user, uint32_t address)
{
    const uint8_t *p = at(user, address, 4);
    return p ? (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24 : 0;
}
static void store8(void *user, uint32_t address, uint8_t v)
{
    uint8_t *p = at(user, address, 1);
    if (p) p[0] = v;
}
static void store16(void *user, uint32_t address, uint16_t v)
{
    uint8_t *p = at(user, address, 2);
    if (p) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); }
}
static void store32(void *user, uint32_t address, uint32_t v)
{
    uint8_t *p = at(user, address, 4);
    if (p) for (unsigned k = 0; k < 4; ++k) p[k] = (uint8_t)(v >> (8 * k));
}
static struct px_pokemon_api api_for(struct space *s)
{
    struct px_pokemon_api a;
    memset(&a, 0, sizeof a);
    a.user = s;
    a.load8 = load8; a.load16 = load16; a.load32 = load32;
    a.store8 = store8; a.store16 = store16; a.store32 = store32;
    return a;
}
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
        memset(&s, 0, sizeof s);
        s.table = table;
        const struct px_pokemon_api a = api_for(&s);
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
        if (s.fault) return fail(error, error_size, "codec accessed memory outside the record");
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
        memset(&s, 0, sizeof s);
        s.table = table;
        memcpy(s.data, in + PX_SAVE_HEADER + i * PX_SAVE_SLOT, sizeof s.data);
        memcpy(s.extra, in + PX_SAVE_HEADER + i * PX_SAVE_SLOT + sizeof s.data, sizeof s.extra);
        const struct px_pokemon_api a = api_for(&s);
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
            s.fault) return fail(error, error_size, "a party record does not form a Pokémon");
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
