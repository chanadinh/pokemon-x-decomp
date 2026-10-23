#include "field.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(c) do { if (!(c)) { \
    fprintf(stderr, "%s:%d: failed: %s\n", __FILE__, __LINE__, #c); \
    exit(EXIT_FAILURE); \
} } while (0)

enum { CAPACITY = 8192 };

static void put16(uint8_t *p, uint16_t value)
{
    p[0] = (uint8_t)value; p[1] = (uint8_t)(value >> 8);
}

static void put32(uint8_t *p, uint32_t value)
{
    put16(p, (uint16_t)value); put16(p + 2, (uint16_t)(value >> 16));
}

static uint32_t get32(const uint8_t *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 |
           (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

static void put_float(uint8_t *p, float value)
{
    uint32_t bits; memcpy(&bits, &value, 4); put32(p, bits);
}

static size_t pack(uint8_t data[CAPACITY], const char magic[2], unsigned count,
                   const uint8_t *const sections[], const size_t lengths[], unsigned alignment)
{
    size_t offset = (4 + 4 * (count + 1) + alignment - 1) / alignment * alignment;
    memset(data, 0, CAPACITY);
    memcpy(data, magic, 2); put16(data + 2, (uint16_t)count);
    for (unsigned i = 0; i < count; ++i) {
        CHECK(offset + lengths[i] < CAPACITY);
        put32(data + 4 + i * 4, (uint32_t)offset);
        if (lengths[i]) memcpy(data + offset, sections[i], lengths[i]);
        offset = (offset + lengths[i] + alignment - 1) / alignment * alignment;
    }
    put32(data + 4 + count * 4, (uint32_t)offset);
    return offset;
}

static size_t zone_fixture(uint8_t data[CAPACITY])
{
    uint8_t header[56] = {3, 0};
    const uint8_t *parts[] = {header, NULL, NULL, NULL};
    const size_t lengths[] = {sizeof header, 0, 0, 0};
    put16(header + 2, 75); put16(header + 4, 12);
    put32(header + 44, 81); put32(header + 52, 297);
    return pack(data, "ZO", 4, parts, lengths, 4);
}

static size_t matrix_fixture(uint8_t data[CAPACITY], int lod)
{
    uint8_t matrix[80] = {0}, camera[324] = {0};
    size_t size = 12;
    put16(matrix, (uint16_t)lod); put16(matrix + 2, 10);
    put16(matrix + 4, 2); put16(matrix + 6, 1);
    put16(matrix + 8, 7); put16(matrix + 10, UINT16_MAX);
    if (lod) {
        /* The finer zone plane is row-major8x4, not 2x16. */
        for (unsigned i = 0; i < 32; ++i) put16(matrix + 12 + i * 2, (uint16_t)(100 + i));
        put16(matrix + 76, 9); put16(matrix + 78, UINT16_MAX);
        size = 80;
    }
    put32(camera, 1);
    put_float(camera + 4, -20); put_float(camera + 8, 740);
    put_float(camera + 12, -40); put_float(camera + 16, 1440);
    put32(camera + 20, 1);
    const uint8_t *parts[] = {matrix, camera};
    const size_t lengths[] = {size, sizeof camera};
    return pack(data, "MM", 2, parts, lengths, 4);
}

static size_t grid_fixture(uint8_t data[CAPACITY])
{
    uint8_t grid[28] = {0}, collision[960] = {0};
    static const uint8_t model[4] = {'B', 'C', 'H', 0};
    static const uint32_t words[6] = {0x20, 0x01000021, 0x20000000,
                                     0xef000021, 0x28, 0x20};
    put16(grid, 2); put16(grid + 2, 3);
    for (unsigned i = 0; i < 6; ++i) put32(grid + 4 + i * 4, words[i]);
    memcpy(collision, "coll", 4); put32(collision + 4, sizeof collision - 32);
    put32(collision + 8, 0x5d8);
    const unsigned constants[] = {1, 3, 2, 1, 2};
    for (unsigned i = 0; i < 5; ++i) put32(collision + 12 + i * 4, constants[i]);
    for (unsigned i = 0; i < 16; ++i) {
        put32(collision + 672 + i * 8, i < 3 ? i * 3 : 9);
        put32(collision + 676 + i * 8, i < 3 ? 3 : 0);
    }
    const float vertices[9][3] = {
        {0,0,0}, {0,4,0}, {0,0,4}, /* vertical: no XZ area */
        {0,0,0}, {4,4,0}, {0,8,4}, /* y = x + 2*z */
        {0,20,0}, {4,20,0}, {0,20,4} /* overlaps, later in stored order */
    };
    for (unsigned i = 0; i < 9; ++i) {
        for (unsigned axis = 0; axis < 3; ++axis)
            put_float(collision + 800 + i * 16 + axis * 4, vertices[i][axis]);
        put_float(collision + 812 + i * 16, 1);
    }
    memcpy(collision + 944, "term", 4);
    put32(collision + 952, 0x5d8); put32(collision + 956, 1);
    const uint8_t *parts[] = {grid, model, collision, NULL, NULL, NULL};
    const size_t lengths[] = {sizeof grid, sizeof model, sizeof collision, 0, 0, 0};
    return pack(data, "GR", 6, parts, lengths, 128);
}

static void test_sections(void)
{
    uint8_t data[CAPACITY]; struct px_field_span span = {0};
    size_t size = zone_fixture(data);
    CHECK(px_field_section(data, size, "ZO", 0, &span) == PX_FIELD_OK);
    CHECK(span.offset == 24 && span.size == 56 && span.section_count == 4);
    CHECK(span.data == data + 24);
    CHECK(px_field_section(data, size, "ZO", 3, &span) == PX_FIELD_OK && span.size == 0);
    CHECK(px_field_section(data, size + 127, "ZO", 0, &span) == PX_FIELD_OK);
    CHECK(px_field_section(data, size + 128, "ZO", 0, &span) != PX_FIELD_OK);
    data[size] = 1;
    CHECK(px_field_section(data, size + 1, "ZO", 0, &span) != PX_FIELD_OK);
    size = zone_fixture(data);
    put32(data + 16, 12); /* Invalid unselected section must still reject. */
    CHECK(px_field_section(data, size, "ZO", 0, &span) != PX_FIELD_OK);
    size = zone_fixture(data); put32(data + 4, 20);
    CHECK(px_field_section(data, size, "ZO", 0, &span) != PX_FIELD_OK);
    size = zone_fixture(data); put32(data + 20, (uint32_t)size + 1);
    CHECK(px_field_section(data, size, "ZO", 0, &span) != PX_FIELD_OK);
    size = zone_fixture(data);
    CHECK(px_field_section(data, size, "MM", 0, &span) != PX_FIELD_OK);
    CHECK(px_field_section(data, size, "ZO", 4, &span) != PX_FIELD_OK);
    for (size_t n = 0; n < 24; ++n)
        CHECK(px_field_section(data, n, "ZO", 0, &span) != PX_FIELD_OK);
    CHECK(px_field_section(NULL, size, "ZO", 0, &span) != PX_FIELD_OK);
    CHECK(px_field_section(data, size, "ZO", 0, NULL) != PX_FIELD_OK);
    CHECK(px_field_section(data, (size_t)PX_FIELD_INPUT_LIMIT + 1, "ZO", 0, &span) != PX_FIELD_OK);
}

static void test_zones_and_matrices(void)
{
    uint8_t data[CAPACITY], original[56]; struct px_field_zone zone = {0};
    size_t size = zone_fixture(data); memcpy(original, data + 24, 56);
    CHECK(px_field_zone_decode(data, size, &zone) == PX_FIELD_OK);
    CHECK(zone.map_type == 3 && zone.map_move == 0 && zone.area_id == 75 && zone.matrix_id == 12);
    CHECK(memcmp(zone.raw_header, original, 56) == 0);
    data[24] = 99; CHECK(zone.raw_header[0] == 3); /* Owned header. */
    size = zone_fixture(data); put16(data + 24 + 4, UINT16_MAX);
    CHECK(px_field_zone_decode(data, size, &zone) == PX_FIELD_OK);
    CHECK(zone.matrix_id == UINT16_MAX); /* Original zone getter uses unsigned16. */
    CHECK(px_field_zone_decode(data, 24 + 55, &zone) != PX_FIELD_OK);
    CHECK(zone.matrix_id == 0 && zone.raw_header[0] == 0);
    for (int lod = 0; lod <= 1; ++lod) {
        struct px_field_matrix matrix = {0}; size = matrix_fixture(data, lod);
        CHECK(px_field_matrix_decode(data, size, &matrix) == PX_FIELD_OK);
        CHECK(matrix.width == 2 && matrix.height == 1 && matrix.has_lod == lod && matrix.unknown == 10);
        CHECK(matrix.field_ids[0] == 7 && matrix.field_ids[1] == -1);
        CHECK(matrix.camera_count == 1 && matrix.camera_bounds[0].north == -20);
        CHECK(matrix.camera_bounds[0].east == 1440 && matrix.camera_bounds[0].raw_flag == 1);
        if (lod) {
            CHECK(matrix.zone_ids[0] == 100 && matrix.zone_ids[8] == 108 && matrix.zone_ids[31] == 131);
            CHECK(matrix.lod_ids[0] == 9 && matrix.lod_ids[1] == -1);
        } else CHECK(matrix.zone_ids == NULL && matrix.lod_ids == NULL);
        data[24] = 99; CHECK(matrix.field_ids[0] == 7);
        px_field_matrix_free(&matrix);
        CHECK(matrix.field_ids == NULL && matrix.camera_bounds == NULL && matrix.width == 0);
        px_field_matrix_free(&matrix);
    }
    struct px_field_matrix matrix = {0};
    const unsigned offsets[] = {16, 20, 22, 24};
    const uint16_t values[] = {2, 0, 257, 0xfffe};
    for (unsigned i = 0; i < 4; ++i) {
        size = matrix_fixture(data, 0); put16(data + offsets[i], values[i]);
        CHECK(px_field_matrix_decode(data, size, &matrix) != PX_FIELD_OK);
        CHECK(matrix.field_ids == NULL && matrix.camera_bounds == NULL);
    }
    size = matrix_fixture(data, 0); put32(data + 28, PX_FIELD_CAMERA_LIMIT + 1);
    CHECK(px_field_matrix_decode(data, size, &matrix) != PX_FIELD_OK);
    size = matrix_fixture(data, 0); put_float(data + 32, NAN);
    CHECK(px_field_matrix_decode(data, size, &matrix) != PX_FIELD_OK);
    size = matrix_fixture(data, 0); data[size - 1] = 1;
    CHECK(px_field_matrix_decode(data, size, &matrix) != PX_FIELD_OK);
    px_field_matrix_free(NULL);
}

static void test_grid_and_collision(void)
{
    uint8_t data[CAPACITY]; size_t size = grid_fixture(data);
    struct px_field_grid grid = {0}; struct px_field_collision collision = {0};
    CHECK(px_field_grid_decode(data, size, &grid) == PX_FIELD_OK);
    CHECK(grid.width == 2 && grid.height == 3);
    CHECK(grid.tiles[0] == 0x20 && grid.tiles[1] == 0x01000021 && grid.tiles[2] == 0x20000000);
    CHECK(grid.tiles[3] == 0xef000021 && grid.tiles[4] == 0x28 && grid.tiles[5] == 0x20);
    CHECK(grid.sections[0].offset == 128 && grid.sections[0].size == 128);
    CHECK(memcmp(grid.sections[1].head, "BCH", 3) == 0 && memcmp(grid.sections[2].head, "coll", 4) == 0);
    put32(data + 132, 999); CHECK(grid.tiles[0] == 0x20);
    px_field_grid_free(&grid); CHECK(grid.tiles == NULL && grid.width == 0);
    size = grid_fixture(data);
    CHECK(px_field_collision_decode(data, size, &collision) == PX_FIELD_OK);
    CHECK(collision.vertex_count == 9 && collision.first_vertex[1] == 3 && collision.group_vertex_count[2] == 3);
    CHECK(collision.bounds_min[0] == 0 && collision.bounds_max[0] == 4 && collision.bounds_max[1] == 20);
    CHECK(collision.positions[4 * 3] == 4 && collision.positions[4 * 3 + 1] == 4);
    double height = -1;
    CHECK(px_field_collision_first_height(&collision, 1, 1, &height) == PX_FIELD_OK && height == 3);
    CHECK(px_field_collision_first_height(&collision, 0, 0, &height) == PX_FIELD_OK && height == 0);
    CHECK(px_field_collision_first_height(&collision, 2, 2, &height) == PX_FIELD_OK && height == 6);
    CHECK(px_field_collision_first_height(&collision, 4, 4, &height) == PX_FIELD_NOT_FOUND && height == 0);
    CHECK(px_field_collision_first_height(&collision, NAN, 1, &height) != PX_FIELD_OK && height == 0);
    CHECK(px_field_collision_first_height(&collision, 1e13, 1, &height) != PX_FIELD_OK && height == 0);
    px_field_collision_free(&collision); CHECK(collision.positions == NULL && collision.vertex_count == 0);
    px_field_collision_free(&collision); px_field_collision_free(NULL); px_field_grid_free(NULL);
    const unsigned mutations[] = {4, 8, 672, 676, 800, 812, 944, 952};
    const uint32_t values[] = {0, 0, 1, 2, 0x7f800000, 0x7fc00000, 0, 0};
    for (unsigned i = 0; i < sizeof mutations / sizeof mutations[0]; ++i) {
        size = grid_fixture(data); size_t offset = get32(data + 12);
        put32(data + offset + mutations[i], values[i]);
        CHECK(px_field_collision_decode(data, size, &collision) != PX_FIELD_OK);
        CHECK(collision.positions == NULL && collision.vertex_count == 0);
    }
    size = grid_fixture(data); put16(data + 128, 0);
    CHECK(px_field_grid_decode(data, size, &grid) != PX_FIELD_OK);
    size = grid_fixture(data); put16(data + 130, 257);
    CHECK(px_field_grid_decode(data, size, &grid) != PX_FIELD_OK);
    size = grid_fixture(data); data[128 + 28] = 1;
    CHECK(px_field_grid_decode(data, size, &grid) != PX_FIELD_OK && grid.tiles == NULL);
    size = grid_fixture(data); put16(data + 2, 5);
    CHECK(px_field_grid_decode(data, size, &grid) != PX_FIELD_OK);
}

int main(void)
{
    test_sections(); test_zones_and_matrices(); test_grid_and_collision();
    puts("field parser tests passed");
    return 0;
}
