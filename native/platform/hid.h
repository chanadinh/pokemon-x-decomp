#ifndef PX_HID_H
#define PX_HID_H
#include <stdint.h>

/* The input service: the 3DS pad as the game reads it. Button bits follow the
 * HID PAD register layout documented on 3dbrew ("HID Services"); the circle
 * pad is a signed position (about -156..156 on hardware, clamped here); the
 * touch screen reports bottom-screen pixels 0..319 x 0..239 while touched.
 * Each frame the host feeds what is held and what was pressed, and the state
 * gives held / down (pressed this frame) / up (released this frame), as the
 * game's per-frame HID reads do. A press and release inside one frame still
 * counts as down. */
#define PX_KEY_A (UINT32_C(1) << 0)
#define PX_KEY_B (UINT32_C(1) << 1)
#define PX_KEY_SELECT (UINT32_C(1) << 2)
#define PX_KEY_START (UINT32_C(1) << 3)
#define PX_KEY_DRIGHT (UINT32_C(1) << 4)
#define PX_KEY_DLEFT (UINT32_C(1) << 5)
#define PX_KEY_DUP (UINT32_C(1) << 6)
#define PX_KEY_DDOWN (UINT32_C(1) << 7)
#define PX_KEY_R (UINT32_C(1) << 8)
#define PX_KEY_L (UINT32_C(1) << 9)
#define PX_KEY_X (UINT32_C(1) << 10)
#define PX_KEY_Y (UINT32_C(1) << 11)
#define PX_KEY_ZL (UINT32_C(1) << 14)
#define PX_KEY_ZR (UINT32_C(1) << 15)
#define PX_KEY_TOUCH (UINT32_C(1) << 20)
#define PX_KEY_CPAD_RIGHT (UINT32_C(1) << 28)
#define PX_KEY_CPAD_LEFT (UINT32_C(1) << 29)
#define PX_KEY_CPAD_UP (UINT32_C(1) << 30)
#define PX_KEY_CPAD_DOWN (UINT32_C(1) << 31)
#define PX_KEY_DPAD (PX_KEY_DRIGHT | PX_KEY_DLEFT | PX_KEY_DUP | PX_KEY_DDOWN)
#define PX_KEY_CPAD (PX_KEY_CPAD_RIGHT | PX_KEY_CPAD_LEFT | PX_KEY_CPAD_UP | PX_KEY_CPAD_DOWN)
enum { PX_TOUCH_WIDTH = 320, PX_TOUCH_HEIGHT = 240, PX_CPAD_MAX = 156, PX_CPAD_THRESHOLD = 40 };
struct px_hid_state {
    uint32_t held, down, up;
    int16_t cpad_x, cpad_y;    /* right and up positive */
    uint16_t touch_x, touch_y; /* valid while held & PX_KEY_TOUCH */
    uint64_t frame;
};
struct px_hid {
    struct px_hid_state state;
    uint32_t pending_held, pending_pressed;
    int16_t pending_x, pending_y;
    int pending_touch; uint16_t pending_tx, pending_ty;
};
void px_hid_init(struct px_hid *hid);
/* Host feed for the coming frame. */
void px_hid_set_held(struct px_hid *hid, uint32_t keys);  /* replaces what is held */
void px_hid_press(struct px_hid *hid, uint32_t keys);     /* a press seen this frame (held or not) */
void px_hid_circle_pad(struct px_hid *hid, int x, int y); /* clamped to +-PX_CPAD_MAX */
void px_hid_touch(struct px_hid *hid, int touching, int x, int y); /* clamped to the screen */
/* End of frame: the new state (the circle pad beyond PX_CPAD_THRESHOLD also
 * sets its direction bits, as the game's HID reads report). */
const struct px_hid_state *px_hid_update(struct px_hid *hid);

/* A --play script as pad input: U D L R (D-pad), u d l r (D-pad with B held:
 * running), '.' (A), 'x' (B); 's' is the host's save request and no button.
 * px_hid_script_buttons gives the buttons one character presses (0 for an
 * unknown character or 's'). */
uint32_t px_hid_script_buttons(char c);
int px_hid_script_valid(const char *script);
#endif
