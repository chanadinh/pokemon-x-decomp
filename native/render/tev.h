#ifndef PX_TEV_H
#define PX_TEV_H

#include <stdint.h>

/* PICA texture-environment (TEV) combiner evaluation on 8-bit RGBA values.
 *
 * Register layouts follow the pinned CTRMap PICACommandReader. Arithmetic
 * follows the documented integer behavior of the Azahar software rasterizer
 * (an emulator, not hardware measurement; its Dot3 is noted there as up to
 * +/-3 off). The code is independently written; no emulator code is copied.
 *
 * Per stage: source register (RGB sources in bits 0..11, alpha 16..27, four
 * bits each), operand register (RGB 0..11, alpha 12..23), combine register
 * (RGB 0..15, alpha 16..31), constant RGBA (byte0=R .. byte3=A) and scale
 * (RGB bits 0..1, alpha 16..17; 0/1/2 multiply by 1/2/4, 3 by 1). Buffer
 * color uses the constant layout. buffer_update bits 8..11 / 12..15 select
 * which of stages 0..3 write RGB / alpha to the combiner buffer; a stage sees
 * the buffer as updated by stages before the previous one (one-stage delay). */

enum px_tev_source {
    PX_TEV_PRIMARY = 0, PX_TEV_FRAGMENT_PRIMARY = 1, PX_TEV_FRAGMENT_SECONDARY = 2,
    PX_TEV_TEXTURE0 = 3, PX_TEV_TEXTURE1 = 4, PX_TEV_TEXTURE2 = 5, PX_TEV_TEXTURE3 = 6,
    PX_TEV_PREVIOUS_BUFFER = 13, PX_TEV_CONSTANT = 14, PX_TEV_PREVIOUS = 15
};

struct px_tev_stage { uint32_t source, operand, combine, constant, scale; };
struct px_tev_config {
    struct px_tev_stage stages[6];
    uint32_t buffer_color, buffer_update;
};
struct px_tev_inputs {
    uint8_t primary[4], fragment_primary[4], fragment_secondary[4];
    uint8_t texture[4][4];
};

/* Nonzero when every stage uses defined sources (0..6, 13..15), operands
 * (RGB 0..5, 8, 9, 12, 13; alpha 0..7) and operations (0..9), and no reserved
 * register bits are set. Evaluation of an invalid config is not defined. */
int px_tev_valid(const struct px_tev_config *config);

/* Bitmask of sources (bit = enum value) read by any stage, after the stage-0
 * substitution below. A conservative dependency set, not a sensitivity proof. */
uint32_t px_tev_sources(const struct px_tev_config *config);

/* Six stages in order. At stage 0, an RGB first/second source of PREVIOUS is
 * replaced by its third source; PREVIOUS otherwise starts as zero. */
void px_tev_evaluate(const struct px_tev_config *config, const struct px_tev_inputs *inputs,
                     uint8_t out[4]);

#endif
