#ifndef PX_BATTLE_ITEMS_3_H
#define PX_BATTLE_ITEMS_3_H

#include "../battle_modifier/battle_modifier.h"

/* DllBattle item handlers, third batch: held items that raise a move's power
 * on event 0x3d (static analysis addresses; runtime addresses are
 * module-relative). Analyst names. Item ids are the original ids.
 *
 * Each requires key 0x03 (the attacker) to be the holder, then multiplies
 * key 0x32 (the power multiplier, 1084ce84) by the item factor 1084cbe4
 * ((parameter 3 + 100)% in 4096 units) when the move fits: key 0x16 (the
 * move type) for the type items, plates and incenses; the holder's species
 * and type for the orbs; the move's category (key 0x12, 108007dc) for Muscle
 * Band and Wise Glasses. Types: 0 Normal, 1 Fighting, 2 Flying, 3 Poison,
 * 4 Ground, 5 Rock, 6 Bug, 7 Ghost, 8 Steel, 9 Fire, 10 Water, 11 Grass,
 * 12 Electric, 13 Psychic, 14 Ice, 15 Dragon, 16 Dark, 17 Fairy. */
struct px_battle_items_3_api {
    /* Event stack, battler lookup and move_category through base; item_param
     * for the item factor. */
    const struct px_battle_modifier_api *modifier;
    /* For Poison Barb's event-0x7c handler. */
    uint32_t (*action_record)(void *user, uint32_t context, uint32_t kind, uint32_t id); /* 10869f7c */
    void (*commit_actions)(void *user, uint32_t context, uint32_t record);             /* 10867b48 */
    uint32_t (*condition)(void *user, uint32_t status);                                /* 10860e00 */
};

#define PX_BATTLE_ITEMS_3_HANDLER(name) \
    void name(uint32_t handler, uint32_t context, uint32_t id, uint32_t work, const struct px_battle_items_3_api *api)
/* Type items: the move's whole type word. */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_silk_scarf);       /* 108a9268, item 251, Normal */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_black_belt);       /* 108a5de0, items 241 and 303 (Fist Plate), Fighting */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_sharp_beak);       /* 108a92b0, items 244 and 306 (Sky Plate), Flying */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_poison_barb);      /* 108a6140, items 245 and 304 (Toxic Plate), Poison */
/* Poison Barb's other entry, event 0x7c (the item's use, as when flung): a
 * kind-0xb poison record for battler key 0x03 when key 0x02 is the holder. */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_poison_barb_use);  /* 108a8ddc, item 245 */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_soft_sand);        /* 108a82b8, items 237 and 305 (Earth Plate), Ground */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_hard_stone);       /* 108a6254, items 238 and 309 (Stone Plate), Rock */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_rock_incense);     /* 108a6e10, item 315, Rock */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_silver_powder);    /* 108a65c8, items 222 and 308 (Insect Plate), Bug */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_spell_tag);        /* 108a78c4, items 247 and 310 (Spooky Plate), Ghost */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_metal_coat);       /* 108a6650, items 233 and 313 (Iron Plate), Steel */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_charcoal);         /* 108a5e28, items 249 and 298 (Flame Plate), Fire */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_mystic_water);     /* 108a8170, items 243 and 299 (Splash Plate), Water */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_sea_incense);      /* 108a6d50, item 254, Water */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_wave_incense);     /* 108a8940, item 317, Water */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_miracle_seed);     /* 108a77e4, items 239 and 301 (Meadow Plate), Grass */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_rose_incense);     /* 108a72bc, item 318, Grass */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_magnet);           /* 108a6080, items 242 and 300 (Zap Plate), Electric */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_twisted_spoon);    /* 108a8058, items 248 and 307 (Mind Plate), Psychic */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_odd_incense);      /* 108a699c, item 314, Psychic */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_never_melt_ice);   /* 108a7b98, items 246 and 302 (Icicle Plate), Ice */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_dragon_fang);      /* 108a6c2c, items 250 and 311 (Draco Plate), Dragon */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_black_glasses);    /* 108a70e8, items 240 and 312 (Dread Plate), Dark */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_pixie_plate);      /* 108a7398, item 644, Fairy */
/* Orbs: the holder's species (battler +0xc) and the low byte of the type. */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_adamant_orb);      /* 108a6b14, item 135: Dialga (483), Dragon or Steel */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_lustrous_orb);     /* 108a63e4, item 136: Palkia (484), Dragon or Water */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_griseous_orb);     /* 108a69e4, item 112: Giratina (487), Dragon or Ghost, x0x1333 */
/* Category items: 1 physical, 2 special. */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_muscle_band);      /* 108aa384, item 266, physical */
PX_BATTLE_ITEMS_3_HANDLER(px_battle_wise_glasses);     /* 108a8820, item 267, special */

#endif
