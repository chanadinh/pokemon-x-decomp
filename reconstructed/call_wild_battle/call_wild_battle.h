#ifndef PX_CALL_WILD_BATTLE_H
#define PX_CALL_WILD_BATTLE_H

#include <stdint.h>

/* Opaque host context; this is not a recovered 3DS structure definition. */
struct px_cwb_context;

struct px_cwb_api {
    void *user;
    struct px_cwb_context *(*get_context)(void *user);
    uint32_t (*read_context_word_0c)(void *user, struct px_cwb_context *context);
    void (*field_helper)(void *user, uint32_t field_value,
                         uint32_t argument, uint32_t zero);
    void (*context_helper)(void *user, struct px_cwb_context *context);
};

/*
 * Host reconstruction of the wrapper registered as CallWildBattle.
 * args must contain at least two readable, decoded host uint32_t words.
 * api and all callbacks must be valid; callbacks must return normally.
 * The unused incoming ARM r0 argument is omitted from this host API.
 * See README.md for evidence, adapter contracts, and limits.
 */
uint32_t px_call_wild_battle(const uint32_t *args, const struct px_cwb_api *api);

#endif
