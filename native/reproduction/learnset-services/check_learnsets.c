/* Optional local-data comparison. No original resources are embedded. */
#include "../../assets/garc.h"
#include "../../game/battle.h"
#include "../../platform/guest.h"
#include "../../../reconstructed/pokemon_learnset/pokemon_learnset.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(c) do { if (!(c)) { fprintf(stderr, "check_learnsets:%d: %s\n", __LINE__, #c); exit(1); } } while (0)
enum { RECORD = 0x100, CACHE = 0x300, TABLE = 0x400, BUFFER = 0x1000, BUFFER_CAP = 4096,
       SELECTED = 0x5d17b4, RESOURCE = 0x5d17e8 };
struct resources {
    struct px_guest guest;
    struct px_personal *personal; size_t personal_count;
    struct px_move *moves; size_t move_count;
    struct px_learnset *learnsets; uint8_t **raw; size_t *sizes, count;
    uint32_t personal_species;
    uint64_t loads;
};
static struct px_garc *open_archive(const char *romfs, const char *relative)
{
    char path[2048], error[256]; struct px_garc *g = NULL;
    CHECK(snprintf(path, sizeof path, "%s/%s", romfs, relative) < (int)sizeof path);
    if (!px_garc_open(path, &g, error, sizeof error)) { fprintf(stderr, "%s\n", error); exit(1); }
    return g;
}
static uint8_t *read_member(struct px_garc *g, size_t index, size_t *size)
{
    const struct px_garc_member *m = px_garc_member_at(g, index);
    char error[256]; uint8_t *data = NULL; CHECK(m);
    if (!px_garc_read(g, m->entry, m->subentry, 1u << 24, &data, size, error, sizeof error)) {
        fprintf(stderr, "%s\n", error); exit(1);
    }
    return data;
}
static uint32_t u16(const uint8_t *p) { return (uint32_t)p[0] | (uint32_t)p[1] << 8; }
static void select_personal(void *user, uint32_t record, uint32_t species, uint32_t form)
{
    struct resources *r = user; CHECK(record == 0x700 && species < r->personal_count && form == 0);
    r->personal_species = species;
}
static uint32_t personal_value(void *user, uint32_t record, uint32_t selector)
{
    const struct resources *r = user; CHECK(record == 0x700 && (selector == 0x1e || selector == 0x20));
    const struct px_personal *p = r->personal + r->personal_species;
    return selector == 0x1e ? p->form_index : p->form_count;
}
static void load_member(void *user, uint32_t archive, uint32_t index, uint32_t destination)
{
    struct resources *r = user; CHECK(archive == 0x900 && index < r->count && r->sizes[index] <= BUFFER_CAP);
    uint8_t *out = px_guest_span(&r->guest, destination, BUFFER_CAP); CHECK(out);
    memset(out, 0xff, BUFFER_CAP); memcpy(out, r->raw[index], r->sizes[index]); ++r->loads;
}
static uint64_t check_entries(struct resources *r, uint32_t index, const struct px_pokemon_api *memory)
{
    const uint32_t count = r->learnsets[index].count < 26 ? r->learnsets[index].count : 26;
    CHECK(px_current_learnset_count(memory) == count);
    uint64_t cases = 0;
    for (uint32_t i = 0; i <= count; ++i) {
        const uint32_t selected = i < count ? i : 0;
        CHECK(px_current_learnset_move(i, memory) == u16(r->raw[index] + 4 * selected));
        CHECK(px_current_learnset_level(i, memory) == u16(r->raw[index] + 4 * selected + 2));
        ++cases;
    }
    CHECK(px_current_learnset_move(UINT32_MAX, memory) == u16(r->raw[index]));
    return cases + 1;
}
int main(int argc, char **argv)
{
    if (argc != 2) { fprintf(stderr, "usage: check_learnsets ROMFS\n"); return 2; }
    struct resources r = {0};
    struct px_garc *g = open_archive(argv[1], "a/2/1/8");
    r.personal_count = px_garc_count(g) - 1;
    r.personal = calloc(r.personal_count, sizeof *r.personal); CHECK(r.personal);
    for (size_t i = 0; i < r.personal_count; ++i) {
        size_t size; uint8_t *d = read_member(g, i, &size);
        CHECK(px_personal_decode(d, size, r.personal + i) == PX_POKEDATA_OK); free(d);
    }
    px_garc_close(g); g = open_archive(argv[1], "a/2/1/2");
    r.move_count = px_garc_count(g); r.moves = calloc(r.move_count, sizeof *r.moves); CHECK(r.moves);
    for (size_t i = 0; i < r.move_count; ++i) {
        size_t size; uint8_t *d = read_member(g, i, &size);
        CHECK(px_move_decode(d, size, r.moves + i) == PX_POKEDATA_OK); free(d);
    }
    px_garc_close(g); g = open_archive(argv[1], "a/2/1/4");
    r.count = px_garc_count(g); r.learnsets = calloc(r.count, sizeof *r.learnsets);
    r.raw = calloc(r.count, sizeof *r.raw); r.sizes = calloc(r.count, sizeof *r.sizes);
    CHECK(r.learnsets && r.raw && r.sizes);
    uint32_t max_count = 0;
    for (size_t i = 0; i < r.count; ++i) {
        r.raw[i] = read_member(g, i, r.sizes + i);
        CHECK(px_learnset_decode(r.raw[i], r.sizes[i], r.learnsets + i) == PX_POKEDATA_OK);
        if (r.learnsets[i].count > max_count) max_count = r.learnsets[i].count;
    }
    px_garc_close(g);
    uint8_t control[0x700] = {0}, buffers[5][BUFFER_CAP] = {{0}}, globals[8] = {0}, resource[8] = {0};
    px_guest_init(&r.guest);
    CHECK(px_guest_map_view(&r.guest, RECORD, sizeof control, PX_GUEST_READ | PX_GUEST_WRITE, control, "records"));
    CHECK(px_guest_map_view(&r.guest, BUFFER, sizeof buffers, PX_GUEST_READ | PX_GUEST_WRITE, buffers, "buffers"));
    CHECK(px_guest_map_view(&r.guest, SELECTED, sizeof globals, PX_GUEST_READ | PX_GUEST_WRITE, globals, "selection"));
    CHECK(px_guest_map_view(&r.guest, RESOURCE, sizeof resource, PX_GUEST_READ | PX_GUEST_WRITE, resource, "resources"));
    px_guest_store32(&r.guest, SELECTED, RECORD); px_guest_store32(&r.guest, SELECTED + 4, CACHE);
    px_guest_store32(&r.guest, RESOURCE, 0x700); px_guest_store32(&r.guest, RESOURCE + 4, 0x900);
    px_guest_store32(&r.guest, RECORD + 4, BUFFER);
    px_guest_store32(&r.guest, CACHE + 4, TABLE); px_guest_store32(&r.guest, CACHE + 8, 4);
    for (unsigned i = 0; i < 4; ++i) {
        const uint32_t record = 0x500 + 12 * i;
        px_guest_store32(&r.guest, TABLE + 4 * i, record);
        px_guest_store16(&r.guest, record, UINT16_MAX); px_guest_store8(&r.guest, record + 2, 0xff);
        px_guest_store32(&r.guest, record + 4, BUFFER + (i + 1) * BUFFER_CAP);
    }
    const struct px_pokemon_api memory = px_guest_pokemon_api(&r.guest);
    const struct px_learnset_api api = {&memory, &r, select_personal, personal_value, load_member};
    const struct px_battle_data battle = {.moves = r.moves, .move_count = (uint32_t)r.move_count};
    uint64_t entry_cases = 0, level_cases = 0, selections = 0, hits = 0;
    for (uint32_t i = 0; i < r.count; ++i) {
        CHECK(r.sizes[i] <= BUFFER_CAP);
        memset(buffers[0], 0xff, BUFFER_CAP); memcpy(buffers[0], r.raw[i], r.sizes[i]);
        px_guest_store8(&r.guest, RECORD + 8, (uint8_t)(r.learnsets[i].count < 26 ? r.learnsets[i].count : 26));
        entry_cases += check_entries(&r, i, &memory);
        struct px_learnset prefix = r.learnsets[i]; if (prefix.count > 26) prefix.count = 26;
        for (unsigned level = 0; level <= 100; ++level) {
            uint16_t actual[4], expected[4];
            CHECK(px_battle_initial_moves(&battle, r.learnsets + i, level, actual));
            px_learnset_moves_at(&prefix, level, expected); CHECK(!memcmp(actual, expected, sizeof actual));
            ++level_cases;
        }
    }
    px_guest_store16(&r.guest, RECORD, UINT16_MAX); px_guest_store8(&r.guest, RECORD + 2, 0xff);
    for (uint32_t species = 0; species <= 721; ++species) {
        CHECK(species < r.personal_count);
        const struct px_personal *p = r.personal + species;
        const uint32_t forms = p->form_count ? p->form_count : 1;
        for (uint32_t form = 0; form < forms; ++form) {
            const uint32_t index = form && p->form_index ? p->form_index + form - 1 : species;
            CHECK(index < r.count);
            px_select_pokemon_learnset(species, form, &api);
            entry_cases += check_entries(&r, index, &memory); ++selections;
            const uint64_t before = r.loads;
            px_select_pokemon_learnset(species, form, &api); CHECK(r.loads == before); ++hits;
        }
    }
    CHECK(r.guest.fault == PX_FAULT_NONE);
    printf("{\"learnsets\":%zu,\"max_raw_entries\":%" PRIu32 ",\"entry_cases\":%" PRIu64 ","
           "\"level_cases\":%" PRIu64 ",\"species_form_selections\":%" PRIu64 ",\"repeat_cache_hits\":%" PRIu64
           ",\"resource_loads\":%" PRIu64 ",\"all_match\":true}\n",
           r.count, max_count, entry_cases, level_cases, selections, hits, r.loads);
    px_guest_free(&r.guest);
    for (size_t i = 0; i < r.count; ++i) free(r.raw[i]);
    free(r.raw); free(r.sizes); free(r.learnsets); free(r.personal); free(r.moves);
    return 0;
}
