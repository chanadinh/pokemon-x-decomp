/* The PICA200 shader interpreter on hand-assembled programs. */
#include "pica_shader.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static uint8_t blob[8192];
static size_t at;
static void p32(size_t o, uint32_t v) { for (int k = 0; k < 4; ++k) blob[o + k] = (uint8_t)(v >> (8 * k)); }
static void p16(size_t o, uint32_t v) { blob[o] = (uint8_t)v; blob[o + 1] = (uint8_t)(v >> 8); }

/* Operand descriptors: mask (x = 8), swizzles as "xyzw" strings, negations. */
static uint32_t sw(const char *s) { uint32_t v = 0; for (int k = 0; k < 4; ++k) v = v << 2 | (uint32_t)(strchr("xyzw", s[k]) - "xyzw"); return v; }
static uint32_t desc(unsigned mask, const char *a, int na, const char *b, int nb, const char *c, int nc)
{
    return mask | (uint32_t)na << 4 | sw(a) << 5 | (uint32_t)nb << 13 | sw(b) << 14 | (uint32_t)nc << 22 | sw(c) << 23;
}
static uint32_t f1(unsigned op, unsigned dst, unsigned s1, unsigned s2, unsigned idx, unsigned d)
{ return op << 26 | dst << 21 | idx << 19 | s1 << 12 | s2 << 7 | d; }
static uint32_t f1i(unsigned op, unsigned dst, unsigned s1, unsigned s2, unsigned idx, unsigned d)
{ return op << 26 | dst << 21 | idx << 19 | s1 << 14 | s2 << 7 | d; }
static uint32_t mad(unsigned dst, unsigned s1, unsigned s2, unsigned s3, unsigned d) { return 7u << 29 | dst << 24 | s1 << 17 | s2 << 10 | s3 << 5 | d; }
static uint32_t flow(unsigned op, unsigned dst, unsigned num, unsigned extra) { return op << 26 | extra << 22 | dst << 10 | num; }
static uint32_t cmpi(unsigned cx, unsigned cy, unsigned s1, unsigned s2, unsigned d) { return 0x17u << 27 | cx << 24 | cy << 21 | s1 << 12 | s2 << 7 | d; }
static uint32_t cond(unsigned op, unsigned dst, unsigned num, unsigned refx, unsigned refy, unsigned condop)
{ return op << 26 | refx << 25 | refy << 24 | condop << 22 | dst << 10 | num; }

/* A DVLB with one DVLE: code, descriptors, one float constant, one uniform name. */
static size_t build(const uint32_t *code, unsigned n, const uint32_t *ops, unsigned m)
{
    memset(blob, 0, sizeof blob);
    memcpy(blob, "DVLB", 4); p32(4, 1); p32(8, 0x400);
    memcpy(blob + 12, "DVLP", 4); p32(20, 0x40); p32(24, n); p32(28, 0x40 + 4 * n); p32(32, m);
    for (unsigned k = 0; k < n; ++k) p32(12 + 0x40 + 4 * k, code[k]);
    for (unsigned k = 0; k < m; ++k) p32(12 + 0x40 + 4 * n + 8 * k, ops[k]);
    const size_t e = 0x400;
    memcpy(blob + e, "DVLE", 4); p32(e + 8, 0); p32(e + 12, n);
    p32(e + 0x18, 0x40); p32(e + 0x1c, 1);                        /* constants */
    p16(e + 0x40, 2); p16(e + 0x42, 93);
    p32(e + 0x44, 0); p32(e + 0x48, 0x3f0000); p32(e + 0x4c, 0x400000); p32(e + 0x50, 0xc08000); /* 0, 1, 2, -3 */
    p32(e + 0x28, 0x60); p32(e + 0x2c, 1); p16(e + 0x60, 0); p16(e + 0x62, 0); p32(e + 0x64, 0xf);
    p32(e + 0x30, 0x70); p32(e + 0x34, 1); p32(e + 0x70, 0); p16(e + 0x74, 16); p16(e + 0x76, 18);
    p32(e + 0x38, 0x80); p32(e + 0x3c, 8); memcpy(blob + e + 0x80, "WrldMtx", 8);
    at = e + 0x100;
    return at;
}

static void run(const uint32_t *code, unsigned n, const uint32_t *ops, unsigned m, struct px_pica_state *s, int want)
{
    static struct px_dvlb dvlb;
    const size_t size = build(code, n, ops, m);
    assert(px_dvlb_decode(blob, size, &dvlb) == PX_PICA_OK && dvlb.entry_count == 1 && dvlb.code_count == n);
    px_pica_constants(&dvlb.entries[0], s);
    assert(px_pica_run(&dvlb, &dvlb.entries[0], s) == want);
}

int main(void)
{
    assert(px_pica_float24(0x3f0000) == 1.0f && px_pica_float24(0xc08000) == -3.0f && px_pica_float24(0) == 0.0f &&
           px_pica_float24(0x400000) == 2.0f && px_pica_float24(0x3e0000) == 0.5f);
    struct px_pica_state s;
    const uint32_t full = desc(15, "xyzw", 0, "xyzw", 0, "xyzw", 0);
    /* MUL / ADD / MAD with constants c93 = (0, 1, 2, -3) and inputs. */
    {
        memset(&s, 0, sizeof s);
        s.v[0][0] = 1; s.v[0][1] = 2; s.v[0][2] = 3; s.v[0][3] = 4;
        const uint32_t ops[] = {full, desc(8, "wwww", 1, "xyzw", 0, "xyzw", 0), desc(15, "yyyy", 0, "xyzw", 0, "xyzw", 0)};
        const uint32_t code[] = {
            f1(0x08, 0x10, 0x20 + 93, 0x00, 0, 0),       /* r0 = c93 * v0 = (0, 2, 6, -12) */
            f1(0x00, 0x11, 0x20 + 93, 0x10, 0, 0),       /* r1 = c93 + r0 = (0, 3, 8, -15) */
            mad(0x00, 0x11, 0x20 + 93, 0x00, 0),          /* o0 = r1 * c93 + v0 = (1, 5, 19, 49) */
            f1(0x13, 0x01, 0x20 + 93, 0x00, 0, 1),       /* o1.x = -c93.w = 3 */
            f1(0x13, 0x02, 0x20 + 93, 0x00, 0, 2),       /* o2 = c93.y (1) */
            flow(0x22, 0, 0, 0)};
        run(code, 6, ops, 3, &s, PX_PICA_OK);
        assert(s.r[0][1] == 2 && s.r[0][3] == -12 && s.r[1][3] == -15);
        assert(s.o[0][0] == 1 && s.o[0][1] == 5 && s.o[0][2] == 19 && s.o[0][3] == 49);
        assert(s.o[1][0] == 3 && s.o[1][1] == 0); /* mask x only */
        assert(s.o[2][3] == 1);
    }
    /* 0 x inf is 0; DP4, DP3, RCP, RSQ, MAX, MIN, FLR, SGE, SLT, DST. */
    {
        memset(&s, 0, sizeof s);
        s.v[0][0] = INFINITY; s.v[0][1] = 4; s.v[0][2] = -2.5f; s.v[0][3] = 1;
        s.v[1][0] = 0; s.v[1][1] = 1; s.v[1][2] = 2; s.v[1][3] = 3;
        const uint32_t ops[] = {full};
        const uint32_t code[] = {
            f1(0x02, 0x00, 0x00, 0x01, 0, 0),   /* o0 = dp4(v0, v1) = 0 + 4 - 5 + 3 = 2 */
            f1(0x01, 0x01, 0x00, 0x01, 0, 0),   /* o1 = dp3 = -1 */
            f1(0x0e, 0x02, 0x01, 0x00, 0, 0),   /* rcp(v1.x = 0) = inf */
            f1(0x0f, 0x10, 0x00, 0x00, 0, 0),   /* rsq(inf) = 0 */
            f1(0x0c, 0x03, 0x00, 0x01, 0, 0),   /* max */
            f1(0x0d, 0x04, 0x00, 0x01, 0, 0),   /* min */
            f1(0x0b, 0x05, 0x00, 0x00, 0, 0),   /* flr */
            f1(0x09, 0x06, 0x00, 0x01, 0, 0),   /* sge */
            f1(0x0a, 0x07, 0x00, 0x01, 0, 0),   /* slt */
            f1(0x04, 0x08, 0x00, 0x01, 0, 0),   /* dst */
            f1(0x03, 0x09, 0x00, 0x01, 0, 0),   /* dph = 0 + 4 - 5 + 3 */
            f1i(0x1b, 0x0a, 0x01, 0x20 + 93, 0, 0), /* slti(v1, c93): (0 < 0, 1 < 1, 2 < 2, 3 < -3) */
            flow(0x22, 0, 0, 0)};
        run(code, 13, ops, 1, &s, PX_PICA_OK);
        assert(s.o[10][0] == 0 && s.o[10][3] == 0);
        assert(s.o[0][0] == 2 && s.o[1][3] == -1 && isinf(s.o[2][0]) && s.r[0][0] == 0);
        assert(isinf(s.o[3][0]) && s.o[3][1] == 4 && s.o[3][2] == 2 && s.o[4][0] == 0 && s.o[4][2] == -2.5f);
        assert(s.o[5][2] == -3 && s.o[6][0] == 1 && s.o[6][2] == 0 && s.o[7][2] == 1);
        assert(s.o[8][0] == 1 && s.o[8][1] == 4 && s.o[8][2] == -2.5f && s.o[8][3] == 3 && s.o[9][0] == 2);
    }
    /* MOVA and relative uniforms; CALL; IFU both ways; JMPU. */
    {
        memset(&s, 0, sizeof s);
        for (int k = 0; k < 4; ++k) { s.c[10][k] = 10; s.c[12][k] = 12; }
        s.v[0][0] = 2; s.v[0][1] = 1;
        s.b[3] = 1;
        const uint32_t ops[] = {full, desc(12, "xyzw", 0, "xyzw", 0, "xyzw", 0)};
        const uint32_t code[] = {
            /* 0 */ f1(0x12, 0, 0x00, 0x00, 0, 1),            /* a0.x = 2, a0.y = 1 */
            /* 1 */ f1(0x13, 0x00, 0x20 + 10, 0x00, 1, 0),    /* o0 = c[10 + a0.x] = 12 */
            /* 2 */ flow(0x24, 9, 2, 0),                       /* call 9..10 */
            /* 3 */ flow(0x27, 5, 1, 3),                       /* ifu b3: 4, else 5 */
            /* 4 */ f1(0x13, 0x02, 0x20 + 12, 0x00, 0, 0),    /* o2 = 12 (taken) */
            /* 5 */ f1(0x13, 0x03, 0x20 + 10, 0x00, 0, 0),    /* else: o3 = 10 (skipped) */
            /* 6 */ flow(0x27, 8, 1, 4),                       /* ifu b4 (false): run 8 */
            /* 7 */ f1(0x13, 0x04, 0x20 + 12, 0x00, 0, 0),    /* skipped */
            /* 8 */ flow(0x2d, 11, 0, 3),                      /* else block: jmpu b3 to 11 */
            /* 9 */ f1(0x13, 0x01, 0x20 + 12, 0x00, 0, 0),    /* the called routine: o1 = 12 */
            /* 10 */ f1(0x13, 0x05, 0x20 + 10, 0x00, 2, 0),   /* o5 = c[10 + a0.y] = c11 (0) */
            /* 11 */ flow(0x22, 0, 0, 0)};
        run(code, 12, ops, 2, &s, PX_PICA_OK);
        assert(s.a[0] == 2 && s.a[1] == 1 && s.o[0][0] == 12 && s.o[1][0] == 12 && s.o[2][0] == 12);
        assert(s.o[3][0] == 0 && s.o[4][0] == 0 && s.o[5][0] == 0);
    }
    /* LOOP with aL, CMP, JMPC and BREAKC. */
    {
        memset(&s, 0, sizeof s);
        for (int u = 0; u < 8; ++u) for (int k = 0; k < 4; ++k) s.c[20 + u][k] = (float)(u + 1);
        s.i[0][0] = 4; s.i[0][1] = 0; s.i[0][2] = 1; /* 5 iterations, aL 0, 1, ... */
        s.v[1][0] = 3.5f;
        const uint32_t ops[] = {full};
        const uint32_t code[] = {
            /* 0 */ f1(0x13, 0x10, 0x20 + 93, 0x00, 0, 0),       /* r0 = c93: r0.x = 0 */
            /* 1 */ flow(0x29, 2, 0, 0),                          /* loop i0 over 2 */
            /* 2 */ f1(0x00, 0x10, 0x20 + 20, 0x10, 3, 0),       /* r0 += c[20 + aL] */
            /* 3 */ f1(0x13, 0x00, 0x10, 0x00, 0, 0),            /* o0 = r0: 1+2+3+4+5 = 15 */
            /* 4 */ cmpi(4, 4, 0x10, 0x01, 0),                    /* cmp r0 > v1 (x and y) */
            /* 5 */ cond(0x2c, 7, 0, 1, 1, 2),                    /* jmpc x: to 7 */
            /* 6 */ f1(0x13, 0x01, 0x20 + 20, 0x00, 0, 0),       /* skipped */
            /* 7 */ f1(0x13, 0x11, 0x20 + 93, 0x00, 0, 0),       /* r1 = 0 */
            /* 8 */ flow(0x29, 10, 0, 0),                         /* loop again, break at r1 > v1 */
            /* 9 */ f1(0x00, 0x11, 0x20 + 20, 0x11, 3, 0),       /* r1 += c[20 + aL] */
            /* 10 */ cmpi(4, 4, 0x11, 0x01, 0),
            /* 11 */ flow(0x22, 0, 0, 0),
        };
        /* 10 must be a BREAKC: patch it in after building the comparison at 9.5 */
        uint32_t c2[14];
        memcpy(c2, code, sizeof code);
        c2[8] = flow(0x29, 10, 0, 0);
        c2[10] = cmpi(4, 4, 0x11, 0x01, 0);
        c2[11] = cond(0x23, 0, 0, 1, 1, 2);                         /* breakc x */
        c2[10] = cmpi(4, 4, 0x11, 0x01, 0);
        /* loop body 9..11: add, cmp, breakc; then end */
        c2[8] = flow(0x29, 11, 0, 0);
        c2[12] = f1(0x13, 0x02, 0x11, 0x00, 0, 0);                 /* o2 = r1 */
        c2[13] = flow(0x22, 0, 0, 0);
        run(c2, 14, ops, 1, &s, PX_PICA_OK);
        assert(s.o[0][0] == 15 && s.o[1][0] == 0);
        assert(s.o[2][0] == 6); /* 1 + 2 + 3 > 3.5 breaks after the third pass */
    }
    /* Errors: LITP unsupported, a uniform index out of range, the step limit. */
    {
        memset(&s, 0, sizeof s);
        const uint32_t ops[] = {full};
        const uint32_t litp[] = {f1(0x07, 0, 0, 0, 0, 0), flow(0x22, 0, 0, 0)};
        run(litp, 2, ops, 1, &s, PX_PICA_UNSUPPORTED);
        s.a[0] = 50;
        const uint32_t far[] = {f1(0x13, 0x00, 0x20 + 90, 0, 1, 0), flow(0x22, 0, 0, 0)};
        run(far, 2, ops, 1, &s, PX_PICA_INVALID);
        const uint32_t spin[] = {flow(0x2d, 0, 1, 0), flow(0x22, 0, 0, 0)}; /* jmpu !b0 to 0 forever */
        memset(&s, 0, sizeof s);
        run(spin, 2, ops, 1, &s, PX_PICA_STEPS);
    }
    /* Decoding: names, outputs, constants; truncation. */
    {
        static struct px_dvlb d;
        const uint32_t ops[] = {full}, code[] = {flow(0x22, 0, 0, 0)};
        const size_t size = build(code, 1, ops, 1);
        assert(px_dvlb_decode(blob, size, &d) == PX_PICA_OK);
        assert(px_pica_uniform(&d.entries[0], "WrldMtx") == 16 && px_pica_uniform(&d.entries[0], "ProjMtx") == -1);
        assert(d.entries[0].output_count == 1 && d.entries[0].outputs[0].mask == 0xf && d.entries[0].constant_count == 1);
        assert(px_dvlb_decode(blob, 0x20, &d) == PX_PICA_TRUNCATED);
        blob[0] = 'X';
        assert(px_dvlb_decode(blob, size, &d) == PX_PICA_INVALID);
    }
    assert(!strcmp(px_pica_result_string(PX_PICA_STEPS), "shader step limit exceeded"));
    puts("PICA shader tests passed");
    return 0;
}
