#include "etc1.h"

/* Independently implemented from the ETC1 bit layout and PICA storage facts.
 * Pinned references: native/reproduction/texture-formats/README.txt. */
static uint64_t little_word(const uint8_t *data)
{
    uint64_t word = 0;
    for (unsigned byte = 0; byte < 8; ++byte)
        word |= (uint64_t)data[byte] << (8 * byte);
    return word;
}

static uint8_t clamp_channel(int value)
{
    if (value < 0) return 0;
    if (value > 255) return 255;
    return (uint8_t)value;
}

enum px_etc1_result px_etc1_decode_block(const uint8_t *data, size_t size,
                                        int alpha, uint8_t *rgba,
                                        size_t rgba_size)
{
    if (data == NULL || rgba == NULL || (alpha != 0 && alpha != 1))
        return PX_ETC1_INVALID_ARGUMENT;
    const size_t block_size = alpha ? 16 : 8;
    if (size < block_size) return PX_ETC1_TRUNCATED;
    if (rgba_size < 64) return PX_ETC1_OUTPUT_TOO_SMALL;

    const uint64_t colors = little_word(data + (alpha ? 8 : 0));
    const uint64_t alphas = alpha ? little_word(data) : UINT64_MAX;
    const unsigned differential = (unsigned)((colors >> 33) & 1);
    const unsigned horizontal = (unsigned)((colors >> 32) & 1);
    int base[2][3];
    for (unsigned channel = 0; channel < 3; ++channel) {
        const unsigned encoded = (unsigned)((colors >> (56 - 8 * channel)) & 255);
        if (differential) {
            const int first = (int)(encoded >> 3);
            const int bits = (int)(encoded & 7);
            const int second = first + (bits < 4 ? bits : bits - 8);
            if (second < 0 || second > 31) return PX_ETC1_UNSUPPORTED_BLOCK;
            base[0][channel] = first * 8 + first / 4;
            base[1][channel] = second * 8 + second / 4;
        } else {
            base[0][channel] = (int)(encoded >> 4) * 17;
            base[1][channel] = (int)(encoded & 15) * 17;
        }
    }
    const unsigned tables[2] = {
        (unsigned)((colors >> 37) & 7), (unsigned)((colors >> 34) & 7)
    };
    static const int modifiers[8][4] = {
        {2,8,-2,-8}, {5,17,-5,-17}, {9,29,-9,-29}, {13,42,-13,-42},
        {18,60,-18,-60}, {24,80,-24,-80}, {33,106,-33,-106}, {47,183,-47,-183}
    };
    for (unsigned y = 0; y < 4; ++y) {
        for (unsigned x = 0; x < 4; ++x) {
            const unsigned index = 4 * x + y;
            const unsigned half = (horizontal ? y : x) / 2;
            const unsigned selector = (unsigned)((colors >> index) & 1) |
                (unsigned)(((colors >> (16 + index)) & 1) << 1);
            const int modifier = modifiers[tables[half]][selector];
            uint8_t *pixel = rgba + 4 * (4 * y + x);
            for (unsigned channel = 0; channel < 3; ++channel)
                pixel[channel] = clamp_channel(base[half][channel] + modifier);
            pixel[3] = (uint8_t)(((alphas >> (4 * index)) & 15) * 17);
        }
    }
    return PX_ETC1_OK;
}
