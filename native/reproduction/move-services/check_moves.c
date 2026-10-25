/* Optional local-data comparison: reconstructed constructor insertion versus
 * the native learnset selector, at every level of every archive member.
 * Uses invented identity block order, not an embedded game resource. */
#include "../../assets/garc.h"
#include "../../assets/pokedata.h"
#include "../../platform/guest.h"
#include "../../../reconstructed/pokemon_moves/pokemon_moves.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { OBJECT = 0x100, MANAGER = 0x200, DATA = 0x400, TABLE = 0x570eec };
#define REQUIRE(c) do { if (!(c)) { fprintf(stderr, "check_moves:%d: %s\n", __LINE__, #c); exit(1); } } while (0)

struct resources { struct px_move *moves; size_t count; };
static uint32_t move_value(void *user, uint32_t move, uint32_t selector)
{
    const struct resources *r = user;
    REQUIRE(selector == 5 && move < r->count);
    return r->moves[move].pp;
}
static struct px_garc *archive(const char *romfs, const char *relative)
{
    char path[2048], error[256];
    REQUIRE(snprintf(path, sizeof path, "%s/%s", romfs, relative) < (int)sizeof path);
    struct px_garc *g = NULL;
    if (!px_garc_open(path, &g, error, sizeof error)) { fprintf(stderr, "%s\n", error); exit(1); }
    return g;
}
static uint8_t *member(struct px_garc *g, size_t index, size_t *size)
{
    const struct px_garc_member *m = px_garc_member_at(g, index);
    char error[256]; uint8_t *out = NULL;
    REQUIRE(m);
    if (!px_garc_read(g, m->entry, m->subentry, 1u << 24, &out, size, error, sizeof error)) {
        fprintf(stderr, "%s\n", error); exit(1);
    }
    return out;
}
int main(int argc, char **argv)
{
    if (argc != 2) { fprintf(stderr, "usage: check_moves ROMFS\n"); return 2; }
    struct resources r = {0};
    struct px_garc *g = archive(argv[1], "a/2/1/2");
    r.count = px_garc_count(g); r.moves = calloc(r.count, sizeof *r.moves); REQUIRE(r.moves);
    for (size_t i = 0; i < r.count; ++i) {
        size_t size; uint8_t *d = member(g, i, &size);
        REQUIRE(px_move_decode(d, size, r.moves + i) == PX_POKEDATA_OK);
        free(d);
    }
    px_garc_close(g);
    uint8_t object[16] = {0}, manager[16] = {0}, data[PX_POKEMON_DATA_SIZE] = {0}, order[] = {0, 1, 2, 3};
    struct px_guest guest; px_guest_init(&guest);
    REQUIRE(px_guest_map_view(&guest, OBJECT, sizeof object, PX_GUEST_READ | PX_GUEST_WRITE, object, "object"));
    REQUIRE(px_guest_map_view(&guest, MANAGER, sizeof manager, PX_GUEST_READ | PX_GUEST_WRITE, manager, "manager"));
    REQUIRE(px_guest_map_view(&guest, DATA, sizeof data, PX_GUEST_READ | PX_GUEST_WRITE, data, "data"));
    REQUIRE(px_guest_map_view(&guest, TABLE, sizeof order, PX_GUEST_READ, order, "invented order"));
    const struct px_pokemon_api memory = px_guest_pokemon_api(&guest);
    const struct px_move_api api = {&memory, &r, move_value};
    uint64_t pp_cases = 0, level_cases = 0, changed = 0;
    for (uint32_t i = 0; i < r.count; ++i) {
        const uint32_t ups[] = {0, 1, 2, 3, 255, UINT32_MAX};
        for (unsigned u = 0; u < sizeof ups / sizeof ups[0]; ++u) {
            const uint32_t capped = ups[u] < 3 ? ups[u] : 3, base = r.moves[i].pp;
            REQUIRE(px_pokemon_maximum_pp(i, ups[u], &api) == (base + base * capped / 5) % 256);
            ++pp_cases;
        }
    }
    g = archive(argv[1], "a/2/1/4");
    const size_t learnsets = px_garc_count(g);
    for (size_t i = 0; i < learnsets; ++i) {
        size_t size; uint8_t *d = member(g, i, &size);
        struct px_learnset l;
        REQUIRE(px_learnset_decode(d, size, &l) == PX_POKEDATA_OK); free(d);
        for (unsigned level = 0; level <= 100; ++level) {
            memset(manager, 0, sizeof manager); memset(data, 0, sizeof data);
            px_guest_store32(&guest, OBJECT + 0xc, MANAGER);
            px_guest_store32(&guest, MANAGER + 8, DATA);
            px_guest_store8(&guest, MANAGER + 0xd, 1);
            uint16_t native[4], old[4] = {0}; unsigned old_slot = 0;
            const unsigned used = px_learnset_moves_at(&l, level, native);
            for (uint32_t k = 0; k < l.count && (unsigned)l.level[k] <= level; ++k) {
                px_pokemon_learn_move(OBJECT, l.move[k], &api);
                if (old[0] == l.move[k] || old[1] == l.move[k] || old[2] == l.move[k] || old[3] == l.move[k]) continue;
                old[old_slot] = l.move[k]; old_slot = (old_slot + 1) % 4;
            }
            unsigned actual_used = 0;
            for (unsigned k = 0; k < 4; ++k) {
                const uint32_t move = px_read_pokemon_state_slot(MANAGER, k, PX_STATE_MOVE, &memory);
                REQUIRE(move == native[k]);
                REQUIRE(px_read_pokemon_state_slot(MANAGER, k, PX_STATE_PP, &memory) == (move ? r.moves[move].pp : 0));
                if (move) ++actual_used;
            }
            REQUIRE(used == actual_used && guest.fault == PX_FAULT_NONE);
            changed += memcmp(native, old, sizeof old) != 0;
            ++level_cases;
        }
    }
    px_garc_close(g); free(r.moves); px_guest_free(&guest);
    printf("{\"move_records\":%zu,\"maximum_pp_cases\":%" PRIu64 ",\"learnsets\":%zu,"
           "\"level_cases\":%" PRIu64 ",\"corrected_order_cases\":%" PRIu64 ",\"all_match\":true}\n",
           r.count, pp_cases, learnsets, level_cases, changed);
    return 0;
}
