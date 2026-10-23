#include "stats_fields.h"

uint32_t px_read_pokemon_iv(uint32_t manager, enum px_stat stat,
                            const struct px_pokemon_api *m)
{
    /* HP/Atk/Def/Speed/SpAtk/SpDef: 16b318/152d20/152d54/152cec/152de0/152e14. */
    if ((unsigned)stat > PX_STAT_SPECIAL_DEFENSE) return 0;
    px_decode_pokemon_manager(manager, m);
    const uint32_t block = px_pokemon_block(manager, 1, 0, m);
    const uint32_t packed = m->load32(m->user, block + UINT32_C(0x34));
    const uint32_t value = (packed >> (5 * (unsigned)stat)) & UINT32_C(31);
    px_encode_pokemon_manager(manager, m);
    return value;
}

uint32_t px_read_pokemon_ev(uint32_t manager, enum px_stat stat,
                            const struct px_pokemon_api *m)
{
    /* HP/Atk/Def/Speed/SpAtk/SpDef: 16b2ec/152c94/152cc0/152c68/152d88/152db4. */
    if ((unsigned)stat > PX_STAT_SPECIAL_DEFENSE) return 0;
    px_decode_pokemon_manager(manager, m);
    const uint32_t block = px_pokemon_block(manager, 0, 0, m);
    const uint32_t value = m->load8(m->user, block + UINT32_C(0x16) + (uint32_t)stat);
    px_encode_pokemon_manager(manager, m);
    return value;
}

uint32_t px_read_pokemon_nature(uint32_t manager, const struct px_pokemon_api *m)
{
    /* 00152c3c */
    px_decode_pokemon_manager(manager, m);
    const uint32_t block = px_pokemon_block(manager, 0, 0, m);
    const uint32_t value = m->load8(m->user, block + UINT32_C(0x14));
    px_encode_pokemon_manager(manager, m);
    return value;
}

uint32_t px_pokemon_has_extra(uint32_t manager, const struct px_pokemon_api *m)
{
    /* 0016b348: no decoding. */
    return m->load32(m->user, manager + UINT32_C(4)) != 0;
}

static uint32_t read_extra(uint32_t manager, uint32_t offset, int byte,
                            const struct px_pokemon_api *m)
{
    if (m->load32(m->user, manager + UINT32_C(4)) == 0) return 0;
    px_decode_pokemon_manager(manager, m);
    const uint32_t data = m->load32(m->user, manager + UINT32_C(8));
    const uint16_t flags = m->load16(m->user, data + UINT32_C(4));
    const uint32_t extra = (flags & UINT16_C(4)) != 0 ? UINT32_C(0x00648284) :
        m->load32(m->user, manager + UINT32_C(4));
    const uint32_t value = byte ? m->load8(m->user, extra + offset) :
                                 m->load16(m->user, extra + offset);
    px_encode_pokemon_manager(manager, m);
    return value;
}

uint32_t px_read_pokemon_current_hp(uint32_t manager, const struct px_pokemon_api *m)
{
    return read_extra(manager, UINT32_C(8), 0, m); /* 00152ea8 */
}

uint32_t px_read_pokemon_extra_level(uint32_t manager, const struct px_pokemon_api *m)
{
    return read_extra(manager, UINT32_C(4), 1, m); /* 00176268 */
}

void px_write_pokemon_stat(uint32_t manager, enum px_stat stat, uint32_t value,
                            const struct px_pokemon_api *m)
{
    /* HP: 0014fd54. Others: inline tails in 14fb00/fbd4/fa2c/fda4/fe78. */
    if ((unsigned)stat > PX_STAT_SPECIAL_DEFENSE) return;
    if (m->load32(m->user, manager + UINT32_C(4)) == 0) return;
    px_decode_pokemon_manager(manager, m);
    const uint32_t data = m->load32(m->user, manager + UINT32_C(8));
    const uint16_t flags = m->load16(m->user, data + UINT32_C(4));
    const uint32_t extra = (flags & UINT16_C(4)) != 0 ? UINT32_C(0x00648380) :
        m->load32(m->user, manager + UINT32_C(4));
    m->store16(m->user, extra + UINT32_C(0x0a) + 2 * (uint32_t)stat, (uint16_t)value);
    px_encode_pokemon_manager(manager, m);
}
