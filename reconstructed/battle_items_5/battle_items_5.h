#ifndef PX_BATTLE_ITEMS_5_H
#define PX_BATTLE_ITEMS_5_H

#include "../battle_ability/battle_ability.h"

/* DllBattle item handlers, fifth batch: held items that multiply a stat or
 * raise the critical-hit stage, with those items' other entries (static
 * analysis addresses; runtime addresses are module-relative). Analyst names.
 * Item ids are the original ids; species are national dex numbers.
 *
 * Events and keys: 0x13 speed (key 0x02 the holder), 0x40 attacking stat
 * (key 0x03 the attacker), 0x41 defending stat (key 0x04 the defender); all
 * three multiply key 0x36 (1084ce84). 0x3b raises the critical-hit stage
 * key 0x2d (rewritten through 108658d4). Key 0x12 is the move, key 0x1b the
 * move's category (1 physical, 2 special), 108007dc the move's category from
 * its data. Battlers: species u16 +0xc, form byte +0x153.
 *
 * Other entries: the Choice items lock the move on event 0x22 and release it
 * on 0xa8; Iron Ball sets key 0x42 (grounded) on 0x12; Light Ball's
 * event-0x7c use paralyzes. */
struct px_battle_items_5_api {
    /* Event stack, battler lookup and move_category. */
    const struct px_battle_ability_api *base;
    uint32_t (*transformed)(void *user, uint32_t pokemon);                              /* 10851c28 */
    uint32_t (*can_evolve)(void *user, uint32_t context, uint32_t id);                  /* 1081caf0 */
    uint32_t (*lock_condition)(void *user, uint32_t move);                              /* 1084c3b0 */
    uint32_t (*condition)(void *user, uint32_t status);                                 /* 10860e00 */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);              /* 10867b48 */
};

#define PX_BATTLE_ITEMS_5_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_items_5_api *api)
/* Event 0x13, speed. */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_macho_brace);     /* 108a802c, item 215: x0.5 */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_quick_powder);    /* 108a73e0, item 274: Ditto (132), not transformed: x2 */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_iron_ball_speed); /* 108ab0d4, item 278: x0.5 */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_choice_scarf);    /* 108a7fd4, item 287: x1.5 */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_power_item);      /* 108ab168, items 289-294: x0.5 */
/* Event 0x3b, critical-hit stage. */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_scope_lens);      /* 108a63ac, items 232 and 326 (Razor Claw): +1 */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_lucky_punch);     /* 108a6b90, item 256: Chansey (113): +2 */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_stick);           /* 108a629c, item 259: Farfetch'd (83): +2 */
/* Event 0x40, the attacker's stat. */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_choice_band);     /* 108ab3c4, item 220: physical x1.5 */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_choice_specs);    /* 108aa5c4, item 297: special x1.5 */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_soul_dew_attack); /* 108aa608, item 225: Latias or Latios (380, 381), key 0x1b special: x1.5 */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_deep_sea_tooth);  /* 108a7b34, item 226: Clamperl (366), special: x2 */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_light_ball);      /* 108a64bc, item 236: Pikachu (25): x2 */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_thick_club);      /* 108a6564, item 258: Cubone or Marowak (104, 105), physical: x2 */
/* Event 0x41, the defender's stat (key 0x1b the move's category). */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_soul_dew_defense); /* 108ab074, item 225: Latias or Latios, special: x1.5 */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_deep_sea_scale);  /* 108a80a8, item 227: Clamperl, special: x2 */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_metal_powder);    /* 108a7188, item 257: Ditto, not transformed, physical: x2 */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_eviolite);        /* 108a8104, item 538: can still evolve: x1.5 (not Floette 670 form 5) */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_assault_vest);    /* 108a8b0c, item 640: special x1.5 */
/* Other entries. */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_choice_lock);     /* 108ab898, items 220, 287, 297, event 0x22 */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_choice_release);  /* 108abb94, items 220, 287, 297, event 0xa8 */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_iron_ball_ground); /* 108ab408, item 278, event 0x12 */
PX_BATTLE_ITEMS_5_HANDLER(px_battle_light_ball_use);  /* 108a95c0, item 236, event 0x7c */

#endif
