#include "battle_mid_1.h"

static void *user(const struct px_battle_mid_1_api *api)
{
    return api->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_mid_1_api *api)
{
    return api->events->memory;
}

static uint32_t load8(uint32_t at, const struct px_battle_mid_1_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load8(m->user, at);
}

static uint32_t load16(uint32_t at, const struct px_battle_mid_1_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load16(m->user, at);
}

static uint32_t load32(uint32_t at, const struct px_battle_mid_1_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    return m->load32(m->user, at);
}

static void store16(uint32_t at, uint32_t value, const struct px_battle_mid_1_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store16(m->user, at, (uint16_t)value);
}

static void store32(uint32_t at, uint32_t value, const struct px_battle_mid_1_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store32(m->user, at, value);
}

/* Appends a word to the record's list while it holds fewer than 8 (count in header bits 7-13). */
static void append(uint32_t out, uint32_t word, const struct px_battle_mid_1_api *api)
{
    const uint32_t header = load16(out + 2, api);
    const uint32_t count = header >> 7 & 0x7f;
    if (count >= 8) return;
    store16(out + 2, header + 0x80, api);
    store32(out + 4 + 4 * count, word, api);
}

uint32_t px_battle_mid_effect_record(uint32_t kind, uint32_t argument, uint32_t pokemon, uint32_t value, uint32_t out,
                                     const struct px_battle_mid_1_api *api)
{
    const int second = value != 0;
    uint32_t i = 0;
    for (; i < 0x11; ++i) {
        if (load16(api->effect_table + 6 * i, api) == kind) break;
    }
    if (i == 0x11) return 0;
    const uint32_t entry = api->effect_table + 6 * i;
    int32_t id = (int16_t)load16(entry + (second ? 4 : 2), api);
    if (id < 0) id = (int16_t)load16(entry + 2, api);
    if (id < 0) return 0;
    store16(out, (uint32_t)id, api);
    store16(out + 2, (load16(out + 2, api) & 0xc000) | 2, api);
    append(out, load8(pokemon + 0x19, api), api);
    if (second) append(out, value, api);
    if (kind == 8) append(out, api->word_owner(user(api), argument), api);
    else if (kind == 0x12) append(out, 0x49, api);
    return 1;
}

void px_battle_mid_pending_flags(uint32_t object, const struct px_battle_mid_1_api *api)
{
    const uint32_t first = load32(object + 0x60, api);
    if ((first & 7) != 0) {
        const uint32_t owner = api->word_owner(user(api), first) & 0xff;
        if (owner == (px_battle_event_get(0x15, api->events) & 0xff)) {
            const uint32_t kind = first & 7;
            uint32_t flag = 0;
            int known = 1;
            if (kind == 1 || kind == 3 || kind == 2) flag = (first & 0x2000000) >> 25;
            else if (kind == 4) flag = first >> 31;
            else known = 0;
            if (known && flag != 0) px_battle_event_rewrite(0x4d, 1, api->events);
            else px_battle_event_rewrite(0x4c, 1, api->events);
        }
    }
    if ((load32(object + 0x70, api) & 7) != 0 && px_battle_event_get(0x16, api->events) == 4)
        px_battle_event_rewrite(0x4c, 1, api->events);
    if ((load32(object + 0x98, api) & 7) != 0 && px_battle_event_get(0x16, api->events) == 4)
        px_battle_event_rewrite(0x4c, 1, api->events);
}

uint32_t px_battle_mid_free_slots(uint32_t context, uint32_t out, const struct px_battle_mid_1_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    uint32_t count = 0;
    for (uint32_t i = 0; i < 0x12; ++i) {
        const uint32_t status = api->slot_status(user(api), context, i);
        if (status == 6 || status == 0) {
            m->store8(m->user, out + count, (uint8_t)i);
            count = (count + 1) & 0xff;
        }
    }
    return count;
}

uint32_t px_battle_mid_scale_12(uint32_t a, uint32_t b)
{
    const uint32_t product = a * b;
    uint32_t result = product >> 12;
    if ((product & 0xfff) > 0x800) ++result;
    return result == 0 ? 1 : result;
}

uint32_t px_battle_mid_random_unused(uint32_t list, uint32_t count, const struct px_battle_mid_1_api *api)
{
    const uint32_t buffer = load32(api->state + 0x58, api);
    uint32_t found = 0;
    for (uint32_t value = 1; value < 0x26a; ++value) {
        uint32_t listed = 0;
        for (uint32_t i = 0; i < count; ++i) {
            if (load16(list + 2 * i, api) == value) {
                listed = 1;
                break;
            }
        }
        if (listed) continue;
        store16(buffer + 2 * found, value, api);
        found = (found + 1) & 0xffff;
    }
    if (found == 0) return 0;
    const uint32_t index = api->modulo(user(api), load32(api->state + 0x54, api), found) & 0xffff;
    return load16(load32(api->state + 0x58, api) + 2 * index, api);
}

uint32_t px_battle_listed_46c4(uint32_t value, const struct px_battle_mid_1_api *api)
{
    return api->in_list(user(api), value, api->list_25c6, 0x15);
}

uint32_t px_battle_listed_4714(uint32_t value, const struct px_battle_mid_1_api *api)
{
    return api->in_list(user(api), value, api->list_25b8, 7);
}

uint32_t px_battle_listed_4724(uint32_t value, const struct px_battle_mid_1_api *api)
{
    if (api->in_list(user(api), value, api->list_24fa, 0xe) != 0) return 1;
    return api->in_list(user(api), value, api->list_256e, 0x19);
}

uint32_t px_battle_listed_4764(uint32_t value, const struct px_battle_mid_1_api *api)
{
    for (uint32_t i = 0; i < 6; i += 2) {
        if (load16(api->list_146a + 2 * i, api) == value || load16(api->list_146a + 2 * i + 2, api) == value) return 1;
    }
    return 0;
}

uint32_t px_battle_listed_47b4(uint32_t value, const struct px_battle_mid_1_api *api)
{
    if (api->in_list(user(api), value, api->list_24fa, 0xe) != 0) return 1;
    return api->in_list(user(api), value, api->list_252e, 0x20);
}

uint32_t px_battle_listed_47f4(uint32_t value, const struct px_battle_mid_1_api *api)
{
    return api->in_list(user(api), value, api->list_25ac, 6);
}

uint32_t px_battle_listed_4804(uint32_t value, const struct px_battle_mid_1_api *api)
{
    return api->in_list(user(api), value, api->list_2378, 6);
}

uint32_t px_battle_listed_4814(uint32_t value, const struct px_battle_mid_1_api *api)
{
    return api->in_list(user(api), value, api->list_25f0, 7);
}

uint32_t px_battle_listed_4824(uint32_t value, const struct px_battle_mid_1_api *api)
{
    return api->in_list(user(api), value, api->list_2384, 0xb);
}

uint32_t px_battle_listed_4844(uint32_t value, const struct px_battle_mid_1_api *api)
{
    return api->in_list(user(api), value, api->list_25a0, 6);
}

uint32_t px_battle_listed_48e4(uint32_t value, const struct px_battle_mid_1_api *api)
{
    if (api->in_list(user(api), value, api->list_10f0, 2) != 0) return 1;
    return api->in_list(user(api), value, api->list_23b6, 10);
}

uint32_t px_battle_listed_4854(uint32_t value, const struct px_battle_mid_1_api *api)
{
    if (value == 0x19) return 0;
    return px_battle_listed_48e4(value, api);
}

uint32_t px_battle_mid_list_1476(uint32_t count, const struct px_battle_mid_1_api *api)
{
    store32(count, 0x36, api);
    return api->list_1476;
}
