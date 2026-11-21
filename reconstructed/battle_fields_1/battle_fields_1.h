#ifndef PX_BATTLE_FIELDS_1_H
#define PX_BATTLE_FIELDS_1_H

#include "../battle_items_1/battle_items_1.h"
#include "../battle_abilities_1/battle_abilities_1.h"

/* DllBattle side-effect handlers: the 17 rows of the side-effect table
 * (108d4fe8, 12 bytes each: id, table function, maximum layers) with 20
 * handlers (static analysis addresses; runtime addresses are
 * module-relative). The names are analyst interpretations of what each row
 * does; the ids are the table's.
 *
 * A handler's id parameter is the side (0 or 1) it was registered for. A
 * battler slot belongs to side 1 from slot 12 up (1084bfc0). Keys: 0x02
 * (or 0x03, 0x04) a battler, 0x12 the move, 0x1b its category (1 physical, 2
 * special), 0x36 a multiplier, 0x39 effectiveness (7 neutral), 0x40 a
 * message buffer, 0x41 move blocked, 0x42 and 0x46 flags set to 1, 0x46
 * critical hit. The handler's value halfword (+0x38) holds the row's id,
 * which indexes the layer counts at 10b11c00: 17 rows of 16 bytes per side,
 * the layer byte at +0xc. */
struct px_battle_fields_1_api {
    /* Event stack, battler lookup, records, messages and commit. */
    const struct px_battle_items_api *items;
    /* For 1084d2f4 (max HP / divisor, battle_abilities_1). */
    const struct px_battle_abilities_1_api *abilities;
    /* Logical address of the layer counts (the literal 10b11c00). */
    uint32_t layers;
    uint32_t (*airborne)(void *user, uint32_t context, uint32_t id);                  /* 1084bf24 */
    uint32_t (*slot_present)(void *user, uint32_t context, uint32_t id);              /* 1084bfa0 */
    uint32_t (*battle_decided)(void *user, uint32_t context);                         /* 10865c48 */
    uint32_t (*side_of_position)(void *user, uint32_t context, uint32_t id);          /* 10873158 */
    uint32_t (*has_type)(void *user, uint32_t pokemon, uint32_t type);                /* 1086f960 */
    uint32_t (*packed_types)(void *user, uint32_t pokemon);                           /* 10870994 */
    uint32_t (*effectiveness)(void *user, uint32_t type, uint32_t packed);            /* 10870900; Rock is 5 */
    uint32_t (*move_value)(void *user, uint32_t move, uint32_t field);                /* 10800494; 0x1b target class */
    uint32_t (*damaging_move)(void *user, uint32_t move);                             /* 10800434 */
    uint32_t (*move_flag)(void *user, uint32_t move, uint32_t flag);                  /* 1080049c */
};

/* 1084bfc0: the side of a battler slot (1 from slot 12). */
uint32_t px_battle_slot_side(uint32_t slot);
/* 1084bf04: a major status (below 6). */
uint32_t px_battle_major_status(uint32_t status);
/* 1084bf14: the battle mode, byte +1 of the structure behind context +4's
 * pointer at +0x10. */
uint32_t px_battle_mode(uint32_t context, const struct px_pokemon_api *memory);
/* 108802a8: set bit 15 of the halfword at +2 of a message buffer. */
void px_battle_message_flag(uint32_t buffer, const struct px_pokemon_api *memory);

#define PX_BATTLE_FIELDS_1_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t side, uint32_t work, const struct px_battle_fields_1_api *api)
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_reflect);        /* 108ac688, row 0, event 0x4f */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_light_screen);   /* 108aca28, row 1, event 0x4f */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_safeguard);      /* 108acc94, row 2, event 0x6e */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_safeguard_show); /* 108acc24, row 2, event 0x70 */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_mist);           /* 108acbc8, row 3, event 0x64 */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_mist_show);      /* 108acb2c, row 3, event 0x65 */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_tailwind);       /* 108ac16c, row 4, event 0x13: speed x2 */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_lucky_chant);    /* 108ac568, row 5, event 0x3b */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_spikes);         /* 108ac460, row 6, event 0x5e */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_toxic_spikes);   /* 108ac300, row 7, event 0x5e */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_stealth_rock);   /* 108ac8fc, row 8, event 0x5e */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_wide_guard);     /* 108ac704, row 9, event 0x2e */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_quick_guard);    /* 108ac5e4, row 10, events 0x2e and 0xb2 */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_rainbow_chance); /* 108ac02c, row 11, events 0x6d and 0x59 */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_rainbow_75);     /* 108ac5b0, row 11, event 0x75 */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_sea_of_fire);    /* 108ac1c8, row 12, event 0x7f */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_swamp);          /* 108ac0d4, row 13, event 0x13: speed x0.25 */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_sticky_web);     /* 108ac850, row 14, event 0x5e */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_mat_block);      /* 108acaa4, row 15, events 0x2f and 0xb2 */
PX_BATTLE_FIELDS_1_HANDLER(px_battle_side_crafty_shield);  /* 108ac7a4, row 16, event 0x2e */

#endif
