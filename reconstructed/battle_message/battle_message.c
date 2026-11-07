#include "battle_message.h"

static uint32_t r16(uint32_t address, const struct px_battle_command_api *api)
{
    return api->memory->load16(api->memory->user, address);
}

static uint32_t r32(uint32_t address, const struct px_battle_command_api *api)
{
    return api->memory->load32(api->memory->user, address);
}

void px_battle_message_command(uint32_t queue, uint32_t group, uint32_t id, const uint32_t *args, unsigned count,
                               const struct px_battle_command_api *api)
{
    group &= 0xff;
    px_battle_queue_u16(queue, group, api);
    px_battle_queue_u16(queue, id & 0xffff, api);
    unsigned next = 0;
    if (group == 0x6b && next < count) px_battle_queue_u32(queue, args[next++], api);
    /* Words up to and including the terminator (one overflow check each, limit 0x176d, literal 1086b6ac). */
    while (next < count) {
        const uint32_t word = args[next++];
        px_battle_queue_u32(queue, word, api);
        if (word == PX_BATTLE_MESSAGE_END) break;
    }
}

void px_battle_message_plain(uint32_t context, uint32_t id, uint32_t count, uint32_t args, const struct px_battle_command_api *api)
{
    uint32_t list[5];
    unsigned n = 0;
    if (count >= 1 && count <= 4)
        for (uint32_t i = 0; i < count; ++i) list[n++] = r32(args + 4 * i, api);
    list[n++] = PX_BATTLE_MESSAGE_END;
    px_battle_message_command(r32(context + 0xc, api), 0x69, id, list, n, api);
}

uint32_t px_battle_show_message(uint32_t context, uint32_t message, const struct px_battle_command_api *api)
{
    const uint32_t flags = r16(message + 2, api), kind = flags & 0x7f, count = flags >> 7 & 0x7f, id = r16(message, api);
    uint32_t list[6];
    unsigned n = 0;
    if ((flags & 0x4000) == 0) {
        if (kind == 1) {
            px_battle_message_plain(context, id, count, message + 4, api);
            return 1;
        }
        if (kind != 2) return 0;
        if (count >= 1 && count <= 4)
            for (uint32_t i = 0; i < count; ++i) list[n++] = r32(message + 4 + 4 * i, api);
        list[n++] = PX_BATTLE_MESSAGE_END;
        px_battle_message_command(r32(context + 0xc, api), 0x6a, id, list, n, api);
        return 1;
    }
    /* Flag bit 14: the extra word (+0x20) first, groups 0x6b and 0x6c. */
    if (kind != 1 && kind != 2) return 0;
    list[n++] = r32(message + 0x20, api);
    if (count >= 1 && count <= 4)
        for (uint32_t i = 0; i < count; ++i) list[n++] = r32(message + 4 + 4 * i, api);
    list[n++] = PX_BATTLE_MESSAGE_END;
    px_battle_message_command(r32(context + 0xc, api), kind == 1 ? 0x6b : 0x6c, id, list, n, api);
    return 1;
}
