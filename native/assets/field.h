#ifndef PX_FIELD_H
#define PX_FIELD_H

#include <stddef.h>
#include <stdint.h>

#define PX_FIELD_INPUT_LIMIT (64u * 1024u * 1024u)
#define PX_FIELD_SECTION_LIMIT 256u
#define PX_FIELD_GRID_SIDE_LIMIT 256u
#define PX_FIELD_GRID_CELL_LIMIT 65536u
#define PX_FIELD_CAMERA_LIMIT 4096u
#define PX_FIELD_COLLISION_VERTEX_LIMIT 1048576u

enum px_field_result {
    PX_FIELD_OK = 0, PX_FIELD_INVALID_ARGUMENT, PX_FIELD_TRUNCATED,
    PX_FIELD_INVALID_HEADER, PX_FIELD_UNSUPPORTED, PX_FIELD_LIMIT_EXCEEDED,
    PX_FIELD_NO_MEMORY, PX_FIELD_NOT_FOUND
};

/* Borrowed slice; all minipack offsets are absolute from the container start.
 * The complete offset table is validated, including unselected sections.
 * Zero-length sections are allowed. At most127 zero bytes may trail the final
 * declared offset. No decompression occurs in this module. */
struct px_field_span {
    const uint8_t *data;
    size_t size, offset;
    uint16_t section_count;
};
enum px_field_result px_field_section(const uint8_t *data, size_t size,
    const char magic[2], uint16_t selected, struct px_field_span *out);

/* XY ZO header subset. Original56 bytes are retained; coordinates/camera
 * metadata are not interpreted as a spawn or reconstructed game camera. */
struct px_field_zone {
    uint8_t map_type, map_move;
    uint16_t area_id, matrix_id;
    uint8_t raw_header[56];
};
enum px_field_result px_field_zone_decode(const uint8_t *data, size_t size,
    struct px_field_zone *out);

struct px_field_camera_bound {
    float north, south, west, east;
    int32_t raw_flag;
};
struct px_field_matrix {
    uint16_t width, height, has_lod, unknown;
    int16_t *field_ids; /* Owned row-major width*height; -1 means no region. */
    int16_t *zone_ids;  /* Owned (width*4)*(height*4), or NULL when has_lod=0. */
    int16_t *lod_ids;   /* Owned width*height, or NULL when has_lod=0. */
    uint32_t camera_count;
    struct px_field_camera_bound *camera_bounds;
};
enum px_field_result px_field_matrix_decode(const uint8_t *data, size_t size,
    struct px_field_matrix *out);
void px_field_matrix_free(struct px_field_matrix *matrix);

struct px_field_section_info {
    size_t offset, size;
    uint8_t head[4]; /* Zero-filled when fewer than4 bytes are present. */
};
struct px_field_grid {
    uint16_t width, height;
    uint32_t *tiles; /* Owned raw little-endian four-byte words, row-major. */
    struct px_field_section_info sections[6];
};
/* Restricted to six-section GR. Base tile data is returned without inferring
 * passability from partial flags. Models/props/extended layers are not decoded. */
enum px_field_result px_field_grid_decode(const uint8_t *data, size_t size,
    struct px_field_grid *out);
void px_field_grid_free(struct px_field_grid *grid);

struct px_field_collision {
    uint32_t vertex_count;
    uint32_t first_vertex[16], group_vertex_count[16];
    float *positions; /* Owned XYZ triples; stored triangle order/duplicates. */
    float bounds_min[3], bounds_max[3];
};
/* Decode the original local-coordinate GR section2 coll triangle lists.
 * Region placement belongs to the caller; this is not game collision logic. */
enum px_field_result px_field_collision_decode(const uint8_t *gr, size_t size,
    struct px_field_collision *out);
void px_field_collision_free(struct px_field_collision *collision);
/* Geometric vertical probe at finite local X/Z. Returns the first stored
 * nonvertical triangle hit, including its boundary, or NOT_FOUND. It does not
 * choose a walkable floor, infer walls, or reconstruct movement/collision rules.
 * Failure/no hit zeros *height. Absolute query coordinates must be <=1e12. */
enum px_field_result px_field_collision_first_height(
    const struct px_field_collision *collision, double x, double z, double *height);

/* Decode outputs own allocations and borrow no input. Pass fresh/freed output;
 * every failure clears it. Free accepts NULL/zero output. Inputs stay immutable.
 * Names, labels, tile interpretation, placement and movement policy are external. */
const char *px_field_result_string(enum px_field_result result);

#endif
