#include "lz.h"

#include <stdio.h>
#include <stdlib.h>

/* Independently implemented from stream layouts cross-checked against
 * https://github.com/magical/nlzss/blob/f27414f373eab53bfe3c1a819c40eb800323e690/lzss3.py and local assets.
 * Extended LZ11 size header also cross-checked against
 * https://github.com/kwsch/pk3DS/blob/6daaca934ca2284a73ab743bf89c848c57cd9de1/pk3DS.Core/CTR/LZSS.cs
 * These are forward LZ streams, not backwards-compressed executable overlays. */
static int fail(char *error, size_t size, const char *message)
{
    if (error != NULL && size != 0) (void)snprintf(error, size, "%s", message);
    return 0;
}

int px_lz_decode(const uint8_t *input, size_t input_size, size_t max_output,
                  uint8_t **output, size_t *output_size, char *error, size_t error_size)
{
    if (output != NULL) *output = NULL;
    if (output_size != NULL) *output_size = 0;
    if (error != NULL && error_size != 0) error[0] = '\0';
    if (input == NULL || output == NULL || output_size == NULL)
        return fail(error, error_size, "LZ: invalid argument");
    if (input_size < 4) return fail(error, error_size, "LZ: truncated header");
    if (input[0] != 0x10 && input[0] != 0x11)
        return fail(error, error_size, "LZ: only LZ10 and LZ11 are supported");
    size_t cursor = 4;
    uint32_t wanted = (uint32_t)input[1] | (uint32_t)input[2] << 8 | (uint32_t)input[3] << 16;
    if (wanted == 0 && input[0] == 0x11) {
        if (input_size < 8) return fail(error, error_size, "LZ: truncated extended size");
        wanted = (uint32_t)input[4] | (uint32_t)input[5] << 8 |
                 (uint32_t)input[6] << 16 | (uint32_t)input[7] << 24;
        cursor = 8;
    }
    if (wanted == 0) return fail(error, error_size, "LZ: empty streams are unsupported");
    if ((uintmax_t)wanted > (uintmax_t)max_output)
        return fail(error, error_size, "LZ: output exceeds byte budget");
    uint8_t *buffer = malloc(wanted);
    if (buffer == NULL) return fail(error, error_size, "LZ: output allocation failed");
    size_t written = 0;
    const char *problem = "LZ: truncated token";
    while (written < wanted) {
        if (cursor == input_size) goto bad;
        uint8_t flags = input[cursor++];
        for (unsigned bit = 0; bit < 8 && written < wanted; ++bit) {
            if (cursor == input_size) goto bad;
            uint8_t first = input[cursor++];
            if ((flags & (0x80u >> bit)) == 0) {
                buffer[written++] = first;
                continue;
            }
            size_t count, distance;
            if (input[0] == 0x10) {
                if (cursor == input_size) goto bad;
                count = (first >> 4) + 3u;
                distance = ((size_t)(first & 15u) << 8 | input[cursor++]) + 1;
            } else {
                uint8_t indicator = first >> 4;
                uint8_t middle = first;
                if (indicator == 0) {
                    if (cursor == input_size) goto bad;
                    middle = input[cursor++];
                    count = ((size_t)(first & 15u) << 4 | (middle >> 4)) + 0x11;
                } else if (indicator == 1) {
                    if (input_size - cursor < 2) goto bad;
                    uint8_t second = input[cursor++];
                    middle = input[cursor++];
                    count = ((size_t)(first & 15u) << 12 | (size_t)second << 4 |
                             (middle >> 4)) + 0x111;
                } else count = (size_t)indicator + 1;
                if (cursor == input_size) goto bad;
                distance = ((size_t)(middle & 15u) << 8 | input[cursor++]) + 1;
            }
            if (distance > written) {
                problem = "LZ: backreference precedes output";
                goto bad;
            }
            if (count > wanted - written) {
                problem = "LZ: token exceeds declared output size";
                goto bad;
            }
            for (size_t i = 0; i < count; ++i) {
                buffer[written] = buffer[written - distance];
                ++written;
            }
        }
    }
    if (input_size - cursor > 3) {
        problem = "LZ: unexpected trailing data";
        goto bad;
    }
    while (cursor < input_size)
        if (input[cursor++] != 0) {
            problem = "LZ: nonzero trailing data";
            goto bad;
        }
    *output = buffer;
    *output_size = wanted;
    return 1;
bad:
    free(buffer);
    return fail(error, error_size, problem);
}
