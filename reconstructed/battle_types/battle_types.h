#ifndef PX_BATTLE_TYPES_H
#define PX_BATTLE_TYPES_H

#include "../pokemon_constructor/pokemon_constructor.h"

/* DllBattle type effectiveness (static analysis addresses; runtime addresses
 * are module-relative). Analyst names. Logical ARM32 addresses.
 * Effectiveness codes 0..13: 0 is immune, 7 is neutral, each step doubles or
 * halves (6 = x1/2, 8 = x2). Type 0x12 is typeless. */
#define PX_BATTLE_TYPELESS UINT32_C(0x12)
#define PX_BATTLE_NEUTRAL UINT32_C(7)
/* Static addresses of the relocated data the routines reach through literals. */
#define PX_BATTLE_TYPE_CHART_STATIC UINT32_C(0x108d12a8)       /* literal 10860d1c; 18 x 18 bytes */
#define PX_BATTLE_EFFECTIVENESS_TABLE_STATIC UINT32_C(0x108d2248) /* literal 10860c7c; 14 words */
#define PX_BATTLE_RULES_OBJECT_STATIC UINT32_C(0x108fca00)     /* literal 10860d18; byte +1 */

struct px_battle_types_api {
    const struct px_pokemon_api *memory;
    void *user;
    void (*abort)(void *user);   /* 10800394(0, 0, 0); returns */
    uint32_t chart;              /* bytes 0, 2, 4, 8: immune, half, neutral, double */
    uint32_t table;              /* code -> power of two (0, 1, 2, ... 4096) */
    uint32_t rules;              /* signed byte +1 nonzero: Inverse Battle */
};

/* 10860c80: one attacking type against one defending type. */
uint32_t px_battle_type_effectiveness(uint32_t attack_type, uint32_t defend_type,
                                      const struct px_battle_types_api *api);
/* 10860b80: product of two codes, clamped to 0x1000 and to 1 unless either
 * code is immune, returned as a code. */
uint32_t px_battle_combine_effectiveness(uint32_t first, uint32_t second,
                                         const struct px_battle_types_api *api);
/* 1089e7f0: damage scaled by a code with unsigned shifts. */
uint32_t px_battle_apply_effectiveness(uint32_t damage, uint32_t code,
                                       const struct px_battle_types_api *api);

#endif
