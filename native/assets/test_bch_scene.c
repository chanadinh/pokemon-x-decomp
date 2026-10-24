#include "bch_scene.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

/* Independently laid out environment: three lights in category 5 and one
 * scene in category 14 whose two light sets reference lights by name, in an
 * order different from the light table. */
enum { CONTENT = 56, STRING = 1080, RELOC = 1336,
       LIGHT_TABLE = 240, LIGHT_TREE = 256, SCENE_TABLE = 304, SCENE_TREE = 312,
       LIGHT0 = 336, LIGHT_SIZE = 108, SCENE = 660, SETS = 688, NAMES0 = 712, NAMES1 = 720,
       S_SCENE = 0, S_SUN = 6, S_NIGHT = 10, S_POINT = 16 };
struct fixture { uint8_t data[2048]; size_t size; };

static void word(uint8_t *p, uint32_t value)
{
    for (unsigned i = 0; i < 4; ++i) p[i] = (uint8_t)(value >> (8 * i));
}
static uint32_t float_bits(float value)
{
    uint32_t bits;
    memcpy(&bits, &value, sizeof bits);
    return bits;
}
static void pointer(struct fixture *f, size_t field, size_t target, unsigned tag)
{
    const size_t base = tag == 1 ? STRING : CONTENT;
    word(f->data + field, (uint32_t)(target - base));
    word(f->data + f->size, (uint32_t)(tag << 25) | (uint32_t)((field - CONTENT) / (tag == 1 ? 1 : 4)));
    f->size += 4;
}
static void light(struct fixture *f, unsigned index, unsigned name, unsigned type, unsigned flags,
                  const uint8_t ambient[4], const uint8_t diffuse[4], const float direction[3])
{
    const size_t at = LIGHT0 + (size_t)index * LIGHT_SIZE;
    pointer(f, at, STRING + name, 1);
    for (unsigned i = 0; i < 3; ++i) {
        word(f->data + at + 4 + i * 4, float_bits(1));
        word(f->data + at + 28 + i * 4, float_bits((float)(index * 10 + i)));
        word(f->data + at + 64 + i * 4, float_bits(direction[i]));
    }
    f->data[at + 40] = (uint8_t)type;
    f->data[at + 41] = 1;
    f->data[at + 42] = (uint8_t)flags;
    memcpy(f->data + at + 48, ambient, 4);
    memcpy(f->data + at + 52, diffuse, 4);
    f->data[at + 56] = 200; /* specular0 red */
    f->data[at + 63] = 77;  /* specular1 alpha */
    word(f->data + at + 84, float_bits(10));
    word(f->data + at + 88, float_bits(7000));
    pointer(f, LIGHT_TREE + ((size_t)index + 1) * 12 + 8, STRING + name, 1);
    pointer(f, LIGHT_TABLE + (size_t)index * 4, at, 0);
}
static struct fixture fixture(void)
{
    struct fixture f;
    memset(&f, 0, sizeof f);
    f.size = RELOC;
    memcpy(f.data, "BCH\0\7\7", 6);
    const uint32_t starts[] = {CONTENT, STRING, RELOC, RELOC, RELOC};
    const uint32_t lengths[] = {STRING - CONTENT, RELOC - STRING, 0, 0};
    for (unsigned i = 0; i < 5; ++i) word(f.data + 8 + i * 4, starts[i]);
    for (unsigned i = 0; i < 4; ++i) word(f.data + 28 + i * 4, lengths[i]);
    memcpy(f.data + STRING, "scene\0sun\0night\0point\0", 22);
    pointer(&f, CONTENT + 5 * 12, LIGHT_TABLE, 0);
    word(f.data + CONTENT + 5 * 12 + 4, 3);
    pointer(&f, CONTENT + 5 * 12 + 8, LIGHT_TREE, 0);
    pointer(&f, CONTENT + 14 * 12, SCENE_TABLE, 0);
    word(f.data + CONTENT + 14 * 12 + 4, 1);
    pointer(&f, CONTENT + 14 * 12 + 8, SCENE_TREE, 0);
    const uint8_t sun_a[4] = {199, 208, 226, 255}, sun_d[4] = {79, 104, 98, 255};
    const uint8_t night_a[4] = {61, 71, 107, 255}, night_d[4] = {89, 158, 112, 255};
    const uint8_t point_a[4] = {0, 0, 0, 255}, point_d[4] = {0, 3, 0, 255};
    const float down[3] = {0, -0.997f, 0.08f}, up[3] = {0, -1, 0};
    light(&f, 0, S_NIGHT, 9, 0, night_a, night_d, down);
    light(&f, 1, S_SUN, 9, 0, sun_a, sun_d, down);
    light(&f, 2, S_POINT, 10, 2, point_a, point_d, up);
    pointer(&f, SCENE_TABLE, SCENE, 0);
    pointer(&f, SCENE_TREE + 20, STRING + S_SCENE, 1);
    pointer(&f, SCENE, STRING + S_SCENE, 1);
    pointer(&f, SCENE + 0xc, SETS, 0);
    word(f.data + SCENE + 0x10, 2);
    word(f.data + SCENE + 0x18, 1); /* one fog, metadata only */
    word(f.data + SETS, 0);
    pointer(&f, SETS + 4, NAMES0, 0);
    word(f.data + SETS + 8, 2);
    word(f.data + SETS + 12, 3);
    pointer(&f, SETS + 16, NAMES1, 0);
    word(f.data + SETS + 20, 1);
    pointer(&f, NAMES0, STRING + S_SUN, 1);
    pointer(&f, NAMES0 + 4, STRING + S_POINT, 1);
    pointer(&f, NAMES1, STRING + S_NIGHT, 1);
    word(f.data + 44, (uint32_t)(f.size - RELOC));
    return f;
}
static void reject(struct fixture f, size_t at, uint32_t value, enum px_bch_model_result expected)
{
    struct px_bch_environment e;
    memset(&e, 0xff, sizeof e);
    word(f.data + at, value);
    const enum px_bch_model_result result = px_bch_environment_decode(f.data, f.size, 0, &e);
    assert(result != PX_BCH_MODEL_OK);
    if (expected != PX_BCH_MODEL_OK) assert(result == expected);
    const struct px_bch_environment empty = {0};
    assert(memcmp(&e, &empty, sizeof e) == 0);
}
static void test_decode(void)
{
    struct fixture f = fixture(), before = f;
    struct px_bch_environment e;
    assert(px_bch_environment_decode(f.data, f.size, 0, &e) == PX_BCH_MODEL_OK);
    assert(memcmp(&f, &before, sizeof f) == 0);
    memset(f.data, 0, sizeof f.data); /* output must not borrow the input */
    assert(strcmp(e.scene_name.bytes, "scene") == 0 && e.camera_count == 0 && e.fog_count == 1);
    assert(e.light_count == 3 && e.set_count == 2 && e.set_light_count == 3);
    const struct px_bch_light *night = &e.lights[0], *sun = &e.lights[1], *point = &e.lights[2];
    assert(strcmp(night->name.bytes, "night") == 0 && strcmp(sun->name.bytes, "sun") == 0);
    assert(sun->type == PX_BCH_LIGHT_FRAGMENT_DIRECTIONAL && sun->enabled == 1 && sun->flags == 0);
    assert(point->type == PX_BCH_LIGHT_FRAGMENT_POINT && point->flags == 2);
    assert(sun->ambient[0] == 199 && sun->ambient[2] == 226 && sun->diffuse[1] == 104);
    assert(night->diffuse[0] == 89 && point->diffuse[1] == 3 && point->ambient[3] == 255);
    assert(sun->specular0[0] == 200 && sun->specular1[3] == 77);
    assert(sun->direction[1] == -0.997f && sun->direction[2] == 0.08f && point->direction[1] == -1);
    assert(sun->translation[0] == 10 && point->translation[2] == 22 && sun->scale[2] == 1);
    assert(sun->attenuation_start == 10 && sun->attenuation_end == 7000);
    assert(!sun->has_distance_sampler && !sun->has_angle_sampler);
    /* Set 0 lists sun then point; set 3 lists night: indices into lights. */
    const struct px_bch_light_set *zero = px_bch_environment_set(&e, 0);
    const struct px_bch_light_set *three = px_bch_environment_set(&e, 3);
    assert(zero && zero->count == 2 && three && three->count == 1);
    assert(px_bch_environment_set(&e, 1) == NULL && px_bch_environment_set(NULL, 0) == NULL);
    assert(e.set_lights[zero->first] == 1 && e.set_lights[zero->first + 1] == 2);
    assert(e.set_lights[three->first] == 0);
    px_bch_environment_free(&e);
    px_bch_environment_free(&e);
    assert(e.lights == NULL && e.light_count == 0);
    px_bch_environment_free(NULL);
}
static void test_rejections(void)
{
    struct fixture f = fixture();
    struct px_bch_environment e;
    assert(px_bch_environment_decode(NULL, 0, 0, &e) == PX_BCH_MODEL_INVALID_ARGUMENT);
    assert(px_bch_environment_decode(f.data, f.size, 0, NULL) == PX_BCH_MODEL_INVALID_ARGUMENT);
    assert(px_bch_environment_decode(f.data, f.size, 1, &e) != PX_BCH_MODEL_OK); /* no scene 1 */
    assert(e.lights == NULL);
    for (size_t size = 0; size < f.size; ++size) {
        assert(px_bch_environment_decode(f.data, size, 0, &e) != PX_BCH_MODEL_OK);
        assert(e.lights == NULL && e.sets == NULL);
    }
    const size_t sun = LIGHT0 + LIGHT_SIZE;
    reject(f, sun + 40, 3, PX_BCH_MODEL_UNSUPPORTED);         /* hemisphere/vertex type */
    reject(f, sun + 40, 2u << 8 | 9, PX_BCH_MODEL_UNSUPPORTED); /* enabled byte 2 */
    reject(f, sun + 40, 4u << 16 | 1u << 8 | 9, PX_BCH_MODEL_UNSUPPORTED); /* unknown flag */
    reject(f, sun + 68, 0x7fc00000, PX_BCH_MODEL_OK);        /* nonfinite direction */
    reject(f, sun + 88, 0x7f800000, PX_BCH_MODEL_OK);        /* nonfinite attenuation */
    reject(f, LIGHT_TREE + 2 * 12 + 8, S_NIGHT, PX_BCH_MODEL_OK); /* tree/object name differ */
    reject(f, LIGHT0, S_SUN, PX_BCH_MODEL_OK);               /* duplicate light names */
    reject(f, SETS + 12, 0, PX_BCH_MODEL_OK);                /* duplicate set index */
    reject(f, NAMES1, S_SCENE, PX_BCH_MODEL_OK);             /* set names no light */
    reject(f, SETS + 8, 100, PX_BCH_MODEL_OK);               /* names array overruns */
    reject(f, SCENE + 0x10, 257, PX_BCH_MODEL_LIMIT_EXCEEDED);
    reject(f, CONTENT + 5 * 12 + 4, 257, PX_BCH_MODEL_OK);   /* light count beyond table */
    /* Two relocations of one field are rejected. */
    struct fixture raw = fixture();
    word(raw.data + raw.size, (uint32_t)((SETS + 4 - CONTENT) / 4)); /* duplicate relocation */
    raw.size += 4;
    word(raw.data + 44, (uint32_t)(raw.size - RELOC));
    assert(px_bch_environment_decode(raw.data, raw.size, 0, &e) != PX_BCH_MODEL_OK);
    /* Zero light sets and a light with a LUT sampler pointer are valid. */
    f = fixture();
    word(f.data + SCENE + 0x10, 0);
    word(f.data + sun + 76, 1);
    assert(px_bch_environment_decode(f.data, f.size, 0, &e) == PX_BCH_MODEL_OK);
    assert(e.set_count == 0 && e.lights[1].has_distance_sampler && !e.lights[1].has_angle_sampler);
    px_bch_environment_free(&e);
}
int main(void)
{
    test_decode();
    test_rejections();
    puts("BCH environment light and scene tests passed");
    return 0;
}
