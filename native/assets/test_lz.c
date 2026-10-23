#include "lz.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c); exit(1); } } while (0)

static void expect(const uint8_t *stream, size_t length, const uint8_t *expected, size_t wanted)
{
    uint8_t *output = NULL; size_t size = 0; char error[160];
    CHECK(px_lz_decode(stream, length, wanted, &output, &size, error, sizeof(error)));
    CHECK(size == wanted && memcmp(output, expected, size) == 0);
    free(output);
    CHECK(!px_lz_decode(stream, length, wanted - 1, &output, &size, error, sizeof(error)));
    CHECK(output == NULL && size == 0 && error[0]);
}

static void rejects(const uint8_t *stream, size_t length)
{
    uint8_t *output = NULL; size_t size = 0; char error[160];
    CHECK(!px_lz_decode(stream, length, 70000, &output, &size, error, sizeof(error)));
    CHECK(output == NULL && size == 0 && error[0]);
}

int main(void)
{
    /* Fixed byte streams and expected strings; no matching encoder. */
    const uint8_t ten[] = {0x10,9,0,0, 0x10,'a','b','c',0x30,0x02};
    const uint8_t eleven[] = {0x11,9,0,0, 0x10,'a','b','c',0x50,0x02};
    const uint8_t overlap[] = {0x11,7,0,0, 0x40,'Q',0x50,0x00};
    expect(ten,sizeof(ten),(const uint8_t *)"abcabcabc",9);
    expect(eleven,sizeof(eleven),(const uint8_t *)"abcabcabc",9);
    expect(overlap,sizeof(overlap),(const uint8_t *)"QQQQQQQ",7);
    uint8_t repeated[65809]; memset(repeated, 'X', sizeof(repeated));
    const uint8_t medium[] = {0x11,18,0,0,0x40,'X',0x00,0x00,0x00};
    const uint8_t medium_max[] = {0x11,0x11,0x01,0,0x40,'X',0x0f,0xf0,0x00};
    const uint8_t long_min[] = {0x11,0x12,0x01,0,0x40,'X',0x10,0x00,0x00,0x00};
    const uint8_t long_max[] = {0x11,0x11,0x01,0x01,0x40,'X',0x1f,0xff,0xf0,0x00};
    expect(medium,sizeof(medium),repeated,18);
    expect(medium_max,sizeof(medium_max),repeated,273);
    expect(long_min,sizeof(long_min),repeated,274);
    expect(long_max,sizeof(long_max),repeated,65809);
    const uint8_t extended[] = {0x11,0,0,0,3,0,0,0,0,'a','b','c'};
    expect(extended,sizeof(extended),(const uint8_t *)"abc",3);
    const uint8_t padded[] = {0x11,1,0,0,0,'Z',0,0};
    expect(padded,sizeof(padded),(const uint8_t *)"Z",1);
    const uint8_t bad_magic[] = {0x12,1,0,0,0,'Z'};
    const uint8_t bad_backref[] = {0x11,3,0,0,0x80,0x20,0};
    const uint8_t overrun[] = {0x11,3,0,0,0x40,'X',0x20,0};
    const uint8_t nonzero_tail[] = {0x11,1,0,0,0,'Z',1};
    const uint8_t long_tail[] = {0x11,1,0,0,0,'Z',0,0,0,0};
    const uint8_t zero_size[] = {0x11,0,0,0,0,0,0,0};
    rejects(bad_magic,sizeof(bad_magic)); rejects(bad_backref,sizeof(bad_backref));
    rejects(overrun,sizeof(overrun)); rejects(nonzero_tail,sizeof(nonzero_tail));
    rejects(long_tail,sizeof(long_tail)); rejects(zero_size,sizeof(zero_size));
    for (size_t i = 0; i < sizeof(ten); ++i) rejects(ten,i);
    for (size_t i = 0; i < sizeof(eleven); ++i) rejects(eleven,i);
    for (size_t i = 0; i < sizeof(medium); ++i) rejects(medium,i);
    for (size_t i = 0; i < sizeof(long_min); ++i) rejects(long_min,i);
    for (size_t i = 0; i < sizeof(extended); ++i) rejects(extended,i);
    puts("PASS: LZ10/LZ11 fixed vectors, overlap, all token lengths, extended size, budgets, malformed input");
    return 0;
}
