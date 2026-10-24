#include "bch_material.h"

#include <math.h>

#include <string.h>

static int known(const struct px_bch_register *reg, uint32_t mask)
{
    return (reg->known_mask & mask) == mask;
}

/* Fragment operation: default mode with blending, not a logic op; the add
 * equation with matching RGB/alpha factor pairs. Callers check known bits. */
static int fragment(const struct px_bch_fragment_state *f, struct px_bch_fragment_preview *out)
{
    memset(out, 0, sizeof *out);
    if ((f->color_operation.value & 3) || ((f->color_operation.value >> 8) & 3) != 1) return 0;
    const uint32_t blend = f->blend.value;
    const uint32_t src_rgb = (blend >> 16) & 15, dst_rgb = (blend >> 20) & 15;
    const uint32_t src_alpha = (blend >> 24) & 15, dst_alpha = blend >> 28;
    if ((blend & 0xffff) || src_rgb != src_alpha || dst_rgb != dst_alpha) return 0;
    if (src_rgb == 1 && dst_rgb == 0) out->blend = PX_BCH_BLEND_REPLACE;
    else if (src_rgb == 6 && dst_rgb == 7) out->blend = PX_BCH_BLEND_SOURCE_ALPHA;
    else if (src_rgb == 6 && dst_rgb == 1) out->blend = PX_BCH_BLEND_ADD_SOURCE_ALPHA;
    else return 0;
    out->alpha_test = f->alpha_test.value & 1;
    out->alpha_compare = (f->alpha_test.value >> 4) & 7;
    out->alpha_reference = (f->alpha_test.value >> 8) & 255;
    out->depth_test = f->depth_color.value & 1;
    out->depth_compare = (f->depth_color.value >> 4) & 7;
    out->color_write_mask = (f->depth_color.value >> 8) & 15;
    out->depth_write = (f->depth_color.value >> 12) & 1;
    out->cull = f->cull.value & 3;
    return 1;
}

static int fragment_known(const struct px_bch_fragment_state *f)
{
    return known(&f->cull, 0x3) && known(&f->color_operation, 0x303) &&
        known(&f->blend, UINT32_MAX) && known(&f->alpha_test, 0xff71) &&
        known(&f->depth_color, 0x1f71);
}

enum px_bch_material_result px_bch_material_combiners(
    const struct px_bch_material *m, struct px_bch_combiner_material *out)
{
    if (m == NULL || out == NULL) return PX_BCH_MATERIAL_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    if (!m->parameters_present) return PX_BCH_MATERIAL_NO_PARAMETERS;
    const struct px_bch_fragment_state *f = &m->fragment;
    struct px_tev_config tev;
    memset(&tev, 0, sizeof tev);
    for (unsigned i = 0; i < 6; ++i) {
        uint32_t *fields[5] = {&tev.stages[i].source, &tev.stages[i].operand,
                               &tev.stages[i].combine, &tev.stages[i].constant, &tev.stages[i].scale};
        for (unsigned j = 0; j < 5; ++j) {
            if (!known(&f->tev[i][j], UINT32_MAX)) return PX_BCH_MATERIAL_UNKNOWN_STATE;
            *fields[j] = f->tev[i][j].value;
        }
    }
    if (!known(&f->tev_buffer, 0xff00) || !known(&f->buffer_color, UINT32_MAX) ||
        !fragment_known(f)) return PX_BCH_MATERIAL_UNKNOWN_STATE;
    tev.buffer_update = f->tev_buffer.value & 0xff00;
    tev.buffer_color = f->buffer_color.value;
    if (!px_tev_valid(&tev)) return PX_BCH_MATERIAL_UNSUPPORTED_COMBINER;
    const uint32_t sources = px_tev_sources(&tev);
    if (sources & (UINT32_C(1) << PX_TEV_TEXTURE3)) return PX_BCH_MATERIAL_UNSUPPORTED_COMBINER;
    struct px_bch_combiner_material result;
    memset(&result, 0, sizeof result);
    for (unsigned unit = 0; unit < 3; ++unit) {
        if (!(sources & (UINT32_C(1) << (PX_TEV_TEXTURE0 + unit)))) continue;
        if (!((m->texture_unit_config >> unit) & 1)) { result.disabled_units |= 1u << unit; continue; }
        if (!m->texture_names[unit].present) return PX_BCH_MATERIAL_UNSUPPORTED_COMBINER;
        const struct px_bch_coordinator *c = &m->coordinators[unit];
        const uint32_t mapping = (c->packed >> 16) & 255;
        if (mapping == 3) { result.projected_units |= 1u << unit; continue; }
        if (mapping != 0) return PX_BCH_MATERIAL_UNSUPPORTED_COORDINATES;
        if (c->scale[0] != 1 || c->scale[1] != 1 || c->rotation != 0 ||
            c->translation[0] != 0 || c->translation[1] != 0) {
            const uint32_t type = (c->packed >> 8) & 255;
            if (type > 2 || !isfinite(c->scale[0]) || !isfinite(c->scale[1]) || !isfinite(c->rotation) ||
                !isfinite(c->translation[0]) || !isfinite(c->translation[1]))
                return PX_BCH_MATERIAL_UNSUPPORTED_COORDINATES;
            const float sx = c->scale[0], sy = c->scale[1], tx = c->translation[0], ty = c->translation[1];
            const float ca = (float)cos(c->rotation), sa = (float)sin(c->rotation);
            float *mt = result.uv_matrix[unit];
            /* SPICA's Matrix3x4: u' = M11 u + M21 v + M41, v' = M12 u + M22 v + M42. */
            mt[0] = sx * ca; mt[1] = sx * -sa;
            mt[3] = sy * sa; mt[4] = sy * ca;
            if (type == 0) {
                mt[2] = sx * ((0.5f * sa - 0.5f * ca) + 0.5f - tx);
                mt[5] = sy * ((0.5f * -sa - 0.5f * ca) + 0.5f - ty);
            } else if (type == 1) {
                mt[2] = sx * (-ca * tx - sa * ty);
                mt[5] = sy * (sa * tx - ca * ty);
            } else {
                mt[2] = sx * ca * (-tx - 0.5f) - sx * sa * (ty - 0.5f) + 0.5f;
                mt[5] = sy * sa * (-tx - 0.5f) + sy * ca * (ty - 0.5f) + 0.5f;
            }
            result.transformed_units |= 1u << unit;
        }
        const struct px_bch_register *sampler = &m->unit_parameters[unit];
        if (!known(sampler, UINT32_MAX)) return PX_BCH_MATERIAL_UNKNOWN_STATE;
        const uint32_t wrap_v = (sampler->value >> 8) & 7, wrap_u = (sampler->value >> 12) & 7;
        if (wrap_u > 3 || wrap_v > 3 || wrap_u == 1 || wrap_v == 1 || ((sampler->value >> 28) & 7))
            return PX_BCH_MATERIAL_UNSUPPORTED_SAMPLER;
        result.sampled_units |= 1u << unit;
        result.wrap_u[unit] = wrap_u;
        result.wrap_v[unit] = wrap_v;
        result.mag_linear[unit] = (sampler->value >> 1) & 1;
        result.min_linear[unit] = (sampler->value >> 2) & 1;
    }
    if (!fragment(f, &result.fragment)) return PX_BCH_MATERIAL_UNSUPPORTED_FRAGMENT;
    result.tev = tev;
    result.sources = sources;
    *out = result;
    return PX_BCH_MATERIAL_OK;
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

    struct px_bch_fragment_preview frag;
    if (!fragment(f, &frag)) return PX_BCH_MATERIAL_UNSUPPORTED_FRAGMENT;
    out->blend = frag.blend;
    out->unit = unit;
    out->combine_rgb = combine_rgb;
    out->combine_alpha = combine_alpha;
    out->wrap_u = wrap_u;
    out->wrap_v = wrap_v;
    out->mag_linear = (sampler->value >> 1) & 1;
    out->min_linear = (sampler->value >> 2) & 1;
    out->alpha_test = frag.alpha_test;
    out->alpha_compare = frag.alpha_compare;
    out->alpha_reference = frag.alpha_reference;
    out->depth_test = frag.depth_test;
    out->depth_compare = frag.depth_compare;
    out->color_write_mask = frag.color_write_mask;
    out->depth_write = frag.depth_write;
    out->cull = frag.cull;
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
