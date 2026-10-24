#include "gf_motion.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

/* Byte builder for a hand-authored GF1 motion pack. */
struct buf { uint8_t d[4096]; size_t n; };
static void b8(struct buf *b, unsigned v) { b->d[b->n++] = (uint8_t)v; }
static void b16(struct buf *b, unsigned v) { b8(b, v & 255); b8(b, v >> 8); }
static void b32(struct buf *b, uint32_t v) { b16(b, v & 0xffff); b16(b, v >> 16); }
static void bf(struct buf *b, float f) { uint32_t v; memcpy(&v, &f, 4); b32(b, v); }
static void align(struct buf *b, unsigned to) { while (b->n % to) b8(b, 0); }
static void put32(struct buf *b, size_t at, uint32_t v)
{
    for (unsigned i = 0; i < 4; ++i) b->d[at + i] = (uint8_t)(v >> (8 * i));
}
/* Codes packed eight per little-endian 24-bit word. */
static void codes(struct buf *b, const uint8_t *c, unsigned n, unsigned frames)
{
    b16(b, n); b16(b, frames);
    for (unsigned i = 0; i < n; i += 8) {
        uint32_t word = 0;
        for (unsigned j = 0; j < 8 && i + j < n; ++j) word |= (uint32_t)c[i + j] << (3 * j);
        b8(b, word & 255); b8(b, (word >> 8) & 255); b8(b, word >> 16);
    }
}

/* Slots: 0 skeleton, 1 motion (4 frames), 2 empty, 3 motion (300 frames,
 * u16 key frames), table end. Skeleton: Origin, A (parent Origin), B
 * (parent A, flags 8 = world space). */
static size_t build(struct buf *b, size_t *slot_offsets)
{
    memset(b, 0, sizeof *b);
    b32(b, 4);
    const size_t table = b->n;
    for (unsigned i = 0; i < 5; ++i) b32(b, 0);
    slot_offsets[0] = b->n;
    b8(b, 3); b8(b, 1);
    b8(b, 0); b8(b, 0); b8(b, 1);
    b8(b, 1); b8(b, 8); b8(b, 0);
    memcpy(b->d + b->n, "A\0B\0", 4); b->n += 4;
    align(b, 4);
    for (unsigned i = 0; i < 3; ++i) {
        bf(b, (float)i); bf(b, 0); bf(b, 0);
        bf(b, 0); bf(b, 0); bf(b, 0); bf(b, 1);
    }
    /* Motion 1: two header codes; Origin skips T, R, S; A: TX 5, TY 6, TZ 0,
     * RX 2, RY 3, RZ 4, scale skipped; B: TX 7, then nothing. */
    slot_offsets[1] = b->n;
    static const uint8_t c1[] = {0, 0, 1, 1, 1, 5, 6, 0, 2, 3, 4, 1, 7};
    codes(b, c1, sizeof c1, 4);
    b8(b, 1); b8(b, 2); /* TY keys at 0, 2, 4 */
    b8(b, 0);           /* B TX keys at 0, 4 */
    align(b, 4);
    bf(b, 2.5f);                               /* A TX */
    bf(b, 0); bf(b, 4); bf(b, 2);              /* A TY */
    bf(b, 0); bf(b, 0); bf(b, 8); bf(b, 0);    /* B TX value, slope */
    /* Motion 3: Origin TX linear over 300 frames, key at 150. */
    slot_offsets[3] = b->n;
    static const uint8_t c3[] = {0, 0, 6};
    codes(b, c3, sizeof c3, 300);
    assert(b->n & 1); /* the fixture exercises the padding byte */
    b8(b, 0); /* SPICA aligns the absolute position to 2 */
    b16(b, 1); b16(b, 150);
    align(b, 4);
    bf(b, 0); bf(b, 30); bf(b, 0);
    slot_offsets[4] = b->n;
    put32(b, table, (uint32_t)slot_offsets[0]);
    put32(b, table + 4, (uint32_t)slot_offsets[1]);
    put32(b, table + 12, (uint32_t)slot_offsets[3]);
    put32(b, table + 16, (uint32_t)slot_offsets[4]);
    return b->n;
}

static int near(float a, float b) { return fabsf(a - b) < 1e-6f; }

static void test_decode_and_values(void)
{
    struct buf b;
    size_t at[5];
    const size_t n = build(&b, at);
    struct px_gf_motion_pack p;
    assert(px_gf_motion_pack_decode(b.d, n, &p) == PX_GF_MOTION_OK);
    assert(p.bone_count == 3 && p.first_bone == 1 && p.slot_count == 4 && p.motion_count == 2);
    assert(!strcmp(p.bones[0].name, "Origin") && !strcmp(p.bones[2].name, "B"));
    assert(p.bones[2].parent == 1 && p.bones[2].flags == 8 && p.bones[1].children == 1 && p.bones[0].children == 0);
    assert(p.bones[1].translation[0] == 1 && p.bones[2].rotation[3] == 1);
    assert(!px_gf_motion_find(&p, 2) && !px_gf_motion_find(&p, 0));
    const struct px_gf_motion *m = px_gf_motion_find(&p, 1);
    assert(m && m->frames == 4 && m->byte_offset == at[1] && m->byte_offset + m->byte_length == at[3]);
    const struct px_gf_channel *a = m->channels + 9, *bb = m->channels + 18;
    for (unsigned c = 0; c < 9; ++c) assert(m->channels[c].kind == PX_GF_CHANNEL_ABSENT);
    assert(a[0].kind == PX_GF_CHANNEL_CONSTANT && near(px_gf_channel_value(&p, a, 3, 9), 2.5f));
    assert(a[1].kind == PX_GF_CHANNEL_LINEAR && a[1].key_count == 3);
    assert(near(px_gf_channel_value(&p, a + 1, 1, 0), 2) && near(px_gf_channel_value(&p, a + 1, 3, 0), 3));
    assert(near(px_gf_channel_value(&p, a + 1, 2, 0), 4) && near(px_gf_channel_value(&p, a + 1, 9, 0), 2));
    assert(px_gf_channel_value(&p, a + 2, 1, 7) == 0);
    assert(a[3].kind == PX_GF_CHANNEL_CONSTANT && px_gf_channel_value(&p, a + 3, 0, 0) == (float)(M_PI / 2));
    assert(px_gf_channel_value(&p, a + 4, 0, 0) == (float)M_PI);
    assert(px_gf_channel_value(&p, a + 5, 0, 0) == (float)(-M_PI / 2));
    for (unsigned c = 6; c < 9; ++c) assert(a[c].kind == PX_GF_CHANNEL_ABSENT);
    assert(px_gf_channel_value(&p, a + 6, 1, 1.5f) == 1.5f); /* absent: fallback */
    /* Hermite with zero slopes: 8 (3 - 2w) w^2 at w = 0.25, 0.5. */
    assert(bb[0].kind == PX_GF_CHANNEL_HERMITE && bb[0].key_count == 2);
    assert(near(px_gf_channel_value(&p, bb, 1, 0), 1.25f) && near(px_gf_channel_value(&p, bb, 2, 0), 4));
    assert(near(px_gf_channel_value(&p, bb, 4, 0), 8));
    for (unsigned c = 1; c < 9; ++c) assert(bb[c].kind == PX_GF_CHANNEL_ABSENT);
    const struct px_gf_motion *m3 = px_gf_motion_find(&p, 3);
    assert(m3 && m3->frames == 300 && m3->channels[0].key_count == 3);
    assert(m3->byte_offset + m3->byte_length == at[4]);
    assert(near(px_gf_channel_value(&p, m3->channels, 75, 0), 15));
    assert(near(px_gf_channel_value(&p, m3->channels, 225, 0), 15));
    px_gf_motion_pack_free(&p);
    assert(p.bones == NULL && p.motions == NULL);
}

static void test_slopes(void)
{
    /* Slopes are divided by 30 (SPICA GF1Motion.SetKeyFrameGroup). */
    struct px_gf_key keys[2] = {{0, 0, 30}, {2, 0, 0}};
    struct px_gf_motion_pack p = {0};
    p.keys = keys;
    struct px_gf_channel ch = {PX_GF_CHANNEL_HERMITE, 0, 2};
    /* w = 0.5, diff = 1: (diff (w - 1)) (ls (w - 1)) = (-0.5)(-0.5) = 0.25. */
    assert(near(px_gf_channel_value(&p, &ch, 1, 0), 0.25f));
}

static void test_rejections(void)
{
    struct buf b;
    size_t at[5];
    const size_t n = build(&b, at);
    struct px_gf_motion_pack p;
    assert(px_gf_motion_pack_decode(NULL, n, &p) == PX_GF_MOTION_INVALID_ARGUMENT);
    assert(px_gf_motion_pack_decode(b.d, n, NULL) == PX_GF_MOTION_INVALID_ARGUMENT);
    assert(px_gf_motion_pack_decode(b.d, 7, &p) == PX_GF_MOTION_TRUNCATED);
    /* Cutting the pack inside motion 3 truncates it. */
    assert(px_gf_motion_pack_decode(b.d, n - 2, &p) != PX_GF_MOTION_OK);
    struct buf c = b;
    put32(&c, 4 + 4 * 3, (uint32_t)(at[3] - 4)); /* motion 1 then runs past slot 3's start */
    assert(px_gf_motion_pack_decode(c.d, n, &p) != PX_GF_MOTION_OK && p.motions == NULL);
    c = b;
    put32(&c, 4 + 4 * 1, (uint32_t)n + 4); /* offset beyond the pack */
    assert(px_gf_motion_pack_decode(c.d, n, &p) == PX_GF_MOTION_INVALID);
    c = b;
    c.d[at[0] + 2] = 2; /* a bone naming itself or a later bone as parent */
    assert(px_gf_motion_pack_decode(c.d, n, &p) == PX_GF_MOTION_INVALID);
    c = b;
    put32(&c, 0, 5000);
    assert(px_gf_motion_pack_decode(c.d, n, &p) == PX_GF_MOTION_LIMIT_EXCEEDED);
    /* More channel codes than nine per bone: motion 1 with extra codes. */
    c = b;
    c.d[at[1]] = 40;
    assert(px_gf_motion_pack_decode(c.d, n, &p) != PX_GF_MOTION_OK);
    /* A non-finite key value. */
    c = b;
    put32(&c, at[3] - 4, 0x7f800000);
    assert(px_gf_motion_pack_decode(c.d, n, &p) == PX_GF_MOTION_INVALID);
    assert(!px_gf_motion_find(NULL, 1));
    px_gf_motion_pack_free(NULL);
    assert(!strcmp(px_gf_motion_result_string(PX_GF_MOTION_TRUNCATED), "truncated motion pack"));
}

int main(void)
{
    test_decode_and_values();
    test_slopes();
    test_rejections();
    puts("GF1 motion tests passed");
    return 0;
}
