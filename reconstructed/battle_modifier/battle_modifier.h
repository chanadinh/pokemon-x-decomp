#ifndef PX_BATTLE_MODIFIER_H
#define PX_BATTLE_MODIFIER_H

#include "../battle_ability/battle_ability.h"

/* DllBattle handlers of the final damage event 0x4f (static analysis
 * addresses; runtime addresses are module-relative). Analyst names.
 * Every ability, item and move entry for event 0x4f is covered. They
 * multiply key 0x36 (the damage multiplier). Additional keys: 0x39
 * effectiveness code (7 neutral), 0x46 critical hit, 0x47 berry suppression.
 * Battlers keep a state bit array at +0x169 (4 underwater, 5 underground,
 * 8 minimized). Handler +0x38 holds the item id for item handlers. */
struct px_battle_modifier_api {
    /* Event stack and battler lookup. */
    const struct px_battle_ability_api *base;
    uint32_t (*item_param)(void *user, uint32_t item, uint32_t param);    /* 1084cf74 -> 108003ac */
    uint32_t (*no_consume)(void *user, uint32_t context);                 /* 1084c70c */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record); /* 10867b48 */
    uint32_t (*full_hp)(void *user, uint32_t pokemon);                    /* 1084d318 */
    uint32_t (*is_ally)(void *user, uint32_t id, uint32_t other);         /* 1084c5a8 */
    uint32_t (*move_excluded)(void *user, uint32_t move);                 /* 1084c808 */
    uint32_t (*use_streak)(void *user, uint32_t pokemon);                 /* 108c86c8 */
    uint32_t (*last_move)(void *user, uint32_t pokemon);                  /* 1084c7fc */
};

/* Helpers. */
uint32_t px_battle_effectiveness_class(uint32_t code);  /* 1089e58c: 0 immune, 1 neutral, 2 super, 3 resisted */
uint32_t px_battle_pokemon_state(uint32_t pokemon, uint32_t flag, const struct px_pokemon_api *memory); /* 1084ca4c */
uint32_t px_battle_item_factor(uint32_t handler, const struct px_battle_modifier_api *api);              /* 1084cbe4 */
uint32_t px_battle_resist_berry(uint32_t context, uint32_t id, uint32_t work, uint32_t type,
                                uint32_t any_hit, const struct px_battle_modifier_api *api);           /* 1084c980 */

#define PX_BATTLE_MODIFIER_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_modifier_api *api)
/* Type-resist berries (items 184-200 and 686): halve a super-effective hit
 * of their type (any hit for Chilan Berry). */
PX_BATTLE_MODIFIER_HANDLER(px_battle_occa_berry);    /* 108a638c, item 184, Fire */
PX_BATTLE_MODIFIER_HANDLER(px_battle_passho_berry);  /* 108a6630, item 185, Water */
PX_BATTLE_MODIFIER_HANDLER(px_battle_wacan_berry);   /* 108a6cc8, item 186, Electric */
PX_BATTLE_MODIFIER_HANDLER(px_battle_rindo_berry);   /* 108a6794, item 187, Grass */
PX_BATTLE_MODIFIER_HANDLER(px_battle_yache_berry);   /* 108a697c, item 188, Ice */
PX_BATTLE_MODIFIER_HANDLER(px_battle_chople_berry);  /* 108a6474, item 189, Fighting */
PX_BATTLE_MODIFIER_HANDLER(px_battle_kebia_berry);   /* 108a6120, item 190, Poison */
PX_BATTLE_MODIFIER_HANDLER(px_battle_shuca_berry);   /* 108a68e4, item 191, Ground */
PX_BATTLE_MODIFIER_HANDLER(px_battle_coba_berry);    /* 108a6494, item 192, Flying */
PX_BATTLE_MODIFIER_HANDLER(px_battle_payapa_berry);  /* 108a6454, item 193, Psychic */
PX_BATTLE_MODIFIER_HANDLER(px_battle_tanga_berry);   /* 108a6904, item 194, Bug */
PX_BATTLE_MODIFIER_HANDLER(px_battle_charti_berry);  /* 108a6df0, item 195, Rock */
PX_BATTLE_MODIFIER_HANDLER(px_battle_kasib_berry);   /* 108a6abc, item 196, Ghost */
PX_BATTLE_MODIFIER_HANDLER(px_battle_haban_berry);   /* 108a6610, item 197, Dragon */
PX_BATTLE_MODIFIER_HANDLER(px_battle_colbur_berry);  /* 108a62f0, item 198, Dark */
PX_BATTLE_MODIFIER_HANDLER(px_battle_babiri_berry);  /* 108a6bec, item 199, Steel */
PX_BATTLE_MODIFIER_HANDLER(px_battle_chilan_berry);  /* 108a6188, item 200, Normal */
PX_BATTLE_MODIFIER_HANDLER(px_battle_roseli_berry);  /* 108a6c0c, item 686, Fairy */
/* Other items. */
PX_BATTLE_MODIFIER_HANDLER(px_battle_expert_belt);   /* 108a8250, item 268 */
PX_BATTLE_MODIFIER_HANDLER(px_battle_life_orb);      /* 108aaa10, item 270 */
PX_BATTLE_MODIFIER_HANDLER(px_battle_metronome_item);/* 108a6698, item 277 */
/* Abilities. */
PX_BATTLE_MODIFIER_HANDLER(px_battle_sniper);        /* 108bd260, ability 97 */
PX_BATTLE_MODIFIER_HANDLER(px_battle_solid_rock);    /* 108be2fc, abilities 116 and 111 (Filter) */
PX_BATTLE_MODIFIER_HANDLER(px_battle_tinted_lens);   /* 108bf300, ability 110 */
PX_BATTLE_MODIFIER_HANDLER(px_battle_multiscale);    /* 108c002c, ability 136 */
PX_BATTLE_MODIFIER_HANDLER(px_battle_friend_guard);  /* 108c07e8, ability 132 */
/* Moves: double damage against a battler in a state. */
PX_BATTLE_MODIFIER_HANDLER(px_battle_hits_underwater_surf);      /* 108b0b4c, move 57 (Surf) */
PX_BATTLE_MODIFIER_HANDLER(px_battle_hits_underwater_whirlpool); /* 108b4934, move 250 (Whirlpool) */
PX_BATTLE_MODIFIER_HANDLER(px_battle_hits_underground);          /* 108b5c70, moves 89 and 222 */
PX_BATTLE_MODIFIER_HANDLER(px_battle_hits_minimized);            /* 108ba188, moves 23, 34, 407, 467, 535, 537, 560, 566 */

#endif
