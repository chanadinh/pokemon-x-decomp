#ifndef PX_BATTLE_SWITCHIN_H
#define PX_BATTLE_SWITCHIN_H

#include "../battle_ability/battle_ability.h"

/* DllBattle ability handlers of the switch-in event 0x94 (static analysis
 * addresses; runtime addresses are module-relative). Analyst names. This
 * milestone covers 23 of its 28 handlers (Trace, Download, Forewarn,
 * Anticipation and Frisk remain). Key 0x02 is the battler entering.
 * Battlers: species at +0xc, held item at +0x12, form byte at +0x153.
 * Weather: 1 sun, 2 rain, 3 hail, 4 sandstorm. */
struct px_battle_switchin_api {
    /* Event stack and battler lookup. */
    const struct px_battle_ability_api *base;
    void (*cure_status)(void *user, uint32_t context, uint32_t id, uint32_t status);  /* 1084aba4 */
    uint32_t (*item_usable)(void *user, uint32_t context, uint32_t id);              /* 1084b5b8 */
    uint32_t (*weather)(void *user, uint32_t context);                                /* 1084be58 */
    uint32_t (*opposing_selector)(void *user, uint32_t context, uint32_t id);         /* 1084cb78 */
    uint32_t (*temp_buffer)(void *user, uint32_t context, uint32_t size);            /* 1084c7d0 */
    uint32_t (*side_battlers)(void *user, uint32_t context, uint32_t selector, uint32_t out); /* 1084bc20 */
    uint32_t (*side_of)(void *user, uint32_t id);                                     /* 1084bfc0 */
    uint32_t (*opposing_side)(void *user, uint32_t side);                             /* 1084af00 */
    uint32_t (*truant_check)(void *user, uint32_t pokemon, uint32_t flag);            /* 1084c818 */
    void (*flower_gift)(void *user, uint32_t handler, uint32_t context, uint32_t id,
                        uint32_t sunny, uint32_t flag);                               /* 1084a944 */
    void (*ability_popup)(void *user, uint32_t context, uint32_t phase, uint32_t id); /* 1084c948 */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*message)(void *user, uint32_t message, uint32_t kind, uint32_t id);        /* 1084d2d4 */
    void (*message_arg)(void *user, uint32_t message, uint32_t value);                /* 1084d29c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);            /* 10867b48 */
    /* Relocated code addresses stored in handler word +0xc (literals 108c20d0, 108c4024). */
    uint32_t unnerve_hook;   /* static 108c4e70 */
    uint32_t klutz_hook;     /* static 108c3750 */
};

/* 1084acbc: an entry weather for 5 turns, 8 with the matching rock held. */
void px_battle_set_entry_weather(uint32_t context, uint32_t id, uint32_t weather, uint32_t rock,
                                 const struct px_battle_switchin_api *api);

#define PX_BATTLE_SWITCHIN_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_switchin_api *api)
PX_BATTLE_SWITCHIN_HANDLER(px_battle_drought);       /* 1084aca8, ability 70 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_drizzle);       /* 108bef5c, ability 2 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_sand_stream);   /* 108bf8f8, ability 45 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_snow_warning);  /* 108c02dc, ability 117 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_limber_entry);  /* 108c06c8, ability 7 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_insomnia_entry);/* 108bfd2c, abilities 15 and 72 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_immunity_entry);/* 108c072c, ability 17 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_own_tempo_entry);/* 108c07b0, ability 20 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_water_veil_entry);/* 108c2b50, ability 41 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_magma_armor_entry);/* 108c4368, ability 40 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_oblivious_entry);/* 108c0518, ability 12 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_intimidate);    /* 108c1ee8, ability 22 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_unnerve);       /* 108c202c, ability 127 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_forecast);      /* 108c2210, ability 59 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_flower_gift);   /* 108c393c, ability 122 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_slow_start);    /* 108c4500, ability 112 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_truant);        /* 108c0074, ability 54 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_mold_breaker);  /* 108c4918, abilities 104, 163 and 164 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_pressure);      /* 108c3c3c, ability 46 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_dark_aura_entry);  /* 108c3800, ability 186 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_fairy_aura_entry); /* 108c4028, ability 187 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_aura_break);    /* 108c3f58, ability 188 */
PX_BATTLE_SWITCHIN_HANDLER(px_battle_klutz);         /* 108c3fe0, ability 103 */

#endif
