#ifndef PX_BATTLE_LOW_2_H
#define PX_BATTLE_LOW_2_H

#include "../pokemon_constructor/pokemon_constructor.h"

/* DllBattle low-address state hooks, second batch: 24 small functions of a
 * menu/page state machine at 108395d8-1083f270 (static analysis addresses;
 * runtime addresses are module-relative). Each starts the same way: a state id
 * (a literal of its own) is stored in word +0x6c of a global object (the
 * relocated pointer 108fcb18, the same literal in every function) when it
 * differs; then it does a small job on the page object ('this', with a
 * subobject pointer at +0x190). The functions are named by address; the
 * names of the fields are analyst interpretations.
 *
 * Calls to routines not reconstructed are services identified by their static
 * address (call: up to four argument registers, call_stack: two more words on
 * the stack); a virtual call loads the function pointer at 'offset' of the
 * object's table and calls it with the object (and one more register value
 * where the original had one, else 0). */
struct px_battle_low_2_api {
    const struct px_pokemon_api *memory;
    void *user;
    uint32_t state_object;   /* the relocated pointer 108fcb18, logical */
    uint32_t (*call)(void *user, uint32_t target, uint32_t a, uint32_t b, uint32_t c, uint32_t d);
    uint32_t (*call_stack)(void *user, uint32_t target, uint32_t a, uint32_t b, uint32_t c, uint32_t d,
                           uint32_t s0, uint32_t s1);
    uint32_t (*vcall)(void *user, uint32_t object, uint32_t offset, uint32_t a);
};

uint32_t px_battle_state_108399ec(uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3, const struct px_battle_low_2_api *api);
uint32_t px_battle_state_1083b44c(const struct px_battle_low_2_api *api);
void px_battle_state_1083c518(const struct px_battle_low_2_api *api);
uint32_t px_battle_state_1083c960(const struct px_battle_low_2_api *api);
uint32_t px_battle_state_1083c984(const struct px_battle_low_2_api *api);
uint32_t px_battle_state_1083e7dc(const struct px_battle_low_2_api *api);
void px_battle_state_1083e800(const struct px_battle_low_2_api *api);
void px_battle_state_1083f134(uint32_t page, const struct px_battle_low_2_api *api);
uint32_t px_battle_state_1083b17c(uint32_t page, const struct px_battle_low_2_api *api);
uint32_t px_battle_state_1083c424(uint32_t page, const struct px_battle_low_2_api *api);
uint32_t px_battle_state_1083bad8(uint32_t page, const struct px_battle_low_2_api *api);
uint32_t px_battle_state_1083f270(uint32_t page, const struct px_battle_low_2_api *api);
uint32_t px_battle_state_1083bdd4(uint32_t page, const struct px_battle_low_2_api *api);
uint32_t px_battle_state_1083e7a0(uint32_t page, const struct px_battle_low_2_api *api);
void px_battle_state_1083e920(uint32_t page, uint32_t a, uint32_t b, const struct px_battle_low_2_api *api);
uint32_t px_battle_state_1083e960(uint32_t page, uint32_t a, const struct px_battle_low_2_api *api);
void px_battle_state_1083ea0c(uint32_t page, uint32_t a, uint32_t b, const struct px_battle_low_2_api *api);
uint32_t px_battle_state_1083f348(uint32_t page, uint32_t a, uint32_t b, const struct px_battle_low_2_api *api);
uint32_t px_battle_state_1083cb44(uint32_t page, const struct px_battle_low_2_api *api);
uint32_t px_battle_state_10839818(uint32_t page, const struct px_battle_low_2_api *api);
uint32_t px_battle_state_108395d8(uint32_t page, uint32_t a, const struct px_battle_low_2_api *api);
uint32_t px_battle_state_1083b110(uint32_t page, uint32_t a, uint32_t b, const struct px_battle_low_2_api *api);
uint32_t px_battle_state_1083cbb0(uint32_t page, const struct px_battle_low_2_api *api);
uint32_t px_battle_state_1083f1f4(uint32_t page, const struct px_battle_low_2_api *api);

#endif
