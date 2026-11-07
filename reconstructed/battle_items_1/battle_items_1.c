#include "battle_items_1.h"

enum { STACK_KEYS = 4, STACK_VALUES = 0xc4, HANDLER_FLAGS = 0x18, HANDLER_ITEM = 0x38,
       RUNNING = 0x1000000, REENTRY = 0x10000000 };

static void *user(const struct px_battle_items_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_items_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_items_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_items_api *api)
{
    return px_battle_pokemon_by_id(context, id, api->base);
}

static uint32_t value(uint32_t p, uint32_t key, const struct px_battle_items_api *api)
{
    return api->base->pokemon_value(user(api), p, key);
}

uint32_t px_battle_event_find(uint32_t key, uint32_t out, const struct px_battle_event_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    if (key == 0 || key == 1) api->abort(api->user);
    for (uint32_t position = m->load32(m->user, api->stack); position < PX_BATTLE_EVENT_CAPACITY; ++position) {
        const uint32_t existing = m->load16(m->user, api->stack + STACK_KEYS + position * UINT32_C(2));
        if (existing == key) {
            m->store32(m->user, out, m->load32(m->user, api->stack + STACK_VALUES + position * UINT32_C(4)));
            return 1;
        }
        if (existing == 0) return 0;
    }
    return 0;
}

void px_battle_handler_mark_reentry(uint32_t handler, const struct px_battle_event_api *api)
{
    const struct px_pokemon_api *m = api->memory;
    const uint32_t flags = m->load32(m->user, handler + HANDLER_FLAGS);
    if (flags & RUNNING) m->store32(m->user, handler + HANDLER_FLAGS, flags | REENTRY);
}

uint32_t px_battle_listed_condition(uint32_t p, const struct px_battle_items_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    for (uint32_t i = 0; i < 6; ++i) {
        const uint32_t condition = m->load16(m->user, api->cures + i * UINT32_C(2));
        if (condition == 0) return 0;
        /* Out-of-range entries fault, then are read anyway. */
        if (condition >= 0x26) api->base->events->abort(user(api));
        if (m->load32(m->user, p + UINT32_C(0x1c) + condition * UINT32_C(4)) & 7) return condition;
    }
    return 0;
}

uint32_t px_battle_item_pinch(uint32_t context, uint32_t id, uint32_t divisor, const struct px_battle_items_api *api)
{
    const uint32_t p = pokemon(context, id, api);
    /* Gluttony eats pinch berries at half the threshold. */
    if (value(p, 0x11, api) == 0x52 && divisor > 2) divisor >>= 1;
    if (divisor == 0) {
        api->base->events->abort(user(api));
        divisor = 1;
    }
    if (value(p, 0xe, api) <= 1) return 0;
    const uint32_t hp = value(p, 0xd, api);
    return hp <= px_battle_hp_fraction(p, divisor, api->base);
}

/* Asks for the berry: marks the handler and commits a kind-0 record. */
static void request_use(uint32_t handler, uint32_t context, uint32_t id, const struct px_battle_items_api *api)
{
    px_battle_handler_mark_reentry(handler, api->base->events);
    api->ability_popup(user(api), context, 0, id);
}

void px_battle_hp_berry_check(uint32_t handler, uint32_t context, uint32_t id, uint32_t divisor, uint32_t flag,
                              const struct px_battle_items_api *api)
{
    if (get(0x02, api) != id) return;
    if (flag != 0) {
        if (get(0x02, api) != id) return;
        if (!(get(0x2f, api) & 1)) return;
    }
    if (px_battle_item_pinch(context, id, divisor, api) == 0) return;
    request_use(handler, context, id, api);
}

/* Whether the battler has what the berry cures. */
static int needs_cure(uint32_t p, uint32_t cure, const struct px_battle_items_api *api)
{
    if (cure == 0x26) return api->base->pokemon_status(user(api), p) != 0;
    if (cure == 0x27)
        return api->base->pokemon_status(user(api), p) != 0 || api->has_status(user(api), p, 6) != 0;
    if (cure == 0x28) return px_battle_listed_condition(p, api) != 0;
    return api->has_status(user(api), p, cure) != 0;
}

void px_battle_cure_berry_check(uint32_t handler, uint32_t context, uint32_t id, uint32_t cure,
                                const struct px_battle_items_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    /* A frame with use flags must allow it (bit 2). */
    if (px_battle_event_find(0x2f, api->scratch, api->base->events) != 0 && !(m->load32(m->user, api->scratch) & 4))
        return;
    const uint32_t p = pokemon(context, id, api);
    if (!needs_cure(p, cure, api)) return;
    request_use(handler, context, id, api);
}

void px_battle_cure_berry_use(uint32_t handler, uint32_t context, uint32_t id, uint32_t cure,
                              const struct px_battle_items_api *api)
{
    (void)handler;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    const uint32_t p = pokemon(context, id, api);
    if (!needs_cure(p, cure, api)) return;
    /* Kind 0xa: the cure for one target. */
    const uint32_t record = api->action_record(user(api), context, 0xa, id);
    m->store8(m->user, record + 4, (uint8_t)cure);
    m->store8(m->user, record + 0x11, 1);
    m->store8(m->user, record + 5, (uint8_t)id);
    api->commit_actions(user(api), context, record);
}

#define CURE_BERRY(check, use, cure) \
    PX_BATTLE_ITEMS_HANDLER(check) { (void)work; px_battle_cure_berry_check(handler, context, id, cure, api); } \
    PX_BATTLE_ITEMS_HANDLER(use) { (void)work; px_battle_cure_berry_use(handler, context, id, cure, api); }
CURE_BERRY(px_battle_cheri_check, px_battle_cheri_use, 1)
CURE_BERRY(px_battle_chesto_check, px_battle_chesto_use, 2)
CURE_BERRY(px_battle_pecha_check, px_battle_pecha_use, 5)
CURE_BERRY(px_battle_rawst_check, px_battle_rawst_use, 4)
CURE_BERRY(px_battle_aspear_check, px_battle_aspear_use, 3)
CURE_BERRY(px_battle_persim_check, px_battle_persim_use, 6)
CURE_BERRY(px_battle_lum_check, px_battle_lum_use, 0x27)

PX_BATTLE_ITEMS_HANDLER(px_battle_hp_berry_9c)
{
    (void)work;
    if (api->has_status(user(api), pokemon(context, id, api), 0xf) != 0) return;
    px_battle_hp_berry_check(handler, context, id, 2, 1, api);
}

PX_BATTLE_ITEMS_HANDLER(px_battle_hp_berry_5e)
{
    (void)work;
    if (api->has_status(user(api), pokemon(context, id, api), 0xf) != 0) return;
    px_battle_hp_berry_check(handler, context, id, 2, 0, api);
}

PX_BATTLE_ITEMS_HANDLER(px_battle_hp_berry_61)
{
    (void)work;
    /* No key 0x02 test here. */
    if (api->has_status(user(api), pokemon(context, id, api), 0xf) != 0) return;
    if (px_battle_item_pinch(context, id, 2, api) == 0) return;
    request_use(handler, context, id, api);
}

/* Kind 4: restore +4 HP to battler +6, with message 0x392 naming the battler
 * and the item. */
static void restore(uint32_t handler, uint32_t context, uint32_t id, uint32_t record, uint32_t amount,
                    const struct px_battle_items_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store16(m->user, record + 4, (uint16_t)amount);
    api->message(user(api), record + 8, 2, 0x392);  /* literals 108a7a9c, 108a79d0 */
    api->message_arg(user(api), record + 8, id);
    api->message_arg(user(api), record + 8, m->load16(m->user, handler + HANDLER_ITEM));
    api->commit_actions(user(api), context, record);
}

PX_BATTLE_ITEMS_HANDLER(px_battle_oran_use)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    const uint32_t record = api->action_record(user(api), context, 4, id);
    m->store8(m->user, record + 6, (uint8_t)id);
    /* A fixed amount: the item's parameter 3. */
    const uint32_t amount = api->item_param(user(api), m->load16(m->user, handler + HANDLER_ITEM), 3);
    restore(handler, context, id, record, amount, api);
}

PX_BATTLE_ITEMS_HANDLER(px_battle_sitrus_use)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id) return;
    const uint32_t maximum = value(pokemon(context, id, api), 0xe, api);
    const uint32_t percent = api->item_param(user(api), m->load16(m->user, handler + HANDLER_ITEM), 3) & UINT32_C(0xff);
    const uint32_t record = api->action_record(user(api), context, 4, id);
    /* Percent of max HP: the 32-bit product divided by 100 through
     * umull with 0x51eb851f (literal 108a79cc) and a shift of 37. */
    const uint32_t product = maximum * percent;
    m->store8(m->user, record + 6, (uint8_t)id);
    restore(handler, context, id, record, (uint32_t)((uint64_t)product * UINT32_C(0x51eb851f) >> 37), api);
}
