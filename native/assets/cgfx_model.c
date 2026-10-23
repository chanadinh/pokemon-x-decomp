#include "cgfx_model.h"

#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_LIMIT (64u * 1024u * 1024u)
#define VERTEX_LIMIT 1048576u
#define INDEX_LIMIT 3145728u
#define SHAPE_LIMIT 1024u
#define RECORD_LIMIT 65536u

_Static_assert(sizeof(float) == 4 && FLT_RADIX == 2 && FLT_MANT_DIG == 24 &&
               FLT_MAX_EXP == 128, "CGFX decoding requires IEEE binary32 float");

struct reader {
    const uint8_t *data;
    size_t size, data_start, data_end, image_start, image_end;
    uint32_t records, vertices, indices;
    enum px_cgfx_model_result error;
};

static int fail(struct reader *r, enum px_cgfx_model_result error) {
    if (r->error == PX_CGFX_MODEL_OK) r->error = error;
    return 0;
}

static int span(struct reader *r, size_t offset, size_t length,
                size_t lower, size_t upper) {
    if (offset < lower || offset > upper || length > upper - offset)
        return fail(r, PX_CGFX_MODEL_INVALID_HEADER);
    return 1;
}

static int meta(struct reader *r, size_t offset, size_t length) {
    return span(r, offset, length, r->data_start, r->data_end);
}

static uint32_t u32(const struct reader *r, size_t offset) {
    const uint8_t *p = r->data + offset;
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 |
           (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

static float f32(const struct reader *r, size_t offset) {
    uint32_t bits = u32(r, offset);
    float value;
    memcpy(&value, &bits, sizeof(value));
    return value;
}

static int relative(struct reader *r, size_t field, size_t length,
                    int image, size_t *target) {
    uint32_t raw;
    int64_t displacement, position;
    if (!meta(r, field, 4)) return 0;
    raw = u32(r, field);
    if (!raw) return fail(r, PX_CGFX_MODEL_INVALID_HEADER);
    displacement = raw < UINT32_C(0x80000000) ? (int64_t)raw :
                   (int64_t)raw - INT64_C(0x100000000);
    position = (int64_t)field + displacement;
    if (position < 0 || (uint64_t)position > r->size)
        return fail(r, PX_CGFX_MODEL_INVALID_HEADER);
    *target = (size_t)position;
    return image ? span(r, *target, length, r->image_start, r->image_end) :
                   meta(r, *target, length);
}

static int table(struct reader *r, size_t field, uint32_t count, size_t *target) {
    if (!count || count > RECORD_LIMIT - r->records)
        return fail(r, count ? PX_CGFX_MODEL_LIMIT_EXCEEDED : PX_CGFX_MODEL_UNSUPPORTED);
    r->records += count;
    return relative(r, field, (size_t)count * 4, 0, target);
}

static int name(struct reader *r, size_t field, char *output) {
    size_t target, length;
    if (!relative(r, field, 1, 0, &target)) return 0;
    for (length = 0; length <= 1024 && length < r->data_end - target; ++length) {
        if (!r->data[target + length]) {
            if (output) {
                memcpy(output, r->data + target, length);
                output[length] = '\0';
            }
            return 1;
        }
    }
    return fail(r, PX_CGFX_MODEL_INVALID_HEADER);
}

static int container(struct reader *r, uint32_t selected, size_t *model) {
    size_t cursor, dictionary, record;
    uint32_t blocks, count, i, total = 0;
    if (r->size < 20) return fail(r, PX_CGFX_MODEL_TRUNCATED);
    if (memcmp(r->data, "CGFX\xff\xfe\x14\0", 8) || u32(r, 12) != r->size)
        return fail(r, PX_CGFX_MODEL_INVALID_HEADER);
    if (u32(r, 8) != UINT32_C(0x05000000)) return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
    blocks = u32(r, 16);
    if (blocks != 2) return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
    cursor = 20;
    for (i = 0; i < blocks; ++i) {
        uint32_t length;
        if (!span(r, cursor, 8, 0, r->size)) return 0;
        length = u32(r, cursor + 4);
        if (length < 8 || memcmp(r->data + cursor, i ? "IMAG" : "DATA", 4))
            return fail(r, PX_CGFX_MODEL_INVALID_HEADER);
        if (!span(r, cursor, length, 0, r->size)) return 0;
        if (!i) { r->data_start = cursor + 8; r->data_end = cursor + length; }
        else { r->image_start = cursor + 8; r->image_end = cursor + length; }
        cursor += length;
    }
    if (cursor != r->size || !meta(r, 28, 128)) return fail(r, PX_CGFX_MODEL_INVALID_HEADER);
    for (i = 0; i < 16; ++i) {
        uint32_t entries = u32(r, 28 + i * 8);
        if (entries > RECORD_LIMIT - total) return fail(r, PX_CGFX_MODEL_LIMIT_EXCEEDED);
        total += entries;
    }
    count = u32(r, 28);
    if (selected >= count) return fail(r, PX_CGFX_MODEL_INVALID_ARGUMENT);
    if (!relative(r, 32, 28, 0, &dictionary)) return 0;
    if (memcmp(r->data + dictionary, "DICT", 4) || u32(r, dictionary + 8) != count ||
        u32(r, dictionary + 4) != 28 + count * 16)
        return fail(r, PX_CGFX_MODEL_INVALID_HEADER);
    if (!meta(r, dictionary, 28 + (size_t)count * 16)) return 0;
    record = dictionary + 28 + selected * 16;
    if (!name(r, record + 8, NULL) || !relative(r, record + 12, 0xe0, 0, model)) return 0;
    if (memcmp(r->data + *model + 4, "CMDL", 4)) return fail(r, PX_CGFX_MODEL_INVALID_HEADER);
    if ((u32(r, *model) & 0x80) || u32(r, *model + 8) != UINT32_C(0x09000000))
        return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
    return 1;
}

static int identity_transform(struct reader *r, size_t model) {
    unsigned i;
    for (i = 0; i < 9; ++i) {
        float value = f32(r, model + 0x30 + i * 4);
        if (!isfinite(value)) return fail(r, PX_CGFX_MODEL_INVALID_HEADER);
        if (value != (i < 3 ? 1.0f : 0.0f)) return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
    }
    for (i = 0; i < 24; ++i) {
        unsigned element = i % 12;
        float value = f32(r, model + 0x54 + i * 4);
        float expected = element == 0 || element == 5 || element == 10 ? 1.0f : 0.0f;
        if (!isfinite(value)) return fail(r, PX_CGFX_MODEL_INVALID_HEADER);
        if (value != expected) return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
    }
    return 1;
}

static int read_component(struct reader *r, size_t offset, uint32_t format, float *value) {
    switch (format) {
    case 0x1400: *value = r->data[offset] < 128 ? r->data[offset] : (int)r->data[offset] - 256; break;
    case 0x1401: *value = r->data[offset]; break;
    case 0x1402: {
        uint32_t raw = (uint32_t)r->data[offset] | (uint32_t)r->data[offset + 1] << 8;
        *value = (float)(raw < 32768 ? (int)raw : (int)raw - 65536);
        break;
    }
    case 0x1406: *value = f32(r, offset); break;
    default: return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
    }
    return isfinite(*value) ? 1 : fail(r, PX_CGFX_MODEL_INVALID_HEADER);
}

static int shape(struct reader *r, size_t offset, struct px_cgfx_model *output) {
    size_t groups, stream = 0, buffer, attributes, position = 0, faces;
    uint32_t length, stride, count, vertices, position_format = 0, component_size = 0;
    uint32_t base = r->vertices, attribute_mask = 0, i;
    float scale = 0, translation[3];
    if (!meta(r, offset, 68)) return 0;
    if (memcmp(r->data + offset + 4, "SOBJ", 4)) return fail(r, PX_CGFX_MODEL_INVALID_HEADER);
    if (u32(r, offset) != UINT32_C(0x10000001) || u32(r, offset + 8) != 0)
        return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
    if (u32(r, offset + 64)) {
        size_t blend;
        if (!relative(r, offset + 64, 20, 0, &blend)) return 0;
        for (i = 0; i < 5; ++i)
            if (u32(r, blend + i * 4)) return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
    }
    for (i = 0; i < 3; ++i) {
        translation[i] = f32(r, offset + 32 + i * 4);
        if (!isfinite(translation[i])) return fail(r, PX_CGFX_MODEL_INVALID_HEADER);
    }
    count = u32(r, offset + 56);
    if (!count || count > 16) return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
    if (!table(r, offset + 60, count, &groups)) return 0;
    for (i = 0; i < count; ++i) {
        size_t group;
        if (!relative(r, groups + i * 4, 12, 0, &group)) return 0;
        if (u32(r, group) == UINT32_C(0x40000002)) {
            if (stream || u32(r, group + 8) != 2) return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
            if (!meta(r, group, 48)) return 0;
            stream = group;
        } else if (u32(r, group) == UINT32_C(0x80000000)) {
            size_t vector;
            uint32_t semantic = u32(r, group + 4), elements, j;
            if (!meta(r, group, 32)) return 0;
            elements = u32(r, group + 16);
            if (!semantic || semantic > 6 || u32(r, group + 8) != 1 ||
                u32(r, group + 12) != 0x1406 || !elements || elements > 4 ||
                u32(r, group + 24) != elements)
                return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
            if (!isfinite(f32(r, group + 20)) || (attribute_mask & (1u << semantic)))
                return fail(r, PX_CGFX_MODEL_INVALID_HEADER);
            attribute_mask |= 1u << semantic;
            if (!relative(r, group + 28, elements * 4, 0, &vector)) return 0;
            for (j = 0; j < elements; ++j)
                if (!isfinite(f32(r, vector + j * 4))) return fail(r, PX_CGFX_MODEL_INVALID_HEADER);
        } else return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
    }
    if (!stream) return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
    length = u32(r, stream + 20); stride = u32(r, stream + 36); count = u32(r, stream + 40);
    if (!stride || stride > 4096 || !length || length % stride || !count || count > 16)
        return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
    vertices = length / stride;
    if (vertices > VERTEX_LIMIT - r->vertices) return fail(r, PX_CGFX_MODEL_LIMIT_EXCEEDED);
    if (!relative(r, stream + 24, length, 1, &buffer) ||
        !table(r, stream + 44, count, &attributes)) return 0;
    for (i = 0; i < count; ++i) {
        size_t attribute;
        uint32_t semantic, format, elements, bytes, local_offset;
        float local_scale;
        if (!relative(r, attributes + i * 4, 52, 0, &attribute)) return 0;
        semantic = u32(r, attribute + 4); format = u32(r, attribute + 36);
        elements = u32(r, attribute + 40); local_offset = u32(r, attribute + 48);
        local_scale = f32(r, attribute + 44);
        if (u32(r, attribute) != UINT32_C(0x40000001) || u32(r, attribute + 8) != 0 ||
            u32(r, attribute + 20) || u32(r, attribute + 24) || semantic > 6 ||
            !elements || elements > 4)
            return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
        if (!isfinite(local_scale)) return fail(r, PX_CGFX_MODEL_INVALID_HEADER);
        if (attribute_mask & (1u << semantic)) return fail(r, PX_CGFX_MODEL_INVALID_HEADER);
        attribute_mask |= 1u << semantic;
        switch (format) {
        case 0x1400: case 0x1401: bytes = 1; break;
        case 0x1402: bytes = 2; break;
        case 0x1406: bytes = 4; break;
        default: return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
        }
        if (local_offset > stride || elements * bytes > stride - local_offset)
            return fail(r, PX_CGFX_MODEL_INVALID_HEADER);
        if (!semantic) {
            if (elements != 3) return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
            position = local_offset; position_format = format; component_size = bytes; scale = local_scale;
        }
    }
    if (!(attribute_mask & 1)) return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
    for (i = 0; i < vertices; ++i) {
        unsigned axis;
        for (axis = 0; axis < 3; ++axis) {
            float raw, value;
            if (!read_component(r, buffer + (size_t)i * stride + position + axis * component_size,
                                position_format, &raw)) return 0;
            value = raw * scale + translation[axis];
            if (!isfinite(value)) return fail(r, PX_CGFX_MODEL_INVALID_HEADER);
            if (fabsf(value) > 1.0e12f) return fail(r, PX_CGFX_MODEL_LIMIT_EXCEEDED);
            if (output) {
                output->positions[((size_t)base + i) * 3 + axis] = value;
                if (!base && !i) output->bounds_min[axis] = output->bounds_max[axis] = value;
                else {
                    if (value < output->bounds_min[axis]) output->bounds_min[axis] = value;
                    if (value > output->bounds_max[axis]) output->bounds_max[axis] = value;
                }
            }
        }
    }
    r->vertices += vertices;
    count = u32(r, offset + 44);
    if (!table(r, offset + 48, count, &faces)) return 0;
    for (i = 0; i < count; ++i) {
        size_t submesh, face_table;
        uint32_t face_count, j;
        if (!relative(r, faces + i * 4, 20, 0, &submesh)) return 0;
        if (u32(r, submesh) || u32(r, submesh + 8)) return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
        face_count = u32(r, submesh + 12);
        if (!table(r, submesh + 16, face_count, &face_table)) return 0;
        for (j = 0; j < face_count; ++j) {
            size_t face, descriptor_table;
            uint32_t descriptor_count, k;
            if (!relative(r, face_table + j * 4, 20, 0, &face)) return 0;
            descriptor_count = u32(r, face);
            if (!table(r, face + 4, descriptor_count, &descriptor_table)) return 0;
            for (k = 0; k < descriptor_count; ++k) {
                size_t descriptor, index_data;
                uint32_t format, index_length, index_count, index_size, index;
                if (!relative(r, descriptor_table + k * 4, 16, 0, &descriptor)) return 0;
                format = u32(r, descriptor);
                if ((format != 0x1401 && format != 0x1403) || r->data[descriptor + 4] != 0)
                    return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
                index_size = format == 0x1403 ? 2 : 1;
                index_length = u32(r, descriptor + 8);
                if (!index_length || index_length % index_size) return fail(r, PX_CGFX_MODEL_INVALID_HEADER);
                index_count = index_length / index_size;
                if (index_count % 3) return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
                if (index_count > INDEX_LIMIT - r->indices) return fail(r, PX_CGFX_MODEL_LIMIT_EXCEEDED);
                if (!relative(r, descriptor + 12, index_length, 1, &index_data)) return 0;
                for (index = 0; index < index_count; ++index) {
                    size_t source = index_data + (size_t)index * index_size;
                    uint32_t value = r->data[source];
                    if (index_size == 2) value |= (uint32_t)r->data[source + 1] << 8;
                    if (value >= vertices) return fail(r, PX_CGFX_MODEL_INVALID_HEADER);
                    if (output) output->indices[r->indices + index] = base + value;
                }
                r->indices += index_count;
            }
        }
    }
    return 1;
}

static int geometry(struct reader *r, size_t model, struct px_cgfx_model *output,
                    uint32_t *shape_count) {
    size_t shapes, objects;
    uint32_t count = u32(r, model + 0xc4), object_count = u32(r, model + 0xb4), i;
    uint8_t seen[SHAPE_LIMIT] = {0};
    if (!count || count > SHAPE_LIMIT) return fail(r, PX_CGFX_MODEL_LIMIT_EXCEEDED);
    if (object_count != count) return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
    if (!table(r, model + 0xc8, count, &shapes) || !table(r, model + 0xb8, count, &objects)) return 0;
    for (i = 0; i < count; ++i) {
        size_t object, owner;
        uint32_t reference;
        if (!relative(r, objects + i * 4, 44, 0, &object)) return 0;
        if (u32(r, object) != UINT32_C(0x01000000) || u32(r, object + 8) != 0 ||
            memcmp(r->data + object + 4, "SOBJ", 4))
            return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
        reference = u32(r, object + 24);
        if (reference >= count || seen[reference] ||
            u32(r, object + 28) >= u32(r, model + 0xbc))
            return fail(r, PX_CGFX_MODEL_INVALID_HEADER);
        seen[reference] = 1;
        if (!relative(r, object + 32, 0xe0, 0, &owner)) return 0;
        if (owner != model) return fail(r, PX_CGFX_MODEL_UNSUPPORTED);
    }
    for (i = 0; i < count; ++i) {
        size_t shape_offset;
        if (!relative(r, shapes + i * 4, 68, 0, &shape_offset) || !shape(r, shape_offset, output)) return 0;
    }
    *shape_count = count;
    return 1;
}

enum px_cgfx_model_result px_cgfx_model_decode(const uint8_t *data, size_t size,
                                             uint32_t model_index,
                                             struct px_cgfx_model *out) {
    struct reader r;
    size_t model;
    uint32_t shapes;
    if (!out) return PX_CGFX_MODEL_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if (!data) return PX_CGFX_MODEL_INVALID_ARGUMENT;
    if (size > INPUT_LIMIT) return PX_CGFX_MODEL_LIMIT_EXCEEDED;
    memset(&r, 0, sizeof(r)); r.data = data; r.size = size;
    if (!container(&r, model_index, &model) || !identity_transform(&r, model) ||
        !name(&r, model + 12, out->name) || !geometry(&r, model, NULL, &shapes)) goto failure;
    out->positions = malloc((size_t)r.vertices * 3 * sizeof(float));
    out->indices = malloc((size_t)r.indices * sizeof(uint32_t));
    if (!out->positions || !out->indices) { r.error = PX_CGFX_MODEL_NO_MEMORY; goto failure; }
    out->vertex_count = r.vertices; out->index_count = r.indices; out->shape_count = shapes;
    r.vertices = r.indices = r.records = 0;
    if (!geometry(&r, model, out, &shapes)) goto failure;
    return PX_CGFX_MODEL_OK;
failure:
    px_cgfx_model_free(out);
    return r.error;
}

void px_cgfx_model_free(struct px_cgfx_model *model) {
    if (!model) return;
    free(model->positions); free(model->indices); memset(model, 0, sizeof(*model));
}

const char *px_cgfx_model_result_string(enum px_cgfx_model_result result) {
    switch (result) {
    case PX_CGFX_MODEL_OK: return "ok";
    case PX_CGFX_MODEL_INVALID_ARGUMENT: return "invalid argument or model selection";
    case PX_CGFX_MODEL_TRUNCATED: return "truncated input";
    case PX_CGFX_MODEL_INVALID_HEADER: return "invalid CGFX geometry data";
    case PX_CGFX_MODEL_UNSUPPORTED: return "unsupported CGFX geometry layout";
    case PX_CGFX_MODEL_LIMIT_EXCEEDED: return "CGFX geometry budget exceeded";
    case PX_CGFX_MODEL_NO_MEMORY: return "allocation failure";
    }
    return "unknown error";
}
