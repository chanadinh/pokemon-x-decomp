#include "gf_motion.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define PX_PI 3.14159265358979323846

enum { MAX_BYTES = 16u << 20, MAX_BONES = 256, MAX_SLOTS = 4096, MAX_KEYS = 4u << 20 };

static uint16_t u16(const uint8_t *p) { return (uint16_t)(p[0] | p[1] << 8); }
static uint32_t u32(const uint8_t *p) { return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24; }
static float f32(const uint8_t *p) { uint32_t b = u32(p); float f; memcpy(&f, &b, sizeof f); return f; }

struct cursor { const uint8_t *data; size_t at, end; };
static int take(struct cursor *c, size_t n) { return n <= c->end - c->at; }

static enum px_gf_motion_result skeleton(struct cursor c, struct px_gf_motion_pack *out)
{
    if (!take(&c, 2)) return PX_GF_MOTION_TRUNCATED;
    const unsigned count = c.data[c.at], first = c.data[c.at + 1];
    c.at += 2;
    if (!count) return PX_GF_MOTION_INVALID;
    out->bones = calloc(count, sizeof *out->bones);
    if (!out->bones) return PX_GF_MOTION_NO_MEMORY;
    out->bone_count = count;
    out->first_bone = first;
    strcpy(out->bones[0].name, "Origin");
    out->bones[0].parent = -1;
    for (unsigned i = 1; i < count; ++i) {
        if (!take(&c, 3)) return PX_GF_MOTION_TRUNCATED;
        out->bones[i].parent = c.data[c.at];
        out->bones[i].flags = c.data[c.at + 1];
        out->bones[i].children = c.data[c.at + 2];
        c.at += 3;
        if (out->bones[i].parent >= (int32_t)i) return PX_GF_MOTION_INVALID;
    }
    for (unsigned i = 1; i < count; ++i) {
        const uint8_t *end = memchr(c.data + c.at, 0, c.end - c.at);
        if (!end) return PX_GF_MOTION_TRUNCATED;
        const size_t length = (size_t)(end - (c.data + c.at));
        if (length >= sizeof out->bones[i].name) return PX_GF_MOTION_LIMIT_EXCEEDED;
        memcpy(out->bones[i].name, c.data + c.at, length);
        c.at += length + 1;
    }
    c.at = (c.at + 3) & ~(size_t)3;
    if (c.at > c.end || !take(&c, (size_t)count * 28)) return PX_GF_MOTION_TRUNCATED;
    for (unsigned i = 0; i < count; ++i, c.at += 28) {
        for (unsigned j = 0; j < 3; ++j) out->bones[i].translation[j] = f32(c.data + c.at + 4 * j);
        for (unsigned j = 0; j < 4; ++j) out->bones[i].rotation[j] = f32(c.data + c.at + 12 + 4 * j);
        for (unsigned j = 0; j < 3; ++j) if (!isfinite(out->bones[i].translation[j])) return PX_GF_MOTION_INVALID;
        for (unsigned j = 0; j < 4; ++j) if (!isfinite(out->bones[i].rotation[j])) return PX_GF_MOTION_INVALID;
    }
    return PX_GF_MOTION_OK;
}

static enum px_gf_motion_result add_keys(struct px_gf_motion_pack *out, uint32_t n, size_t *capacity)
{
    if (n > MAX_KEYS - out->key_count) return PX_GF_MOTION_LIMIT_EXCEEDED;
    if (out->key_count + n <= *capacity) return PX_GF_MOTION_OK;
    size_t grown = *capacity ? *capacity : 1024;
    while (grown < out->key_count + n) grown *= 2;
    void *p = realloc(out->keys, grown * sizeof *out->keys);
    if (!p) return PX_GF_MOTION_NO_MEMORY;
    out->keys = p;
    *capacity = grown;
    return PX_GF_MOTION_OK;
}

static enum px_gf_motion_result motion(struct cursor c, size_t pack_start, struct px_gf_motion_pack *out,
                                       struct px_gf_motion *m, size_t *capacity)
{
    const size_t start_at = c.at;
    if (!take(&c, 4)) return PX_GF_MOTION_TRUNCATED;
    const uint32_t codes = u16(c.data + c.at), frames = u16(c.data + c.at + 2);
    c.at += 4;
    m->frames = frames;
    uint8_t *code = malloc(codes ? codes : 1);
    uint32_t *lists = NULL; /* (frame-key start, count) pairs into a frame pool */
    uint32_t *frame_pool = NULL;
    enum px_gf_motion_result status = PX_GF_MOTION_NO_MEMORY;
    if (!code) return status;
    uint32_t word = 0, list_count = 0;
    for (uint32_t i = 0; i < codes; ++i) {
        if ((i & 7) == 0) {
            if (!take(&c, 3)) { status = PX_GF_MOTION_TRUNCATED; goto done; }
            word = c.data[c.at] | (uint32_t)c.data[c.at + 1] << 8 | (uint32_t)c.data[c.at + 2] << 16;
            c.at += 3;
        }
        code[i] = word & 7;
        word >>= 3;
        list_count += code[i] > 5;
    }
    const int wide = frames > 0xff;
    if (wide && ((c.at - pack_start) & 1)) c.at += 1; /* SPICA aligns the absolute position */
    lists = calloc((size_t)list_count * 2 + 2, sizeof *lists);
    size_t pool_size = 0, pool_capacity = 256;
    frame_pool = malloc(pool_capacity * sizeof *frame_pool);
    if (!lists || !frame_pool) goto done;
    for (uint32_t i = 0; i < list_count; ++i) {
        const size_t width = wide ? 2 : 1;
        if (c.at > c.end || !take(&c, width)) { status = PX_GF_MOTION_TRUNCATED; goto done; }
        const uint32_t n = wide ? u16(c.data + c.at) : c.data[c.at];
        c.at += width;
        if (!take(&c, (size_t)n * width)) { status = PX_GF_MOTION_TRUNCATED; goto done; }
        if (pool_size + n + 2 > pool_capacity) {
            while (pool_size + n + 2 > pool_capacity) pool_capacity *= 2;
            void *p = realloc(frame_pool, pool_capacity * sizeof *frame_pool);
            if (!p) goto done;
            frame_pool = p;
        }
        lists[2 * i] = (uint32_t)pool_size;
        lists[2 * i + 1] = n + 2;
        frame_pool[pool_size++] = 0;
        for (uint32_t j = 0; j < n; ++j, c.at += width)
            frame_pool[pool_size++] = wide ? u16(c.data + c.at) : c.data[c.at];
        frame_pool[pool_size++] = frames;
    }
    c.at = pack_start + (((c.at - pack_start) + 3) & ~(size_t)3);
    if (c.at > c.end) { status = PX_GF_MOTION_TRUNCATED; goto done; }
    uint32_t next_list = 0, element = 0;
    for (uint32_t i = 2; i < codes; ++i) {
        if (code[i] == 1) { element += 3; continue; }
        const uint32_t bone = element / 9, channel = element % 9;
        if (bone >= out->bone_count) { status = PX_GF_MOTION_INVALID; goto done; }
        struct px_gf_channel *ch = &m->channels[bone * 9 + channel];
        if (ch->kind != PX_GF_CHANNEL_ABSENT) { status = PX_GF_MOTION_INVALID; goto done; }
        if (code[i] <= 5) {
            float value = 0;
            if (code[i] == 2) value = (float)(PX_PI * 0.5);
            else if (code[i] == 3) value = (float)PX_PI;
            else if (code[i] == 4) value = (float)(PX_PI * -0.5);
            else if (code[i] == 5) {
                if (!take(&c, 4)) { status = PX_GF_MOTION_TRUNCATED; goto done; }
                value = f32(c.data + c.at);
                c.at += 4;
            }
            if ((status = add_keys(out, 1, capacity)) != PX_GF_MOTION_OK) goto done;
            ch->kind = PX_GF_CHANNEL_CONSTANT;
            ch->first_key = out->key_count;
            ch->key_count = 1;
            out->keys[out->key_count++] = (struct px_gf_key){0, value, 0};
        } else {
            if (next_list >= list_count) { status = PX_GF_MOTION_INVALID; goto done; }
            const uint32_t start = lists[2 * next_list], n = lists[2 * next_list + 1];
            ++next_list;
            const size_t per = code[i] == 7 ? 8 : 4;
            if (!take(&c, (size_t)n * per)) { status = PX_GF_MOTION_TRUNCATED; goto done; }
            if ((status = add_keys(out, n, capacity)) != PX_GF_MOTION_OK) goto done;
            ch->kind = code[i] == 7 ? PX_GF_CHANNEL_HERMITE : PX_GF_CHANNEL_LINEAR;
            ch->first_key = out->key_count;
            ch->key_count = n;
            for (uint32_t k = 0; k < n; ++k, c.at += per) {
                struct px_gf_key *key = &out->keys[out->key_count++];
                key->frame = frame_pool[start + k];
                key->value = f32(c.data + c.at);
                key->slope = per == 8 ? f32(c.data + c.at + 4) : 0;
                if (!isfinite(key->value) || !isfinite(key->slope)) { status = PX_GF_MOTION_INVALID; goto done; }
            }
        }
        ++element;
    }
    m->byte_offset = (uint32_t)start_at;
    m->byte_length = (uint32_t)(c.at - start_at);
    status = next_list == list_count ? PX_GF_MOTION_OK : PX_GF_MOTION_INVALID;
done:
    free(code); free(lists); free(frame_pool);
    return status;
}

enum px_gf_motion_result px_gf_motion_pack_decode(const uint8_t *data, size_t size,
                                                  struct px_gf_motion_pack *out)
{
    if (!out) return PX_GF_MOTION_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    if (!data) return PX_GF_MOTION_INVALID_ARGUMENT;
    if (size > MAX_BYTES) return PX_GF_MOTION_LIMIT_EXCEEDED;
    if (size < 8) return PX_GF_MOTION_TRUNCATED;
    const uint32_t slots = u32(data);
    if (!slots || slots > MAX_SLOTS) return PX_GF_MOTION_LIMIT_EXCEEDED;
    if ((size - 4) / 4 < (size_t)slots + 1) return PX_GF_MOTION_TRUNCATED;
    uint32_t *offsets = malloc(((size_t)slots + 1) * sizeof *offsets);
    if (!offsets) return PX_GF_MOTION_NO_MEMORY;
    for (uint32_t i = 0; i <= slots; ++i) offsets[i] = u32(data + 4 + 4 * (size_t)i);
    enum px_gf_motion_result status = PX_GF_MOTION_INVALID;
    size_t capacity = 0;
    out->slot_count = slots;
    /* Each nonzero slot ends at the next nonzero offset (or the input end). */
    size_t table_end = 4 + 4 * ((size_t)slots + 1);
    for (uint32_t i = 0; i <= slots; ++i)
        if (offsets[i] && (offsets[i] < table_end || offsets[i] > size)) goto fail;
    if (!offsets[0]) goto fail;
    size_t next = size;
    for (uint32_t j = 1; j <= slots; ++j) if (offsets[j]) { next = offsets[j]; break; }
    if (next < offsets[0]) goto fail;
    status = skeleton((struct cursor){data, offsets[0], next}, out);
    if (status != PX_GF_MOTION_OK) goto fail;
    out->motions = calloc(slots, sizeof *out->motions);
    if (!out->motions) { status = PX_GF_MOTION_NO_MEMORY; goto fail; }
    for (uint32_t i = 1; i < slots; ++i) {
        if (!offsets[i]) continue;
        size_t end = size;
        for (uint32_t j = i + 1; j <= slots; ++j) if (offsets[j]) { end = offsets[j]; break; }
        if (end < offsets[i]) { status = PX_GF_MOTION_INVALID; goto fail; }
        struct px_gf_motion *m = &out->motions[out->motion_count];
        m->slot = i;
        m->channels = calloc((size_t)out->bone_count * 9, sizeof *m->channels);
        if (!m->channels) { status = PX_GF_MOTION_NO_MEMORY; goto fail; }
        ++out->motion_count;
        status = motion((struct cursor){data, offsets[i], end}, 0, out, m, &capacity);
        if (status != PX_GF_MOTION_OK) goto fail;
    }
    free(offsets);
    return PX_GF_MOTION_OK;
fail:
    free(offsets);
    px_gf_motion_pack_free(out);
    return status;
}

void px_gf_motion_pack_free(struct px_gf_motion_pack *pack)
{
    if (!pack) return;
    for (uint32_t i = 0; pack->motions && i < pack->motion_count; ++i) free(pack->motions[i].channels);
    free(pack->motions); free(pack->bones); free(pack->keys);
    memset(pack, 0, sizeof *pack);
}

const struct px_gf_motion *px_gf_motion_find(const struct px_gf_motion_pack *pack, uint32_t slot)
{
    for (uint32_t i = 0; pack && i < pack->motion_count; ++i)
        if (pack->motions[i].slot == slot) return &pack->motions[i];
    return NULL;
}

float px_gf_channel_value(const struct px_gf_motion_pack *pack, const struct px_gf_channel *channel,
                          float frame, float fallback)
{
    if (!pack || !channel || channel->kind == PX_GF_CHANNEL_ABSENT || !channel->key_count) return fallback;
    const struct px_gf_key *keys = pack->keys + channel->first_key;
    const uint32_t n = channel->key_count;
    if (n == 1) return keys[0].value;
    /* SPICA: LHS = last key with frame <= f; RHS = first key with frame >= f,
     * scanning in order and keeping the smallest such frame. */
    const struct px_gf_key *lhs = &keys[0], *rhs = &keys[n - 1];
    for (uint32_t i = 0; i < n; ++i) {
        if ((float)keys[i].frame <= frame) lhs = &keys[i];
        if ((float)keys[i].frame >= frame && keys[i].frame < rhs->frame) rhs = &keys[i];
    }
    if (lhs->frame == rhs->frame) return lhs->value;
    const float diff = frame - (float)lhs->frame;
    const float weight = diff / (float)(rhs->frame - lhs->frame);
    if (channel->kind == PX_GF_CHANNEL_LINEAR) return lhs->value + (rhs->value - lhs->value) * weight;
    const float ls = lhs->slope / 30.0f, rs = rhs->slope / 30.0f;
    float result = lhs->value + (lhs->value - rhs->value) * (2 * weight - 3) * weight * weight;
    result += (diff * (weight - 1)) * (ls * (weight - 1) + rs * weight);
    return result;
}

const char *px_gf_motion_result_string(enum px_gf_motion_result result)
{
    switch (result) {
    case PX_GF_MOTION_OK: return "ok";
    case PX_GF_MOTION_INVALID_ARGUMENT: return "invalid argument";
    case PX_GF_MOTION_TRUNCATED: return "truncated motion pack";
    case PX_GF_MOTION_INVALID: return "invalid motion pack";
    case PX_GF_MOTION_LIMIT_EXCEEDED: return "motion pack limit exceeded";
    case PX_GF_MOTION_NO_MEMORY: return "out of memory";
    }
    return "unknown motion result";
}
