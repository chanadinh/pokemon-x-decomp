#ifndef PX_BATTLE_TYPECHANGE_H
#define PX_BATTLE_TYPECHANGE_H

#include "../battle_ability/battle_ability.h"

/* DllBattle handlers of the move-type event 0x28 (static analysis addresses;
 * runtime addresses are module-relative). Analyst names. Every ability and
 * move entry for event 0x28 is covered. Keys: 0x02 the move's user, 0x16
 * move type, 0x1c Curse's form, 0x4c Struggle's flag. Battlers keep their
 * held item at +0x12. Types: 0 Normal, 2 Flying, 7 Ghost, 14 Ice, 17 Fairy. */
struct px_battle_typechange_api {
    /* Event stack and battler lookup. */
    const struct px_battle_ability_api *base;
    uint32_t (*weather)(void *user, uint32_t context);                    /* 1084be58; 1 sun, 2 rain, 3 hail, 4 sand */
    void (*type_changed)(void *user, uint32_t context, uint32_t code);    /* 1084bef8 */
    uint32_t (*item_usable)(void *user, uint32_t context, uint32_t id);   /* 1084b5b8 */
    uint32_t (*hidden_power_type)(void *user, uint32_t data);             /* 10801afc */
    uint32_t (*item_param)(void *user, uint32_t item, uint32_t param);    /* 1084cf74 */
    uint32_t (*has_type)(void *user, uint32_t pokemon, uint32_t type);    /* 1086f960 */
};

/* 1089e61c: Curse's form, 0 for a Ghost-type user and 7 otherwise. */
uint32_t px_battle_curse_form(uint32_t pokemon, const struct px_battle_typechange_api *api);
/* The type Judgment takes from a held plate (0 Normal without one). */
uint32_t px_battle_plate_type(uint32_t item);

#define PX_BATTLE_TYPECHANGE_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_typechange_api *api)
PX_BATTLE_TYPECHANGE_HANDLER(px_battle_normalize);     /* 108c0114, ability 96 */
PX_BATTLE_TYPECHANGE_HANDLER(px_battle_aerilate_type); /* 108bde48, ability 184 */
PX_BATTLE_TYPECHANGE_HANDLER(px_battle_pixilate_type); /* 108bf0b0, ability 182 */
PX_BATTLE_TYPECHANGE_HANDLER(px_battle_refrigerate_type); /* 108bf0fc, ability 174 */
PX_BATTLE_TYPECHANGE_HANDLER(px_battle_weather_ball); /* 108b8750, move 311 */
PX_BATTLE_TYPECHANGE_HANDLER(px_battle_curse);        /* 108b8008, move 174 */
PX_BATTLE_TYPECHANGE_HANDLER(px_battle_struggle);     /* 108ba824, move 165 */
PX_BATTLE_TYPECHANGE_HANDLER(px_battle_hidden_power); /* 108b9a54, move 237 */
PX_BATTLE_TYPECHANGE_HANDLER(px_battle_judgment);     /* 108b7708, move 449 */
PX_BATTLE_TYPECHANGE_HANDLER(px_battle_natural_gift); /* 108b9c28, move 363 */
PX_BATTLE_TYPECHANGE_HANDLER(px_battle_techno_blast); /* 108b6398, move 546 */

#endif
