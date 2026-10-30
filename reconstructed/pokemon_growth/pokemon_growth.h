#ifndef PX_POKEMON_GROWTH_H
#define PX_POKEMON_GROWTH_H

#include "../pokemon_constructor/pokemon_constructor.h"

enum { PX_GROWTH_THRESHOLDS = 101, PX_GROWTH_PAYLOAD_SIZE = 0x194,
       PX_GROWTH_RATE_SELECTOR = 0x17 };

struct px_growth_api {
    const struct px_pokemon_api *memory;
    void *user;
    void (*read_member)(void *user, uint32_t archive, uint32_t index,
                        uint32_t destination); /* 00138214, external */
};

/* Analyst names; addresses are logical ARM32 addresses. A growth record has
 * vtable +0, rate u32 +4 and 101 inline u32 thresholds at +8. Records and
 * cache slots must be 4-byte aligned, valid ordinary memory. */

/* 0017c5b0: copy the 404-byte threshold payload through the word-aligned
 * 00144f34 transfer, then the full rate word. The vtable is unchanged. */
void px_copy_growth_resource(uint32_t source, uint32_t destination,
                              const struct px_pokemon_api *memory);
/* Cache: pointer table u32 +4, capacity u8 +8, cursor u8 +9. Rates are
 * compared as full words. Store requires a positive capacity on a miss. */
uint32_t px_find_cached_growth_resource(uint32_t cache, uint32_t rate,
                                        uint32_t destination, const struct px_pokemon_api *memory); /* 00175fec */
void px_store_cached_growth_resource(uint32_t cache, uint32_t source,
                                      const struct px_pokemon_api *memory); /* 00174cac */
/* 00174dd8: reads archive member `rate` into record+8 only when the stored
 * rate word differs. No range check is applied. Archive cell: 005e52b0. */
void px_load_growth_resource(uint32_t record, uint32_t rate, const struct px_growth_api *api);
/* 00167db0: selects species/form into the personal descriptor at 005d17a8,
 * reads its growth-rate selector, then finds, or loads and stores, the growth
 * record at 005d17ac using the cache at 005d17b0. The record pointer is read
 * before the personal selection; the descriptor and cache cells are reread. */
void px_select_growth_resource(uint32_t species, uint32_t form, const struct px_growth_api *api);

#endif
