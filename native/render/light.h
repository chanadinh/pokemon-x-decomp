#ifndef PX_LIGHT_H
#define PX_LIGHT_H

#include <stdint.h>

/* Fragment-lighting primary color for the subset the selected field uses:
 * directional lights, no LUTs, no distance/spot attenuation, no shadow or
 * bump mapping, no Fresnel. Arithmetic follows Azahar 1b5fd009
 * ComputeFragmentsColors in float32: light_vector = normalize(position),
 * dot = max(dot(L, N), 0) (or |dot| when two-sided), diffuse_sum starts at
 * (0,0,0,1) and accumulates (diffuse*dot + ambient) per light, then adds the
 * global ambient; each channel is clamped to 0..1, multiplied by 255 and
 * truncated to u8.
 *
 * Light colors are 10-bit register fields where 255 means 1.0. The game
 * runtime (code.bin 0x3030a8 loop) writes each as
 * ((2*light+1)*(2*material+1)) >> 10 per channel; px_light_register_color
 * reproduces that product. Light vectors here are in the same space as the
 * normals passed to px_light_primary. */
struct px_light_source {
    uint16_t ambient[3], diffuse[3]; /* register fields, RGB, 0..1023 */
    float vector[3];                 /* register position; normalized on use */
    uint32_t two_sided;
};
struct px_light_setup {
    struct px_light_source lights[8];
    uint32_t count;
    uint16_t global_ambient[3]; /* register 0x1c0 fields, RGB */
};

/* Register product for one channel of an RGBA8 light and material color. */
uint16_t px_light_register_channel(uint8_t light, uint8_t material);
/* Returns 0 for invalid arguments (NULL, count > 8, a field above 1023,
 * non-finite vector or zero-length vector or normal). out is RGBA8. A zero
 * normal is invalid because normalization is undefined. */
int px_light_primary(const struct px_light_setup *setup, const float normal[3],
                     uint8_t out[4]);

#endif
