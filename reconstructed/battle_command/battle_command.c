#include "battle_command.h"

static void *user(const struct px_battle_command_api *api)
{
    return api->memory->user;
}

static uint32_t r8(uint32_t address, const struct px_battle_command_api *api)
{
    return api->memory->load8(user(api), address);
}

static uint32_t r32(uint32_t address, const struct px_battle_command_api *api)
{
    return api->memory->load32(user(api), address);
}

static void w8(uint32_t address, uint32_t value, const struct px_battle_command_api *api)
{
    api->memory->store8(user(api), address, (uint8_t)value);
}

static void w32(uint32_t address, uint32_t value, const struct px_battle_command_api *api)
{
    api->memory->store32(user(api), address, value);
}

static void fault(const struct px_battle_command_api *api)
{
    api->fault(user(api));
}

/* One byte at the queue's position; a position from limit on is an
 * overflow, after which the byte is still written. */
static void put(uint32_t queue, uint32_t limit, uint32_t value, const struct px_battle_command_api *api)
{
    if (r32(queue, api) >= limit) api->overflow(user(api));
    const uint32_t at = r32(queue, api);
    w32(queue, at + 1, api);
    w8(queue + 8 + at, value, api);
}

/* The writers check once, for the whole value. */
static void put_bytes(uint32_t queue, uint32_t limit, uint32_t value, unsigned bytes, const struct px_battle_command_api *api)
{
    if (r32(queue, api) >= limit) api->overflow(user(api));
    for (unsigned i = bytes; i-- > 0;) {
        const uint32_t at = r32(queue, api);
        w32(queue, at + 1, api);
        w8(queue + 8 + at, value >> (8 * i), api);
    }
}

void px_battle_queue_u16(uint32_t queue, uint32_t value, const struct px_battle_command_api *api)
{
    put_bytes(queue, 0x176f /* literal 108620d0 */, value, 2, api);
}

void px_battle_queue_u24(uint32_t queue, uint32_t value, const struct px_battle_command_api *api)
{
    put_bytes(queue, 0x176e /* literal 1084d99c */, value, 3, api);
}

void px_battle_queue_u32(uint32_t queue, uint32_t value, const struct px_battle_command_api *api)
{
    put_bytes(queue, 0x176d /* literal 1084dbf0 */, value, 4, api);
}

uint32_t px_battle_pack3(uint32_t bytes, int32_t a, int32_t b, int32_t c, int32_t wa, int32_t wb, int32_t wc,
                         const struct px_battle_command_api *api)
{
    /* The original checks the third width plus 10 against the size. */
    if (wc + 10 > (int32_t)(bytes << 3)) fault(api);
    if (wa == 0) fault(api);
    if (wb == 0) fault(api);
    if (wc == 0) fault(api);
    const int32_t ma = (int32_t)(((1u << wa) - 1) & 0xffff), mb = (int32_t)(((1u << wb) - 1) & 0xffff),
                  mc = (int32_t)(((1u << wc) - 1) & 0xffff);
    if (a > ma) fault(api);
    if (b > mb) fault(api);
    if (c > mc) fault(api);
    return ((uint32_t)(a & ma) << (wb + wc)) | ((uint32_t)(b & mb) << wc) | (uint32_t)(c & mc);
}

uint32_t px_battle_pack2(int32_t a, int32_t b, int32_t wa, int32_t wb, const struct px_battle_command_api *api)
{
    if (wa + wb > 8) fault(api);
    if (wa == 0) fault(api);
    if (wb == 0) fault(api);
    const int32_t ma = (int32_t)(((1u << wa) - 1) & 0xff), mb = (int32_t)(((1u << wb) - 1) & 0xff);
    if (a > ma) fault(api);
    if (b > mb) fault(api);
    return (((uint32_t)(a & ma) << wb) | (uint32_t)(b & mb)) & 0xff;
}

int px_battle_command_format_args(uint32_t info)
{
    switch (info) {
    case 0x01: case 0x11: case 0x21: return 1;
    case 0x02: case 0x12: case 0x22: case 0x32: case 0x42: return 2;
    case 0x03: case 0x13: case 0x23: case 0x33: case 0x43: case 0x53: return 3;
    case 0x04: case 0x14: case 0x24: case 0x34: case 0x44: case 0x54: return 4;
    case 0x05: case 0x15: case 0x25: case 0x35: return 5;
    case 0x06: case 0x16: case 0x26: case 0x36: return 6;
    case 0x07: return 7;
    case 0x08: return 8;
    case 0x09: return 9;
    case 0x30: return 0;
    default: return -1;
    }
}

void px_battle_command_encode(uint32_t queue, uint32_t id, uint32_t info, uint32_t args, const struct px_battle_command_api *api)
{
    enum { LIMIT = PX_BATTLE_QUEUE_CAPACITY /* literal 1084e804 */ };
#define A(i) r32(args + 4 * (i), api)
#define S(i) ((int32_t)A(i))
#define B(v) put(queue, LIMIT, (v), api)
    px_battle_queue_u16(queue, id, api);
    switch (info) {
    case 0x01: B(A(0)); break;
    case 0x02: B(A(0)); B(A(1)); break;
    case 0x03: B(px_battle_pack2(S(0), S(1), 5, 3, api)); B(A(2)); break;
    case 0x04:
        B(px_battle_pack2(S(0), S(1), 5, 3, api));
        B(A(2));
        px_battle_queue_u16(queue, A(3) & 0xffff, api);
        break;
    case 0x05:
        px_battle_queue_u16(queue, px_battle_pack3(2, S(0), S(1), S(2), 5, 5, 5, api) & 0xffff, api);
        px_battle_queue_u16(queue, A(3) & 0xffff, api);
        px_battle_queue_u16(queue, A(4) & 0xffff, api);
        break;
    case 0x06: {
        const uint32_t first = px_battle_pack3(2, S(0), S(1), S(2), 5, 5, 5, api) & 0xffff;
        const uint32_t second = px_battle_pack3(2, S(3), S(4), S(5), 5, 5, 5, api) & 0xffff;
        px_battle_queue_u16(queue, first, api);
        px_battle_queue_u16(queue, second, api);
        break;
    }
    case 0x07: case 0x08: case 0x09:
        /* 7, 8 or 9 bytes. */
        for (uint32_t i = 0; i < info; ++i) B(A(i));
        break;
    case 0x11: px_battle_queue_u16(queue, A(0) & 0xffff, api); break;
    case 0x12: B(A(0)); px_battle_queue_u16(queue, A(1) & 0xffff, api); break;
    case 0x13: B(px_battle_pack2(S(0), S(1), 5, 3, api)); px_battle_queue_u16(queue, A(2) & 0xffff, api); break;
    case 0x14: B(px_battle_pack2(S(0), S(1), 5, 3, api)); B(px_battle_pack2(S(2), S(3), 5, 3, api)); break;
    case 0x15: {
        const uint32_t first = px_battle_pack2(S(0), S(1), 5, 3, api), second = px_battle_pack2(S(2), S(3), 7, 1, api);
        px_battle_queue_u16(queue, first, api);
        px_battle_queue_u16(queue, second, api);
        px_battle_queue_u16(queue, A(4) & 0xffff, api);
        break;
    }
    case 0x16:
        /* Fields of 3, 2, 1, 1 and 1 bits in one byte. */
        if (S(0) > 7) fault(api);
        if (S(1) > 3) fault(api);
        if (S(2) > 1) fault(api);
        if (S(3) > 1) fault(api);
        if (S(4) > 1) fault(api);
        B((A(0) & 7) << 5 | (A(1) & 3) << 3 | (A(2) & 1) << 2 | (A(3) & 1) << 1 | (A(4) & 1));
        px_battle_queue_u16(queue, A(5) & 0xffff, api);
        break;
    case 0x21: px_battle_queue_u32(queue, A(0), api); break;
    case 0x22: B(A(0)); px_battle_queue_u32(queue, A(1), api); break;
    case 0x23: px_battle_queue_u16(queue, px_battle_pack3(2, S(0), S(1), S(2), 5, 5, 5, api) & 0xffff, api); break;
    case 0x24: px_battle_queue_u24(queue, px_battle_pack3(3, S(0), S(1), S(2), 5, 5, 0xe, api), api); B(A(3)); break;
    case 0x25:
        /* Fields of 3, 3, 1 and 1 bits. */
        if (S(0) > 7) fault(api);
        if (S(1) > 7) fault(api);
        if (S(2) > 1) fault(api);
        if (S(3) > 1) fault(api);
        B((A(0) & 7) << 5 | (A(1) & 7) << 2 | (A(2) & 1) << 1 | (A(3) & 1));
        px_battle_queue_u16(queue, A(4) & 0xffff, api);
        break;
    case 0x26:
        /* Four 5-bit fields in 24 bits. */
        if (S(0) > 0x1f) fault(api);
        if (S(1) > 0x1f) fault(api);
        if (S(2) > 0x1f) fault(api);
        if (S(3) > 0x1f) fault(api);
        px_battle_queue_u24(queue, (A(0) & 0x1f) << 15 | (A(1) & 0x1f) << 10 | (A(2) & 0x1f) << 5 | (A(3) & 0x1f), api);
        px_battle_queue_u16(queue, A(4) & 0xffff, api);
        px_battle_queue_u16(queue, A(5) & 0xffff, api);
        break;
    case 0x30: break;
    case 0x32: B(px_battle_pack2(S(0), S(1), 4, 4, api)); break;
    case 0x33: px_battle_queue_u24(queue, px_battle_pack3(3, S(0), S(1), S(2), 5, 5, 0xe, api), api); break;
    case 0x34:
        px_battle_queue_u24(queue, px_battle_pack3(3, S(0), S(1), S(2), 5, 5, 0xe, api), api);
        px_battle_queue_u16(queue, A(3) & 0xffff, api);
        break;
    case 0x35: {
        const uint32_t first = px_battle_pack3(3, S(0), S(1), S(2), 5, 5, 0xe, api), second = px_battle_pack2(S(3), S(4), 7, 1, api);
        px_battle_queue_u24(queue, first, api);
        B(second);
        break;
    }
    case 0x36:
        px_battle_queue_u16(queue, px_battle_pack3(2, S(0), S(1), S(2), 5, 5, 6, api) & 0xffff, api);
        px_battle_queue_u16(queue, A(3) & 0xffff, api);
        px_battle_queue_u16(queue, A(4) & 0xffff, api);
        px_battle_queue_u16(queue, A(5) & 0xffff, api);
        break;
    case 0x42: B(px_battle_pack2(S(0), S(1), 5, 3, api)); break;
    case 0x43: B(A(0)); B(A(1)); px_battle_queue_u16(queue, A(2) & 0xffff, api); break;
    case 0x44:
        px_battle_queue_u16(queue, px_battle_pack3(2, S(0), S(1), S(2), 5, 5, 6, api) & 0xffff, api);
        px_battle_queue_u16(queue, A(3) & 0xffff, api);
        break;
    case 0x53: B(A(0)); B(A(1)); px_battle_queue_u32(queue, A(2), api); break;
    case 0x54:
        B(A(0));
        B(A(1));
        px_battle_queue_u16(queue, A(2) & 0xffff, api);
        px_battle_queue_u16(queue, A(3) & 0xffff, api);
        break;
    default:
        /* Format 0 and every format without a case. */
        fault(api);
        break;
    }
#undef A
#undef S
#undef B
}

void px_battle_command(uint32_t queue, uint32_t id, const uint32_t *args, unsigned count, const struct px_battle_command_api *api)
{
    id &= 0xff;
    if (id >= PX_BATTLE_COMMANDS) {
        fault(api);
        return;
    }
    const uint32_t info = r8(api->info + id, api), words = info & 0xf;
    if (words > 0x10) fault(api);
    for (uint32_t i = 0; i < words; ++i) w32(api->buffer + 4 * i, i < count ? args[i] : 0, api);
    px_battle_command_encode(queue, id, info, api->buffer, api);
}
