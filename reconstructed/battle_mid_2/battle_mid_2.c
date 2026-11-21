#include "battle_mid_2.h"

static uint32_t load8(uint32_t at, const struct px_battle_mid_2_api *api)
{
    return api->memory->load8(api->memory->user, at);
}

static uint32_t load32(uint32_t at, const struct px_battle_mid_2_api *api)
{
    return api->memory->load32(api->memory->user, at);
}

static void store8(uint32_t at, uint32_t value, const struct px_battle_mid_2_api *api)
{
    api->memory->store8(api->memory->user, at, (uint8_t)value);
}

static uint32_t call(uint32_t function, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e,
                     const struct px_battle_mid_2_api *api)
{
    return api->call(api->user, function, a, b, c, d, e);
}

/* The first half shared by the init functions: the page is armed (+0x24 clear, +0xd9 = 3, the handle at +0xec
 * started with 10801574, state 0 and next state 3 with the value at +0xf4..+0xf6) and `first` runs on it. */
static void begin(uint32_t object, uint32_t first, uint32_t value, const struct px_battle_mid_2_api *api)
{
    store8(object + 0x24, 0, api);
    store8(object + 0xd9, 3, api);
    call(0x10801574, load32(object + 0xec, api), 0, 0, 0, 0, api);
    store8(object + 0xf4, 0, api);
    store8(object + 0xf5, 3, api);
    store8(object + 0xf6, value, api);
    call(first, object, 0, 0, 0, 0, api);
}

/* Variant a: +0xd9 becomes 2 before the second call. */
static void init_a(uint32_t object, uint32_t first, uint32_t value, const struct px_battle_mid_2_api *api)
{
    begin(object, first, value, api);
    store8(object + 0xd9, 2, api);
    call(0x1080157c, load32(object + 0xec, api), 0, 0, 0, 0, api);
    store8(object + 0xf4, 1, api);
    store8(object + 0xf5, 2, api);
}

/* Variant b: +0xd9 becomes 2 last. */
static void init_b(uint32_t object, uint32_t first, uint32_t value, const struct px_battle_mid_2_api *api)
{
    begin(object, first, value, api);
    call(0x1080157c, load32(object + 0xec, api), 0, 0, 0, 0, api);
    store8(object + 0xf4, 1, api);
    store8(object + 0xf5, 2, api);
    store8(object + 0xd9, 2, api);
}

void px_battle_mid_10892810(uint32_t object, const struct px_battle_mid_2_api *api) { init_a(object, 0x1085168c, 0x41, api); }
void px_battle_mid_1089289c(uint32_t object, const struct px_battle_mid_2_api *api) { init_a(object, 0x1085168c, 0x41, api); }
void px_battle_mid_10893568(uint32_t object, const struct px_battle_mid_2_api *api) { init_b(object, 0x108511c8, 0x41, api); }
void px_battle_mid_108935f4(uint32_t object, const struct px_battle_mid_2_api *api) { init_b(object, 0x108511c8, 0x41, api); }
void px_battle_mid_1089498c(uint32_t object, const struct px_battle_mid_2_api *api) { init_b(object, 0x10850cf4, 0x5f, api); }
void px_battle_mid_10894a18(uint32_t object, const struct px_battle_mid_2_api *api) { init_b(object, 0x10850cf4, 0x5f, api); }

/* Teardown: release the handle at `offset` when set (call 1080085c with it and the argument still in r1, then the tail call). */
static uint32_t teardown(uint32_t object, uint32_t offset, uint32_t argument, const struct px_battle_mid_2_api *api)
{
    const uint32_t handle = load32(object + offset, api);
    if (handle == 0) return 0;
    call(0x1080085c, handle, argument, 0, 0, 0, api);
    return call(0x108568ec, argument, 0, 0, 0, 0, api);
}

uint32_t px_battle_mid_10892874(uint32_t object, uint32_t argument, const struct px_battle_mid_2_api *api) { return teardown(object, 0xe4, argument, api); }
uint32_t px_battle_mid_108935cc(uint32_t object, uint32_t argument, const struct px_battle_mid_2_api *api) { return teardown(object, 0xe4, argument, api); }
uint32_t px_battle_mid_108949f0(uint32_t object, uint32_t argument, const struct px_battle_mid_2_api *api) { return teardown(object, 0xe4, argument, api); }
uint32_t px_battle_mid_108954a0(uint32_t object, uint32_t argument, const struct px_battle_mid_2_api *api) { return teardown(object, 0xf4, argument, api); }
uint32_t px_battle_mid_10896744(uint32_t object, uint32_t argument, const struct px_battle_mid_2_api *api) { return teardown(object, 0xf4, argument, api); }
uint32_t px_battle_mid_10897960(uint32_t object, uint32_t argument, const struct px_battle_mid_2_api *api) { return teardown(object, 0xd8, argument, api); }

/* The first half and the handle's reset: its state (10801c54) is saved, the handle's 1080085c runs with 0, the
 * handle is reset (10801c1c) and 1080085c runs again with the saved state (a tail call). */
static uint32_t init_reset(uint32_t object, uint32_t first, uint32_t value, const struct px_battle_mid_2_api *api)
{
    begin(object, first, value, api);
    const uint32_t saved = call(0x10801c54, load32(object + 0xe4, api), 0, 0, 0, 0, api);
    call(0x1080085c, load32(object + 0xe4, api), 0, 0, 0, 0, api);
    call(0x10801c1c, load32(object + 0xe4, api), 0, 0, 0, 0, api);
    return call(0x1080085c, load32(object + 0xe4, api), saved, 0, 0, 0, api);
}

uint32_t px_battle_mid_10892c34(uint32_t object, const struct px_battle_mid_2_api *api) { return init_reset(object, 0x1085168c, 0x41, api); }
uint32_t px_battle_mid_108939f0(uint32_t object, const struct px_battle_mid_2_api *api) { return init_reset(object, 0x108511c8, 0x41, api); }
uint32_t px_battle_mid_10894e14(uint32_t object, const struct px_battle_mid_2_api *api) { return init_reset(object, 0x10850cf4, 0x5f, api); }

/* Per-frame update: 10801bdc is asked first while the frame value (+0xf6) is below `limit`; the next state
 * (+0xf5) then starts the handle at +0xec (10801574 for 0, 1080157c for 1 and 2); state 2 ends the page: +0x24 is set,
 * after the abort helper 10800394 when +0xd9 is 3 (it returns). */
static void update(uint32_t object, uint32_t child, uint32_t limit, const struct px_battle_mid_2_api *api)
{
    call(0x108019f4, load32(object + 0xec, api), 0, 0, 0, 0, api);
    const uint32_t state = load8(object + 0xf4, api);
    if (state == 0) {
        call(child, object, 0, 0, 0, 0, api);
    } else if (state == 1) {
        int advance = 1;
        if (load8(object + 0xf6, api) < limit)
            advance = call(0x10801bdc, object + 4, load32(object + 0xe4, api), 0, 0, 0, api) != 0;
        if (advance) {
            call(0x108015ac, object + 4, load32(object + 0xe4, api), load8(object + 0xf6, api), 0, 0, api);
            const uint32_t next = load8(object + 0xf5, api);
            if (next == 0) call(0x10801574, load32(object + 0xec, api), 0, 0, 0, 0, api);
            else if (next == 1 || next == 2) call(0x1080157c, load32(object + 0xec, api), 0, 0, 0, 0, api);
            store8(object + 0xf4, next, api);
            store8(object + 0xf5, 3, api);
        }
    }
    if (load8(object + 0xf4, api) == 2) {
        if (load8(object + 0xd9, api) == 3) call(0x10800394, 0, 0, 0, 0, 0, api);
        store8(object + 0x24, 1, api);
    }
}

void px_battle_mid_10892ca4(uint32_t object, const struct px_battle_mid_2_api *api) { update(object, 0x10892900, 0x40, api); }
void px_battle_mid_10893a60(uint32_t object, const struct px_battle_mid_2_api *api) { update(object, 0x10893658, 0x40, api); }
void px_battle_mid_10894e84(uint32_t object, const struct px_battle_mid_2_api *api) { update(object, 0x10894a7c, 0x5e, api); }

/* The pages with state at +0x104: armed with +0xe3 = 8, the handle at +0xfc started, the value 0x88 and `first` run;
 * then the second call, state 1 and next state 2, +0xe3 = 7 and the value `limit`, and 108015b4 on the object's +4
 * area (with +0xf4, the value, 1 and a stacked 1); the tail call 108568ec carries `result`. */
static uint32_t late_page(uint32_t object, uint32_t first, uint32_t limit, uint32_t result, const struct px_battle_mid_2_api *api)
{
    store8(object + 0x24, 0, api);
    store8(object + 0xe3, 8, api);
    call(0x10801574, load32(object + 0xfc, api), 0, 0, 0, 0, api);
    store8(object + 0x104, 0, api);
    store8(object + 0x105, 3, api);
    store8(object + 0x106, 0x88, api);
    call(first, object, 0, 0, 0, 0, api);
    call(0x1080157c, load32(object + 0xfc, api), 0, 0, 0, 0, api);
    store8(object + 0x104, 1, api);
    store8(object + 0x105, 2, api);
    store8(object + 0xe3, 7, api);
    store8(object + 0x106, limit, api);
    call(0x108015b4, object + 4, load32(object + 0xf4, api), limit, 1, 1, api);
    return call(0x108568ec, result, 0, 0, 0, 0, api);
}

uint32_t px_battle_mid_10895410(uint32_t object, const struct px_battle_mid_2_api *api) { return late_page(object, 0x10850998, 0xf, 1, api); }
uint32_t px_battle_mid_108954c8(uint32_t object, const struct px_battle_mid_2_api *api) { return late_page(object, 0x10850998, 0x10, 0, api); }
uint32_t px_battle_mid_1089676c(uint32_t object, const struct px_battle_mid_2_api *api) { return late_page(object, 0x1084fe30, 0x10, 0, api); }

uint32_t px_battle_mid_10897bf4(uint32_t object, const struct px_battle_mid_2_api *api)
{
    store8(object + 0x24, 0, api);
    store8(object + 0xd5, 2, api);
    call(0x10801574, load32(object + 0xe0, api), 0, 0, 0, 0, api);
    store8(object + 0xe8, 0, api);
    store8(object + 0xe9, 3, api);
    store8(object + 0xea, 0x10, api);
    call(0x108015ec, load32(object + 0xe4, api), 0, 0, 0, 0, api);
    call(0x108568ec, 1, 0, 0, 0, 0, api);
    return call(0x10853d20, load32(object + 0xd8, api), 0xf, 0, 0, 0, api);
}
