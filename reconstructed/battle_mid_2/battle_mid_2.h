#ifndef PX_BATTLE_MID_2_H
#define PX_BATTLE_MID_2_H

#include <stdint.h>
#include "../pokemon_constructor/pokemon_constructor.h"

/* DllBattle page-object helpers of the static band 10880000-108a6000 (static
 * analysis addresses; runtime addresses are module-relative). Analyst names;
 * the object is a UI page with a handle at +0xe4/+0xec/+0xfc, a state byte
 * (+0xf4/+0x104) and a next-state byte (+0xf5/+0x105), whose five families
 * (10892xxx, 108935xx, 10894xxx, 10895xxx/10896xxx, 10897xxx) share a shape:
 * an init that starts the page's animation and the first call, a teardown
 * that releases a handle, and (three families) a per-frame update.
 * All calls outside this milestone go through `call` by static address with
 * up to five arguments (r0-r3 and the first stacked word); tail calls return
 * its value (10800394, the abort helper that returns, is one of them). Logical ARM32 addresses. */
struct px_battle_mid_2_api {
    const struct px_pokemon_api *memory;
    void *user;
    uint32_t (*call)(void *user, uint32_t function, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);
};

/* Init, variant a: 10892810 and 1089289c (first call 1085168c, value 0x41). */
void px_battle_mid_10892810(uint32_t object, const struct px_battle_mid_2_api *api);
void px_battle_mid_1089289c(uint32_t object, const struct px_battle_mid_2_api *api);
/* Init, variant b (the final state byte last): 10893568, 108935f4 (108511c8, 0x41); 1089498c, 10894a18 (10850cf4, 0x5f). */
void px_battle_mid_10893568(uint32_t object, const struct px_battle_mid_2_api *api);
void px_battle_mid_108935f4(uint32_t object, const struct px_battle_mid_2_api *api);
void px_battle_mid_1089498c(uint32_t object, const struct px_battle_mid_2_api *api);
void px_battle_mid_10894a18(uint32_t object, const struct px_battle_mid_2_api *api);
/* Teardown: when the handle at +0xe4 (+0xf4 for the 108954a0 and 10896744, +0xd8 for 10897960) is set, 1080085c then the tail call 108568ec. */
uint32_t px_battle_mid_10892874(uint32_t object, uint32_t argument, const struct px_battle_mid_2_api *api);
uint32_t px_battle_mid_108935cc(uint32_t object, uint32_t argument, const struct px_battle_mid_2_api *api);
uint32_t px_battle_mid_108949f0(uint32_t object, uint32_t argument, const struct px_battle_mid_2_api *api);
uint32_t px_battle_mid_108954a0(uint32_t object, uint32_t argument, const struct px_battle_mid_2_api *api);
uint32_t px_battle_mid_10896744(uint32_t object, uint32_t argument, const struct px_battle_mid_2_api *api);
uint32_t px_battle_mid_10897960(uint32_t object, uint32_t argument, const struct px_battle_mid_2_api *api);
/* Init plus the handle's reset (the first half of variant a, then 10801c54, 1080085c twice, 10801c1c). */
uint32_t px_battle_mid_10892c34(uint32_t object, const struct px_battle_mid_2_api *api);
uint32_t px_battle_mid_108939f0(uint32_t object, const struct px_battle_mid_2_api *api);
uint32_t px_battle_mid_10894e14(uint32_t object, const struct px_battle_mid_2_api *api);
/* Per-frame update (10892ca4: child 10892900, limit 0x40; 10893a60: 10893658, 0x40; 10894e84: 10894a7c, 0x5e). */
void px_battle_mid_10892ca4(uint32_t object, const struct px_battle_mid_2_api *api);
void px_battle_mid_10893a60(uint32_t object, const struct px_battle_mid_2_api *api);
void px_battle_mid_10894e84(uint32_t object, const struct px_battle_mid_2_api *api);
/* The 0x104-state pages (call 10850998, 10850998, 1084fe30; limits 0xf, 0x10, 0x10; results 1, 0, 0). */
uint32_t px_battle_mid_10895410(uint32_t object, const struct px_battle_mid_2_api *api);
uint32_t px_battle_mid_108954c8(uint32_t object, const struct px_battle_mid_2_api *api);
uint32_t px_battle_mid_1089676c(uint32_t object, const struct px_battle_mid_2_api *api);
/* 10897bf4: the +0xe0 page. */
uint32_t px_battle_mid_10897bf4(uint32_t object, const struct px_battle_mid_2_api *api);

#endif
