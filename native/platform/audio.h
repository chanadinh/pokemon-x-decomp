#ifndef PX_AUDIO_H
#define PX_AUDIO_H
#include <stddef.h>
#include <stdint.h>

/* The audio service in place of the 3DS DSP: 24 voices mixed to stereo at the
 * DSP's output rate (about 32728 Hz, 3dbrew), each playing one channel of
 * PCM8, PCM16 or DSP-ADPCM at its own sample rate with a left/right volume
 * and an optional loop. Voices are resampled linearly (an implementation
 * choice; the DSP firmware's interpolation is not reproduced). The mixer is
 * plain C and deterministic; audio_sdl.c sends it to the host device.
 *
 * DSP-ADPCM: 8-byte frames of a header byte (coefficient pair index in the
 * high nibble, scale exponent in the low) and 14 signed 4-bit samples;
 * sample = (nibble * scale * 2048 + 1024 + c1 * hist1 + c2 * hist2) >> 11,
 * clamped to 16 bits (the GameCube/3DS DSP-ADPCM format). */
enum { PX_AUDIO_VOICES = 24, PX_DSP_RATE = 32728 };
enum px_audio_encoding { PX_PCM8 = 0, PX_PCM16 = 1, PX_DSP_ADPCM = 2 };
struct px_adpcm_context { uint16_t predictor_scale; int16_t hist1, hist2; };
struct px_audio_wave {
    enum px_audio_encoding encoding;
    const uint8_t *data; size_t size; /* PCM16 little-endian */
    uint32_t samples, rate;
    int loop; uint32_t loop_start;    /* loop to loop_start after the last sample */
    int16_t coefs[16];                /* ADPCM */
    struct px_adpcm_context context, loop_context;
};
/* Decode a whole channel to 16-bit samples (out holds wave->samples). */
int px_audio_decode(const struct px_audio_wave *wave, int16_t *out);
struct px_audio_voice {
    int active;
    int16_t *pcm; uint32_t samples, rate, loop_start; int loop; /* decoded, owned */
    double position;   /* in source samples */
    float volume[2];
};
struct px_audio {
    struct px_audio_voice voices[PX_AUDIO_VOICES];
    uint32_t rate;     /* output rate */
    uint64_t mixed;    /* output frames so far */
};
void px_audio_init(struct px_audio *audio, uint32_t rate);
void px_audio_free(struct px_audio *audio);
/* Start a voice (decodes the wave); 0 on a bad voice number or wave. */
int px_audio_play(struct px_audio *audio, unsigned voice, const struct px_audio_wave *wave, float left, float right);
void px_audio_stop(struct px_audio *audio, unsigned voice);
/* Mix `frames` stereo frames (interleaved left, right) and advance. */
void px_audio_mix(struct px_audio *audio, int16_t *out, size_t frames);

/* BCWAV (CWAV) after 3dbrew's description: header with the INFO and DATA
 * block references; INFO holds the encoding, loop flag, sample rate, loop
 * start and end, and a reference table of channel infos (sample offset
 * within DATA, DSP-ADPCM coefficients and contexts). Gives one wave per
 * channel (at most 2). Not yet checked against the game's sound files. */
enum px_cwav_result { PX_CWAV_OK = 0, PX_CWAV_TRUNCATED, PX_CWAV_INVALID, PX_CWAV_UNSUPPORTED };
int px_cwav_decode(const uint8_t *data, size_t size, struct px_audio_wave waves[2], uint32_t *channels);
const char *px_cwav_result_string(int result);
#endif
