#include "battle_abilities_5.h"

static const struct px_battle_immunity_api *im(const struct px_battle_abilities_5_api *api)
{
    return api->immunity;
}

static void *user(const struct px_battle_abilities_5_api *api)
{
    return im(api)->base->events->user;
}

static const struct px_pokemon_api *memory(const struct px_battle_abilities_5_api *api)
{
    return im(api)->base->events->memory;
}

static uint32_t get(uint32_t key, const struct px_battle_abilities_5_api *api)
{
    return px_battle_event_get(key, im(api)->base->events);
}

static uint32_t rewrite(uint32_t key, uint32_t value, const struct px_battle_abilities_5_api *api)
{
    return px_battle_event_rewrite(key, value, im(api)->base->events);
}

static uint32_t pokemon(uint32_t context, uint32_t id, const struct px_battle_abilities_5_api *api)
{
    return px_battle_pokemon_by_id(context, id, im(api)->base);
}

static void mark(uint32_t record, const struct px_battle_abilities_5_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    m->store32(m->user, record, m->load32(m->user, record) | UINT32_C(0x800000));
}

/* Event 0x2b: draw a move of `type` aimed elsewhere to this battler (key 0x04).
 * The work word is 1 when the target changed to it. */
static void redirect(uint32_t context, uint32_t id, uint32_t work, uint32_t type, const struct px_battle_abilities_5_api *api)
{
    const struct px_pokemon_api *m = memory(api);
    uint32_t drawn = 0;
    const uint32_t attacker = get(0x03, api) & UINT32_C(0xff);
    if (attacker != id && get(0x16, api) == type) {
        const uint32_t move = get(0x12, api) & UINT32_C(0xffff);
        /* Not when redirection is blocked, for fixed-target moves, or Sky Drop (507). */
        if (api->redirect_blocked(user(api), context, attacker, id, move) == 0 && api->fixed_target(user(api), move) == 0 &&
            move != 0x1fb) {
            const uint32_t previous = get(0x04, api) & UINT32_C(0xff);
            drawn = rewrite(0x04, id, api) != 0 && previous != id;
        }
    }
    m->store32(m->user, work, drawn);
}

PX_BATTLE_ABILITIES_5_HANDLER(px_battle_lightning_rod_redirect) { (void)handler; redirect(context, id, work, 0xc, api); }
PX_BATTLE_ABILITIES_5_HANDLER(px_battle_storm_drain_redirect) { (void)handler; redirect(context, id, work, 0xa, api); }

PX_BATTLE_ABILITIES_5_HANDLER(px_battle_redirect_message)
{
    (void)handler;
    const struct px_pokemon_api *m = memory(api);
    if (m->load32(m->user, work) == 0) return;
    /* Announce once if this battler is among the targets (keys 0x06 ...). */
    const uint32_t count = get(0x05, api);
    for (uint32_t i = 0; i < count; ++i) {
        if (get((i + 6) & UINT32_C(0xff), api) != id) continue;
        const uint32_t record = im(api)->action_record(user(api), context, 3, id);
        mark(record, api);
        im(api)->message(user(api), record + 4, 2, 0x1c0);
        im(api)->message_arg(user(api), record + 4, id);
        im(api)->commit_actions(user(api), context, record);
        break;
    }
    m->store32(m->user, work, 0);
}

PX_BATTLE_ABILITIES_5_HANDLER(px_battle_sap_sipper)
{
    (void)handler, (void)work;
    /* Grass moves are absorbed and raise Attack by one. */
    if (px_battle_absorbs(context, id, 0xb, im(api)) != 0) im(api)->raise_stat(user(api), context, id, 1, 1);
}

PX_BATTLE_ABILITIES_5_HANDLER(px_battle_flash_fire_leave)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x02, api) != id || api->battle_flag(user(api), pokemon(context, id, api), 0xd) == 0) return;
    /* Kind 0x18 clears state bit +4 (13, Flash Fire active) of battler +5. */
    const uint32_t record = im(api)->action_record(user(api), context, 0x18, id);
    m->store8(m->user, record + 5, (uint8_t)id);
    m->store8(m->user, record + 4, 0xd);
    im(api)->commit_actions(user(api), context, record);
}

PX_BATTLE_ABILITIES_5_HANDLER(px_battle_flash_fire_power)
{
    (void)handler, (void)work;
    /* Active Flash Fire: Fire moves x1.5. */
    if (get(0x03, api) != id || api->battle_flag(user(api), pokemon(context, id, api), 0xd) == 0) return;
    if (get(0x16, api) == 9) px_battle_event_multiply(0x36, 0x1800, im(api)->base->events);
}

PX_BATTLE_ABILITIES_5_HANDLER(px_battle_bulletproof)
{
    (void)handler, (void)work;
    if (get(0x04, api) != id || api->ball_move(user(api), get(0x12, api) & UINT32_C(0xffff)) == 0) return;
    if (rewrite(0x41, 1, api) == 0) return;
    /* "It doesn't affect" (0xd2) inside the popup. */
    im(api)->ability_popup(user(api), context, 1, id);
    const uint32_t record = im(api)->action_record(user(api), context, 3, id);
    im(api)->message(user(api), record + 4, 2, 0xd2);
    im(api)->message_arg(user(api), record + 4, id);
    im(api)->commit_actions(user(api), context, record);
    im(api)->ability_popup(user(api), context, 2, id);
}

PX_BATTLE_ABILITIES_5_HANDLER(px_battle_damp_check)
{
    (void)handler, (void)context, (void)id;
    const struct px_pokemon_api *m = memory(api);
    /* Work: +0 the move failed, +4 the move. Explosion (153) and
     * Self-Destruct (120) fail with reason 0x13 unless one is already set. */
    const uint32_t move = get(0x12, api) & UINT32_C(0xffff);
    m->store32(m->user, work, 0);
    if (move != 0x99 && move != 0x78) return;
    if ((get(0x23, api) & UINT32_C(0xff)) != 0) return;
    m->store32(m->user, work, rewrite(0x23, 0x13, api));
    m->store32(m->user, work + 4, move);
}

PX_BATTLE_ABILITIES_5_HANDLER(px_battle_damp_message)
{
    (void)handler;
    const struct px_pokemon_api *m = memory(api);
    if (m->load32(m->user, work) == 0) return;
    const uint32_t mover = get(0x02, api) & UINT32_C(0xff);
    const uint32_t record = im(api)->action_record(user(api), context, 3, id);
    mark(record, api);
    im(api)->message(user(api), record + 4, 2, 0x38f);  /* literal 108c4490 */
    im(api)->message_arg(user(api), record + 4, mover);
    im(api)->message_arg(user(api), record + 4, m->load32(m->user, work + 4));
    im(api)->commit_actions(user(api), context, record);
    m->store32(m->user, work, 0);
}

PX_BATTLE_ABILITIES_5_HANDLER(px_battle_magic_bounce_reflect)
{
    (void)handler, (void)work;
    if (api->bounce_blocked(user(api), context, id) != 0) return;
    const uint32_t mover = get(0x02, api) & UINT32_C(0xff);
    if (im(api)->is_ally(user(api), mover, id) != 0) return;
    /* A reflectable (move flag 4) move of kind 11 fails with reason 0x19 and is reflected. */
    const uint32_t move = get(0x12, api) & UINT32_C(0xffff);
    if (im(api)->move_kind(user(api), move) != 0xb || im(api)->move_flag(user(api), move, 4) == 0) return;
    if (rewrite(0x23, 0x19, api) == 0) return;
    im(api)->reflect(user(api), context, id, mover);
}

PX_BATTLE_ABILITIES_5_HANDLER(px_battle_magic_bounce_message)
{
    (void)handler, (void)work;
    if (get(0x02, api) != id || rewrite(0x53, 1, api) == 0) return;
    im(api)->ability_popup(user(api), context, 1, id);
    /* The battler key is read again inside the popup. */
    if (get(0x02, api) == id && im(api)->reflection_blocked(user(api), pokemon(context, id, api)) == 0) {
        const uint32_t record = im(api)->action_record(user(api), context, 3, id);
        im(api)->message(user(api), record + 4, 2, 0x2fc);
        im(api)->message_arg(user(api), record + 4, id);
        im(api)->message_arg(user(api), record + 4, get(0x12, api));
        im(api)->commit_actions(user(api), context, record);
    }
    im(api)->ability_popup(user(api), context, 2, id);
}

PX_BATTLE_ABILITIES_5_HANDLER(px_battle_sweet_veil)
{
    (void)handler, (void)context, (void)work;
    /* Sleep is prevented for this battler and its allies. */
    if (im(api)->is_ally(user(api), id, get(0x02, api) & UINT32_C(0xff)) != 0) rewrite(0x42, 1, api);
}

PX_BATTLE_ABILITIES_5_HANDLER(px_battle_insomnia_block)
{
    (void)handler, (void)context, (void)work;
    if (get(0x02, api) == id) rewrite(0x42, 1, api);
}

PX_BATTLE_ABILITIES_5_HANDLER(px_battle_flower_veil)
{
    (void)handler, (void)work;
    /* Grass-type allies (and itself) are protected. */
    const uint32_t target = get(0x02, api) & UINT32_C(0xff);
    if (im(api)->is_ally(user(api), id, target) == 0) return;
    if (api->has_type(user(api), pokemon(context, target, api), 0xb) != 0) rewrite(0x42, 1, api);
}

PX_BATTLE_ABILITIES_5_HANDLER(px_battle_liquid_ooze)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = memory(api);
    if (get(0x04, api) != id) return;
    /* The drain (key 0x21) is cancelled and dealt to the attacker instead. */
    const uint32_t drained = get(0x21, api) & UINT32_C(0xffff);
    rewrite(0x21, 0, api);
    if (drained == 0) return;
    const uint32_t record = im(api)->action_record(user(api), context, 6, id);
    mark(record, api);
    m->store16(m->user, record + 4, (uint16_t)drained);
    m->store8(m->user, record + 6, (uint8_t)get(0x03, api));
    im(api)->message(user(api), record + 0xc, 2, 0x1c9);  /* literal 108bf228 */
    im(api)->message_arg(user(api), record + 0xc, m->load8(m->user, record + 6));
    im(api)->commit_actions(user(api), context, record);
}
