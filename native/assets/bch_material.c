#include "bch_material.h"

#include <string.h>

static int known(const struct px_bch_register *reg, uint32_t mask)
{
    return (reg->known_mask & mask) == mask;
}

enum px_bch_material_result px_bch_material_preview(
    const struct px_bch_material *m, struct px_bch_preview_material *out)
{
    if (m == NULL || out == NULL) return PX_BCH_MATERIAL_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    if (!m->parameters_present) return PX_BCH_MATERIAL_NO_PARAMETERS;
    const struct px_bch_fragment_state *f = &m->fragment;
    const struct px_bch_register *stage = f->tev[0];
    for (unsigned j = 0; j < 5; ++j)
        if (j != 3 && !known(&stage[j], UINT32_MAX)) return PX_BCH_MATERIAL_UNKNOWN_STATE;
    if (!known(&f->cull, 0x3) || !known(&f->color_operation, 0x303) ||
        !known(&f->blend, UINT32_MAX) || !known(&f->alpha_test, 0xff71) ||
        !known(&f->depth_color, 0x1f71)) return PX_BCH_MATERIAL_UNKNOWN_STATE;

    /* Stage 0 only: source0 must be one texture for both RGB and alpha; an
     * optional modulating source1 is primary (0) or fragment primary (1),
     * both white in this unlit preview. Operands select plain color/alpha. */
    const uint32_t source = stage[0].value, operand = stage[1].value;
    const uint32_t combine = stage[2].value, scale = stage[4].value;
    const uint32_t rgb0 = source & 15, rgb1 = (source >> 4) & 15;
    const uint32_t alpha0 = (source >> 16) & 15, alpha1 = (source >> 20) & 15;
    const uint32_t combine_rgb = combine & 0xffff, combine_alpha = combine >> 16;
    if (rgb0 < 3 || rgb0 > 5 || alpha0 != rgb0 || combine_rgb > 1 || combine_alpha > 1 ||
        (combine_rgb == 1 && rgb1 > 1) || (combine_alpha == 1 && alpha1 > 1) ||
        (operand & 0xff) || ((operand >> 12) & 0xff) || (scale & 0x30003))
        return PX_BCH_MATERIAL_UNSUPPORTED_COMBINER;
    const uint32_t unit = rgb0 - 3;
    if (!((m->texture_unit_config >> unit) & 1) || !m->texture_names[unit].present)
        return PX_BCH_MATERIAL_UNSUPPORTED_COMBINER;

    const struct px_bch_coordinator *c = &m->coordinators[unit];
    if (((c->packed >> 16) & 255) != 0 || c->scale[0] != 1 || c->scale[1] != 1 ||
        c->rotation != 0 || c->translation[0] != 0 || c->translation[1] != 0)
        return PX_BCH_MATERIAL_UNSUPPORTED_COORDINATES;

    const struct px_bch_register *sampler = &m->unit_parameters[unit];
    if (!known(sampler, UINT32_MAX)) return PX_BCH_MATERIAL_UNKNOWN_STATE;
    const uint32_t wrap_v = (sampler->value >> 8) & 7, wrap_u = (sampler->value >> 12) & 7;
    if (wrap_u > 3 || wrap_v > 3 || wrap_u == 1 || wrap_v == 1 ||
        ((sampler->value >> 28) & 7)) return PX_BCH_MATERIAL_UNSUPPORTED_SAMPLER;

    /* Fragment operation: default mode with blending, not a logic op. */
    if ((f->color_operation.value & 3) || ((f->color_operation.value >> 8) & 3) != 1)
        return PX_BCH_MATERIAL_UNSUPPORTED_FRAGMENT;
    const uint32_t blend = f->blend.value;
    const uint32_t src_rgb = (blend >> 16) & 15, dst_rgb = (blend >> 20) & 15;
    const uint32_t src_alpha = (blend >> 24) & 15, dst_alpha = blend >> 28;
    if ((blend & 0xffff) || src_rgb != src_alpha || dst_rgb != dst_alpha)
        return PX_BCH_MATERIAL_UNSUPPORTED_FRAGMENT;
    if (src_rgb == 1 && dst_rgb == 0) out->blend = PX_BCH_BLEND_REPLACE;
    else if (src_rgb == 6 && dst_rgb == 7) out->blend = PX_BCH_BLEND_SOURCE_ALPHA;
    else if (src_rgb == 6 && dst_rgb == 1) out->blend = PX_BCH_BLEND_ADD_SOURCE_ALPHA;
    else return PX_BCH_MATERIAL_UNSUPPORTED_FRAGMENT;

    out->unit = unit;
    out->combine_rgb = combine_rgb;
    out->combine_alpha = combine_alpha;
    out->wrap_u = wrap_u;
    out->wrap_v = wrap_v;
    out->mag_linear = (sampler->value >> 1) & 1;
    out->min_linear = (sampler->value >> 2) & 1;
    out->alpha_test = f->alpha_test.value & 1;
    out->alpha_compare = (f->alpha_test.value >> 4) & 7;
    out->alpha_reference = (f->alpha_test.value >> 8) & 255;
    out->depth_test = f->depth_color.value & 1;
    out->depth_compare = (f->depth_color.value >> 4) & 7;
    out->color_write_mask = (f->depth_color.value >> 8) & 15;
    out->depth_write = (f->depth_color.value >> 12) & 1;
    out->cull = f->cull.value & 3;
    out->unexecuted_units = m->texture_unit_config & 7 & ~(UINT32_C(1) << unit);
    return PX_BCH_MATERIAL_OK;
}

const char *px_bch_material_result_string(enum px_bch_material_result result)
{
    switch (result) {
    case PX_BCH_MATERIAL_OK: return "success";
    case PX_BCH_MATERIAL_INVALID_ARGUMENT: return "invalid BCH material argument";
    case PX_BCH_MATERIAL_NO_PARAMETERS: return "BCH material lacks fragment parameter metadata";
    case PX_BCH_MATERIAL_UNKNOWN_STATE: return "required BCH material register bits are unknown";
    case PX_BCH_MATERIAL_UNSUPPORTED_COMBINER: return "unsupported BCH TEV stage 0 or texture unit";
    case PX_BCH_MATERIAL_UNSUPPORTED_COORDINATES: return "unsupported BCH texture coordinator";
    case PX_BCH_MATERIAL_UNSUPPORTED_SAMPLER: return "unsupported BCH texture sampler";
    case PX_BCH_MATERIAL_UNSUPPORTED_FRAGMENT: return "unsupported BCH blend or fragment operation";
    }
    return "unknown BCH material result";
}
