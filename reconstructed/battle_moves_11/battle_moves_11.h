#ifndef PX_BATTLE_MOVES_11_H
#define PX_BATTLE_MOVES_11_H

#include "../battle_ability/battle_ability.h"

/* DllBattle handlers of moves that call other moves (static analysis
 * addresses; runtime addresses are module-relative). Analyst names.
 * Event 0x18 chooses the called move: key 0x02 the battler, 0x0e the
 * original target (0xa none), and the handler writes key 0x12 (the move),
 * key 0x0d (its target) or key 0x42 = 1 (failure). Event 0x19 announces it:
 * key 0x40 the message buffer, key 0x53 = 1 once announced. Battlers: move
 * count +0x152, id +0x19. Statuses (10856a88): 2 sleep. */
struct px_battle_moves_11_api {
    /* Event stack, battler lookup and side_battlers (1084bc20). */
    const struct px_battle_ability_api *base;
    uint32_t (*has_status)(void *user, uint32_t pokemon, uint32_t status);               /* 10856a88 */
    uint32_t (*move_at)(void *user, uint32_t pokemon, uint32_t slot);                    /* 10855c68 */
    uint32_t (*move_flag)(void *user, uint32_t move, uint32_t flag);                     /* 1080049c; 1 charge, 6 mirrorable */
    uint32_t (*random)(void *user, uint32_t range);                                      /* 1084c7bc */
    uint32_t (*choose_target)(void *user, uint32_t context, uint32_t id, uint32_t move); /* 10871bac */
    uint32_t (*sleep_talk_banned)(void *user, uint32_t move);                           /* 108a46d4 */
    uint32_t (*assist_banned)(void *user, uint32_t move);                               /* 108a47b4 */
    uint32_t (*copycat_banned)(void *user, uint32_t move);                              /* 108a4724 */
    uint32_t (*me_first_banned)(void *user, uint32_t move);                             /* 108a4824 */
    uint32_t (*metronome_pool)(void *user, uint32_t out);                               /* 108a48a0 */
    uint32_t (*metronome_pick)(void *user, uint32_t pool, uint32_t count);              /* 1089e724 */
    uint32_t (*party)(void *user, uint32_t context, uint32_t id);                       /* 1084b950 */
    uint32_t (*party_member)(void *user, uint32_t party, uint32_t index);               /* 1085daf8 */
    uint32_t (*temp_buffer)(void *user, uint32_t context, uint32_t size);               /* 1084c7d0 */
    uint32_t (*fainted)(void *user, uint32_t pokemon);                                  /* 1085f844 */
    uint32_t (*can_battle)(void *user, uint32_t pokemon);                               /* 108551d4 */
    uint32_t (*terrain)(void *user, uint32_t context);                                  /* 1084b8d4 */
    uint32_t (*environment)(void *user, uint32_t context);                              /* 1084b81c */
    uint32_t (*last_move_used)(void *user, uint32_t context);                           /* 1081cf08 */
    uint32_t (*last_move)(void *user, uint32_t pokemon);                                /* 1084c7fc */
    uint32_t (*opposing_selector)(void *user, uint32_t context, uint32_t id);           /* 1084cb78 */
    uint32_t (*battle_format)(void *user, uint32_t context);                            /* 1084bf14 */
    uint32_t (*target_selector)(void *user, uint32_t format, uint32_t selector, uint32_t zero); /* 1086de1c */
    uint32_t (*turn_flag)(void *user, uint32_t pokemon, uint32_t flag);                 /* 1084c818; 3 moved */
    uint32_t (*chosen_action)(void *user, uint32_t context, uint32_t id, uint32_t out); /* 1084b784 */
    uint32_t (*slot_present)(void *user, uint32_t context, uint32_t id);                /* 1084bfa0 */
    uint32_t (*charging)(void *user, uint32_t context, uint32_t id);                    /* 1084c030 */
    uint32_t (*action_move)(void *user, uint32_t action);                               /* 1084b648 */
    uint32_t (*move_damages)(void *user, uint32_t move);                                /* 10800434 */
    void (*message)(void *user, uint32_t at, uint32_t kind, uint32_t message);          /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t at, uint32_t value);                       /* 1084d29c */
    /* Logical words standing in for stack slots: Metronome's pool count and Me First's action. */
    uint32_t pool_scratch, action_scratch;
};

#define PX_BATTLE_MOVES_11_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_moves_11_api *api)
/* Event 0x18. */
PX_BATTLE_MOVES_11_HANDLER(px_battle_sleep_talk_call);   /* 108aea60, move 214 */
PX_BATTLE_MOVES_11_HANDLER(px_battle_assist_call);       /* 108b0c58, move 274 */
PX_BATTLE_MOVES_11_HANDLER(px_battle_metronome_call);    /* 108b4aec, move 118 */
PX_BATTLE_MOVES_11_HANDLER(px_battle_nature_power_call); /* 108b711c, move 267 */
PX_BATTLE_MOVES_11_HANDLER(px_battle_copycat_call);      /* 108b99e8, move 383 */
PX_BATTLE_MOVES_11_HANDLER(px_battle_me_first_call);     /* 108ba62c, move 382 */
PX_BATTLE_MOVES_11_HANDLER(px_battle_mirror_move_call);  /* 108bad6c, move 119 */
/* Event 0x19. */
PX_BATTLE_MOVES_11_HANDLER(px_battle_bide_announce);        /* 108b691c, move 117 */
PX_BATTLE_MOVES_11_HANDLER(px_battle_metronome_announce);   /* 108b79c0, move 118 */
PX_BATTLE_MOVES_11_HANDLER(px_battle_nature_power_announce);/* 108b8fd4, move 267 */

#endif
