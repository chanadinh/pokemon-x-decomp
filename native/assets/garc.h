#ifndef PX_NATIVE_GARC_H
#define PX_NATIVE_GARC_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

struct px_garc;
struct px_garc_member {
    uint32_t entry;
    uint32_t subentry;
    uint32_t offset; /* Absolute archive file offset. */
    uint32_t size;   /* Unpadded member size. */
};

/* Return 1 on success, 0 with a diagnostic on failure. Only little-endian
 * GARC v4 with packed allocation records is supported. The index is bounded
 * to 8 MiB / 262144 members; member payloads are not loaded.
 * open_file borrows an exclusively accessed seekable stream until close;
 * open owns its stream. Close never closes a borrowed stream. */
int px_garc_open(const char *path, struct px_garc **out,
                 char *error, size_t error_size);
int px_garc_open_file(FILE *file, struct px_garc **out,
                      char *error, size_t error_size);
void px_garc_close(struct px_garc *archive);
size_t px_garc_count(const struct px_garc *archive);
const struct px_garc_member *px_garc_member_at(const struct px_garc *archive,
                                             size_t index);
/* Reads one member by its original FATO entry and bitmap subentry numbers.
 * Caller frees *data. Failure leaves *data=NULL and *size=0. */
int px_garc_read(struct px_garc *archive, uint32_t entry, uint32_t subentry,
                 size_t max_bytes, uint8_t **data, size_t *size,
                 char *error, size_t error_size);

#endif
