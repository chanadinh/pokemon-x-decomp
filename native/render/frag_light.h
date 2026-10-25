#ifndef PX_FRAG_LIGHT_H
#define PX_FRAG_LIGHT_H

#include <stdint.h>

/* The PICA200 fragment lighting unit for directional lights, with its LUTs,
 * the normal quaternion and bump mapping, as Azahar 93f8939a
 * (video_core/renderer_software/sw_lighting.cpp ComputeFragmentsColors)
 * documents it, in float32:
 *
 * - surface normal: +Z, or a normal-map texel (rgb / 127.5 - 1, z rebuilt
 *   from x and y when renormalization is on), rotated by the normalized
 *   interpolated quaternion; the view vector is normalized;
 * - per light: L = normalize(position) (directional), H = V + L; LUT inputs
 *   N.H, V.H, N.V, L.N (as their cosines; H normalized for N.H and V.H);
 *   a LUT reads entry floor(x * 256) in 0..255 with x clamped at 0 (absolute
 *   inputs, |x| for two-sided lights) or the signed index floor(x * 128)
 *   in -128..127 stored as a byte, then value + difference * fraction, times
 *   the input scale;
 * - specular0 = D0 * specular0 color; reflection R/G/B from their LUTs (G and
 *   B fall back to R, R to 1); specular1 = D1 * reflection * specular1 color;
 *   the last light applies Fresnel to the primary and/or secondary alpha;
 *   dot = max(L.N, 0) (|L.N| two-sided); clamp_highlights zeroes specular
 *   where dot is 0; geometric factors scale specular0/1 by
 *   min(dot / |H|^2, 1);
 * - diffuse_sum = (0, 0, 0, 1) + sum(diffuse * dot + ambient) + global
 *   ambient, specular_sum = (0, 0, 0, 1) + sum(specular0 + specular1), each
 *   channel clamped to 0..1, times 255 and truncated.
 *
 * Not implemented: distance and spot attenuation, shadows, tangent maps,
 * positional lights (the callers' lights are directional). Colors are the
 * hardware's 10-bit register fields where 255 is 1.0. */
enum { PX_LUT_D0 = 0, PX_LUT_D1 = 1, PX_LUT_FR = 3, PX_LUT_RB = 4, PX_LUT_RG = 5, PX_LUT_RR = 6, PX_LUT_SLOTS = 7 };
enum { PX_LUT_NH = 0, PX_LUT_VH = 1, PX_LUT_NV = 2, PX_LUT_LN = 3 };
struct px_frag_light_source {
    uint16_t specular0[3], specular1[3], diffuse[3], ambient[3]; /* register fields RGB */
    float vector[3];                                            /* register position (directional) */
    int two_sided, geometric0, geometric1;
};
struct px_frag_light_config {
    const uint32_t *lut[PX_LUT_SLOTS]; /* 256 raw entries, NULL: disabled */
    uint8_t input[PX_LUT_SLOTS], absolute[PX_LUT_SLOTS];
    float scale[PX_LUT_SLOTS];
    int clamp_highlights, bump, bump_renormalize, bump_unit;
    int fresnel_primary, fresnel_secondary;
    struct px_frag_light_source lights[8];
    uint32_t count;
    uint16_t global_ambient[3];
};
/* Decodes the LUT input registers 0x1d0 (absolute), 0x1d1 (select) and
 * 0x1d2 (scale) for each slot. */
void px_frag_light_inputs(struct px_frag_light_config *config, uint32_t absolute, uint32_t select, uint32_t scale);
/* One fragment. quaternion: interpolated o1 (normalized here); view:
 * interpolated o2; bump: the bump unit's RGBA8 texel (ignored without bump).
 * Returns 0 for invalid input (NULL, count > 8, a zero quaternion). */
int px_frag_light(const struct px_frag_light_config *config, const float quaternion[4], const float view[3],
                  const uint8_t bump[4], uint8_t primary[4], uint8_t secondary[4]);
/* One LUT lookup (exposed for tests). */
float px_frag_lut(const uint32_t *lut, float input, int absolute, int two_sided);
#endif
