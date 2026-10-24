#include "text.h"

#include "text_glyphs.inc"

uint32_t px_text_next(const char **p)
{
    const unsigned char *s = (const unsigned char *)*p;
    uint32_t c = s[0];
    int n = c < 0x80 ? 1 : (c & 0xE0) == 0xC0 ? 2 : (c & 0xF0) == 0xE0 ? 3 : (c & 0xF8) == 0xF0 ? 4 : 0;
    if (!n) { *p += 1; return 0xFFFD; }
    if (n > 1) {
        c &= 0x7F >> n;
        for (int i = 1; i < n; ++i) {
            if ((s[i] & 0xC0) != 0x80) { *p += 1; return 0xFFFD; }
            c = c << 6 | (s[i] & 0x3F);
        }
    }
    *p += n;
    return c;
}

static const uint8_t *glyph(uint32_t code)
{
    for (size_t i = 0; i < sizeof glyphs / sizeof glyphs[0]; ++i) if (glyphs[i].code == code) return glyphs[i].rows;
    return NULL;
}
int px_text_has_glyph(uint32_t code) { return glyph(code) != NULL; }

static void plot(uint8_t *rgba, uint32_t width, uint32_t height, int x, int y, const uint8_t c[4])
{
    if (x < 0 || y < 0 || (uint32_t)x >= width || (uint32_t)y >= height) return;
    uint8_t *p = rgba + ((size_t)y * width + (size_t)x) * 4;
    for (unsigned k = 0; k < 4; ++k) p[k] = c[k];
}
static void fill(uint8_t *rgba, uint32_t width, uint32_t height, int x0, int y0, int x1, int y1, const uint8_t c[4])
{
    for (int y = y0; y < y1; ++y) for (int x = x0; x < x1; ++x) plot(rgba, width, height, x, y, c);
}

void px_text_rect(uint8_t *rgba, uint32_t width, uint32_t height, int x0, int y0, int x1, int y1,
                  const uint8_t color[4])
{
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > (int)width) x1 = (int)width;
    if (y1 > (int)height) y1 = (int)height;
    fill(rgba, width, height, x0, y0, x1, y1, color);
}

int px_text_draw(uint8_t *rgba, uint32_t width, uint32_t height, int x, int y, const char *text,
                 const uint8_t color[4], int scale)
{
    static const uint8_t box[7] = {0x1F, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1F};
    int start = x;
    if (scale < 1) scale = 1;
    while (*text && *text != '\n' && *text != '\f') {
        const uint32_t c = px_text_next(&text);
        const uint8_t *rows = glyph(c == 0x202F ? 0x20 : c);
        if (!rows) rows = box;
        for (int r = 0; r < 7; ++r)
            for (int col = 0; col < 5; ++col)
                if (rows[r] & (0x10 >> col))
                    fill(rgba, width, height, x + col * scale, y + r * scale, x + (col + 1) * scale,
                         y + (r + 1) * scale, color);
        x += 6 * scale;
    }
    return x - start;
}

void px_text_window(uint8_t *rgba, uint32_t width, uint32_t height, const char *page, int more)
{
    static const uint8_t frame[4] = {72, 80, 96, 255}, paper[4] = {250, 250, 246, 255}, ink[4] = {48, 48, 56, 255};
    const int margin = 8, h = 46;
    const int x0 = margin, x1 = (int)width - margin, y1 = (int)height - 4, y0 = y1 - h;
    fill(rgba, width, height, x0 + 2, y0, x1 - 2, y1, frame);
    fill(rgba, width, height, x0, y0 + 2, x1, y1 - 2, frame);
    fill(rgba, width, height, x0 + 3, y0 + 2, x1 - 3, y1 - 2, paper);
    fill(rgba, width, height, x0 + 2, y0 + 3, x1 - 2, y1 - 3, paper);
    const char *line = page;
    for (int row = 0; row < 2 && line && *line; ++row) {
        px_text_draw(rgba, width, height, x0 + 10, y0 + 9 + row * 16, line, ink, 1);
        while (*line && *line != '\n') ++line;
        if (*line == '\n') ++line;
    }
    if (more) /* a downward arrow at the bottom right */
        for (int r = 0; r < 4; ++r) fill(rgba, width, height, x1 - 16 + r, y1 - 12 + r, x1 - 9 - r, y1 - 11 + r, ink);
}
