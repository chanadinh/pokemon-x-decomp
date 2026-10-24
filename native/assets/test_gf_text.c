#include "gf_text.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void put16(uint8_t *p, unsigned v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); }
static void put32(uint8_t *p, uint32_t v) { put16(p, v & 0xffff); put16(p + 2, v >> 16); }

/* Three lines: plain text with a page break and an accent; a TRNAME
 * variable and an unknown one; the empty-line marker. Encrypted as the game
 * stores them. */
static size_t build(uint8_t *d)
{
    static const uint16_t l0[] = {'H', 'i', 0x10, 1, 0xBE01, '\n', 'P', 'o', 'k', 0xE9, 'm', 'o', 'n', 0};
    static const uint16_t l1[] = {'I', ' ', 'a', 'm', ' ', 0x10, 1, 0x0100, '.', 0x10, 2, 0x4242, 7, '!', 0};
    static const uint16_t l2[] = {0x10, 2, 0xBDFF, 0, 0};
    const uint16_t *lines[3] = {l0, l1, l2};
    const unsigned lengths[3] = {14, 15, 5};
    memset(d, 0, 512);
    const uint32_t section = 0x10;
    uint32_t at = 4 + 3 * 8;
    put16(d, 1); put16(d + 2, 3); put32(d + 8, 0); put32(d + 12, section);
    uint16_t key = 0x7C89;
    for (unsigned i = 0; i < 3; ++i, key = (uint16_t)(key + 0x2983)) {
        put32(d + section + 4 + 8 * i, at); put16(d + section + 8 + 8 * i, lengths[i]);
        uint16_t k = key;
        for (unsigned j = 0; j < lengths[i]; ++j) {
            put16(d + section + at + 2 * j, lines[i][j] ^ k);
            k = (uint16_t)(k << 3 | k >> 13);
        }
        at += 2 * lengths[i];
        if (at % 4) at += 2;
    }
    put32(d + 4, at); put32(d + section, at);
    return section + at;
}

int main(void)
{
    uint8_t d[512];
    const size_t n = build(d);
    struct px_gf_text t;
    assert(px_gf_text_decode(d, n, &t) == PX_GF_TEXT_OK && t.line_count == 3);
    assert(t.length[0] == 14 && t.units[t.first[0]] == 'H' && t.units[t.first[0] + 9] == 0xE9);
    char out[128];
    assert(px_gf_text_render(&t, 0, NULL, out, sizeof out) == (int)strlen("Hi\fPok\xc3\xa9mon"));
    assert(!strcmp(out, "Hi\fPok\xc3\xa9mon")); /* the break after a page code belongs to the page */
    assert(px_gf_text_render(&t, 1, "Ana", out, sizeof out) > 0 && !strcmp(out, "I am Ana.[4242]!"));
    assert(px_gf_text_render(&t, 1, NULL, out, sizeof out) > 0 && !strcmp(out, "I am [TRNAME].[4242]!"));
    assert(px_gf_text_render(&t, 2, NULL, out, sizeof out) == 0 && out[0] == 0);
    assert(px_gf_text_render(&t, 3, NULL, out, sizeof out) == -1);
    assert(px_gf_text_render(&t, 0, NULL, out, 5) == -1); /* does not fit */
    px_gf_text_free(&t);
    assert(t.units == NULL);
    uint8_t c[512];
    assert(px_gf_text_decode(NULL, n, &t) == PX_GF_TEXT_INVALID_ARGUMENT);
    assert(px_gf_text_decode(d, 0x13, &t) == PX_GF_TEXT_TRUNCATED);
    assert(px_gf_text_decode(d, n - 2, &t) == PX_GF_TEXT_INVALID);    /* size disagrees with the header */
    memcpy(c, d, n); put16(c, 2);
    assert(px_gf_text_decode(c, n, &t) == PX_GF_TEXT_INVALID);        /* two sections */
    memcpy(c, d, n); put32(c + 8, 1);
    assert(px_gf_text_decode(c, n, &t) == PX_GF_TEXT_INVALID);        /* initial key */
    memcpy(c, d, n); put32(c + 0x10, 4);
    assert(px_gf_text_decode(c, n, &t) == PX_GF_TEXT_INVALID);        /* section length */
    memcpy(c, d, n); put16(c + 0x10 + 8, 500);
    assert(px_gf_text_decode(c, n, &t) == PX_GF_TEXT_TRUNCATED);      /* a line past the section */
    /* A variable whose count runs past the line is rejected when rendered. */
    memcpy(c, d, n);
    assert(px_gf_text_decode(c, n, &t) == PX_GF_TEXT_OK);
    t.units[t.first[1] + 6] = 40;
    assert(px_gf_text_render(&t, 1, NULL, out, sizeof out) == -1);
    px_gf_text_free(&t);
    assert(!strcmp(px_gf_text_result_string(PX_GF_TEXT_TRUNCATED), "truncated text file"));
    puts("GF text tests passed");
    return 0;
}
