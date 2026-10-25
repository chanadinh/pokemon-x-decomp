#include "pokedata.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void put16(uint8_t *p, unsigned v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); }

int main(void)
{
    /* Personal: Froakie-like base stats and Water type. */
    uint8_t p[0x40] = {41, 56, 40, 71, 62, 44, 10, 10, 45, 1};
    p[0x12] = 31; p[0x15] = 3; p[0x18] = 67; p[0x19] = 67; p[0x1a] = 168; p[0x1b] = 0;
    put16(p + 0x1c, 0); p[0x20] = 1; put16(p + 0x22, 63);
    struct px_personal per;
    assert(px_personal_decode(p, sizeof p, &per) == PX_POKEDATA_OK);
    assert(per.base[3] == 71 && per.types[0] == 10 && per.catch_rate == 45 && per.growth == 3 &&
           per.abilities[2] == 168 && per.base_exp == 63 && per.gender == 31 && per.form_count == 1);
    assert(px_personal_decode(p, 0x3f, &per) == PX_POKEDATA_TRUNCATED && per.base[0] == 0);
    p[7] = 18;
    assert(px_personal_decode(p, sizeof p, &per) == PX_POKEDATA_INVALID);
    assert(px_personal_decode(NULL, 0x40, &per) == PX_POKEDATA_INVALID_ARGUMENT);
    /* Move: Growl-like (status, lowers Attack by 1, opponents). */
    uint8_t m[0x24] = {0, 0, 0, 0, 100, 40, 0, 0x11};
    m[20] = 5; m[21] = 1; m[24] = 0xff; m[27] = 0; put16(m + 16, 18); m[30] = 0x34; m[33] = 0x12;
    struct px_move mv;
    assert(px_move_decode(m, sizeof m, &mv) == PX_POKEDATA_OK);
    assert(mv.accuracy == 100 && mv.pp == 40 && mv.hit_min == 1 && mv.hit_max == 1 && mv.target == 5 &&
           mv.stat[0] == 1 && mv.stage[0] == -1 && mv.effect == 18 && mv.flags == 0x12000034);
    assert(px_move_decode(m, 0x21, &mv) == PX_POKEDATA_TRUNCATED);
    m[2] = 3;
    assert(px_move_decode(m, sizeof m, &mv) == PX_POKEDATA_INVALID);
    /* Learnset: moves at levels 1, 1, 5, 8, 10, 14, then the terminator. */
    uint8_t l[64];
    const unsigned pairs[][2] = {{1, 1}, {45, 1}, {145, 5}, {98, 8}, {122, 10}, {352, 14}, {0xffff, 0xffff}};
    for (unsigned i = 0; i < 7; ++i) { put16(l + 4 * i, pairs[i][0]); put16(l + 4 * i + 2, pairs[i][1]); }
    struct px_learnset ls;
    assert(px_learnset_decode(l, 28, &ls) == PX_POKEDATA_OK && ls.count == 6 && ls.move[5] == 352 && ls.level[5] == 14);
    uint16_t at[4];
    assert(px_learnset_moves_at(&ls, 5, at) == 3 && at[0] == 1 && at[1] == 45 && at[2] == 145 && at[3] == 0);
    /* The original constructor shifts the oldest out and appends in order. */
    assert(px_learnset_moves_at(&ls, 14, at) == 4 && at[0] == 145 && at[1] == 98 && at[2] == 122 && at[3] == 352);
    assert(px_learnset_moves_at(&ls, 0, at) == 0 && at[0] == 0);
    const struct px_learnset duplicates = {.count = 8, .move = {1, 2, 3, 4, 5, 3, 6, 1},
                                           .level = {1, 1, 1, 1, 2, 2, 3, 4}};
    assert(px_learnset_moves_at(&duplicates, 3, at) == 4 && at[0] == 3 && at[1] == 4 && at[2] == 5 && at[3] == 6);
    /* Once forgotten, the same move can be learned again at a later entry. */
    assert(px_learnset_moves_at(&duplicates, 4, at) == 4 && at[0] == 4 && at[1] == 5 && at[2] == 6 && at[3] == 1);
    assert(px_learnset_decode(l, 24, &ls) == PX_POKEDATA_TRUNCATED && ls.count == 0); /* no terminator */
    put16(l + 6, 101);
    assert(px_learnset_decode(l, 28, &ls) == PX_POKEDATA_INVALID);
    /* Encounters: Route-2-like grass slots, one slot with a form. */
    uint8_t e[0x10 + 4 * PX_SLOT_COUNT] = {1};
    put16(e + 0x10, 659); e[0x12] = 2; e[0x13] = 2;
    put16(e + 0x14, 664 | 30u << 11); e[0x16] = 2; e[0x17] = 3;
    struct px_encounter_table t;
    assert(px_encounter_table_decode(e, sizeof e, &t) == PX_POKEDATA_OK);
    assert(t.slots[0].species == 659 && t.slots[1].species == 664 && t.slots[1].form == 30 &&
           t.slots[1].max_level == 3 && t.slots[2].species == 0 && t.header[0] == 1);
    assert(px_encounter_table_decode(e, sizeof e - 1, &t) == PX_POKEDATA_TRUNCATED);
    assert(px_encounter_table_decode(NULL, 0, &t) == PX_POKEDATA_TRUNCATED); /* a zone without wild Pokémon */
    e[0x16] = 4;
    assert(px_encounter_table_decode(e, sizeof e, &t) == PX_POKEDATA_INVALID && t.slots[0].species == 0);
    unsigned first, count;
    px_slot_range(PX_SLOTS_GRASS, &first, &count); assert(first == 0 && count == 12);
    px_slot_range(PX_SLOTS_SURF, &first, &count); assert(first == 60 && count == 5);
    px_slot_range(PX_SLOTS_HORDE, &first, &count); assert(first == 79 && count == 15 && first + count == PX_SLOT_COUNT);
    /* Type chart inside a CRO image. */
    static uint8_t cro[0xD12A8 + 324];
    memcpy(cro + 0x80, "CRO0", 4);
    for (unsigned i = 0; i < 324; ++i) cro[0xD12A8 + i] = 4;
    cro[0xD12A8 + 0 * 18 + 7] = 0; /* Normal on Ghost */
    cro[0xD12A8 + 9 * 18 + 11] = 8; /* Fire on Grass */
    uint8_t chart[PX_TYPE_COUNT][PX_TYPE_COUNT];
    assert(px_type_chart_decode(cro, sizeof cro, chart) == PX_POKEDATA_OK && chart[0][7] == 0 && chart[9][11] == 8 &&
           chart[1][1] == 4);
    assert(px_type_chart_decode(cro, sizeof cro - 1, chart) == PX_POKEDATA_TRUNCATED);
    cro[0xD12A8 + 5] = 3;
    assert(px_type_chart_decode(cro, sizeof cro, chart) == PX_POKEDATA_INVALID && chart[9][11] == 0);
    cro[0xD12A8 + 5] = 4; cro[0x80] = 'X';
    assert(px_type_chart_decode(cro, sizeof cro, chart) == PX_POKEDATA_INVALID);
    assert(!strcmp(px_pokedata_result_string(PX_POKEDATA_TRUNCATED), "truncated Pokémon data"));
    puts("Pokemon data tests passed");
    return 0;
}
