#include "pokemon_data.h"

static void set_byte(uint32_t manager, uint32_t block, uint32_t write_mode,
                     uint32_t offset, uint32_t value,
                     const struct px_pokemon_api *memory)
{
    const uint32_t address = px_pokemon_block(manager, block, write_mode, memory);
    memory->store8(memory->user, address + offset, (uint8_t)value);
}

static void replace_byte_bits(uint32_t address, uint32_t mask, uint32_t bits,
                              const struct px_pokemon_api *memory)
{
    const uint32_t old = memory->load8(memory->user, address);
    memory->store8(memory->user, address, (uint8_t)((old & ~mask) | bits));
}

static void set_stat(uint32_t manager, uint32_t shift, uint32_t value,
                     const struct px_pokemon_api *memory)
{
    const uint32_t address = px_pokemon_block(manager, 1, 1, memory) + UINT32_C(0x34);
    const uint32_t old = memory->load32(memory->user, address);
    const uint32_t mask = UINT32_C(0x1f) << shift;
    memory->store32(memory->user, address,
                    (old & ~mask) | ((value << shift) & mask));
}

void px_set_pokemon_field(uint32_t manager, enum px_pokemon_field field,
                          uint32_t value, const struct px_pokemon_api *memory)
{
    uint32_t address;

    /* Dispatch is a host convenience: invalid enum values have no ARM setter. */
    if ((unsigned)field > (unsigned)PX_DATA_GROWTH) return;

    /* 0x0014fd04 tests the extra pointer before entering the decode path. */
    if (field == PX_DATA_EXTRA_LEVEL &&
        memory->load32(memory->user, manager + UINT32_C(4)) == 0)
        return;

    px_decode_pokemon_manager(manager, memory);
    switch (field) {
    case PX_DATA_KEY: /* 0x00168360 */
        address = memory->load32(memory->user, manager + UINT32_C(8));
        memory->store32(memory->user, address, value);
        break;
    case PX_DATA_WORD_10: /* 0x001686e8 */
        address = px_pokemon_block(manager, 0, 1, memory);
        memory->store32(memory->user, address + UINT32_C(4), value);
        break;
    case PX_DATA_WORD_08: /* 0x00168028 */
        address = px_pokemon_block(manager, 0, 1, memory);
        memory->store32(memory->user, address + UINT32_C(0x10), value);
        break;
    case PX_DATA_SPECIES: /* 0x001687f8 */
        address = px_pokemon_block(manager, 0, 1, memory);
        memory->store16(memory->user, address, (uint16_t)value);
        break;
    case PX_DATA_FORM: /* 0x00168780 */
        address = px_pokemon_block(manager, 0, 1, memory);
        replace_byte_bits(address + UINT32_C(0x15), UINT32_C(0xf8),
                           (value << 3) & UINT32_C(0xf8), memory);
        break;
    case PX_DATA_EXTRA_LEVEL: /* 0x0014fd04 */
        address = memory->load32(memory->user, manager + UINT32_C(8));
        if ((memory->load16(memory->user, address + UINT32_C(4)) & UINT16_C(4)) != 0)
            address = UINT32_C(0x00648380);
        else
            address = memory->load32(memory->user, manager + UINT32_C(4));
        memory->store8(memory->user, address + UINT32_C(4), (uint8_t)value);
        break;
    case PX_DATA_NATURE: /* 0x00167ffc */
        set_byte(manager, 0, 1, UINT32_C(0x14), value, memory);
        break;
    case PX_DATA_GENDER: /* 0x00168740 */
        address = px_pokemon_block(manager, 0, 1, memory);
        replace_byte_bits(address + UINT32_C(0x15), UINT32_C(6),
                           (value << 1) & UINT32_C(6), memory);
        break;
    case PX_DATA_LEVEL: /* 0x00168054 */
        address = px_pokemon_block(manager, 3, 1, memory);
        replace_byte_bits(address + UINT32_C(0x2d), UINT32_C(0x7f),
                           value & UINT32_C(0x7f), memory);
        break;
    case PX_DATA_ZERO_BYTE: /* 0x00168568 */
        set_byte(manager, 3, 1, UINT32_C(0x2f), value, memory);
        break;
    case PX_DATA_PARAM_30: /* 0x0016830c */
        address = px_pokemon_block(manager, 2, 0, memory);
        if (memory->load8(memory->user, address + UINT32_C(0x1b)) != 0)
            set_byte(manager, 2, 1, UINT32_C(0x2a), value, memory);
        else
            set_byte(manager, 3, 1, UINT32_C(0x1a), value, memory);
        break;
    case PX_DATA_SETTING_68: /* 0x001687cc */
        set_byte(manager, 3, 0, UINT32_C(0x33), value, memory);
        break;
    case PX_DATA_SETTING_69: /* 0x001682a4 */
        set_byte(manager, 3, 1, UINT32_C(0x30), value, memory);
        break;
    case PX_DATA_SETTING_6A: /* 0x00168594 */
        set_byte(manager, 3, 0, UINT32_C(0x31), value, memory);
        break;
    case PX_DATA_SETTING_6B: /* 0x00168404 */
        set_byte(manager, 3, 0, UINT32_C(0x32), value, memory);
        break;
    case PX_DATA_STAT_24: /* 0x00168090 */
        set_stat(manager, 0, value, memory);
        break;
    case PX_DATA_STAT_26: /* 0x0016810c */
        set_stat(manager, 5, value, memory);
        break;
    case PX_DATA_STAT_28: /* 0x0016814c */
        set_stat(manager, 10, value, memory);
        break;
    case PX_DATA_STAT_2A: /* 0x00168384 */
        set_stat(manager, 20, value, memory);
        break;
    case PX_DATA_STAT_2C: /* 0x001683c4 */
        set_stat(manager, 25, value, memory);
        break;
    case PX_DATA_STAT_2E: /* 0x001680cc */
        set_stat(manager, 15, value, memory);
        break;
    case PX_DATA_ABILITY: /* 0x00168278 */
        set_byte(manager, 0, 1, UINT32_C(0x0c), value, memory);
        break;
    case PX_DATA_ABILITY_FLAG_0: /* 0x00168430 */
        address = px_pokemon_block(manager, 0, 1, memory);
        /* These three setters intentionally do not mask the input to one bit. */
        replace_byte_bits(address + UINT32_C(0x0d), UINT32_C(1), value, memory);
        break;
    case PX_DATA_ABILITY_FLAG_1: /* 0x00168468 */
        address = px_pokemon_block(manager, 0, 1, memory);
        replace_byte_bits(address + UINT32_C(0x0d), UINT32_C(2), value << 1, memory);
        break;
    case PX_DATA_ABILITY_FLAG_2: /* 0x001684a0 */
        address = px_pokemon_block(manager, 0, 1, memory);
        replace_byte_bits(address + UINT32_C(0x0d), UINT32_C(4), value << 2, memory);
        break;
    case PX_DATA_GROWTH: /* 0x00168714 */
        address = px_pokemon_block(manager, 0, 1, memory);
        memory->store32(memory->user, address + UINT32_C(8), value);
        break;
    }
    px_encode_pokemon_manager(manager, memory);
}
