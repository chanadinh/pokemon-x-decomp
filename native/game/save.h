#ifndef PX_SAVE_H
#define PX_SAVE_H
#include <stddef.h>
#include <stdint.h>

#include "battle.h"

/* The viewer's save file (its own container, not the game's save data):
 *
 *   0  "PXSV", u16 version 1, u16 party count (1..6)
 *   8  u32 zone, i32 world tile x, i32 world tile z (scene tile + 40 x the
 *      scene's first cell)
 *  20  u8 facing (0..3), three zero bytes
 *  24  u32 Poké Balls, u32 random state, u32 trainer id
 *  36  six 0x104-byte party slots: the game's party Pokémon layout, a 0xe8-byte
 *      record (8-byte header, four 0x38-byte blocks) and the 0x1c-byte party
 *      extension, encoded by the reconstructed codec of
 *      reconstructed/pokemon_data (checksum, block order by the key's bits
 *      13..17 through the code.bin table at 0x00570eec, the 0x41c64e6d XOR
 *      stream over each region). Unused slots are zero.
 *
 * Fields written (logical block offsets): through the reconstructed setters
 * the key, species (A+0), trainer id (A+4), experience (A+8), ability (A+0x0c,
 * the species' first) and its slot flag, nature (A+0x14), IVs (B+0x34) and
 * the party level (extension +4); directly into the decoded blocks the four
 * moves (B+0x1a) and their PP (B+0x22), the current HP (extension +8) and the
 * six stats (extension +0x0a). Offsets as in PKHeX's PK6 for the cross-check;
 * other fields stay zero. A record that fails its checksum is rejected. */
enum { PX_SAVE_HEADER = 36, PX_SAVE_SLOT = 0x104, PX_SAVE_SIZE = PX_SAVE_HEADER + 6 * PX_SAVE_SLOT };
struct px_save {
    uint32_t zone;
    int32_t world_x, world_z;
    uint8_t facing;
    uint32_t balls, rng, trainer;
    uint32_t party_count;
    struct px_battler party[6];
};
/* table: the 128 bytes at code.bin 0x00570eec (32 rows of 4). Records whose
 * key is 0 are given one derived from their contents (and it is stored back).
 * Returns 1, or 0 with a message. */
int px_save_encode(struct px_save *save, const struct px_battle_data *data, const uint8_t table[128],
                   uint8_t out[PX_SAVE_SIZE], char *error, size_t error_size);
/* Decodes, checks each checksum and rebuilds the battlers from the
 * reconstructed stat routines (stored stats must agree). */
int px_save_decode(const uint8_t *in, size_t size, const struct px_battle_data *data, const uint8_t table[128],
                   struct px_save *save, char *error, size_t error_size);
#endif
