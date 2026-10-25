#include "battle.h"
#include "../platform/guest.h"
#include "../../reconstructed/pokemon_names/pokemon_names.h"

#include <string.h>

int px_battler_set_name(struct px_battler *b, const uint16_t *name, size_t units)
{
    if (!b || !name) return 0;
    size_t length = 0;
    while (length < units && length < 13 && name[length] != 0) {
        if (name[length] == 0x10) return 0;
        ++length;
    }
    if (length >= units || length >= 13) return 0;
    uint8_t source[26] = {0}, destination[26] = {0};
    for (size_t i = 0; i < length; ++i) {
        source[2 * i] = (uint8_t)name[i]; source[2 * i + 1] = (uint8_t)(name[i] >> 8);
    }
    struct px_guest guest;
    px_guest_init(&guest);
    const int mapped = px_guest_map_view(&guest, 0x100, sizeof source, PX_GUEST_READ, source, "name source") &&
        px_guest_map_view(&guest, 0x200, sizeof destination, PX_GUEST_READ | PX_GUEST_WRITE, destination, "name");
    const struct px_pokemon_api memory = px_guest_pokemon_api(&guest);
    const int ok = mapped && px_copy_game_string(0x200, 0x100, &memory) == length && guest.fault == PX_FAULT_NONE;
    if (ok) for (unsigned i = 0; i < 13; ++i)
        b->name[i] = (uint16_t)((uint32_t)destination[2 * i] | (uint32_t)destination[2 * i + 1] << 8);
    px_guest_free(&guest);
    return ok;
}
