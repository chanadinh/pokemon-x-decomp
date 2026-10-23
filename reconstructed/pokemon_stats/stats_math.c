#include "stats_math.h"

static uint32_t scaled_high(uint32_t value)
{
    /* Exact UMULL high half, not a substitution with integer division. */
    return (uint32_t)(((uint64_t)value * UINT64_C(0x028f5c29)) >> 32);
}

uint32_t px_pokemon_hp_value(uint32_t species, uint32_t level, uint32_t base,
                             uint32_t iv, uint32_t ev)
{
    /* 00168b84: caller narrowing is separate from this full-width interface. */
    if (species == UINT32_C(0x124)) return 1;
    const uint32_t sum = iv + (base << 1) + (ev >> 2);
    const uint32_t product = sum * level;
    return (uint16_t)(scaled_high(product) + level + UINT32_C(10));
}

uint32_t px_apply_pokemon_nature(uint32_t value, uint32_t nature, uint32_t stat_index,
                                 const struct px_pokemon_api *m)
{
    /* 001689fc, also exported inside 00150034's body. The full nature word
     * selects logical memory without a clamp, and only signed bytes +/-1 act. */
    if (stat_index - UINT32_C(1) >= UINT32_C(5)) return value;
    const uint32_t address = UINT32_C(0x00570f6c) + nature * UINT32_C(5) +
        stat_index - UINT32_C(1);
    const uint8_t adjustment = m->load8(m->user, address);
    if (adjustment != UINT8_C(1) && adjustment != UINT8_MAX) return value;
    const uint32_t factor = adjustment == 1 ? UINT32_C(110) : UINT32_C(90);
    const uint32_t truncated = (value * factor) & UINT32_C(0xffff);
    const uint32_t rounded = truncated * UINT32_C(0xa3d7) + UINT32_C(0xa3d7);
    return rounded >> 22;
}

uint32_t px_pokemon_stat_value(uint32_t level, uint32_t base, uint32_t iv,
                               uint32_t ev, uint32_t nature, enum px_stat stat,
                               const struct px_pokemon_api *m)
{
    /* The five scalar entry points: 14ffa4/14ffd4/14ff74/150004/150034. */
    if (stat < PX_STAT_ATTACK || stat > PX_STAT_SPECIAL_DEFENSE) return 0;
    static const uint32_t columns[] = {0, 1, 2, 5, 3, 4};
    const uint32_t sum = iv + (base << 1) + (ev >> 2);
    const uint32_t product = sum * level;
    const uint32_t value = (uint16_t)(scaled_high(product) + UINT32_C(5));
    return px_apply_pokemon_nature(value, nature, columns[stat], m);
}

uint32_t px_pokemon_level_from_growth(uint32_t species, uint32_t form, uint32_t growth,
                                      const struct px_data_api *api)
{
    /* 00168b3c captures growth before resource selection and queries ascending
     * levels 1..100, stopping at the first unsigned threshold above growth. */
    api->select_growth(api->user, species, form);
    uint32_t level = 1;
    while (level <= UINT32_C(100)) {
        if (px_pokemon_growth_value(level, api) > growth) break;
        ++level;
    }
    return level > 1 ? level - UINT32_C(1) : UINT32_C(1);
}
