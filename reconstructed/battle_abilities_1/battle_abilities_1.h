#ifndef PX_BATTLE_ABILITIES_1_H
#define PX_BATTLE_ABILITIES_1_H

#include "../battle_ability/battle_ability.h"

/* DllBattle contact abilities: handlers of event 0x53, which runs after a
 * move has hit (static analysis addresses; runtime addresses are
 * module-relative). Analyst names. Event keys: 0x03 attacker, 0x04 defender,
 * 0x12 move, 0x16 move type, 0x1b move category (1 physical), 0x46 critical
 * hit, 0x47 and 0x48 suppress the after-effects. Statuses: 1 paralysis,
 * 2 sleep, 4 burn, 5 poison, 7 infatuation. Stats: 1 Attack, 2 Defense,
 * 5 Speed. pokemon_value keys: 0xd HP, 0xe max HP, 0x10 ability, 0x11
 * effective ability, 0x12 gender (2 none). */
struct px_battle_abilities_1_api {
    /* Event stack, battler lookup and pokemon_value (10874420). */
    const struct px_battle_ability_api *base;
    uint32_t (*move_flag)(void *user, uint32_t move, uint32_t flag);                  /* 1080049c; 0 contact */
    uint32_t (*has_type)(void *user, uint32_t pokemon, uint32_t type);                /* 1086f960 */
    uint32_t (*held_item)(void *user, uint32_t context, uint32_t id);                 /* 1084ae18; 0 when suppressed */
    uint32_t (*random)(void *user, uint32_t range);                                   /* 1084c7bc */
    uint32_t (*chance)(void *user, uint32_t percent);                                 /* 1084c0f8 */
    uint32_t (*forced)(void *user, uint32_t context, uint32_t flag);                  /* 1084c0ec */
    uint32_t (*condition)(void *user, uint32_t status);                               /* 10860e00 */
    void (*condition_for)(void *user, uint32_t status, uint32_t pokemon, uint32_t out); /* 1084c438; word at out */
    uint32_t (*battle_decided)(void *user, uint32_t context);                         /* 10865c48 */
    uint32_t (*is_ally)(void *user, uint32_t id, uint32_t other);                     /* 1084c5a8 */
    int32_t (*stage_room)(void *user, uint32_t pokemon, uint32_t stat);               /* 1084b82c; stages to +6 */
    uint32_t (*stage_can_change)(void *user, uint32_t pokemon, uint32_t stat, int32_t delta); /* 108722e8 */
    void (*ability_popup)(void *user, uint32_t context, uint32_t phase, uint32_t id); /* 1084c948; 1 show, 2 hide */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*message)(void *user, uint32_t message, uint32_t kind, uint32_t id);        /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t message, uint32_t value);                /* 1084d29c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);            /* 10867b48 */
    /* Logical stack word standing in for Cute Charm's frame (sp + 4). */
    uint32_t condition_scratch;
};

/* 1085f844: no HP left. */
uint32_t px_battle_fainted(uint32_t pokemon, const struct px_battle_abilities_1_api *api);
/* 1084d2f4: max HP / divisor, at least 1. */
uint32_t px_battle_hp_share(uint32_t pokemon, uint32_t divisor, const struct px_battle_abilities_1_api *api);
/* 1084ad50: a contact move hitting `id` gives the attacker `status` with
 * `percent` chance (or when forced). */
void px_battle_contact_status(uint32_t context, uint32_t id, uint32_t status, uint32_t condition, uint32_t percent,
                              const struct px_battle_abilities_1_api *api);

#define PX_BATTLE_ABILITIES_1_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_abilities_1_api *api)
PX_BATTLE_ABILITIES_1_HANDLER(px_battle_static);        /* 108be9d0, ability 9 */
PX_BATTLE_ABILITIES_1_HANDLER(px_battle_poison_point);  /* 108bfcf8, ability 38 */
PX_BATTLE_ABILITIES_1_HANDLER(px_battle_flame_body);    /* 108c0edc, ability 49 */
PX_BATTLE_ABILITIES_1_HANDLER(px_battle_effect_spore);  /* 108bc8dc, ability 27 */
PX_BATTLE_ABILITIES_1_HANDLER(px_battle_cute_charm);    /* 108c0f60, ability 56 */
PX_BATTLE_ABILITIES_1_HANDLER(px_battle_poison_touch);  /* 108bd6e0, ability 143 */
PX_BATTLE_ABILITIES_1_HANDLER(px_battle_rough_skin);    /* 108be8f8, abilities 24 and 160 */
PX_BATTLE_ABILITIES_1_HANDLER(px_battle_aftermath);     /* 108bde94, ability 106 */
PX_BATTLE_ABILITIES_1_HANDLER(px_battle_mummy);         /* 108bc9a8, ability 152 */
PX_BATTLE_ABILITIES_1_HANDLER(px_battle_gooey);         /* 108be5f8, ability 183 */
PX_BATTLE_ABILITIES_1_HANDLER(px_battle_rattled);       /* 108bcd28, ability 155 */
PX_BATTLE_ABILITIES_1_HANDLER(px_battle_justified);     /* 108c19bc, ability 154 */
PX_BATTLE_ABILITIES_1_HANDLER(px_battle_anger_point);   /* 108c05ec, ability 83 */
PX_BATTLE_ABILITIES_1_HANDLER(px_battle_weak_armor);    /* 108c1784, ability 133 */

#endif
