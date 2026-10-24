#include "light.h"

#include <math.h>
#include <stddef.h>

uint16_t px_light_register_channel(uint8_t light, uint8_t material)
{
    return (uint16_t)((((uint32_t)light * 2 + 1) * ((uint32_t)material * 2 + 1)) >> 10);
}

static int normalize3(const float in[3], float out[3])
{
    for (unsigned i = 0; i < 3; ++i) if (!isfinite(in[i])) return 0;
    const float length = sqrtf(in[0] * in[0] + in[1] * in[1] + in[2] * in[2]);
    if (!(length > 0) || !isfinite(length)) return 0;
    for (unsigned i = 0; i < 3; ++i) out[i] = in[i] / length;
    return 1;
}
static uint8_t channel(float value)
{
    if (value < 0.0f) value = 0.0f;
    if (value > 1.0f) value = 1.0f;
    return (uint8_t)(value * 255);
}

int px_light_primary(const struct px_light_setup *setup, const float normal[3], uint8_t out[4])
{
    float n[3], sum[3] = {0, 0, 0};
    if (setup == NULL || normal == NULL || out == NULL || setup->count > 8 || !normalize3(normal, n))
        return 0;
    for (unsigned c = 0; c < 3; ++c) if (setup->global_ambient[c] > 1023) return 0;
    for (uint32_t i = 0; i < setup->count; ++i) {
        const struct px_light_source *light = setup->lights + i;
        float l[3];
        if (!normalize3(light->vector, l)) return 0;
        for (unsigned c = 0; c < 3; ++c)
            if (light->ambient[c] > 1023 || light->diffuse[c] > 1023) return 0;
        float dot = l[0] * n[0] + l[1] * n[1] + l[2] * n[2];
        dot = light->two_sided ? fabsf(dot) : (dot > 0 ? dot : 0);
        for (unsigned c = 0; c < 3; ++c)
            sum[c] += (float)light->diffuse[c] / 255.f * dot + (float)light->ambient[c] / 255.f;
    }
    for (unsigned c = 0; c < 3; ++c) out[c] = channel(sum[c] + (float)setup->global_ambient[c] / 255.f);
    out[3] = 255;
    return 1;
}
