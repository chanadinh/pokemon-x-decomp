#ifndef PX_ZONE_TALK_H
#define PX_ZONE_TALK_H

#include "gf_script.h"

/* Talking to an NPC: run the zone script (ZO section 2) with the NPC's script
 * number in the public variable 0xD7477C97 and collect the text lines it
 * shows. Native policy (inferred from the scripts, not traced in the game):
 *
 *   0x9ADF1616  message window: the first argument is a line of the zone's
 *               story text (every call in all 360 zones names a line that
 *               exists); returns 0
 *   0x59008363  player is female: returns `female` (the line pair 0/1 of the
 *               house NPC differs exactly by "young man"/"young lady")
 *   0x8E50F765  read a work value: returns 0 (a new game)
 *   0x15F86E59, 0xC8E7FB61  polled in wait loops after an actor or message
 *               command: return 1 (done)
 *   any other   returns 0
 *
 * Runs stop at the final HALT, 200000 instructions or 500 yields. */
enum { PX_ZONE_TALK_MAX_LINES = 32 };
struct px_zone_talk {
    struct px_gf_run run;
    uint32_t line_count, native_calls;
    int32_t lines[PX_ZONE_TALK_MAX_LINES];
};
void px_zone_talk(const struct px_gf_script *script, int32_t script_id, int female, struct px_zone_talk *out);

#endif
