/* Bridge for verify_fragment_lighting.py: decodes one skinned BCH model and
 * prints its LUT samplers (table set, sampler, flags, entry checksums) and
 * each material's LUT registers and slot names. */
#include "../../assets/bch_model.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    FILE *f = fopen(argv[1], "rb");
    if (!f) return 2;
    fseek(f, 0, SEEK_END);
    const long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char *d = malloc(n > 0 ? (size_t)n : 1);
    if (!d || fread(d, 1, (size_t)n, f) != (size_t)n) return 2;
    fclose(f);
    struct px_bch_model m;
    const enum px_bch_model_result r = px_bch_model_decode_skinned(d, (size_t)n, 0, &m);
    if (r != PX_BCH_MODEL_OK) { printf("error %s\n", px_bch_model_result_string(r)); return 1; }
    for (uint32_t i = 0; i < m.lut_count; ++i) {
        const struct px_bch_lut *l = &m.luts[i];
        uint64_t sum = 0, mix = 1469598103934665603ull;
        for (unsigned k = 0; k < 256; ++k) { sum += l->entries[k]; mix = (mix ^ l->entries[k]) * 1099511628211ull; }
        printf("lut %s %s flags=%u sum=%llu fnv=%016llx\n", l->table.bytes, l->sampler.bytes, l->flags,
               (unsigned long long)sum, (unsigned long long)mix);
    }
    for (uint32_t i = 0; i < m.material_count; ++i) {
        const struct px_bch_material *x = &m.materials[i];
        printf("material %s frag=%u bump=%u/%u fresnel=%u known=%u regs=%x,%x,%x", x->name.bytes, x->fragment_flags,
               x->bump_mode, x->bump_texture, x->fresnel_selector, x->lut_known, x->lut_registers[0],
               x->lut_registers[1], x->lut_registers[2]);
        for (unsigned k = 0; k < 6; ++k)
            printf(" %s/%s", x->lut_table[k].present ? x->lut_table[k].bytes : "-",
                   x->lut_sampler[k].present ? x->lut_sampler[k].bytes : "-");
        printf("\n");
    }
    px_bch_model_free(&m);
    free(d);
    return 0;
}
