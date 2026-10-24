#include "gf_script.h"

#include <stdlib.h>
#include <string.h>

enum { MAX_INPUT = 1u << 20, MAX_DATA = 1u << 20, MAX_WORDS = 256u << 10, MAX_ARGS = 256 };

static uint16_t u16(const uint8_t *p) { return (uint16_t)(p[0] | p[1] << 8); }
static uint32_t u32(const uint8_t *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

static enum px_gf_script_result table(const uint8_t *data, uint32_t start, uint32_t end,
                                      struct px_gf_script_entry **out, uint32_t *count)
{
    *count = (end - start) / 8;
    *out = calloc(*count ? *count : 1, sizeof **out);
    if (!*out) return PX_GF_SCRIPT_NO_MEMORY;
    for (uint32_t i = 0; i < *count; ++i) {
        (*out)[i].address = u32(data + start + 8 * (size_t)i);
        (*out)[i].hash = u32(data + start + 8 * (size_t)i + 4);
    }
    return PX_GF_SCRIPT_OK;
}

enum px_gf_script_result px_gf_script_decode(const uint8_t *data, size_t size, struct px_gf_script *out)
{
    if (!out) return PX_GF_SCRIPT_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    if (!data) return PX_GF_SCRIPT_INVALID_ARGUMENT;
    if (size > MAX_INPUT) return PX_GF_SCRIPT_INVALID;
    if (size < 0x3c) return PX_GF_SCRIPT_TRUNCATED;
    const uint32_t declared = u32(data);
    if (u16(data + 4) != 0xF1E0 || data[6] != 10 || data[7] != 10 || u16(data + 10) != 8) return PX_GF_SCRIPT_INVALID;
    if (declared < 0x3c || declared > size) return PX_GF_SCRIPT_TRUNCATED;
    const uint32_t cod = u32(data + 12), dat = u32(data + 16), hea = u32(data + 20), stp = u32(data + 24),
                   cip = u32(data + 28), publics = u32(data + 32), natives = u32(data + 36),
                   libraries = u32(data + 40), pubvars = u32(data + 44), tags = u32(data + 48),
                   nametable = u32(data + 52);
    if (publics < 0x3c || publics > natives || natives > libraries || libraries > pubvars || pubvars > tags ||
        tags > nametable || nametable > cod || cod > declared || (natives - publics) % 8 ||
        (pubvars - natives) % 8 || (tags - pubvars) % 8 || libraries > pubvars) return PX_GF_SCRIPT_INVALID;
    if (cod > dat || dat > hea || hea > stp || (dat - cod) % 4 || (hea - dat) % 4 || (stp - dat) % 4 ||
        stp - dat > MAX_DATA || (hea - cod) / 4 > MAX_WORDS || cip >= dat - cod || cip % 4)
        return PX_GF_SCRIPT_INVALID;
    out->size = declared; out->flags = u16(data + 8);
    out->cod = cod; out->dat = dat; out->hea = hea; out->stp = stp; out->cip = cip;
    enum px_gf_script_result r = table(data, publics, natives, &out->publics, &out->public_count);
    if (r == PX_GF_SCRIPT_OK) r = table(data, natives, libraries, &out->natives, &out->native_count);
    if (r == PX_GF_SCRIPT_OK) r = table(data, pubvars, tags, &out->pubvars, &out->pubvar_count);
    if (r != PX_GF_SCRIPT_OK) { px_gf_script_free(out); return r; }
    out->code_words = (dat - cod) / 4; out->data_words = (hea - dat) / 4;
    const uint32_t words = out->code_words + out->data_words;
    uint32_t *all = malloc((words ? words : 1) * sizeof *all);
    if (!all) { px_gf_script_free(out); return PX_GF_SCRIPT_NO_MEMORY; }
    /* 7-bit groups, most significant first; the first byte's bit 6 is the sign. */
    size_t at = cod;
    for (uint32_t i = 0; i < words; ++i) {
        uint32_t x = 0;
        for (unsigned j = 0;; ++j) {
            if (at >= declared || j > 4) { free(all); px_gf_script_free(out); return PX_GF_SCRIPT_TRUNCATED; }
            const uint8_t b = data[at++];
            const uint32_t v = b & 0x7F;
            x = j == 0 ? ((v & 0x40) ? (UINT32_MAX << 7) | v : v) : (x << 7) | v;
            if (!(b & 0x80)) break;
        }
        all[i] = x;
    }
    out->code = malloc((out->code_words ? out->code_words : 1) * sizeof *out->code);
    out->data_init = malloc((out->data_words ? out->data_words : 1) * sizeof *out->data_init);
    if (!out->code || !out->data_init) { free(all); px_gf_script_free(out); return PX_GF_SCRIPT_NO_MEMORY; }
    memcpy(out->code, all, (size_t)out->code_words * sizeof *all);
    memcpy(out->data_init, all + out->code_words, (size_t)out->data_words * sizeof *all);
    free(all);
    return PX_GF_SCRIPT_OK;
}

void px_gf_script_free(struct px_gf_script *s)
{
    if (!s) return;
    free(s->natives); free(s->pubvars); free(s->publics); free(s->code); free(s->data_init);
    memset(s, 0, sizeof *s);
}

/* ---------------------------------------------------------------- machine */
struct machine {
    const struct px_gf_script *s;
    uint8_t *mem;
    uint32_t size; /* data segment bytes */
    int fault;
};
static int32_t rd(struct machine *m, int32_t a)
{
    if (a < 0 || (uint32_t)a > m->size - 4) { m->fault = 1; return 0; }
    int32_t v; memcpy(&v, m->mem + a, 4); return v;
}
static void wr(struct machine *m, int32_t a, int32_t v)
{
    if (a < 0 || (uint32_t)a > m->size - 4) { m->fault = 1; return; }
    memcpy(m->mem + a, &v, 4);
}
static int range(struct machine *m, int32_t a, int32_t n)
{
    if (a < 0 || n < 0 || (uint32_t)a > m->size || (uint32_t)n > m->size - (uint32_t)a) { m->fault = 1; return 0; }
    return 1;
}

enum {
    OP_LOAD_PRI = 1, OP_LOAD_ALT, OP_LOAD_S_PRI, OP_LOAD_S_ALT, OP_LREF_PRI, OP_LREF_ALT, OP_LREF_S_PRI,
    OP_LREF_S_ALT, OP_LOAD_I, OP_LODB_I, OP_CONST_PRI, OP_CONST_ALT, OP_ADDR_PRI, OP_ADDR_ALT, OP_STOR_PRI,
    OP_STOR_ALT, OP_STOR_S_PRI, OP_STOR_S_ALT, OP_SREF_PRI, OP_SREF_ALT, OP_SREF_S_PRI, OP_SREF_S_ALT,
    OP_STOR_I, OP_STRB_I, OP_LIDX, OP_LIDX_B, OP_IDXADDR, OP_IDXADDR_B, OP_ALIGN_PRI, OP_ALIGN_ALT, OP_LCTRL,
    OP_SCTRL, OP_MOVE_PRI, OP_MOVE_ALT, OP_XCHG, OP_PUSH_PRI, OP_PUSH_ALT, OP_PICK, OP_PUSH_C, OP_PUSH,
    OP_PUSH_S, OP_POP_PRI, OP_POP_ALT, OP_STACK, OP_HEAP, OP_PROC, OP_RET, OP_RETN, OP_CALL, OP_CALL_PRI,
    OP_JUMP, OP_JREL, OP_JZER, OP_JNZ, OP_JEQ, OP_JNEQ, OP_JLESS, OP_JLEQ, OP_JGRTR, OP_JGEQ, OP_JSLESS,
    OP_JSLEQ, OP_JSGRTR, OP_JSGEQ, OP_SHL, OP_SHR, OP_SSHR, OP_SHL_C_PRI, OP_SHL_C_ALT, OP_SHR_C_PRI,
    OP_SHR_C_ALT, OP_SMUL, OP_SDIV, OP_SDIV_ALT, OP_UMUL, OP_UDIV, OP_UDIV_ALT, OP_ADD, OP_SUB, OP_SUB_ALT,
    OP_AND, OP_OR, OP_XOR, OP_NOT, OP_NEG, OP_INVERT, OP_ADD_C, OP_SMUL_C, OP_ZERO_PRI, OP_ZERO_ALT, OP_ZERO,
    OP_ZERO_S, OP_SIGN_PRI, OP_SIGN_ALT, OP_EQ, OP_NEQ, OP_LESS, OP_LEQ, OP_GRTR, OP_GEQ, OP_SLESS, OP_SLEQ,
    OP_SGRTR, OP_SGEQ, OP_EQ_C_PRI, OP_EQ_C_ALT, OP_INC_PRI, OP_INC_ALT, OP_INC, OP_INC_S, OP_INC_I,
    OP_DEC_PRI, OP_DEC_ALT, OP_DEC, OP_DEC_S, OP_DEC_I, OP_MOVS, OP_CMPS, OP_FILL, OP_HALT, OP_BOUNDS,
    OP_SYSREQ_PRI, OP_SYSREQ_C, OP_FILE, OP_LINE, OP_SYMBOL, OP_SRANGE, OP_JUMP_PRI, OP_SWITCH, OP_CASETBL,
    OP_SWAP_PRI, OP_SWAP_ALT, OP_PUSH_ADR, OP_NOP, OP_SYSREQ_N, OP_SYMTAG, OP_BREAK,
    OP_PUSH2_C, OP_PUSH2, OP_PUSH2_S, OP_PUSH2_ADR, OP_PUSH3_C, OP_PUSH3, OP_PUSH3_S, OP_PUSH3_ADR,
    OP_PUSH4_C, OP_PUSH4, OP_PUSH4_S, OP_PUSH4_ADR, OP_PUSH5_C, OP_PUSH5, OP_PUSH5_S, OP_PUSH5_ADR,
    OP_LOAD_BOTH, OP_LOAD_S_BOTH, OP_CONST, OP_CONST_S, OP_ICALL, OP_IRETN, OP_ISWITCH, OP_ICASETBL,
    OP_LOAD_P_PRI, OP_LOAD_P_ALT, OP_LOAD_P_S_PRI, OP_LOAD_P_S_ALT, OP_LREF_P_PRI, OP_LREF_P_ALT,
    OP_LREF_P_S_PRI, OP_LREF_P_S_ALT, OP_LODB_P_I, OP_CONST_P_PRI, OP_CONST_P_ALT, OP_ADDR_P_PRI,
    OP_ADDR_P_ALT, OP_STOR_P_PRI, OP_STOR_P_ALT, OP_STOR_P_S_PRI, OP_STOR_P_S_ALT, OP_SREF_P_PRI,
    OP_SREF_P_ALT, OP_SREF_P_S_PRI, OP_SREF_P_S_ALT, OP_STRB_P_I, OP_LIDX_P_B, OP_IDXADDR_P_B,
    OP_ALIGN_P_PRI, OP_ALIGN_P_ALT, OP_PUSH_P_C, OP_PUSH_P, OP_PUSH_P_S, OP_STACK_P, OP_HEAP_P,
    OP_SHL_P_C_PRI, OP_SHL_P_C_ALT, OP_SHR_P_C_PRI, OP_SHR_P_C_ALT, OP_ADD_P_C, OP_SMUL_P_C, OP_ZERO_P,
    OP_ZERO_P_S, OP_EQ_P_C_PRI, OP_EQ_P_C_ALT, OP_INC_P, OP_INC_P_S, OP_DEC_P, OP_DEC_P_S, OP_MOVS_P,
    OP_CMPS_P, OP_FILL_P, OP_HALT_P, OP_BOUNDS_P, OP_PUSH_P_ADR
};

static int32_t load_bytes(struct machine *m, int32_t a, int32_t width)
{
    if (width == 4) return rd(m, a);
    if (!range(m, a, width)) return 0;
    if (width == 1) return m->mem[a];
    if (width == 2) { uint16_t v; memcpy(&v, m->mem + a, 2); return v; }
    m->fault = 1; return 0;
}
static void store_bytes(struct machine *m, int32_t a, int32_t width, int32_t v)
{
    if (width == 4) { wr(m, a, v); return; }
    if (!range(m, a, width)) return;
    if (width == 1) m->mem[a] = (uint8_t)v;
    else if (width == 2) { uint16_t h = (uint16_t)v; memcpy(m->mem + a, &h, 2); }
    else m->fault = 1;
}
static int32_t add(int32_t a, int32_t b) { return (int32_t)((uint32_t)a + (uint32_t)b); }
static int32_t sub(int32_t a, int32_t b) { return (int32_t)((uint32_t)a - (uint32_t)b); }
static int32_t mul(int32_t a, int32_t b) { return (int32_t)((uint32_t)a * (uint32_t)b); }
static int32_t shl(int32_t a, int32_t b) { return (int32_t)((uint32_t)a << (b & 31)); }

void px_gf_script_run(const struct px_gf_script *s, uint32_t pubvar, int32_t value, px_gf_native_fn native,
                      void *context, uint32_t budget, uint32_t max_yields, struct px_gf_run *run)
{
    memset(run, 0, sizeof *run);
    if (!s || !s->code || !native) { run->result = PX_GF_SCRIPT_INVALID_ARGUMENT; return; }
    struct machine m = {s, NULL, s->stp - s->dat, 0};
    m.mem = calloc(m.size ? m.size : 4, 1);
    if (!m.mem || m.size < 8) { free(m.mem); run->result = m.mem ? PX_GF_SCRIPT_INVALID : PX_GF_SCRIPT_NO_MEMORY; return; }
    memcpy(m.mem, s->data_init, (size_t)s->data_words * 4);
    for (uint32_t i = 0; i < s->pubvar_count; ++i)
        if (s->pubvars[i].hash == pubvar) wr(&m, (int32_t)s->pubvars[i].address, value);
    const int32_t code_bytes = (int32_t)(s->code_words * 4);
    int32_t pri = 0, alt = 0, frm = 0, stk = (int32_t)m.size, hea = (int32_t)(s->hea - s->dat);
    int32_t cip = (int32_t)s->cip;
    enum px_gf_script_result result = PX_GF_SCRIPT_BUDGET;
#define PUSH(v) do { stk -= 4; wr(&m, stk, (v)); } while (0)
#define POP(v) do { (v) = rd(&m, stk); stk += 4; } while (0)
#define ARG(k) ((int32_t)s->code[cip / 4 + (k)])
#define NEED(n) do { if (cip / 4 + (n) >= (int32_t)s->code_words) { result = PX_GF_SCRIPT_BAD_INSTRUCTION; goto stop; } } while (0)
    m.fault = 0;
    PUSH(0); PUSH(0); /* no arguments; return to address 0 (the image's final HALT) */
    for (run->steps = 0; run->steps < budget; ++run->steps) {
        if (cip < 0 || cip >= code_bytes || cip % 4) { result = PX_GF_SCRIPT_BAD_INSTRUCTION; goto stop; }
        run->last_cip = (uint32_t)cip;
        const uint32_t word = s->code[cip / 4];
        const uint32_t op = word & 0xFFFF;
        const int32_t p = (int32_t)word >> 16; /* packed operand */
        int32_t next = cip + 4, a, b, t;
        switch (op) {
        /* one-operand forms */
        case OP_LOAD_PRI: NEED(1); pri = rd(&m, ARG(1)); next += 4; break;
        case OP_LOAD_ALT: NEED(1); alt = rd(&m, ARG(1)); next += 4; break;
        case OP_LOAD_S_PRI: NEED(1); pri = rd(&m, frm + ARG(1)); next += 4; break;
        case OP_LOAD_S_ALT: NEED(1); alt = rd(&m, frm + ARG(1)); next += 4; break;
        case OP_LREF_PRI: NEED(1); pri = rd(&m, rd(&m, ARG(1))); next += 4; break;
        case OP_LREF_ALT: NEED(1); alt = rd(&m, rd(&m, ARG(1))); next += 4; break;
        case OP_LREF_S_PRI: NEED(1); pri = rd(&m, rd(&m, frm + ARG(1))); next += 4; break;
        case OP_LREF_S_ALT: NEED(1); alt = rd(&m, rd(&m, frm + ARG(1))); next += 4; break;
        case OP_LODB_I: NEED(1); pri = load_bytes(&m, pri, ARG(1)); next += 4; break;
        case OP_CONST_PRI: NEED(1); pri = ARG(1); next += 4; break;
        case OP_CONST_ALT: NEED(1); alt = ARG(1); next += 4; break;
        case OP_ADDR_PRI: NEED(1); pri = frm + ARG(1); next += 4; break;
        case OP_ADDR_ALT: NEED(1); alt = frm + ARG(1); next += 4; break;
        case OP_STOR_PRI: NEED(1); wr(&m, ARG(1), pri); next += 4; break;
        case OP_STOR_ALT: NEED(1); wr(&m, ARG(1), alt); next += 4; break;
        case OP_STOR_S_PRI: NEED(1); wr(&m, frm + ARG(1), pri); next += 4; break;
        case OP_STOR_S_ALT: NEED(1); wr(&m, frm + ARG(1), alt); next += 4; break;
        case OP_SREF_PRI: NEED(1); wr(&m, rd(&m, ARG(1)), pri); next += 4; break;
        case OP_SREF_ALT: NEED(1); wr(&m, rd(&m, ARG(1)), alt); next += 4; break;
        case OP_SREF_S_PRI: NEED(1); wr(&m, rd(&m, frm + ARG(1)), pri); next += 4; break;
        case OP_SREF_S_ALT: NEED(1); wr(&m, rd(&m, frm + ARG(1)), alt); next += 4; break;
        case OP_STRB_I: NEED(1); store_bytes(&m, alt, ARG(1), pri); next += 4; break;
        case OP_LIDX_B: NEED(1); pri = rd(&m, add(shl(pri, ARG(1)), alt)); next += 4; break;
        case OP_IDXADDR_B: NEED(1); pri = add(shl(pri, ARG(1)), alt); next += 4; break;
        case OP_ALIGN_PRI: NEED(1); if (ARG(1) >= 0 && ARG(1) < 4) pri ^= 4 - ARG(1); next += 4; break;
        case OP_ALIGN_ALT: NEED(1); if (ARG(1) >= 0 && ARG(1) < 4) alt ^= 4 - ARG(1); next += 4; break;
        case OP_LCTRL:
            NEED(1);
            switch (ARG(1)) {
            case 0: pri = (int32_t)s->cod; break;
            case 1: pri = (int32_t)s->dat; break;
            case 2: pri = hea; break;
            case 3: pri = (int32_t)m.size; break;
            case 4: pri = stk; break;
            case 5: pri = frm; break;
            case 6: pri = cip + 8; break;
            default: result = PX_GF_SCRIPT_BAD_INSTRUCTION; goto stop;
            }
            next += 4; break;
        case OP_SCTRL:
            NEED(1);
            switch (ARG(1)) {
            case 2: hea = pri; break;
            case 4: stk = pri; break;
            case 5: frm = pri; break;
            case 6: next = pri - 4; break;
            default: break;
            }
            next += 4; break;
        case OP_PICK: NEED(1); pri = rd(&m, stk + ARG(1)); next += 4; break;
        case OP_PUSH_C: NEED(1); PUSH(ARG(1)); next += 4; break;
        case OP_PUSH: NEED(1); PUSH(rd(&m, ARG(1))); next += 4; break;
        case OP_PUSH_S: NEED(1); PUSH(rd(&m, frm + ARG(1))); next += 4; break;
        case OP_PUSH_ADR: NEED(1); PUSH(frm + ARG(1)); next += 4; break;
        case OP_STACK: NEED(1); alt = stk; stk = add(stk, ARG(1)); next += 4; break;
        case OP_HEAP: NEED(1); alt = hea; hea = add(hea, ARG(1)); next += 4; break;
        case OP_CALL: NEED(1); PUSH(cip + 8); next = cip + ARG(1); break;
        case OP_JUMP: NEED(1); next = cip + ARG(1); break;
        case OP_JREL: NEED(1); next = cip + 8 + ARG(1); break;
        case OP_JZER: case OP_JNZ: case OP_JEQ: case OP_JNEQ: case OP_JLESS: case OP_JLEQ: case OP_JGRTR:
        case OP_JGEQ: case OP_JSLESS: case OP_JSLEQ: case OP_JSGRTR: case OP_JSGEQ: {
            NEED(1);
            const uint32_t ua = (uint32_t)pri, ub = (uint32_t)alt;
            int c = 0;
            switch (op) {
            case OP_JZER: c = pri == 0; break;
            case OP_JNZ: c = pri != 0; break;
            case OP_JEQ: c = pri == alt; break;
            case OP_JNEQ: c = pri != alt; break;
            case OP_JLESS: c = ua < ub; break;
            case OP_JLEQ: c = ua <= ub; break;
            case OP_JGRTR: c = ua > ub; break;
            case OP_JGEQ: c = ua >= ub; break;
            case OP_JSLESS: c = pri < alt; break;
            case OP_JSLEQ: c = pri <= alt; break;
            case OP_JSGRTR: c = pri > alt; break;
            default: c = pri >= alt; break;
            }
            next = c ? cip + ARG(1) : cip + 8;
            break;
        }
        case OP_SHL_C_PRI: NEED(1); pri = shl(pri, ARG(1)); next += 4; break;
        case OP_SHL_C_ALT: NEED(1); alt = shl(alt, ARG(1)); next += 4; break;
        case OP_SHR_C_PRI: NEED(1); pri = (int32_t)((uint32_t)pri >> (ARG(1) & 31)); next += 4; break;
        case OP_SHR_C_ALT: NEED(1); alt = (int32_t)((uint32_t)alt >> (ARG(1) & 31)); next += 4; break;
        case OP_ADD_C: NEED(1); pri = add(pri, ARG(1)); next += 4; break;
        case OP_SMUL_C: NEED(1); pri = mul(pri, ARG(1)); next += 4; break;
        case OP_ZERO: NEED(1); wr(&m, ARG(1), 0); next += 4; break;
        case OP_ZERO_S: NEED(1); wr(&m, frm + ARG(1), 0); next += 4; break;
        case OP_EQ_C_PRI: NEED(1); pri = pri == ARG(1); next += 4; break;
        case OP_EQ_C_ALT: NEED(1); pri = alt == ARG(1); next += 4; break;
        case OP_INC: NEED(1); wr(&m, ARG(1), add(rd(&m, ARG(1)), 1)); next += 4; break;
        case OP_INC_S: NEED(1); wr(&m, frm + ARG(1), add(rd(&m, frm + ARG(1)), 1)); next += 4; break;
        case OP_DEC: NEED(1); wr(&m, ARG(1), sub(rd(&m, ARG(1)), 1)); next += 4; break;
        case OP_DEC_S: NEED(1); wr(&m, frm + ARG(1), sub(rd(&m, frm + ARG(1)), 1)); next += 4; break;
        case OP_MOVS: case OP_CMPS: case OP_FILL: case OP_MOVS_P: case OP_CMPS_P: case OP_FILL_P:
            if (op <= OP_FILL) { NEED(1); t = ARG(1); next += 4; } else t = p;
            if (op == OP_MOVS || op == OP_MOVS_P) {
                if (range(&m, pri, t) && range(&m, alt, t)) memmove(m.mem + alt, m.mem + pri, (size_t)t);
            } else if (op == OP_CMPS || op == OP_CMPS_P) {
                if (range(&m, pri, t) && range(&m, alt, t)) pri = memcmp(m.mem + alt, m.mem + pri, (size_t)t);
            } else {
                for (int32_t k = 0; k + 4 <= t && !m.fault; k += 4) wr(&m, alt + k, pri);
            }
            break;
        case OP_HALT: case OP_HALT_P:
            if (op == OP_HALT) { NEED(1); t = ARG(1); next += 4; } else t = p;
            if (t != 12) { result = PX_GF_SCRIPT_HALTED; goto stop; }
            if (++run->yields > max_yields) { result = PX_GF_SCRIPT_YIELD_LIMIT; goto stop; }
            break;
        case OP_BOUNDS: case OP_BOUNDS_P:
            if (op == OP_BOUNDS) { NEED(1); t = ARG(1); next += 4; } else t = p;
            if ((uint32_t)pri > (uint32_t)t) { result = PX_GF_SCRIPT_MEMORY_FAULT; goto stop; }
            break;
        case OP_SWITCH: {
            NEED(1);
            const int32_t table_at = cip + ARG(1);
            if (table_at < 0 || table_at + 12 > code_bytes || table_at % 4 || s->code[table_at / 4] != OP_CASETBL) {
                result = PX_GF_SCRIPT_BAD_INSTRUCTION; goto stop;
            }
            const int32_t count = (int32_t)s->code[table_at / 4 + 1];
            if (count < 0 || table_at + 12 + 8 * (int64_t)count > code_bytes) { result = PX_GF_SCRIPT_BAD_INSTRUCTION; goto stop; }
            /* Each target is relative to its own word, less one cell (JUMPREL). */
            next = table_at + 8 + (int32_t)s->code[table_at / 4 + 2] - 4;
            for (int32_t k = 0; k < count; ++k)
                if ((int32_t)s->code[table_at / 4 + 3 + 2 * k] == pri) {
                    next = table_at + 16 + 8 * k + (int32_t)s->code[table_at / 4 + 4 + 2 * k] - 4;
                    break;
                }
            break;
        }
        case OP_SYSREQ_N: case OP_SYSREQ_C: case OP_SYSREQ_PRI: {
            int32_t index, bytes;
            if (op == OP_SYSREQ_N) { NEED(2); index = ARG(1); bytes = ARG(2); PUSH(bytes); next += 8; }
            else if (op == OP_SYSREQ_C) { NEED(1); index = ARG(1); bytes = rd(&m, stk); next += 4; }
            else { index = pri; bytes = rd(&m, stk); }
            if (index < 0 || (uint32_t)index >= s->native_count || bytes < 0 || bytes % 4 || bytes / 4 > MAX_ARGS) {
                result = PX_GF_SCRIPT_BAD_INSTRUCTION; goto stop;
            }
            int32_t args[MAX_ARGS];
            for (int32_t k = 0; k < bytes / 4; ++k) args[k] = rd(&m, stk + 4 + 4 * k);
            if (m.fault) { result = PX_GF_SCRIPT_MEMORY_FAULT; goto stop; }
            int failed = 0;
            pri = native(context, s->natives[index].hash, args, (uint32_t)(bytes / 4), &failed);
            if (failed) { result = PX_GF_SCRIPT_NATIVE_FAILED; goto stop; }
            if (op == OP_SYSREQ_N) stk = add(stk, bytes + 4);
            break;
        }
        case OP_LOAD_BOTH: NEED(2); pri = rd(&m, ARG(1)); alt = rd(&m, ARG(2)); next += 8; break;
        case OP_LOAD_S_BOTH: NEED(2); pri = rd(&m, frm + ARG(1)); alt = rd(&m, frm + ARG(2)); next += 8; break;
        case OP_CONST: NEED(2); wr(&m, ARG(1), ARG(2)); next += 8; break;
        case OP_CONST_S: NEED(2); wr(&m, frm + ARG(1), ARG(2)); next += 8; break;
        case OP_PUSH2_C: case OP_PUSH2: case OP_PUSH2_S: case OP_PUSH2_ADR: case OP_PUSH3_C: case OP_PUSH3:
        case OP_PUSH3_S: case OP_PUSH3_ADR: case OP_PUSH4_C: case OP_PUSH4: case OP_PUSH4_S: case OP_PUSH4_ADR:
        case OP_PUSH5_C: case OP_PUSH5: case OP_PUSH5_S: case OP_PUSH5_ADR: {
            const int32_t n = (int32_t)(op - OP_PUSH2_C) / 4 + 2, kind = (int32_t)(op - OP_PUSH2_C) % 4;
            NEED(n);
            for (int32_t k = 1; k <= n; ++k) {
                const int32_t v = ARG(k);
                PUSH(kind == 0 ? v : kind == 1 ? rd(&m, v) : kind == 2 ? rd(&m, frm + v) : frm + v);
            }
            next += 4 * n;
            break;
        }
        /* no-operand forms */
        case OP_LOAD_I: pri = rd(&m, pri); break;
        case OP_STOR_I: wr(&m, alt, pri); break;
        case OP_LIDX: pri = rd(&m, add(mul(pri, 4), alt)); break;
        case OP_IDXADDR: pri = add(mul(pri, 4), alt); break;
        case OP_MOVE_PRI: pri = alt; break;
        case OP_MOVE_ALT: alt = pri; break;
        case OP_XCHG: t = pri; pri = alt; alt = t; break;
        case OP_PUSH_PRI: PUSH(pri); break;
        case OP_PUSH_ALT: PUSH(alt); break;
        case OP_POP_PRI: POP(pri); break;
        case OP_POP_ALT: POP(alt); break;
        case OP_PROC: PUSH(frm); frm = stk; break;
        case OP_RET: POP(frm); POP(next); break;
        case OP_RETN: POP(frm); POP(next); stk = add(stk, add(rd(&m, stk), 4)); break;
        case OP_CALL_PRI: PUSH(cip + 4); next = pri; break;
        case OP_JUMP_PRI: next = pri; break;
        case OP_SHL: pri = shl(pri, alt); break;
        case OP_SHR: pri = (int32_t)((uint32_t)pri >> (alt & 31)); break;
        case OP_SSHR: pri = pri >> (alt & 31); break;
        case OP_SMUL: pri = mul(pri, alt); break;
        case OP_SDIV: case OP_SDIV_ALT:
            a = op == OP_SDIV ? pri : alt; b = op == OP_SDIV ? alt : pri;
            if (b == 0 || (a == INT32_MIN && b == -1)) { result = PX_GF_SCRIPT_BAD_INSTRUCTION; goto stop; }
            t = a / b;
            if ((a % b != 0) && ((a < 0) != (b < 0))) --t; /* floored, as Pawn */
            pri = t; alt = sub(a, mul(t, b));
            break;
        case OP_UMUL: pri = mul(pri, alt); break;
        case OP_UDIV: case OP_UDIV_ALT: {
            const uint32_t ua = (uint32_t)(op == OP_UDIV ? pri : alt), ub = (uint32_t)(op == OP_UDIV ? alt : pri);
            if (!ub) { result = PX_GF_SCRIPT_BAD_INSTRUCTION; goto stop; }
            pri = (int32_t)(ua / ub); alt = (int32_t)(ua % ub);
            break;
        }
        case OP_ADD: pri = add(pri, alt); break;
        case OP_SUB: pri = sub(pri, alt); break;
        case OP_SUB_ALT: pri = sub(alt, pri); break;
        case OP_AND: pri &= alt; break;
        case OP_OR: pri |= alt; break;
        case OP_XOR: pri ^= alt; break;
        case OP_NOT: pri = !pri; break;
        case OP_NEG: pri = sub(0, pri); break;
        case OP_INVERT: pri = ~pri; break;
        case OP_ZERO_PRI: pri = 0; break;
        case OP_ZERO_ALT: alt = 0; break;
        case OP_SIGN_PRI: if ((pri & 0xFF) >= 0x80) pri |= ~0xFF; break;
        case OP_SIGN_ALT: if ((alt & 0xFF) >= 0x80) alt |= ~0xFF; break;
        case OP_EQ: pri = pri == alt; break;
        case OP_NEQ: pri = pri != alt; break;
        case OP_LESS: pri = (uint32_t)pri < (uint32_t)alt; break;
        case OP_LEQ: pri = (uint32_t)pri <= (uint32_t)alt; break;
        case OP_GRTR: pri = (uint32_t)pri > (uint32_t)alt; break;
        case OP_GEQ: pri = (uint32_t)pri >= (uint32_t)alt; break;
        case OP_SLESS: pri = pri < alt; break;
        case OP_SLEQ: pri = pri <= alt; break;
        case OP_SGRTR: pri = pri > alt; break;
        case OP_SGEQ: pri = pri >= alt; break;
        case OP_INC_PRI: pri = add(pri, 1); break;
        case OP_INC_ALT: alt = add(alt, 1); break;
        case OP_INC_I: wr(&m, pri, add(rd(&m, pri), 1)); break;
        case OP_DEC_PRI: pri = sub(pri, 1); break;
        case OP_DEC_ALT: alt = sub(alt, 1); break;
        case OP_DEC_I: wr(&m, pri, sub(rd(&m, pri), 1)); break;
        case OP_SWAP_PRI: t = rd(&m, stk); wr(&m, stk, pri); pri = t; break;
        case OP_SWAP_ALT: t = rd(&m, stk); wr(&m, stk, alt); alt = t; break;
        case OP_NOP: case OP_BREAK: break;
        /* packed forms */
        case OP_LOAD_P_PRI: pri = rd(&m, p); break;
        case OP_LOAD_P_ALT: alt = rd(&m, p); break;
        case OP_LOAD_P_S_PRI: pri = rd(&m, frm + p); break;
        case OP_LOAD_P_S_ALT: alt = rd(&m, frm + p); break;
        case OP_LREF_P_PRI: pri = rd(&m, rd(&m, p)); break;
        case OP_LREF_P_ALT: alt = rd(&m, rd(&m, p)); break;
        case OP_LREF_P_S_PRI: pri = rd(&m, rd(&m, frm + p)); break;
        case OP_LREF_P_S_ALT: alt = rd(&m, rd(&m, frm + p)); break;
        case OP_LODB_P_I: pri = load_bytes(&m, pri, p); break;
        case OP_CONST_P_PRI: pri = p; break;
        case OP_CONST_P_ALT: alt = p; break;
        case OP_ADDR_P_PRI: pri = frm + p; break;
        case OP_ADDR_P_ALT: alt = frm + p; break;
        case OP_STOR_P_PRI: wr(&m, p, pri); break;
        case OP_STOR_P_ALT: wr(&m, p, alt); break;
        case OP_STOR_P_S_PRI: wr(&m, frm + p, pri); break;
        case OP_STOR_P_S_ALT: wr(&m, frm + p, alt); break;
        case OP_SREF_P_PRI: wr(&m, rd(&m, p), pri); break;
        case OP_SREF_P_ALT: wr(&m, rd(&m, p), alt); break;
        case OP_SREF_P_S_PRI: wr(&m, rd(&m, frm + p), pri); break;
        case OP_SREF_P_S_ALT: wr(&m, rd(&m, frm + p), alt); break;
        case OP_STRB_P_I: store_bytes(&m, alt, p, pri); break;
        case OP_LIDX_P_B: pri = rd(&m, add(shl(pri, p), alt)); break;
        case OP_IDXADDR_P_B: pri = add(shl(pri, p), alt); break;
        case OP_ALIGN_P_PRI: if (p >= 0 && p < 4) pri ^= 4 - p; break;
        case OP_ALIGN_P_ALT: if (p >= 0 && p < 4) alt ^= 4 - p; break;
        case OP_PUSH_P_C: PUSH(p); break;
        case OP_PUSH_P: PUSH(rd(&m, p)); break;
        case OP_PUSH_P_S: PUSH(rd(&m, frm + p)); break;
        case OP_PUSH_P_ADR: PUSH(frm + p); break;
        case OP_STACK_P: alt = stk; stk = add(stk, p); break;
        case OP_HEAP_P: alt = hea; hea = add(hea, p); break;
        case OP_SHL_P_C_PRI: pri = shl(pri, p); break;
        case OP_SHL_P_C_ALT: alt = shl(alt, p); break;
        case OP_SHR_P_C_PRI: pri = (int32_t)((uint32_t)pri >> (p & 31)); break;
        case OP_SHR_P_C_ALT: alt = (int32_t)((uint32_t)alt >> (p & 31)); break;
        case OP_ADD_P_C: pri = add(pri, p); break;
        case OP_SMUL_P_C: pri = mul(pri, p); break;
        case OP_ZERO_P: wr(&m, p, 0); break;
        case OP_ZERO_P_S: wr(&m, frm + p, 0); break;
        case OP_EQ_P_C_PRI: pri = pri == p; break;
        case OP_EQ_P_C_ALT: pri = alt == p; break;
        case OP_INC_P: wr(&m, p, add(rd(&m, p), 1)); break;
        case OP_INC_P_S: wr(&m, frm + p, add(rd(&m, frm + p), 1)); break;
        case OP_DEC_P: wr(&m, p, sub(rd(&m, p), 1)); break;
        case OP_DEC_P_S: wr(&m, frm + p, sub(rd(&m, frm + p), 1)); break;
        default: result = PX_GF_SCRIPT_BAD_INSTRUCTION; goto stop;
        }
        if (m.fault || stk < hea) { result = PX_GF_SCRIPT_MEMORY_FAULT; goto stop; }
        cip = next;
    }
stop:
#undef PUSH
#undef POP
#undef ARG
#undef NEED
    run->result = result;
    free(m.mem);
}

const char *px_gf_script_result_string(enum px_gf_script_result result)
{
    switch (result) {
    case PX_GF_SCRIPT_OK: return "ok";
    case PX_GF_SCRIPT_INVALID_ARGUMENT: return "invalid argument";
    case PX_GF_SCRIPT_TRUNCATED: return "truncated script";
    case PX_GF_SCRIPT_INVALID: return "invalid script";
    case PX_GF_SCRIPT_NO_MEMORY: return "out of memory";
    case PX_GF_SCRIPT_HALTED: return "halted";
    case PX_GF_SCRIPT_BUDGET: return "instruction budget";
    case PX_GF_SCRIPT_YIELD_LIMIT: return "yield limit";
    case PX_GF_SCRIPT_MEMORY_FAULT: return "memory fault";
    case PX_GF_SCRIPT_BAD_INSTRUCTION: return "bad instruction";
    case PX_GF_SCRIPT_NATIVE_FAILED: return "native failed";
    }
    return "unknown script result";
}
