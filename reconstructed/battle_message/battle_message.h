#ifndef PX_BATTLE_MESSAGE_H
#define PX_BATTLE_MESSAGE_H

#include "../battle_command/battle_command.h"

/* The DllBattle message routines (static analysis addresses, runtime
 * addresses module-relative). Analyst names.
 *
 * Message commands go to the same queue as the command writer's, outside
 * its info table: groups 0x69 and 0x6a (a message of kind 1 or 2), 0x6b and
 * 0x6c (the same with an extra word, message flag bit 14). Each is the
 * group and the message id as big-endian u16s, then 32-bit words up to and
 * including the terminator 0xffff0000; group 0x6b writes its first word
 * before that list.
 *
 * A message struct (battle_action): u16 id, u16 kind (bits 0-6) and
 * argument count (7-13), flag bit 14, arguments from +4, the extra word at
 * +0x20. */

#define PX_BATTLE_MESSAGE_END UINT32_C(0xffff0000) /* literals 1086be74, 10867c98 */

/* 1086b5e0: a message command (the original is variadic; a list without
 * the terminator ends after count words here). */
void px_battle_message_command(uint32_t queue, uint32_t group, uint32_t id, const uint32_t *args, unsigned count,
                               const struct px_battle_command_api *api);
/* 10867bbc: group 0x69 with up to four arguments at args (logical); other
 * counts send none. */
void px_battle_message_plain(uint32_t context, uint32_t id, uint32_t count, uint32_t args, const struct px_battle_command_api *api);
/* 1086bbe8: show a message struct; 0 when its kind has no group. */
uint32_t px_battle_show_message(uint32_t context, uint32_t message, const struct px_battle_command_api *api);

#endif
