#include "gf_script.h"
#include "zone_talk.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Hand-assembled Pawn 3.3 images in the GF encoding. Opcode numbers follow
 * the Pawn 3.3 enumeration (see gf_script.h). */
enum { HALT_P = 0xD2, PROC = 0x2E, RETN = 0x30, CALL = 0x31, JUMP = 0x33, JZER = 0x35, LOAD_P_PRI = 0xA2,
       SWITCH = 0x81, CASETBL = 0x82, PUSH_P_C = 0xBC, PUSH_P_S = 0xBE, SYSREQ_N = 0x87, ZERO_PRI = 0x59,
       CONST_P_PRI = 0xAB, CONST_P_ALT = 0xAC, ADD = 0x4E, STOR_P_PRI = 0xAF, EQ_P_C_PRI = 0xC9, HALT = 0x78,
       SDIV = 0x49, LOAD_P_S_PRI = 0xA4 };
static uint32_t packed(unsigned op, int32_t v) { return (uint32_t)op | (uint32_t)v << 16; }

struct image { uint8_t bytes[4096]; size_t n; };
static void put32(uint8_t *p, uint32_t v) { for (unsigned i = 0; i < 4; ++i) p[i] = (uint8_t)(v >> (8 * i)); }
/* GF word encoding: 7-bit groups, most significant first, continuation in
 * bit 7; the first group's bit 6 carries the sign. Shortest form. */
static void encode(struct image *im, uint32_t w)
{
    const int32_t v = (int32_t)w;
    unsigned n = 1;
    while (n < 5) {
        const int64_t lo = -((int64_t)1 << (7 * n - 1)), hi = ((int64_t)1 << (7 * n - 1)) - 1;
        if (v >= lo && v <= hi) break;
        ++n;
    }
    for (unsigned i = n; i-- > 0;) {
        uint8_t b = (uint8_t)(((uint32_t)v >> (7 * i)) & 0x7F);
        if (i) b |= 0x80;
        im->bytes[im->n++] = b;
    }
}
/* Header with no publics, `natives` hashes, one pubvar (0xD7477C97 at data
 * address 0), code and data words, 256 bytes of stack. */
static void build(struct image *im, const uint32_t *code, unsigned code_words, const uint32_t *data,
                  unsigned data_words, const uint32_t *natives, unsigned native_count)
{
    memset(im, 0, sizeof *im);
    const uint32_t publics = 0x3c, nat = publics, libraries = nat + 8 * native_count, pubvars = libraries,
                   tags = pubvars + 8, nametable = tags, cod = nametable + 4;
    const uint32_t dat = cod + 4 * code_words, hea = dat + 4 * data_words, stp = hea + 256;
    put32(im->bytes + 4, 0x0A0AF1E0); put32(im->bytes + 8, 0x0008001C);
    const uint32_t fields[] = {cod, dat, hea, stp, 4, publics, nat, libraries, pubvars, tags, nametable, nametable};
    for (unsigned i = 0; i < 12; ++i) put32(im->bytes + 12 + 4 * i, fields[i]);
    for (unsigned i = 0; i < native_count; ++i) put32(im->bytes + nat + 8 * i + 4, natives[i]);
    put32(im->bytes + pubvars, 0); put32(im->bytes + pubvars + 4, 0xD7477C97u);
    im->n = cod;
    for (unsigned i = 0; i < code_words; ++i) encode(im, code[i]);
    for (unsigned i = 0; i < data_words; ++i) encode(im, data[i]);
    put32(im->bytes, (uint32_t)im->n);
}

struct calls { uint32_t hash[64]; int32_t arg[64]; unsigned n; int32_t answer; };
static int32_t record(void *ctx, uint32_t hash, const int32_t *args, uint32_t count, int *failed)
{
    struct calls *c = ctx;
    (void)failed;
    if (c->n < 64) { c->hash[c->n] = hash; c->arg[c->n] = count ? args[0] : -999; ++c->n; }
    return c->answer;
}

/* main: switch on the pubvar (data 0): case 1 calls f(7) which shows line
 * 2 + its argument when native 1 says so, else line 3; case 2 yields twice
 * then divides; default returns. f uses a frame argument. */
static const uint32_t natives[] = {0x9ADF1616u, 0x59008363u};
static void program(struct image *im)
{
    uint32_t code[64];
    unsigned n = 0;
    code[n++] = packed(HALT_P, 0);                 /* 0x00 */
    code[n++] = PROC;                              /* 0x04 main */
    code[n++] = packed(LOAD_P_PRI, 0);             /* 0x08 */
    code[n++] = SWITCH; code[n++] = 0;             /* 0x0c -> table, patched */
    const unsigned switch_at = n - 2;
    /* case 1 at 0x14 */
    code[n++] = packed(PUSH_P_C, 7);               /* 0x14 */
    code[n++] = packed(PUSH_P_C, 4);               /* 0x18 arg bytes */
    code[n++] = CALL; code[n++] = 0;               /* 0x1c -> f, patched */
    const unsigned call_at = n - 2;
    code[n++] = JUMP; code[n++] = 0;               /* 0x24 -> end, patched */
    const unsigned jump1 = n - 2;
    /* case 2 at 0x2c */
    const unsigned case2 = n;
    code[n++] = HALT; code[n++] = 12;              /* yield */
    code[n++] = packed(HALT_P, 12);                /* yield */
    code[n++] = packed(CONST_P_PRI, -7);
    code[n++] = packed(CONST_P_ALT, 2);
    code[n++] = SDIV;                              /* floored: -4, remainder 1 */
    code[n++] = packed(STOR_P_PRI, 4);
    code[n++] = JUMP; code[n++] = 0;
    const unsigned jump2 = n - 2;
    /* table */
    const unsigned table = n;
    code[n++] = CASETBL; code[n++] = 2; code[n++] = 0; /* default patched */
    code[n++] = 1; code[n++] = 0;
    code[n++] = 2; code[n++] = 0;
    /* end */
    const unsigned end = n;
    code[n++] = ZERO_PRI;
    code[n++] = RETN;
    /* f(x) */
    const unsigned f = n;
    code[n++] = PROC;
    code[n++] = SYSREQ_N; code[n++] = 1; code[n++] = 0;  /* native 1, no args */
    code[n++] = JZER; code[n++] = 0;
    const unsigned jz = n - 2;
    code[n++] = packed(LOAD_P_S_PRI, 12);
    code[n++] = packed(CONST_P_ALT, 2);
    code[n++] = ADD;
    code[n++] = JUMP; code[n++] = 0;
    const unsigned jshow = n - 2;
    const unsigned other = n;
    code[n++] = packed(CONST_P_PRI, 3);
    const unsigned show = n;
    code[n++] = 0x24;                              /* PUSH_PRI */
    code[n++] = SYSREQ_N; code[n++] = 0; code[n++] = 4;  /* message(line) */
    code[n++] = ZERO_PRI;
    code[n++] = RETN;
#define REL(at, target) (code[(at) + 1] = (uint32_t)(4 * ((int32_t)(target) - (int32_t)(at))))
    REL(switch_at, table);
    REL(call_at, f);
    REL(jump1, end);
    REL(jump2, end);
    REL(jz, other);
    REL(jshow, show);
    /* Case targets are relative to their own word, plus one cell. */
    code[table + 2] = (uint32_t)(4 * ((int32_t)end - (int32_t)(table + 2)) + 4);
    code[table + 4] = (uint32_t)(4 * ((int32_t)(switch_at + 2) - (int32_t)(table + 4)) + 4);
    code[table + 6] = (uint32_t)(4 * ((int32_t)case2 - (int32_t)(table + 6)) + 4);
    const uint32_t data[2] = {0, 0x12345678};
    build(im, code, n, data, 2, natives, 2);
}

int main(void)
{
    struct image im;
    program(&im);
    struct px_gf_script s;
    assert(px_gf_script_decode(im.bytes, im.n, &s) == PX_GF_SCRIPT_OK);
    assert(s.native_count == 2 && s.natives[1].hash == 0x59008363u && s.pubvar_count == 1 && s.cip == 4);
    assert(s.data_words == 2 && s.data_init[1] == 0x12345678u && s.code[0] == (uint32_t)HALT_P);
    struct calls c = {{0}, {0}, 0, 1};
    struct px_gf_run run;
    px_gf_script_run(&s, 0xD7477C97u, 1, record, &c, 1000, 10, &run);
    assert(run.result == PX_GF_SCRIPT_HALTED && run.yields == 0);
    assert(c.n == 2 && c.hash[0] == 0x59008363u && c.hash[1] == 0x9ADF1616u && c.arg[1] == 9);
    memset(&c, 0, sizeof c);                       /* native 1 answers 0: line 3 */
    px_gf_script_run(&s, 0xD7477C97u, 1, record, &c, 1000, 10, &run);
    assert(run.result == PX_GF_SCRIPT_HALTED && c.n == 2 && c.arg[1] == 3);
    memset(&c, 0, sizeof c);                       /* case 2: two yields, no natives */
    px_gf_script_run(&s, 0xD7477C97u, 2, record, &c, 1000, 10, &run);
    assert(run.result == PX_GF_SCRIPT_HALTED && run.yields == 2 && c.n == 0);
    px_gf_script_run(&s, 0xD7477C97u, 2, record, &c, 1000, 1, &run);
    assert(run.result == PX_GF_SCRIPT_YIELD_LIMIT);
    px_gf_script_run(&s, 0xD7477C97u, 5, record, &c, 1000, 10, &run); /* default case */
    assert(run.result == PX_GF_SCRIPT_HALTED);
    px_gf_script_run(&s, 0xD7477C97u, 1, record, &c, 3, 10, &run);
    assert(run.result == PX_GF_SCRIPT_BUDGET && run.steps == 3);
    /* The talk policy: female answers 1 (line 9), male 0 (line 3). */
    struct px_zone_talk talk;
    px_zone_talk(&s, 1, 1, &talk);
    assert(talk.run.result == PX_GF_SCRIPT_HALTED && talk.line_count == 1 && talk.lines[0] == 9);
    px_zone_talk(&s, 1, 0, &talk);
    assert(talk.line_count == 1 && talk.lines[0] == 3 && talk.native_calls == 2);
    px_gf_script_free(&s);
    /* Faults: a store outside the data segment, a native index out of range. */
    {
        const uint32_t code[] = {packed(HALT_P, 0), PROC, packed(CONST_P_PRI, 1), packed(STOR_P_PRI, 30000),
                                 ZERO_PRI, RETN};
        build(&im, code, 6, NULL, 0, natives, 2);
        assert(px_gf_script_decode(im.bytes, im.n, &s) == PX_GF_SCRIPT_OK);
        px_gf_script_run(&s, 0, 0, record, &c, 1000, 10, &run);
        assert(run.result == PX_GF_SCRIPT_MEMORY_FAULT);
        px_gf_script_free(&s);
        const uint32_t bad[] = {packed(HALT_P, 0), PROC, SYSREQ_N, 9, 0, ZERO_PRI, RETN};
        build(&im, bad, 7, NULL, 0, natives, 2);
        assert(px_gf_script_decode(im.bytes, im.n, &s) == PX_GF_SCRIPT_OK);
        px_gf_script_run(&s, 0, 0, record, &c, 1000, 10, &run);
        assert(run.result == PX_GF_SCRIPT_BAD_INSTRUCTION);
        px_gf_script_free(&s);
        const uint32_t unknown[] = {packed(HALT_P, 0), PROC, 0xFF, RETN};
        build(&im, unknown, 4, NULL, 0, natives, 2);
        assert(px_gf_script_decode(im.bytes, im.n, &s) == PX_GF_SCRIPT_OK);
        px_gf_script_run(&s, 0, 0, record, &c, 1000, 10, &run);
        assert(run.result == PX_GF_SCRIPT_BAD_INSTRUCTION && run.last_cip == 8);
        px_gf_script_free(&s);
    }
    /* Decoding rejections. */
    program(&im);
    uint8_t copy[4096];
    assert(px_gf_script_decode(NULL, im.n, &s) == PX_GF_SCRIPT_INVALID_ARGUMENT);
    assert(px_gf_script_decode(im.bytes, 0x3b, &s) == PX_GF_SCRIPT_TRUNCATED);
    memcpy(copy, im.bytes, im.n); copy[4] = 0;     /* magic */
    assert(px_gf_script_decode(copy, im.n, &s) == PX_GF_SCRIPT_INVALID);
    memcpy(copy, im.bytes, im.n); copy[6] = 9;     /* file version */
    assert(px_gf_script_decode(copy, im.n, &s) == PX_GF_SCRIPT_INVALID);
    memcpy(copy, im.bytes, im.n); put32(copy, (uint32_t)im.n - 3); /* code cut short */
    assert(px_gf_script_decode(copy, im.n, &s) == PX_GF_SCRIPT_TRUNCATED && s.code == NULL);
    memcpy(copy, im.bytes, im.n); put32(copy + 16, 1u << 22); /* dat beyond hea */
    assert(px_gf_script_decode(copy, im.n, &s) == PX_GF_SCRIPT_INVALID);
    memcpy(copy, im.bytes, im.n); put32(copy + 28, 2); /* misaligned cip */
    assert(px_gf_script_decode(copy, im.n, &s) == PX_GF_SCRIPT_INVALID);
    assert(!strcmp(px_gf_script_result_string(PX_GF_SCRIPT_YIELD_LIMIT), "yield limit"));
    puts("GF script tests passed");
    return 0;
}
