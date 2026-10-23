#include "state_fields.h"

/* These helpers use the concrete sixth-milestone codec and selectors. The
 * original value is captured before encoding, including getter return values. */
uint32_t px_read_pokemon_state_value(uint32_t manager, enum px_state_value field,
                                     const struct px_pokemon_api *memory)
{
    if (field != PX_STATE_SPECIES && field != PX_STATE_FORM &&
        field != PX_STATE_GROWTH && field != PX_STATE_MAX_HP) return 0;
    if (field == PX_STATE_MAX_HP &&
        memory->load32(memory->user, manager + UINT32_C(4)) == 0) return 0;

    px_decode_pokemon_manager(manager, memory);
    uint32_t value;
    if (field == PX_STATE_MAX_HP) { /* 00152f54 */
        const uint32_t data = memory->load32(memory->user, manager + UINT32_C(8));
        const uint16_t flags = memory->load16(memory->user, data + UINT32_C(4));
        const uint32_t extra = (flags & UINT16_C(4)) != 0 ? UINT32_C(0x00648284) :
            memory->load32(memory->user, manager + UINT32_C(4));
        value = memory->load16(memory->user, extra + UINT32_C(0x0a));
    } else {
        const uint32_t block = px_pokemon_block(manager, 0, 0, memory);
        if (field == PX_STATE_SPECIES) /* 00152fe0 */
            value = memory->load16(memory->user, block);
        else if (field == PX_STATE_FORM) /* 00152fa8 */
            value = memory->load8(memory->user, block + UINT32_C(0x15)) >> 3;
        else /* 0016b45c */
            value = memory->load32(memory->user, block + UINT32_C(8));
    }
    px_encode_pokemon_manager(manager, memory);
    return value;
}

void px_write_pokemon_state_value(uint32_t manager, enum px_state_value field,
                                   uint32_t value, const struct px_pokemon_api *memory)
{
    if (field != PX_STATE_CURRENT_HP && field != PX_STATE_STATUS &&
        field != PX_STATE_NAME_FLAG) return;
    if (field != PX_STATE_NAME_FLAG &&
        memory->load32(memory->user, manager + UINT32_C(4)) == 0) return;

    px_decode_pokemon_manager(manager, memory);
    if (field == PX_STATE_NAME_FLAG) { /* 0014f8cc */
        const uint32_t block = px_pokemon_block(manager, 1, 1, memory);
        const uint32_t old = memory->load32(memory->user, block + UINT32_C(0x34));
        memory->store32(memory->user, block + UINT32_C(0x34),
                        (old & UINT32_C(0x7fffffff)) | (value << 31));
    } else {
        const uint32_t data = memory->load32(memory->user, manager + UINT32_C(8));
        const uint16_t flags = memory->load16(memory->user, data + UINT32_C(4));
        const uint32_t extra = (flags & UINT16_C(4)) != 0 ? UINT32_C(0x00648380) :
            memory->load32(memory->user, manager + UINT32_C(4));
        if (field == PX_STATE_CURRENT_HP) /* 0014f930 */
            memory->store16(memory->user, extra + UINT32_C(8), (uint16_t)value);
        else /* 0014fcb4 */
            memory->store32(memory->user, extra, value);
    }
    px_encode_pokemon_manager(manager, memory);
}

uint32_t px_read_pokemon_state_slot(uint32_t manager, uint32_t slot,
                                    enum px_state_slot field,
                                    const struct px_pokemon_api *memory)
{
    if (slot >= UINT32_C(4) || (field != PX_STATE_MOVE &&
        field != PX_STATE_PP_UPS && field != PX_STATE_PP)) return 0;
    px_decode_pokemon_manager(manager, memory);
    const uint32_t block = px_pokemon_block(manager, 1, 0, memory);
    uint32_t value;
    if (field == PX_STATE_MOVE) /* 00153014 */
        value = memory->load16(memory->user, block + UINT32_C(0x1a) + 2 * slot);
    else if (field == PX_STATE_PP_UPS) /* 00152e50 */
        value = memory->load8(memory->user, block + UINT32_C(0x26) + slot);
    else /* 00152efc */
        value = memory->load8(memory->user, block + UINT32_C(0x22) + slot);
    px_encode_pokemon_manager(manager, memory);
    return value;
}

void px_write_pokemon_state_slot(uint32_t manager, uint32_t slot,
                                  enum px_state_slot field, uint32_t value,
                                  const struct px_pokemon_api *memory)
{
    if (slot >= UINT32_C(4) || (field != PX_STATE_MOVE &&
        field != PX_STATE_PP_UPS && field != PX_STATE_PP)) return;
    px_decode_pokemon_manager(manager, memory);
    const uint32_t block = px_pokemon_block(manager, 1, 1, memory);
    if (field == PX_STATE_MOVE) /* 00168824 */
        memory->store16(memory->user, block + UINT32_C(0x1a) + 2 * slot, (uint16_t)value);
    else if (field == PX_STATE_PP_UPS) /* 001684f4 */
        memory->store8(memory->user, block + UINT32_C(0x26) + slot, (uint8_t)value);
    else /* 0014f9d8 */
        memory->store8(memory->user, block + UINT32_C(0x22) + slot, (uint8_t)value);
    px_encode_pokemon_manager(manager, memory);
}
