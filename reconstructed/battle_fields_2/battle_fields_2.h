#ifndef PX_BATTLE_FIELDS_2_H
#define PX_BATTLE_FIELDS_2_H

#include "../battle_items_1/battle_items_1.h"
#include "../battle_abilities_1/battle_abilities_1.h"

/* DllBattle position-effect handlers: the 11 rows of the position-effect
 * table (108d6768, 8 bytes each: an id and a table function; the ids run 0-8
 * and 10, with 9 and 8 in the ninth and tenth rows) with 17 handlers (static
 * analysis addresses; runtime addresses are module-relative). The names are
 * analyst interpretations of what each row does; the ids are the table's.
 *
 * Row 10 (the terrains) has a table function that takes a parameter: 1 gives
 * the Grassy Terrain handlers (events 0x3d and 0x7f), 2 the Misty Terrain
 * handlers (0x3d, 0x6e, 0x70), 3 the Electric Terrain handlers (0x3d, 0x6e,
 * 0x70) and anything else a single no-op on event 0. Keys: 0x03 the attacker,
 * 0x04 the battler hit, 0x02 the battler, 0x12 the move, 0x16 the move type,
 * 0x1e the status being given, 0x32 the power multiplier, 0x36 another
 * multiplier, 0x42 a flag set to 1. A terrain handler's battler is
 * unaffected when it is airborne (1084bf24) or exempt (1084af30). */
struct px_battle_fields_2_api {
    /* Event stack, battler lookup, records, messages and commit. */
    const struct px_battle_items_api *items;
    /* For 1084d2f4 (max HP / divisor, battle_abilities_1). */
    const struct px_battle_abilities_1_api *abilities;
    uint32_t (*airborne)(void *user, uint32_t context, uint32_t id);   /* 1084bf24 */
    uint32_t (*exempt)(void *user, uint32_t context, uint32_t id);     /* 1084af30 */
};

#define PX_BATTLE_FIELDS_2_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_fields_2_api *api)
/* The regular rows. */
PX_BATTLE_FIELDS_2_HANDLER(px_battle_position_noop_4f);       /* 108bc154, row 0, event 0x4f: returns at once */
PX_BATTLE_FIELDS_2_HANDLER(px_battle_position_flag_4b);       /* 108bc28c, row 1, event 0x13: key 0x4b = 1 */
PX_BATTLE_FIELDS_2_HANDLER(px_battle_position_accuracy);      /* 108bc680, row 2, event 0x39: key 0x36 x 0x1ab8 */
PX_BATTLE_FIELDS_2_HANDLER(px_battle_position_ground);        /* 108bc658, row 2, event 0x44: Ground type, key 0x4c = 1 */
PX_BATTLE_FIELDS_2_HANDLER(px_battle_position_noop);          /* 108bc07c, rows 3, 7, 9 and the terrain default, event 0 */
PX_BATTLE_FIELDS_2_HANDLER(px_battle_position_water_sport);   /* 108bc260, row 4, event 0x3d: Fire x 0x548 */
PX_BATTLE_FIELDS_2_HANDLER(px_battle_position_mud_sport);     /* 108bc234, row 5, event 0x3d: Electric x 0x548 */
PX_BATTLE_FIELDS_2_HANDLER(px_battle_position_count_3e);      /* 108bc3a4, row 6, event 0x3f: key 0x3e + 1 */
PX_BATTLE_FIELDS_2_HANDLER(px_battle_position_ion_deluge);    /* 108bc298, row 8, event 0x29: Normal becomes Electric */
/* Row 10: the terrains. */
PX_BATTLE_FIELDS_2_HANDLER(px_battle_grassy_power);           /* 108bc158, parameter 1, event 0x3d */
PX_BATTLE_FIELDS_2_HANDLER(px_battle_grassy_heal);            /* 108bc500, parameter 1, event 0x7f */
PX_BATTLE_FIELDS_2_HANDLER(px_battle_misty_power);            /* 108bc0f4, parameter 2, event 0x3d */
PX_BATTLE_FIELDS_2_HANDLER(px_battle_misty_block);            /* 108bc3c0, parameter 2, event 0x6e */
PX_BATTLE_FIELDS_2_HANDLER(px_battle_misty_show);             /* 108bc5b0, parameter 2, event 0x70 */
PX_BATTLE_FIELDS_2_HANDLER(px_battle_electric_power);         /* 108bc094, parameter 3, event 0x3d */
PX_BATTLE_FIELDS_2_HANDLER(px_battle_electric_block);         /* 108bc2c0, parameter 3, event 0x6e */
PX_BATTLE_FIELDS_2_HANDLER(px_battle_electric_show);          /* 108bc458, parameter 3, event 0x70 */

#endif
