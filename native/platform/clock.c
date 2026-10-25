#include "clock.h"

#include <string.h>

void px_clock_init(struct px_clock *c, int fixed, uint64_t (*now_ns)(void *), void *user)
{
    memset(c, 0, sizeof *c);
    c->fixed = fixed || !now_ns;
    c->now_ns = now_ns; c->user = user;
    if (!c->fixed) c->start_ns = c->frame_start_ns = now_ns(user);
}

static uint64_t elapsed_ns(const struct px_clock *c)
{
    if (c->fixed) return c->frame * UINT64_C(1000000000) / PX_FRAMES_PER_SECOND;
    return c->now_ns(c->user) - c->start_ns;
}

uint64_t px_clock_ms(const struct px_clock *c)
{
    return c->fixed ? c->frame * 1000 / PX_FRAMES_PER_SECOND : elapsed_ns(c) / 1000000;
}

uint64_t px_clock_ns_to_ticks(uint64_t ns)
{
    /* Split so that the product never overflows 64 bits. */
    const uint64_t s = ns / 1000000000, rest = ns % 1000000000;
    return s * PX_SYSTEM_TICKS_PER_SECOND + rest * PX_SYSTEM_TICKS_PER_SECOND / 1000000000;
}

uint64_t px_clock_ticks(const struct px_clock *c)
{
    if (c->fixed) return c->frame * PX_SYSTEM_TICKS_PER_SECOND / PX_FRAMES_PER_SECOND;
    return px_clock_ns_to_ticks(elapsed_ns(c));
}

uint64_t px_clock_end_frame(struct px_clock *c)
{
    ++c->frame;
    if (c->fixed) return 0;
    const uint64_t period = UINT64_C(1000000000) / PX_FRAMES_PER_SECOND, now = c->now_ns(c->user);
    const uint64_t spent = now - c->frame_start_ns;
    const uint64_t wait = spent < period ? period - spent : 0;
    c->frame_start_ns = now + wait;
    return wait;
}
