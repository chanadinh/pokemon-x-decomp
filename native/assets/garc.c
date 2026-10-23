#include "garc.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

/* Field layout checked against local v4 archives and the original pk3DS
 * implementation: https://github.com/kwsch/pk3DS/blob/6daaca934ca2284a73ab743bf89c848c57cd9de1/pk3DS.Core/CTR/GARC.cs
 * No original game resources are embedded here. */
#define PX_GARC_MAX_INDEX (8u * 1024u * 1024u)
#define PX_GARC_MAX_MEMBERS 262144u

struct px_garc {
    FILE *file;
    int owns_file;
    size_t count;
    struct px_garc_member *members;
};

static int fail(char *error, size_t size, const char *message)
{
    if (error != NULL && size != 0) (void)snprintf(error, size, "%s", message);
    return 0;
}

static uint32_t get32(const uint8_t *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 |
           (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

static uint16_t get16(const uint8_t *p)
{
    return (uint16_t)((uint16_t)p[0] | (uint16_t)p[1] << 8);
}

static int span(size_t offset, size_t size, size_t limit)
{
    return offset <= limit && size <= limit - offset;
}

static int parse_index(FILE *file, const uint8_t *index, size_t length,
                       uint32_t file_size, struct px_garc **out,
                       char *error, size_t error_size)
{
    size_t fato = 28;
    if (!span(fato, 12, length) || memcmp(index + fato, "OTAF", 4) != 0)
        return fail(error, error_size, "GARC: missing FATO section");
    uint32_t fato_size = get32(index + fato + 4);
    uint16_t entries = get16(index + fato + 8);
    if (fato_size != 12u + (uint32_t)entries * 4u ||
        !span(fato, fato_size, length))
        return fail(error, error_size, "GARC: invalid FATO size");
    size_t fatb = fato + fato_size;
    if (!span(fatb, 12, length) || memcmp(index + fatb, "BTAF", 4) != 0)
        return fail(error, error_size, "GARC: missing FATB section");
    uint32_t fatb_size = get32(index + fatb + 4);
    uint32_t count = get32(index + fatb + 8);
    if (fatb_size < 12 || !span(fatb, fatb_size, length))
        return fail(error, error_size, "GARC: invalid FATB size");
    if (count > PX_GARC_MAX_MEMBERS || count > (fatb_size - 12) / 12)
        return fail(error, error_size, "GARC: member count exceeds bounded index");
    size_t fimb = fatb + fatb_size;
    if (!span(fimb, 12, length) || fimb + 12 != length ||
        memcmp(index + fimb, "BMIF", 4) != 0 || get32(index + fimb + 4) != 12)
        return fail(error, error_size, "GARC: invalid FIMB section/data offset");
    uint32_t data_size = get32(index + fimb + 8);
    if (data_size != file_size - length)
        return fail(error, error_size, "GARC: data size disagrees with file size");

    struct px_garc *archive = calloc(1, sizeof(*archive));
    if (archive == NULL) return fail(error, error_size, "GARC: allocation failed");
    archive->file = file;
    if (count != 0) {
        archive->members = calloc(count, sizeof(*archive->members));
        if (archive->members == NULL) {
            free(archive);
            return fail(error, error_size, "GARC: index allocation failed");
        }
    }
    size_t cursor = 0;
    const size_t records_size = fatb_size - 12;
    const uint8_t *records = index + fatb + 12;
    for (uint32_t entry = 0; entry < entries; ++entry) {
        uint32_t offset = get32(index + fato + 12 + entry * 4);
        /* The supported v4 layout has packed records in FATO order. This
         * rejects aliases, overlaps and unknown record gaps explicitly. */
        if (offset != cursor || !span(cursor, 4, records_size)) {
            px_garc_close(archive);
            return fail(error, error_size, "GARC: unsupported/nonpacked FATO record offset");
        }
        uint32_t mask = get32(records + cursor);
        cursor += 4;
        for (uint32_t sub = 0; sub < 32; ++sub) {
            if ((mask & (UINT32_C(1) << sub)) == 0) continue;
            if (!span(cursor, 12, records_size) || archive->count == count) {
                px_garc_close(archive);
                return fail(error, error_size, "GARC: truncated/excess FATB member records");
            }
            uint32_t start = get32(records + cursor);
            uint32_t end = get32(records + cursor + 4);
            uint32_t size = get32(records + cursor + 8);
            cursor += 12;
            if (start > end || end > data_size || size > end - start) {
                px_garc_close(archive);
                return fail(error, error_size, "GARC: member outside data section");
            }
            archive->members[archive->count++] = (struct px_garc_member) {
                entry, sub, (uint32_t)length + start, size
            };
        }
    }
    if (archive->count != count || cursor != records_size) {
        px_garc_close(archive);
        return fail(error, error_size, "GARC: allocation table count/size mismatch");
    }
    *out = archive;
    return 1;
}

int px_garc_open_file(FILE *file, struct px_garc **out,
                      char *error, size_t error_size)
{
    if (out != NULL) *out = NULL;
    if (error != NULL && error_size != 0) error[0] = '\0';
    if (file == NULL || out == NULL)
        return fail(error, error_size, "GARC: invalid argument");
    if (fseek(file, 0, SEEK_END) != 0)
        return fail(error, error_size, "GARC: stream is not seekable");
    long end = ftell(file);
    if (end < 0 || (uintmax_t)end > UINT32_MAX)
        return fail(error, error_size, "GARC: unsupported file size");
    if (fseek(file, 0, SEEK_SET) != 0)
        return fail(error, error_size, "GARC: seek failed");
    uint8_t header[28];
    if (fread(header, 1, sizeof(header), file) != sizeof(header))
        return fail(error, error_size, "GARC: truncated header");
    if (memcmp(header, "CRAG", 4) != 0)
        return fail(error, error_size, "GARC: unrecognized magic");
    if (get16(header + 8) != 0xfeff || get16(header + 10) != 0x0400 ||
        get32(header + 4) != 28 || get32(header + 12) != 4)
        return fail(error, error_size, "GARC: only little-endian v4 with four sections is supported");
    uint32_t length = get32(header + 16);
    if (get32(header + 20) != (uint32_t)end || length > (uint32_t)end || length < 64)
        return fail(error, error_size, "GARC: inconsistent header sizes");
    if (length > PX_GARC_MAX_INDEX)
        return fail(error, error_size, "GARC: index exceeds 8 MiB limit");
    uint8_t *index = malloc(length);
    if (index == NULL) return fail(error, error_size, "GARC: index allocation failed");
    memcpy(index, header, sizeof(header));
    if (fread(index + sizeof(header), 1, length - sizeof(header), file) != length - sizeof(header)) {
        free(index);
        return fail(error, error_size, "GARC: truncated index");
    }
    int ok = parse_index(file, index, length, (uint32_t)end, out, error, error_size);
    free(index);
    return ok;
}

int px_garc_open(const char *path, struct px_garc **out,
                 char *error, size_t error_size)
{
    if (out != NULL) *out = NULL;
    if (path == NULL || out == NULL) return fail(error, error_size, "GARC: invalid argument");
    FILE *file = fopen(path, "rb");
    if (file == NULL) return fail(error, error_size, "GARC: cannot open archive");
    if (!px_garc_open_file(file, out, error, error_size)) {
        fclose(file);
        return 0;
    }
    (*out)->owns_file = 1;
    return 1;
}

void px_garc_close(struct px_garc *archive)
{
    if (archive == NULL) return;
    if (archive->owns_file) fclose(archive->file);
    free(archive->members);
    free(archive);
}

size_t px_garc_count(const struct px_garc *archive)
{
    return archive == NULL ? 0 : archive->count;
}

const struct px_garc_member *px_garc_member_at(const struct px_garc *archive, size_t index)
{
    return archive != NULL && index < archive->count ? archive->members + index : NULL;
}

int px_garc_read(struct px_garc *archive, uint32_t entry, uint32_t subentry,
                 size_t max_bytes, uint8_t **data, size_t *size,
                 char *error, size_t error_size)
{
    if (data != NULL) *data = NULL;
    if (size != NULL) *size = 0;
    if (error != NULL && error_size != 0) error[0] = '\0';
    if (archive == NULL || data == NULL || size == NULL)
        return fail(error, error_size, "GARC: invalid argument");
    const struct px_garc_member *member = NULL;
    for (size_t i = 0; i < archive->count; ++i)
        if (archive->members[i].entry == entry && archive->members[i].subentry == subentry) {
            member = archive->members + i;
            break;
        }
    if (member == NULL) return fail(error, error_size, "GARC: member does not exist");
    if (member->size > max_bytes) return fail(error, error_size, "GARC: member exceeds byte budget");
#if LONG_MAX < UINT32_MAX
    if (member->offset > (uint32_t)LONG_MAX)
        return fail(error, error_size, "GARC: offset exceeds platform seek range");
#endif
    uint8_t *buffer = malloc(member->size == 0 ? 1 : member->size);
    if (buffer == NULL) return fail(error, error_size, "GARC: member allocation failed");
    if (fseek(archive->file, (long)member->offset, SEEK_SET) != 0 ||
        fread(buffer, 1, member->size, archive->file) != member->size) {
        free(buffer);
        return fail(error, error_size, "GARC: member read failed/truncated");
    }
    *data = buffer;
    *size = member->size;
    return 1;
}
