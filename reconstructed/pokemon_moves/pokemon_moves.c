#include "pokemon_moves.h"

uint32_t px_pokemon_maximum_pp(uint32_t move, uint32_t pp_ups,
                              const struct px_move_api *api)
{
    if (pp_ups > UINT32_C(3)) pp_ups = 3;
    const uint32_t base = api->move_value(api->user, move, 5) & UINT32_C(0xff);
    /* The ARM UMULL by 0x51eb851f and shift by 37 divide base*ups*20
     * by 100 exactly over this bounded range (0..15300). */
    return (base + base * pp_ups * 20 / 100) & UINT32_C(0xff);
}

static uint32_t manager(uint32_t object, const struct px_pokemon_api *memory)
{
    return memory->load32(memory->user, object + UINT32_C(0x0c));
}

void px_pokemon_learn_move(uint32_t object, uint32_t move,
                           const struct px_move_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    for (uint32_t slot = 0; slot < 4; ++slot)
        if (px_read_pokemon_state_slot(manager(object, m), slot, PX_STATE_MOVE, m) == move)
            return;

    uint32_t slot;
    for (slot = 0; slot < 4; ++slot)
        if (px_read_pokemon_state_slot(manager(object, m), slot, PX_STATE_MOVE, m) == 0)
            break;

    if (slot == 4) {
        for (uint32_t source = 1; source < 4; ++source) {
            const uint32_t old_move = px_read_pokemon_state_slot(manager(object, m), source, PX_STATE_MOVE, m);
            const uint32_t old_pp = px_read_pokemon_state_slot(manager(object, m), source, PX_STATE_PP, m);
            const uint32_t old_ups = px_read_pokemon_state_slot(manager(object, m), source, PX_STATE_PP_UPS, m);
            px_write_pokemon_state_slot(manager(object, m), source - 1, PX_STATE_MOVE, old_move, m);
            px_write_pokemon_state_slot(manager(object, m), source - 1, PX_STATE_PP, (uint8_t)old_pp, m);
            px_write_pokemon_state_slot(manager(object, m), source - 1, PX_STATE_PP_UPS, (uint8_t)old_ups, m);
        }
        slot = 3;
    }

    /* Capture PP before any setter, then reload the manager for each write.
     * The move is passed to the resource service without halfword narrowing. */
    const uint32_t pp = api->move_value(api->user, move, 5);
    px_write_pokemon_state_slot(manager(object, m), slot, PX_STATE_MOVE, move, m);
    px_write_pokemon_state_slot(manager(object, m), slot, PX_STATE_PP_UPS, 0, m);
    px_write_pokemon_state_slot(manager(object, m), slot, PX_STATE_PP, (uint8_t)pp, m);
}
