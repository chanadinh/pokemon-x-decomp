#include "battle_task.h"

static uint32_t request32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static void or_state_flags(uint32_t task, uint32_t mask,
                            const struct px_battle_task_api *api)
{
    const uint32_t state = api->load32(api->user, task + UINT32_C(0x20));
    const uint32_t flags = api->load32(api->user, state + UINT32_C(0xa8));
    api->store32(api->user, state + UINT32_C(0xa8), flags | mask);
}

uint32_t px_create_battle_task(uint32_t manager, const struct px_battle_task_api *api)
{
    const uint32_t first_heap = api->heap_for_id(api->user, UINT32_C(8));
    const uint32_t task = api->allocate(api->user, PX_BATTLE_TASK_SIZE, first_heap);
    if (task != 0) {
        /* 0x0012b27c falls through into heap_for_id(8) on EACH invocation. */
        const uint32_t second_heap = api->heap_for_id(api->user, UINT32_C(8));
        /* Base initializer 0x00109498. Vtable values are original logical tokens. */
        api->store32(api->user, task + UINT32_C(4), UINT32_C(0));
        api->store32(api->user, task, UINT32_C(0x0058ea24));
        api->store32(api->user, task + UINT32_C(8), UINT32_C(0));
        api->store32(api->user, task + UINT32_C(0x10), UINT32_C(0));
        api->store32(api->user, task + UINT32_C(0x0c), second_heap);
        api->store32(api->user, task + UINT32_C(0x14), UINT32_C(0));
        api->store32(api->user, task + UINT32_C(0x18), UINT32_C(0));
        api->store32(api->user, task + UINT32_C(0x1c), UINT32_C(0));
        api->store32(api->user, task + UINT32_C(0x20), UINT32_C(0));
        api->store32(api->user, task, UINT32_C(0x0059911c));
        api->store8(api->user, task + UINT32_C(0x28), UINT8_C(0));
        api->store8(api->user, task + UINT32_C(0x29), UINT8_C(0));
        api->store8(api->user, task + UINT32_C(0x2a), UINT8_C(0));
        api->store8(api->user, task + UINT32_C(0x2b), UINT8_C(0));
        api->store32(api->user, task + UINT32_C(0x258), UINT32_C(0));
        api->store8(api->user, task + UINT32_C(0x262), UINT8_C(0));
        api->store32(api->user, task + UINT32_C(0x264), UINT32_C(0));
        api->store32(api->user, task + UINT32_C(0x268), UINT32_MAX);
    }
    /* 0x00108cf0 registers even a zero result; it has no failure guard. */
    const uint32_t old_head = api->load32(api->user, manager);
    api->store32(api->user, task + UINT32_C(4), old_head);
    api->store32(api->user, manager, task);
    return task;
}

uint32_t px_decode_optional_key(uint32_t value)
{
    /* 0x003e0aa8: high-byte 0xff takes a disabled-assert path returning zero. */
    return (value >> 24) == UINT32_C(0xff) ? UINT32_C(0) :
           ((value >> 16) & UINT32_C(0xff));
}

void px_consume_wild_request(uint32_t task, uint32_t field,
                            const uint8_t *request, uint32_t mode,
                            const struct px_battle_task_api *api)
{
    const uint32_t heap = api->heap_for_id(api->user, UINT32_C(8));
    const uint32_t primary = request[0x8c];
    const uint32_t secondary = request[0x8d];
    const uint32_t saved_field_context = api->load32(api->user, field + UINT32_C(0x1c));
    api->init_scene(api->user, task + UINT32_C(0x230), field, primary, secondary, 0);

    /* Original checks this old value only for a disabled assertion. */
    (void)api->load32(api->user, task + UINT32_C(0x258));
    uint32_t aux = api->allocate(api->user, PX_BATTLE_AUX_SIZE, heap);
    if (aux != 0) {
        aux = api->construct_aux(api->user, aux, heap);
    }
    api->store32(api->user, task + UINT32_C(0x258), aux);
    api->populate_aux(api->user, request, heap, aux);
    aux = api->load32(api->user, task + UINT32_C(0x258));
    const uint32_t extra = api->load8(api->user, aux + UINT32_C(0x18)) > 1 ? 4 : 0;
    const uint32_t config = api->select_config(api->user, request);
    /* The original reloads task+0x258 after the config selector returns. */
    aux = api->load32(api->user, task + UINT32_C(0x258));
    const uint32_t inline_state = task + UINT32_C(0x2c);
    api->init_state(api->user, inline_state, field, aux,
                     task + UINT32_C(0x230), extra, config, heap);
    api->store32(api->user, task + UINT32_C(0x20), inline_state);

    const uint32_t location_object = api->load32(api->user, saved_field_context + UINT32_C(0x28));
    const uint32_t location = api->load16(api->user, location_object + UINT32_C(0x0c));
    if (location == UINT32_C(0x103) || location == UINT32_C(0x11e)) {
        /* Full effect of 0x001ceeb0. */
        api->store8(api->user, inline_state + UINT32_C(0xb2), UINT8_C(1));
    }

    switch (request[0x8f]) {
    case 1: or_state_flags(task, UINT32_C(0x80), api); break;
    case 2: or_state_flags(task, UINT32_C(4), api); break;
    case 3: or_state_flags(task, UINT32_C(0x200), api); break;
    default: break;
    }
    if ((request32(request + 0xa4) & UINT32_C(2)) != 0) {
        api->store8(api->user, task + UINT32_C(0x2a), UINT8_C(1));
        or_state_flags(task, UINT32_C(1), api);
        const uint32_t captured_state = api->load32(api->user, task + UINT32_C(0x20));
        const uint32_t key = px_decode_optional_key(request32(request + 0x80));
        uint8_t decoded[10];
        api->decode_environment(api->user, primary, key, decoded);
        api->store8(api->user, captured_state + UINT32_C(0x0a), decoded[2]);
    }
    /* Unlike the earlier location access, this reloads the field context. */
    const uint32_t current_field_context = api->load32(api->user, field + UINT32_C(0x1c));
    const uint32_t category = api->load16(api->user, current_field_context + UINT32_C(0x128)) & UINT32_C(0xff);
    if (category == 3 || category == 11) {
        or_state_flags(task, UINT32_C(0x4000), api);
    }
    if (category >= 4 && category <= 7) {
        or_state_flags(task, UINT32_C(0x8000), api);
    }
    api->store32(api->user, task + UINT32_C(0x1c), mode);
    api->store8(api->user, task + UINT32_C(0x28), UINT8_C(1));
    if ((mode & UINT32_C(1)) != 0) {
        api->store8(api->user, task + UINT32_C(0x29), UINT8_C(1));
    }
}

uint32_t px_submit_wild_request(uint32_t manager, uint32_t field,
                                const uint8_t *request, uint32_t mode,
                                const struct px_battle_task_api *api)
{
    const uint32_t task = px_create_battle_task(manager, api);
    px_consume_wild_request(task, field, request, mode, api);
    return task;
}
