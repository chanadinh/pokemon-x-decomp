#ifndef PX_TEXT_H
#define PX_TEXT_H

#include <stddef.h>
#include <stdint.h>

/* Message-window drawing for the inspection viewer: a self-authored 5x7
 * bitmap font (render/text_glyphs.inc; ASCII and the accented letters,
 * quotes, ellipsis and symbols that the English story text uses; other
 * characters draw as a hollow box) and a two-line window. Not the game's
 * font or window art. Pixels are straight RGBA8, top-down. */

/* Next UTF-8 code point from *p (advanced); invalid bytes give U+FFFD. */
uint32_t px_text_next(const char **p);
/* Whether the font has a glyph for the code point. */
int px_text_has_glyph(uint32_t code);
/* Draw one line (stops at '\n', '\f' or the end); returns the advance in
 * pixels. Each glyph is 5x7 cells of `scale` pixels, advance 6 * scale. */
int px_text_draw(uint8_t *rgba, uint32_t width, uint32_t height, int x, int y, const char *text,
                 const uint8_t color[4], int scale);
/* A message window over the bottom of the frame: a framed box and up to two
 * lines of `page` (lines separated by '\n'), with a small arrow when `more`. */
void px_text_window(uint8_t *rgba, uint32_t width, uint32_t height, const char *page, int more);

#endif
