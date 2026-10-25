#include "pokedata.h"

#include <string.h>

static uint16_t u16(const uint8_t *p) { return (uint16_t)(p[0] | p[1] << 8); }
static uint32_t u32(const uint8_t *p) { return (uint32_t)u16(p) | (uint32_t)u16(p + 2) << 16; }

void px_slot_range(enum px_slot_kind kind, unsigned *first, unsigned *count)
{
    static const unsigned counts[PX_SLOT_KINDS] = {12, 12, 12, 12, 12, 5, 5, 3, 3, 3, 15};
    unsigned at = 0;
    for (unsigned k = 0; k < (unsigned)kind && k < PX_SLOT_KINDS; ++k) at += counts[k];
    *first = at;
    *count = (unsigned)kind < PX_SLOT_KINDS ? counts[kind] : 0;
}

enum px_pokedata_result px_personal_decode(const uint8_t *d, size_t size, struct px_personal *out)
{
    if (!out) return PX_POKEDATA_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    if (!d) return PX_POKEDATA_INVALID_ARGUMENT;
    if (size < 0x40) return PX_POKEDATA_TRUNCATED;
    memcpy(out->base, d, 6);
    out->types[0] = d[6]; out->types[1] = d[7];
    if (out->types[0] >= PX_TYPE_COUNT || out->types[1] >= PX_TYPE_COUNT) return PX_POKEDATA_INVALID;
    out->catch_rate = d[8]; out->evolution_stage = d[9];
    out->gender = d[0x12]; out->growth = d[0x15];
    for (unsigned k = 0; k < 3; ++k) out->abilities[k] = d[0x18 + k];
    out->escape_rate = d[0x1b];
    out->form_index = u16(d + 0x1c); out->form_count = d[0x20];
    out->base_exp = u16(d + 0x22);
    return PX_POKEDATA_OK;
}

enum px_pokedata_result px_move_decode(const uint8_t *d, size_t size, struct px_move *out)
{
    if (!out) return PX_POKEDATA_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    if (!d) return PX_POKEDATA_INVALID_ARGUMENT;
    if (size < 0x22) return PX_POKEDATA_TRUNCATED;
    out->type = d[0]; out->quality = d[1]; out->category = d[2]; out->power = d[3];
    out->accuracy = d[4]; out->pp = d[5]; out->priority = d[6];
    out->hit_min = d[7] & 15; out->hit_max = d[7] >> 4;
    out->inflict = u16(d + 8); out->inflict_percent = d[10]; out->inflict_duration = d[11];
    out->turn_min = d[12]; out->turn_max = d[13]; out->crit_stage = d[14]; out->flinch = d[15];
    out->effect = u16(d + 16); out->recoil = d[18]; out->healing = d[19]; out->target = d[20];
    for (unsigned k = 0; k < 3; ++k) {
        out->stat[k] = d[21 + k];
        out->stage[k] = (int8_t)d[24 + k];
        out->stat_percent[k] = d[27 + k];
    }
    out->flags = u32(d + 30);
    if (out->type >= PX_TYPE_COUNT || out->category > 2) return PX_POKEDATA_INVALID;
    return PX_POKEDATA_OK;
}

enum px_pokedata_result px_learnset_decode(const uint8_t *d, size_t size, struct px_learnset *out)
{
    if (!out) return PX_POKEDATA_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    if (!d && size) return PX_POKEDATA_INVALID_ARGUMENT;
    for (size_t at = 0;; at += 4) {
        if (at + 4 > size) { memset(out, 0, sizeof *out); return PX_POKEDATA_TRUNCATED; }
        const uint16_t move = u16(d + at), level = u16(d + at + 2);
        if (move == 0xffff && level == 0xffff) return PX_POKEDATA_OK;
        if (out->count == PX_MAX_LEARNSET || level > 100) { memset(out, 0, sizeof *out); return PX_POKEDATA_INVALID; }
        out->move[out->count] = move; out->level[out->count] = (int16_t)level;
        ++out->count;
    }
}

unsigned px_learnset_moves_at(const struct px_learnset *l, unsigned level, uint16_t out[4])
{
    memset(out, 0, 4 * sizeof *out);
    unsigned used = 0;
    for (uint32_t i = 0; l && i < l->count; ++i) {
        if ((unsigned)l->level[i] > level) break;
        const uint16_t move = l->move[i];
        if (out[0] == move || out[1] == move || out[2] == move || out[3] == move) continue;
        /* Constructor insertion (00150064) retains slot order when full:
         * discard the oldest move, then append. Known moves keep their slot. */
        if (used == 4) memmove(out, out + 1, 3 * sizeof *out);
        else ++used;
        out[used - 1] = move;
    }
    return used;
}

enum px_pokedata_result px_encounter_table_decode(const uint8_t *d, size_t size, struct px_encounter_table *out)
{
    if (!out) return PX_POKEDATA_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    if (!d && size) return PX_POKEDATA_INVALID_ARGUMENT;
    if (size < 0x10 + 4 * PX_SLOT_COUNT) return PX_POKEDATA_TRUNCATED;
    memcpy(out->header, d, 16);
    for (unsigned i = 0; i < PX_SLOT_COUNT; ++i) {
        const uint8_t *s = d + 0x10 + 4 * i;
        struct px_encounter_slot *slot = &out->slots[i];
        slot->species = u16(s) & 0x7ff; slot->form = u16(s) >> 11;
        slot->min_level = s[2]; slot->max_level = s[3];
        if (slot->species && (slot->min_level < 1 || slot->max_level > 100 || slot->min_level > slot->max_level)) {
            memset(out, 0, sizeof *out);
            return PX_POKEDATA_INVALID;
        }
    }
    return PX_POKEDATA_OK;
}

enum px_pokedata_result px_type_chart_decode(const uint8_t *cro, size_t size, uint8_t out[PX_TYPE_COUNT][PX_TYPE_COUNT])
{
    enum { OFFSET = 0xD12A8 };
    if (!out) return PX_POKEDATA_INVALID_ARGUMENT;
    memset(out, 0, PX_TYPE_COUNT * PX_TYPE_COUNT);
    if (!cro) return PX_POKEDATA_INVALID_ARGUMENT;
    if (size < OFFSET + PX_TYPE_COUNT * PX_TYPE_COUNT) return PX_POKEDATA_TRUNCATED;
    if (memcmp(cro + 0x80, "CRO0", 4)) return PX_POKEDATA_INVALID;
    for (unsigned i = 0; i < PX_TYPE_COUNT * PX_TYPE_COUNT; ++i) {
        const uint8_t v = cro[OFFSET + i];
        if (v != 0 && v != 2 && v != 4 && v != 8) { memset(out, 0, PX_TYPE_COUNT * PX_TYPE_COUNT); return PX_POKEDATA_INVALID; }
        out[i / PX_TYPE_COUNT][i % PX_TYPE_COUNT] = v;
    }
    return PX_POKEDATA_OK;
}

const char *px_pokedata_result_string(enum px_pokedata_result result)
{
    switch (result) {
    case PX_POKEDATA_OK: return "ok";
    case PX_POKEDATA_INVALID_ARGUMENT: return "invalid argument";
    case PX_POKEDATA_TRUNCATED: return "truncated Pokémon data";
    case PX_POKEDATA_INVALID: return "invalid Pokémon data";
    case PX_POKEDATA_NO_MEMORY: return "out of memory";
    }
    return "unknown Pokémon data result";
}
