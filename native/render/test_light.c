#include "light.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

/* Expected bytes were computed separately with numpy float32 arithmetic
 * following the documented operation order, then truncated. */
static struct px_light_source source(uint16_t ar, uint16_t ag, uint16_t ab, uint16_t dr, uint16_t dg,
                                     uint16_t db, float x, float y, float z, uint32_t two_sided)
{
    struct px_light_source s = {{ar, ag, ab}, {dr, dg, db}, {x, y, z}, two_sided};
    return s;
}
static void expect(const struct px_light_setup *setup, float x, float y, float z,
                   unsigned r, unsigned g, unsigned b)
{
    const float normal[3] = {x, y, z};
    uint8_t out[4];
    assert(px_light_primary(setup, normal, out));
    assert(out[0] == r && out[1] == g && out[2] == b && out[3] == 255);
}
static void test_register_product(void)
{
    /* ((2l+1)(2m+1)) >> 10, the runtime's packing loop. */
    assert(px_light_register_channel(0, 255) == 0);
    assert(px_light_register_channel(255, 255) == 255);
    assert(px_light_register_channel(79, 255) == 79);
    assert(px_light_register_channel(128, 128) == 64);
    assert(px_light_register_channel(255, 0) == 0);
    assert(px_light_register_channel(1, 1) == 0);
    for (unsigned l = 0; l < 256; ++l) assert(px_light_register_channel((uint8_t)l, 255) == l);
}
static void test_primary(void)
{
    struct px_light_setup setup;
    memset(&setup, 0, sizeof setup);
    setup.count = 1;
    setup.lights[0] = source(50, 0, 1023, 100, 255, 0, 0, 2, 0, 0); /* vector normalized on use */
    expect(&setup, 0, 1, 0, 150, 255, 255);   /* 1023/255 clamps */
    expect(&setup, 0, -1, 0, 50, 0, 255);     /* back face: dot clamps to zero */
    expect(&setup, 0, 1, 1, 120, 180, 255);   /* normal renormalized: dot 0.7071 */
    setup.lights[0].two_sided = 1;
    expect(&setup, 0, -1, 0, 150, 255, 255);  /* |dot| */
    /* The selected field's day and night lights on a floor and a wall. */
    setup.lights[0] = source(199, 208, 226, 79, 104, 98, 0, 0.997f, -0.08f, 0);
    expect(&setup, 0, 1, 0, 255, 255, 255);
    setup.lights[0] = source(61, 71, 107, 89, 158, 112, 0, 0.997f, -0.08f, 0);
    expect(&setup, 0, 1, 0, 149, 228, 218);
    expect(&setup, 1, 0, 0, 61, 71, 107);
    /* No lights: global ambient only. Two lights sum before the clamp. */
    setup.count = 0;
    setup.global_ambient[0] = 10; setup.global_ambient[1] = 20; setup.global_ambient[2] = 30;
    expect(&setup, 0, 0, 1, 10, 20, 30);
    setup.count = 2;
    setup.global_ambient[0] = 1; setup.global_ambient[1] = 2; setup.global_ambient[2] = 3;
    setup.lights[0] = source(10, 10, 10, 30, 0, 0, 1, 0, 0, 0);
    setup.lights[1] = source(5, 5, 5, 0, 40, 0, 0, 1, 0, 0);
    expect(&setup, 1, 1, 0, 37, 45, 18);
}
static void test_invalid(void)
{
    struct px_light_setup setup;
    memset(&setup, 0, sizeof setup);
    const float normal[3] = {0, 1, 0}, zero[3] = {0, 0, 0}, nan_normal[3] = {NAN, 1, 0};
    uint8_t out[4];
    assert(!px_light_primary(NULL, normal, out));
    assert(!px_light_primary(&setup, NULL, out));
    assert(!px_light_primary(&setup, normal, NULL));
    assert(!px_light_primary(&setup, zero, out));
    assert(!px_light_primary(&setup, nan_normal, out));
    setup.count = 9;
    assert(!px_light_primary(&setup, normal, out));
    setup.count = 1;
    setup.lights[0] = source(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    assert(!px_light_primary(&setup, normal, out)); /* zero light vector */
    setup.lights[0].vector[1] = INFINITY;
    assert(!px_light_primary(&setup, normal, out));
    setup.lights[0].vector[1] = 1;
    setup.lights[0].diffuse[2] = 1024;
    assert(!px_light_primary(&setup, normal, out));
    setup.lights[0].diffuse[2] = 0;
    setup.global_ambient[1] = 1024;
    assert(!px_light_primary(&setup, normal, out));
    setup.global_ambient[1] = 0;
    assert(px_light_primary(&setup, normal, out));
}
int main(void)
{
    test_register_product();
    test_primary();
    test_invalid();
    puts("fragment lighting tests passed");
    return 0;
}
