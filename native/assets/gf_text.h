#ifndef PX_GF_TEXT_H
#define PX_GF_TEXT_H

#include <stddef.h>
#include <stdint.h>

/* XY text files (story text a/0/8/0.., game text a/0/7/2..), as read by
 * pk3DS 6daaca9 (TextFile.cs): u16 section count (1), u16 line count, u32
 * total length, u32 initial key (0), u32 section offset; at the section, u32
 * section length (equal to the total), then per line a u32 offset (from the
 * section start) and a u16 length in UTF-16 units; section offset + total
 * length is the file size. Line i is XOR-encrypted with a key that starts at
 * 0x7C89 + i * 0x2983 and rotates left by 3 after every unit.
 *
 * Decrypted lines contain UTF-16 text and variables: 0x0010, a count n, then
 * n units (the variable code and n - 1 arguments). Codes 0xBE00 (scroll),
 * 0xBE01 (clear) and 0xBE02 (wait) control the message window; 0xBDFF marks
 * an empty line. Names of the other codes follow pk3DS TextVariableCode.cs. */

enum px_gf_text_result {
    PX_GF_TEXT_OK = 0, PX_GF_TEXT_INVALID_ARGUMENT, PX_GF_TEXT_TRUNCATED, PX_GF_TEXT_INVALID,
    PX_GF_TEXT_NO_MEMORY
};
struct px_gf_text {
    uint32_t line_count;
    uint32_t *first, *length; /* per line: start in units, length in units */
    uint16_t *units;          /* all decrypted lines */
};

enum px_gf_text_result px_gf_text_decode(const uint8_t *data, size_t size, struct px_gf_text *out);
void px_gf_text_free(struct px_gf_text *text);
/* Render line `index` as UTF-8 for a message window: '\n' between lines,
 * '\f' where the window scrolls or clears (a new page), variables shown as
 * [NAME] except TRNAME (the player's name), the window controls and the
 * text-colour codes (dropped). Text ends at the first 0 unit. Returns the
 * length written (excluding the terminating 0, always written when size > 0),
 * or -1 for a bad index or an output that does not fit. */
int px_gf_text_render(const struct px_gf_text *text, uint32_t index, const char *player_name,
                      char *out, size_t size);
const char *px_gf_text_result_string(enum px_gf_text_result result);

#endif
