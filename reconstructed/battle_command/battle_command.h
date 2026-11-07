#ifndef PX_BATTLE_COMMAND_H
#define PX_BATTLE_COMMAND_H

#include "../pokemon_constructor/pokemon_constructor.h"

/* The DllBattle command writer 1086c22c and the queue encoder 1084dbf4 it
 * calls (static analysis addresses, runtime addresses module-relative).
 * Analyst names.
 *
 * Every routine reports its effects to the display side as commands: a
 * command id below 0x6d and up to 15 word arguments. The writer looks up
 * the command's info byte (arguments in the low nibble, the format in the
 * byte as a whole), copies the arguments into a static buffer and has the
 * encoder append the command to a queue: word +0 the bytes in use, data
 * from +8, 6000 bytes. The encoder writes the id as a big-endian u16, then
 * the arguments as bytes, big-endian u16/u24/u32 words or packed fields. */
struct px_battle_command_api {
    const struct px_pokemon_api *memory;
    void (*fault)(void *user);    /* 10800394(0, 0) */
    void (*overflow)(void *user); /* 108003a4(0, 0): the queue is full; the write still happens */
    /* Logical addresses of the original data the writer reads and fills:
     * the info bytes (0x6d, at 108d13fc, literal 1086c2c4) and the argument
     * buffer (16 words, at 10b0f460, literal 1086c2c8). */
    uint32_t info, buffer;
};

enum { PX_BATTLE_COMMANDS = 0x6d, PX_BATTLE_QUEUE_CAPACITY = 0x1770 };

/* 1086c22c: append command id with its arguments (the original is
 * variadic; arguments the caller does not supply read as 0). */
void px_battle_command(uint32_t queue, uint32_t id, const uint32_t *args, unsigned count, const struct px_battle_command_api *api);
/* 1084dbf4: encode a command of the given info byte from the argument
 * words at args (logical). */
void px_battle_command_encode(uint32_t queue, uint32_t id, uint32_t info, uint32_t args, const struct px_battle_command_api *api);

/* Queue writers: 10862078 (u16), 1084d930 (u24), 1084db70 (u32), big-endian. */
void px_battle_queue_u16(uint32_t queue, uint32_t value, const struct px_battle_command_api *api);
void px_battle_queue_u24(uint32_t queue, uint32_t value, const struct px_battle_command_api *api);
void px_battle_queue_u32(uint32_t queue, uint32_t value, const struct px_battle_command_api *api);
/* 1084d9a0: three fields of the given widths into bytes (2 or 3) bytes. */
uint32_t px_battle_pack3(uint32_t bytes, int32_t a, int32_t b, int32_t c, int32_t wa, int32_t wb, int32_t wc,
                         const struct px_battle_command_api *api);
/* 1084dab0: two fields of the given widths into one byte. */
uint32_t px_battle_pack2(int32_t a, int32_t b, int32_t wa, int32_t wb, const struct px_battle_command_api *api);

/* The number of argument words a format uses, or -1 for a format the
 * encoder faults on (0 and every value without a case). */
int px_battle_command_format_args(uint32_t info);

#endif
