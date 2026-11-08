#ifndef PX_BATTLE_ABILITIES_4_H
#define PX_BATTLE_ABILITIES_4_H

#include "../battle_ability/battle_ability.h"

/* DllBattle move-calculation abilities: accuracy, critical hits, speed,
 * weight and other values a move's calculation reads (static analysis
 * addresses; runtime addresses are module-relative). Analyst names. Each
 * handler checks one battler key (0x02 the battler concerned, 0x03 the
 * attacker, 0x04 the defender) and then multiplies key 0x36 (1084ce84) or
 * rewrites a key (108658d4). Battlers: held item +0x12. */
struct px_battle_abilities_4_api {
    /* Event stack, battler lookup, move_category (108007dc) and pokemon_status (1086d8ac). */
    const struct px_battle_ability_api *base;
    uint32_t (*is_ally)(void *user, uint32_t id, uint32_t other);                     /* 1084c5a8 */
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status);            /* 10856a88; 6 confusion */
    uint32_t (*move_flag)(void *user, uint32_t move, uint32_t flag);                  /* 1080049c */
    uint32_t (*bond_excluded)(void *user, uint32_t move);                             /* 108a4814; a 7-move list */
};

#define PX_BATTLE_ABILITIES_4_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_abilities_4_api *api)
/* Accuracy (events 0x39 and 0x38). */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_compound_eyes);   /* 108bd7cc, ability 14, event 0x39 */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_victory_star);    /* 108be2c0, ability 162, event 0x39 */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_hustle_accuracy); /* 108c3ab8, ability 55, event 0x39 */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_tangled_feet);    /* 108bf910, ability 77, event 0x39 */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_wonder_skin);     /* 108c0764, ability 147, event 0x39 */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_keen_eye);        /* 108c3d64, ability 51, event 0x38 */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_unaware_stages);  /* 108c23f4, ability 109, event 0x38 */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_unaware_defense); /* 108c239c, ability 109, event 0x3e */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_unaware_attack);  /* 108c23c8, ability 109, event 0x3f */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_scrappy);         /* 108bf554, ability 113, event 0x44 */
/* Critical hits (event 0x3b). */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_super_luck);      /* 108bd17c, ability 105 */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_battle_armor);    /* 108c0700, abilities 4 and 75 */
/* Speed and order (events 0x0f, 0x13, 0xa8). */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_stall);           /* 108bd6b4, ability 100, event 0x0f */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_quick_feet);      /* 108bd8a8, ability 95, event 0x13 */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_unburden);        /* 108c31ac, ability 84, event 0x13 */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_slow_start_speed);/* 108c3cd0, ability 112, event 0x13 */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_unburden_item);   /* 108c551c, ability 84, event 0xa8 */
/* Weight (event 0x84). */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_heavy_metal);     /* 108bfde4, ability 134 */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_light_metal);     /* 108bfed8, ability 135 */
/* Damage, hits and secondary effects. */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_magic_guard);     /* 108bff04, ability 98, event 0x89 */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_rock_head);       /* 108be4ec, ability 69, event 0x58 */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_skill_link);      /* 108bf8cc, ability 92, event 0x3a */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_parental_bond);   /* 108bddcc, ability 185, event 0x3a */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_adaptability);    /* 108c0a74, ability 91, event 0x49 */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_inner_focus);     /* 108c09f8, ability 39, event 0x76 */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_serene_grace);    /* 108c0ab0, ability 32, events 0x6d, 0x59, 0xb5 */
PX_BATTLE_ABILITIES_4_HANDLER(px_battle_serene_grace_flinch); /* 108c46dc, ability 32, event 0x75 */

#endif
