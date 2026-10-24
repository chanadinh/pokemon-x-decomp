#include "gf_text.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { MAX_LINES = 65535, MAX_BYTES = 16u << 20 };

static uint16_t u16(const uint8_t *p) { return (uint16_t)(p[0] | p[1] << 8); }
static uint32_t u32(const uint8_t *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

enum px_gf_text_result px_gf_text_decode(const uint8_t *data, size_t size, struct px_gf_text *out)
{
    if (!out) return PX_GF_TEXT_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    if (!data) return PX_GF_TEXT_INVALID_ARGUMENT;
    if (size > MAX_BYTES) return PX_GF_TEXT_INVALID;
    if (size < 0x14) return PX_GF_TEXT_TRUNCATED;
    const uint32_t sections = u16(data), lines = u16(data + 2), total = u32(data + 4), key = u32(data + 8),
                   section = u32(data + 12);
    if (sections != 1 || key != 0 || section < 0x10) return PX_GF_TEXT_INVALID;
    if ((uint64_t)section + total != size || total < 4 || u32(data + section) != total) return PX_GF_TEXT_INVALID;
    if (lines > MAX_LINES || (uint64_t)lines * 8 + 4 > total) return PX_GF_TEXT_INVALID;
    size_t units = 0;
    for (uint32_t i = 0; i < lines; ++i) {
        const uint32_t offset = u32(data + section + 4 + 8 * (size_t)i);
        const uint32_t length = u16(data + section + 8 + 8 * (size_t)i);
        if ((uint64_t)offset + 2 * (uint64_t)length > total) return PX_GF_TEXT_TRUNCATED;
        units += length;
    }
    out->first = malloc((lines ? lines : 1) * sizeof *out->first);
    out->length = malloc((lines ? lines : 1) * sizeof *out->length);
    out->units = malloc((units ? units : 1) * sizeof *out->units);
    if (!out->first || !out->length || !out->units) { px_gf_text_free(out); return PX_GF_TEXT_NO_MEMORY; }
    out->line_count = lines;
    size_t at = 0;
    uint16_t line_key = 0x7C89;
    for (uint32_t i = 0; i < lines; ++i, line_key = (uint16_t)(line_key + 0x2983)) {
        const uint8_t *p = data + section + u32(data + section + 4 + 8 * (size_t)i);
        const uint32_t length = u16(data + section + 8 + 8 * (size_t)i);
        out->first[i] = (uint32_t)at; out->length[i] = length;
        uint16_t k = line_key;
        for (uint32_t j = 0; j < length; ++j) {
            out->units[at++] = (uint16_t)(u16(p + 2 * (size_t)j) ^ k);
            k = (uint16_t)(k << 3 | k >> 13);
        }
    }
    return PX_GF_TEXT_OK;
}

void px_gf_text_free(struct px_gf_text *t)
{
    if (!t) return;
    free(t->first); free(t->length); free(t->units);
    memset(t, 0, sizeof *t);
}

static const char *variable_name(uint16_t code)
{
    static const struct { uint16_t code; const char *name; } names[] = {
        {0xFF00, "COLOR"}, {0x0100, "TRNAME"}, {0x0101, "PKNAME"}, {0x0102, "PKNICK"}, {0x0103, "TYPE"},
        {0x0105, "LOCATION"}, {0x0106, "ABILITY"}, {0x0107, "MOVE"}, {0x0108, "ITEM1"}, {0x0109, "ITEM2"},
        {0x010A, "sTRBAG"}, {0x010B, "BOX"}, {0x010D, "EVSTAT"}, {0x0110, "OPOWER"}, {0x0127, "RIBBON"},
        {0x0134, "MIINAME"}, {0x013E, "WEATHER"}, {0x0189, "TRNICK"}, {0x018A, "1stchrTR"},
        {0x018B, "SHOUTOUT"}, {0x018E, "BERRY"}, {0x018F, "REMFEEL"}, {0x0190, "REMQUAL"},
        {0x0191, "WEBSITE"}, {0x019C, "CHOICECOS"}, {0x01A1, "GSYNCID"}, {0x0192, "PRVIDSAY"},
        {0x0193, "BTLTEST"}, {0x0195, "GENLOC"}, {0x0199, "CHOICEFOOD"}, {0x019A, "HOTELITEM"},
        {0x019B, "TAXISTOP"}, {0x019F, "MAISTITLE"}, {0x1000, "ITEMPLUR0"}, {0x1001, "ITEMPLUR1"},
        {0x1100, "GENDBR"}, {0x1101, "NUMBRNCH"}, {0x1302, "iCOLOR2"}, {0x1303, "iCOLOR3"},
        {0x0200, "NUM1"}, {0x0201, "NUM2"}, {0x0202, "NUM3"}, {0x0203, "NUM4"}, {0x0204, "NUM5"},
        {0x0205, "NUM6"}, {0x0206, "NUM7"}, {0x0207, "NUM8"}, {0x0208, "NUM9"}};
    for (size_t i = 0; i < sizeof names / sizeof names[0]; ++i) if (names[i].code == code) return names[i].name;
    return NULL;
}

static int put(char *out, size_t size, size_t *at, const char *s, size_t n)
{
    if (*at + n >= size) return 0;
    memcpy(out + *at, s, n);
    *at += n;
    return 1;
}
static int put_unit(char *out, size_t size, size_t *at, uint32_t c)
{
    char b[4]; size_t n;
    if (c == 0xE08D) c = 0x2026;      /* private-use glyphs pk3DS remaps */
    else if (c == 0xE08E) c = 0x2642;
    else if (c == 0xE08F) c = 0x2640;
    else if (c == 0xE07F) c = 0x202F;
    if (c < 0x80) { b[0] = (char)c; n = 1; }
    else if (c < 0x800) { b[0] = (char)(0xC0 | c >> 6); b[1] = (char)(0x80 | (c & 63)); n = 2; }
    else { b[0] = (char)(0xE0 | c >> 12); b[1] = (char)(0x80 | ((c >> 6) & 63)); b[2] = (char)(0x80 | (c & 63)); n = 3; }
    return put(out, size, at, b, n);
}

int px_gf_text_render(const struct px_gf_text *t, uint32_t index, const char *player_name, char *out, size_t size)
{
    if (!t || !out || !size || index >= t->line_count) return -1;
    out[0] = 0;
    const uint16_t *u = t->units + t->first[index];
    const uint32_t n = t->length[index];
    size_t at = 0;
    for (uint32_t i = 0; i < n; ++i) {
        const uint16_t c = u[i];
        if (c == 0) break;
        if (c == 0x10) {
            if (i + 1 >= n || !u[i + 1] || (uint64_t)i + 1 + u[i + 1] >= n) { out[at] = 0; return -1; }
            const uint16_t count = u[i + 1], code = u[i + 2];
            i += 1 + count;
            if (code == 0xBE00 || code == 0xBE01) {
                if (!put(out, size, &at, "\f", 1)) return -1;
                if (i + 1 < n && u[i + 1] == '\n') ++i; /* the break that follows starts the page */
                continue;
            }
            if (code == 0xBE02 || code == 0xBDFF || code == 0xFF00 || code == 0x1302 || code == 0x1303) continue;
            const char *name = code == 0x0100 && player_name ? player_name : NULL;
            char label[32];
            if (!name) {
                const char *v = variable_name(code);
                if (v) snprintf(label, sizeof label, "[%s]", v);
                else snprintf(label, sizeof label, "[%04X]", code);
                name = label;
            }
            if (!put(out, size, &at, name, strlen(name))) return -1;
            continue;
        }
        if (!put_unit(out, size, &at, c)) return -1;
    }
    out[at] = 0;
    return (int)at;
}

const char *px_gf_text_result_string(enum px_gf_text_result result)
{
    switch (result) {
    case PX_GF_TEXT_OK: return "ok";
    case PX_GF_TEXT_INVALID_ARGUMENT: return "invalid argument";
    case PX_GF_TEXT_TRUNCATED: return "truncated text file";
    case PX_GF_TEXT_INVALID: return "invalid text file";
    case PX_GF_TEXT_NO_MEMORY: return "out of memory";
    }
    return "unknown text result";
}
