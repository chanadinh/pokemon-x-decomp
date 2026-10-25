#include "audio_sdl.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

struct px_audio_device { SDL_AudioDeviceID id; struct px_audio *audio; SDL_AudioSpec have; };

static void callback(void *user, Uint8 *stream, int length)
{
    struct px_audio_device *d = user;
    px_audio_mix(d->audio, (int16_t *)(void *)stream, (size_t)length / 4);
}

struct px_audio_device *px_audio_device_open(struct px_audio *audio, char *error, size_t error_size)
{
    if (!SDL_WasInit(SDL_INIT_AUDIO) && SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        snprintf(error, error_size, "%s", SDL_GetError()); return NULL;
    }
    struct px_audio_device *d = calloc(1, sizeof *d);
    if (!d) { snprintf(error, error_size, "out of memory"); return NULL; }
    SDL_AudioSpec want;
    SDL_zero(want);
    want.freq = (int)audio->rate; want.format = AUDIO_S16SYS; want.channels = 2; want.samples = 512;
    want.callback = callback; want.userdata = d;
    d->audio = audio;
    d->id = SDL_OpenAudioDevice(NULL, 0, &want, &d->have, 0); /* no changes allowed: SDL converts */
    if (!d->id) { snprintf(error, error_size, "%s", SDL_GetError()); free(d); return NULL; }
    SDL_PauseAudioDevice(d->id, 0);
    return d;
}

void px_audio_device_lock(struct px_audio_device *d) { if (d) SDL_LockAudioDevice(d->id); }
void px_audio_device_unlock(struct px_audio_device *d) { if (d) SDL_UnlockAudioDevice(d->id); }
const char *px_audio_device_driver(void) { const char *n = SDL_GetCurrentAudioDriver(); return n ? n : "none"; }
uint32_t px_audio_device_rate(const struct px_audio_device *d) { return d ? (uint32_t)d->have.freq : 0; }

void px_audio_device_close(struct px_audio_device *d)
{
    if (!d) return;
    SDL_CloseAudioDevice(d->id);
    free(d);
}
