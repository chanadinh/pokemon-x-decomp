#ifndef PX_AUDIO_SDL_H
#define PX_AUDIO_SDL_H
#include "audio.h"

/* The host audio device for the mixer: stereo 16-bit at the DSP rate (SDL
 * converts to the device's own). The device thread mixes under a lock;
 * px_audio_device_lock / unlock guard changes to voices. */
struct px_audio_device;
struct px_audio_device *px_audio_device_open(struct px_audio *audio, char *error, size_t error_size);
void px_audio_device_lock(struct px_audio_device *device);
void px_audio_device_unlock(struct px_audio_device *device);
const char *px_audio_device_driver(void);
uint32_t px_audio_device_rate(const struct px_audio_device *device);
void px_audio_device_close(struct px_audio_device *device);
#endif
