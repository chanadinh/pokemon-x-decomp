#include "bch_material.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

/* Hand-authored register states. Values follow the pinned CTRMap layouts and
 * are deliberately asymmetric so swapped fields or units cannot pass. */
static struct px_bch_register full(uint32_t value)
{
    struct px_bch_register r = {value, UINT32_MAX};
    return r;
}
static struct px_bch_material ordinary(void)
{
    struct px_bch_material m;
    memset(&m, 0, sizeof m);
    memcpy(m.texture_names[0].bytes, "base", 5);
    m.texture_names[0].size = 4; m.texture_names[0].present = 1;
    m.texture_unit_config = 0x11001;
    m.parameters_present = 1;
    for (unsigned unit = 0; unit < 3; ++unit) {
        m.coordinators[unit].packed = 0xff000001;
        m.coordinators[unit].scale[0] = m.coordinators[unit].scale[1] = 1;
        m.unit_parameters[unit] = full(0x2220);
    }
    /* Unit 0: wrap U mirror (bits 12..14), V clamp-edge, linear min/mag. */
    m.unit_parameters[0] = full(0x3006);
    m.fragment.cull = full(2);
    m.fragment.color_operation = full(0x100);
    m.fragment.blend = full(0x01010000);
    m.fragment.alpha_test = full(0x2a61);
    m.fragment.depth_color = full(0x1b41);
    const uint32_t stage0[5] = {0x00030003, 0, 0x00000001, 0xff000000, 0};
    for (unsigned j = 0; j < 5; ++j) m.fragment.tev[0][j] = full(stage0[j]);
    return m;
}
/* Projection on unit 0, ordinary base texture on unit 1 named by stage 0. */
static struct px_bch_material projected(void)
{
    struct px_bch_material m = ordinary();
    memcpy(m.texture_names[0].bytes, "projection", 11); m.texture_names[0].size = 10;
    memcpy(m.texture_names[1].bytes, "floor", 6);
    m.texture_names[1].size = 5; m.texture_names[1].present = 1;
    m.texture_unit_config = 0x11003;
    m.coordinators[0].packed = 0x00030001;
    m.unit_parameters[0] = full(0x30002220);
    m.unit_parameters[1] = full(0x2220);
    m.fragment.tev[0][0] = full(0x00040014); /* RGB tex1*fragment primary; A tex1*primary */
    m.fragment.tev[0][2] = full(0x00010001);
    return m;
}
static void expect(struct px_bch_material m, enum px_bch_material_result result)
{
    struct px_bch_preview_material out;
    memset(&out, 0xff, sizeof out);
    assert(px_bch_material_preview(&m, &out) == result);
    const struct px_bch_preview_material empty = {0};
    assert(memcmp(&out, &empty, sizeof out) == 0);
}
static void test_ordinary_state(void)
{
    struct px_bch_material m = ordinary();
    struct px_bch_preview_material out;
    assert(px_bch_material_preview(&m, &out) == PX_BCH_MATERIAL_OK);
    assert(out.unit == 0 && out.combine_rgb == 1 && out.combine_alpha == 0);
    assert(out.wrap_u == 3 && out.wrap_v == 0);
    assert(out.mag_linear == 1 && out.min_linear == 1);
    assert(out.alpha_test == 1 && out.alpha_compare == 6 && out.alpha_reference == 0x2a);
    assert(out.depth_test == 1 && out.depth_compare == 4);
    assert(out.color_write_mask == 0xb && out.depth_write == 1);
    assert(out.cull == 2 && out.blend == PX_BCH_BLEND_REPLACE && out.unexecuted_units == 0);
    m.fragment.blend = full(0x76760000);
    assert(px_bch_material_preview(&m, &out) == PX_BCH_MATERIAL_OK && out.blend == PX_BCH_BLEND_SOURCE_ALPHA);
    m.fragment.blend = full(0x16160000);
    assert(px_bch_material_preview(&m, &out) == PX_BCH_MATERIAL_OK && out.blend == PX_BCH_BLEND_ADD_SOURCE_ALPHA);
    m.fragment.alpha_test = full(0x10);
    m.fragment.depth_color = full(0x0f50);
    assert(px_bch_material_preview(&m, &out) == PX_BCH_MATERIAL_OK);
    assert(!out.alpha_test && out.alpha_compare == 1 && !out.depth_test && !out.depth_write);
    assert(px_bch_material_preview(NULL, &out) == PX_BCH_MATERIAL_INVALID_ARGUMENT);
    assert(px_bch_material_preview(&m, NULL) == PX_BCH_MATERIAL_INVALID_ARGUMENT);
}
static void test_stage_zero_selects_projected_base_unit(void)
{
    struct px_bch_material m = projected();
    struct px_bch_preview_material out;
    assert(px_bch_material_preview(&m, &out) == PX_BCH_MATERIAL_OK);
    assert(out.unit == 1 && out.unexecuted_units == 1);
    assert(out.combine_rgb == 1 && out.combine_alpha == 1);
    assert(out.wrap_u == 2 && out.wrap_v == 2 && !out.mag_linear);
    /* Stage 0 naming the projection unit is outside the preview contract. */
    m.fragment.tev[0][0] = full(0x00030003);
    expect(m, PX_BCH_MATERIAL_UNSUPPORTED_COORDINATES);
    m = projected(); m.coordinators[0].packed = 0xff000001;
    m.fragment.tev[0][0] = full(0x00030003);
    expect(m, PX_BCH_MATERIAL_UNSUPPORTED_SAMPLER); /* sampler type 3 */
    m = projected(); m.texture_unit_config = 0x11001; /* unit 1 disabled */
    expect(m, PX_BCH_MATERIAL_UNSUPPORTED_COMBINER);
    m = projected(); m.texture_names[1].present = 0;
    expect(m, PX_BCH_MATERIAL_UNSUPPORTED_COMBINER);
}
static void test_rejections(void)
{
    struct px_bch_material m = ordinary();
    m.parameters_present = 0; expect(m, PX_BCH_MATERIAL_NO_PARAMETERS);
    m = ordinary(); m.fragment.tev[0][1].known_mask = 0xffff; expect(m, PX_BCH_MATERIAL_UNKNOWN_STATE);
    m = ordinary(); m.fragment.blend.known_mask = 0xffff0000; expect(m, PX_BCH_MATERIAL_UNKNOWN_STATE);
    m = ordinary(); m.fragment.alpha_test.known_mask = 0xff; expect(m, PX_BCH_MATERIAL_UNKNOWN_STATE);
    m = ordinary(); m.fragment.depth_color.known_mask = 0xff; expect(m, PX_BCH_MATERIAL_UNKNOWN_STATE);
    m = ordinary(); m.unit_parameters[0].known_mask = 0xffff; expect(m, PX_BCH_MATERIAL_UNKNOWN_STATE);
    /* A missing constant register is irrelevant to stage-0 sources here. */
    m = ordinary(); m.fragment.tev[0][3].known_mask = 0;
    struct px_bch_preview_material out;
    assert(px_bch_material_preview(&m, &out) == PX_BCH_MATERIAL_OK);
    const uint32_t bad_sources[] = {0x00000000, 0x00030006, 0x00040003, 0x000300e3};
    for (unsigned i = 0; i < sizeof bad_sources / sizeof *bad_sources; ++i) {
        m = ordinary(); m.fragment.tev[0][0] = full(bad_sources[i]);
        expect(m, PX_BCH_MATERIAL_UNSUPPORTED_COMBINER);
    }
    m = ordinary(); m.fragment.tev[0][0] = full(0x00030023); /* modulate by fsecondary */
    expect(m, PX_BCH_MATERIAL_UNSUPPORTED_COMBINER);
    m = ordinary(); m.fragment.tev[0][2] = full(0x00000002); expect(m, PX_BCH_MATERIAL_UNSUPPORTED_COMBINER);
    m = ordinary(); m.fragment.tev[0][1] = full(0x1); expect(m, PX_BCH_MATERIAL_UNSUPPORTED_COMBINER);
    m = ordinary(); m.fragment.tev[0][1] = full(0x1000); expect(m, PX_BCH_MATERIAL_UNSUPPORTED_COMBINER);
    m = ordinary(); m.fragment.tev[0][4] = full(0x10000); expect(m, PX_BCH_MATERIAL_UNSUPPORTED_COMBINER);
    /* Nonzero operand1 with replace is unused, but only operand0 is checked. */
    m = ordinary(); m.fragment.tev[0][2] = full(0); m.fragment.tev[0][0] = full(0x00e300e3);
    assert(px_bch_material_preview(&m, &out) == PX_BCH_MATERIAL_OK);
    m = ordinary(); m.coordinators[0].scale[1] = 2; expect(m, PX_BCH_MATERIAL_UNSUPPORTED_COORDINATES);
    m = ordinary(); m.coordinators[0].rotation = .25f; expect(m, PX_BCH_MATERIAL_UNSUPPORTED_COORDINATES);
    m = ordinary(); m.coordinators[0].translation[1] = 1; expect(m, PX_BCH_MATERIAL_UNSUPPORTED_COORDINATES);
    m = ordinary(); m.coordinators[0].packed = 0xff010001; expect(m, PX_BCH_MATERIAL_UNSUPPORTED_COORDINATES);
    m = ordinary(); m.unit_parameters[0] = full(0x1000); expect(m, PX_BCH_MATERIAL_UNSUPPORTED_SAMPLER);
    m = ordinary(); m.unit_parameters[0] = full(0x0400); expect(m, PX_BCH_MATERIAL_UNSUPPORTED_SAMPLER);
    m = ordinary(); m.unit_parameters[0] = full(0x10000000); expect(m, PX_BCH_MATERIAL_UNSUPPORTED_SAMPLER);
    m = ordinary(); m.fragment.color_operation = full(0x000); expect(m, PX_BCH_MATERIAL_UNSUPPORTED_FRAGMENT);
    m = ordinary(); m.fragment.color_operation = full(0x101); expect(m, PX_BCH_MATERIAL_UNSUPPORTED_FRAGMENT);
    const uint32_t bad_blends[] = {0x01010001, 0x01010100, 0x76710000, 0x77770000, 0x11110000, 0x01000000};
    for (unsigned i = 0; i < sizeof bad_blends / sizeof *bad_blends; ++i) {
        m = ordinary(); m.fragment.blend = full(bad_blends[i]);
        expect(m, PX_BCH_MATERIAL_UNSUPPORTED_FRAGMENT);
    }
    assert(strcmp(px_bch_material_result_string(PX_BCH_MATERIAL_OK), "success") == 0);
    assert(strcmp(px_bch_material_result_string((enum px_bch_material_result)99),
                  "unknown BCH material result") == 0);
}
static struct px_bch_material full_chain(void)
{
    struct px_bch_material m = projected();
    for (unsigned i = 1; i < 6; ++i) {
        m.fragment.tev[i][0] = full(0x0fff0fff);
        for (unsigned j = 1; j < 5; ++j) m.fragment.tev[i][j] = full(0);
    }
    m.fragment.tev[0][3] = full(0xff000000);
    /* Stage 1 also reads texture 0 (projected) and texture 2 (disabled). */
    m.fragment.tev[1][0] = full(0x0f530f53);
    m.fragment.tev[1][2] = full(0x00020002);
    m.fragment.tev_buffer = full(0x00000800);
    m.fragment.buffer_color = full(0x11223344);
    return m;
}
static void test_combiner_chain(void)
{
    struct px_bch_material m = full_chain();
    struct px_bch_combiner_material out;
    assert(px_bch_material_combiners(&m, &out) == PX_BCH_MATERIAL_OK);
    assert(out.sampled_units == 2 && out.projected_units == 1 && out.disabled_units == 4);
    assert(out.wrap_u[1] == 2 && out.wrap_v[1] == 2 && !out.mag_linear[1]);
    assert(out.tev.buffer_update == 0x800 && out.tev.buffer_color == 0x11223344);
    assert(out.tev.stages[1].source == 0x0f530f53 && out.tev.stages[0].combine == 0x00010001);
    assert(out.sources & (1u << PX_TEV_TEXTURE2));
    assert(out.fragment.blend == PX_BCH_BLEND_REPLACE && out.fragment.cull == 2);
    struct px_bch_combiner_material empty;
    memset(&empty, 0, sizeof empty);
#define REJECT(mutation, code) do { struct px_bch_material x = full_chain(); mutation; \
        memset(&out, 0xff, sizeof out); \
        assert(px_bch_material_combiners(&x, &out) == (code)); \
        assert(memcmp(&out, &empty, sizeof out) == 0); } while (0)
    REJECT(x.parameters_present = 0, PX_BCH_MATERIAL_NO_PARAMETERS);
    REJECT(x.fragment.tev[5][4].known_mask = 0, PX_BCH_MATERIAL_UNKNOWN_STATE);
    REJECT(x.fragment.tev[2][3].known_mask = 0xffffff, PX_BCH_MATERIAL_UNKNOWN_STATE);
    REJECT(x.fragment.buffer_color.known_mask = 0, PX_BCH_MATERIAL_UNKNOWN_STATE);
    REJECT(x.fragment.tev_buffer.known_mask = 0xff, PX_BCH_MATERIAL_UNKNOWN_STATE);
    REJECT(x.fragment.tev[3][0] = full(0x0fff0ff7), PX_BCH_MATERIAL_UNSUPPORTED_COMBINER);
    REJECT(x.fragment.tev[3][0] = full(0x0fff0ff6), PX_BCH_MATERIAL_UNSUPPORTED_COMBINER);
    REJECT(x.texture_names[1].present = 0, PX_BCH_MATERIAL_UNSUPPORTED_COMBINER);
    REJECT(x.coordinators[0].packed = 0x00010001, PX_BCH_MATERIAL_UNSUPPORTED_COORDINATES);
    REJECT(x.coordinators[1].translation[0] = .5f; x.coordinators[1].packed = 0x00000301,
           PX_BCH_MATERIAL_UNSUPPORTED_COORDINATES);  /* transform type 3 */
    REJECT(x.coordinators[1].translation[0] = NAN, PX_BCH_MATERIAL_UNSUPPORTED_COORDINATES);
    REJECT(x.coordinators[1].scale[0] = INFINITY, PX_BCH_MATERIAL_UNSUPPORTED_COORDINATES);
    REJECT(x.unit_parameters[1] = full(0x1220), PX_BCH_MATERIAL_UNSUPPORTED_SAMPLER);
    REJECT(x.unit_parameters[1].known_mask = 0, PX_BCH_MATERIAL_UNKNOWN_STATE);
    REJECT(x.fragment.color_operation = full(0), PX_BCH_MATERIAL_UNSUPPORTED_FRAGMENT);
    REJECT(x.fragment.depth_color.known_mask = 0xff, PX_BCH_MATERIAL_UNKNOWN_STATE);
#undef REJECT
    /* A transformed coordinator (Maya type 0): u' = u - .5, v' = v. */
    m = full_chain();
    m.coordinators[1].packed &= ~UINT32_C(0xff00);
    m.coordinators[1].translation[0] = .5f;
    assert(px_bch_material_combiners(&m, &out) == PX_BCH_MATERIAL_OK && out.transformed_units == 2);
    assert(out.uv_matrix[1][0] == 1 && out.uv_matrix[1][1] == 0 && out.uv_matrix[1][2] == -.5f);
    assert(out.uv_matrix[1][3] == 0 && out.uv_matrix[1][4] == 1 && out.uv_matrix[1][5] == 0);
    /* Type 1 (SoftImage) with scale 2: u' = 2u - 2*.5. */
    m.coordinators[1].packed |= 0x100; m.coordinators[1].scale[0] = 2;
    assert(px_bch_material_combiners(&m, &out) == PX_BCH_MATERIAL_OK);
    assert(out.uv_matrix[1][0] == 2 && out.uv_matrix[1][2] == -1 && out.uv_matrix[1][5] == 0);
    m = full_chain();
    assert(px_bch_material_combiners(&m, &out) == PX_BCH_MATERIAL_OK && out.transformed_units == 0);
    /* A projected unit needs no sampler of its own; unread units are ignored. */
    m = full_chain();
    m.unit_parameters[0].known_mask = 0;
    m.unit_parameters[2] = full(0x1000);
    assert(px_bch_material_combiners(&m, &out) == PX_BCH_MATERIAL_OK);
    assert(px_bch_material_combiners(NULL, &out) == PX_BCH_MATERIAL_INVALID_ARGUMENT);
    assert(px_bch_material_combiners(&m, NULL) == PX_BCH_MATERIAL_INVALID_ARGUMENT);
}
int main(void)
{
    test_combiner_chain();
    test_ordinary_state();
    test_stage_zero_selects_projected_base_unit();
    test_rejections();
    puts("BCH material preview tests passed");
    return 0;
}
