#ifndef PX_BATTLE_LOW_4_H
#define PX_BATTLE_LOW_4_H

#include "../battle_low_2/battle_low_2.h"
#include "../battle_event/battle_event.h"

/* DllBattle low-address functions, fourth batch (static analysis addresses;
 * runtime addresses are module-relative): three state hooks of the page state
 * machine of battle_low_2/3 (1083b268, 1083ef8c, 10839a0c, each storing its
 * state id in word +0x6c of the global object 108fcb18 first), the release
 * function of the page's helper objects (10836230), a code-to-pair mapper
 * (1083c0b8, a jump table over the table object at 108d688c), the message
 * writer for the 0x6a/0x69 records (1081cd58) and the event 0xa5 walk over a
 * battler list (1082399c). The names are analyst interpretations; the
 * functions are named by address.
 *
 * Services and virtual methods are called through the low_2 API; 'events' is
 * the event stack of the battle (for 1082399c); 'table' is the logical address
 * of the relocated data object 108d688c. */
struct px_battle_low_4_api {
    const struct px_battle_low_2_api *base;
    const struct px_battle_event_api *events;
    uint32_t table;
};

void px_battle_release_objects_10836230(uint32_t page, const struct px_battle_low_4_api *api);
void px_battle_state_1083b268(uint32_t page, uint32_t a, const struct px_battle_low_4_api *api);
uint32_t px_battle_state_1083ef8c(uint32_t page, const struct px_battle_low_4_api *api);
uint32_t px_battle_state_10839a0c(uint32_t page, uint32_t a, const struct px_battle_low_4_api *api);
void px_battle_pair_for_code_1083c0b8(uint32_t object, uint32_t code, const struct px_battle_low_4_api *api);
void px_battle_write_message_1081cd58(uint32_t context, uint32_t battler, uint32_t kind, uint32_t code,
                                      const struct px_battle_low_4_api *api);
void px_battle_event_0xa5_walk_1082399c(uint32_t context, uint32_t unused, uint32_t battler, uint32_t list,
                                        const struct px_battle_low_4_api *api);

#endif
