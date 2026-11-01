#ifndef PX_BATTLE_IMMUNITY_H
#define PX_BATTLE_IMMUNITY_H

#include "../battle_ability/battle_ability.h"

/* DllBattle handlers of event 0x31, which decides whether a move fails
 * against its target (static analysis addresses; runtime addresses are
 * module-relative). Analyst names. Every ability, item and move entry for
 * event 0x31 is covered. Keys: 0x03 attacker, 0x04 defender, 0x12 move id,
 * 0x16 type, 0x39 effectiveness code, 0x41 move blocked, 0x4f and 0x53
 * reflection. Battlers keep their species at +0xc and state bits at +0x169
 * (bit 13: Flash Fire active). Types: 9 Fire, 10 Water, 12 Electric. */
struct px_battle_immunity_api {
    /* Event stack and battler lookup. */
    const struct px_battle_ability_api *base;
    uint32_t (*move_kind)(void *user, uint32_t move);                       /* 108007d4 */
    uint32_t (*stat_change_count)(void *user, uint32_t move);               /* 108014bc */
    void (*stat_change)(void *user, uint32_t move, uint32_t index, uint32_t out); /* 108014c4; signed stages at out */
    uint32_t (*move_flag)(void *user, uint32_t move, uint32_t flag);        /* 1080049c; 4 reflectable, 8 sound */
    uint32_t (*damaging_move)(void *user, uint32_t move);                   /* 10800434 */
    uint32_t (*powder_move)(void *user, uint32_t move);                     /* 108a47f4 */
    uint32_t (*is_ally)(void *user, uint32_t id, uint32_t other);           /* 1084c5a8 */
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status);  /* 10856a88 */
    uint32_t (*mega_gengar)(void *user, uint32_t pokemon);                  /* 108c8540 */
    uint32_t (*reflection_blocked)(void *user, uint32_t pokemon);           /* 1084b804 */
    void (*reflect)(void *user, uint32_t context, uint32_t id, uint32_t attacker); /* 1084b3f8 */
    void (*heal_fraction)(void *user, uint32_t context, uint32_t id, uint32_t divisor); /* 1084aaac */
    void (*raise_stat)(void *user, uint32_t context, uint32_t id, uint32_t stat, uint32_t stages); /* 10867a88 */
    void (*ability_popup)(void *user, uint32_t context, uint32_t phase, uint32_t id); /* 1084c948; 1 show, 2 hide */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*message)(void *user, uint32_t message, uint32_t kind, uint32_t id); /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t message, uint32_t value);      /* 1084d29c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);  /* 10867b48 */
    uint32_t scratch;   /* logical word standing in for the original stack slot */
};

/* 10865824: the shared absorb test. Blocks (key 0x41) a move of the type
 * aimed at this battler by another; a status move (kind 2) only when one of
 * its stat changes is negative or it has none. Returns the rewrite result. */
uint32_t px_battle_absorbs(uint32_t context, uint32_t id, uint32_t type, const struct px_battle_immunity_api *api);

#define PX_BATTLE_IMMUNITY_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_immunity_api *api)
PX_BATTLE_IMMUNITY_HANDLER(px_battle_water_absorb);  /* 108c1320, ability 11 */
PX_BATTLE_IMMUNITY_HANDLER(px_battle_volt_absorb);   /* 108c2db4, ability 10 */
PX_BATTLE_IMMUNITY_HANDLER(px_battle_dry_skin_absorb); /* 108c3170, ability 87 */
PX_BATTLE_IMMUNITY_HANDLER(px_battle_motor_drive);   /* 108c485c, ability 78 */
PX_BATTLE_IMMUNITY_HANDLER(px_battle_lightning_rod); /* 108c54dc, ability 31 */
PX_BATTLE_IMMUNITY_HANDLER(px_battle_storm_drain);   /* 108c5698, ability 114 */
PX_BATTLE_IMMUNITY_HANDLER(px_battle_flash_fire);    /* 108c53cc, ability 18 */
PX_BATTLE_IMMUNITY_HANDLER(px_battle_wonder_guard);  /* 108c1d5c, ability 25 */
PX_BATTLE_IMMUNITY_HANDLER(px_battle_soundproof);    /* 108bc750, ability 43 */
PX_BATTLE_IMMUNITY_HANDLER(px_battle_oblivious);     /* 108c3888, ability 12 */
PX_BATTLE_IMMUNITY_HANDLER(px_battle_sticky_hold);   /* 108c213c, ability 60 */
PX_BATTLE_IMMUNITY_HANDLER(px_battle_telepathy);     /* 108be0c8, ability 140 */
PX_BATTLE_IMMUNITY_HANDLER(px_battle_overcoat);      /* 108c47b0, ability 142 */
PX_BATTLE_IMMUNITY_HANDLER(px_battle_magic_bounce);  /* 108c3244, ability 156 */
PX_BATTLE_IMMUNITY_HANDLER(px_battle_safety_goggles);/* 108aba84, item 650 */
PX_BATTLE_IMMUNITY_HANDLER(px_battle_magic_coat);    /* 108b7570, move 277 */
PX_BATTLE_IMMUNITY_HANDLER(px_battle_telekinesis);   /* 108bb7dc, move 477 */

#endif
