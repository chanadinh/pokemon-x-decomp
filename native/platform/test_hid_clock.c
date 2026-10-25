/* HID frame semantics and the script mapping; fixed and live clocks. */
#include "clock.h"
#include "hid.h"

#include <assert.h>
#include <stdio.h>

static uint64_t fake_now;
static uint64_t now_ns(void *user) { (void)user; return fake_now; }

int main(void)
{
    struct px_hid h;
    px_hid_init(&h);
    px_hid_set_held(&h, PX_KEY_A);
    const struct px_hid_state *s = px_hid_update(&h);
    assert(s->held == PX_KEY_A && s->down == PX_KEY_A && !s->up && s->frame == 1);
    s = px_hid_update(&h);
    assert(s->held == PX_KEY_A && !s->down);
    px_hid_set_held(&h, PX_KEY_B);
    s = px_hid_update(&h);
    assert(s->down == PX_KEY_B && s->up == PX_KEY_A);
    /* A tap within one frame (pressed and released before the read) still counts. */
    px_hid_set_held(&h, 0); px_hid_press(&h, PX_KEY_DUP);
    s = px_hid_update(&h);
    assert(s->down == PX_KEY_DUP && !(s->held & PX_KEY_DUP) && s->up == PX_KEY_B);
    s = px_hid_update(&h);
    assert(!s->down);
    /* The circle pad: clamped, and its direction bits past the threshold. */
    px_hid_circle_pad(&h, 500, -41);
    s = px_hid_update(&h);
    assert(s->cpad_x == PX_CPAD_MAX && s->cpad_y == -41 && (s->held & PX_KEY_CPAD_RIGHT) && (s->held & PX_KEY_CPAD_DOWN));
    px_hid_circle_pad(&h, 0, 40);
    s = px_hid_update(&h);
    assert(!(s->held & PX_KEY_CPAD) && (s->up & PX_KEY_CPAD_RIGHT));
    /* Touch: the bit while touched, coordinates clamped to 320 x 240. */
    px_hid_touch(&h, 1, 400, -3);
    s = px_hid_update(&h);
    assert((s->down & PX_KEY_TOUCH) && s->touch_x == 319 && s->touch_y == 0);
    px_hid_touch(&h, 0, 0, 0);
    s = px_hid_update(&h);
    assert((s->up & PX_KEY_TOUCH) && s->touch_x == 319); /* the last position remains */
    /* Hosts cannot fake the derived bits. */
    px_hid_set_held(&h, PX_KEY_TOUCH | PX_KEY_CPAD_UP | PX_KEY_X);
    s = px_hid_update(&h);
    assert(s->held == PX_KEY_X);
    /* Script characters. */
    assert(px_hid_script_buttons('u') == (PX_KEY_DUP | PX_KEY_B) && px_hid_script_buttons('.') == PX_KEY_A &&
           px_hid_script_buttons('x') == PX_KEY_B && !px_hid_script_buttons('s') && !px_hid_script_buttons('?'));
    assert(px_hid_script_valid("RRdd..xs") && !px_hid_script_valid("R?") && !px_hid_script_valid(NULL));
    /* The fixed clock: one frame per frame, ms as frame * 1000 / 60. */
    struct px_clock c;
    px_clock_init(&c, 1, NULL, NULL);
    for (int i = 0; i < 7; ++i) assert(px_clock_end_frame(&c) == 0);
    assert(px_clock_ms(&c) == 7 * 1000 / 60 && px_clock_ticks(&c) == 7ull * 268111856 / 60);
    /* The live clock paces to 60 Hz. */
    fake_now = 1000;
    px_clock_init(&c, 0, now_ns, NULL);
    fake_now += 5000000; /* 5 ms of work */
    assert(px_clock_end_frame(&c) == 1000000000ull / 60 - 5000000);
    fake_now = c.frame_start_ns + 30000000; /* a slow frame: no wait */
    assert(px_clock_end_frame(&c) == 0);
    assert(px_clock_ms(&c) == (fake_now - 1000) / 1000000);
    assert(px_clock_ns_to_ticks(2000000000ull) == 2ull * 268111856 && px_clock_ns_to_ticks(500000000) == 134055928);
    puts("hid and clock tests passed");
    return 0;
}
