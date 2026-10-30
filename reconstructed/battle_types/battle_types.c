#include "battle_types.h"

uint32_t px_battle_type_effectiveness(uint32_t attack_type, uint32_t defend_type,
                                      const struct px_battle_types_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    if (attack_type == PX_BATTLE_TYPELESS || defend_type == PX_BATTLE_TYPELESS) return PX_BATTLE_NEUTRAL;
    const uint8_t value = m->load8(m->user, api->chart + attack_type * UINT32_C(18) + defend_type);
    const uint8_t raw = m->load8(m->user, api->rules + UINT32_C(1));
    const int inverse = raw != 0;   /* ldrsb; only zero/nonzero is tested */
    switch (value) {
    case 0: return inverse ? 8 : 0;
    case 2: return inverse ? 8 : 6;
    case 4: return 7;
    case 8: return inverse ? 6 : 8;
    default: api->abort(api->user); return 0;
    }
}

uint32_t px_battle_combine_effectiveness(uint32_t first, uint32_t second,
                                         const struct px_battle_types_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    if (first >= 0xe) api->abort(api->user);
    if (second >= 0xe) api->abort(api->user);
    const uint32_t a = m->load32(m->user, api->table + first * UINT32_C(4));
    const uint32_t b = m->load32(m->user, api->table + second * UINT32_C(4));
    uint32_t product = (b * a) >> 6;
    if (product > 0x1000) {
        api->abort(api->user);
        product = 0x1000;
    } else if (product == 0 && first != 0 && second != 0) {
        api->abort(api->user);
        product = 1;
    }
    /* Index of the lowest set bit plus one; zero when no bit is set. */
    uint32_t code = 0;
    for (uint32_t bit = 0; bit < 0x20; ++bit) {
        if (product & (UINT32_C(1) << bit)) {
            code = bit + 1;
            break;
        }
    }
    if (code >= 0xe) {
        api->abort(api->user);
        code = 0xd;
    }
    return code & UINT32_C(0xff);
}

uint32_t px_battle_apply_effectiveness(uint32_t damage, uint32_t code,
                                       const struct px_battle_types_api *api)
{
    if (code >= 0xe) {
        api->abort(api->user);
        return damage;
    }
    if (code == 0) return 0;
    if (code < 7) return damage >> (7 - code);
    return damage << (code - 7);
}
