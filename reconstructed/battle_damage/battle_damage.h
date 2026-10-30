#ifndef PX_BATTLE_DAMAGE_H
#define PX_BATTLE_DAMAGE_H

#include "../pokemon_constructor/pokemon_constructor.h"

/* DllBattle damage calculation (static analysis address 1086a1e0; runtime
 * address is module-relative). Analyst names. Logical ARM32 addresses.
 * Battle Pokemon: id byte +0x19. Move: id u16 +0, type byte +6, byte +7.
 * Context: +4 -> object whose word +0x10 is tested by the rule service,
 * +0x1c -> field state whose byte +1 is the weather. */
struct px_battle_damage_api {
    const struct px_pokemon_api *memory;
    void *user;
    /* Battle event system (DllBattle). */
    void (*event_push)(void *user);                                        /* 1086bb84 */
    void (*event_pop)(void *user);                                         /* 1086b880 */
    void (*event_set_id)(void *user, uint32_t key, uint32_t value);        /* 1086bac0 */
    void (*event_set_value)(void *user, uint32_t key, uint32_t value);     /* 1086b9fc */
    void (*event_set_scaled)(void *user, uint32_t key, uint32_t value);    /* 1086b520 */
    void (*event_set_multiplier)(void *user, uint32_t key, uint32_t value,
                                 uint32_t minimum, uint32_t maximum);       /* 10867c9c */
    void (*event_dispatch)(void *user, uint32_t context, uint32_t event);  /* 1086b9a4 */
    uint32_t (*event_get)(void *user, uint32_t key);                       /* 1086b91c */
    /* Battle helpers (DllBattle). */
    uint32_t (*attack)(void *user, uint32_t context, uint32_t attacker, uint32_t defender,
                       uint32_t move, uint32_t critical, uint32_t flag_out); /* 108202d8 */
    uint32_t (*defense)(void *user, uint32_t context, uint32_t attacker, uint32_t defender,
                        uint32_t move, uint32_t critical);                 /* 10822330 */
    uint32_t (*pokemon_value)(void *user, uint32_t pokemon, uint32_t key); /* 10874420; key 0xf is level */
    uint32_t (*attacker_types)(void *user, uint32_t pokemon);              /* 1086f960 */
    uint32_t (*pokemon_status)(void *user, uint32_t pokemon);              /* 1086d8ac; 4 is burn */
    uint32_t (*apply_effectiveness)(void *user, uint32_t damage, uint32_t effectiveness); /* 1089e7f0 */
    /* Main-executable services reached through import veneers. */
    uint32_t (*move_category)(void *user, uint32_t move);                  /* 0039ddc0 */
    uint32_t (*move_power)(void *user, uint32_t move);                     /* 0039e044 */
    uint32_t (*rule_flag)(void *user, uint32_t rules, uint32_t flag);      /* 001ce05c */
    uint32_t (*random)(void *user, uint32_t generator, uint32_t range);    /* 00174bb4 */
    /* Static DllBattle data object whose word +0x54 is the random generator
     * (literal at 1086a6dc); relocated, so supplied by the caller. */
    uint32_t random_owner;
    /* Logical byte standing in for the original stack flag written by attack(). */
    uint32_t flag_scratch;
};

struct px_battle_damage_args {
    uint32_t context, attacker, defender, move;
    uint32_t effectiveness;   /* stack +0x38: also event key 0x39 */
    uint32_t spread;          /* stack +0x3c: 4096-based, 0x1000 means none */
    uint32_t critical;        /* stack +0x40 */
    uint32_t fixed_roll;      /* stack +0x44: nonzero uses 85 instead of a random roll */
    uint32_t extra;           /* stack +0x48: event key 0x47 */
    uint32_t output;          /* stack +0x4c: u16 damage destination */
};

/* Returns 1 when event 0x4e supplied fixed damage, otherwise 0. The unsigned
 * divide helper 0015e958 is represented by C division; defense must be nonzero. */
uint32_t px_battle_calculate_damage(const struct px_battle_damage_args *args,
                                    const struct px_battle_damage_api *api);

/* 4096-based fixed-point product, rounded up when the remainder exceeds 0x800. */
uint32_t px_battle_apply_modifier(uint32_t value, uint32_t modifier);

#endif
