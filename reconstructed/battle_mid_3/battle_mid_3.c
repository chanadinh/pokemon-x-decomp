#include "battle_mid_3.h"

static uint32_t load32(uint32_t at, const struct px_battle_mid_3_api *api)
{
    return api->memory->load32(api->memory->user, at);
}

static uint32_t loadsb(uint32_t at, const struct px_battle_mid_3_api *api)
{
    return (uint32_t)(int32_t)(int8_t)api->memory->load8(api->memory->user, at);
}

static uint32_t call(uint32_t function, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e,
                     const struct px_battle_mid_3_api *api)
{
    return api->call(api->user, function, a, b, c, d, e);
}

static uint32_t teardown(uint32_t object, uint32_t offset, uint32_t argument, const struct px_battle_mid_3_api *api)
{
    const uint32_t handle = load32(object + offset, api);
    if (handle == 0) return 0;
    call(0x1080085c, handle, argument, 0, 0, 0, api);
    return call(0x108568ec, argument, 0, 0, 0, 0, api);
}

uint32_t px_battle_mid_1088b2e8(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api) { return teardown(object, 0xe8, argument, api); }
uint32_t px_battle_mid_1088e858(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api) { return teardown(object, 0xe8, argument, api); }
uint32_t px_battle_mid_10898a04(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api) { return teardown(object, 0xe8, argument, api); }
uint32_t px_battle_mid_108993b8(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api) { return teardown(object, 0xe8, argument, api); }
uint32_t px_battle_mid_1088bcb8(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api) { return teardown(object, 0xec, argument, api); }
uint32_t px_battle_mid_1088db3c(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api) { return teardown(object, 0xd8, argument, api); }
uint32_t px_battle_mid_1088f210(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api) { return teardown(object, 0xe0, argument, api); }
uint32_t px_battle_mid_1088fa38(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api) { return teardown(object, 0xe0, argument, api); }
uint32_t px_battle_mid_10890590(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api) { return teardown(object, 0xe0, argument, api); }
uint32_t px_battle_mid_1089c5dc(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api) { return teardown(object, 0xe0, argument, api); }
uint32_t px_battle_mid_108910d4(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api) { return teardown(object, 0xf4, argument, api); }
uint32_t px_battle_mid_10899ff4(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api) { return teardown(object, 0xdc, argument, api); }
uint32_t px_battle_mid_1089a9b0(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api) { return teardown(object, 0x118, argument, api); }

uint32_t px_battle_mid_1088a340(uint32_t object, const struct px_battle_mid_3_api *api)
{
    call(0x108019f4, load32(object + 0xe8, api), 0, 0, 0, 0, api);
    return call(0x1088a35c, object, 0, 0, 0, 0, api);
}

uint32_t px_battle_mid_1088d064(uint32_t object, const struct px_battle_mid_3_api *api)
{
    return call(0x10801a2c, load32(load32(object + 0x88, api) + 0x118, api), 0, 0, 0, 0, api);
}

uint32_t px_battle_mid_1088d98c(uint32_t object, const struct px_battle_mid_3_api *api)
{
    call(0x10801c0c, load32(object + 0xe0, api), loadsb(object + 0xd5, api), 0, 0, 0, api);
    call(0x108019f4, load32(object + 0xe0, api), 0, 0, 0, 0, api);
    return call(0x1088d9b4, object, 0, 0, 0, 0, api);
}

uint32_t px_battle_mid_10889aac(uint32_t a, uint32_t b, const struct px_battle_mid_3_api *api)
{
    call(0x10800504, a, api->scratch, 1, 0, 0, api);
    return call(0x1083ac28, b, 0, 0, 0, 0, api);
}

uint32_t px_battle_mid_10886734(uint32_t a, uint32_t b, const struct px_battle_mid_3_api *api)
{
    call(0x10800504, a, api->scratch, 1, 0, 0, api);
    return call(0x1083c0b8, b, load32(api->scratch, api), 0, 0, 0, api);
}

uint32_t px_battle_mid_10886bcc(uint32_t a, uint32_t b, const struct px_battle_mid_3_api *api)
{
    call(0x10801ab4, a, api->scratch, 1, 0, 0, api);
    return call(0x108759c8, b, api->scratch, 0, 0, 0, api);
}
