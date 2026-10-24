#ifndef PX_BCH_SCENE_H
#define PX_BCH_SCENE_H

#include "bch_model.h"

/* Environment BCH decoding: lights (resource category 5) and one scene's light
 * sets (category 14). Implemented in bch_model.c to share its checked
 * section/relocation reader; nothing is executed.
 *
 * Light records follow the pinned SPICA H3DLight/H3DFragmentLight field order:
 * name, scale/rotation/translation vec3s, type, enabled, flags, LUT config,
 * metadata pointer, then (fragment lights) ambient, diffuse, specular0,
 * specular1 RGBA8, direction vec3, two nullable LUT sampler pointers,
 * attenuation start/end. Ambient lights (type 2) carry one RGBA8 color in the
 * ambient field. Hemisphere and vertex lights are rejected.
 *
 * Scene records: name, then (pointer, count) pairs for cameras, light sets and
 * fogs, as in the pinned Ohana3DS reader. In the observed files each
 * light-set entry is 12 bytes: set index, pointer to an array of light-name
 * pointers, and that array's count. Every name must resolve to exactly one
 * light. This grouping is an observation recorded with the evidence, not a
 * documented layout. */

enum px_bch_light_type {
    PX_BCH_LIGHT_AMBIENT = 2,
    PX_BCH_LIGHT_FRAGMENT = 8, PX_BCH_LIGHT_FRAGMENT_DIRECTIONAL = 9,
    PX_BCH_LIGHT_FRAGMENT_POINT = 10, PX_BCH_LIGHT_FRAGMENT_SPOT = 11
};

struct px_bch_light {
    struct px_bch_name name;
    uint32_t type, enabled, flags, lut_config;
    float scale[3], rotation[3], translation[3], direction[3];
    float attenuation_start, attenuation_end;
    uint8_t ambient[4], diffuse[4], specular0[4], specular1[4];
    uint32_t has_distance_sampler, has_angle_sampler;
};
struct px_bch_light_set { uint32_t index, first, count; };
struct px_bch_environment {
    struct px_bch_name scene_name;
    struct px_bch_light *lights;
    struct px_bch_light_set *sets;
    uint32_t *set_lights; /* indices into lights; sets reference ranges */
    uint32_t light_count, set_count, set_light_count;
    uint32_t camera_count, fog_count; /* metadata only */
};

/* Decode every light and the light sets of scene scene_index. Caps: 256
 * lights, 256 sets, 4096 set entries. Output owns all arrays and names; on
 * failure it is zeroed. Pass a fresh or freed output. */
enum px_bch_model_result px_bch_environment_decode(const uint8_t *data, size_t size,
    uint32_t scene_index, struct px_bch_environment *out);
void px_bch_environment_free(struct px_bch_environment *environment);
/* Light set with the given set index, or NULL. */
const struct px_bch_light_set *px_bch_environment_set(const struct px_bch_environment *e,
                                                      uint32_t set_index);

#endif
