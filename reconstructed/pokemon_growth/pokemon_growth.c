#include "pokemon_growth.h"
#include "../pokemon_personal/pokemon_personal.h"

static void copy_words(uint32_t to, uint32_t from, uint32_t count, const struct px_pokemon_api *m)
{
    uint32_t words[8];
    for (uint32_t i = 0; i < count; ++i) words[i] = m->load32(m->user, from + i * UINT32_C(4));
    for (uint32_t i = 0; i < count; ++i) m->store32(m->user, to + i * UINT32_C(4), words[i]);
}

void px_copy_growth_resource(uint32_t source, uint32_t destination, const struct px_pokemon_api *m)
{
    /* 00144f34 with length 0x194: twelve groups of eight word loads followed
     * by eight word stores, then a four-word group and one final word. */
    uint32_t from = source + UINT32_C(8), to = destination + UINT32_C(8);
    for (uint32_t group = 0; group < 12; ++group, from += 32, to += 32) copy_words(to, from, 8, m);
    copy_words(to, from, 4, m);
    copy_words(to + UINT32_C(16), from + UINT32_C(16), 1, m);
    m->store32(m->user, destination + UINT32_C(4), m->load32(m->user, source + UINT32_C(4)));
}

uint32_t px_find_cached_growth_resource(uint32_t cache, uint32_t rate,
                                        uint32_t destination, const struct px_pokemon_api *m)
{
    if (m->load8(m->user, cache + UINT32_C(8)) == 0) return 0;
    uint32_t index = 0;
    do {
        uint32_t table = m->load32(m->user, cache + UINT32_C(4));
        uint32_t record = m->load32(m->user, table + index * UINT32_C(4));
        if (m->load32(m->user, record + UINT32_C(4)) == rate) {
            table = m->load32(m->user, cache + UINT32_C(4));
            record = m->load32(m->user, table + index * UINT32_C(4));
            px_copy_growth_resource(record, destination, m);
            return 1;
        }
        ++index;
    } while (m->load8(m->user, cache + UINT32_C(8)) > index);
    return 0;
}

void px_store_cached_growth_resource(uint32_t cache, uint32_t source, const struct px_pokemon_api *m)
{
    const uint32_t rate = m->load32(m->user, source + UINT32_C(4));
    uint32_t index = 0;
    if (m->load8(m->user, cache + UINT32_C(8)) != 0) {
        do {
            const uint32_t table = m->load32(m->user, cache + UINT32_C(4));
            const uint32_t record = m->load32(m->user, table + index * UINT32_C(4));
            if (m->load32(m->user, record + UINT32_C(4)) == rate) return;
            ++index;
        } while (m->load8(m->user, cache + UINT32_C(8)) > index);
    }
    const uint32_t table = m->load32(m->user, cache + UINT32_C(4));
    const uint32_t cursor = m->load8(m->user, cache + UINT32_C(9));
    px_copy_growth_resource(source, m->load32(m->user, table + cursor * UINT32_C(4)), m);
    const uint32_t next = (uint32_t)m->load8(m->user, cache + UINT32_C(9)) + UINT32_C(1);
    const uint32_t capacity = m->load8(m->user, cache + UINT32_C(8));
    m->store8(m->user, cache + UINT32_C(9), (uint8_t)(next % capacity));
}

void px_load_growth_resource(uint32_t record, uint32_t rate, const struct px_growth_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    if (m->load32(m->user, record + UINT32_C(4)) == rate) return;
    const uint32_t archive = m->load32(m->user, UINT32_C(0x005e52b0));
    api->read_member(api->user, archive, rate, record + UINT32_C(8));
    m->store32(m->user, record + UINT32_C(4), rate);
}

void px_select_growth_resource(uint32_t species, uint32_t form, const struct px_growth_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    const uint32_t descriptor = m->load32(m->user, UINT32_C(0x005d17a8));
    const uint32_t record = m->load32(m->user, UINT32_C(0x005d17ac));
    px_load_personal_record(descriptor, species, form, m);
    const uint32_t rate = px_personal_resource_field(m->load32(m->user, UINT32_C(0x005d17a8)),
                                                     PX_GROWTH_RATE_SELECTOR, m);
    if (px_find_cached_growth_resource(m->load32(m->user, UINT32_C(0x005d17b0)), rate, record, m)) return;
    px_load_growth_resource(record, rate, api);
    px_store_cached_growth_resource(m->load32(m->user, UINT32_C(0x005d17b0)), record, m);
}
