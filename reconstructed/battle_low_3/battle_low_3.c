#include "battle_low_3.h"

static const struct px_pokemon_api *mem(const struct px_battle_low_3_api *api)
{
    return api->base->memory;
}

static uint32_t load8(uint32_t a, const struct px_battle_low_3_api *api)
{
    return mem(api)->load8(mem(api)->user, a);
}

static uint32_t load16(uint32_t a, const struct px_battle_low_3_api *api)
{
    return mem(api)->load16(mem(api)->user, a);
}

static uint32_t load32(uint32_t a, const struct px_battle_low_3_api *api)
{
    return mem(api)->load32(mem(api)->user, a);
}

static void store8(uint32_t a, uint32_t v, const struct px_battle_low_3_api *api)
{
    mem(api)->store8(mem(api)->user, a, (uint8_t)v);
}

static void store16(uint32_t a, uint32_t v, const struct px_battle_low_3_api *api)
{
    mem(api)->store16(mem(api)->user, a, (uint16_t)v);
}

static void store32(uint32_t a, uint32_t v, const struct px_battle_low_3_api *api)
{
    mem(api)->store32(mem(api)->user, a, v);
}

static void track(uint32_t id, const struct px_battle_low_3_api *api)
{
    if (load32(api->base->state_object + 0x6c, api) != id) store32(api->base->state_object + 0x6c, id, api);
}

static uint32_t call(const struct px_battle_low_3_api *api, uint32_t target, uint32_t a, uint32_t b, uint32_t c, uint32_t d)
{
    return api->base->call(api->base->user, target, a, b, c, d);
}

static uint32_t vcall(const struct px_battle_low_3_api *api, uint32_t object, uint32_t offset, uint32_t a)
{
    return api->base->vcall(api->base->user, object, offset, a);
}

static uint32_t call_stack(const struct px_battle_low_3_api *api, uint32_t target, uint32_t a, uint32_t b, uint32_t c, uint32_t d)
{
    return api->base->call_stack(api->base->user, target, a, b, c, d, 0, 0);
}

/* The selection block reset every group starts with: 1086fd18 fills the six bytes at +0x6ca with 0xff. */
static void fill_selection(uint32_t page, const struct px_battle_low_3_api *api)
{
    call(api, 0x1086fd18, page + 0x6ca, 0xff, 0, 0);
}

uint32_t px_battle_state_1083b3cc(uint32_t page, uint32_t a, uint32_t b, const struct px_battle_low_3_api *api)
{
    track(0xa7f, api);
    const uint32_t busy = vcall(api, page, 0xbc, 0);
    if (busy != 0) return busy;
    call(api, 0x1089d648, load32(page + 0x1e0, api), a, b, 0);
    const uint32_t sub = load32(page + 0x190, api);
    return call_stack(api, 0x10858150, sub + 0x3a0, load32(page + 0x190 + 0x50, api), 0, 0);
}

uint32_t px_battle_state_1083b470(uint32_t page, const struct px_battle_low_3_api *api)
{
    track(0x9bc, api);
    const uint32_t step = load32(page + 0xf0, api);
    if (step != 0 && step != 1) return 0;
    if (step == 0) {
        if (call(api, 0x108a2abc, load32(page + 0x190, api), load32(page + 0x764, api), 0, 0) == 0) return 0;
        call(api, 0x10836230, load32(page + 0x190, api) + 0xa8, 0, 0, 0);
        store32(page + 0xf0, load32(page + 0xf0, api) + 1, api);
    }
    if (call(api, 0x108573b4, load32(page + 0x190, api), 0, 0, 0) == 0) return 0;
    const uint32_t sub = load32(page + 0x190, api);
    if (load8(sub + 0xa0, api) == 0) {
        store8(sub + 0xa0, 1, api);
        call(api, 0x10857178, sub + 0x5c, sub + 0xa1, 0, 0);
        store8(sub + 8, 1, api);
    }
    store32(page + 0xf0, 0, api);
    return 1;
}

void px_battle_state_1083b530(uint32_t page, const struct px_battle_low_3_api *api)
{
    track(0x1465, api);
    const uint32_t sub = load32(page + 0x190, api);
    const uint32_t object = load32(sub + 0x240, api);
    if (object == 0) return;
    vcall(api, object, 4, 0);
    store32(sub + 0x240, 0, api);
}

/* 1083b9f0 and 1083c770: the entry for the chosen index (virtual call 0x10c twice) decides between pending (0x64) and a start message. */
static uint32_t choose_entry(uint32_t page, uint32_t a, uint32_t message, const struct px_battle_low_3_api *api)
{
    fill_selection(page, api);
    store8(page + 0x6c8, 0xff, api);
    store8(page + 0x6c9, 0xff, api);
    store8(page + 0x25d, 0, api);
    store8(page + 0x6c8, vcall(api, page, 0x10c, a), api);
    store8(page + 0x6c9, vcall(api, page, 0x10c, a), api);
    const uint32_t entry = load32(page + 0x100 + 4 * load8(page + 0x6c8, api), api);
    if (vcall(api, entry, 0x7c, 0) == 0) {
        const uint32_t other = load32(page + 0x100 + 4 * load8(page + 0x6c8, api), api);
        if (load8(other + 0x15c, api) == 0) {
            store32(page + 0x6b8, 0x64, api);
            return 0x64;
        }
    }
    store32(page + 0x6b8, 0, api);
    return call(api, 0x10859464, page, message, 0, 0);
}

uint32_t px_battle_state_1083b9f0(uint32_t page, uint32_t a, const struct px_battle_low_3_api *api)
{
    track(0x116b, api);
    return choose_entry(page, a, 0x26f, api);   /* literal 1083bad4 */
}

uint32_t px_battle_state_1083c770(uint32_t page, uint32_t a, const struct px_battle_low_3_api *api)
{
    track(0x1190, api);
    return choose_entry(page, a, 0x270, api);
}

void px_battle_state_1083bb1c(uint32_t page, uint32_t a, uint32_t b, uint32_t c, uint32_t d, const struct px_battle_low_3_api *api)
{
    track(0x993, api);
    store32(page + 0x760, a, api);
    store32(page + 0x764, d, api);
    const uint32_t list = vcall(api, page, 0x24, 0);
    const uint32_t size = load8(list + 0x18, api);
    uint32_t filled = 0;
    for (; filled < size; ++filled) {
        const uint32_t member = call(api, 0x1085daf8, list, filled & 0xff, 0, 0);
        store8(api->scratch + filled, call(api, 0x10856a88, member, 0x13, 0, 0), api);
    }
    for (uint32_t i = filled; i < 6; ++i) store8(api->scratch + i, 0, api);
    const uint32_t sub = load32(page + 0x190, api);
    store32(sub + 0x3c, 0, api);
    store8(sub + 0x4a, c ^ 1, api);
    store8(sub + 0x138, c ^ 1, api);
    store8(sub + 0x13f, b, api);
    store8(sub + 0x40, b, api);
    call(api, 0x1085fa8c, sub + 0x139, api->scratch, 0, 0);
}

void px_battle_state_1083c1d8(uint32_t page, uint32_t a, uint32_t b, uint32_t c, const struct px_battle_low_3_api *api)
{
    track(0x102f, api);
    fill_selection(page, api);
    store8(page + 0x25d, 0, api);
    store8(page + 0x6c8, a, api);
    store8(page + 0x6c9, b, api);
    store8(page + 0x6ca, b, api);
    store32(page + 0x6b8, 0, api);
    store32(page + 0x6e0, c, api);
    const uint32_t entry = load32(page + 0x100 + 4 * a, api);
    if (entry != 0 && vcall(api, entry, 0x7c, 0) != 0) {
        const uint32_t sub = load32(page + 0x190, api);
        const uint32_t index = load8(page + 0x6c8, api);
        const uint32_t mode = vcall(api, load32(sub, api), 0x44, 0);
        const int flag = mode == 4 ? index != 0 : (index & 1) != 0;
        store8(page + 0x25d, flag ? 1 : 0, api);
        call(api, 0x10859464, page, load32(page + 0x6e0, api), 0, 0);
    } else {
        store32(page + 0x6b8, 0xff, api);
    }
    track(0x1051, api);
}

uint32_t px_battle_state_1083c9a8(uint32_t page, uint32_t a, uint32_t b, uint32_t c, const struct px_battle_low_3_api *api)
{
    track(0xabe, api);
    if (vcall(api, page, 0xbc, 0) != 0) return 1;
    call(api, 0x1089da54, load32(load32(page + 0x190, api) + 0x7b0, api), a & 0xffff, (uint32_t)(int32_t)(int8_t)b, 0);
    const uint32_t sub = load32(page + 0x190, api);
    call_stack(api, 0x10858150, sub + 0x3a0, load32(sub + 0x7b0, api), 0, c);
    return 1;
}

uint32_t px_battle_state_1083ca38(uint32_t page, uint32_t a, uint32_t b, uint32_t c, uint32_t d, const struct px_battle_low_3_api *api)
{
    track(0xa18, api);
    store32(page + 0x760, a, api);
    store32(page + 0x764, d, api);
    const uint32_t sub = load32(page + 0x190, api);
    call(api, 0x10856954, sub, 0, 0, 0);
    store8(sub + 8, 0, api);
    store8(sub + 0x1c9, 0, api);
    store32(sub + 0x1c4, a, api);
    store8(sub + 0x1c8, c, api);
    store8(sub + 0x1cc, 1, api);
    store32(sub + 0x1d0, b, api);
    call(api, 0x1084a214, sub + 0x140, sub + 0x1c4, 0, 0);
    call(api, 0x1084a0bc, sub + 0x140, 0, 0, 0);
    return call(api, 0x108577b4, load32(page + 0x190, api), 0, 0, 0);
}

uint32_t px_battle_state_1083cac0(uint32_t page, uint32_t out, const struct px_battle_low_3_api *api)
{
    track(0x1479, api);
    store8(out, 0, api);
    const uint32_t kind = call(api, 0x108a31a0, load32(page + 0x190, api), 0, 0, 0);
    if (kind == 2) {
        store8(out, 4, api);
        return 1;
    }
    if (kind >= 0xa && kind <= 0xd) {
        store8(out, kind - 0xa, api);
        return 1;
    }
    return 0;
}

uint32_t px_battle_state_1083e4c4(uint32_t page, uint32_t a, const struct px_battle_low_3_api *api)
{
    track(0x11ca, api);
    fill_selection(page, api);
    store8(page + 0x25d, 0, api);
    store8(page + 0x6c8, a, api);
    store8(page + 0x6c9, a, api);
    store8(page + 0x6ca, a, api);
    const uint32_t object = vcall(api, page, 0x20, a);
    uint32_t message = 0x272;   /* literal 1083e588 */
    if (load16(object + 0xc, api) == 0x2a9) message = load8(object + 0x153, api) != 0 ? 0x284 : 0x285;   /* literal 1083e584 */
    return call(api, 0x1085746c, page, a, message, 0);
}

void px_battle_state_1083e58c(uint32_t page, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e,
                              const struct px_battle_low_3_api *api)
{
    track(0x1711, api);
    fill_selection(page, api);
    store8(page + 0x25d, 0, api);
    store8(page + 0x6c8, 0, api);
    store8(page + 0x6c9, a, api);
    store8(page + 0x6ca, a, api);
    store32(page + 0x6d0, 0, api);
    store32(page + 0x6d4, 0, api);
    store16(page + 0x6e4, call(api, 0x10800c04, b, 0, 0, 0), api);   /* literal 1083e624 is the offset 0x6e4 */
    store8(page + 0x25d, 0, api);
    store32(page + 0x6b8, 0, api);
    store32(page + 0x6bc, 0, api);
    store32(page + 0x6fc, b, api);
    store32(page + 0x700, c, api);
    store8(page + 0x71c, d, api);
    store8(page + 0x71d, e, api);
}

uint32_t px_battle_state_1083f15c(uint32_t page, uint32_t a, uint32_t b, const struct px_battle_low_3_api *api)
{
    track(0x14a3, api);
    call(api, 0x108577b4, load32(page + 0x190, api), 0, 0, 0);
    const uint32_t block = load32(page + 0x190, api) + 0x4dc;
    call(api, 0x1082d830, block, 0, 0, 0);
    call(api, 0x1082d6d8, block, a, b, 0);
    const uint32_t busy = vcall(api, page, 0xc0, 0);
    if (busy != 0) return busy;
    return call(api, 0x1080024c, 0x60000, 0, 0xffffffffu, 0);
}

void px_battle_state_1083f2b4(uint32_t page, uint32_t a, const struct px_battle_low_3_api *api)
{
    track(0x111c, api);
    fill_selection(page, api);
    store8(page + 0x6c9, 0xff, api);
    store8(page + 0x25d, 0, api);
    store8(page + 0x6c8, a, api);
    const uint32_t entry = load32(page + 0x100 + 4 * a, api);
    if (vcall(api, entry, 0x7c, 0) == 0) {
        const uint32_t other = load32(page + 0x100 + 4 * load8(page + 0x6c8, api), api);
        if (load8(other + 0x15c, api) == 0) {
            store32(page + 0x6b8, 0x64, api);
            return;
        }
    }
    store32(page + 0x6b4, 0, api);
    store32(page + 0x6b8, 1, api);
}

uint32_t px_battle_state_1083f3ac(uint32_t page, const struct px_battle_low_3_api *api)
{
    track(0xd8b, api);
    uint32_t ok = 1;
    for (uint32_t i = 0; i < 6; ++i) {
        const uint32_t value = load8(page + 0x6ca + i, api);
        if (value == 0xff) continue;
        if (call(api, 0x1083be10, page, value, load32(page + 0x6b4, api) - 10 * i, 0) == 0) ok = 0;
    }
    store32(page + 0x6b4, load32(page + 0x6b4, api) + 1, api);
    if (ok != 0) call(api, 0x10859cc4, load32(page + 0x190, api) + 0x280, 0, 0, 0);
    return ok;
}
