#include "battle_immunity.h"
#include "../battle_modifier/battle_modifier.h"

static void *user(const struct px_battle_immunity_api *api)
{
    return api->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_immunity_api *api)
{
    return api->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_immunity_api *api)
{
    return px_battle_event_get(key, api->base->events);
}

static uint32_t block(const struct px_battle_immunity_api *api)
{
    return px_battle_event_rewrite(0x41, 1, api->base->events);
}

static uint32_t move(const struct px_battle_immunity_api *api)
{
    return get(0x12, api) & UINT32_C(0xffff);
}

uint32_t px_battle_absorbs(uint32_t context, uint32_t id, uint32_t type, const struct px_battle_immunity_api *api)
{
    (void)context;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x04, api) != id || get(0x03, api) == id || get(0x16, api) != type) return 0;
    const uint32_t mv = move(api);
    if (api->move_kind(user(api), mv) == 2) {
        const uint32_t count = api->stat_change_count(user(api), mv);
        if (count != 0) {
            /* A status move that only raises stats is not absorbed. */
            uint32_t i = 0;
            for (; i < count; ++i) {
                api->stat_change(user(api), mv, i, api->scratch);
                if ((int32_t)m->load32(m->user, api->scratch) < 0) break;
            }
            if (i == count) return 0;
        }
    }
    return block(api);
}

/* "It doesn't affect" (message 0xd2 or another id) for this battler,
 * optionally inside the ability popup and with record flag 0x800000. */
static void report(uint32_t context, uint32_t id, uint32_t message, int popup, int flag,
                   const struct px_battle_immunity_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    if (popup) api->ability_popup(user(api), context, 1, id);
    const uint32_t record = api->action_record(user(api), context, 3, id);
    if (flag) m->store32(m->user, record, m->load32(m->user, record) | UINT32_C(0x800000));
    api->message(user(api), record + 4, 2, message);
    api->message_arg(user(api), record + 4, id);
    api->commit_actions(user(api), context, record);
    if (popup) api->ability_popup(user(api), context, 2, id);
}

#define HEALER(name, type) \
    PX_BATTLE_IMMUNITY_HANDLER(name) \
    { \
        (void)handler, (void)work; \
        if (px_battle_absorbs(context, id, type, api) != 0) api->heal_fraction(user(api), context, id, 4); \
    }
#define BOOSTER(name, type, stat) \
    PX_BATTLE_IMMUNITY_HANDLER(name) \
    { \
        (void)handler, (void)work; \
        if (px_battle_absorbs(context, id, type, api) != 0) api->raise_stat(user(api), context, id, stat, 1); \
    }
HEALER(px_battle_water_absorb, 10)
HEALER(px_battle_volt_absorb, 12)
HEALER(px_battle_dry_skin_absorb, 10)
BOOSTER(px_battle_motor_drive, 12, 5)
BOOSTER(px_battle_lightning_rod, 12, 3)
BOOSTER(px_battle_storm_drain, 10, 3)

PX_BATTLE_IMMUNITY_HANDLER(px_battle_flash_fire)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (px_battle_absorbs(context, id, 9, api) == 0) return;
    api->ability_popup(user(api), context, 1, id);
    const uint32_t pokemon = px_battle_pokemon_by_id(context, id, api->base);
    if (px_battle_pokemon_state(pokemon, 0xd, m) != 0) {
        /* Already active: only the message. */
        const uint32_t record = api->action_record(user(api), context, 3, id);
        api->message(user(api), record + 4, 2, 0xd2);
        api->message_arg(user(api), record + 4, id);
        api->commit_actions(user(api), context, record);
    } else {
        const uint32_t record = api->action_record(user(api), context, 3, id);
        api->message(user(api), record + 4, 2, 0x1ab);  /* literal 108c54d8 */
        api->message_arg(user(api), record + 4, id);
        api->commit_actions(user(api), context, record);
        /* Record kind 0x17 sets state bit 13 for the battler. */
        const uint32_t state = api->action_record(user(api), context, 0x17, id);
        m->store8(m->user, state + 5, (uint8_t)id);
        m->store8(m->user, state + 4, 0xd);
        api->commit_actions(user(api), context, state);
    }
    api->ability_popup(user(api), context, 2, id);
}

PX_BATTLE_IMMUNITY_HANDLER(px_battle_wonder_guard)
{
    (void)handler, (void)work;
    if (get(0x04, api) != id || get(0x03, api) == id) return;
    (void)px_battle_pokemon_by_id(context, id, api->base);  /* looked up, result unused */
    const uint32_t mv = move(api);
    /* Damaging moves other than Struggle (165) that are not super effective. */
    if (api->damaging_move(user(api), mv) == 0 || mv == 0xa5) return;
    if ((get(0x39, api) & UINT32_C(0xff)) > 7) return;
    if (block(api) != 0) report(context, id, 0xd2, 1, 0, api);
}

PX_BATTLE_IMMUNITY_HANDLER(px_battle_soundproof)
{
    (void)handler, (void)work;
    if (get(0x04, api) != id || api->move_flag(user(api), move(api), 8) == 0) return;
    if (block(api) != 0) report(context, id, 0xd2, 1, 0, api);
}

PX_BATTLE_IMMUNITY_HANDLER(px_battle_oblivious)
{
    (void)handler, (void)work;
    if (get(0x04, api) != id) return;
    /* Captivate (445) and Taunt (269). */
    const uint32_t mv = move(api);
    if (mv != 0x1bd && mv != 0x10d) return;
    if (block(api) != 0) report(context, id, 0xd2, 1, 0, api);
}

PX_BATTLE_IMMUNITY_HANDLER(px_battle_sticky_hold)
{
    (void)handler, (void)work;
    if (get(0x04, api) != id) return;
    /* Trick (271) and Switcheroo (415). */
    const uint32_t mv = move(api);
    if (mv != 0x10f && mv != 0x19f) return;
    if (block(api) != 0) report(context, id, 0xd2, 0, 1, api);
}

PX_BATTLE_IMMUNITY_HANDLER(px_battle_telepathy)
{
    (void)handler, (void)work;
    if (get(0x04, api) != id) return;
    const uint32_t attacker = get(0x03, api) & UINT32_C(0xff);
    if (api->is_ally(user(api), id, attacker) == 0 || id == attacker) return;
    if (api->damaging_move(user(api), move(api)) == 0) return;
    if (block(api) != 0) report(context, id, 0x1d5, 0, 1, api);  /* literal 108be188 */
}

PX_BATTLE_IMMUNITY_HANDLER(px_battle_overcoat)
{
    (void)handler, (void)work;
    if (get(0x04, api) != id || api->powder_move(user(api), move(api)) == 0) return;
    if (block(api) != 0) report(context, id, 0xd2, 1, 0, api);
}

PX_BATTLE_IMMUNITY_HANDLER(px_battle_safety_goggles)
{
    (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x04, api) != id || api->powder_move(user(api), move(api)) == 0 || block(api) == 0) return;
    const uint32_t item = m->load16(m->user, handler + UINT32_C(0x38));
    const uint32_t mv = get(0x12, api);
    const uint32_t record = api->action_record(user(api), context, 3, id);
    api->message(user(api), record + 4, 2, 0x4ae);  /* literal 108abb44 */
    api->message_arg(user(api), record + 4, id);
    api->message_arg(user(api), record + 4, item);
    api->message_arg(user(api), record + 4, mv);
    api->commit_actions(user(api), context, record);
}

/* Magic Coat and Magic Bounce: a reflectable move is blocked and returned. */
static void reflect_move(uint32_t context, uint32_t id, const struct px_battle_immunity_api *api)
{
    if (get(0x04, api) != id || get(0x4f, api) != 0) return;
    if (api->reflection_blocked(user(api), px_battle_pokemon_by_id(context, id, api->base)) != 0) return;
    if (api->move_flag(user(api), move(api), 4) == 0 || block(api) == 0) return;
    api->reflect(user(api), context, id, get(0x03, api) & UINT32_C(0xff));
    px_battle_event_rewrite(0x53, 1, api->base->events);
}

PX_BATTLE_IMMUNITY_HANDLER(px_battle_magic_bounce) { (void)handler, (void)work; reflect_move(context, id, api); }
PX_BATTLE_IMMUNITY_HANDLER(px_battle_magic_coat) { (void)handler, (void)work; reflect_move(context, id, api); }

PX_BATTLE_IMMUNITY_HANDLER(px_battle_telekinesis)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x03, api) != id) return;
    const uint32_t target = px_battle_pokemon_by_id(context, get(0x04, api) & UINT32_C(0xff), api->base);
    const uint32_t species = m->load16(m->user, target + UINT32_C(0xc));
    /* Diglett (50), Dugtrio (51), Mega Gengar and two statuses make it fail. */
    if (species == 0x32 || species == 0x33 || (species == 0x5e && api->mega_gengar(user(api), target) != 0) ||
        api->has_status(user(api), target, 0x1f) != 0 || api->has_status(user(api), target, 0x15) != 0)
        block(api);
}
