#ifndef PX_BATTLE_TASK_H
#define PX_BATTLE_TASK_H

#include <stdint.h>

enum { PX_BATTLE_TASK_SIZE = 0x26c, PX_BATTLE_AUX_SIZE = 0x1c };

/* Logical ARM32 addresses; no game address is a native host pointer. */
struct px_battle_task_api {
    void *user;
    uint8_t (*load8)(void *user, uint32_t address);
    uint16_t (*load16)(void *user, uint32_t address);
    uint32_t (*load32)(void *user, uint32_t address);
    void (*store8)(void *user, uint32_t address, uint8_t value);
    void (*store32)(void *user, uint32_t address, uint32_t value);
    uint32_t (*heap_for_id)(void *user, uint32_t id);
    uint32_t (*allocate)(void *user, uint32_t size, uint32_t heap);
    void (*init_scene)(void *user, uint32_t destination, uint32_t field,
                       uint32_t primary, uint32_t secondary, uint32_t zero);
    uint32_t (*construct_aux)(void *user, uint32_t storage, uint32_t heap);
    void (*populate_aux)(void *user, const uint8_t *request,
                         uint32_t heap, uint32_t aux);
    uint32_t (*select_config)(void *user, const uint8_t *request);
    void (*init_state)(void *user, uint32_t destination, uint32_t field,
                       uint32_t aux, uint32_t scene, uint32_t extra,
                       uint32_t config, uint32_t heap);
    /* Original helper writes bytes 0..9; caller consumes byte 2. */
    void (*decode_environment)(void *user, uint32_t primary,
                               uint32_t secondary, uint8_t *output);
};

uint32_t px_create_battle_task(uint32_t manager, const struct px_battle_task_api *api);
void px_consume_wild_request(uint32_t task, uint32_t field,
                            const uint8_t *request, uint32_t mode,
                            const struct px_battle_task_api *api);
uint32_t px_submit_wild_request(uint32_t manager, uint32_t field,
                                const uint8_t *request, uint32_t mode,
                                const struct px_battle_task_api *api);
uint32_t px_decode_optional_key(uint32_t value);

#endif
