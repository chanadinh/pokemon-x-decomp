#ifndef PX_BATTLE_TABLES_1_H
#define PX_BATTLE_TABLES_1_H

#include "../battle_handler/battle_handler.h"

/* DllBattle handler-table functions and the registrars that read them (static
 * analysis addresses; runtime addresses are module-relative). Names are
 * analyst interpretations.
 *
 * Each row of a handler table (ability, item, move, field, side-effect and
 * position-effect) holds an id and a table function. The 382 regular table
 * functions of the non-move tables are four-instruction stubs: they store a
 * count word (priority << 16 | count) through their first argument and return
 * the address of their entries table of (event byte, handler) pairs. Two of
 * them load the count word from a literal; all others move a small constant.
 * Row 10 of the position-effect table (the terrains) has one irregular table
 * function that also takes a parameter. */
struct px_battle_tables_api {
    const struct px_battle_handler_api *handler;
    uint32_t layers;                 /* 10b11c00: 17 records of 16 bytes per side */
    uint32_t side_table;             /* 108d4fe8: 17 rows of 12 bytes */
    uint32_t position_table;         /* 108d6768: 11 rows of 8 bytes */
    void (*overflow)(void *user);    /* 108003a4(0, 0, 0): a registration failed; returns */
    /* Indirect call of a position table function with the registered value as its parameter (r1). */
    uint32_t (*position_entries)(void *user, uint32_t function, uint32_t count_out, uint32_t parameter);
};

/* A regular table function: stores `count_word` at `count_out`, returns `entries`. */
uint32_t px_battle_table_stub(uint32_t count_out, uint32_t count_word, uint32_t entries,
                              const struct px_pokemon_api *memory);

/* 108bbf98: the terrain table function. Parameter 1 gives the Grassy entries
 * (count 2), 2 the Misty entries (count 3), 3 the Electric entries (count 3)
 * and anything else the default entries (count 1). */
struct px_battle_terrain_tables {
    uint32_t grassy;     /* 108d67d0 */
    uint32_t misty;      /* 108d67e0 */
    uint32_t electric;   /* 108d67f8 */
    uint32_t fallback;   /* 108d380c */
};
uint32_t px_battle_terrain_table(uint32_t count_out, uint32_t parameter,
                                 const struct px_battle_terrain_tables *tables,
                                 const struct px_pokemon_api *memory);

/* 1087c024: handlers of `owner` whose byte +0x3b is `slot` lose flag bit 30; 1 when any matched. */
uint32_t px_battle_handlers_unmark(uint32_t slot, uint32_t owner, const struct px_battle_handler_api *api);
/* 10882674: set flag bit 26 of a handler. */
void px_battle_handler_mark_26(uint32_t handler, const struct px_battle_handler_api *api);

/* 108acd0c: register a side effect (owner 2) for `side` (0 or 1) and `id` (below 17), or add a layer. */
uint32_t px_battle_register_side(uint32_t side, uint32_t id, uint32_t value, const struct px_battle_tables_api *api);
/* 108bc690: register a position effect (owner 3); 0 when the id has no row. */
uint32_t px_battle_register_position(uint32_t id, uint32_t value, const struct px_battle_tables_api *api);
/* 108a5bd8: register the item held by a battler's pokemon, unless the battler has handlers of owner 5 to unmark. */
void px_battle_register_item_checked(uint32_t pokemon, const struct px_battle_tables_api *api);
/* 108abf28: register a given item for a pokemon and flag it (bit 26, work word 6 = 1); returns the handler. */
uint32_t px_battle_register_item_given(uint32_t pokemon, uint32_t item, const struct px_battle_tables_api *api);

#endif
