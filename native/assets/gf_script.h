#ifndef PX_GF_SCRIPT_H
#define PX_GF_SCRIPT_H

#include <stddef.h>
#include <stdint.h>

/* Zone scripts (ZO sections 1 and 2) are Pawn 3.3 abstract-machine images:
 * the AMX header (size, magic 0xF1E0, file and AMX version 10, flags,
 * definition size 8, cod, dat, hea, stp, cip, publics, natives, libraries,
 * pubvars, tags, nametable, overlays) with 32-bit name hashes in place of
 * name offsets, and the instructions and initial data (cod..hea) stored
 * with Game Freak's 7-bit variable-length encoding, as described by pk3DS
 * 6daaca9 (Script.cs, FireyFly's QuickDecompress) and FireyFly poketools
 * 0dce8f0 (script.c): bytes with bit 7 set continue a word, the first byte's
 * bit 6 sign-extends it. Instruction semantics follow the Pawn 3.3 abstract
 * machine (compuphase/pawn 6d82fa4, amx/amx.c), including its packed opcodes
 * (operand in the high 16 bits), position-independent jumps (target =
 * opcode address + operand) and HALT with AMX_ERR_SLEEP (12) as a yield.
 *
 * Natives are called through a callback with their name hash; what each
 * native does in the game is not known here. */

enum px_gf_script_result {
    PX_GF_SCRIPT_OK = 0, PX_GF_SCRIPT_INVALID_ARGUMENT, PX_GF_SCRIPT_TRUNCATED, PX_GF_SCRIPT_INVALID,
    PX_GF_SCRIPT_NO_MEMORY,
    /* run outcomes */
    PX_GF_SCRIPT_HALTED, PX_GF_SCRIPT_BUDGET, PX_GF_SCRIPT_YIELD_LIMIT, PX_GF_SCRIPT_MEMORY_FAULT,
    PX_GF_SCRIPT_BAD_INSTRUCTION, PX_GF_SCRIPT_NATIVE_FAILED
};
struct px_gf_script_entry { uint32_t address, hash; };
struct px_gf_script {
    uint32_t size, flags, cod, dat, hea, stp, cip;
    uint32_t native_count, pubvar_count, public_count;
    struct px_gf_script_entry *natives, *pubvars, *publics;
    uint32_t code_words, data_words;
    uint32_t *code;      /* cod..dat, decoded */
    uint32_t *data_init; /* dat..hea, decoded */
};
/* Caps: 1 MiB input, 1 MiB data segment (stp - dat), 256 Ki words. */
enum px_gf_script_result px_gf_script_decode(const uint8_t *data, size_t size, struct px_gf_script *out);
void px_gf_script_free(struct px_gf_script *script);

/* A native receives its arguments in call order (args[0] is the first) and
 * returns the value for PRI; setting *failed stops the run. */
typedef int32_t (*px_gf_native_fn)(void *context, uint32_t hash, const int32_t *args, uint32_t count,
                                   int *failed);
struct px_gf_run {
    enum px_gf_script_result result;
    uint32_t steps, yields, last_cip;
};
/* Run the entry point (cip) on a fresh copy of the data segment, after
 * writing `value` to the public variable whose hash is `pubvar` (the game
 * selects a zone script's event that way; the variable hash 0xD7477C97 is
 * the one the zone scripts switch on). Stops at the final HALT (result
 * HALTED), after `budget` instructions or `max_yields` yields, or on a fault. */
void px_gf_script_run(const struct px_gf_script *script, uint32_t pubvar, int32_t value, px_gf_native_fn native,
                      void *context, uint32_t budget, uint32_t max_yields, struct px_gf_run *run);
const char *px_gf_script_result_string(enum px_gf_script_result result);

#endif
