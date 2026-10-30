/* Replay traced DllBattle attack/defense calls through the reconstruction.
 * Input (written by scripts/replay_live_stats.py), per traced call:
 *   F kind ctx attacker defender move critical flag result exit_flag read_count call_count
 *   M address hex                     (read_count lines; memory the function reads)
 *   C target r0 r1 r2 r3 s0 s1 result (call_count lines)
 * kind 0 is attack (108202d8), 1 is defense (10822330). Each callback must
 * match the next recorded callee and arguments and returns its recorded result. */
#include "../reconstructed/battle_stats/battle_stats.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"replay_stats:%d: %s\n",__LINE__,#c);exit(2); } } while (0)
enum { MAX_CALLS = 128, CELLS = 512 };
struct call { uint32_t target, r[4], s[2], result; };
static struct call calls[MAX_CALLS];
static unsigned call_count, next_call, mismatches, cells;
static uint32_t cell_address[CELLS];
static uint8_t cell_value[CELLS];
static const char *where;
static void set_byte(uint32_t a, uint8_t v)
{
    for (unsigned i = 0; i < cells; ++i) if (cell_address[i] == a) { cell_value[i] = v; return; }
    CHECK(cells < CELLS); cell_address[cells] = a; cell_value[cells++] = v;
}
static uint8_t r8(void *u, uint32_t a)
{
    (void)u;
    for (unsigned i = 0; i < cells; ++i) if (cell_address[i] == a) return cell_value[i];
    fprintf(stderr, "%s: unrecorded read at %08x\n", where, a); ++mismatches; return 0;
}
static uint16_t r16(void *u, uint32_t a) { return (uint16_t)(r8(u, a) | r8(u, a + 1) << 8); }
static uint32_t r32(void *u, uint32_t a) { return (uint32_t)r16(u, a) | (uint32_t)r16(u, a + 2) << 16; }
static void w8(void *u, uint32_t a, uint8_t v) { (void)u; set_byte(a, v); }
static void w16(void *u, uint32_t a, uint16_t v) { w8(u, a, (uint8_t)v); w8(u, a + 1, (uint8_t)(v >> 8)); }
static void w32(void *u, uint32_t a, uint32_t v) { w16(u, a, (uint16_t)v); w16(u, a + 2, (uint16_t)(v >> 16)); }
static const struct px_pokemon_api memory = {.load8 = r8, .load16 = r16, .load32 = r32, .store8 = w8, .store16 = w16, .store32 = w32};
static const struct call *expect(uint32_t target, unsigned argc, const uint32_t *argv)
{
    if (next_call >= call_count) { fprintf(stderr, "%s: extra call to %08x\n", where, target); ++mismatches; static struct call none; return &none; }
    const struct call *c = &calls[next_call++];
    if (c->target != target) { fprintf(stderr, "%s: call %u is %08x, reconstruction called %08x\n", where, next_call - 1, c->target, target); ++mismatches; }
    for (unsigned i = 0; i < argc; ++i)
        if (c->r[i] != argv[i]) { fprintf(stderr, "%s: call %u to %08x argument %u is %08x, reconstruction passed %08x\n", where, next_call - 1, target, i, c->r[i], argv[i]); ++mismatches; }
    return c;
}
#define CALL(target, ...) (expect(target, sizeof((uint32_t[]){__VA_ARGS__}) / 4, (uint32_t[]){__VA_ARGS__}))
static void push(void *u) { (void)u; expect(0x1086bb84, 0, NULL); }
static void pop(void *u) { (void)u; expect(0x1086b880, 0, NULL); }
static void set_id(void *u, uint32_t k, uint32_t v) { (void)u; CALL(0x1086bac0, k, v); }
static void set_value(void *u, uint32_t k, uint32_t v) { (void)u; CALL(0x1086b9fc, k, v); }
static void set_scaled(void *u, uint32_t k, uint32_t v) { (void)u; CALL(0x1086b520, k, v); }
static void set_multiplier(void *u, uint32_t k, uint32_t v, uint32_t lo, uint32_t hi) { (void)u; CALL(0x10867c9c, k, v, lo, hi); }
static void dispatch(void *u, uint32_t ctx, uint32_t event) { (void)u; CALL(0x1086b9a4, ctx, event); }
static uint32_t get(void *u, uint32_t k) { (void)u; return CALL(0x1086b91c, k)->result; }
static uint32_t stat(void *u, uint32_t p, uint32_t k) { (void)u; return CALL(0x10874420, p, k)->result; }
static uint32_t raw_stat(void *u, uint32_t p, uint32_t k) { (void)u; return CALL(0x108743a8, p, k)->result; }
static uint32_t stage_key(void *u, uint32_t p, uint32_t k) { (void)u; return CALL(0x108620d4, p, k)->result; }
static uint32_t has_type(void *u, uint32_t p, uint32_t t) { (void)u; return CALL(0x1086f960, p, t)->result; }
static uint32_t category(void *u, uint32_t m) { (void)u; return CALL(0x108007dc, m)->result; }
static void abort_battle(void *u) { (void)u; CALL(0x10800394, 0, 0, 0); }
int main(int argc, char **argv)
{
    if (argc != 2) { fprintf(stderr, "usage: replay_live_stats CALLS.txt\n"); return 2; }
    FILE *in = fopen(argv[1], "r"); CHECK(in);
    static char line[4096]; unsigned done[2] = {0}, matched[2] = {0};
    const struct px_battle_stats_api api = {&memory, NULL, push, pop, set_id, set_value, set_scaled, set_multiplier, dispatch, get,
                                            stat, raw_stat, stage_key, has_type, category, abort_battle};
    while (fgets(line, sizeof line, in)) {
        if (line[0] != 'F') continue;
        uint32_t kind, ctx, attacker, defender, move, critical, flag, result, exit_flag; unsigned reads;
        CHECK(sscanf(line + 2, "%" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %u %u",
                     &kind, &ctx, &attacker, &defender, &move, &critical, &flag, &result, &exit_flag, &reads, &call_count) == 11);
        CHECK(kind < 2 && call_count <= MAX_CALLS);
        cells = 0; next_call = 0; mismatches = 0;
        for (unsigned i = 0; i < reads; ++i) {
            uint32_t address; static char hex[1024];
            CHECK(fgets(line, sizeof line, in) && line[0] == 'M' && sscanf(line + 2, "%" SCNu32 " %1023s", &address, hex) == 2);
            for (size_t k = 0; 2 * k < strlen(hex); ++k) { unsigned v; CHECK(sscanf(hex + 2 * k, "%2x", &v) == 1); set_byte(address + (uint32_t)k, (uint8_t)v); }
        }
        for (unsigned i = 0; i < call_count; ++i) {
            struct call *c = &calls[i];
            CHECK(fgets(line, sizeof line, in) && line[0] == 'C' &&
                  sscanf(line + 2, "%" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32,
                         &c->target, &c->r[0], &c->r[1], &c->r[2], &c->r[3], &c->s[0], &c->s[1], &c->result) == 8);
        }
        char label[64]; snprintf(label, sizeof label, "%s call %u", kind ? "defense" : "attack", done[kind]); where = label;
        const uint32_t got = kind ? px_battle_defense_value(ctx, attacker, defender, move, critical, &api)
                                  : px_battle_attack_value(ctx, attacker, defender, move, critical, flag, &api);
        if (next_call != call_count) { fprintf(stderr, "%s: %u recorded calls not made\n", label, call_count - next_call); ++mismatches; }
        if (got != result) { fprintf(stderr, "%s: value %u, game %u\n", label, got, result); ++mismatches; }
        if (kind == 0 && r8(NULL, flag) != exit_flag) { fprintf(stderr, "%s: flag %u, game %u\n", label, r8(NULL, flag), exit_flag); ++mismatches; }
        printf("%s: move %u value %u (game %u)%s\n", label, (unsigned)r16(NULL, move), got, result, mismatches ? "  MISMATCH" : "");
        ++done[kind]; matched[kind] += mismatches == 0;
    }
    fclose(in);
    printf("{\"attack\":[%u,%u],\"defense\":[%u,%u]}\n", done[0], matched[0], done[1], matched[1]);
    return matched[0] == done[0] && matched[1] == done[1] ? 0 : 1;
}
