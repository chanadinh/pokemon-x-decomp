#include "battle_low_2.h"

static uint32_t load32(uint32_t a, const struct px_battle_low_2_api *api)
{
    return api->memory->load32(api->memory->user, a);
}

static void store8(uint32_t a, uint32_t v, const struct px_battle_low_2_api *api)
{
    api->memory->store8(api->memory->user, a, (uint8_t)v);
}

static void store32(uint32_t a, uint32_t v, const struct px_battle_low_2_api *api)
{
    api->memory->store32(api->memory->user, a, v);
}

/* Every function starts with this: the state id goes to word +0x6c of the global object when it differs. */
static void track(uint32_t id, const struct px_battle_low_2_api *api)
{
    if (load32(api->state_object + 0x6c, api) != id) store32(api->state_object + 0x6c, id, api);
}

static uint32_t call(const struct px_battle_low_2_api *api, uint32_t target, uint32_t a, uint32_t b, uint32_t c, uint32_t d)
{
    return api->call(api->user, target, a, b, c, d);
}

uint32_t px_battle_state_108399ec(uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3, const struct px_battle_low_2_api *api)
{
    track(0x11fd, api);
    return call(api, 0x10858318, r0, r1, r2, r3);   /* tail branch with the argument registers untouched */
}

uint32_t px_battle_state_1083b44c(const struct px_battle_low_2_api *api)
{
    track(0x115b, api);
    return 1;
}

void px_battle_state_1083c518(const struct px_battle_low_2_api *api)
{
    track(0xdf3, api);
}

uint32_t px_battle_state_1083c960(const struct px_battle_low_2_api *api)
{
    track(0x14c7, api);
    return 1;
}

uint32_t px_battle_state_1083c984(const struct px_battle_low_2_api *api)
{
    track(0x1598, api);
    return 1;
}

uint32_t px_battle_state_1083e7dc(const struct px_battle_low_2_api *api)
{
    track(0x14e0, api);
    return 1;
}

void px_battle_state_1083e800(const struct px_battle_low_2_api *api)
{
    track(0x1592, api);
}

void px_battle_state_1083f134(uint32_t page, const struct px_battle_low_2_api *api)
{
    track(0x9e1, api);
    store32(page + 0xf0, 0, api);
}

/* 1083b17c and 1083c424: done at once when word +0x6b8 is set, else the 10858cb8 service with 0. */
static uint32_t wait_or_service(uint32_t page, const struct px_battle_low_2_api *api)
{
    if (load32(page + 0x6b8, api) != 0) return 1;
    return call(api, 0x10858cb8, page, 0, 0, 0);
}

uint32_t px_battle_state_1083b17c(uint32_t page, const struct px_battle_low_2_api *api)
{
    track(0x1180, api);
    return wait_or_service(page, api);
}

uint32_t px_battle_state_1083c424(uint32_t page, const struct px_battle_low_2_api *api)
{
    track(0x11a6, api);
    return wait_or_service(page, api);
}

uint32_t px_battle_state_1083bad8(uint32_t page, const struct px_battle_low_2_api *api)
{
    track(0x1155, api);
    const uint32_t sub = load32(page + 0x190, api);
    call(api, 0x10800864, load32(sub + 0x65c, api), 3, 0, 0);
    return call(api, 0x10800874, load32(sub + 0x65c, api), 2, 1, 0);
}

uint32_t px_battle_state_1083f270(uint32_t page, const struct px_battle_low_2_api *api)
{
    track(0x1162, api);
    const uint32_t sub = load32(page + 0x190, api);
    call(api, 0x10800864, load32(sub + 0x65c, api), 2, 0, 0);
    return call(api, 0x10800874, load32(sub + 0x65c, api), 3, 1, 0);
}

uint32_t px_battle_state_1083bdd4(uint32_t page, const struct px_battle_low_2_api *api)
{
    track(0xdf9, api);
    call(api, 0x10859cc4, load32(page + 0x190, api) + 0x280, 0, 0, 0);
    return 1;
}

uint32_t px_battle_state_1083e7a0(uint32_t page, const struct px_battle_low_2_api *api)
{
    track(0x14ab, api);
    const uint32_t result = call(api, 0x10801094, load32(load32(page + 0x190, api) + 0x4f4, api), 0, 0, 0);
    return result != 0 ? 1 : 0;
}

void px_battle_state_1083e920(uint32_t page, uint32_t a, uint32_t b, const struct px_battle_low_2_api *api)
{
    track(0x97d, api);
    call(api, 0x108a26c8, load32(page + 0x190, api), a, 0, 0);
    store32(load32(page + 0x190, api) + 0x448, b, api);
}

uint32_t px_battle_state_1083e960(uint32_t page, uint32_t a, const struct px_battle_low_2_api *api)
{
    track(0x969, api);
    const uint32_t sub = load32(page + 0x190, api);
    return api->call_stack(api->user, 0x10858150, sub + 0x3a0, load32(sub + 0x7b0, api), 0, a, 0, 0);
}

void px_battle_state_1083ea0c(uint32_t page, uint32_t a, uint32_t b, const struct px_battle_low_2_api *api)
{
    track(0x146f, api);
    const uint32_t sub = load32(page + 0x190, api);
    const uint32_t object = api->vcall(api->user, load32(sub, api), 0x24, 0);
    const uint32_t result = call(api, 0x10876384, object, a, 0, 0);
    store32(sub + 0x278, result, api);
    api->memory->store16(api->memory->user, sub + 0x27c, (uint16_t)b);
    store32(sub + 0x3c, 0, api);
}

uint32_t px_battle_state_1083f348(uint32_t page, uint32_t a, uint32_t b, const struct px_battle_low_2_api *api)
{
    track(0xa44, api);
    store32(page + 0x764, 0, api);
    const uint32_t sub = load32(page + 0x190, api);
    call(api, 0x10856954, sub, 0, 0, 0);
    store8(sub + 8, 0, api);
    store8(sub + 0x1c9, 1, api);
    store8(sub + 0x1ca, a, api);
    store8(sub + 0x1cb, b, api);
    store8(sub + 0x1cc, 1, api);
    return call(api, 0x1084a214, sub + 0x140, sub + 0x1c4, 0, 0);
}

/* 1083cb44 and 10839818: the service 10858cb8 with 0 decides; when it says yes, 1085db0c and 10859cc4 follow. */
static uint32_t service_then_start(uint32_t page, const struct px_battle_low_2_api *api)
{
    const uint32_t ready = call(api, 0x10858cb8, page, 0, 0, 0);
    if (ready == 0) return ready;
    call(api, 0x1085db0c, page, 0, 0, 0);
    call(api, 0x10859cc4, load32(page + 0x190, api) + 0x280, 0, 0, 0);
    return 1;
}

uint32_t px_battle_state_1083cb44(uint32_t page, const struct px_battle_low_2_api *api)
{
    track(0x1057, api);
    if (load32(page + 0x6b8, api) == 0xff) return 1;
    return service_then_start(page, api);
}

uint32_t px_battle_state_10839818(uint32_t page, const struct px_battle_low_2_api *api)
{
    track(0xfc4, api);
    return service_then_start(page, api);
}

uint32_t px_battle_state_108395d8(uint32_t page, uint32_t a, const struct px_battle_low_2_api *api)
{
    track(0xfb3, api);
    call(api, 0x1086fd18, page + 0x6ca, 0xff, 0, 0);
    store8(page + 0x25d, 0, api);
    store8(page + 0x6c8, 0, api);
    store8(page + 0x6c9, 0, api);
    store8(page + 0x6ca, 0, api);
    store32(page + 0x6b8, 0, api);
    store32(page + 0x6e0, a, api);
    return call(api, 0x10859464, page, a, 0, 0);
}

uint32_t px_battle_state_1083b110(uint32_t page, uint32_t a, uint32_t b, const struct px_battle_low_2_api *api)
{
    track(0x11f1, api);
    call(api, 0x1086fd18, page + 0x6ca, 0xff, 0, 0);
    store8(page + 0x25d, 0, api);
    store8(page + 0x6c8, a, api);
    store8(page + 0x6c9, b, api);
    store8(page + 0x6ca, b, api);
    return call(api, 0x1085746c, page, a, 0x273, 0);
}

uint32_t px_battle_state_1083cbb0(uint32_t page, const struct px_battle_low_2_api *api)
{
    track(0x145e, api);
    const uint32_t sub = load32(page + 0x190, api);
    store32(sub + 0x23c, 0, api);
    store8(sub + 0x244, 0, api);
    call(api, 0x108568ec, 0, 0, 0, 0);
    call(api, 0x1085a18c, load32(sub + 0x238, api), 1, 0, 0);
    api->vcall(api->user, load32(sub + 0x88, api), 0x10, 0);
    store32(sub + 0x88, 0, api);
    store8(sub + 0x93, 0, api);
    call(api, 0x108568ec, 0, 0, 0, 0);
    return call(api, 0x1085a18c, load32(sub + 0x74, api), 1, 0, 0);
}

uint32_t px_battle_state_1083f1f4(uint32_t page, const struct px_battle_low_2_api *api)
{
    track(0x14d8, api);
    const uint32_t sub = load32(page + 0x190, api);
    call(api, 0x10860270, sub + 0x4ec, load32(sub + 0x4e0, api), 0, 0);
    const uint32_t busy = api->vcall(api->user, page, 0xc0, 0);
    if (busy != 0) return busy;
    return call(api, 0x1080024c, 0x60000, 0, 0xffffffffu, 0);
}
