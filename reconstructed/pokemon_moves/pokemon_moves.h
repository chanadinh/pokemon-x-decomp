#ifndef PX_POKEMON_MOVES_H
#define PX_POKEMON_MOVES_H

#include "../pokemon_state/state_fields.h"

/* Analyst names. The move-resource service remains external; selector 5
 * supplies base PP. It may mutate logical memory, including object+0x0c.
 * Required callbacks and ordinary logical memory must remain valid. */
struct px_move_api {
    const struct px_pokemon_api *memory;
    void *user;
    uint32_t (*move_value)(void *user, uint32_t move, uint32_t selector); /* 00168bbc */
};

/* 00150220: unsigned PP Ups clamp to 3, low-byte base PP and result.
 * Only move_value is required; memory may be NULL for this scalar routine. */
uint32_t px_pokemon_maximum_pp(uint32_t move, uint32_t pp_ups,
                              const struct px_move_api *api);

/* 00150064: constructor move insertion. Skip a known move, fill the first
 * empty slot, otherwise discard slot 0 and shift slots 1..3 down. Moves,
 * remaining PP and PP Ups move together. This is not the level-up prompt. */
void px_pokemon_learn_move(uint32_t object, uint32_t move,
                           const struct px_move_api *api);

#endif
