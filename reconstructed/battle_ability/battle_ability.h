#ifndef PX_BATTLE_ABILITY_H
#define PX_BATTLE_ABILITY_H

#include "../battle_event/battle_event.h"

/* DllBattle ability handlers for the attack (0x40) and defense (0x41)
 * multiplier events (static analysis addresses; runtime addresses are
 * module-relative). Analyst names. Logical ARM32 addresses.
 * Event keys: 0x03 attacker id, 0x04 defender id, 0x12 move id, 0x16 move
 * type, 0x1b move category byte, 0x34 attack value, 0x36 multiplier,
 * 0x54 burn exception. Types: 6 Bug, 9 Fire, 10 Water, 11 Grass, 14 Ice.
 * Move categories: 1 physical, 2 special. */
struct px_battle_ability_api {
    /* Event stack, memory, user context and the abort helper 10800394. */
    const struct px_battle_event_api *events;
    uint32_t (*pokemon_value)(void *user, uint32_t pokemon, uint32_t key);  /* 10874420; 0xd HP, 0xe max HP, 0x11 ability */
    uint32_t (*move_category)(void *user, uint32_t move);                  /* 108007dc -> 0039ddc0 */
    uint32_t (*pokemon_status)(void *user, uint32_t pokemon);              /* 1086d8ac */
    uint32_t (*side_of)(void *user, uint32_t context, uint32_t id);        /* 10873158; 0xa means none */
    uint32_t (*side_battlers)(void *user, uint32_t context, uint32_t selector,
                              uint32_t out);                               /* 1084bc20; ids as bytes, returns count */
    uint32_t (*field_check)(void *user, uint32_t context);                 /* 1084b8d4 */
};

/* Event-stack services. */
void px_battle_event_multiply(uint32_t key, uint32_t factor, const struct px_battle_event_api *api);       /* 1084ce84 */
uint32_t px_battle_event_rewrite(uint32_t key, uint32_t value, const struct px_battle_event_api *api);     /* 108658d4 */
/* Battle helpers. */
uint32_t px_battle_pokemon_by_id(uint32_t context, uint32_t id, const struct px_battle_ability_api *api);  /* 108544a0 */
uint32_t px_battle_hp_fraction(uint32_t pokemon, uint32_t divisor, const struct px_battle_ability_api *api); /* 1084c124 */
uint32_t px_battle_modify(uint32_t value, uint32_t factor);                                               /* 1089e708 */

/* Handlers: (handler, context, battler id, work area), as called by the
 * event walk. Ability ids in the original table are noted. */
#define PX_BATTLE_ABILITY_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_ability_api *api)
PX_BATTLE_ABILITY_HANDLER(px_battle_thick_fat);     /* 108bf010, ability 47 */
PX_BATTLE_ABILITY_HANDLER(px_battle_huge_power);    /* 108c0258, abilities 37 and 74 */
PX_BATTLE_ABILITY_HANDLER(px_battle_blaze);         /* 108bca9c, ability 66 */
PX_BATTLE_ABILITY_HANDLER(px_battle_torrent);       /* 108bd838, ability 67 */
PX_BATTLE_ABILITY_HANDLER(px_battle_overgrow);      /* 108bea50, ability 65 */
PX_BATTLE_ABILITY_HANDLER(px_battle_swarm);         /* 108c1030, ability 68 */
PX_BATTLE_ABILITY_HANDLER(px_battle_guts);          /* 108bd110, ability 62 */
PX_BATTLE_ABILITY_HANDLER(px_battle_marvel_scale);  /* 108c1648, ability 63 */
PX_BATTLE_ABILITY_HANDLER(px_battle_hustle);        /* 108c3a64, ability 55 (event 0x40 entry) */
PX_BATTLE_ABILITY_HANDLER(px_battle_plus_minus);    /* 1084cd44, abilities 57 and 58 */
PX_BATTLE_ABILITY_HANDLER(px_battle_defeatist);     /* 108bd618, ability 129 */
PX_BATTLE_ABILITY_HANDLER(px_battle_fur_coat);      /* 108bd7fc, ability 169 */
PX_BATTLE_ABILITY_HANDLER(px_battle_grass_pelt);    /* 108c0f10, ability 179 */

#endif
