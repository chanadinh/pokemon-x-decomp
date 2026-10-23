#include "garc.h"

#include <stdlib.h>
#include <string.h>

#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c); exit(1); } } while (0)

static void put32(uint8_t *p, uint32_t value)
{
    for (unsigned i = 0; i < 4; ++i) p[i] = (uint8_t)(value >> (8 * i));
}

/* Two groups, three members; the first group's bit 31 and the second
 * group's bit 2 exercise original subentry numbers rather than dense IDs. */
static void fixture(uint8_t bytes[132])
{
    memset(bytes, 0, 132);
    memcpy(bytes, "CRAG", 4); put32(bytes + 4, 28);
    bytes[8] = 0xff; bytes[9] = 0xfe; bytes[11] = 4;
    put32(bytes + 12, 4); put32(bytes + 16, 116);
    put32(bytes + 20, 132); put32(bytes + 24, 5);
    memcpy(bytes + 28, "OTAF", 4); put32(bytes + 32, 20);
    bytes[36] = 2; bytes[38] = bytes[39] = 0xff;
    put32(bytes + 40, 0); put32(bytes + 44, 28);
    memcpy(bytes + 48, "BTAF", 4); put32(bytes + 52, 56); put32(bytes + 56, 3);
    put32(bytes + 60, UINT32_C(0x80000001));
    put32(bytes + 64, 0); put32(bytes + 68, 4); put32(bytes + 72, 3);
    put32(bytes + 76, 4); put32(bytes + 80, 12); put32(bytes + 84, 5);
    put32(bytes + 88, 4);
    put32(bytes + 92, 12); put32(bytes + 96, 16); put32(bytes + 100, 1);
    memcpy(bytes + 104, "BMIF", 4); put32(bytes + 108, 12); put32(bytes + 112, 16);
    memcpy(bytes + 116, "abc_defgh___z___", 16);
}

static FILE *stream(const uint8_t *bytes, size_t size)
{
    FILE *file = tmpfile();
    CHECK(file != NULL);
    CHECK(fwrite(bytes, 1, size, file) == size);
    CHECK(fflush(file) == 0);
    return file;
}

static void rejects(const uint8_t *bytes, size_t size)
{
    FILE *file = stream(bytes, size);
    struct px_garc *archive = NULL;
    char error[160];
    CHECK(!px_garc_open_file(file, &archive, error, sizeof(error)));
    CHECK(archive == NULL && error[0] != '\0');
    CHECK(fclose(file) == 0);
}

int main(void)
{
    uint8_t bytes[132]; fixture(bytes);
    FILE *file = stream(bytes, sizeof(bytes));
    struct px_garc *archive = NULL; char error[160];
    CHECK(px_garc_open_file(file, &archive, error, sizeof(error)));
    CHECK(px_garc_count(archive) == 3);
    CHECK(px_garc_member_at(archive, 3) == NULL);
    const struct px_garc_member *member = px_garc_member_at(archive, 1);
    CHECK(member && member->entry == 0 && member->subentry == 31);
    CHECK(member->offset == 120 && member->size == 5);
    uint8_t *data = NULL; size_t size = 0;
    CHECK(px_garc_read(archive, 0, 31, 5, &data, &size, error, sizeof(error)));
    CHECK(size == 5 && memcmp(data, "defgh", 5) == 0); free(data);
    CHECK(px_garc_read(archive, 1, 2, 1, &data, &size, error, sizeof(error)));
    CHECK(size == 1 && data[0] == 'z'); free(data);
    CHECK(!px_garc_read(archive, 0, 31, 4, &data, &size, error, sizeof(error)));
    CHECK(data == NULL && size == 0);
    CHECK(!px_garc_read(archive, 0, 1, 20, &data, &size, error, sizeof(error)));
    CHECK(data == NULL && size == 0);
    px_garc_close(archive);
    CHECK(fseek(file, 0, SEEK_SET) == 0); /* borrowed stream survives close */
    CHECK(fclose(file) == 0);

    const struct { size_t offset; uint32_t value; } corruptions[] = {
        {0, 0}, {4, 36}, {8, UINT32_C(0x0600feff)}, {12, 3},
        {16, UINT32_MAX}, {20, 133}, {28, 0}, {32, 16},
        {40, 4}, {44, 0}, {48, 0}, {52, UINT32_MAX}, {56, 2}, {56, 4},
        {64, 5}, {68, 17}, {72, 5}, {88, 0}, {104, 0}, {108, 8}, {112, 17}
    };
    for (size_t i = 0; i < sizeof(corruptions) / sizeof(corruptions[0]); ++i) {
        fixture(bytes); put32(bytes + corruptions[i].offset, corruptions[i].value);
        rejects(bytes, sizeof(bytes));
    }
    fixture(bytes);
    for (size_t size_limit = 0; size_limit < sizeof(bytes); ++size_limit)
        rejects(bytes, size_limit);

    /* Empty v4 archive is structurally valid and allocates no member array. */
    uint8_t empty[64] = {0};
    memcpy(empty, "CRAG", 4); put32(empty + 4, 28);
    empty[8] = 0xff; empty[9] = 0xfe; empty[11] = 4;
    put32(empty + 12, 4); put32(empty + 16, 64); put32(empty + 20, 64);
    memcpy(empty + 28, "OTAF", 4); put32(empty + 32, 12);
    memcpy(empty + 40, "BTAF", 4); put32(empty + 44, 12);
    memcpy(empty + 52, "BMIF", 4); put32(empty + 56, 12);
    file = stream(empty, sizeof(empty));
    CHECK(px_garc_open_file(file, &archive, error, sizeof(error)));
    CHECK(px_garc_count(archive) == 0);
    CHECK(!px_garc_read(archive, 0, 0, 0, &data, &size, error, sizeof(error)));
    px_garc_close(archive); CHECK(fclose(file) == 0);
    CHECK(!px_garc_open_file(NULL, &archive, error, sizeof(error)));
    CHECK(!px_garc_open(NULL, &archive, error, sizeof(error)));
    puts("PASS: GARC v4 sparse subentries, bounded selected reads, ownership, malformed tables, all truncations");
    return 0;
}
