/* Local-only original-data comparison. Generated records belong in outputs/. */
#include "../../assets/garc.h"
#include "../../assets/gf_text.h"
#include "../../game/save.h"
#include "../../platform/guest.h"
#include "../../../reconstructed/pokemon_names/pokemon_names.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(c) do { if (!(c)) { fprintf(stderr, "check_names:%d: %s\n", __LINE__, #c); exit(1); } } while (0)
enum { STRING = 0x100, BUFFER = 0x1000, DESTINATION = 0x2000, GLOBALS = 0x5d1790 };
struct resources { struct px_gf_text text; struct px_guest guest; uint32_t loads; };
static struct px_garc *open_archive(const char *romfs, const char *relative)
{
    char path[2048], error[256]; struct px_garc *g = NULL;
    CHECK(snprintf(path, sizeof path, "%s/%s", romfs, relative) < (int)sizeof path);
    if (!px_garc_open(path, &g, error, sizeof error)) { fprintf(stderr, "%s\n", error); exit(1); }
    return g;
}
static uint8_t *member(struct px_garc *g, uint32_t index, size_t *size)
{
    char error[256]; uint8_t *data = NULL;
    if (!px_garc_read(g, index, 0, 1u << 24, &data, size, error, sizeof error)) {
        fprintf(stderr, "%s\n", error); exit(1);
    }
    return data;
}
static uint32_t load_text(void *user, uint32_t resource, uint32_t index, uint32_t string)
{
    struct resources *r = user;
    CHECK(resource == 0x900 && string == STRING && index < r->text.line_count);
    const uint16_t *name = r->text.units + r->text.first[index];
    const uint32_t count = r->text.length[index];
    CHECK(count < 2048);
    const uint32_t buffer = px_guest_load32(&r->guest, string + 4);
    for (uint32_t i = 0; i < count; ++i) px_guest_store16(&r->guest, buffer + 2 * i, name[i]);
    ++r->loads;
    return count;
}
int main(int argc, char **argv)
{
    if (argc != 4) { fprintf(stderr, "usage: check_names ROMFS CODE_BIN OUTPUT_RECORDS\n"); return 2; }
    struct resources r = {0};
    struct px_garc *g = open_archive(argv[1], "a/0/7/4");
    size_t size; uint8_t *raw = member(g, 80, &size);
    CHECK(px_gf_text_decode(raw, size, &r.text) == PX_GF_TEXT_OK); free(raw); px_garc_close(g);
    CHECK(r.text.line_count >= 724);
    uint8_t string[16] = {0}, globals[8] = {0}, buffer[4096] = {0}, destination[4096] = {0};
    px_guest_init(&r.guest);
    CHECK(px_guest_map_view(&r.guest, STRING, sizeof string, PX_GUEST_READ | PX_GUEST_WRITE, string, "string"));
    CHECK(px_guest_map_view(&r.guest, GLOBALS, sizeof globals, PX_GUEST_READ | PX_GUEST_WRITE, globals, "globals"));
    CHECK(px_guest_map_view(&r.guest, BUFFER, sizeof buffer, PX_GUEST_READ | PX_GUEST_WRITE, buffer, "resource"));
    CHECK(px_guest_map_view(&r.guest, DESTINATION, sizeof destination, PX_GUEST_READ | PX_GUEST_WRITE, destination, "name"));
    px_guest_store32(&r.guest, STRING + 4, BUFFER);
    px_guest_store32(&r.guest, GLOBALS, STRING); px_guest_store32(&r.guest, GLOBALS + 4, 0x900);
    const struct px_pokemon_api memory = px_guest_pokemon_api(&r.guest);
    const struct px_name_api names = {&memory, &r, load_text};
    uint32_t builder_cases = 0, max_units = 0;
    for (uint32_t species = 0; species < 724; ++species) {
        const uint32_t index = species <= 721 ? species : 0;
        memset(destination, 0xcc, sizeof destination);
        CHECK(px_build_pokemon_name(DESTINATION, species, &names) == r.text.length[index]);
        const uint16_t *name = r.text.units + r.text.first[index];
        uint32_t i = 0;
        do { CHECK(i < r.text.length[index]); CHECK(px_guest_load16(&r.guest, DESTINATION + 2 * i) == name[i]); } while (name[i++] != 0);
        CHECK(px_guest_load16(&r.guest, DESTINATION + 2 * i) == 0xcccc);
        ++builder_cases;
    }
    uint32_t plain_lines = 0, rejected_lines = 0;
    for (uint32_t index = 0; index < r.text.line_count; ++index) {
        const uint16_t *name = r.text.units + r.text.first[index];
        const uint32_t count = r.text.length[index];
        uint32_t length = 0;
        while (length < count && name[length] != 0 && name[length] != 0x10) ++length;
        const int valid = length < count && length <= 12 && name[length] == 0;
        struct px_battler b = {0};
        CHECK(px_battler_set_name(&b, name, count) == valid);
        if (valid) ++plain_lines;
        else { ++rejected_lines; CHECK(b.name[0] == 0); }
    }
    struct px_personal *personal;
    g = open_archive(argv[1], "a/2/1/8");
    const uint32_t personal_count = (uint32_t)px_garc_count(g) - 1;
    personal = calloc(personal_count, sizeof *personal); CHECK(personal && personal_count > 721);
    for (uint32_t i = 0; i < personal_count; ++i) {
        raw = member(g, i, &size); CHECK(px_personal_decode(raw, size, personal + i) == PX_POKEDATA_OK); free(raw);
    }
    px_garc_close(g);
    uint8_t order[128]; FILE *code = fopen(argv[2], "rb"); CHECK(code);
    CHECK(fseek(code, 0x470eec, SEEK_SET) == 0 && fread(order, 1, sizeof order, code) == sizeof order);
    CHECK(fclose(code) == 0);
    FILE *records = fopen(argv[3], "wbx"); CHECK(records);
    const struct px_move empty_move = {0};
    const struct px_battle_data data = {.personal = personal, .personal_count = personal_count, .moves = &empty_move, .move_count = 1};
    const uint16_t moves[4] = {0}; const uint8_t iv[6] = {0};
    uint32_t saved = 0, permutations = 0;
    for (uint32_t species = 1; species <= 721; ++species) {
        struct px_save save = {0}; save.party_count = 1;
        CHECK(px_battler_make(&data, (uint16_t)species, 5, iv, 0, moves, save.party));
        const uint16_t *name = r.text.units + r.text.first[species];
        CHECK(px_battler_set_name(save.party, name, r.text.length[species]));
        unsigned length = 0; while (save.party[0].name[length]) ++length;
        if (length > max_units) max_units = length;
        const uint32_t row = species % 32;
        permutations |= UINT32_C(1) << row;
        save.party[0].key = row << 13 | species;
        uint8_t file[PX_SAVE_SIZE], again[PX_SAVE_SIZE]; char error[256];
        CHECK(px_save_encode(&save, &data, order, file, error, sizeof error));
        struct px_save back;
        CHECK(px_save_decode(file, sizeof file, &data, order, &back, error, sizeof error));
        CHECK(!memcmp(back.party, save.party, sizeof save.party));
        CHECK(px_save_encode(&back, &data, order, again, error, sizeof error) && !memcmp(again, file, sizeof file));
        CHECK(fwrite(file + PX_SAVE_HEADER, 1, PX_SAVE_SLOT, records) == PX_SAVE_SLOT); ++saved;
    }
    CHECK(r.guest.fault == PX_FAULT_NONE && permutations == UINT32_MAX);
    CHECK(fclose(records) == 0);
    printf("{\"text_lines\":%" PRIu32 ",\"builder_cases\":%" PRIu32 ",\"resource_calls\":%" PRIu32
           ",\"saved_names\":%" PRIu32 ",\"max_name_units\":%" PRIu32 ",\"plain_text_lines\":%" PRIu32
           ",\"rejected_placeholder_lines\":%" PRIu32 ",\"block_selector_rows\":32,\"all_match\":true}\n",
           r.text.line_count, builder_cases, r.loads, saved, max_units, plain_lines, rejected_lines);
    px_gf_text_free(&r.text); px_guest_free(&r.guest); free(personal);
    return 0;
}
