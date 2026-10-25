#include "hid.h"

#include <string.h>

void px_hid_init(struct px_hid *h) { memset(h, 0, sizeof *h); }

void px_hid_set_held(struct px_hid *h, uint32_t keys) { h->pending_held = keys & ~(uint32_t)(PX_KEY_TOUCH | PX_KEY_CPAD); }
void px_hid_press(struct px_hid *h, uint32_t keys) { h->pending_pressed |= keys & ~(uint32_t)(PX_KEY_TOUCH | PX_KEY_CPAD); }

static int clamp(int v, int lo, int hi) { return v < lo ? lo : v > hi ? hi : v; }

void px_hid_circle_pad(struct px_hid *h, int x, int y)
{
    h->pending_x = (int16_t)clamp(x, -PX_CPAD_MAX, PX_CPAD_MAX);
    h->pending_y = (int16_t)clamp(y, -PX_CPAD_MAX, PX_CPAD_MAX);
}

void px_hid_touch(struct px_hid *h, int touching, int x, int y)
{
    h->pending_touch = touching != 0;
    h->pending_tx = (uint16_t)clamp(x, 0, PX_TOUCH_WIDTH - 1);
    h->pending_ty = (uint16_t)clamp(y, 0, PX_TOUCH_HEIGHT - 1);
}

const struct px_hid_state *px_hid_update(struct px_hid *h)
{
    struct px_hid_state *s = &h->state;
    const uint32_t before = s->held;
    uint32_t held = h->pending_held;
    if (h->pending_x > PX_CPAD_THRESHOLD) held |= PX_KEY_CPAD_RIGHT;
    if (h->pending_x < -PX_CPAD_THRESHOLD) held |= PX_KEY_CPAD_LEFT;
    if (h->pending_y > PX_CPAD_THRESHOLD) held |= PX_KEY_CPAD_UP;
    if (h->pending_y < -PX_CPAD_THRESHOLD) held |= PX_KEY_CPAD_DOWN;
    if (h->pending_touch) held |= PX_KEY_TOUCH;
    s->down = (held & ~before) | (h->pending_pressed & ~before);
    s->up = before & ~held;
    s->held = held;
    s->cpad_x = h->pending_x; s->cpad_y = h->pending_y;
    if (h->pending_touch) { s->touch_x = h->pending_tx; s->touch_y = h->pending_ty; }
    ++s->frame;
    h->pending_pressed = 0;
    return s;
}

uint32_t px_hid_script_buttons(char c)
{
    switch (c) {
    case 'U': return PX_KEY_DUP;
    case 'D': return PX_KEY_DDOWN;
    case 'L': return PX_KEY_DLEFT;
    case 'R': return PX_KEY_DRIGHT;
    case 'u': return PX_KEY_DUP | PX_KEY_B;
    case 'd': return PX_KEY_DDOWN | PX_KEY_B;
    case 'l': return PX_KEY_DLEFT | PX_KEY_B;
    case 'r': return PX_KEY_DRIGHT | PX_KEY_B;
    case '.': return PX_KEY_A;
    case 'x': return PX_KEY_B;
    default: return 0;
    }
}

int px_hid_script_valid(const char *script)
{
    for (const char *p = script; p && *p; ++p)
        if (*p != 's' && !px_hid_script_buttons(*p)) return 0;
    return script != NULL;
}
