/* The viewer save file: round trip through the reconstructed codec, the
 * stored layout read back without it, and rejections. Synthetic data only. */
#include "save.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static struct px_personal personal[3];
static struct px_move moves[3];
static uint8_t chart[PX_TYPE_COUNT][PX_TYPE_COUNT];

/* A synthetic block order: the 24 permutations of 0..3 in lexical order,
 * then the first eight again (the game's table is read from code.bin). */
static void permutations(uint8_t table[128])
{
    unsigned row = 0;
    for (unsigned a = 0; a < 4; ++a)
        for (unsigned b = 0; b < 4; ++b)
            for (unsigned c = 0; c < 4; ++c)
                for (unsigned d = 0; d < 4; ++d)
                    if (a != b && a != c && a != d && b != c && b != d && c != d) {
                        table[4 * row] = (uint8_t)a; table[4 * row + 1] = (uint8_t)b;
                        table[4 * row + 2] = (uint8_t)c; table[4 * row + 3] = (uint8_t)d;
                        ++row;
                    }
    memcpy(table + 96, table, 32);
}

/* Independent decode of one stored record (no reconstructed code): the XOR
 * stream, the checksum and the block order. */
static uint32_t stored_word(const uint8_t *slot, const uint8_t table[128], unsigned block, unsigned offset,
                            unsigned width, unsigned *checksum_ok)
{
    uint8_t plain[0xe8];
    memcpy(plain, slot, sizeof plain);
    uint32_t key = (uint32_t)slot[0] | (uint32_t)slot[1] << 8 | (uint32_t)slot[2] << 16 | (uint32_t)slot[3] << 24;
    uint32_t k = key, sum = 0;
    for (unsigned i = 8; i < 0xe8; i += 2) {
        k = k * 0x41c64e6du + 0x6073u;
        const uint16_t h = (uint16_t)((plain[i] | plain[i + 1] << 8) ^ (k >> 16));
        plain[i] = (uint8_t)h; plain[i + 1] = (uint8_t)(h >> 8);
        sum += h;
    }
    *checksum_ok = (uint16_t)sum == (uint16_t)(slot[6] | slot[7] << 8);
    const unsigned physical = table[4 * ((key >> 13) & 31) + block];
    const uint8_t *p = plain + 8 + 0x38 * physical + offset;
    uint32_t v = 0;
    for (unsigned i = 0; i < width; ++i) v |= (uint32_t)p[i] << (8 * i);
    return v;
}

int main(void)
{
    for (unsigned i = 0; i < PX_TYPE_COUNT * PX_TYPE_COUNT; ++i) chart[i / PX_TYPE_COUNT][i % PX_TYPE_COUNT] = 4;
    const uint8_t base[2][6] = {{41, 56, 40, 71, 62, 44}, {38, 36, 38, 57, 32, 36}};
    for (unsigned s = 1; s <= 2; ++s) { memcpy(personal[s].base, base[s - 1], 6); personal[s].abilities[0] = 60 + s; }
    moves[1] = (struct px_move){.category = 1, .power = 40, .accuracy = 100, .pp = 35};
    moves[2] = (struct px_move){.category = 0, .accuracy = 100, .pp = 40};
    const struct px_battle_data data = {personal, 3, moves, 3, (const uint8_t (*)[PX_TYPE_COUNT])chart, NULL, NULL, 0,
                                        NULL, 0};
    uint8_t table[128];
    permutations(table);
    struct px_save save;
    memset(&save, 0, sizeof save);
    save.zone = 259; save.world_x = 1234; save.world_z = -5; save.facing = 2;
    save.balls = 7; save.rng = 0xdeadbeef; save.trainer = 0x00012345; save.party_count = 2;
    const uint8_t iv0[6] = {31, 1, 2, 3, 4, 5}, iv1[6] = {0, 10, 20, 30, 15, 25};
    const uint16_t m0[4] = {1, 2, 0, 0}, m1[4] = {2, 0, 0, 0};
    assert(px_battler_make(&data, 1, 7, iv0, 0, m0, &save.party[0]));
    assert(px_battler_make(&data, 2, 3, iv1, 0, m1, &save.party[1]));
    save.party[0].exp = 400; save.party[0].hp = 9; save.party[0].pp[0] = 20;
    save.party[1].key = 0x00006000; /* key bits 13..17: row 3 */
    uint8_t file[PX_SAVE_SIZE];
    char error[128];
    assert(px_save_encode(&save, &data, table, file, error, sizeof error));
    assert(save.party[0].key != 0 && save.party[1].key == 0x00006000);
    assert(!memcmp(file, "PXSV", 4) && file[6] == 2 && file[20] == 2);
    /* The stored layout, read back independently. */
    for (unsigned i = 0; i < 2; ++i) {
        const uint8_t *slot = file + PX_SAVE_HEADER + i * PX_SAVE_SLOT;
        const struct px_battler *b = &save.party[i];
        unsigned ok;
        assert(stored_word(slot, table, 0, 0, 2, &ok) == b->species && ok);
        assert(stored_word(slot, table, 0, 4, 4, &ok) == save.trainer);
        assert(stored_word(slot, table, 0, 8, 4, &ok) == b->exp);
        assert(stored_word(slot, table, 0, 0x0c, 1, &ok) == 60u + b->species);
        assert(stored_word(slot, table, 0, 0x14, 1, &ok) == b->nature);
        const uint32_t ivs = stored_word(slot, table, 1, 0x34, 4, &ok);
        assert((ivs & 31) == b->iv[0] && ((ivs >> 5) & 31) == b->iv[1] && ((ivs >> 10) & 31) == b->iv[2] &&
               ((ivs >> 15) & 31) == b->iv[3] && ((ivs >> 20) & 31) == b->iv[4] && ((ivs >> 25) & 31) == b->iv[5]);
        for (unsigned k = 0; k < 4; ++k) {
            assert(stored_word(slot, table, 1, 0x1a + 2 * k, 2, &ok) == b->moves[k]);
            assert(stored_word(slot, table, 1, 0x22 + k, 1, &ok) == b->pp[k]);
        }
    }
    /* Round trip. */
    struct px_save back;
    assert(px_save_decode(file, sizeof file, &data, table, &back, error, sizeof error));
    assert(back.zone == 259 && back.world_x == 1234 && back.world_z == -5 && back.facing == 2 && back.balls == 7 &&
           back.rng == 0xdeadbeef && back.trainer == 0x12345 && back.party_count == 2);
    for (unsigned i = 0; i < 2; ++i) assert(!memcmp(&back.party[i], &save.party[i], sizeof back.party[i]));
    uint8_t again[PX_SAVE_SIZE];
    assert(px_save_encode(&back, &data, table, again, error, sizeof error) && !memcmp(again, file, sizeof file));
    /* Rejections. */
    uint8_t bad[PX_SAVE_SIZE];
    memcpy(bad, file, sizeof bad); bad[PX_SAVE_HEADER + 0x40] ^= 1; /* one encoded byte */
    assert(!px_save_decode(bad, sizeof bad, &data, table, &back, error, sizeof error) && strstr(error, "checksum"));
    memcpy(bad, file, sizeof bad); bad[0] = 'Q';
    assert(!px_save_decode(bad, sizeof bad, &data, table, &back, error, sizeof error));
    assert(!px_save_decode(file, sizeof file - 1, &data, table, &back, error, sizeof error));
    memcpy(bad, file, sizeof bad); bad[6] = 7;
    assert(!px_save_decode(bad, sizeof bad, &data, table, &back, error, sizeof error));
    memcpy(bad, file, sizeof bad); bad[20] = 4;
    assert(!px_save_decode(bad, sizeof bad, &data, table, &back, error, sizeof error));
    struct px_save tampered = save; /* stored stats must follow from the record */
    tampered.party[0].stats[1] += 1;
    assert(px_save_encode(&tampered, &data, table, bad, error, sizeof error));
    assert(!px_save_decode(bad, sizeof bad, &data, table, &back, error, sizeof error) && strstr(error, "stats"));
    tampered = save; tampered.party[0].hp = tampered.party[0].stats[0] + 1;
    assert(px_save_encode(&tampered, &data, table, bad, error, sizeof error));
    assert(!px_save_decode(bad, sizeof bad, &data, table, &back, error, sizeof error) && strstr(error, "HP"));
    tampered = save; tampered.party_count = 0;
    assert(!px_save_encode(&tampered, &data, table, bad, error, sizeof error));
    tampered = save; tampered.party[1].species = 3;
    assert(!px_save_encode(&tampered, &data, table, bad, error, sizeof error));
    puts("save tests passed");
    return 0;
}
