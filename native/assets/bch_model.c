#include "bch_model.h"

#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

_Static_assert(sizeof(float) == 4 && FLT_RADIX == 2 && FLT_MANT_DIG == 24 &&
               FLT_MAX_EXP == 128, "BCH geometry requires IEEE binary32 float");

/* Section-relative pointers retain their checked relocation provenance. This
 * implementation never patches input or runs shaders and GPU command lists. */
enum { CONTENTS, STRINGS, COMMANDS, RAW, RELOCATION, SECTION_COUNT };
enum { MAX_BYTES = 64 * 1024 * 1024, MAX_RESOURCES = 65535,
       MAX_ITEMS = 1024, MAX_RELOCS = 262144, MAX_VERTICES = 262144,
       MAX_INDICES = 786432, MAX_WORDS = 65536, MAX_TOTAL_WORDS = 262144 };
struct section { size_t start, end; };
struct relocation { uint32_t source, tag; };
struct reader {
    const uint8_t *data;
    struct section sections[SECTION_COUNT];
    struct relocation *relocations;
    size_t relocation_count, total_words;
    uint32_t counts[15];
    size_t tables[15], trees[15];
    enum px_bch_model_result error;
};
struct commands {
    uint32_t values[1024], known[1024], sources[1024];
    float uniforms[3][4];
    unsigned uniform_mask, primitive_writes;
    uint32_t draw_primitive_value, draw_primitive_known_mask;
    uint32_t draw_count, last_draw_offset;
    /* Fixed (default) attribute uploads: index register 0x232, then three
     * data words per attribute through 0x233..0x235; the index advances
     * after each complete attribute, as in the pinned emulator reference. */
    uint32_t fixed_words[12][3];
    unsigned fixed_known, fixed_index, fixed_count, fixed_index_set;
};
static uint16_t u16(const uint8_t *p)
{
    return (uint16_t)((uint32_t)p[0] | (uint32_t)p[1] << 8);
}
static uint32_t u32(const uint8_t *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 |
        (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}
static float f32(const uint8_t *p)
{
    const uint32_t bits = u32(p);
    float value;
    memcpy(&value, &bits, sizeof value);
    return value;
}
static int fail(struct reader *r, enum px_bch_model_result error)
{
    r->error = error;
    return 0;
}
static int span(size_t at, size_t length, struct section s)
{
    return at >= s.start && at <= s.end && length <= s.end - at;
}
static int compare_relocations(const void *left, const void *right)
{
    const struct relocation *a = left, *b = right;
    return (a->source > b->source) - (a->source < b->source);
}
static const struct relocation *relocation(const struct reader *r, size_t source)
{
    size_t first = 0, last = r->relocation_count;
    while (first < last) {
        const size_t middle = first + (last - first) / 2;
        const struct relocation *item = r->relocations + middle;
        if (item->source < source) first = middle + 1;
        else if (item->source > source) last = middle;
        else return item;
    }
    return NULL;
}
static int target_section(unsigned tag)
{
    switch (tag) {
    case 0: return CONTENTS;
    case 1: return STRINGS;
    case 2: case 3: return COMMANDS;
    case 0x24: case 0x26: case 0x27: case 0x28: return RAW;
    default: return -1;
    }
}
static int pointer(struct reader *r, size_t source, unsigned section,
                   size_t length, size_t *target)
{
    const struct relocation *rel = relocation(r, source);
    if (rel == NULL || target_section(rel->tag) != (int)section) return 0;
    const struct section s = r->sections[section];
    const size_t relative = u32(r->data + source);
    if (relative > s.end - s.start) return 0;
    *target = s.start + relative;
    return span(*target, length, s);
}
static int name(struct reader *r, size_t source, int nullable,
                struct px_bch_name *out)
{
    memset(out, 0, sizeof *out);
    if (nullable && relocation(r, source) == NULL && u32(r->data + source) == 0)
        return 1;
    size_t start;
    if (!pointer(r, source, STRINGS, 1, &start)) return 0;
    size_t available = r->sections[STRINGS].end - start;
    if (available > sizeof out->bytes) available = sizeof out->bytes;
    const uint8_t *end = memchr(r->data + start, 0, available);
    if (end == NULL) return 0;
    out->size = (uint32_t)(end - (r->data + start));
    out->present = 1;
    memcpy(out->bytes, r->data + start, out->size);
    return 1;
}
static int same_name(const struct px_bch_name *a, const struct px_bch_name *b)
{
    return a->present == b->present && a->size == b->size &&
        memcmp(a->bytes, b->bytes, a->size) == 0;
}
static int tree(struct reader *r, size_t field, uint32_t count, size_t *start)
{
    if (!pointer(r, field, CONTENTS, ((size_t)count + 1) * 12, start)) return 0;
    for (uint32_t i = 0; i <= count; ++i) {
        const size_t at = *start + (size_t)i * 12;
        if (u16(r->data + at + 4) > count || u16(r->data + at + 6) > count)
            return 0;
        if (i != 0) {
            struct px_bch_name ignored;
            if (!name(r, at + 8, 0, &ignored)) return 0;
        }
    }
    return 1;
}
static int read_relocations(struct reader *r)
{
    const struct section s = r->sections[RELOCATION];
    if ((s.end - s.start) % 4) return 0;
    r->relocation_count = (s.end - s.start) / 4;
    if (r->relocation_count > MAX_RELOCS) return fail(r, PX_BCH_MODEL_LIMIT_EXCEEDED);
    if (r->relocation_count == 0) return 1;
    r->relocations = calloc(r->relocation_count, sizeof *r->relocations);
    if (r->relocations == NULL) return fail(r, PX_BCH_MODEL_NO_MEMORY);
    for (size_t i = 0; i < r->relocation_count; ++i) {
        const uint32_t word = u32(r->data + s.start + i * 4);
        const unsigned tag = word >> 25;
        const int target = target_section(tag);
        if (target < 0) return fail(r, PX_BCH_MODEL_UNSUPPORTED);
        size_t relative = word & UINT32_C(0x1ffffff);
        if (tag != 1) relative *= 4;
        const struct section source = r->sections[tag >= 0x24 ? COMMANDS : CONTENTS];
        if (relative > source.end - source.start ||
            !span(source.start + relative, 4, source)) return 0;
        const size_t field = source.start + relative;
        if (u32(r->data + field) > r->sections[target].end - r->sections[target].start)
            return 0;
        r->relocations[i].source = (uint32_t)field;
        r->relocations[i].tag = tag;
    }
    qsort(r->relocations, r->relocation_count, sizeof *r->relocations, compare_relocations);
    for (size_t i = 1; i < r->relocation_count; ++i)
        if (r->relocations[i].source - r->relocations[i - 1].source < 4) return 0;
    return 1;
}
static int read_index(struct reader *r)
{
    const size_t base = r->sections[CONTENTS].start;
    if (!span(base, 180, r->sections[CONTENTS])) return 0;
    size_t total = 0;
    for (unsigned i = 0; i < 15; ++i) {
        const size_t field = base + i * 12;
        const uint32_t count = u32(r->data + field + 4);
        if (count > MAX_RESOURCES - total) return fail(r, PX_BCH_MODEL_LIMIT_EXCEEDED);
        total += count;
        r->counts[i] = count;
        if (count || u32(r->data + field) || relocation(r, field))
            if (!pointer(r, field, CONTENTS, (size_t)count * 4, &r->tables[i])) return 0;
        if (!count && !u32(r->data + field + 8) && !relocation(r, field + 8)) continue;
        if (!tree(r, field + 8, count, &r->trees[i])) return 0;
        for (uint32_t j = 0; j < count; ++j) {
            size_t ignored;
            if (!pointer(r, r->tables[i] + (size_t)j * 4, CONTENTS, 4, &ignored)) return 0;
        }
    }
    return 1;
}
static int bounded(double value)
{
    return isfinite(value) && fabs(value) <= 1e12;
}
static int read_commands(struct reader *r, size_t field, struct commands *out)
{
    memset(out, 0, sizeof *out);
    for (unsigned i = 0; i < 1024; ++i) out->sources[i] = UINT32_MAX;
    const uint32_t words = u32(r->data + field + 4);
    if (!words || words > MAX_WORDS || words > MAX_TOTAL_WORDS - r->total_words)
        return fail(r, PX_BCH_MODEL_LIMIT_EXCEEDED);
    r->total_words += words;
    size_t start;
    if (!pointer(r, field, COMMANDS, (size_t)words * 4, &start) || start % 4) return 0;
    const size_t end = start + (size_t)words * 4;
    unsigned uniform_component = 0, uniform_initialized = 0;
    float uniform_values[4] = {0};
    for (size_t cursor = start; cursor < end;) {
        if (end - cursor < 8) return 0;
        const uint32_t header = u32(r->data + cursor + 4);
        const unsigned reg = header & 0xffff, extra = (header >> 20) & 0x7ff;
        const unsigned consecutive = header >> 31;
        const size_t packet = ((size_t)(2 + extra) * 4 + 7) & ~(size_t)7;
        if (packet > end - cursor || reg > 0x3ff ||
            (consecutive && extra > 0x3ff - reg)) return 0;
        uint32_t mask = 0;
        for (unsigned byte = 0; byte < 4; ++byte)
            if ((header >> (16 + byte)) & 1) mask |= UINT32_C(255) << (byte * 8);
        for (unsigned parameter = 0; parameter <= extra; ++parameter) {
            const unsigned target = reg + (consecutive ? parameter : 0);
            const size_t source = cursor + (parameter ? 4 + (size_t)parameter * 4 : 0);
            const uint32_t value = u32(r->data + source);
            if (mask == 0) continue;
            if (target == 0x22e) return fail(r, PX_BCH_MODEL_UNSUPPORTED);
            if (out->draw_count && (target == 0x227 || target == 0x228))
                return fail(r, PX_BCH_MODEL_UNSUPPORTED);
            out->values[target] = (out->values[target] & ~mask) | (value & mask);
            out->known[target] |= mask;
            out->sources[target] = mask == UINT32_MAX ? (uint32_t)source : UINT32_MAX;
            if (target == 0x25e) ++out->primitive_writes;
            if (target == 0x22f) {
                /* The extractor accepts one draw in each stored index stream.
                 * Streams with none are useful synthetic extraction fixtures. */
                if (mask != UINT32_MAX || value != 1 || ++out->draw_count > 1)
                    return fail(r, PX_BCH_MODEL_UNSUPPORTED);
                if (out->known[0x228] != UINT32_MAX || out->sources[0x227] == UINT32_MAX)
                    return 0;
                out->draw_primitive_value = out->values[0x25e];
                out->draw_primitive_known_mask = out->known[0x25e];
                out->last_draw_offset = (uint32_t)source;
            }
            if (target == 0x232) {
                if (mask != UINT32_MAX || (value & 15) >= 12) return fail(r, PX_BCH_MODEL_UNSUPPORTED);
                out->fixed_index = value & 15;
                out->fixed_count = 0;
                out->fixed_index_set = 1;
            } else if (target >= 0x233 && target <= 0x235) {
                if (mask != UINT32_MAX || !out->fixed_index_set || out->fixed_index >= 12)
                    return fail(r, PX_BCH_MODEL_UNSUPPORTED);
                out->fixed_words[out->fixed_index][out->fixed_count++] = value;
                if (out->fixed_count == 3) {
                    out->fixed_known |= 1u << out->fixed_index;
                    ++out->fixed_index;
                    out->fixed_count = 0;
                }
            }
            if (target == 0x2c0) {
                if (mask != UINT32_MAX || !(value & UINT32_C(0x80000000)))
                    return fail(r, PX_BCH_MODEL_UNSUPPORTED);
                if ((value & 255) >= 96) return 0;
                uniform_component = (value & 255) * 4;
                uniform_initialized = 1;
            } else if (target >= 0x2c1 && target <= 0x2c8) {
                if (mask != UINT32_MAX) return fail(r, PX_BCH_MODEL_UNSUPPORTED);
                if (!uniform_initialized || uniform_component >= 96 * 4) return 0;
                const unsigned index = uniform_component / 4, component = uniform_component % 4;
                if (index >= 6 && index <= 8) {
                    const float number = f32(r->data + source);
                    if (!bounded(number)) return 0;
                    if (component == 0) out->uniform_mask &= ~(1u << (index - 6));
                    uniform_values[component] = number;
                    if (component == 3) {
                        for (unsigned j = 0; j < 4; ++j)
                            out->uniforms[index - 6][j] = uniform_values[3 - j];
                        out->uniform_mask |= 1u << (index - 6);
                    }
                }
                ++uniform_component;
            }
        }
        cursor += packet;
    }
    return 1;
}
static int value(const struct commands *commands, unsigned reg, uint32_t mask,
                 uint32_t *out)
{
    if ((commands->known[reg] & mask) != mask) return 0;
    *out = commands->values[reg] & mask;
    return 1;
}
static int raw_pointer(struct reader *r, const struct commands *commands,
                       unsigned reg, unsigned tag, size_t length, size_t *out)
{
    const uint32_t source = commands->sources[reg];
    const struct relocation *rel = relocation(r, source);
    return rel != NULL && rel->tag == tag && pointer(r, source, RAW, length, out);
}
static void *grow(struct reader *r, void *array, size_t count, size_t item_size)
{
    void *next = realloc(array, count * item_size);
    if (next == NULL) fail(r, PX_BCH_MODEL_NO_MEMORY);
    return next;
}
static void observe(const struct commands *commands, unsigned reg,
                    struct px_bch_register *out)
{
    out->known_mask = commands->known[reg];
    out->value = commands->values[reg] & out->known_mask;
}
static int read_parameters(struct reader *r, size_t params, struct px_bch_material *out)
{
    /* Older synthetic or nonstandard parameter objects stay geometry-valid;
     * only a relocated command pointer at +0xc8 opts into this metadata. */
    const struct relocation *rel = span(params, 0xd0, r->sections[CONTENTS]) ?
        relocation(r, params + 0xc8) : NULL;
    if (rel == NULL || rel->tag != 2) return 1;
    for (unsigned unit = 0; unit < 3; ++unit) {
        const size_t at = params + 12 + (size_t)unit * 24;
        struct px_bch_coordinator *c = out->coordinators + unit;
        float values[5];
        for (unsigned j = 0; j < 5; ++j) {
            values[j] = f32(r->data + at + 4 + j * 4);
            if (!bounded(values[j])) return 0;
        }
        c->packed = u32(r->data + at);
        c->scale[0] = values[0]; c->scale[1] = values[1]; c->rotation = values[2];
        c->translation[0] = values[3]; c->translation[1] = values[4];
    }
    struct commands fragment;
    if (!read_commands(r, params + 0xc8, &fragment)) return 0;
    if (fragment.draw_count) return fail(r, PX_BCH_MODEL_UNSUPPORTED);
    struct px_bch_fragment_state *f = &out->fragment;
    observe(&fragment, 0x40, &f->cull);
    observe(&fragment, 0x100, &f->color_operation);
    observe(&fragment, 0x101, &f->blend);
    observe(&fragment, 0x104, &f->alpha_test);
    observe(&fragment, 0x107, &f->depth_color);
    observe(&fragment, 0xe0, &f->tev_buffer);
    observe(&fragment, 0xfd, &f->buffer_color);
    static const unsigned stages[6] = {0xc0, 0xc8, 0xd0, 0xd8, 0xf0, 0xf8};
    for (unsigned stage = 0; stage < 6; ++stage)
        for (unsigned j = 0; j < 5; ++j) observe(&fragment, stages[stage] + j, &f->tev[stage][j]);
    out->parameters_present = 1;
    return 1;
}
static int read_materials(struct reader *r, size_t model, struct px_bch_model *out)
{
    const uint32_t count = u32(r->data + model + 0x38);
    if (!count || count > MAX_ITEMS) return fail(r, PX_BCH_MODEL_LIMIT_EXCEEDED);
    size_t start, names;
    if (!pointer(r, model + 0x34, CONTENTS, (size_t)count * 0x58, &start) ||
        !tree(r, model + 0x3c, count, &names)) return 0;
    out->materials = calloc(count, sizeof *out->materials);
    if (out->materials == NULL) return fail(r, PX_BCH_MODEL_NO_MEMORY);
    out->material_count = count;
    for (uint32_t i = 0; i < count; ++i) {
        const size_t at = start + i * 0x58;
        struct px_bch_material *material = out->materials + i;
        struct px_bch_name dictionary_name;
        size_t params;
        if (!name(r, at + 0x54, 0, &material->name) ||
            !name(r, names + ((size_t)i + 1) * 12 + 8, 0, &dictionary_name) ||
            !same_name(&material->name, &dictionary_name) ||
            !pointer(r, at, CONTENTS, 4, &params)) return 0;
        int found = 0;
        for (uint32_t j = 0; j < r->counts[1]; ++j) {
            size_t target;
            if (!pointer(r, r->tables[1] + (size_t)j * 4, CONTENTS, 4, &target)) return 0;
            if (target == params) { found = 1; break; }
        }
        if (!found) return 0;
        struct commands commands;
        if (!read_commands(r, at + 0x10, &commands) ||
            !value(&commands, 0x80, UINT32_MAX, &material->texture_unit_config)) return 0;
        if (commands.draw_count) return fail(r, PX_BCH_MODEL_UNSUPPORTED);
        static const unsigned unit_registers[3] = {0x83, 0x93, 0x9b};
        for (unsigned unit = 0; unit < 3; ++unit) {
            if (!name(r, at + 0x48 + unit * 4, 1, material->texture_names + unit)) return 0;
            observe(&commands, unit_registers[unit], &material->unit_parameters[unit]);
        }
        if (!read_parameters(r, params, material)) return 0;
    }
    return 1;
}
/* PICA float24: sign bit 23, exponent bits 16..22 (bias 63), mantissa 0..15.
 * Exponent 127 (infinity/NaN) is rejected; subnormals keep their exact value. */
static int float24(uint32_t bits, double *out)
{
    const unsigned exponent = (bits >> 16) & 127;
    const double mantissa = bits & 0xffff;
    if (exponent == 127) return 0;
    const double magnitude = exponent ? ldexp(1 + mantissa / 65536, (int)exponent - 63)
                                      : ldexp(mantissa / 65536, -62);
    *out = (bits & 0x800000) ? -magnitude : magnitude;
    return 1;
}
struct attribute { unsigned semantic, kind, elements, offset; float scale; };
static int semantics_stored_color(const struct attribute *attributes, unsigned stored)
{
    for (unsigned j = 0; j < stored; ++j) if (attributes[j].semantic == 3) return 1;
    return 0;
}
static int read_mesh(struct reader *r, size_t at, size_t names, uint32_t nodes,
                     size_t visibility, struct px_bch_model *out, struct px_bch_mesh *mesh)
{
    const uint16_t material = u16(r->data + at), flags = u16(r->data + at + 2);
    const uint16_t node = u16(r->data + at + 4), key = u16(r->data + at + 6);
    if (flags || (key & ~0x3ffu)) return fail(r, PX_BCH_MODEL_UNSUPPORTED);
    if (material >= out->material_count || node >= nodes) return 0;
    for (unsigned j = 0; j < 3; ++j) if (!bounded(f32(r->data + at + 32 + j * 4))) return 0;
    mesh->material_index = material;
    mesh->node_index = node;
    mesh->visible = (r->data[visibility + node / 8] >> (node % 8)) & 1;
    mesh->layer = key >> 8;
    mesh->priority = key & 255;
    if (!name(r, names + ((size_t)node + 1) * 12 + 8, 0, &mesh->name)) return 0;
    struct commands enable, other;
    if (!read_commands(r, at + 8, &enable) || !read_commands(r, at + 24, &other)) return 0;
    if (enable.draw_count || other.draw_count) return fail(r, PX_BCH_MODEL_UNSUPPORTED);
    const uint32_t subcount = u32(r->data + at + 20);
    if (!subcount || subcount > MAX_ITEMS) return fail(r, PX_BCH_MODEL_LIMIT_EXCEEDED);
    size_t subs;
    if (!pointer(r, at + 16, CONTENTS, (size_t)subcount * 52, &subs)) return 0;
    /* Every submesh requires at least two command words, which independently
     * caps total submesh metadata even for many tiny meshes. */
    if (subcount > MAX_TOTAL_WORDS / 2 - out->submesh_count)
        return fail(r, PX_BCH_MODEL_LIMIT_EXCEEDED);
    mesh->first_submesh = out->submesh_count;
    mesh->submesh_count = subcount;
    void *allocation = grow(r, out->submeshes, out->submesh_count + subcount, sizeof *out->submeshes);
    if (allocation == NULL) return 0;
    out->submeshes = allocation;
    memset(out->submeshes + out->submesh_count, 0, (size_t)subcount * sizeof *out->submeshes);
    out->submesh_count += subcount;
    mesh->first_index = out->index_count;
    mesh->first_vertex = out->vertex_count;
    uint32_t largest_index = 0;
    for (uint32_t i = 0; i < subcount; ++i) {
        const size_t sub = subs + (size_t)i * 52;
        if (u16(r->data + sub) || u16(r->data + sub + 2)) return fail(r, PX_BCH_MODEL_UNSUPPORTED);
        if (!read_commands(r, sub + 44, &other)) return 0;
        uint32_t count;
        if (!value(&other, 0x228, UINT32_MAX, &count)) return 0;
        if (!count || count > MAX_INDICES - out->index_count)
            return fail(r, PX_BCH_MODEL_LIMIT_EXCEEDED);
        const struct relocation *rel = relocation(r, other.sources[0x227]);
        if (rel == NULL || (rel->tag != 0x27 && rel->tag != 0x28)) return 0;
        const unsigned width = rel->tag == 0x27 ? 2 : 1;
        size_t start;
        if (!raw_pointer(r, &other, 0x227, rel->tag, (size_t)count * width, &start)) return 0;
        allocation = grow(r, out->indices, out->index_count + count, sizeof *out->indices);
        if (allocation == NULL) return 0;
        out->indices = allocation;
        struct px_bch_submesh *record = out->submeshes + mesh->first_submesh + i;
        record->first_index = out->index_count;
        record->index_count = count;
        record->index_width = width;
        record->primitive_value = other.values[0x25e];
        record->primitive_known_mask = other.known[0x25e];
        record->primitive_write_count = other.primitive_writes;
        record->draw_primitive_value = other.draw_primitive_value;
        record->draw_primitive_known_mask = other.draw_primitive_known_mask;
        record->draw_count = other.draw_count;
        record->draw_source_offset = other.last_draw_offset;
        record->index_source_offset = (uint32_t)start;
        for (uint32_t j = 0; j < count; ++j) {
            const uint32_t index = width == 2 ? u16(r->data + start + (size_t)j * 2) : r->data[start + j];
            if (index > largest_index) largest_index = index;
            out->indices[out->index_count + j] = mesh->first_vertex + index;
        }
        out->index_count += count;
    }
    mesh->index_count = out->index_count - mesh->first_index;
    mesh->vertex_count = largest_index + 1;
    if (mesh->vertex_count > MAX_VERTICES - out->vertex_count)
        return fail(r, PX_BCH_MODEL_LIMIT_EXCEEDED);
    uint32_t low, high, config, total;
    if (!value(&enable, 0x201, UINT32_MAX, &low) || !value(&enable, 0x202, UINT32_MAX, &high)) return 0;
    const uint64_t formats = low | (uint64_t)high << 32;
    if (!value(&enable, 0x2bb, UINT32_MAX, &low) || !value(&enable, 0x2bc, UINT32_MAX, &high)) return 0;
    const uint64_t permutation = low | (uint64_t)high << 32;
    if (!value(&enable, 0x205, UINT32_MAX, &config) || !value(&enable, 0x204, UINT32_MAX, &low) ||
        !value(&enable, 0x242, 255, &total)) return 0;
    const uint64_t order = low | (uint64_t)(config & 0xffff) << 32;
    const unsigned stride = (config >> 16) & 255, stored = config >> 28;
    const unsigned fixed = (unsigned)(formats >> 48) & 0xfff;
    unsigned fixed_count = 0;
    for (unsigned j = 0; j < 12; ++j) fixed_count += (fixed >> j) & 1;
    ++total;
    if (total > 12 || total != (formats >> 60) + 1 || !stride || !stored || stored > total ||
        stored + fixed_count != total || (fixed >> total)) return fail(r, PX_BCH_MODEL_UNSUPPORTED);
    for (unsigned reg = 0x206; reg < 0x227; ++reg)
        if (enable.known[reg] && enable.values[reg]) return fail(r, PX_BCH_MODEL_UNSUPPORTED);
    if (enable.uniform_mask != 7) return 0;
    struct attribute attributes[12];
    unsigned slots = 0, semantics = 0, byte = 0;
    for (unsigned j = 0; j < stored; ++j) {
        const unsigned slot = (unsigned)(order >> (j * 4)) & 15;
        if (slot >= total || (fixed & (1u << slot)) || (slots & (1u << slot))) return 0;
        slots |= 1u << slot;
        const unsigned semantic = (unsigned)(permutation >> (slot * 4)) & 15;
        if (semantic > 6 || (semantics & (1u << semantic))) return fail(r, PX_BCH_MODEL_UNSUPPORTED);
        semantics |= 1u << semantic;
        const unsigned fmt = (unsigned)(formats >> (slot * 4)) & 15;
        const unsigned kind = fmt & 3, elements = (fmt >> 2) + 1;
        const unsigned width = kind < 2 ? 1 : kind == 2 ? 2 : 4;
        if (byte % width || stride % width) return fail(r, PX_BCH_MODEL_UNSUPPORTED);
        struct attribute *attr = attributes + j;
        attr->semantic = semantic; attr->kind = kind; attr->elements = elements; attr->offset = byte;
        attr->scale = enable.uniforms[1 + semantic / 4][semantic % 4];
        if ((semantic == 0 && elements != 3) || (semantic == 4 && elements != 2)) return 0;
        byte += width * elements;
    }
    if (byte > stride || !(semantics & 1)) return 0;
    double fixed_color[4] = {1, 1, 1, 1};
    for (unsigned slot = 0; slot < total; ++slot) if (fixed & (1u << slot)) {
        const unsigned semantic = (unsigned)(permutation >> (slot * 4)) & 15;
        if (semantic > 6 || semantic == 0 || semantic == 4 || (semantics & (1u << semantic)))
            return fail(r, PX_BCH_MODEL_UNSUPPORTED);
        semantics |= 1u << semantic;
        if (semantic != 3) continue;
        if (!(enable.fixed_known & (1u << slot))) return 0;
        const uint32_t *w = enable.fixed_words[slot];
        const uint32_t components[4] = {
            w[2] & 0xffffff,                               /* x: red */
            ((w[1] & 0xffff) << 8) | (w[2] >> 24),         /* y: green */
            ((w[0] & 0xff) << 16) | (w[1] >> 16),          /* z: blue */
            w[0] >> 8                                      /* w: alpha */
        };
        for (unsigned c = 0; c < 4; ++c)
            if (!float24(components[c], &fixed_color[c]) || !bounded(fixed_color[c])) return 0;
        mesh->color_source = 2;
    }
    if (!(semantics & (1u << 4))) return 0;
    size_t start;
    if (!raw_pointer(r, &enable, 0x203, 0x26, (size_t)mesh->vertex_count * stride, &start)) return 0;
    const uint32_t new_count = out->vertex_count + mesh->vertex_count;
    allocation = grow(r, out->positions, (size_t)new_count * 3, sizeof *out->positions);
    if (allocation == NULL) return 0;
    out->positions = allocation;
    allocation = grow(r, out->uvs, (size_t)new_count * 2, sizeof *out->uvs);
    if (allocation == NULL) return 0;
    out->uvs = allocation;
    allocation = grow(r, out->colors, (size_t)new_count * 4, sizeof *out->colors);
    if (allocation == NULL) return 0;
    out->colors = allocation;
    if (semantics_stored_color(attributes, stored)) mesh->color_source = 1;
    for (uint32_t vertex = 0; vertex < mesh->vertex_count; ++vertex)
        for (unsigned c = 0; c < 4; ++c)
            out->colors[((size_t)mesh->first_vertex + vertex) * 4 + c] = (float)fixed_color[c];
    for (uint32_t vertex = 0; vertex < mesh->vertex_count; ++vertex) {
        for (unsigned j = 0; j < stored; ++j) {
            const struct attribute *attr = attributes + j;
            const uint8_t *raw = r->data + start + (size_t)vertex * stride + attr->offset;
            const unsigned width = attr->kind < 2 ? 1 : attr->kind == 2 ? 2 : 4;
            for (unsigned component = 0; component < attr->elements; ++component) {
                const uint8_t *p = raw + component * width;
                double decoded;
                if (attr->kind == 0) decoded = *p < 128 ? *p : (int)*p - 256;
                else if (attr->kind == 1) decoded = *p;
                else if (attr->kind == 2) decoded = u16(p) < 32768 ? u16(p) : (int)u16(p) - 65536;
                else decoded = f32(p);
                if (!bounded(decoded)) return 0;
                decoded *= attr->scale;
                if (!bounded(decoded)) return 0;
                if (attr->semantic == 0) {
                    decoded += enable.uniforms[0][component];
                    if (!bounded(decoded)) return 0;
                    const float number = (float)decoded;
                    out->positions[((size_t)mesh->first_vertex + vertex) * 3 + component] = number;
                    if (!vertex || number < mesh->bounds_min[component]) mesh->bounds_min[component] = number;
                    if (!vertex || number > mesh->bounds_max[component]) mesh->bounds_max[component] = number;
                } else if (attr->semantic == 4) {
                    out->uvs[((size_t)mesh->first_vertex + vertex) * 2 + component] = (float)decoded;
                } else if (attr->semantic == 3) {
                    out->colors[((size_t)mesh->first_vertex + vertex) * 4 + component] = (float)decoded;
                }
            }
        }
    }
    out->vertex_count = new_count;
    return 1;
}
static int read_model(struct reader *r, uint32_t index, struct px_bch_model *out)
{
    if (index >= r->counts[0]) return 0;
    size_t at;
    if (!pointer(r, r->tables[0] + (size_t)index * 4, CONTENTS, 0x98, &at)) return 0;
    static const float identity[12] = {1,0,0,0,0,1,0,0,0,0,1,0};
    if (u32(r->data + at) || u32(r->data + at + 0x6c) || u32(r->data + at + 0x74))
        return fail(r, PX_BCH_MODEL_UNSUPPORTED);
    for (unsigned j = 0; j < 12; ++j)
        if (f32(r->data + at + 4 + j * 4) != identity[j]) return fail(r, PX_BCH_MODEL_UNSUPPORTED);
    struct px_bch_name indexed;
    if (!name(r, at + 0x84, 0, &out->name) ||
        !name(r, r->trees[0] + ((size_t)index + 1) * 12 + 8, 0, &indexed) ||
        !same_name(&out->name, &indexed) || !read_materials(r, at, out)) return 0;
    const uint32_t count = u32(r->data + at + 0x44), nodes = u32(r->data + at + 0x88);
    if (!count || count > MAX_ITEMS || !nodes || nodes > MAX_ITEMS)
        return fail(r, PX_BCH_MODEL_LIMIT_EXCEEDED);
    if (u32(r->data + at + 0x80) != nodes) return 0;
    size_t meshes, names, visibility;
    if (!pointer(r, at + 0x40, CONTENTS, (size_t)count * 56, &meshes) ||
        !tree(r, at + 0x8c, nodes, &names) ||
        !pointer(r, at + 0x7c, CONTENTS, (nodes + 7) / 8, &visibility)) return 0;
    out->meshes = calloc(count, sizeof *out->meshes);
    if (out->meshes == NULL) return fail(r, PX_BCH_MODEL_NO_MEMORY);
    out->mesh_count = count;
    for (uint32_t i = 0; i < count; ++i) {
        struct px_bch_mesh *mesh = out->meshes + i;
        if (!read_mesh(r, meshes + (size_t)i * 56, names, nodes, visibility, out, mesh)) return 0;
        for (unsigned j = 0; j < 3; ++j) {
            if (!i || mesh->bounds_min[j] < out->bounds_min[j]) out->bounds_min[j] = mesh->bounds_min[j];
            if (!i || mesh->bounds_max[j] > out->bounds_max[j]) out->bounds_max[j] = mesh->bounds_max[j];
        }
    }
    return 1;
}

enum px_bch_model_result px_bch_model_decode(const uint8_t *data, size_t size,
    uint32_t model_index, struct px_bch_model *out)
{
    if (out == NULL) return PX_BCH_MODEL_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    if (data == NULL) return PX_BCH_MODEL_INVALID_ARGUMENT;
    if (size > MAX_BYTES) return PX_BCH_MODEL_LIMIT_EXCEEDED;
    if (size < 56) return PX_BCH_MODEL_TRUNCATED;
    if (memcmp(data, "BCH\0", 4)) return PX_BCH_MODEL_INVALID_HEADER;
    if (data[4] != 7 || data[5] != 7 || u32(data + 48) || u32(data + 52)) return PX_BCH_MODEL_UNSUPPORTED;
    struct reader r = {0};
    r.data = data;
    r.error = PX_BCH_MODEL_INVALID_HEADER;
    for (unsigned i = 0; i < SECTION_COUNT; ++i) {
        const size_t start = u32(data + 8 + i * 4), length = u32(data + 28 + i * 4);
        if (start > size || length > size - start) return PX_BCH_MODEL_TRUNCATED;
        if (length && (start < 56 || (i != STRINGS && start % 4))) return PX_BCH_MODEL_INVALID_HEADER;
        r.sections[i].start = start;
        r.sections[i].end = start + length;
        if (!length) continue;
        for (unsigned j = 0; j < i; ++j)
            if (r.sections[j].end != r.sections[j].start && start < r.sections[j].end &&
                r.sections[j].start < start + length) return PX_BCH_MODEL_INVALID_HEADER;
    }
    const int ok = read_relocations(&r) && read_index(&r) && read_model(&r, model_index, out);
    free(r.relocations);
    if (!ok) { px_bch_model_free(out); return r.error; }
    return PX_BCH_MODEL_OK;
}
void px_bch_model_free(struct px_bch_model *model)
{
    if (model == NULL) return;
    free(model->positions); free(model->uvs); free(model->colors); free(model->indices);
    free(model->meshes); free(model->submeshes); free(model->materials);
    memset(model, 0, sizeof *model);
}
const char *px_bch_model_result_string(enum px_bch_model_result result)
{
    switch (result) {
    case PX_BCH_MODEL_OK: return "success";
    case PX_BCH_MODEL_INVALID_ARGUMENT: return "invalid BCH model argument";
    case PX_BCH_MODEL_TRUNCATED: return "truncated BCH model";
    case PX_BCH_MODEL_INVALID_HEADER: return "invalid BCH geometry, index, relocation, or command";
    case PX_BCH_MODEL_UNSUPPORTED: return "unsupported BCH version or static geometry feature";
    case PX_BCH_MODEL_LIMIT_EXCEEDED: return "BCH geometry resource or command limit exceeded";
    case PX_BCH_MODEL_NO_MEMORY: return "cannot allocate BCH geometry";
    }
    return "unknown BCH model result";
}
