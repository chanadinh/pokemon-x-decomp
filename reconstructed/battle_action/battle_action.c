#include "battle_action.h"

static void *user(const struct px_battle_action_api *api)
{
    return api->memory->user;
}

static uint32_t used(uint32_t header)
{
    return header >> 10 & UINT32_C(0x1ff);
}

static uint32_t with_used(uint32_t header, uint32_t bytes)
{
    return (header & ~UINT32_C(0x7fc00)) | (bytes & UINT32_C(0x1ff)) << 10;
}

uint32_t px_battle_action_push(uint32_t stack, uint32_t kind, uint32_t id, const struct px_battle_action_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    uint32_t i = 0;
    for (; i < PX_BATTLE_ACTION_KINDS; ++i)
        if (api->sizes[2 * i] == kind) break;
    uint32_t size = i < PX_BATTLE_ACTION_KINDS ? api->sizes[2 * i + 1] : 0;
    if (size != 0) {
        while (size & 3) ++size;
        const uint32_t top = used(m->load32(m->user, stack));
        if (size + top <= PX_BATTLE_ACTION_CAPACITY) {
            const uint32_t record = stack + top + 4;
            for (uint32_t k = 0; k < size; ++k) m->store8(m->user, record + k, 0);
            m->store8(m->user, record, (uint8_t)kind);
            uint32_t header = m->load32(m->user, record);
            header = (header & ~UINT32_C(0x7fe000)) | (size << 13 & UINT32_C(0x7fffff));
            header = (header & ~UINT32_C(0x1f00)) | (id << 8 & UINT32_C(0x1f00));
            header = (header & ~UINT32_C(0x800000)) | UINT32_C(0x4000000);
            m->store32(m->user, record, header);
            const uint32_t word = m->load32(m->user, stack);
            m->store32(m->user, stack, with_used(word, used(word) + size));
            return record;
        }
    }
    api->fault(user(api));
    return 0;
}

uint32_t px_battle_action_record(uint32_t context, uint32_t kind, uint32_t id, const struct px_battle_action_api *api)
{
    return px_battle_action_push(context + PX_BATTLE_ACTION_STACK, kind, id, api);
}

void px_battle_action_commit(uint32_t context, uint32_t record, const struct px_battle_action_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    api->execute(user(api), context, record);
    const uint32_t stack = context + PX_BATTLE_ACTION_STACK;
    const uint32_t size = m->load32(m->user, record) >> 13 & UINT32_C(0x3ff);
    const uint32_t word = m->load32(m->user, stack);
    /* Only the top record can be popped. */
    if (size > used(word) || record - stack - 4 + size != used(word)) {
        api->fault(user(api));
        return;
    }
    m->store32(m->user, stack, with_used(word, used(word) - size));
}

void px_battle_ability_popup(uint32_t context, uint32_t phase, uint32_t id, const struct px_battle_action_api *api)
{
    px_battle_action_commit(context, px_battle_action_record(context, phase, id, api), api);
}

void px_battle_message(uint32_t message, uint32_t kind, uint32_t id, const struct px_battle_action_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    const uint32_t flags = m->load16(m->user, message + 2);
    m->store16(m->user, message, (uint16_t)id);
    m->store16(m->user, message + 2, (uint16_t)((kind & 0x7f) | (flags & 0xc000)));
}

void px_battle_message_arg(uint32_t message, uint32_t value, const struct px_battle_action_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    const uint32_t flags = m->load16(m->user, message + 2);
    const uint32_t count = flags >> 7 & 0x7f;
    if (count >= 8) return;
    m->store16(m->user, message + 2, (uint16_t)(flags + 0x80));
    m->store32(m->user, message + 4 + 4 * count, value);
}

uint32_t px_battle_temp_buffer(uint32_t context, uint32_t size, const struct px_battle_action_api *api)
{
    if (size >= PX_BATTLE_TEMP_BUFFER_SIZE) api->fault(user(api));
    return context + PX_BATTLE_TEMP_BUFFER;
}
