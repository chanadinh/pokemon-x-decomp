#include "bch_texture.h"

#include <stdlib.h>
#include <string.h>

/* Independently implemented from the checked-in BCH observations and the
 * pinned format references recorded in reproduction/bch-model. No input is
 * patched with absolute pointers; relocation provenance is checked on use. */
enum { MAX_BYTES = 64 * 1024 * 1024, MAX_ENTRIES = 65535, MAX_NAME = 1024,
       MAX_RELOCS = 262144, MAX_COMMAND_WORDS = 4096,
       MAX_TOTAL_WORDS = 4194304 };
/* Keep section IDs separate from the limits above. */
enum { SECTION_CONTENTS, SECTION_STRINGS, SECTION_COMMANDS, SECTION_RAW,
       SECTION_RELOCATION, SECTION_COUNT };
struct section { size_t start, end; };
struct relocation { uint32_t source, tag; };
struct px_bch {
    const uint8_t *data;
    struct section sections[SECTION_COUNT];
    size_t count, relocation_count;
    struct relocation *relocations;
    struct px_bch_texture *textures;
    int mixed; /* px_bch_open_mixed: other resources present and ignored */
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
static int span(size_t offset, size_t length, struct section section)
{
    return offset >= section.start && offset <= section.end &&
           length <= section.end - offset;
}
static int compare_relocations(const void *left, const void *right)
{
    const struct relocation *a = left, *b = right;
    return (a->source > b->source) - (a->source < b->source);
}
static const struct relocation *find_relocation(const struct px_bch *bch,
                                                size_t source)
{
    size_t first = 0, last = bch->relocation_count;
    while (first < last) {
        const size_t middle = first + (last - first) / 2;
        const struct relocation *relocation = bch->relocations + middle;
        if (relocation->source < source) first = middle + 1;
        else if (relocation->source > source) last = middle;
        else return relocation;
    }
    return NULL;
}
static int pointer(const struct px_bch *bch, size_t source, unsigned tag,
                   unsigned section, size_t length, size_t *target)
{
    const struct relocation *relocation = find_relocation(bch, source);
    if (relocation == NULL || relocation->tag != tag) return 0;
    const struct section bounds = bch->sections[section];
    const size_t relative = u32(bch->data + source);
    if (relative > bounds.end - bounds.start) return 0;
    *target = bounds.start + relative;
    return span(*target, length, bounds);
}
static int name(const struct px_bch *bch, size_t source,
                const char **text, size_t *length)
{
    size_t target;
    if (!pointer(bch, source, 1, SECTION_STRINGS, 1, &target)) return 0;
    size_t available = bch->sections[SECTION_STRINGS].end - target;
    if (available > MAX_NAME + 1) available = MAX_NAME + 1;
    const uint8_t *end = memchr(bch->data + target, 0, available);
    if (end == NULL) return 0;
    *text = (const char *)bch->data + target;
    *length = (size_t)(end - (bch->data + target));
    return 1;
}

static enum px_bch_result read_relocations(struct px_bch *bch)
{
    const struct section table = bch->sections[SECTION_RELOCATION];
    if ((table.end - table.start) % 4 != 0) return PX_BCH_INVALID_HEADER;
    bch->relocation_count = (table.end - table.start) / 4;
    if (bch->relocation_count > MAX_RELOCS) return PX_BCH_LIMIT_EXCEEDED;
    if (bch->relocation_count == 0) return PX_BCH_OK;
    bch->relocations = calloc(bch->relocation_count, sizeof *bch->relocations);
    if (bch->relocations == NULL) return PX_BCH_NO_MEMORY;
    for (size_t i = 0; i < bch->relocation_count; ++i) {
        const uint32_t value = u32(bch->data + table.start + 4 * i);
        const unsigned tag = value >> 25;
        unsigned source_section = SECTION_CONTENTS, target_section;
        size_t offset = value & UINT32_C(0x1ffffff);
        switch (tag) {
        case 0: target_section = SECTION_CONTENTS; offset *= 4; break;
        case 1: target_section = SECTION_STRINGS; break;
        case 2: target_section = SECTION_COMMANDS; offset *= 4; break;
        case 3:
            if (!bch->mixed) return PX_BCH_UNSUPPORTED;
            target_section = SECTION_COMMANDS; offset *= 4; break;
        case 0x24: case 0x25: case 0x26: case 0x27: case 0x28:
            if (tag != 0x24 && !bch->mixed) return PX_BCH_UNSUPPORTED;
            source_section = SECTION_COMMANDS;
            target_section = SECTION_RAW;
            offset *= 4;
            break;
        default: return PX_BCH_UNSUPPORTED;
        }
        const struct section source = bch->sections[source_section];
        const struct section target = bch->sections[target_section];
        if (offset > source.end - source.start ||
            !span(source.start + offset, 4, source)) return PX_BCH_INVALID_HEADER;
        const size_t relative = u32(bch->data + source.start + offset);
        if (relative >= target.end - target.start) return PX_BCH_INVALID_HEADER;
        bch->relocations[i].source = (uint32_t)(source.start + offset);
        bch->relocations[i].tag = tag;
    }
    qsort(bch->relocations, bch->relocation_count,
          sizeof *bch->relocations, compare_relocations);
    for (size_t i = 1; i < bch->relocation_count; ++i)
        if (bch->relocations[i].source - bch->relocations[i - 1].source < 4)
            return PX_BCH_INVALID_HEADER;
    return PX_BCH_OK;
}

struct command_texture {
    uint32_t dimensions, format, masks[2];
    size_t payload;
    int address_seen, end_seen, unsupported;
};
static enum px_bch_result read_commands(const struct px_bch *bch, size_t object,
    unsigned unit, size_t *total_words, struct command_texture *result)
{
    static const unsigned dimensions[3] = {0x82, 0x92, 0x9a};
    static const unsigned addresses[3] = {0x85, 0x95, 0x9d};
    static const unsigned formats[3] = {0x8e, 0x96, 0x9e};
    const size_t field = object + 8 * unit;
    const uint32_t words = u32(bch->data + field + 4);
    if (words > MAX_COMMAND_WORDS || words > MAX_TOTAL_WORDS - *total_words)
        return PX_BCH_LIMIT_EXCEEDED;
    *total_words += words;
    size_t start;
    if (words < 2 || (words & 1) != 0 ||
        !pointer(bch, field, 2, SECTION_COMMANDS, (size_t)words * 4, &start) ||
        start % 8 != 0) return PX_BCH_INVALID_HEADER;
    const size_t end = start + (size_t)words * 4;
    size_t cursor = start;
    while (cursor < end) {
        if (end - cursor < 8 || result->end_seen) return PX_BCH_INVALID_HEADER;
        const uint32_t header = u32(bch->data + cursor + 4);
        const unsigned reg = header & 0xffff;
        const unsigned extra = (header >> 20) & 0x7ff;
        const unsigned consecutive = header >> 31;
        const size_t packet_bytes = (size_t)(2 + extra) * 4;
        const size_t padded_bytes = (packet_bytes + 7) & ~(size_t)7;
        if (padded_bytes > end - cursor || reg > 0x3ff ||
            (consecutive && extra > 0x3ff - reg)) return PX_BCH_INVALID_HEADER;
        uint32_t mask = 0;
        for (unsigned byte = 0; byte < 4; ++byte)
            if ((header >> (16 + byte)) & 1) mask |= UINT32_C(255) << (8 * byte);
        for (unsigned parameter = 0; parameter <= extra; ++parameter) {
            const unsigned target = reg + (consecutive ? parameter : 0);
            const size_t source = cursor + (parameter == 0 ? 0 : 4 + 4 * parameter);
            const uint32_t value = u32(bch->data + source);
            if (target == dimensions[unit] || target == formats[unit]) {
                const unsigned slot = target == dimensions[unit] ? 0 : 1;
                uint32_t *destination = slot == 0 ? &result->dimensions : &result->format;
                *destination = (*destination & ~mask) | (value & mask);
                result->masks[slot] |= mask;
                if (find_relocation(bch, source) != NULL) return PX_BCH_INVALID_HEADER;
            } else if (target == addresses[unit]) {
                /* Partial address writes cannot identify a single relocated
                 * raw payload. Keep them explicitly unsupported. */
                if (mask != UINT32_MAX) result->unsupported = 1;
                if (!pointer(bch, source, 0x24, SECTION_RAW, 1, &result->payload))
                    return PX_BCH_INVALID_HEADER;
                result->address_seen = 1;
            } else if (target == 0x23d) {
                if (mask != UINT32_MAX || value != 1 || parameter != extra ||
                    cursor + padded_bytes != end ||
                    find_relocation(bch, source) != NULL) return PX_BCH_INVALID_HEADER;
                result->end_seen = 1;
            } else {
                result->unsupported = 1;
            }
        }
        cursor += padded_bytes;
    }
    if (!result->end_seen || !result->address_seen ||
        result->masks[0] != UINT32_MAX || result->masks[1] != UINT32_MAX)
        return PX_BCH_INVALID_HEADER;
    return PX_BCH_OK;
}

static int power_of_two(uint32_t value)
{
    return value >= 8 && (value & (value - 1)) == 0;
}
static enum px_bch_result read_texture(const struct px_bch *bch, size_t object,
    size_t *total_words, struct px_bch_texture *texture)
{
    if (!span(object, 32, bch->sections[SECTION_CONTENTS]) || object % 4 != 0)
        return PX_BCH_INVALID_HEADER;
    texture->object_offset = (uint32_t)object;
    texture->flags = u32(bch->data + object + 24);
    texture->mipmap_levels = (texture->flags >> 8) & 255;
    if (!name(bch, object + 28, &texture->name, &texture->name_size) ||
        texture->name_size != texture->dictionary_name_size ||
        memcmp(texture->name, texture->dictionary_name, texture->name_size) != 0)
        return PX_BCH_INVALID_HEADER;
    struct command_texture commands[3] = {{0}};
    for (unsigned unit = 0; unit < 3; ++unit) {
        const enum px_bch_result status = read_commands(bch, object, unit,
                                                       total_words, commands + unit);
        if (status != PX_BCH_OK) return status;
        if (unit != 0 && (commands[unit].dimensions != commands[0].dimensions ||
            commands[unit].format != commands[0].format ||
            commands[unit].payload != commands[0].payload)) return PX_BCH_INVALID_HEADER;
    }
    texture->width = commands[0].dimensions >> 16;
    texture->height = commands[0].dimensions & 0xffff;
    texture->format = commands[0].format;
    texture->data_offset = (uint32_t)commands[0].payload;
    texture->decode_status = PX_BCH_UNSUPPORTED;
    if (texture->width == 0 || texture->height == 0) {
        texture->decode_status = PX_BCH_INVALID_HEADER;
        return PX_BCH_OK;
    }
    if (texture->width > 4096 || texture->height > 4096) {
        texture->decode_status = PX_BCH_LIMIT_EXCEEDED;
        return PX_BCH_OK;
    }
    const unsigned bits = px_texture_bits_per_pixel(texture->format);
    if (bits != 0) {
        const size_t bytes = (size_t)texture->width * texture->height * bits / 8;
        if (bytes == 0 || !span(texture->data_offset, bytes, bch->sections[SECTION_RAW]))
            return PX_BCH_INVALID_HEADER;
        texture->data_size = (uint32_t)bytes;
    }
    /* Mixed packs also admit the raw formats, under the same observed
     * descriptor rule flags == 0x100 | format (single level). */
    const int raw_format = bch->mixed && bits != 0 && texture->format <= PX_TEXTURE_A4;
    if ((texture->format != PX_TEXTURE_ETC1 && texture->format != PX_TEXTURE_ETC1A4 && !raw_format) ||
        texture->flags != (UINT32_C(0x100) | texture->format) ||
        !power_of_two(texture->width) || !power_of_two(texture->height) ||
        commands[0].unsupported || commands[1].unsupported || commands[2].unsupported)
        return PX_BCH_OK;
    texture->decode_status = PX_BCH_OK;
    return PX_BCH_OK;
}

static enum px_bch_result read_index(struct px_bch *bch)
{
    const size_t base = bch->sections[SECTION_CONTENTS].start;
    if (!span(base, 180, bch->sections[SECTION_CONTENTS])) return PX_BCH_INVALID_HEADER;
    size_t total = 0, total_words = 0;
    for (unsigned category = 0; category < 15; ++category) {
        const size_t field = base + category * 12;
        const uint32_t count = u32(bch->data + field + 4);
        if (count > MAX_ENTRIES - total) return PX_BCH_LIMIT_EXCEEDED;
        total += count;
        if (category != 3 && count != 0 && !bch->mixed) return PX_BCH_UNSUPPORTED;
        if (category != 3 && bch->mixed) continue;
        if (category == 3) {
            bch->count = count;
            if (count != 0) {
                bch->textures = calloc(count, sizeof *bch->textures);
                if (bch->textures == NULL) return PX_BCH_NO_MEMORY;
            }
        }
        size_t table = 0, tree = 0;
        if (count != 0 || u32(bch->data + field) != 0 ||
            find_relocation(bch, field) != NULL) {
            if (!pointer(bch, field, 0, SECTION_CONTENTS, (size_t)count * 4, &table) ||
                table % 4 != 0) return PX_BCH_INVALID_HEADER;
        }
        if (count == 0 && u32(bch->data + field + 8) == 0 &&
            find_relocation(bch, field + 8) == NULL) continue;
        if (!pointer(bch, field + 8, 0, SECTION_CONTENTS,
                     ((size_t)count + 1) * 12, &tree) || tree % 4 != 0)
            return PX_BCH_INVALID_HEADER;
        for (size_t node = 0; node <= count; ++node) {
            const size_t at = tree + node * 12;
            if (u16(bch->data + at + 4) > count || u16(bch->data + at + 6) > count)
                return PX_BCH_INVALID_HEADER;
            if (node == 0) continue;
            struct px_bch_texture *texture = bch->textures + node - 1;
            size_t object;
            if (!name(bch, at + 8, &texture->dictionary_name,
                      &texture->dictionary_name_size) ||
                !pointer(bch, table + (node - 1) * 4, 0, SECTION_CONTENTS, 32, &object))
                return PX_BCH_INVALID_HEADER;
            const enum px_bch_result status = read_texture(bch, object, &total_words, texture);
            if (status != PX_BCH_OK) return status;
        }
    }
    return PX_BCH_OK;
}

static enum px_bch_result open_pack(const uint8_t *data, size_t size, struct px_bch **out,
                                    int mixed)
{
    if (out == NULL) return PX_BCH_INVALID_ARGUMENT;
    *out = NULL;
    if (data == NULL) return PX_BCH_INVALID_ARGUMENT;
    if (size > MAX_BYTES) return PX_BCH_LIMIT_EXCEEDED;
    if (size < 56) return PX_BCH_TRUNCATED;
    if (memcmp(data, "BCH\0", 4) != 0) return PX_BCH_INVALID_HEADER;
    if (data[4] != 7 || data[5] != 7) return PX_BCH_UNSUPPORTED;
    struct px_bch parsed = {0};
    parsed.data = data;
    parsed.mixed = mixed;
    for (unsigned i = 0; i < SECTION_COUNT; ++i) {
        const size_t offset = u32(data + 8 + 4 * i), length = u32(data + 28 + 4 * i);
        if (offset > size || length > size - offset) return PX_BCH_TRUNCATED;
        if (length != 0 && offset < 56) return PX_BCH_INVALID_HEADER;
        if (length != 0 && i != SECTION_STRINGS && offset % 4 != 0)
            return PX_BCH_INVALID_HEADER;
        parsed.sections[i].start = offset;
        parsed.sections[i].end = offset + length;
        if (length == 0) continue;
        for (unsigned j = 0; j < i; ++j) {
            const struct section previous = parsed.sections[j];
            if (previous.start != previous.end && offset < previous.end &&
                previous.start < offset + length) return PX_BCH_INVALID_HEADER;
        }
    }
    if (u32(data + 48) != 0 || u32(data + 52) != 0) return PX_BCH_UNSUPPORTED;
    enum px_bch_result status = read_relocations(&parsed);
    if (status == PX_BCH_OK) status = read_index(&parsed);
    if (status != PX_BCH_OK) {
        free(parsed.relocations);
        free(parsed.textures);
        return status;
    }
    struct px_bch *bch = malloc(sizeof *bch);
    if (bch == NULL) {
        free(parsed.relocations);
        free(parsed.textures);
        return PX_BCH_NO_MEMORY;
    }
    *bch = parsed;
    *out = bch;
    return PX_BCH_OK;
}
enum px_bch_result px_bch_open(const uint8_t *data, size_t size, struct px_bch **out)
{
    return open_pack(data, size, out, 0);
}
enum px_bch_result px_bch_open_mixed(const uint8_t *data, size_t size, struct px_bch **out)
{
    return open_pack(data, size, out, 1);
}
size_t px_bch_texture_count(const struct px_bch *bch)
{
    return bch == NULL ? 0 : bch->count;
}
const struct px_bch_texture *px_bch_texture(const struct px_bch *bch, size_t index)
{
    return bch == NULL || index >= bch->count ? NULL : bch->textures + index;
}
enum px_bch_result px_bch_decode_texture(const struct px_bch *bch,
    size_t index, struct px_texture_image *out)
{
    if (out == NULL) return PX_BCH_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    if (bch == NULL) return PX_BCH_INVALID_ARGUMENT;
    const struct px_bch_texture *texture = px_bch_texture(bch, index);
    if (texture == NULL) return PX_BCH_NOT_FOUND;
    if (texture->decode_status != PX_BCH_OK) return texture->decode_status;
    const enum px_texture_result result = px_texture_decode_payload(
        bch->data + texture->data_offset, texture->data_size,
        texture->width, texture->height, texture->width, texture->height,
        texture->format, out);
    switch (result) {
    case PX_TEXTURE_OK: return PX_BCH_OK;
    case PX_TEXTURE_INVALID_ARGUMENT: return PX_BCH_INVALID_ARGUMENT;
    case PX_TEXTURE_TRUNCATED: return PX_BCH_TRUNCATED;
    case PX_TEXTURE_UNSUPPORTED: return PX_BCH_UNSUPPORTED;
    case PX_TEXTURE_LIMIT_EXCEEDED: return PX_BCH_LIMIT_EXCEEDED;
    case PX_TEXTURE_NO_MEMORY: return PX_BCH_NO_MEMORY;
    }
    return PX_BCH_UNSUPPORTED;
}
void px_bch_close(struct px_bch *bch)
{
    if (bch == NULL) return;
    free(bch->relocations);
    free(bch->textures);
    free(bch);
}
const char *px_bch_result_string(enum px_bch_result result)
{
    switch (result) {
    case PX_BCH_OK: return "success";
    case PX_BCH_INVALID_ARGUMENT: return "invalid BCH argument";
    case PX_BCH_TRUNCATED: return "truncated BCH data";
    case PX_BCH_INVALID_HEADER: return "invalid BCH index, relocation, command, or texture extent";
    case PX_BCH_UNSUPPORTED: return "unsupported BCH version, resource, relocation, or texture layout";
    case PX_BCH_LIMIT_EXCEEDED: return "BCH file, resource, command, or texture dimension limit exceeded";
    case PX_BCH_NO_MEMORY: return "cannot allocate BCH index or texture";
    case PX_BCH_NOT_FOUND: return "BCH texture index does not exist";
    }
    return "unknown BCH result";
}
