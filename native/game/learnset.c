#include "battle.h"
#include "../platform/guest.h"
#include "../../reconstructed/pokemon_learnset/pokemon_learnset.h"
#include "../../reconstructed/pokemon_moves/pokemon_moves.h"

#include <string.h>

enum { OBJECT = 0x100, MANAGER = 0x200, DATA = 0x400, RECORD = 0x600, BUFFER = 0x700,
       SELECTED = 0x5d17b4, ORDER = 0x570eec };

static uint32_t move_value(void *user, uint32_t move, uint32_t selector)
{
    const struct px_battle_data *d = user;
    return selector == 5 && move < d->move_count ? d->moves[move].pp : 0;
}

int px_battle_initial_moves(const struct px_battle_data *d, const struct px_learnset *l,
                            uint32_t level, uint16_t out[4])
{
    if (!out) return 0;
    memset(out, 0, 4 * sizeof *out);
    if (!d || !d->moves || !l || l->count > PX_MAX_LEARNSET || level > 100) return 0;
    uint8_t object[16] = {0}, manager[16] = {0}, data[PX_POKEMON_DATA_SIZE] = {0};
    uint8_t record[PX_LEARNSET_RECORD_SIZE] = {0}, buffer[PX_LEARNSET_BUFFER_SIZE], selected[4] = {0};
    /* Temporary records use an invented identity block-order row. Their
     * contents never become save records or depend on a game's table. */
    uint8_t order[] = {0, 1, 2, 3};
    memset(buffer, 0xff, sizeof buffer);
    const uint32_t count = l->count < PX_LEARNSET_ENTRY_LIMIT ? l->count : PX_LEARNSET_ENTRY_LIMIT;
    record[8] = (uint8_t)count;
    for (uint32_t i = 0; i < count; ++i) {
        buffer[4 * i] = (uint8_t)l->move[i]; buffer[4 * i + 1] = (uint8_t)(l->move[i] >> 8);
        buffer[4 * i + 2] = (uint8_t)l->level[i]; buffer[4 * i + 3] = (uint8_t)((uint16_t)l->level[i] >> 8);
    }
    struct px_guest guest; px_guest_init(&guest);
    int ok = px_guest_map_view(&guest, OBJECT, sizeof object, PX_GUEST_READ | PX_GUEST_WRITE, object, "object") &&
        px_guest_map_view(&guest, MANAGER, sizeof manager, PX_GUEST_READ | PX_GUEST_WRITE, manager, "manager") &&
        px_guest_map_view(&guest, DATA, sizeof data, PX_GUEST_READ | PX_GUEST_WRITE, data, "data") &&
        px_guest_map_view(&guest, RECORD, sizeof record, PX_GUEST_READ | PX_GUEST_WRITE, record, "learnset") &&
        px_guest_map_view(&guest, BUFFER, sizeof buffer, PX_GUEST_READ, buffer, "entries") &&
        px_guest_map_view(&guest, SELECTED, sizeof selected, PX_GUEST_READ | PX_GUEST_WRITE, selected, "selected") &&
        px_guest_map_view(&guest, ORDER, sizeof order, PX_GUEST_READ, order, "invented order");
    if (!ok) { px_guest_free(&guest); return 0; }
    px_guest_store32(&guest, OBJECT + 12, MANAGER);
    px_guest_store32(&guest, MANAGER + 8, DATA);
    px_guest_store8(&guest, MANAGER + 13, 1); /* acquired, decoded */
    px_guest_store32(&guest, RECORD + 4, BUFFER);
    px_guest_store32(&guest, SELECTED, RECORD);
    const struct px_pokemon_api memory = px_guest_pokemon_api(&guest);
    const struct px_move_api api = {&memory, (void *)d, move_value};
    for (uint32_t i = 0; i < px_current_learnset_count(&memory); ++i) {
        if (px_current_learnset_level(i, &memory) > level) break;
        const uint32_t move = px_current_learnset_move(i, &memory);
        if (move >= d->move_count) { ok = 0; break; }
        px_pokemon_learn_move(OBJECT, move, &api);
    }
    uint16_t result[4];
    for (unsigned i = 0; i < 4; ++i)
        result[i] = (uint16_t)px_read_pokemon_state_slot(MANAGER, i, PX_STATE_MOVE, &memory);
    ok = ok && guest.fault == PX_FAULT_NONE;
    if (ok) memcpy(out, result, sizeof result);
    px_guest_free(&guest);
    return ok;
}
