#ifndef PX_BATTLE_ABILITIES_6_H
#define PX_BATTLE_ABILITIES_6_H

#include "../battle_abilities_1/battle_abilities_1.h"

/* DllBattle status-cure and secondary-effect abilities (static analysis
 * addresses; runtime addresses are module-relative). Analyst names.
 * Event 0x02 runs after a status changes; its handlers cure their own
 * statuses through 1084aba4. The secondary-effect handlers check the
 * attacker (key 0x03) or target (key 0x04, or the target list: count key
 * 0x05, targets from key 0x06) and set a "no effect" key: 0x42, 0x48 or
 * 0x49. Statuses: 1 paralysis, 2 sleep, 3 freeze, 4 burn, 5 poison,
 * 6 confusion, 7 infatuation, 0xb taunt. Switch-out (event 0x5c) and
 * status-damage (event 0x74, damage key 0x33, status key 0x1e) handlers are
 * included; fainted (1085f844) and the max-HP share (1084d2f4) come from
 * battle_abilities_1. pokemon_value keys: 0xd HP, 0xe max HP. */
struct px_battle_abilities_6_api {
    /* Event stack, battler lookup and pokemon_value (10874420). */
    const struct px_battle_ability_api *base;
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status);            /* 10856a88 */
    uint32_t (*move_kind)(void *user, uint32_t move);                                 /* 108007d4 */
    uint32_t (*move_value)(void *user, uint32_t move, uint32_t field);                /* 10800494; 0xa effect chance, 0xb */
    uint32_t (*stat_change_count)(void *user, uint32_t move);                         /* 108014bc */
    void (*stat_change)(void *user, uint32_t move, uint32_t index, uint32_t out);     /* 108014c4; signed stages at out */
    void (*ability_popup)(void *user, uint32_t context, uint32_t phase, uint32_t id); /* 1084c948; 1 show, 2 hide */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);            /* 10867b48 */
    /* Logical stack word standing in for 1084a890's frame (sp). */
    uint32_t stat_scratch;
};

/* 1084aba4: if the battler has `status`, cure it inside the ability popup. */
void px_battle_cure_status(uint32_t context, uint32_t id, uint32_t status, const struct px_battle_abilities_6_api *api);
/* 1084a890: the move has a secondary effect Sheer Force removes. */
uint32_t px_battle_secondary_effect(uint32_t move, const struct px_battle_abilities_6_api *api);
/* 1084d318: HP equals max HP. */
uint32_t px_battle_full_hp(uint32_t pokemon, const struct px_battle_abilities_6_api *api);
/* 1089e6fc: the signed larger of two values. */
int32_t px_battle_max(int32_t value, int32_t minimum);

#define PX_BATTLE_ABILITIES_6_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_abilities_6_api *api)
/* Event 0x02. */
PX_BATTLE_ABILITIES_6_HANDLER(px_battle_insomnia_cure);    /* 1084ab94, abilities 15 and 72 */
PX_BATTLE_ABILITIES_6_HANDLER(px_battle_limber_cure);      /* 108c1774, ability 7 */
PX_BATTLE_ABILITIES_6_HANDLER(px_battle_immunity_cure);    /* 108c18b8, ability 17 */
PX_BATTLE_ABILITIES_6_HANDLER(px_battle_own_tempo_cure);   /* 108c18c8, ability 20 */
PX_BATTLE_ABILITIES_6_HANDLER(px_battle_water_veil_cure);  /* 108c3ba8, ability 41 */
PX_BATTLE_ABILITIES_6_HANDLER(px_battle_magma_armor_cure); /* 108c4f50, ability 40 */
PX_BATTLE_ABILITIES_6_HANDLER(px_battle_oblivious_cure);   /* 108c1578, ability 12 */
PX_BATTLE_ABILITIES_6_HANDLER(px_battle_truant_sleep);     /* 108c18d8, ability 54 */
/* Shield Dust (19). */
PX_BATTLE_ABILITIES_6_HANDLER(px_battle_shield_dust_effect);  /* 108c08b8, event 0x59 */
PX_BATTLE_ABILITIES_6_HANDLER(px_battle_shield_dust_status);  /* 108c08f4, event 0x6d */
PX_BATTLE_ABILITIES_6_HANDLER(px_battle_shield_dust_poison);  /* 108c11c0, event 0x52 */
PX_BATTLE_ABILITIES_6_HANDLER(px_battle_shield_dust_flinch);  /* 108c1990, event 0x76 */
PX_BATTLE_ABILITIES_6_HANDLER(px_battle_shield_dust_targets); /* 108c43a0, event 0x8b */
/* Sheer Force (125). */
PX_BATTLE_ABILITIES_6_HANDLER(px_battle_sheer_force_power);   /* 108c3e88, event 0x8b */
PX_BATTLE_ABILITIES_6_HANDLER(px_battle_sheer_force_effect);  /* 108c5178, events 0x6d and 0x59 */
PX_BATTLE_ABILITIES_6_HANDLER(px_battle_sheer_force_flinch);  /* 108c566c, event 0x75 */
/* Switch-out (event 0x5c) and status damage (event 0x74). */
PX_BATTLE_ABILITIES_6_HANDLER(px_battle_natural_cure);       /* 108c11ec, ability 30 */
PX_BATTLE_ABILITIES_6_HANDLER(px_battle_regenerator);        /* 108c0940, ability 144 */
PX_BATTLE_ABILITIES_6_HANDLER(px_battle_poison_heal);        /* 108c0140, ability 90 */
PX_BATTLE_ABILITIES_6_HANDLER(px_battle_heatproof_burn);     /* 108c349c, ability 85 */
/* Stench (1). */
PX_BATTLE_ABILITIES_6_HANDLER(px_battle_stench);              /* 108bd678, event 0x75 */

#endif
