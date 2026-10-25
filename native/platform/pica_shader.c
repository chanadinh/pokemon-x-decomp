#include "pica_shader.h"

#include <math.h>
#include <string.h>

static uint32_t u32(const uint8_t *p) { return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24; }
static uint16_t u16(const uint8_t *p) { return (uint16_t)(p[0] | p[1] << 8); }

float px_pica_float24(uint32_t bits)
{
    /* 1 sign bit, 7 exponent bits (bias 63), 16 mantissa bits. */
    const uint32_t sign = (bits >> 23) & 1, exponent = (bits >> 16) & 0x7f, mantissa = bits & 0xffff;
    uint32_t f;
    if (!exponent && !mantissa) f = sign << 31;
    else if (exponent == 0x7f) f = sign << 31 | 0xffu << 23 | mantissa << 7;
    else f = sign << 31 | (exponent + 64) << 23 | mantissa << 7;
    float out;
    memcpy(&out, &f, sizeof out);
    return out;
}

int px_dvlb_decode(const uint8_t *d, size_t size, struct px_dvlb *out)
{
    memset(out, 0, sizeof *out);
    if (size < 8) return PX_PICA_TRUNCATED;
    if (memcmp(d, "DVLB", 4)) return PX_PICA_INVALID;
    const uint32_t count = u32(d + 4);
    if (!count || count > PX_PICA_MAX_ENTRIES) return PX_PICA_LIMIT;
    if (8 + 4 * (size_t)count + 0x28 > size) return PX_PICA_TRUNCATED;
    const size_t p = 8 + 4 * (size_t)count;
    if (memcmp(d + p, "DVLP", 4)) return PX_PICA_INVALID;
    const uint32_t code_at = u32(d + p + 8), code_n = u32(d + p + 12), op_at = u32(d + p + 16), op_n = u32(d + p + 20);
    if (code_n > PX_PICA_MAX_CODE || op_n > PX_PICA_MAX_OPS) return PX_PICA_LIMIT;
    if (p + (uint64_t)code_at + 4 * (uint64_t)code_n > size || p + (uint64_t)op_at + 8 * (uint64_t)op_n > size)
        return PX_PICA_TRUNCATED;
    for (uint32_t k = 0; k < code_n; ++k) out->code[k] = u32(d + p + code_at + 4 * k);
    for (uint32_t k = 0; k < op_n; ++k) out->ops[k] = u32(d + p + op_at + 8 * k);
    out->code_count = code_n; out->op_count = op_n;
    for (uint32_t e = 0; e < count; ++e) {
        const size_t at = u32(d + 8 + 4 * e);
        if (at + 0x40 > size) return PX_PICA_TRUNCATED;
        const uint8_t *h = d + at;
        if (memcmp(h, "DVLE", 4)) return PX_PICA_INVALID;
        struct px_pica_entry *x = &out->entries[e];
        x->type = h[6]; x->main = u32(h + 8); x->end = u32(h + 12);
        x->input_mask = u16(h + 0x10); x->output_mask = u16(h + 0x12);
        const uint32_t c_at = u32(h + 0x18), c_n = u32(h + 0x1c), o_at = u32(h + 0x28), o_n = u32(h + 0x2c);
        const uint32_t u_at = u32(h + 0x30), u_n = u32(h + 0x34), s_at = u32(h + 0x38), s_n = u32(h + 0x3c);
        if (c_n > 96 || o_n > 16 || u_n > PX_PICA_MAX_UNIFORMS || x->main >= code_n) return PX_PICA_LIMIT;
        if (at + (uint64_t)c_at + 20 * (uint64_t)c_n > size || at + (uint64_t)o_at + 8 * (uint64_t)o_n > size ||
            at + (uint64_t)u_at + 8 * (uint64_t)u_n > size || at + (uint64_t)s_at + s_n > size) return PX_PICA_TRUNCATED;
        for (uint32_t k = 0; k < c_n; ++k) {
            const uint8_t *c = h + c_at + 20 * k;
            x->constants[k].type = c[0]; x->constants[k].reg = c[2];
            for (unsigned j = 0; j < 4; ++j) x->constants[k].value[j] = u32(c + 4 + 4 * j);
        }
        x->constant_count = c_n;
        for (uint32_t k = 0; k < o_n; ++k) {
            const uint8_t *o = h + o_at + 8 * k;
            x->outputs[k] = (struct px_pica_output){u16(o), u16(o + 2), u32(o + 4)};
        }
        x->output_count = o_n;
        for (uint32_t k = 0; k < u_n; ++k) {
            const uint8_t *u = h + u_at + 8 * k;
            const uint32_t name = u32(u);
            if (name >= s_n) return PX_PICA_INVALID;
            size_t n = 0;
            while (name + n < s_n && h[s_at + name + n] && n + 1 < sizeof x->uniforms[k].name) ++n;
            memcpy(x->uniforms[k].name, h + s_at + name, n);
            x->uniforms[k].name[n] = 0;
            x->uniforms[k].first = u16(u + 4); x->uniforms[k].last = u16(u + 6);
        }
        x->uniform_count = u_n;
    }
    out->entry_count = count;
    return PX_PICA_OK;
}

int px_pica_uniform(const struct px_pica_entry *e, const char *name)
{
    for (uint32_t k = 0; k < e->uniform_count; ++k)
        if (!strcmp(e->uniforms[k].name, name)) return e->uniforms[k].first;
    return -1;
}

void px_pica_constants(const struct px_pica_entry *e, struct px_pica_state *s)
{
    for (uint32_t k = 0; k < e->constant_count; ++k) {
        const struct px_pica_constant *c = &e->constants[k];
        if (c->type == 0 && c->reg < 16) s->b[c->reg] = (c->value[0] & 0xff) != 0;
        else if (c->type == 1 && c->reg < 4) for (unsigned j = 0; j < 4; ++j) s->i[c->reg][j] = (uint8_t)(c->value[0] >> (8 * j));
        else if (c->type == 2 && c->reg < 96)
            for (unsigned j = 0; j < 4; ++j) s->c[c->reg][j] = px_pica_float24(c->value[j]);
    }
}

/* ----------------------------------------------------------- execution */
static float mul(float a, float b) { return a == 0 || b == 0 ? 0.0f : a * b; } /* 0 x inf = 0 */

struct frame { uint32_t final, ret, loop; int32_t repeat, increment; int is_loop; };
struct run { const struct px_dvlb *p; struct px_pica_state *s; int bad; };

static const float *source(struct run *x, uint32_t index, int32_t offset, float scratch[4])
{
    struct px_pica_state *s = x->s;
    if (index < 0x10) return s->v[index];
    if (index < 0x20) return s->r[index - 0x10];
    const int32_t u = (int32_t)index - 0x20 + offset;
    if (u < 0 || u >= 96) { scratch[0] = scratch[1] = scratch[2] = scratch[3] = 0; x->bad = 1; return scratch; }
    return s->c[u];
}

static void operand(struct run *x, uint32_t index, int32_t offset, uint32_t swizzle, int negate, float out[4])
{
    float scratch[4];
    const float *r = source(x, index, offset, scratch);
    for (unsigned k = 0; k < 4; ++k) {
        const float v = r[(swizzle >> (6 - 2 * k)) & 3];
        out[k] = negate ? -v : v;
    }
}

static void store(struct run *x, uint32_t dst, uint32_t mask, const float v[4])
{
    float *d = dst < 0x10 ? x->s->o[dst] : x->s->r[dst - 0x10];
    for (unsigned k = 0; k < 4; ++k) if (mask & (8u >> k)) d[k] = v[k];
}

static int compare(float a, float b, unsigned op)
{
    switch (op) {
    case 0: return a == b;
    case 1: return a != b;
    case 2: return a < b;
    case 3: return a <= b;
    case 4: return a > b;
    case 5: return a >= b;
    default: return 0;
    }
}

static int condition(const struct px_pica_state *s, uint32_t w)
{
    const int refx = (w >> 25) & 1, refy = (w >> 24) & 1;
    const int rx = s->cmp[0] == refx, ry = s->cmp[1] == refy;
    switch ((w >> 22) & 3) {
    case 0: return rx || ry;
    case 1: return rx && ry;
    case 2: return rx;
    default: return ry;
    }
}

int px_pica_run(const struct px_dvlb *p, const struct px_pica_entry *e, struct px_pica_state *s)
{
    struct run x = {p, s, 0};
    struct frame stack[16];
    unsigned depth = 0;
    uint32_t pc = e->main;
    s->steps = 0;
    for (;;) {
        if (depth && pc == stack[depth - 1].final) {
            struct frame *top = &stack[depth - 1];
            s->aL += top->increment;
            if (top->repeat-- == 0) { pc = top->ret; --depth; }
            else pc = top->loop;
            continue;
        }
        if (pc >= p->code_count) return PX_PICA_INVALID;
        if (++s->steps > 1u << 20) return PX_PICA_STEPS;
        const uint32_t w = p->code[pc];
        const uint32_t op = w >> 26;
        uint32_t next = pc + 1;
#define CALL(offset, count, ret_, repeat_, inc_) do { \
            if (depth >= 16) return PX_PICA_STACK; \
            stack[depth++] = (struct frame){(offset) + (count), (ret_), (offset), (repeat_), (inc_), op == 0x29}; \
            next = (offset); } while (0)
        if (op >= 0x30) {
            /* MAD / MADI: dst 24-28, idx 22-23, src1 17-21 (5 bits); MAD
             * src2 10-16 (7) src3 5-9 (5); MADI src2 12-16 (5) src3 5-11 (7). */
            const int inverted = (w >> 29) == 6;
            const uint32_t desc = p->ops[(w & 31) < p->op_count ? (w & 31) : 0], dst = (w >> 24) & 31;
            const uint32_t idx = (w >> 22) & 3, s1 = (w >> 17) & 31;
            const uint32_t s2 = inverted ? (w >> 12) & 31 : (w >> 10) & 127, s3 = inverted ? (w >> 5) & 127 : (w >> 5) & 31;
            if ((w & 31) >= p->op_count) return PX_PICA_INVALID;
            const int32_t off = idx == 1 ? s->a[0] : idx == 2 ? s->a[1] : idx == 3 ? s->aL : 0;
            float a[4], b[4], c[4], r[4];
            operand(&x, s1, 0, (desc >> 5) & 255, (desc >> 4) & 1, a);
            operand(&x, s2, inverted ? 0 : off, (desc >> 14) & 255, (desc >> 13) & 1, b);
            operand(&x, s3, inverted ? off : 0, (desc >> 23) & 255, (desc >> 22) & 1, c);
            for (unsigned k = 0; k < 4; ++k) r[k] = mul(a[k], b[k]) + c[k];
            store(&x, dst, desc & 15, r);
        } else if ((op >> 1) == 0x17) {
            /* CMP: cmpx 24-26, cmpy 21-23, idx 19-20, src1 12-18, src2 7-11. */
            const uint32_t d = w & 127;
            if (d >= p->op_count) return PX_PICA_INVALID;
            const uint32_t desc = p->ops[d], idx = (w >> 19) & 3;
            const int32_t off = idx == 1 ? s->a[0] : idx == 2 ? s->a[1] : idx == 3 ? s->aL : 0;
            float a[4], b[4];
            operand(&x, (w >> 12) & 127, off, (desc >> 5) & 255, (desc >> 4) & 1, a);
            operand(&x, (w >> 7) & 31, 0, (desc >> 14) & 255, (desc >> 13) & 1, b);
            s->cmp[0] = (uint8_t)compare(a[0], b[0], (w >> 24) & 7);
            s->cmp[1] = (uint8_t)compare(a[1], b[1], (w >> 21) & 7);
        } else if (op < 0x20) {
            const uint32_t d = w & 127;
            if (d >= p->op_count) return PX_PICA_INVALID;
            const uint32_t desc = p->ops[d], dst = (w >> 21) & 31, idx = (w >> 19) & 3, mask = desc & 15;
            const int inverted = op >= 0x18;
            const uint32_t s1 = inverted ? (w >> 14) & 31 : (w >> 12) & 127, s2 = inverted ? (w >> 7) & 127 : (w >> 7) & 31;
            const int32_t off = idx == 1 ? s->a[0] : idx == 2 ? s->a[1] : idx == 3 ? s->aL : 0;
            float a[4], b[4], r[4] = {0, 0, 0, 0};
            operand(&x, s1, inverted ? 0 : off, (desc >> 5) & 255, (desc >> 4) & 1, a);
            operand(&x, s2, inverted ? off : 0, (desc >> 14) & 255, (desc >> 13) & 1, b);
            switch (op) {
            case 0x00: for (unsigned k = 0; k < 4; ++k) r[k] = a[k] + b[k]; break;                  /* ADD */
            case 0x01: { const float t = mul(a[0], b[0]) + mul(a[1], b[1]) + mul(a[2], b[2]);    /* DP3 */
                         r[0] = r[1] = r[2] = r[3] = t; break; }
            case 0x02: { const float t = mul(a[0], b[0]) + mul(a[1], b[1]) + mul(a[2], b[2]) + mul(a[3], b[3]);
                         r[0] = r[1] = r[2] = r[3] = t; break; }                                      /* DP4 */
            case 0x03: case 0x18: { const float t = mul(a[0], b[0]) + mul(a[1], b[1]) + mul(a[2], b[2]) + b[3];
                         r[0] = r[1] = r[2] = r[3] = t; break; }                                      /* DPH(I) */
            case 0x04: case 0x19: r[0] = 1; r[1] = mul(a[1], b[1]); r[2] = a[2]; r[3] = b[3]; break; /* DST(I) */
            case 0x05: r[0] = r[1] = r[2] = r[3] = exp2f(a[0]); break;                             /* EX2 */
            case 0x06: r[0] = r[1] = r[2] = r[3] = log2f(a[0]); break;                             /* LG2 */
            case 0x07: return PX_PICA_UNSUPPORTED;                                                  /* LITP */
            case 0x08: for (unsigned k = 0; k < 4; ++k) r[k] = mul(a[k], b[k]); break;             /* MUL */
            case 0x09: case 0x1a: for (unsigned k = 0; k < 4; ++k) r[k] = a[k] >= b[k] ? 1.0f : 0.0f; break;
            case 0x0a: case 0x1b: for (unsigned k = 0; k < 4; ++k) r[k] = a[k] < b[k] ? 1.0f : 0.0f; break;
            case 0x0b: for (unsigned k = 0; k < 4; ++k) r[k] = floorf(a[k]); break;                /* FLR */
            case 0x0c: for (unsigned k = 0; k < 4; ++k) r[k] = a[k] > b[k] ? a[k] : b[k]; break;   /* MAX */
            case 0x0d: for (unsigned k = 0; k < 4; ++k) r[k] = a[k] < b[k] ? a[k] : b[k]; break;   /* MIN */
            case 0x0e: r[0] = r[1] = r[2] = r[3] = 1.0f / a[0]; break;                             /* RCP */
            case 0x0f: r[0] = r[1] = r[2] = r[3] = 1.0f / sqrtf(a[0]); break;                      /* RSQ */
            case 0x12:                                                                              /* MOVA */
                if (mask & 8) s->a[0] = (int32_t)a[0];
                if (mask & 4) s->a[1] = (int32_t)a[1];
                break;
            case 0x13: memcpy(r, a, sizeof r); break;                                               /* MOV */
            default: return PX_PICA_INVALID;
            }
            if (op != 0x12) store(&x, dst, mask, r);
        } else {
            const uint32_t num = w & 255, dst = (w >> 10) & 0xfff;
            switch (op) {
            case 0x20: /* BREAK: leave the innermost loop */
            case 0x23: /* BREAKC */
                if (op == 0x20 || condition(s, w)) {
                    while (depth && !stack[depth - 1].is_loop) --depth; /* calls inside the loop end too */
                    if (!depth) return PX_PICA_INVALID;
                    next = stack[--depth].ret;
                }
                break;
            case 0x21: break; /* NOP */
            case 0x22: return x.bad ? PX_PICA_INVALID : PX_PICA_OK; /* END */
            case 0x24: CALL(dst, num, pc + 1, 0, 0); break;
            case 0x25: if (condition(s, w)) CALL(dst, num, pc + 1, 0, 0); break;
            case 0x26: if (s->b[(w >> 22) & 15]) CALL(dst, num, pc + 1, 0, 0); break;
            case 0x27: /* IFU */
            case 0x28: { /* IFC */
                const int take = op == 0x27 ? s->b[(w >> 22) & 15] != 0 : condition(s, w);
                if (dst <= pc) return PX_PICA_INVALID;
                if (take) CALL(pc + 1, dst - pc - 1, dst + num, 0, 0);
                else CALL(dst, num, dst + num, 0, 0);
                break;
            }
            case 0x29: { /* LOOP: i.x + 1 iterations, aL from i.y, step i.z */
                const uint8_t *i = s->i[(w >> 22) & 3];
                if (dst < pc) return PX_PICA_INVALID;
                s->aL = i[1];
                CALL(pc + 1, dst - pc, dst + 1, i[0], (int8_t)i[2]);
                break;
            }
            case 0x2a: case 0x2b: return PX_PICA_UNSUPPORTED; /* EMIT, SETEMIT */
            case 0x2c: if (condition(s, w)) next = dst; break; /* JMPC */
            case 0x2d: if ((s->b[(w >> 22) & 15] != 0) == !(num & 1)) next = dst; break; /* JMPU */
            default: return PX_PICA_INVALID;
            }
        }
#undef CALL
        pc = next;
    }
}

const char *px_pica_result_string(int r)
{
    switch (r) {
    case PX_PICA_OK: return "success";
    case PX_PICA_TRUNCATED: return "truncated DVLB";
    case PX_PICA_INVALID: return "invalid shader program or access";
    case PX_PICA_LIMIT: return "shader program limit exceeded";
    case PX_PICA_UNSUPPORTED: return "unsupported shader instruction";
    case PX_PICA_STACK: return "shader call stack overflow";
    case PX_PICA_STEPS: return "shader step limit exceeded";
    }
    return "unknown result";
}
