/* Surveys DVLB shader containers: decodes each file, lists the opcodes its
 * program uses, and runs every entry point under every combination of the
 * bool uniforms it names (identity matrices in the named matrix uniforms,
 * a fixed vertex), reporting the result and step count. Used to check that
 * the software PICA interpreter covers every shader the game's models use. */
#include "pica_shader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t *slurp(const char *path, size_t *size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    const long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *d = n > 0 ? malloc((size_t)n) : NULL;
    if (d && fread(d, 1, (size_t)n, f) != (size_t)n) { free(d); d = NULL; }
    fclose(f);
    *size = d ? (size_t)n : 0;
    return d;
}

static unsigned opcode(uint32_t w)
{
    const unsigned op = w >> 26;
    return op >= 0x30 ? (op & 0x38) : op; /* MAD/MADI and CMP share their top bits */
}

int main(int argc, char **argv)
{
    int failures = 0;
    static struct px_dvlb program;
    for (int f = 1; f < argc; ++f) {
        size_t size;
        uint8_t *data = slurp(argv[f], &size);
        if (!data) { printf("file=%s error=unreadable\n", argv[f]); ++failures; continue; }
        const int decoded = px_dvlb_decode(data, size, &program);
        free(data);
        if (decoded) { printf("file=%s error=%s\n", argv[f], px_pica_result_string(decoded)); ++failures; continue; }
        unsigned used[64] = {0};
        for (uint32_t k = 0; k < program.code_count; ++k) used[opcode(program.code[k])]++;
        printf("file=%s instructions=%u descriptors=%u entries=%u opcodes=", argv[f], program.code_count,
               program.op_count, program.entry_count);
        for (unsigned k = 0, first = 1; k < 64; ++k)
            if (used[k]) { printf("%s%02x", first ? "" : ",", k); first = 0; }
        printf("\n");
        for (uint32_t e = 0; e < program.entry_count; ++e) {
            const struct px_pica_entry *entry = &program.entries[e];
            int bools[16], bool_count = 0;
            for (uint32_t u = 0; u < entry->uniform_count; ++u)
                for (unsigned r = entry->uniforms[u].first; r <= entry->uniforms[u].last && bool_count < 16; ++r)
                    if (r >= 120 && r < 136) bools[bool_count++] = (int)r - 120;
            const int combos = bool_count > 8 ? 256 : 1 << bool_count;
            unsigned ok = 0, max_steps = 0;
            int worst = PX_PICA_OK;
            for (int c = 0; c < combos; ++c) {
                struct px_pica_state s;
                memset(&s, 0, sizeof s);
                for (unsigned r = 0; r < 96; ++r) s.c[r][r % 4] = 1.0f; /* rows of identity matrices */
                px_pica_constants(entry, &s);
                for (unsigned v = 0; v < 16; ++v) {
                    s.v[v][0] = 0.25f; s.v[v][1] = 0.5f; s.v[v][2] = 0.75f; s.v[v][3] = 1.0f;
                }
                for (int b = 0; b < bool_count; ++b) s.b[bools[b]] = (uint8_t)(c >> b & 1);
                const int r = px_pica_run(&program, entry, &s);
                if (r == PX_PICA_OK) ++ok; else worst = r;
                if (s.steps > max_steps) max_steps = s.steps;
            }
            printf("  entry=%u type=%u main=%u end=%u outputs=%u uniforms=%u bools=%d runs=%d ok=%u max_steps=%u%s%s\n",
                   e, entry->type, entry->main, entry->end, entry->output_count, entry->uniform_count, bool_count,
                   combos, ok, max_steps, worst ? " error=" : "", worst ? px_pica_result_string(worst) : "");
            if (entry->type == 0 && ok != (unsigned)combos) ++failures;
        }
    }
    printf("probe failures=%d\n", failures);
    return failures != 0;
}
