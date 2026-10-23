#include "field.h"

#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* Independent bounded implementation based on original local bytes and:
 * https://github.com/Rynbo/CTRMap/tree/4b780a7bc32a75a7f31c162829b086355caf32f2
 * src/ctrmap/formats/{containers/AbstractGamefreakContainer.java,
 * zone/ZoneHeader.java,mapmatrix/MapMatrix.java,tilemap/Tilemap.java,
 * gfcollision/GRCollisionFile.java,gfcollision/GRCollisionMesh.java}.
 * This module preserves resource data, not the editor's movement assumptions. */

_Static_assert(sizeof(float) == 4 && FLT_RADIX == 2 && FLT_MANT_DIG == 24 &&
               FLT_MAX_EXP == 128, "field decoding requires IEEE binary32");

static uint16_t u16(const uint8_t *p)
{
    return (uint16_t)((uint16_t)p[0] | (uint16_t)p[1] << 8);
}

static uint32_t u32(const uint8_t *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 |
           (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

static int16_t s16(const uint8_t *p)
{
    uint16_t value = u16(p);
    return (int16_t)(value < UINT16_C(0x8000) ? (int)value : (int)value - 65536);
}

static int32_t s32(const uint8_t *p)
{
    uint32_t value = u32(p);
    return (int32_t)(value < UINT32_C(0x80000000) ? (int64_t)value :
                     (int64_t)value - INT64_C(0x100000000));
}

static float f32(const uint8_t *p)
{
    uint32_t bits = u32(p);
    float value;
    memcpy(&value, &bits, sizeof(value));
    return value;
}

static int zero_tail(const uint8_t *data, size_t size, size_t used, size_t limit)
{
    if (used > size || size - used > limit) return 0;
    for (size_t i = used; i < size; ++i) if (data[i]) return 0;
    return 1;
}

enum px_field_result px_field_section(const uint8_t *data, size_t size,
    const char magic[2], uint16_t selected, struct px_field_span *out)
{
    uint16_t count;
    size_t table_end, previous, start = 0, end = 0;
    if (!out) return PX_FIELD_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if (!data || !magic) return PX_FIELD_INVALID_ARGUMENT;
    if (size > PX_FIELD_INPUT_LIMIT) return PX_FIELD_LIMIT_EXCEEDED;
    if (size < 4) return PX_FIELD_TRUNCATED;
    if (memcmp(data, magic, 2)) return PX_FIELD_INVALID_HEADER;
    count = u16(data + 2);
    if (!count) return PX_FIELD_UNSUPPORTED;
    if (count > PX_FIELD_SECTION_LIMIT) return PX_FIELD_LIMIT_EXCEEDED;
    table_end = 4 + ((size_t)count + 1) * 4;
    if (size < table_end) return PX_FIELD_TRUNCATED;
    previous = u32(data + 4);
    if (previous < table_end || previous > size) return PX_FIELD_INVALID_HEADER;
    for (uint16_t i = 0; i < count; ++i) {
        size_t next = u32(data + 8 + (size_t)i * 4);
        if (next < previous || next > size) return PX_FIELD_INVALID_HEADER;
        if (i == selected) { start = previous; end = next; }
        previous = next;
    }
    if (!zero_tail(data, size, previous, 127)) return PX_FIELD_INVALID_HEADER;
    if (selected >= count) return PX_FIELD_NOT_FOUND;
    out->data = data + start;
    out->size = end - start;
    out->offset = start;
    out->section_count = count;
    return PX_FIELD_OK;
}

enum px_field_result px_field_zone_decode(const uint8_t *data, size_t size,
                                         struct px_field_zone *out)
{
    struct px_field_span header;
    enum px_field_result result;
    if (!out) return PX_FIELD_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    result = px_field_section(data, size, "ZO", 0, &header);
    if (result != PX_FIELD_OK) return result;
    if (header.section_count != 4) return PX_FIELD_UNSUPPORTED;
    if (header.size < sizeof(out->raw_header)) return PX_FIELD_TRUNCATED;
    if (header.size != sizeof(out->raw_header)) return PX_FIELD_UNSUPPORTED;
    out->area_id = u16(header.data + 2);
    out->matrix_id = u16(header.data + 4);
    out->map_type = header.data[0];
    out->map_move = header.data[1];
    memcpy(out->raw_header, header.data, sizeof(out->raw_header));
    return PX_FIELD_OK;
}

static enum px_field_result dimensions(const uint8_t *data, uint16_t *width,
                                        uint16_t *height)
{
    int w = s16(data), h = s16(data + 2);
    if (w <= 0 || h <= 0) return PX_FIELD_INVALID_HEADER;
    if ((unsigned)w > PX_FIELD_GRID_SIDE_LIMIT || (unsigned)h > PX_FIELD_GRID_SIDE_LIMIT ||
        (uint64_t)(unsigned)w * (unsigned)h > PX_FIELD_GRID_CELL_LIMIT)
        return PX_FIELD_LIMIT_EXCEEDED;
    *width = (uint16_t)w;
    *height = (uint16_t)h;
    return PX_FIELD_OK;
}

void px_field_matrix_free(struct px_field_matrix *matrix)
{
    if (!matrix) return;
    free(matrix->field_ids);
    free(matrix->zone_ids);
    free(matrix->lod_ids);
    free(matrix->camera_bounds);
    memset(matrix, 0, sizeof(*matrix));
}

enum px_field_result px_field_matrix_decode(const uint8_t *data, size_t size,
                                           struct px_field_matrix *out)
{
    struct px_field_span body, cameras;
    enum px_field_result result;
    size_t cells, records, used, camera_used, camera_capacity;
    if (!out) return PX_FIELD_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    result = px_field_section(data, size, "MM", 0, &body);
    if (result != PX_FIELD_OK) return result;
    if (body.section_count != 2) return PX_FIELD_UNSUPPORTED;
    result = px_field_section(data, size, "MM", 1, &cameras);
    if (result != PX_FIELD_OK) return result;
    if (body.size < 8 || cameras.size < 4) return PX_FIELD_TRUNCATED;
    result = dimensions(body.data + 4, &out->width, &out->height);
    if (result != PX_FIELD_OK) goto failure;
    out->has_lod = u16(body.data);
    out->unknown = u16(body.data + 2);
    if (out->has_lod > 1) { result = PX_FIELD_UNSUPPORTED; goto failure; }
    cells = (size_t)out->width * out->height;
    records = cells * (out->has_lod ? 18 : 1);
    used = 8 + records * 2;
    if (body.size < used) { result = PX_FIELD_TRUNCATED; goto failure; }
    if (!zero_tail(body.data, body.size, used, 3)) { result = PX_FIELD_INVALID_HEADER; goto failure; }
    for (size_t i = 0; i < records; ++i)
        if (s16(body.data + 8 + i * 2) < -1) { result = PX_FIELD_INVALID_HEADER; goto failure; }
    out->camera_count = u32(cameras.data);
    if (out->camera_count > PX_FIELD_CAMERA_LIMIT) { result = PX_FIELD_LIMIT_EXCEEDED; goto failure; }
    camera_used = 4 + (size_t)out->camera_count * 20;
    if (cameras.size < camera_used) { result = PX_FIELD_TRUNCATED; goto failure; }
    /* Original XY matrices reserve16 records, even with fewer active bounds.
     * The reference writer also emits a compact count-sized table. */
    camera_capacity = out->camera_count <= 16 ? 324 : camera_used;
    if (cameras.size != camera_used && cameras.size != camera_capacity) {
        result = PX_FIELD_UNSUPPORTED; goto failure;
    }
    if (!zero_tail(cameras.data, cameras.size, camera_used, camera_capacity - camera_used)) {
        result = PX_FIELD_INVALID_HEADER; goto failure;
    }
    for (uint32_t i = 0; i < out->camera_count; ++i)
        for (unsigned c = 0; c < 4; ++c)
            if (!isfinite(f32(cameras.data + 4 + (size_t)i * 20 + c * 4))) {
                result = PX_FIELD_INVALID_HEADER; goto failure;
            }
    out->field_ids = malloc(cells * sizeof(*out->field_ids));
    if (out->has_lod) {
        out->zone_ids = malloc(cells * 16 * sizeof(*out->zone_ids));
        out->lod_ids = malloc(cells * sizeof(*out->lod_ids));
    }
    if (out->camera_count) out->camera_bounds = calloc(out->camera_count, sizeof(*out->camera_bounds));
    if (!out->field_ids || (out->has_lod && (!out->zone_ids || !out->lod_ids)) ||
        (out->camera_count && !out->camera_bounds)) { result = PX_FIELD_NO_MEMORY; goto failure; }
    for (size_t i = 0; i < cells; ++i) out->field_ids[i] = s16(body.data + 8 + i * 2);
    if (out->has_lod) {
        for (size_t i = 0; i < cells * 16; ++i)
            out->zone_ids[i] = s16(body.data + 8 + (cells + i) * 2);
        for (size_t i = 0; i < cells; ++i)
            out->lod_ids[i] = s16(body.data + 8 + (cells * 17 + i) * 2);
    }
    for (uint32_t i = 0; i < out->camera_count; ++i) {
        const uint8_t *p = cameras.data + 4 + (size_t)i * 20;
        out->camera_bounds[i].north = f32(p);
        out->camera_bounds[i].south = f32(p + 4);
        out->camera_bounds[i].west = f32(p + 8);
        out->camera_bounds[i].east = f32(p + 12);
        out->camera_bounds[i].raw_flag = s32(p + 16);
    }
    return PX_FIELD_OK;
failure:
    px_field_matrix_free(out);
    return result;
}

void px_field_grid_free(struct px_field_grid *grid)
{
    if (!grid) return;
    free(grid->tiles);
    memset(grid, 0, sizeof(*grid));
}

enum px_field_result px_field_grid_decode(const uint8_t *data, size_t size,
                                         struct px_field_grid *out)
{
    struct px_field_span tiles;
    enum px_field_result result;
    size_t cells, used;
    if (!out) return PX_FIELD_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    result = px_field_section(data, size, "GR", 0, &tiles);
    if (result != PX_FIELD_OK) return result;
    if (tiles.section_count != 6) return PX_FIELD_UNSUPPORTED;
    if (tiles.size < 4) return PX_FIELD_TRUNCATED;
    result = dimensions(tiles.data, &out->width, &out->height);
    if (result != PX_FIELD_OK) goto failure;
    cells = (size_t)out->width * out->height;
    used = 4 + cells * 4;
    if (tiles.size < used) { result = PX_FIELD_TRUNCATED; goto failure; }
    if (!zero_tail(tiles.data, tiles.size, used, 127)) { result = PX_FIELD_INVALID_HEADER; goto failure; }
    for (uint16_t i = 0; i < 6; ++i) {
        struct px_field_span section;
        result = px_field_section(data, size, "GR", i, &section);
        if (result != PX_FIELD_OK) goto failure;
        out->sections[i].offset = section.offset;
        out->sections[i].size = section.size;
        memcpy(out->sections[i].head, section.data, section.size < 4 ? section.size : 4);
    }
    out->tiles = malloc(cells * sizeof(*out->tiles));
    if (!out->tiles) { result = PX_FIELD_NO_MEMORY; goto failure; }
    for (size_t i = 0; i < cells; ++i) out->tiles[i] = u32(tiles.data + 4 + i * 4);
    return PX_FIELD_OK;
failure:
    px_field_grid_free(out);
    return result;
}

void px_field_collision_free(struct px_field_collision *collision)
{
    if (!collision) return;
    free(collision->positions);
    memset(collision, 0, sizeof(*collision));
}

enum px_field_result px_field_collision_decode(const uint8_t *gr, size_t size,
                                               struct px_field_collision *out)
{
    static const uint32_t constants[6] = {0x5d8, 1, 3, 2, 1, 2};
    struct px_field_span section;
    enum px_field_result result;
    uint32_t total = 0;
    size_t end;
    if (!out) return PX_FIELD_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    result = px_field_section(gr, size, "GR", 2, &section);
    if (result != PX_FIELD_OK) return result;
    if (section.section_count != 6) return PX_FIELD_UNSUPPORTED;
    if (section.size < 816) return PX_FIELD_TRUNCATED;
    if (memcmp(section.data, "coll", 4)) return PX_FIELD_INVALID_HEADER;
    for (unsigned i = 0; i < 6; ++i)
        if (u32(section.data + 8 + i * 4) != constants[i]) return PX_FIELD_UNSUPPORTED;
    for (unsigned i = 0; i < 16; ++i) {
        uint32_t first = u32(section.data + 672 + i * 8);
        uint32_t count = u32(section.data + 676 + i * 8);
        if (count > PX_FIELD_COLLISION_VERTEX_LIMIT - total) { result = PX_FIELD_LIMIT_EXCEEDED; goto failure; }
        if (first != total || count % 3) { result = PX_FIELD_INVALID_HEADER; goto failure; }
        out->first_vertex[i] = first;
        out->group_vertex_count[i] = count;
        total += count;
    }
    end = 800 + (size_t)total * 16;
    if (section.size < end + 16) { result = PX_FIELD_TRUNCATED; goto failure; }
    if ((size_t)u32(section.data + 4) != end - 16 ||
        memcmp(section.data + end, "term", 4) || u32(section.data + end + 4) ||
        u32(section.data + end + 8) != 0x5d8 || u32(section.data + end + 12) != 1 ||
        !zero_tail(section.data, section.size, end + 16, 127)) {
        result = PX_FIELD_INVALID_HEADER; goto failure;
    }
    /* Bounds consist of40 four-float records; XYZ are coordinates while the
     * fourth component is retained only as validated finite metadata. */
    for (size_t p = 32; p < 672; p += 4)
        if (!isfinite(f32(section.data + p))) { result = PX_FIELD_INVALID_HEADER; goto failure; }
    for (uint32_t i = 0; i < total; ++i) for (unsigned c = 0; c < 4; ++c) {
        float value = f32(section.data + 800 + (size_t)i * 16 + c * 4);
        if (!isfinite(value)) { result = PX_FIELD_INVALID_HEADER; goto failure; }
        if (c < 3 && fabs((double)value) > 1e12) { result = PX_FIELD_LIMIT_EXCEEDED; goto failure; }
    }
    if (total) {
        out->positions = malloc((size_t)total * 3 * sizeof(*out->positions));
        if (!out->positions) { result = PX_FIELD_NO_MEMORY; goto failure; }
    }
    out->vertex_count = total;
    for (uint32_t i = 0; i < total; ++i) for (unsigned c = 0; c < 3; ++c) {
        float value = f32(section.data + 800 + (size_t)i * 16 + c * 4);
        out->positions[(size_t)i * 3 + c] = value;
        if (!i || value < out->bounds_min[c]) out->bounds_min[c] = value;
        if (!i || value > out->bounds_max[c]) out->bounds_max[c] = value;
    }
    return PX_FIELD_OK;
failure:
    px_field_collision_free(out);
    return result;
}

enum px_field_result px_field_collision_first_height(
    const struct px_field_collision *collision, double x, double z, double *height)
{
    if (!height) return PX_FIELD_INVALID_ARGUMENT;
    *height = 0;
    if (!collision || !isfinite(x) || !isfinite(z) || collision->vertex_count % 3 ||
        (collision->vertex_count && !collision->positions)) return PX_FIELD_INVALID_ARGUMENT;
    if (collision->vertex_count > PX_FIELD_COLLISION_VERTEX_LIMIT || fabs(x) > 1e12 || fabs(z) > 1e12)
        return PX_FIELD_LIMIT_EXCEEDED;
    for (uint32_t i = 0; i < collision->vertex_count; i += 3) {
        const float *p = collision->positions + (size_t)i * 3;
        double ax = (double)p[3] - p[0], az = (double)p[5] - p[2];
        double bx = (double)p[6] - p[0], bz = (double)p[8] - p[2];
        double qx = x - p[0], qz = z - p[2];
        double determinant = ax * bz - az * bx;
        double numerator_u, numerator_v, u, v;
        if (!isfinite(determinant)) return PX_FIELD_INVALID_ARGUMENT;
        if (determinant == 0) continue; /* Vertical or XZ-degenerate triangle. */
        numerator_u = qx * bz - qz * bx;
        numerator_v = ax * qz - az * qx;
        if (determinant > 0 ?
            (numerator_u < 0 || numerator_v < 0 || numerator_u + numerator_v > determinant) :
            (numerator_u > 0 || numerator_v > 0 || numerator_u + numerator_v < determinant)) continue;
        u = numerator_u / determinant;
        v = numerator_v / determinant;
        *height = p[1] + u * ((double)p[4] - p[1]) + v * ((double)p[7] - p[1]);
        if (!isfinite(*height)) { *height = 0; return PX_FIELD_INVALID_ARGUMENT; }
        return PX_FIELD_OK;
    }
    return PX_FIELD_NOT_FOUND;
}

const char *px_field_result_string(enum px_field_result result)
{
    switch (result) {
    case PX_FIELD_OK: return "ok";
    case PX_FIELD_INVALID_ARGUMENT: return "invalid field argument";
    case PX_FIELD_TRUNCATED: return "truncated field resource";
    case PX_FIELD_INVALID_HEADER: return "invalid field structure";
    case PX_FIELD_UNSUPPORTED: return "unsupported field layout";
    case PX_FIELD_LIMIT_EXCEEDED: return "field resource limit exceeded";
    case PX_FIELD_NO_MEMORY: return "out of memory";
    case PX_FIELD_NOT_FOUND: return "field item or geometric hit not found";
    }
    return "unknown field result";
}
