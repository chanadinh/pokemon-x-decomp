/* Runs a skinned BCH model's own vertex shader programs in the software PICA
 * interpreter, in the rest pose with identity world, view and projection
 * matrices, and compares every drawn vertex with the CPU path
 * (render/skin.h, a translation of SPICA's default vertex shader):
 * position (o0.xyz against the posed position; o0.w must be 1) and texture
 * coordinate 0 (o4.xy against the decoded UV through the material's TexMtx0
 * rows, as the uniform stream supplies them, where the program's UvMap0 bool
 * selects mapped coordinates; otherwise it generates them and they are only
 * counted). The projection is a uniform scale (below). The normal is not compared:
 * the PokePack programs light per vertex (o3) and output an identity normal
 * quaternion (o1) and a tangent-space light vector (o2), whose deviations
 * from identity and from unit length are reported.
 *
 * Usage: shader_check FILE.bch [MODEL] [--section N] [--motion PACK --slot S]
 * (N: PC container section holding the BCH; PACK: a GF1 motion pack, whose
 * slot S adds its first, middle and last frames as poses). Prints one line per mesh and a summary; exits
 * nonzero if a program fails to load or run. */
#include "../assets/bch_model.h"
#include "../platform/pica_shader.h"
#include "../render/skin.h"
#include "../render/vertex_program.h"
#include "../assets/gf_motion.h"

#include <math.h>
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

static uint32_t u32(const uint8_t *p) { return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24; }

int main(int argc, char **argv)
{
    const char *path = NULL;
    uint32_t model_index = 0;
    int section = -1;
    const char *motion_path = NULL;
    uint32_t slot = 1;
    for (int k = 1; k < argc; ++k) {
        if (!strcmp(argv[k], "--section") && k + 1 < argc) section = atoi(argv[++k]);
        else if (!strcmp(argv[k], "--motion") && k + 1 < argc) motion_path = argv[++k];
        else if (!strcmp(argv[k], "--slot") && k + 1 < argc) slot = (uint32_t)strtoul(argv[++k], NULL, 10);
        else if (!path) path = argv[k];
        else model_index = (uint32_t)strtoul(argv[k], NULL, 10);
    }
    if (!path) { fprintf(stderr, "usage: shader_check FILE [MODEL] [--section N] [--motion PACK --slot S]\n"); return 2; }
    size_t size;
    uint8_t *file = slurp(path, &size);
    if (!file) { fprintf(stderr, "cannot read %s\n", path); return 2; }
    const uint8_t *data = file;
    if (section >= 0) {
        if (size < 8 || file[0] < 'A' || file[1] < 'A') { fprintf(stderr, "not a container\n"); return 2; }
        const uint32_t count = (uint32_t)(file[2] | file[3] << 8);
        if ((uint32_t)section >= count || 8 + 4 * (size_t)count > size) { fprintf(stderr, "no section\n"); return 2; }
        const uint32_t start = u32(file + 4 + 4 * (size_t)section), end = u32(file + 8 + 4 * (size_t)section);
        if (start > end || end > size) { fprintf(stderr, "bad section\n"); return 2; }
        data = file + start;
        size = end - start;
    }
    struct px_bch_model m;
    const enum px_bch_model_result decoded = px_bch_model_decode_skinned(data, size, model_index, &m);
    if (decoded != PX_BCH_MODEL_OK) { printf("decode error=%s\n", px_bch_model_result_string(decoded)); free(file); return 1; }
    double *world = calloc(m.bone_count ? m.bone_count : 1, 12 * sizeof *world);
    struct px_vertex_program *program = malloc(sizeof *program);
    struct px_pica_state *prepared = malloc(sizeof *prepared), *work = malloc(sizeof *work);
    if (!world || !program || !prepared || !work || !px_skin_rest_world(m.bones, m.bone_count, world)) {
        printf("setup error\n");
        return 1;
    }
    /* A uniform scale keeps every rest-pose vertex well inside the clip
     * volume: the programs snap x in (-w, -0.9915w) to -w (c95.z), an edge
     * fix that would otherwise show up as a difference. */
    /* Poses: the rest pose, then with --motion the first, middle and last
     * frames of the motion in that slot (the pack's GF1 data, as
     * render/skin.h poses it). */
    struct px_gf_motion_pack pack;
    memset(&pack, 0, sizeof pack);
    const struct px_gf_motion *motion = NULL;
    float (*locals)[9] = NULL;
    if (motion_path) {
        size_t motion_size;
        uint8_t *motion_data = slurp(motion_path, &motion_size);
        if (!motion_data || px_gf_motion_pack_decode(motion_data, motion_size, &pack) != PX_GF_MOTION_OK ||
            !(motion = px_gf_motion_find(&pack, slot)) ||
            !(locals = calloc(m.bone_count ? m.bone_count : 1, sizeof *locals))) {
            printf("motion error\n");
            return 1;
        }
        free(motion_data);
    }
    const unsigned pose_count = motion ? 4 : 1;
    double extent = 1;
    for (unsigned c = 0; c < 3; ++c) {
        if (fabs(m.bounds_min[c]) + 1 > extent) extent = fabs(m.bounds_min[c]) + 1;
        if (fabs(m.bounds_max[c]) + 1 > extent) extent = fabs(m.bounds_max[c]) + 1;
    }
    const float scale = (float)(0.25 / extent);
    struct px_vertex_frame frame = {{1,0,0,0, 0,1,0,0, 0,0,1,0}, {1,0,0,0, 0,1,0,0, 0,0,1,0},
                                    {scale,0,0,0, 0,scale,0,0, 0,0,scale,0, 0,0,0,1}, world};
    int failures = 0;
    unsigned long vertices = 0;
    double worst_position = 0, worst_w = 0, worst_angle = 0, worst_uv = 0, worst_view = 0;
    for (unsigned pose = 0; pose < pose_count; ++pose) {
    const uint32_t frame_number = pose < 2 || !motion ? 0 : pose == 2 ? motion->frames / 2 : motion->frames;
    if (pose && (!px_skin_motion_locals(&m, &pack, motion, frame_number, locals) ||
                 !px_skin_world_from_locals(m.bones, m.bone_count, (const float (*)[9])locals, world))) {
        printf("pose=%u error=pose\n", pose);
        ++failures;
        continue;
    }
    for (uint32_t i = 0; i < m.mesh_count; ++i) {
        const struct px_bch_mesh *mesh = m.meshes + i;
        const struct px_bch_material *material = m.materials + mesh->material_index;
        if (!px_vertex_program_load(data, size, &m, mesh->material_index, program)) {
            printf("mesh=%u material=%s shader=%s error=load\n", i, material->name.bytes,
                   material->shader.present ? material->shader.bytes : "(none)");
            ++failures;
            continue;
        }
        int texture = -1, uv_map = -1;
        for (uint32_t u = 0; u < program->entry->uniform_count; ++u) {
            if (!strcmp(program->entry->uniforms[u].name, "TexMtx0")) texture = program->entry->uniforms[u].first - 16;
            if (!strcmp(program->entry->uniforms[u].name, "UvMap0") && program->entry->uniforms[u].first >= 120)
                uv_map = program->entry->uniforms[u].first - 120;
        }
        unsigned long generated = 0;
        double position = 0, w_error = 0, angle = 0, uv = 0, view_length = 0;
        unsigned long count = 0, run_errors = 0, normals = 0;
        uint32_t bools = 0;
        for (uint32_t s = 0; s < mesh->submesh_count; ++s) {
            const struct px_bch_submesh *sub = m.submeshes + mesh->first_submesh + s;
            if (!px_vertex_program_prepare(program, &m, i, s, &frame, prepared)) { ++run_errors; continue; }
            for (unsigned b = 0; b < 16; ++b) bools |= (uint32_t)prepared->b[b] << b;
            for (uint32_t k = sub->first_index; k < sub->first_index + sub->index_count; ++k) {
                const uint32_t vertex = m.indices[k];
                double p[3], n[3];
                if (vertex < mesh->first_vertex ||
                    !px_skin_vertex(&m, mesh, sub, world, vertex - mesh->first_vertex, p, n)) { ++run_errors; continue; }
                if (px_vertex_program_run(program, &m, vertex, prepared, work) != PX_PICA_OK) { ++run_errors; continue; }
                float o[4] = {0, 0, 0, 0}, q[4] = {0, 0, 0, 0}, t[4] = {0, 0, 0, 0};
                px_vertex_program_output(program, work, PX_VERTEX_OUTPUT_POSITION, o);
                for (unsigned c = 0; c < 3; ++c) {
                    const double e = fabs(o[c] / scale - p[c]) / (1.0 + fabs(p[c]));
                    if (e > position) position = e;
                }
                if (fabs(o[3] - 1.0) > w_error) w_error = fabs(o[3] - 1.0);
                /* o1 is the normal quaternion the fragment lighting reads; o2 the
                 * view vector. Recorded as found: their range, not a comparison. */
                if (px_vertex_program_output(program, work, PX_VERTEX_OUTPUT_QUATERNION, q) == 15) {
                    const double d = fabs(q[0]) + fabs(q[1]) + fabs(q[2]) + fabs(q[3] - 1);
                    if (d > angle) angle = d;
                    ++normals;
                }
                float view[4] = {0, 0, 0, 0};
                if (px_vertex_program_output(program, work, PX_VERTEX_OUTPUT_VIEW, view) == 15) {
                    const double l = fabs(sqrt(view[0] * view[0] + view[1] * view[1] + view[2] * view[2]) - 1);
                    if (l > view_length) view_length = l;
                }
                (void)n;
                if (uv_map >= 0 && !prepared->b[uv_map]) ++generated; /* coordinates generated, not mapped */
                else if (texture >= 0 && mesh->uv_source &&
                    (px_vertex_program_output(program, work, PX_VERTEX_OUTPUT_TEXCOORD0, t) & 3) == 3) {
                    const float *u = m.uvs + (size_t)vertex * 2;
                    for (unsigned c = 0; c < 2; ++c) {
                        const float *row = prepared->c[texture + (int)c];
                        const double want = row[0] * u[0] + row[1] * u[1] + row[3];
                        const double e = fabs(t[c] - want);
                        if (e > uv) uv = e;
                    }
                }
                ++count;
            }
        }
        printf("pose=%u frame=%u mesh=%u name=%s material=%s shader=%s entry=%u bools=%04x vertices=%lu quaternions=%lu generated_uv=%lu "
               "position_error=%.3g w_error=%.3g uv_error=%.3g quaternion_from_identity=%.3g view_from_unit=%.3g errors=%lu\n",
               pose, frame_number, i, mesh->name.bytes, material->name.bytes, material->shader.bytes, program->entry_index, bools, count,
               normals, generated, position, w_error, uv, angle, view_length, run_errors);
        if (run_errors) ++failures;
        vertices += count;
        if (position > worst_position) worst_position = position;
        if (w_error > worst_w) worst_w = w_error;
        if (angle > worst_angle) worst_angle = angle;
        if (uv > worst_uv) worst_uv = uv;
        if (view_length > worst_view) worst_view = view_length;
    }
    }
    printf("summary poses=%u meshes=%u vertices=%lu position_error=%.3g w_error=%.3g uv_error=%.3g quaternion_from_identity=%.3g view_from_unit=%.3g failures=%d\n",
           pose_count, m.mesh_count, vertices, worst_position, worst_w, worst_uv, worst_angle, worst_view, failures);
    free(locals); px_gf_motion_pack_free(&pack);
    free(world); free(program); free(prepared); free(work);
    px_bch_model_free(&m);
    free(file);
    return failures != 0;
}
