#ifndef PX_BATTLE_ABILITIES_9_H
#define PX_BATTLE_ABILITIES_9_H

#include "../battle_moves_3/battle_moves_3.h"
#include "../battle_items_4/battle_items_4.h"

/* DllBattle hook abilities (static analysis addresses; runtime addresses
 * are module-relative). Analyst names. These handlers install a relocated
 * code address in their handler's word +0xc (the hook the event walk calls
 * for them) when a move starts, and clear it again when it ends, or set a
 * key or work word for the move. Keys: 0x02 the battler concerned, 0x03
 * attacker, 0x04 target, 0x12 move, 0x23 failure reason, 0x36 multiplier,
 * 0x53 flag. The hook word helpers 1084aef8 / 1084aeec come from
 * battle_moves_3 and the owner-byte helper 1084c700 from battle_items_4. */
struct px_battle_abilities_9_api {
    /* Event stack, battler lookup and move_category (108007dc). */
    const struct px_battle_ability_api *base;
    uint32_t (*side_of)(void *user, uint32_t id);                                     /* 1084bfc0 */
    uint32_t (*opposing_side)(void *user, uint32_t side);                             /* 1084af00 */
    void (*ability_popup)(void *user, uint32_t context, uint32_t phase, uint32_t id); /* 1084c948; 1 show, 2 hide */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*message)(void *user, uint32_t message, uint32_t kind, uint32_t id);        /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t message, uint32_t value);                /* 1084d29c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);            /* 10867b48 */
    /* Relocated code addresses (ABS32 literals), taken from the caller. */
    uint32_t infiltrator_hook;   /* literal 108c220c -> static 108c45d0 */
    uint32_t damp_hook;          /* literal 108c3ccc -> static 108c4494 */
    uint32_t mold_breaker_hook;  /* literal 108c3240 -> static 108c4e48 */
    uint32_t unnerve_hook;       /* literals 108c4a84, 108c53c8 -> static 108c4e70 */
    uint32_t klutz_hook;         /* literal 108c4ca8 -> static 108c3750 */
};

#define PX_BATTLE_ABILITIES_9_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_abilities_9_api *api)
/* Damp (6). */
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_damp_hook);            /* 108c3cc4, event 0x03 */
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_damp_unhook);          /* 1084aee8, event 0x04 (falls through into 1084aeec) */
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_damp_unhook_entry);    /* 108c1a3c, event 0x73 */
/* Infiltrator (151). */
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_infiltrator_hook);     /* 108c21dc, event 0x03 */
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_infiltrator_unhook);   /* 108c12a8, event 0x04 */
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_infiltrator_pass);     /* 108c5640, event 0x05 */
/* Mold Breaker (104), Turboblaze (163), Teravolt (164). */
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_mold_breaker_hook);    /* 108c3200, event 0x03 */
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_mold_breaker_unhook);  /* 108c1fbc, event 0x04 */
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_mold_breaker_leave);   /* 108c2aac, event 0x73 */
/* Scrappy (113) and Liquid Ooze (64). */
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_scrappy_self);         /* 108c1ff8, event 0xb7 */
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_liquid_ooze_owner);    /* 108c1ebc, event 0xaf */
/* Klutz (103). */
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_klutz_fling);          /* 108c2f4c, event 0x1f */
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_klutz_fling_message);  /* 108c26a0, event 0x21 */
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_klutz_leave);          /* 108c36c8, event 0x73 */
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_klutz_unhook);         /* 108c3724, event 0x93 */
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_klutz_rehook);         /* 108c4c80, events 0x5f and 0x61 */
/* Unnerve (127). */
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_unnerve_effect);       /* 108c49c8, event 0x5e */
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_unnerve_rehook);       /* 108c5324, events 0x61 and 0x5f */
/* Slow Start (112). */
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_slow_start_restart);   /* 108c503c, event 0x61 */
PX_BATTLE_ABILITIES_9_HANDLER(px_battle_slow_start_attack);    /* 108c44ac, event 0x40 */

#endif
