#ifndef PX_BATTLE_MID_3_H
#define PX_BATTLE_MID_3_H

#include <stdint.h>
#include "../pokemon_constructor/pokemon_constructor.h"

/* DllBattle page-object teardowns and small accessors of the static band
 * 10880000-108a6000 (static analysis addresses; runtime addresses are
 * module-relative). Analyst names; the functions are named by address. All
 * calls outside this milestone go through `call` by static address with up to
 * five arguments (r0-r3 and the first stacked word); tail calls return its
 * value. Logical ARM32 addresses. */
struct px_battle_mid_3_api {
    const struct px_pokemon_api *memory;
    void *user;
    uint32_t (*call)(void *user, uint32_t function, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);
    /* Sixteen bytes standing in for the stack buffer some functions pass by pointer. */
    uint32_t scratch;
};

/* Teardown: with the handle at the offset set, 1080085c with it and the argument still in r1, then the tail call 108568ec.
 * Offsets: +0xe8 (1088b2e8, 1088e858, 10898a04, 108993b8), +0xec (1088bcb8), +0xd8 (1088db3c),
 * +0xe0 (1088f210, 1088fa38, 10890590, 1089c5dc), +0xf4 (108910d4), +0xdc (10899ff4), +0x118 (1089a9b0). */
uint32_t px_battle_mid_1088b2e8(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api);
uint32_t px_battle_mid_1088e858(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api);
uint32_t px_battle_mid_10898a04(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api);
uint32_t px_battle_mid_108993b8(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api);
uint32_t px_battle_mid_1088bcb8(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api);
uint32_t px_battle_mid_1088db3c(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api);
uint32_t px_battle_mid_1088f210(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api);
uint32_t px_battle_mid_1088fa38(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api);
uint32_t px_battle_mid_10890590(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api);
uint32_t px_battle_mid_1089c5dc(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api);
uint32_t px_battle_mid_108910d4(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api);
uint32_t px_battle_mid_10899ff4(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api);
uint32_t px_battle_mid_1089a9b0(uint32_t object, uint32_t argument, const struct px_battle_mid_3_api *api);
/* 1088a340: 108019f4 on the +0xe8 handle, then the object itself falls through to 1088a35c (a tail call with r0 = object). */
uint32_t px_battle_mid_1088a340(uint32_t object, const struct px_battle_mid_3_api *api);
/* 1088d064: the tail call 10801a2c with the word at +0x118 of the object at +0x88 and 0. */
uint32_t px_battle_mid_1088d064(uint32_t object, const struct px_battle_mid_3_api *api);
/* 1088d98c: 10801c0c with the +0xe0 handle and the signed byte at +0xd5, then 108019f4 with the handle; falls through to 1088d9b4 with the object. */
uint32_t px_battle_mid_1088d98c(uint32_t object, const struct px_battle_mid_3_api *api);
/* Three accessors that pass a stack buffer: 10889aac: 10800504(a, buffer, 1), then the tail call 1083ac28(b);
 * 10886734: the same but the tail call is 1083c0b8(b, the buffer's word); 10886bcc: 10801ab4(a, buffer, 1) then
 * the tail call 108759c8(b, buffer). */
uint32_t px_battle_mid_10889aac(uint32_t a, uint32_t b, const struct px_battle_mid_3_api *api);
uint32_t px_battle_mid_10886734(uint32_t a, uint32_t b, const struct px_battle_mid_3_api *api);
uint32_t px_battle_mid_10886bcc(uint32_t a, uint32_t b, const struct px_battle_mid_3_api *api);

#endif
