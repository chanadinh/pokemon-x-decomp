#ifndef PX_GF_MOTION_H
#define PX_GF_MOTION_H

#include <stddef.h>
#include <stdint.h>

/* Game Freak "GF1" skeletal motion packs, as read by SPICA bd29a78
 * (Formats/GFL/GF1MotionPack.cs, Motion/GF1Motion.cs, GF1MotBone.cs): u32
 * slot count, slot offsets from the pack start, slot 0 a motion skeleton,
 * slots 1.. motions (offset 0 = empty slot).
 *
 * Skeleton: u8 bone count, u8 first-bone index, then per bone after the
 * implicit "Origin": parent, flags, child count bytes; NUL-terminated names;
 * 4-byte alignment; per bone translation (3 floats) and quaternion (4).
 *
 * Motion: u16 code count, u16 frame count, 3-bit codes packed eight per
 * 24-bit word; one key-frame list per code above 5 (u8 counts and frames, or
 * u16 when frames > 255, then aligned to 2), with implicit first frame 0 and
 * last frame = frame count; 4-byte alignment; then values. Codes after the
 * first two address nine channels per bone (TX TY TZ RX RY RZ SX SY SZ): 1
 * skips three channels, 0 constant 0, 2 +pi/2, 3 pi, 4 -pi/2, 5 one float, 6
 * linear list (one float per key), 7 Hermite list (value and slope per key).
 * The parser bounds every read and requires each motion to end at or before
 * the next slot. Bone flag 8 marks world-space channels (SPICA IsWorldSpace). */

enum px_gf_motion_result {
    PX_GF_MOTION_OK = 0, PX_GF_MOTION_INVALID_ARGUMENT, PX_GF_MOTION_TRUNCATED,
    PX_GF_MOTION_INVALID, PX_GF_MOTION_LIMIT_EXCEEDED, PX_GF_MOTION_NO_MEMORY
};
enum px_gf_channel_kind { PX_GF_CHANNEL_ABSENT = 0, PX_GF_CHANNEL_CONSTANT, PX_GF_CHANNEL_LINEAR,
                          PX_GF_CHANNEL_HERMITE };
struct px_gf_motion_bone {
    char name[64];
    int32_t parent;
    uint8_t flags, children;
    float translation[3], rotation[4]; /* quaternion x, y, z, w */
};
struct px_gf_key { uint32_t frame; float value, slope; };
struct px_gf_channel { uint32_t kind, first_key, key_count; };
struct px_gf_motion {
    uint32_t slot, frames;
    uint32_t byte_offset, byte_length; /* bytes the parser consumed */
    struct px_gf_channel *channels; /* 9 per skeleton bone */
};
struct px_gf_motion_pack {
    uint32_t bone_count, first_bone, slot_count, motion_count, key_count;
    struct px_gf_motion_bone *bones;
    struct px_gf_motion *motions;
    struct px_gf_key *keys;
};

/* Caps: 16 MiB input, 256 bones, 4096 slots, 65535 frames, 4 Mi keys.
 * Output owns everything; on failure it is zeroed. */
enum px_gf_motion_result px_gf_motion_pack_decode(const uint8_t *data, size_t size,
                                                  struct px_gf_motion_pack *out);
void px_gf_motion_pack_free(struct px_gf_motion_pack *pack);
const struct px_gf_motion *px_gf_motion_find(const struct px_gf_motion_pack *pack, uint32_t slot);
/* Channel value at a frame, following SPICA H3DFloatKeyFrameGroup.GetFrameValue
 * with Hermite slopes scaled by 1/30 as GF1Motion.SetKeyFrameGroup does (SPICA
 * notes that scale was found by trial). Absent channels return fallback. */
float px_gf_channel_value(const struct px_gf_motion_pack *pack, const struct px_gf_channel *channel,
                          float frame, float fallback);
const char *px_gf_motion_result_string(enum px_gf_motion_result result);

#endif
