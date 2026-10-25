#ifndef PX_PICA_SHADER_H
#define PX_PICA_SHADER_H
#include <stddef.h>
#include <stdint.h>

/* The PICA200 vertex shader, run in software: DVLB containers (the shared
 * program DVLP and its entry points DVLE, with output maps, constants and
 * named uniforms) and an interpreter for the shader instruction set as
 * 3dbrew documents it (arithmetic, comparisons, MOVA, conditional and
 * counted flow control with a call stack). Values are float32; PICA float24
 * constants are widened exactly; multiplication treats 0 x anything as 0,
 * as the hardware does (Citra's documented behaviour). Geometry-shader
 * instructions (EMIT, SETEMIT) and LITP stop the run as unsupported. */
enum { PX_PICA_MAX_CODE = 4096, PX_PICA_MAX_OPS = 128, PX_PICA_MAX_ENTRIES = 16, PX_PICA_MAX_UNIFORMS = 96 };
struct px_pica_output { uint16_t type, reg; uint32_t mask; };
struct px_pica_constant { uint16_t type, reg; uint32_t value[4]; }; /* type 0 bool, 1 int, 2 float24 x4 */
struct px_pica_uniform { char name[48]; uint16_t first, last; }; /* registers: 0-15 v, 16-111 c0-c95, 112-115 i, 120-135 b */
struct px_pica_entry {
    uint16_t type; uint32_t main, end;
    uint16_t input_mask, output_mask;
    struct px_pica_output outputs[16]; uint32_t output_count;
    struct px_pica_constant constants[96]; uint32_t constant_count;
    struct px_pica_uniform uniforms[PX_PICA_MAX_UNIFORMS]; uint32_t uniform_count;
};
struct px_dvlb {
    uint32_t code[PX_PICA_MAX_CODE]; uint32_t code_count;
    uint32_t ops[PX_PICA_MAX_OPS]; uint32_t op_count;
    struct px_pica_entry entries[PX_PICA_MAX_ENTRIES]; uint32_t entry_count;
};
enum px_pica_result {
    PX_PICA_OK = 0, PX_PICA_TRUNCATED, PX_PICA_INVALID, PX_PICA_LIMIT, PX_PICA_UNSUPPORTED, PX_PICA_STACK,
    PX_PICA_STEPS
};
/* Decodes a DVLB (at the start of data). */
int px_dvlb_decode(const uint8_t *data, size_t size, struct px_dvlb *out);
/* A uniform's first register by name (-1 when absent). */
int px_pica_uniform(const struct px_pica_entry *entry, const char *name);

struct px_pica_state {
    float v[16][4], o[16][4], r[16][4], c[96][4];
    uint8_t i[4][4];     /* integer uniforms: x loop count - 1, y start, z increment */
    uint8_t b[16];       /* bool uniforms */
    int32_t a[2], aL;    /* address registers */
    uint8_t cmp[2];
    uint32_t steps;
};
/* Loads the entry's constants into the state (as the game's loader does
 * before a draw), leaving other uniforms as they are. */
void px_pica_constants(const struct px_pica_entry *entry, struct px_pica_state *state);
/* Runs from the entry's main until END. */
int px_pica_run(const struct px_dvlb *program, const struct px_pica_entry *entry, struct px_pica_state *state);
float px_pica_float24(uint32_t bits);
const char *px_pica_result_string(int result);
#endif
