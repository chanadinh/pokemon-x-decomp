#ifndef PX_BATTLE_HANDLER_H
#define PX_BATTLE_HANDLER_H

#include "../pokemon_constructor/pokemon_constructor.h"

/* DllBattle event handler registration (static analysis addresses; runtime
 * addresses are module-relative). Analyst names. Logical ARM32 addresses.
 * Handler layout as in ../battle_event/battle_event.h, plus: +0xc word,
 * +0x14 sort key ((7 - priority) << 24 | sub-priority, larger first), +0x38
 * u16 id (ability, item, move or field kind), +0x3b byte. Battle state +0x64
 * is the number of handlers in use: the pool array holds free handlers from
 * that index on (157 in all). Owners: 0 move, 1 field, 4 ability, 5 item.
 * Lookup tables hold 8-byte rows: an id (byte, word, u16, byte) and a
 * function returning the handler entries and storing (priority << 16 | count). */
#define PX_BATTLE_HANDLER_POOL 0x9d
#define PX_BATTLE_ABILITY_TABLE_STATIC UINT32_C(0x108d510c) /* literal 108679a8; 182 rows */
#define PX_BATTLE_ITEM_TABLE_STATIC UINT32_C(0x108d382c)    /* literal 108abed4; 182 rows */
#define PX_BATTLE_MOVE_TABLE_STATIC UINT32_C(0x108d3ddc)    /* literal 10869eb4; 296 rows */
#define PX_BATTLE_FIELD_TABLE_STATIC UINT32_C(0x108d6810)   /* literal 108a5bd0; 5 rows */

struct px_battle_handler_api {
    const struct px_pokemon_api *memory;
    void *user;
    void (*abort)(void *user);                                             /* 10800394(0, 0, 0); returns */
    uint32_t (*pokemon_value)(void *user, uint32_t pokemon, uint32_t key);   /* 10874420; key 0x10 ability */
    uint32_t (*raw_value)(void *user, uint32_t pokemon, uint32_t key);       /* 108743a8; key 0xc speed */
    /* Indirect call of a table function (guest address) with a count word. */
    uint32_t (*entries)(void *user, uint32_t function, uint32_t count_out);
    uint32_t battle;          /* battle state object (108fca00) */
    uint32_t pool;            /* handler pool array (10b1196c) */
    uint32_t ability_table, item_table, move_table, field_table;
    uint32_t count_scratch;   /* logical word standing in for the original stack slot */
};

/* 1084cfc0: take a pool handler, fill it and insert it by sort key. */
uint32_t px_battle_handler_register(uint32_t owner, uint32_t id, uint32_t priority, uint32_t sub_priority,
                                    uint32_t value, uint32_t entries, uint32_t count,
                                    const struct px_battle_handler_api *api);
/* 1084d1cc: remove one handler (deferred while it runs). */
void px_battle_handler_remove(uint32_t handler, const struct px_battle_handler_api *api);
/* 1088164c: remove every handler of owner 6. */
void px_battle_handler_remove_owner6(const struct px_battle_handler_api *api);
/* 1084d3c0 / 1084d370: registered handlers of one owner and value byte. */
uint32_t px_battle_handler_find(uint32_t owner, uint32_t value, const struct px_battle_handler_api *api);
uint32_t px_battle_handler_find_next(uint32_t handler, const struct px_battle_handler_api *api);
/* 1084cf8c: work word 0..6. */
void px_battle_handler_set_work(uint32_t handler, uint32_t index, uint32_t value,
                                const struct px_battle_handler_api *api);
/* 108678f8, 108abe30, 10869dd4, 108a5aac: register by id. */
uint32_t px_battle_register_ability(uint32_t pokemon, const struct px_battle_handler_api *api);
uint32_t px_battle_register_item(uint32_t pokemon, const struct px_battle_handler_api *api);
uint32_t px_battle_register_move(uint32_t pokemon, uint32_t move, uint32_t sub_priority,
                                 const struct px_battle_handler_api *api);
uint32_t px_battle_register_field(uint32_t kind, uint32_t value, uint32_t work6, uint32_t words,
                                  uint32_t word_count, const struct px_battle_handler_api *api);

#endif
