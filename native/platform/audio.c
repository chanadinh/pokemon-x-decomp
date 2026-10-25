#include "audio.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

static int16_t clamp16(int32_t v) { return (int16_t)(v < -32768 ? -32768 : v > 32767 ? 32767 : v); }

int px_audio_decode(const struct px_audio_wave *w, int16_t *out)
{
    if (!w || !out || !w->data) return 0;
    switch (w->encoding) {
    case PX_PCM8:
        if (w->size < w->samples) return 0;
        for (uint32_t i = 0; i < w->samples; ++i) out[i] = (int16_t)((int8_t)w->data[i] * 256);
        return 1;
    case PX_PCM16:
        if (w->size / 2 < w->samples) return 0;
        for (uint32_t i = 0; i < w->samples; ++i) out[i] = (int16_t)(w->data[2 * i] | w->data[2 * i + 1] << 8);
        return 1;
    case PX_DSP_ADPCM: {
        const uint32_t frames = (w->samples + 13) / 14;
        if (w->size / 8 < frames) return 0;
        int32_t h1 = w->context.hist1, h2 = w->context.hist2;
        uint32_t n = 0;
        for (uint32_t f = 0; f < frames; ++f) {
            const uint8_t *frame = w->data + 8 * (size_t)f, header = frame[0];
            const int32_t scale = 1 << (header & 15);
            const unsigned pair = (header >> 4) & 7;
            const int32_t c1 = w->coefs[2 * pair], c2 = w->coefs[2 * pair + 1];
            for (unsigned k = 0; k < 14 && n < w->samples; ++k, ++n) {
                const int nibble = (frame[1 + k / 2] >> (k % 2 ? 0 : 4)) & 15;
                const int32_t signed4 = nibble >= 8 ? nibble - 16 : nibble;
                const int32_t s = clamp16((signed4 * scale * 2048 + 1024 + c1 * h1 + c2 * h2) >> 11);
                out[n] = (int16_t)s; h2 = h1; h1 = s;
            }
        }
        return 1;
    }
    }
    return 0;
}

void px_audio_init(struct px_audio *a, uint32_t rate)
{
    memset(a, 0, sizeof *a);
    a->rate = rate ? rate : PX_DSP_RATE;
}

void px_audio_stop(struct px_audio *a, unsigned v)
{
    if (v >= PX_AUDIO_VOICES) return;
    free(a->voices[v].pcm);
    memset(&a->voices[v], 0, sizeof a->voices[v]);
}

void px_audio_free(struct px_audio *a)
{
    for (unsigned v = 0; v < PX_AUDIO_VOICES; ++v) px_audio_stop(a, v);
}

int px_audio_play(struct px_audio *a, unsigned v, const struct px_audio_wave *w, float left, float right)
{
    if (v >= PX_AUDIO_VOICES || !w || !w->samples || !w->rate || (w->loop && w->loop_start >= w->samples)) return 0;
    int16_t *pcm = malloc((size_t)w->samples * sizeof *pcm);
    if (!pcm || !px_audio_decode(w, pcm)) { free(pcm); return 0; }
    px_audio_stop(a, v);
    struct px_audio_voice *voice = &a->voices[v];
    voice->active = 1; voice->pcm = pcm; voice->samples = w->samples; voice->rate = w->rate;
    voice->loop = w->loop; voice->loop_start = w->loop_start;
    voice->volume[0] = left; voice->volume[1] = right;
    return 1;
}

void px_audio_mix(struct px_audio *a, int16_t *out, size_t frames)
{
    for (size_t f = 0; f < frames; ++f) {
        double mix[2] = {0, 0};
        for (unsigned v = 0; v < PX_AUDIO_VOICES; ++v) {
            struct px_audio_voice *s = &a->voices[v];
            if (!s->active) continue;
            const uint32_t i = (uint32_t)s->position;
            const double t = s->position - i;
            const uint32_t j = i + 1 < s->samples ? i + 1 : s->loop ? s->loop_start : i;
            const double sample = s->pcm[i] + (s->pcm[j] - s->pcm[i]) * t;
            mix[0] += sample * s->volume[0]; mix[1] += sample * s->volume[1];
            s->position += (double)s->rate / a->rate;
            if (s->position >= s->samples) {
                if (s->loop) {
                    const double span = s->samples - s->loop_start;
                    s->position = s->loop_start + fmod(s->position - s->samples, span);
                } else { free(s->pcm); memset(s, 0, sizeof *s); }
            }
        }
        out[2 * f] = clamp16((int32_t)lrint(mix[0]));
        out[2 * f + 1] = clamp16((int32_t)lrint(mix[1]));
    }
    a->mixed += frames;
}

/* ------------------------------------------------------------------ CWAV */
static uint16_t u16(const uint8_t *p) { return (uint16_t)(p[0] | p[1] << 8); }
static uint32_t u32(const uint8_t *p) { return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24; }

int px_cwav_decode(const uint8_t *d, size_t size, struct px_audio_wave waves[2], uint32_t *channels)
{
    memset(waves, 0, 2 * sizeof *waves);
    *channels = 0;
    if (size < 0x40) return PX_CWAV_TRUNCATED;
    if (memcmp(d, "CWAV", 4) || u16(d + 4) != 0xfeff) return PX_CWAV_INVALID;
    const uint32_t file_size = u32(d + 0x0c), blocks = u16(d + 0x10);
    if (file_size > size || blocks < 2 || 0x14 + 12 * (size_t)blocks > size) return PX_CWAV_TRUNCATED;
    uint32_t info = 0, data = 0;
    for (uint32_t b = 0; b < blocks; ++b) {
        const uint8_t *ref = d + 0x14 + 12 * b;
        if (u16(ref) == 0x7000) info = u32(ref + 4);
        if (u16(ref) == 0x7001) data = u32(ref + 4);
    }
    if (!info || !data || (uint64_t)info + 0x20 > file_size || (uint64_t)data + 8 > file_size) return PX_CWAV_TRUNCATED;
    if (memcmp(d + info, "INFO", 4) || memcmp(d + data, "DATA", 4)) return PX_CWAV_INVALID;
    const uint8_t *in = d + info;
    const uint8_t encoding = in[8], loop = in[9];
    const uint32_t rate = u32(in + 0x0c), loop_start = u32(in + 0x10), loop_end = u32(in + 0x14);
    if (encoding > 2) return PX_CWAV_UNSUPPORTED;
    const uint8_t *table = in + 0x1c; /* channel info reference table */
    if ((uint64_t)info + 0x20 > file_size) return PX_CWAV_TRUNCATED;
    const uint32_t count = u32(table);
    if (!count || count > 2) return PX_CWAV_UNSUPPORTED;
    if ((uint64_t)(table - d) + 4 + 8 * (uint64_t)count > file_size) return PX_CWAV_TRUNCATED;
    const uint8_t *samples_base = d + data + 8;
    const size_t data_size = file_size - (data + 8);
    for (uint32_t c = 0; c < count; ++c) {
        const uint8_t *ref = table + 4 + 8 * c;
        if (u16(ref) != 0x7100) return PX_CWAV_INVALID;
        const uint64_t at = (uint64_t)(table - d) + u32(ref + 4);
        if (at + 0x14 > file_size) return PX_CWAV_TRUNCATED;
        const uint8_t *ch = d + at;
        if (u16(ch) != 0x1f00) return PX_CWAV_INVALID;
        const uint32_t offset = u32(ch + 4);
        if (offset > data_size) return PX_CWAV_TRUNCATED;
        struct px_audio_wave *w = &waves[c];
        w->encoding = (enum px_audio_encoding)encoding;
        w->data = samples_base + offset; w->size = data_size - offset;
        w->samples = loop_end; w->rate = rate; w->loop = loop != 0; w->loop_start = loop_start;
        if (encoding == PX_DSP_ADPCM) {
            if (u16(ch + 8) != 0x0300) return PX_CWAV_INVALID;
            const uint64_t ad = at + u32(ch + 0x0c);
            if (ad + 0x2e > file_size) return PX_CWAV_TRUNCATED;
            const uint8_t *p = d + ad;
            for (unsigned k = 0; k < 16; ++k) w->coefs[k] = (int16_t)u16(p + 2 * k);
            w->context = (struct px_adpcm_context){u16(p + 0x20), (int16_t)u16(p + 0x22), (int16_t)u16(p + 0x24)};
            w->loop_context = (struct px_adpcm_context){u16(p + 0x26), (int16_t)u16(p + 0x28), (int16_t)u16(p + 0x2a)};
        }
        if (loop && loop_start >= loop_end) return PX_CWAV_INVALID;
    }
    *channels = count;
    return PX_CWAV_OK;
}

const char *px_cwav_result_string(int r)
{
    switch (r) {
    case PX_CWAV_OK: return "success";
    case PX_CWAV_TRUNCATED: return "truncated BCWAV";
    case PX_CWAV_INVALID: return "invalid BCWAV";
    case PX_CWAV_UNSUPPORTED: return "unsupported BCWAV encoding or channel count";
    }
    return "unknown result";
}
