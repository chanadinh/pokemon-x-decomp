#include "call_wild_battle.h"

uint32_t px_call_wild_battle(const uint32_t *args, const struct px_cwb_api *api)
{
    /* LDR followed by UXTH, before the context accessor can change anything. */
    const uint32_t script_word = args[1];
    const uint32_t argument = script_word & UINT32_C(0xffff);
    struct px_cwb_context *const context = api->get_context(api->user);
    const uint32_t field_value = api->read_context_word_0c(api->user, context);

    api->field_helper(api->user, field_value, argument, UINT32_C(0));
    api->context_helper(api->user, context);
    return UINT32_C(0);
}
