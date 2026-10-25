#include "vertex_program.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

static int same(const struct px_bch_name *a, const struct px_bch_name *b)
{
    return a->present && b->present && a->size == b->size && !memcmp(a->bytes, b->bytes, a->size);
}

/* First c register of a named float uniform covering rows registers. */
static int find(const struct px_pica_entry *entry, const char *name, int rows)
{
    for (uint32_t u = 0; u < entry->uniform_count; ++u) {
        const struct px_pica_uniform *x = entry->uniforms + u;
        if (strcmp(x->name, name) || x->first < 16 || x->last >= 112) continue;
        if ((int)(x->last - x->first + 1) < rows) return -1;
        return x->first - 16;
    }
    return -1;
}

int px_vertex_program_load(const uint8_t *data, size_t size, const struct px_bch_model *model,
                           uint32_t material, struct px_vertex_program *out)
{
    if (!out) return 0;
    memset(out, 0, sizeof *out);
    if (!data || !model || material >= model->material_count) return 0;
    const struct px_bch_name *reference = &model->materials[material].shader;
    if (!reference->present) return 0;
    uint32_t entry = 0, digits = 0;
    while (digits < reference->size && reference->bytes[digits] >= '0' && reference->bytes[digits] <= '9') {
        entry = entry * 10 + (uint32_t)(reference->bytes[digits] - '0');
        if (++digits > 4) return 0;
    }
    if (!digits || digits >= reference->size || reference->bytes[digits] != '@') return 0;
    for (uint32_t k = 0; k < model->shader_count; ++k) {
        const struct px_bch_shader *shader = model->shaders + k;
        if (!same(&shader->name, reference)) continue;
        if (shader->offset > size || shader->size > size - shader->offset) return 0;
        if (px_dvlb_decode(data + shader->offset, shader->size, &out->dvlb) != PX_PICA_OK) return 0;
        if (entry >= out->dvlb.entry_count || out->dvlb.entries[entry].type != 0) return 0;
        out->entry = out->dvlb.entries + entry;
        out->resource = k;
        out->entry_index = entry;
        out->world = find(out->entry, "WrldMtx", 3);
        out->normal = find(out->entry, "NormMtx", 3);
        out->view = find(out->entry, "ViewMtx", 3);
        out->projection = find(out->entry, "ProjMtx", 4);
        out->palette = find(out->entry, "UnivReg", 3);
        out->palette_rows = 0;
        for (uint32_t u = 0; out->palette >= 0 && u < out->entry->uniform_count; ++u)
            if (!strcmp(out->entry->uniforms[u].name, "UnivReg"))
                out->palette_rows = out->entry->uniforms[u].last - out->entry->uniforms[u].first + 1;
        return 1;
    }
    return 0;
}

static void apply_stream(const struct px_bch_uniforms *u, struct px_pica_state *s)
{
    for (unsigned c = 0; c < 96; ++c)
        if (u->written[c]) memcpy(s->c[c], u->c[c], sizeof s->c[c]);
    for (unsigned b = 0; b < 16; ++b)
        if (u->bools_known >> b & 1) s->b[b] = (uint8_t)(u->bools >> b & 1);
}

static void rows(struct px_pica_state *s, int first, const double *m, unsigned count)
{
    for (unsigned r = 0; r < count; ++r)
        for (unsigned c = 0; c < 4; ++c) s->c[first + (int)r][c] = (float)m[r * 4 + c];
}

static void multiply(const double a[12], const double b[12], double out[12])
{
    double r[12];
    for (unsigned i = 0; i < 3; ++i)
        for (unsigned j = 0; j < 4; ++j)
            r[i * 4 + j] = a[i * 4] * b[j] + a[i * 4 + 1] * b[4 + j] + a[i * 4 + 2] * b[8 + j] + (j == 3 ? a[i * 4 + 3] : 0);
    memcpy(out, r, sizeof r);
}

int px_vertex_program_prepare(const struct px_vertex_program *p, const struct px_bch_model *m,
                              uint32_t mesh_index, uint32_t submesh_index, const struct px_vertex_frame *f,
                              struct px_pica_state *s)
{
    if (!p || !p->entry || !m || !f || !s || mesh_index >= m->mesh_count) return 0;
    const struct px_bch_mesh *mesh = m->meshes + mesh_index;
    if (submesh_index >= mesh->submesh_count || mesh->material_index >= m->material_count) return 0;
    const struct px_bch_submesh *sub = m->submeshes + mesh->first_submesh + submesh_index;
    memset(s, 0, sizeof *s);
    px_pica_constants(p->entry, s);
    apply_stream(&m->materials[mesh->material_index].uniforms, s);
    apply_stream(&mesh->uniforms, s);
    apply_stream(&sub->uniforms, s);
    double w[12], v[12], proj[16];
    for (unsigned k = 0; k < 12; ++k) { w[k] = f->world[k]; v[k] = f->view[k]; }
    for (unsigned k = 0; k < 16; ++k) proj[k] = f->projection[k];
    if (p->world >= 0) rows(s, p->world, w, 3);
    if (p->view >= 0) rows(s, p->view, v, 3);
    if (p->projection >= 0) rows(s, p->projection, proj, 4);
    if (p->normal >= 0) {
        /* inverse transpose of the 3x3 part: the cofactor matrix over the determinant */
        const double a = w[0], b = w[1], c = w[2], d = w[4], e = w[5], g = w[6], h = w[8], i = w[9], k = w[10];
        const double det = a * (e * k - g * i) - b * (d * k - g * h) + c * (d * i - e * h);
        if (det == 0 || !isfinite(det)) return 0;
        const double n[12] = {(e * k - g * i) / det, -(d * k - g * h) / det, (d * i - e * h) / det, 0,
                              -(b * k - c * i) / det, (a * k - c * h) / det, -(a * i - b * h) / det, 0,
                              (b * g - c * e) / det, -(a * g - c * d) / det, (a * e - b * d) / det, 0};
        rows(s, p->normal, n, 3);
    }
    if (p->palette >= 0 && sub->bone_count) {
        if (!f->bones || (int)sub->bone_count * 3 > p->palette_rows) return 0;
        for (uint32_t k = 0; k < sub->bone_count; ++k) {
            const uint32_t bone = sub->bone_indices[k];
            if (bone >= m->bone_count) return 0;
            double entry[12];
            memcpy(entry, f->bones + (size_t)bone * 12, sizeof entry);
            if (sub->skinning == 1) {
                double inverse[12];
                for (unsigned j = 0; j < 12; ++j) inverse[j] = m->bones[bone].inverse[j];
                multiply(entry, inverse, entry);
            }
            rows(s, p->palette + 3 * (int)k, entry, 3);
        }
    }
    return 1;
}

int px_vertex_program_run(const struct px_vertex_program *p, const struct px_bch_model *m, uint32_t vertex,
                          const struct px_pica_state *prepared, struct px_pica_state *work)
{
    if (!p || !p->entry || !m || !m->inputs || vertex >= m->vertex_count || !prepared || !work)
        return PX_PICA_INVALID;
    *work = *prepared;
    for (unsigned r = 0; r < 12; ++r)
        memcpy(work->v[r], m->inputs + (size_t)vertex * 48 + r * 4, sizeof work->v[r]);
    return px_pica_run(&p->dvlb, p->entry, work);
}

unsigned px_vertex_program_output(const struct px_vertex_program *p, const struct px_pica_state *s,
                                  unsigned semantic, float out[4])
{
    unsigned found = 0;
    if (!p || !p->entry || !s || !out) return 0;
    for (uint32_t k = 0; k < p->entry->output_count; ++k) {
        const struct px_pica_output *o = p->entry->outputs + k;
        if (o->type != semantic || o->reg >= 16) continue;
        for (unsigned c = 0, n = 0; c < 4; ++c)
            if (o->mask >> c & 1) {
                if (n < 4) { out[n] = s->o[o->reg][c]; found |= 1u << n; }
                ++n;
            }
    }
    return found;
}
