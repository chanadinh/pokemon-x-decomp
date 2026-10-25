/* The audio service: PCM8/PCM16/DSP-ADPCM decoding, mixing, resampling,
 * looping, clamping, and a synthetic BCWAV. */
#include "audio.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void put16(uint8_t *p, uint32_t v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); }
static void put32(uint8_t *p, uint32_t v) { for (int k = 0; k < 4; ++k) p[k] = (uint8_t)(v >> (8 * k)); }

int main(void)
{
    int16_t pcm[64];
    /* PCM8 and PCM16. */
    const uint8_t eight[3] = {0x01, 0xff, 0x80};
    struct px_audio_wave w = {PX_PCM8, eight, 3, 3, 32728, 0, 0, {0}, {0, 0, 0}, {0, 0, 0}};
    assert(px_audio_decode(&w, pcm) && pcm[0] == 256 && pcm[1] == -256 && pcm[2] == -32768);
    const uint8_t sixteen[4] = {0x34, 0x12, 0xff, 0xff};
    w = (struct px_audio_wave){PX_PCM16, sixteen, 4, 2, 32728, 0, 0, {0}, {0, 0, 0}, {0, 0, 0}};
    assert(px_audio_decode(&w, pcm) && pcm[0] == 0x1234 && pcm[1] == -1);
    w.samples = 3;
    assert(!px_audio_decode(&w, pcm)); /* not enough data */
    /* DSP-ADPCM: scale 1 and no prediction gives the nibbles; coefficient
     * 2048 (1.0) on hist1 accumulates them. */
    uint8_t frame[16] = {0x00, 0x12, 0x3f, 0x80, 0, 0, 0, 0, 0x10, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
    w = (struct px_audio_wave){PX_DSP_ADPCM, frame, 16, 28, 32728, 0, 0, {0}, {0, 0, 0}, {0, 0, 0}};
    w.coefs[2] = 2048; /* pair 1: c1 = 1.0, c2 = 0 */
    assert(px_audio_decode(&w, pcm));
    assert(pcm[0] == 1 && pcm[1] == 2 && pcm[2] == 3 && pcm[3] == -1 && pcm[4] == -8 && pcm[5] == 0);
    assert(pcm[13] == 0);
    for (int k = 0; k < 14; ++k) assert(pcm[14 + k] == k + 1); /* pair 1, +1 each sample */
    frame[0] = 0x0f; /* scale 2^15 saturates */
    w.samples = 3;
    assert(px_audio_decode(&w, pcm) && pcm[0] == 32767 && pcm[2] == 32767);
    /* Mixing at the output rate copies a voice; volumes pan it. */
    struct px_audio a;
    px_audio_init(&a, 1000);
    const uint8_t ramp[8] = {0, 0, 100, 0, 200, 0, 0x2c, 1}; /* 0, 100, 200, 300 */
    w = (struct px_audio_wave){PX_PCM16, ramp, 8, 4, 1000, 0, 0, {0}, {0, 0, 0}, {0, 0, 0}};
    assert(px_audio_play(&a, 0, &w, 1.0f, 0.5f) && !px_audio_play(&a, 24, &w, 1, 1));
    int16_t out[16];
    px_audio_mix(&a, out, 5);
    assert(out[0] == 0 && out[2] == 100 && out[3] == 50 && out[6] == 300 && out[8] == 0 && !a.voices[0].active);
    /* Half the output rate: linear interpolation between samples. */
    w.rate = 500;
    px_audio_play(&a, 0, &w, 1, 1);
    px_audio_mix(&a, out, 4);
    assert(out[0] == 0 && out[2] == 50 && out[4] == 100 && out[6] == 150);
    /* A loop returns to its start. */
    w.rate = 1000; w.loop = 1; w.loop_start = 2;
    px_audio_play(&a, 1, &w, 1, 1);
    px_audio_stop(&a, 0);
    px_audio_mix(&a, out, 7);
    const int16_t want[7] = {0, 100, 200, 300, 200, 300, 200};
    for (int k = 0; k < 7; ++k) assert(out[2 * k] == want[k]);
    w.loop_start = 4;
    assert(!px_audio_play(&a, 2, &w, 1, 1)); /* loop start beyond the end */
    /* Two loud voices clamp. */
    const uint8_t loud[2] = {0x00, 0x60};
    w = (struct px_audio_wave){PX_PCM16, loud, 2, 1, 1000, 0, 0, {0}, {0, 0, 0}, {0, 0, 0}};
    px_audio_free(&a);
    px_audio_play(&a, 0, &w, 1, 1); px_audio_play(&a, 1, &w, 1, 1);
    px_audio_mix(&a, out, 1);
    assert(out[0] == 32767 && a.mixed == 1 + 4 + 5 + 7);
    px_audio_free(&a);
    /* A synthetic BCWAV: one DSP-ADPCM channel, looping. */
    static uint8_t cwav[0x200];
    memcpy(cwav, "CWAV", 4); put16(cwav + 4, 0xfeff); put16(cwav + 6, 0x40); put32(cwav + 8, 0x02010000);
    put32(cwav + 0x0c, sizeof cwav); put16(cwav + 0x10, 2);
    put16(cwav + 0x14, 0x7000); put32(cwav + 0x18, 0x40); put32(cwav + 0x1c, 0xc0);
    put16(cwav + 0x20, 0x7001); put32(cwav + 0x24, 0x100); put32(cwav + 0x28, 0x100);
    uint8_t *info = cwav + 0x40;
    memcpy(info, "INFO", 4); put32(info + 4, 0xc0); info[8] = 2; info[9] = 1;
    put32(info + 0x0c, 32000); put32(info + 0x10, 14); put32(info + 0x14, 28);
    uint8_t *table = info + 0x1c;
    put32(table, 1); put16(table + 4, 0x7100); put32(table + 8, 0x0c);
    uint8_t *ch = table + 0x0c;
    put16(ch, 0x1f00); put32(ch + 4, 0); put16(ch + 8, 0x0300); put32(ch + 0x0c, 0x14);
    uint8_t *adpcm = ch + 0x14;
    put16(adpcm + 2 * 2, 2048);
    put16(adpcm + 0x20, 0x10); put16(adpcm + 0x22, 5);
    uint8_t *data = cwav + 0x100;
    memcpy(data, "DATA", 4); put32(data + 4, 0x100);
    memcpy(data + 8, frame, 16);
    data[8] = 0x10; /* pair 1, scale 1 */
    struct px_audio_wave waves[2];
    uint32_t channels;
    assert(px_cwav_decode(cwav, sizeof cwav, waves, &channels) == PX_CWAV_OK && channels == 1);
    assert(waves[0].encoding == PX_DSP_ADPCM && waves[0].rate == 32000 && waves[0].samples == 28 &&
           waves[0].loop && waves[0].loop_start == 14 && waves[0].coefs[2] == 2048 && waves[0].context.hist1 == 5);
    assert(px_audio_decode(&waves[0], pcm) && pcm[0] == 6 && pcm[1] == 8); /* hist1 5 + 1, then + 2 */
    assert(px_cwav_decode(cwav, 0x30, waves, &channels) == PX_CWAV_TRUNCATED);
    cwav[0] = 'X';
    assert(px_cwav_decode(cwav, sizeof cwav, waves, &channels) == PX_CWAV_INVALID);
    cwav[0] = 'C'; info[8] = 3;
    assert(px_cwav_decode(cwav, sizeof cwav, waves, &channels) == PX_CWAV_UNSUPPORTED);
    assert(!strcmp(px_cwav_result_string(PX_CWAV_OK), "success"));
    puts("audio tests passed");
    return 0;
}
