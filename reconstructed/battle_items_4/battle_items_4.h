#ifndef PX_BATTLE_ITEMS_4_H
#define PX_BATTLE_ITEMS_4_H

#include "../battle_ability/battle_ability.h"

/* DllBattle item handlers, fourth batch: the gems, items 548-564 and 715
 * (static analysis addresses; runtime addresses are module-relative).
 * Analyst names. Item ids are the original ids.
 *
 * Each gem has three handlers. On event 0x8a (before the move) its stub
 * calls 1084c284 with the gem's type: when the holder (key 0x03) uses a
 * damaging move of that type, the gem is marked consumed (handler byte +0x10
 * = 6), a kind-0x24 record shows message 0xbc and work word 0 becomes 1. On
 * event 0x3d its handler multiplies the power (key 0x32) by 0x14cd (x1.3)
 * for that type when work word 0 is 1 or the pass does not consume items.
 * On event 0x91 one shared handler turns work word 0 from 1 to 2.
 *
 * Types: 0 Normal, 1 Fighting, 2 Flying, 3 Poison, 4 Ground, 5 Rock, 6 Bug,
 * 7 Ghost, 8 Steel, 9 Fire, 10 Water, 11 Grass, 12 Electric, 13 Psychic,
 * 14 Ice, 15 Dragon, 16 Dark, 17 Fairy. */
struct px_battle_items_4_api {
    /* Event stack and multiplier update. */
    const struct px_battle_ability_api *base;
    uint32_t (*no_consume)(void *user, uint32_t context);                               /* 1084c70c */
    uint32_t (*damaging_move)(void *user, uint32_t move);                               /* 10800434 */
    uint32_t (*fixed_target)(void *user, uint32_t move);                                /* 1084c720 */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*message)(void *user, uint32_t message, uint32_t kind, uint32_t id);          /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t message, uint32_t value);                  /* 1084d29c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);              /* 10867b48 */
};

/* 1084c700: the handler's item is marked consumed (byte +0x10 = 6). */
void px_battle_item_mark_consumed(uint32_t handler, const struct px_pokemon_api *memory);
/* 10881638: whether it is (byte +0x10 is 6). */
uint32_t px_battle_item_consumed(uint32_t handler, const struct px_pokemon_api *memory);
/* 1084c284: a gem's event-0x8a use for one type (the fifth, stack argument). */
void px_battle_gem_use(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, uint32_t type,
                       const struct px_battle_items_4_api *api);

#define PX_BATTLE_ITEMS_4_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_items_4_api *api)
/* Event 0x91, every gem: work word 0 from 1 (used) to 2. */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_gem_spent);            /* 108a85cc */
/* Per gem: event 0x3d power, event 0x8a use. */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_fire_gem_power);       /* 108a8ec0, item 548 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_fire_gem_use);         /* 108aa510 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_water_gem_power);      /* 108a87bc, item 549 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_water_gem_use);        /* 108aa120 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_electric_gem_power);   /* 108a8d78, item 550 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_electric_gem_use);     /* 108aa46c */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_grass_gem_power);      /* 108a86b0, item 551 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_grass_gem_use);        /* 108aa048 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_ice_gem_power);        /* 108a9168, item 552 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_ice_gem_use);          /* 108aa668 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_fighting_gem_power);   /* 108a9104, item 553 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_fighting_gem_use);     /* 108aa564 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_poison_gem_power);     /* 108a84cc, item 554 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_poison_gem_use);       /* 108a9de8 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_ground_gem_power);     /* 108a903c, item 555 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_ground_gem_use);       /* 108aa550 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_flying_gem_power);     /* 108a8e5c, item 556 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_flying_gem_use);       /* 108aa480 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_psychic_gem_power);    /* 108a7d50, item 557 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_psychic_gem_use);      /* 108a9684 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_bug_gem_power);        /* 108a8870, item 558 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_bug_gem_use);          /* 108aa134 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_rock_gem_power);       /* 108a7ea8, item 559 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_rock_gem_use);         /* 108a97fc */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_ghost_gem_power);      /* 108a7db4, item 560 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_ghost_gem_use);        /* 108a9698 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_dragon_gem_power);     /* 108a9620, item 561 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_dragon_gem_use);       /* 108aa948 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_dark_gem_power);       /* 108a7cd0, item 562 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_dark_gem_use);         /* 108a958c */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_steel_gem_power);      /* 108a96ac, item 563 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_steel_gem_use);        /* 108aa95c */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_normal_gem_power);     /* 108a88dc, item 564 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_normal_gem_use);       /* 108aa148 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_fairy_gem_power);      /* 108a8aa8, item 715 */
PX_BATTLE_ITEMS_4_HANDLER(px_battle_fairy_gem_use);        /* 108aa1f4 */

#endif
