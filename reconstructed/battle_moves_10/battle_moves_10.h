#ifndef PX_BATTLE_MOVES_10_H
#define PX_BATTLE_MOVES_10_H

#include "../battle_ability/battle_ability.h"

/* DllBattle move handlers of event 0x1f, the check before a move is executed
 * (static analysis addresses; runtime addresses are module-relative).
 * Analyst names. Key 0x02 is the battler about to move, 0x12 its move, and
 * key 0x23 the failure code: 0 none, 0x1b the move fails, 0xd/0xe/0xf Rest's
 * own failures, 0x19 bounced (Magic Coat). Work words: +0, +4, +8. Battlers:
 * move count +0x152, held item +0x12, species +0xc, id +0x19, state bits
 * +0x169 (bit 0 first turn). Counters (1084bb2c): 0 stockpile, 3 protect
 * streak, 4 Sky Drop partner + 1. Statuses (10856a88): 2 sleep, 8 and 0x16
 * trapped, 0xf heal block, 0x15 and 0x1f grounded. */
struct px_battle_moves_10_api {
    /* Event stack, battler lookup, pokemon_value and move_category (108007dc). */
    const struct px_battle_ability_api *base;
    uint32_t (*damage_record)(void *user, uint32_t pokemon, uint32_t kind, uint32_t index, uint32_t out); /* 1084bac4 */
    uint32_t (*is_ally)(void *user, uint32_t id, uint32_t other);                        /* 1084c5a8 */
    uint32_t (*move_at)(void *user, uint32_t pokemon, uint32_t slot);                    /* 10855c68 */
    uint32_t (*move_used)(void *user, uint32_t pokemon, uint32_t slot);                  /* 108c8714 */
    uint32_t (*counter)(void *user, uint32_t pokemon, uint32_t index);                   /* 1084bb2c */
    uint32_t (*random)(void *user, uint32_t range);                                      /* 1084c7bc */
    uint32_t (*moves_last)(void *user, uint32_t context, uint32_t id);                   /* 1084b8e0 */
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status);               /* 10856a88 */
    uint32_t (*full_hp)(void *user, uint32_t pokemon);                                   /* 1084d318 */
    uint32_t (*escape_forbidden)(void *user, uint32_t context);                          /* 108544ec */
    uint32_t (*battle_format)(void *user, uint32_t context);                             /* 1084bf14 */
    uint32_t (*escape_allowed)(void *user, uint32_t context, uint32_t id);               /* 1081d770 */
    uint32_t (*field_active)(void *user, uint32_t context, uint32_t condition);          /* 1084c374; 9 Fairy Lock */
    uint32_t (*slot_present)(void *user, uint32_t context, uint32_t id);                 /* 1084bfa0 */
    uint32_t (*typed)(void *user, uint32_t context, uint32_t id);                        /* 1084af30 */
    uint32_t (*move_target)(void *user, uint32_t move);                                  /* 108007d4 */
    uint32_t (*move_flag)(void *user, uint32_t move, uint32_t flag);                     /* 1080049c; 4 reflectable */
    void (*bounce)(void *user, uint32_t context, uint32_t id, uint32_t other);           /* 1084b3f8 */
    uint32_t (*item_param)(void *user, uint32_t item, uint32_t param);                   /* 1084cf74 */
    uint32_t (*item_usable)(void *user, uint32_t context, uint32_t id);                  /* 1084b5b8 */
    uint32_t (*item_locked)(void *user, uint32_t context, uint32_t species, uint32_t item); /* 1085441c */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);               /* 10867b48 */
    /* Logical 8 bytes standing in for the Counter routine's stack record. */
    uint32_t record_scratch;
};

/* 1084b2d8: fails the move unless a damage record of the battler from a
 * non-ally (of category `kind` when nonzero) exists; `work` is unused. */
void px_battle_counter_check(uint32_t context, uint32_t id, uint32_t work, uint32_t kind,
                             const struct px_battle_moves_10_api *api);

#define PX_BATTLE_MOVES_10_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_10_api *api)
PX_BATTLE_MOVES_10_HANDLER(px_battle_counter_ready);      /* 1084b2c4, move 68 (physical) */
PX_BATTLE_MOVES_10_HANDLER(px_battle_mirror_coat_ready);  /* 108b9af4, move 243 (special) */
PX_BATTLE_MOVES_10_HANDLER(px_battle_metal_burst_ready);  /* 108ba3b8, move 368 (any) */
PX_BATTLE_MOVES_10_HANDLER(px_battle_last_resort_ready);  /* 108b10e8, move 387 */
PX_BATTLE_MOVES_10_HANDLER(px_battle_bide_ready);         /* 108b73c8, move 117 */
PX_BATTLE_MOVES_10_HANDLER(px_battle_protect_ready);      /* 108b7d30, moves 182, 197, 203, 588, 596 */
PX_BATTLE_MOVES_10_HANDLER(px_battle_rest_ready);         /* 108b7f2c, move 156 */
PX_BATTLE_MOVES_10_HANDLER(px_battle_spit_up_ready);      /* 108b8c38, moves 255 and 256 (Swallow) */
PX_BATTLE_MOVES_10_HANDLER(px_battle_snore_ready);        /* 108b8cc4, moves 173 and 214 (Sleep Talk) */
PX_BATTLE_MOVES_10_HANDLER(px_battle_teleport_ready);     /* 108b9080, move 100 */
PX_BATTLE_MOVES_10_HANDLER(px_battle_sky_drop_ready);     /* 108b94ec, move 507 */
PX_BATTLE_MOVES_10_HANDLER(px_battle_magic_coat_ready);   /* 108b98e8, move 277 */
PX_BATTLE_MOVES_10_HANDLER(px_battle_stockpile_ready);    /* 108b9cdc, move 254 */
PX_BATTLE_MOVES_10_HANDLER(px_battle_guard_ready);        /* 108b9e78, moves 469, 501, 578 (Wide, Quick Guard, Crafty Shield) */
PX_BATTLE_MOVES_10_HANDLER(px_battle_fling_ready);        /* 108ba4a0, move 374 */
PX_BATTLE_MOVES_10_HANDLER(px_battle_magnet_rise_ready);  /* 108ba850, move 393 */
PX_BATTLE_MOVES_10_HANDLER(px_battle_mat_block_ready);    /* 108bb3cc, move 561 */
PX_BATTLE_MOVES_10_HANDLER(px_battle_natural_gift_ready); /* 108bb76c, move 363 */
PX_BATTLE_MOVES_10_HANDLER(px_battle_follow_me_ready);    /* 108bbb4c, moves 266 and 476 (Rage Powder) */
PX_BATTLE_MOVES_10_HANDLER(px_battle_fake_out_ready);     /* 108b3c6c, move 252 */

#endif
