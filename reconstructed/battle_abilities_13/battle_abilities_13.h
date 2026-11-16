#ifndef PX_BATTLE_ABILITIES_13_H
#define PX_BATTLE_ABILITIES_13_H

#include "../battle_ability/battle_ability.h"

/* The last DllBattle ability handlers: Magician's theft after a move, the
 * work-word reset it ends with, Scrappy's hook removal and the -ate
 * abilities' work reset (static analysis addresses; runtime addresses are
 * module-relative). Analyst names. Keys: 0x03 attacker, 0x05 target count
 * with the targets from key 0x06, 0x4e the move's extra flag. Battlers:
 * held item +0x12. */
struct px_battle_abilities_13_api {
    /* Event stack and battler lookup. */
    const struct px_battle_ability_api *base;
    uint32_t (*speed)(void *user, uint32_t context, uint32_t pokemon, uint32_t flag); /* 1081d2b0 */
    uint32_t (*is_ally)(void *user, uint32_t id, uint32_t other);                     /* 1084c5a8 */
    uint32_t (*steal_blocked)(void *user, uint32_t context, uint32_t thief, uint32_t victim); /* 1084bb5c */
    void (*remove_handler)(void *user, uint32_t handler);                             /* 1084d1cc (battle_handler) */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*message)(void *user, uint32_t message, uint32_t kind, uint32_t id);        /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t message, uint32_t value);                /* 1084d29c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);            /* 10867b48 */
    /* The global halfword speed table (ABS32 literal 108bef08 -> static 10b11bf4), taken from the caller. */
    uint32_t speed_table;
};

/* 1087a19c: the handler's seven work words (+0x1c ... +0x34) are cleared. */
void px_battle_clear_work(uint32_t handler, const struct px_battle_abilities_13_api *api);

#define PX_BATTLE_ABILITIES_13_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_abilities_13_api *api)
PX_BATTLE_ABILITIES_13_HANDLER(px_battle_magician);        /* 108beca4, ability 170, event 0x8c */
PX_BATTLE_ABILITIES_13_HANDLER(px_battle_scrappy_unhook);  /* 108c2ae8, ability 113, event 0x04 */
PX_BATTLE_ABILITIES_13_HANDLER(px_battle_ate_reset);       /* 108c1a68, abilities 174, 182 and 184, event 0x91 */

#endif
