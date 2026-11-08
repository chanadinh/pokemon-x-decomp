#ifndef PX_BATTLE_MOVES_4_H
#define PX_BATTLE_MOVES_4_H

#include "../battle_ability/battle_ability.h"
#include "../battle_handler/battle_handler.h"

/* DllBattle move handlers for accuracy, semi-invulnerable targets, added
 * stat effects and end-of-turn handler removal (static analysis addresses;
 * runtime addresses are module-relative). Analyst names. Events:
 * 0xa5 hit check (key 0x22 the defender's semi-invulnerable state: 3 in the
 * air, 4 underwater, 5 underground; key 0x43 nonzero misses, key 0x50),
 * 0x37 accuracy (key 0x53 nonzero never misses), 0x62 added stat effect
 * (key 0x20 chance in percent, key 0x21 stages), 0x7f handler removal (key
 * 0x02 the battler; 0x1f for all). Key 0x03 is the attacker, 0x04 the
 * defender; handler +0x38 holds the move id. Battler state bit 8 (+0x169):
 * minimized. Weather: 1 sun, 2 rain, 3 hail. Abilities: 57 Plus, 58 Minus. */
struct px_battle_moves_4_api {
    /* Event stack, battler lookup and pokemon_value (10874420; 0x11 ability). */
    const struct px_battle_ability_api *base;
    /* Registered handlers (removed on event 0x7f). */
    const struct px_battle_handler_api *handlers;
    uint32_t (*weather)(void *user, uint32_t context);                       /* 1084be58 */
    uint32_t (*target_switching)(void *user, uint32_t context);              /* 1084c020 */
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status);   /* 10856a88; 5 poison */
    uint32_t (*has_type)(void *user, uint32_t pokemon, uint32_t type);       /* 1086f960; 11 Grass */
};

#define PX_BATTLE_MOVES_4_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_4_api *api)
/* Event 0xa5: hits a semi-invulnerable defender (key 0x43 = 0). */
PX_BATTLE_MOVES_4_HANDLER(px_battle_sky_uppercut_hit);  /* 108b0e58, move 327, in the air */
PX_BATTLE_MOVES_4_HANDLER(px_battle_thunder_hit);       /* 108b97c0, moves 87, 479, 542, 614 (Thunder, Smack Down, Hurricane, Thousand Arrows), in the air */
PX_BATTLE_MOVES_4_HANDLER(px_battle_gust_hit);          /* 108b9d28, moves 16 and 239 (Twister), in the air */
PX_BATTLE_MOVES_4_HANDLER(px_battle_earthquake_hit);    /* 108b7b14, moves 89 and 222 (Magnitude), underground */
PX_BATTLE_MOVES_4_HANDLER(px_battle_whirlpool_hit);     /* 108b8714, move 250, underwater */
PX_BATTLE_MOVES_4_HANDLER(px_battle_surf_hit);          /* 108b9b08, move 57, underwater */
PX_BATTLE_MOVES_4_HANDLER(px_battle_heal_bell_hit);     /* 108b9d64, moves 215, 270, 312 (Heal Bell, Helping Hand, Aromatherapy) */
PX_BATTLE_MOVES_4_HANDLER(px_battle_rototiller_hit);    /* 108b9cb0, moves 563 and 579 (Rototiller, Flower Shield): key 0x50 = 0 */
/* Event 0x37: never misses (key 0x53 = 1). */
PX_BATTLE_MOVES_4_HANDLER(px_battle_blizzard_accuracy); /* 108ae714, move 59, in hail */
PX_BATTLE_MOVES_4_HANDLER(px_battle_thunder_accuracy);  /* 108bb520, moves 87 and 542 (Hurricane), in rain */
PX_BATTLE_MOVES_4_HANDLER(px_battle_pursuit_accuracy);  /* 108b76c8, move 228, against a switching target */
PX_BATTLE_MOVES_4_HANDLER(px_battle_stomp_accuracy);    /* 108bb1f0, moves 23, 34, 407, 467, 535, 537, 560, 566, against a minimized defender */
/* Event 0x62. */
PX_BATTLE_MOVES_4_HANDLER(px_battle_growth_stages);     /* 108afc4c, move 74: in sun, 1 stage becomes 2 */
PX_BATTLE_MOVES_4_HANDLER(px_battle_rototiller_target); /* 108b0e94, move 563: no effect on a non-Grass defender */
PX_BATTLE_MOVES_4_HANDLER(px_battle_venom_drench_target);/* 108b1314, move 599: no effect on an unpoisoned defender */
PX_BATTLE_MOVES_4_HANDLER(px_battle_magnetic_flux_target);/* 108b3324, move 602: no effect without Plus or Minus */
PX_BATTLE_MOVES_4_HANDLER(px_battle_ancient_power_chance);/* 108b69b8, moves 246, 318, 466: 10% on the user */
/* Event 0x7f: the handler is removed for its battler. */
PX_BATTLE_MOVES_4_HANDLER(px_battle_endure_end);        /* 108b8dcc, move 203 (Endure) */
PX_BATTLE_MOVES_4_HANDLER(px_battle_helping_hand_end);  /* 108b9dbc, move 270 (Helping Hand) */
PX_BATTLE_MOVES_4_HANDLER(px_battle_snatch_end);        /* 108b9ee4, move 289 (Snatch) */
PX_BATTLE_MOVES_4_HANDLER(px_battle_magic_coat_end);    /* 108ba38c, move 277 (Magic Coat) */
PX_BATTLE_MOVES_4_HANDLER(px_battle_kings_shield_end);  /* 108b8318, moves 588 and 596 (King's Shield, Spiky Shield): key 0x02 0x1f */
PX_BATTLE_MOVES_4_HANDLER(px_battle_follow_me_end);     /* 108bbc64, moves 266 and 476 (Follow Me, Rage Powder): every handler of the move */

#endif
