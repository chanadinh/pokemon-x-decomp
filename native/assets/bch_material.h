#ifndef PX_BCH_MATERIAL_H
#define PX_BCH_MATERIAL_H

#include "bch_model.h"

/* A bounded, unlit preview interpretation of one decoded BCH material.
 *
 * Register semantics follow the pinned CTRMap PICACommandReader/H3DMaterial
 * sources recorded with the reproduction evidence; nothing here is recovered
 * from the original executable. The preview evaluates only TEV stage 0, with
 * primary and fragment-lighting colors treated as white. The sampled unit is
 * the one that stage 0 names as its first RGB and alpha source; this is the
 * material's own combiner selection, not a fallback. Other enabled units and
 * stages 1..5 are reported as not executed. Culling is reported, not executed.
 * Enumerations keep PICA numbering:
 *   wrap: clamp-edge0, clamp-border1, repeat2, mirror3;
 *   compare: never0, always1, equal2, notequal3, less4, lequal5, greater6,
 *            gequal7;
 *   blend factor: zero0, one1, source-alpha6, one-minus-source-alpha7. */
enum px_bch_material_result {
    PX_BCH_MATERIAL_OK = 0,
    PX_BCH_MATERIAL_INVALID_ARGUMENT,
    PX_BCH_MATERIAL_NO_PARAMETERS,
    PX_BCH_MATERIAL_UNKNOWN_STATE,
    PX_BCH_MATERIAL_UNSUPPORTED_COMBINER,
    PX_BCH_MATERIAL_UNSUPPORTED_COORDINATES,
    PX_BCH_MATERIAL_UNSUPPORTED_SAMPLER,
    PX_BCH_MATERIAL_UNSUPPORTED_FRAGMENT
};

enum px_bch_preview_blend {
    PX_BCH_BLEND_REPLACE = 0,     /* one, zero */
    PX_BCH_BLEND_SOURCE_ALPHA,    /* source alpha, one minus source alpha */
    PX_BCH_BLEND_ADD_SOURCE_ALPHA /* source alpha, one */
};

struct px_bch_preview_material {
    uint32_t unit;             /* 0..2: texture unit sampled by TEV stage 0 */
    uint32_t combine_rgb, combine_alpha; /* 0 replace or 1 modulate */
    uint32_t wrap_u, wrap_v;   /* PICA wrap: 0 or 2 or 3 */
    uint32_t mag_linear, min_linear;
    uint32_t alpha_test, alpha_compare, alpha_reference;
    uint32_t depth_test, depth_compare, depth_write, color_write_mask;
    uint32_t cull;             /* register 0x40 bits 0..1, metadata only */
    enum px_bch_preview_blend blend;
    uint32_t unexecuted_units; /* enabled units other than the sampled unit */
};

/* Fails without partial output when any required register byte is unknown or
 * any state lies outside the subset above. The selected unit must be enabled,
 * have a present texture name, a 2D sampler type, ordinary UV mapping (mode 0)
 * and an identity [1,1,0,0,0] coordinator. Blending must use the add equation
 * with matching RGB/alpha factor pairs; logic operations are rejected. */
enum px_bch_material_result px_bch_material_preview(
    const struct px_bch_material *material, struct px_bch_preview_material *out);
const char *px_bch_material_result_string(enum px_bch_material_result result);

#endif
