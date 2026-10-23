#include "wild_battle_setup.h"

#include <stddef.h>
#include <string.h>

static void put32(uint8_t *p, uint32_t value)
{
    p[0] = (uint8_t)value;
    p[1] = (uint8_t)(value >> 8);
    p[2] = (uint8_t)(value >> 16);
    p[3] = (uint8_t)(value >> 24);
}

static void set_record_modes(uint8_t *workspace)
{
    put32(workspace + 0x70, UINT32_C(1));
    put32(workspace + 0x74, UINT32_C(1));
    put32(workspace + 0x6c, UINT32_C(1));
}

uint8_t *px_init_wild_workspace(uint8_t *workspace)
{
    static const size_t zero_words[] = {0x70, 0x74, 0x88, 0x9c, 0xa4};
    static const size_t zero_bytes[] = {
        0x84, 0x85, 0x86, 0x8c, 0x8d, 0x8f,
        0x90, 0x91, 0x92, 0x93, 0x95, 0x96, 0x97, 0x98, 0xa0
    };
    put32(workspace + 0x6c, UINT32_C(1));
    workspace[0x8e] = UINT8_C(0x40);
    for (size_t i = 0; i < sizeof zero_words / sizeof zero_words[0]; ++i) {
        put32(workspace + zero_words[i], UINT32_C(0));
    }
    for (size_t i = 0; i < sizeof zero_bytes / sizeof zero_bytes[0]; ++i) {
        workspace[zero_bytes[i]] = UINT8_C(0);
    }
    /* The original clears only this prefix; padding and other holes survive. */
    memset(workspace, 0, 0x6c);
    return workspace;
}

static void patch_record_fields(uint8_t *workspace, const uint8_t *record)
{
    const uint32_t flags = (uint32_t)record[6] | ((uint32_t)record[7] << 8);
    workspace[0x8c] = record[8];
    workspace[0x8d] = record[9];
    workspace[0x8e] = record[10];
    workspace[0x8f] = (uint8_t)(flags >> 7); /* Original STRB discards bit 15. */
    workspace[0] = record[0];
    workspace[1] = record[1];
    workspace[2] = record[4];
    workspace[3] = record[5];
    workspace[4] = record[3];
    workspace[5] = record[2];
    workspace[6] = (uint8_t)(flags & UINT32_C(3));
    workspace[7] = (uint8_t)((flags >> 4) & UINT32_C(7));
    workspace[0x10] = (uint8_t)((flags >> 2) & UINT32_C(3));
}

void px_patch_wild_record(uint8_t *workspace, const uint8_t *record)
{
    set_record_modes(workspace);
    patch_record_fields(workspace, record);
}

uint32_t px_prepare_wild_battle(uint32_t field, uint32_t index, uint32_t mode,
                               uint8_t *workspace, const struct px_wild_api *api)
{
    (void)px_init_wild_workspace(workspace);
    uint32_t optional = UINT32_C(0);
    const uint32_t optional_object = api->load32(api->user, field + UINT32_C(0x48));
    if (optional_object != 0) {
        const uint32_t nested = api->load32(api->user, optional_object + UINT32_C(0x158));
        const uint32_t leaf = api->load32(api->user, nested + UINT32_C(0x0c));
        /* Main accessor 0x004a4000 is exactly one word load at +0x148. */
        optional = api->load32(api->user, leaf + UINT32_C(0x148));
    }
    const uint32_t byte_object = api->load32(api->user, field + UINT32_C(0x2c));
    const uint32_t context_byte = api->load8(api->user, byte_object + UINT32_C(0x34));
    api->setup(api->user, workspace, field, UINT32_C(1), optional,
               UINT32_C(0xff), context_byte);
    set_record_modes(workspace);
    const uint8_t *const record = api->get_record(api->user, index);
    patch_record_fields(workspace, record);
    const uint32_t manager = api->load32(api->user, field + UINT32_C(0x18));
    return api->submit(api->user, manager, field, workspace, mode);
}

uint32_t px_finish_wild_script(uint32_t context, const struct px_wild_api *api)
{
    const uint32_t object = api->load32(api->user, context + UINT32_C(0x34));
    api->store32(api->user, object + UINT32_C(0x58), UINT32_C(12));
    return UINT32_C(0);
}
