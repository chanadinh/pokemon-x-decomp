#ifndef PX_BATTLE_STATS_H
#define PX_BATTLE_STATS_H

#include "../pokemon_constructor/pokemon_constructor.h"

/* DllBattle attack and defense values for the damage calculation (static
 * analysis addresses 108202d8 and 10822330). Analyst names; logical ARM32
 * addresses. Battle Pokemon: id byte +0x19, signed stat stages at +0x104..+0x107
 * (6 is neutral). Move: id u16 +0, type byte +6, byte +7. Context: +8 -> table
 * whose words +0x84 + 4*n are battle Pokemon, +0x1c -> field state (weather +1). */
struct px_battle_stats_api {
    const struct px_pokemon_api *memory;
    void *user;
    void (*event_push)(void *user);                                        /* 1086bb84 */
    void (*event_pop)(void *user);                                         /* 1086b880 */
    void (*event_set_id)(void *user, uint32_t key, uint32_t value);        /* 1086bac0 */
    void (*event_set_value)(void *user, uint32_t key, uint32_t value);     /* 1086b9fc */
    void (*event_set_scaled)(void *user, uint32_t key, uint32_t value);    /* 1086b520 */
    void (*event_set_multiplier)(void *user, uint32_t key, uint32_t value,
                                 uint32_t minimum, uint32_t maximum);       /* 10867c9c */
    void (*event_dispatch)(void *user, uint32_t context, uint32_t event);  /* 1086b9a4 */
    uint32_t (*event_get)(void *user, uint32_t key);                       /* 1086b91c */
    uint32_t (*stat)(void *user, uint32_t pokemon, uint32_t key);          /* 10874420: with stages */
    uint32_t (*raw_stat)(void *user, uint32_t pokemon, uint32_t key);      /* 108743a8: without stages */
    uint32_t (*stage_key)(void *user, uint32_t pokemon, uint32_t key);     /* 108620d4 */
    uint32_t (*has_type)(void *user, uint32_t pokemon, uint32_t type);     /* 1086f960 */
    uint32_t (*move_category)(void *user, uint32_t move);                  /* 0039ddc0 via veneer 108007dc */
    void (*abort)(void *user);                                             /* 10800394(0, 0, 0); returns */
};

/* Stat keys: 8 attack, 9 defense, 10 special attack, 11 special defense.
 * Category 2 selects the special stats. */

/* 108202d8: returns the 16-bit attack value and writes whether event key 0x54
 * was set (a burn exception) as a byte to flag. */
uint32_t px_battle_attack_value(uint32_t context, uint32_t attacker, uint32_t defender, uint32_t move,
                                uint32_t critical, uint32_t flag, const struct px_battle_stats_api *api);
/* 10822330: returns the 16-bit defense value. */
uint32_t px_battle_defense_value(uint32_t context, uint32_t attacker, uint32_t defender, uint32_t move,
                                 uint32_t critical, const struct px_battle_stats_api *api);

#endif
