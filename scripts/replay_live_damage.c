/* Replay traced DllBattle damage calculations through the reconstruction.
 * Input (written by scripts/replay_live_damage.py), per traced call:
 *   D ctx attacker defender move eff spread crit fixed extra output
 *     attacker_id defender_id move_hex16 rules_object rules_word field weather
 *     random_owner generator result damage call_count
 *   C target r0 r1 r2 r3 s0 s1 result flag      (call_count lines)
 * Every reconstructed callback must match the next recorded call's callee and
 * arguments and receives its recorded result. Calls to the divide helper are
 * represented by C division and are skipped. */
#include "../reconstructed/battle_damage/battle_damage.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"replay_damage:%d: %s\n",__LINE__,#c);exit(2); } } while (0)
enum { MAX_CALLS = 256, CELLS = 64, FLAG = 0x100 };
struct call { uint32_t target, r[4], s[2], result, flag; };
static struct call calls[MAX_CALLS];
static unsigned call_count, next_call, mismatches;
static uint32_t cell_address[CELLS];
static uint8_t cell_value[CELLS];
static unsigned cells;
static const char *where;
static void set_byte(uint32_t a, uint8_t v)
{
    for (unsigned i = 0; i < cells; ++i) if (cell_address[i] == a) { cell_value[i] = v; return; }
    CHECK(cells < CELLS); cell_address[cells] = a; cell_value[cells++] = v;
}
static void set_word(uint32_t a, uint32_t v) { for (unsigned i = 0; i < 4; ++i) set_byte(a + i, (uint8_t)(v >> (8 * i))); }
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

/* Next recorded call, skipping the divide veneer; check callee and arguments. */
static const struct call *expect(uint32_t target, unsigned argc, const uint32_t *argv)
{
    while (next_call < call_count && calls[next_call].target == 0x1080041c) ++next_call;
    if (next_call >= call_count) { fprintf(stderr, "%s: extra call to %08x\n", where, target); ++mismatches; static struct call none; return &none; }
    const struct call *c = &calls[next_call++];
    if (c->target != target) { fprintf(stderr, "%s: call %u is %08x, reconstruction called %08x\n", where, next_call - 1, c->target, target); ++mismatches; }
    for (unsigned i = 0; i < argc; ++i) {
        const uint32_t recorded = i < 4 ? c->r[i] : c->s[i - 4];
        if (recorded != argv[i]) { fprintf(stderr, "%s: call %u to %08x argument %u is %08x, reconstruction passed %08x\n", where, next_call - 1, target, i, recorded, argv[i]); ++mismatches; }
    }
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
static uint32_t attack(void *u, uint32_t ctx, uint32_t at, uint32_t df, uint32_t mv, uint32_t crit, uint32_t flag)
{
    (void)u; (void)flag;
    const struct call *c = CALL(0x108202d8, ctx, at, df, mv, crit);
    set_byte(FLAG, (uint8_t)c->flag); return c->result;
}
static uint32_t defense(void *u, uint32_t ctx, uint32_t at, uint32_t df, uint32_t mv, uint32_t crit) { (void)u; return CALL(0x10822330, ctx, at, df, mv, crit)->result; }
static uint32_t pokemon_value(void *u, uint32_t p, uint32_t k) { (void)u; return CALL(0x10874420, p, k)->result; }
static uint32_t has_type(void *u, uint32_t p, uint32_t t) { (void)u; return CALL(0x1086f960, p, t)->result; }
static uint32_t status(void *u, uint32_t p) { (void)u; return CALL(0x1086d8ac, p)->result; }
static uint32_t effectiveness(void *u, uint32_t d, uint32_t e) { (void)u; return CALL(0x1089e7f0, d, e)->result; }
static uint32_t category(void *u, uint32_t m) { (void)u; return CALL(0x108007dc, m)->result; }
static uint32_t power(void *u, uint32_t m) { (void)u; return CALL(0x10801634, m)->result; }
static uint32_t rule(void *u, uint32_t w, uint32_t f) { (void)u; return CALL(0x108007e4, w, f)->result; }
static uint32_t random_value(void *u, uint32_t g, uint32_t r) { (void)u; return CALL(0x108003dc, g, r)->result; }

int main(int argc, char **argv)
{
    if (argc != 2) { fprintf(stderr, "usage: replay_live_damage CALLS.txt\n"); return 2; }
    FILE *in = fopen(argv[1], "r"); CHECK(in);
    static char line[1024]; unsigned done = 0, matched = 0;
    while (fgets(line, sizeof line, in)) {
        if (line[0] != 'D') continue;
        struct px_battle_damage_args a; uint32_t attacker_id, defender_id, rules_object, rules_word, field, weather, owner, generator, result, damage;
        char move_hex[32];
        CHECK(sscanf(line + 2, "%" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32
                     " %" SCNu32 " %" SCNu32 " %31s %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %u",
                     &a.context, &a.attacker, &a.defender, &a.move, &a.effectiveness, &a.spread, &a.critical, &a.fixed_roll, &a.extra, &a.output,
                     &attacker_id, &defender_id, move_hex, &rules_object, &rules_word, &field, &weather, &owner, &generator, &result, &damage,
                     &call_count) == 22);
        CHECK(call_count <= MAX_CALLS);
        for (unsigned i = 0; i < call_count; ++i) {
            CHECK(fgets(line, sizeof line, in) && line[0] == 'C');
            struct call *c = &calls[i];
            CHECK(sscanf(line + 2, "%" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32,
                         &c->target, &c->r[0], &c->r[1], &c->r[2], &c->r[3], &c->s[0], &c->s[1], &c->result, &c->flag) == 9);
        }
        cells = 0; next_call = 0; mismatches = 0;
        set_byte(a.attacker + 0x19, (uint8_t)attacker_id); set_byte(a.defender + 0x19, (uint8_t)defender_id);
        for (unsigned i = 0; i < 8; ++i) { unsigned v; CHECK(sscanf(move_hex + 2 * i, "%2x", &v) == 1); set_byte(a.move + i, (uint8_t)v); }
        set_word(a.context + 4, rules_object); set_word(rules_object + 0x10, rules_word);
        set_word(a.context + 0x1c, field); set_byte(field + 1, (uint8_t)weather);
        set_word(owner + 0x54, generator);
        char label[64]; snprintf(label, sizeof label, "damage call %u", done); where = label;
        const struct px_battle_damage_api api = {&memory, NULL, push, pop, set_id, set_value, set_scaled, set_multiplier, dispatch, get,
                                                 attack, defense, pokemon_value, has_type, status, effectiveness,
                                                 category, power, rule, random_value, owner, FLAG};
        const uint32_t got = px_battle_calculate_damage(&a, &api);
        while (next_call < call_count && calls[next_call].target == 0x1080041c) ++next_call;
        if (next_call != call_count) { fprintf(stderr, "%s: %u recorded calls not made\n", label, call_count - next_call); ++mismatches; }
        const uint32_t out = r16(NULL, a.output);
        if (got != result || out != damage) { fprintf(stderr, "%s: result %u/%u damage %u/%u\n", label, got, result, out, damage); ++mismatches; }
        ++done; matched += mismatches == 0;
        printf("damage call %u: move %u damage %u (game %u)%s%c", done - 1, (unsigned)r16(NULL, a.move), out, damage, mismatches ? "  MISMATCH" : "", 10);
    }
    fclose(in);
    printf("{\"damage_calls\":%u,\"matched\":%u}\n", done, matched);
    return matched == done ? 0 : 1;
}
