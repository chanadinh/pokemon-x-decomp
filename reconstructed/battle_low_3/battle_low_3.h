#ifndef PX_BATTLE_LOW_3_H
#define PX_BATTLE_LOW_3_H

#include "../battle_low_2/battle_low_2.h"

/* DllBattle low-address state hooks, third batch: 15 more functions of the page
 * state machine of battle_low_2 (1083b3cc-1083f3ac, static analysis addresses;
 * runtime addresses are module-relative). Same conventions: each stores its
 * state id in word +0x6c of the global object (108fcb18) when it differs,
 * 'page' is the page object (subobject pointer at +0x190, entry pointers from
 * +0x100, selection bytes +0x6c8/+0x6c9/+0x6ca.., see below), services and
 * virtual methods are called through the low_2 API. Names are by address; the
 * field names are analyst interpretations.
 *
 * The selection block: the six bytes at +0x6ca are filled with 0xff by 1086fd18
 * (the first function of every group does this), +0x25d, +0x6c8 and +0x6c9 hold
 * the current choice, +0x6b8 the page result (0x64 pending, 0xff none, 0). */
struct px_battle_low_3_api {
    const struct px_battle_low_2_api *base;
    uint32_t scratch;   /* logical scratch: the 12-byte stack buffer of 1083bb1c */
};

uint32_t px_battle_state_1083b3cc(uint32_t page, uint32_t a, uint32_t b, const struct px_battle_low_3_api *api);
uint32_t px_battle_state_1083b470(uint32_t page, const struct px_battle_low_3_api *api);
void px_battle_state_1083b530(uint32_t page, const struct px_battle_low_3_api *api);
uint32_t px_battle_state_1083b9f0(uint32_t page, uint32_t a, const struct px_battle_low_3_api *api);
void px_battle_state_1083bb1c(uint32_t page, uint32_t a, uint32_t b, uint32_t c, uint32_t d, const struct px_battle_low_3_api *api);
void px_battle_state_1083c1d8(uint32_t page, uint32_t a, uint32_t b, uint32_t c, const struct px_battle_low_3_api *api);
uint32_t px_battle_state_1083c770(uint32_t page, uint32_t a, const struct px_battle_low_3_api *api);
uint32_t px_battle_state_1083c9a8(uint32_t page, uint32_t a, uint32_t b, uint32_t c, const struct px_battle_low_3_api *api);
uint32_t px_battle_state_1083ca38(uint32_t page, uint32_t a, uint32_t b, uint32_t c, uint32_t d, const struct px_battle_low_3_api *api);
uint32_t px_battle_state_1083cac0(uint32_t page, uint32_t out, const struct px_battle_low_3_api *api);
uint32_t px_battle_state_1083e4c4(uint32_t page, uint32_t a, const struct px_battle_low_3_api *api);
void px_battle_state_1083e58c(uint32_t page, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e,
                              const struct px_battle_low_3_api *api);
uint32_t px_battle_state_1083f15c(uint32_t page, uint32_t a, uint32_t b, const struct px_battle_low_3_api *api);
void px_battle_state_1083f2b4(uint32_t page, uint32_t a, const struct px_battle_low_3_api *api);
uint32_t px_battle_state_1083f3ac(uint32_t page, const struct px_battle_low_3_api *api);

#endif
