#ifndef PX_BATTLE_INSIGHT_H
#define PX_BATTLE_INSIGHT_H

#include "../battle_ability/battle_ability.h"

/* The last five DllBattle switch-in handlers (event 0x94; static analysis
 * addresses, runtime addresses module-relative). Analyst names. Each looks
 * at the opposing battlers. Battlers: id byte +0x19, held item +0x12, move
 * count +0x152. Key 0x02 is the battler entering. */
struct px_battle_insight_api {
    /* Event stack, battler lookup and pokemon_value (10874420). */
    const struct px_battle_ability_api *base;
    uint32_t (*opponents)(void *user, uint32_t context, uint32_t id, uint32_t out);   /* 1084ac1c; ids as bytes */
    uint32_t (*opposing_selector)(void *user, uint32_t context, uint32_t id);         /* 1084cb78 */
    uint32_t (*side_battlers)(void *user, uint32_t context, uint32_t selector, uint32_t out); /* 1084bc20 */
    uint32_t (*temp_buffer)(void *user, uint32_t context, uint32_t size);            /* 1084c7d0 */
    uint32_t (*random)(void *user, uint32_t range);                                   /* 1084c7bc */
    uint32_t (*untraceable)(void *user, uint32_t ability);                            /* 108a4854 */
    uint32_t (*is_ally)(void *user, uint32_t id, uint32_t other);                     /* 1084c5a8 */
    uint32_t (*position)(void *user, uint32_t context, uint32_t id);                  /* 10873158; 0xa none */
    uint32_t (*battle_mode)(void *user, uint32_t context);                            /* 1084bf14 */
    uint32_t (*position_in)(void *user, uint32_t selector, uint32_t position);        /* 108673f8 */
    uint32_t (*move_at)(void *user, uint32_t pokemon, uint32_t slot);                 /* 10855c68 */
    uint32_t (*move_category)(void *user, uint32_t move);                             /* 108007dc; 0 status */
    uint32_t (*move_power)(void *user, uint32_t move);                                /* 10801634 */
    uint32_t (*move_kind)(void *user, uint32_t move);                                 /* 108007d4; 9 one-hit KO */
    uint32_t (*damaging_move)(void *user, uint32_t move);                             /* 10800434 */
    uint32_t (*move_type)(void *user, uint32_t move);                                 /* 1080043c */
    uint32_t (*hidden_power_type)(void *user, uint32_t data);                         /* 10801afc */
    uint32_t (*packed_types)(void *user, uint32_t pokemon);                           /* 10870994 (battle_matchup) */
    uint32_t (*effectiveness)(void *user, uint32_t type, uint32_t packed);            /* 10870900; above 7 super effective */
    void (*ability_popup)(void *user, uint32_t context, uint32_t phase, uint32_t id); /* 1084c948 */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*message)(void *user, uint32_t message, uint32_t kind, uint32_t id);        /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t message, uint32_t value);                /* 1084d29c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);            /* 10867b48 */
    /* Logical stack areas standing in for the original frames (8 and 8 bytes). */
    uint32_t list_scratch, candidate_scratch;
};

#define PX_BATTLE_INSIGHT_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_insight_api *api)
PX_BATTLE_INSIGHT_HANDLER(px_battle_trace);         /* 108bcb0c, ability 36 */
PX_BATTLE_INSIGHT_HANDLER(px_battle_download);      /* 108be1dc, ability 88 */
PX_BATTLE_INSIGHT_HANDLER(px_battle_forewarn);      /* 108bd460, ability 108 */
PX_BATTLE_INSIGHT_HANDLER(px_battle_anticipation);  /* 108bf390, ability 107 */
PX_BATTLE_INSIGHT_HANDLER(px_battle_frisk);         /* 108be6bc, ability 119 */

/* Forewarn's power rule for one move. */
uint32_t px_battle_forewarn_power(uint32_t move, const struct px_battle_insight_api *api);

#endif
