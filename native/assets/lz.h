#ifndef PX_NATIVE_LZ_H
#define PX_NATIVE_LZ_H

#include <stddef.h>
#include <stdint.h>

/* Decode Nintendo LZ10 or LZ11, including the LZ11 extended size header.
 * Return 1 on success, 0 on malformed/unsupported input or budget failure.
 * Caller frees *output. Failure leaves *output=NULL and *output_size=0.
 * Up to three trailing zero bytes are accepted as alignment padding.
 * The input is a single stream, not a container or a game-specific wrapper. */
int px_lz_decode(const uint8_t *input, size_t input_size, size_t max_output,
                  uint8_t **output, size_t *output_size,
                  char *error, size_t error_size);

#endif
