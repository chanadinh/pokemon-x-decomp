#include "battle_items_3.h"

static const struct px_battle_ability_api *base(const struct px_battle_items_3_api *api)
{
    return api->modifier->base;
}

static uint32_t get(uint32_t key, const struct px_battle_items_3_api *api)
{
    return px_battle_event_get(key, base(api)->events);
}

static void boost(uint32_t factor, const struct px_battle_items_3_api *api)
{
    px_battle_event_multiply(0x32, factor, base(api)->events);
}

/* The holder's move of one type: the item factor. */
static void type_item(uint32_t handler, uint32_t id, uint32_t type, const struct px_battle_items_3_api *api)
{
    if (get(0x03, api) != id) return;
    if (get(0x16, api) != type) return;
    boost(px_battle_item_factor(handler, api->modifier), api);
}

#define TYPE_ITEM(name, type) \
    PX_BATTLE_ITEMS_3_HANDLER(name) { (void)context, (void)work; type_item(handler, id, type, api); }
TYPE_ITEM(px_battle_silk_scarf, 0)
TYPE_ITEM(px_battle_black_belt, 1)
TYPE_ITEM(px_battle_sharp_beak, 2)
TYPE_ITEM(px_battle_poison_barb, 3)
TYPE_ITEM(px_battle_soft_sand, 4)
TYPE_ITEM(px_battle_hard_stone, 5)
TYPE_ITEM(px_battle_rock_incense, 5)
TYPE_ITEM(px_battle_silver_powder, 6)
TYPE_ITEM(px_battle_spell_tag, 7)
TYPE_ITEM(px_battle_metal_coat, 8)
TYPE_ITEM(px_battle_charcoal, 9)
TYPE_ITEM(px_battle_mystic_water, 0xa)
TYPE_ITEM(px_battle_sea_incense, 0xa)
TYPE_ITEM(px_battle_wave_incense, 0xa)
TYPE_ITEM(px_battle_miracle_seed, 0xb)
TYPE_ITEM(px_battle_rose_incense, 0xb)
TYPE_ITEM(px_battle_magnet, 0xc)
TYPE_ITEM(px_battle_twisted_spoon, 0xd)
TYPE_ITEM(px_battle_odd_incense, 0xd)
TYPE_ITEM(px_battle_never_melt_ice, 0xe)
TYPE_ITEM(px_battle_dragon_fang, 0xf)
TYPE_ITEM(px_battle_black_glasses, 0x10)
TYPE_ITEM(px_battle_pixie_plate, 0x11)

PX_BATTLE_ITEMS_3_HANDLER(px_battle_poison_barb_use)
{
    (void)handler, (void)work;
    const struct px_pokemon_api *m = base(api)->events->memory;
    void *user = base(api)->events->user;
    if (get(0x02, api) != id) return;
    /* Kind 0xb for the low byte of key 0x03: +4 status 5 (poison), +8 its
     * condition word, +0xf the holder. */
    const uint32_t record = api->action_record(user, context, 0xb, get(0x03, api) & UINT32_C(0xff));
    m->store8(m->user, record + 0xf, (uint8_t)id);
    m->store16(m->user, record + 4, 5);
    m->store32(m->user, record + 8, api->condition(user, 5));
    api->commit_actions(user, context, record);
}

/* The orbs: the holder must be the species, and the move's type (low byte)
 * Dragon or the orb's second type. */
static int orb_fits(uint32_t context, uint32_t id, uint32_t species, uint32_t second,
                    const struct px_battle_items_3_api *api)
{
    const struct px_pokemon_api *m = base(api)->events->memory;
    if (get(0x03, api) != id) return 0;
    if (m->load16(m->user, px_battle_pokemon_by_id(context, id, base(api)) + UINT32_C(0xc)) != species) return 0;
    const uint32_t type = get(0x16, api) & UINT32_C(0xff);
    return type == 0xf || type == second;
}

PX_BATTLE_ITEMS_3_HANDLER(px_battle_adamant_orb)
{
    (void)work;
    if (orb_fits(context, id, 0x1e3, 8, api)) boost(px_battle_item_factor(handler, api->modifier), api);
}

PX_BATTLE_ITEMS_3_HANDLER(px_battle_lustrous_orb)
{
    (void)work;
    if (orb_fits(context, id, 0x1e4, 0xa, api)) boost(px_battle_item_factor(handler, api->modifier), api);
}

PX_BATTLE_ITEMS_3_HANDLER(px_battle_griseous_orb)
{
    (void)handler, (void)work;
    /* A fixed x1.2 (literal 108a6a4c), not the item factor. */
    if (orb_fits(context, id, 0x1e7, 7, api)) boost(0x1333, api);
}

/* The holder's move of one category (the low half of key 0x12). */
static void category_item(uint32_t handler, uint32_t id, uint32_t category, const struct px_battle_items_3_api *api)
{
    if (get(0x03, api) != id) return;
    if (base(api)->move_category(base(api)->events->user, get(0x12, api) & UINT32_C(0xffff)) != category) return;
    boost(px_battle_item_factor(handler, api->modifier), api);
}

PX_BATTLE_ITEMS_3_HANDLER(px_battle_muscle_band) { (void)context, (void)work; category_item(handler, id, 1, api); }
PX_BATTLE_ITEMS_3_HANDLER(px_battle_wise_glasses) { (void)context, (void)work; category_item(handler, id, 2, api); }
