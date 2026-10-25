#ifndef PX_CLOCK_H
#define PX_CLOCK_H
#include <stdint.h>

/* Timing: the 3DS system tick (svcGetSystemTick counts at 268,111,856 Hz,
 * the ARM11 clock, per 3dbrew) and the 60 Hz frame the game steps on. A
 * fixed clock (scripted and captured runs) advances exactly one frame per
 * rendered frame, so every run repeats; milliseconds are then frame x 1000 /
 * 60, rounded down, as the viewer has always counted. A live clock follows
 * the host's monotonic time (now_ns), and paces frames to 60 Hz. */
enum { PX_SYSTEM_TICKS_PER_SECOND = 268111856, PX_FRAMES_PER_SECOND = 60 };
struct px_clock {
    int fixed;
    uint64_t frame, start_ns, frame_start_ns;
    uint64_t (*now_ns)(void *user);
    void *user;
};
void px_clock_init(struct px_clock *clock, int fixed, uint64_t (*now_ns)(void *), void *user);
uint64_t px_clock_ms(const struct px_clock *clock);
uint64_t px_clock_ticks(const struct px_clock *clock);
/* After a frame: count it; returns how many nanoseconds to wait (live
 * clocks) so that frames start 1/60 s apart, never more than one period. */
uint64_t px_clock_end_frame(struct px_clock *clock);
/* Ticks between two host times (for code that measures intervals). */
uint64_t px_clock_ns_to_ticks(uint64_t ns);
#endif
