# First host source reconstruction: CallWildBattle wrapper

This small C11 project reconstructs the control flow and argument handling of
the 52-byte Field-module wrapper registered as `CallWildBattle`. It compiles
on a PC and tests calls through stand-in dependencies. It does not implement
a wild battle, render a game, or reconstruct the helper functions.

The function name comes from script registration evidence. Original source
types and the full script calling convention remain unknown. The source here
is an analyst-authored host adaptation, not recovered original source,
an instruction-matching build, or a verified native port.

## Build and test

With a C11 compiler and Make:

```sh
make test
```

Without Make, from this directory (GCC/Clang, including MinGW on Windows):

```sh
cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror call_wild_battle.c test_call_wild_battle.c -o test_call_wild_battle
./test_call_wild_battle
```

On Windows use the compiler's executable name (typically `gcc`) and run
`.\test_call_wild_battle.exe` in PowerShell. No game files are needed to run
these isolated tests. See `validation.txt` for the checks actually run here.

## Evidence and adapter contracts

The reviewed instructions are in `evidence/15c91330.asm`. The original CRO
file offset is `0x91330`; its synthetic analysis address is `0x15c91330`.
CRO addresses in this document are static-analysis addresses, not measured
runtime addresses. Main-executable addresses retain the original mapping.

| ARM operation | Reconstructed behavior |
| --- | --- |
| Load `[r1 + 4]`, then `UXTH` | Read `args[1]`, keep low 16 bits, before any callbacks |
| Call `0x15c016c0` | `get_context`; veneer resolves to main accessor `0x003dcfc8` |
| Save context; load `[context + 0x0c]` | `read_context_word_0c`; retain original context identity |
| Call `0x15ca16dc` | `field_helper(field_value, argument, 0)` |
| Call `0x15c02f60` | `context_helper(original_context)`; veneer resolves to main `0x003dea54` |
| Set r0 to zero and return | Return `uint32_t` zero |

`get_context`, `field_helper`, and `context_helper` stand in for the three
original calls; their internal behavior is not reconstructed. The additional
`read_context_word_0c` callback adapts one original 32-bit memory load. It
must supply that logical field's value after the accessor returns. Whether
the value is a pointer or another kind of handle has not been established
by this wrapper alone, so the API preserves it as an opaque `uint32_t`.

The opaque host context is not a byte-for-byte 3DS structure. Do not cast
an original 32-bit game address to a native host pointer or assume that a
64-bit host struct has a field at the same byte offset. Future object
mapping belongs in the adapters. The `user` callback parameter is host test
plumbing; it is not an original game argument.

Inputs are decoded host `uint32_t` words, not a raw ROM byte stream. The
original incoming r0 value is unused and is omitted from this host API.
The API requires two valid words, valid callbacks, and normal callback
returns. It does not emulate invalid-memory faults, ARM registers, physical
load width, instruction timing, or the original ABI. As with ordinary C
memory accesses, concurrent unsynchronized mutation is outside its contract.

The tests check eight boundary/value cases and one callback mutation case.
They check narrowing, sampling before callbacks, exact helper arguments,
call order, reuse of the same context despite changes to the active context,
zero return, and input preservation apart from deliberate callback writes.
They validate the reconstruction against these assembly-derived expectations;
they do not compare a running 3DS game against this C implementation.

## Next reconstruction target

Trace Field helper `0x15ca16dc` (CRO file offset `0xa16dc`, text-relative offset
`0xa155c`) and its callers to establish parameter types and effects. Then
replace one stand-in at a time with reconstructed, independently checked
logic. The shared helper `0x003dea54` and the context layout also need review.
A playable port additionally needs many game systems, assets, graphics,
audio, input, save handling, and replacements for 3DS platform services.
