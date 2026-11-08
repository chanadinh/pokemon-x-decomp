#ifndef PX_BATTLE_ABILITIES_11_H
#define PX_BATTLE_ABILITIES_11_H

#include "../battle_abilities_6/battle_abilities_6.h"

/* DllBattle field (position) handlers: the five handlers the field table
 * (0x108d6810) binds (static analysis addresses; runtime addresses are
 * module-relative). Analyst names. Each handler's value is a battle
 * position, not a battler; its work words hold the effect's state. When the
 * effect ends the handler clears the field entry (1086c218, with the
 * handler's id byte +0x38) and removes itself (1084d1cc, battle_handler).
 * Key 0x02 is the battler concerned (0x1f: none). Fainted (1085f844) and
 * the max-HP share (1084d2f4) come from battle_abilities_1, full HP
 * (1084d318) from battle_abilities_6. */
struct px_battle_abilities_11_api {
    /* Event stack, battler lookup, pokemon_value (10874420; 0xd HP, 0xe max
     * HP), pokemon_status (1086d8ac) and side_of (10873158, the position). */
    const struct px_battle_ability_api *base;
    uint32_t (*battle_decided)(void *user, uint32_t context);                         /* 10865c48 */
    uint32_t (*turn)(void *user, uint32_t context);                                   /* 1084d364; a halfword */
    uint32_t (*battler_at)(void *user, uint32_t context, uint32_t position);          /* 1084d348; 0x1f none */
    uint32_t (*pp_missing)(void *user, uint32_t pokemon, uint32_t slot);              /* 1084d184 */
    void (*field_clear)(void *user, uint32_t context, uint32_t field, uint32_t position); /* 1086c218 */
    void (*remove_handler)(void *user, uint32_t handler);                             /* 1084d1cc (battle_handler) */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*message)(void *user, uint32_t message, uint32_t kind, uint32_t id);        /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t message, uint32_t value);                /* 1084d29c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);            /* 10867b48 */
};

#define PX_BATTLE_ABILITIES_11_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t position, uint32_t work, const struct px_battle_abilities_11_api *api)
PX_BATTLE_ABILITIES_11_HANDLER(px_battle_field_wish);          /* 108a5430, field 0, event 0x7f */
PX_BATTLE_ABILITIES_11_HANDLER(px_battle_field_lunar_dance);   /* 108a5890, field 1, events 0x5e and 0xb0 */
PX_BATTLE_ABILITIES_11_HANDLER(px_battle_field_healing_wish);  /* 108a5714, field 2, events 0x5e and 0xb0 */
PX_BATTLE_ABILITIES_11_HANDLER(px_battle_field_future_sight);  /* 108a55e0, field 3, event 0x7f */
PX_BATTLE_ABILITIES_11_HANDLER(px_battle_field_entry);         /* 108a555c, field 4, event 0x5d */

#endif
