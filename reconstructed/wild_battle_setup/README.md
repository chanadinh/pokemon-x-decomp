# Wild-battle record preparation

This C11 reconstruction extends the earlier `CallWildBattle` wrapper with
verified initialization, record decoding, helper call sequencing, and the
small final context update. It uses synthetic test memory. It does not yet
create a running battle or implement the larger game-state setup and battle
submission functions.

Run `make test` here, or `make test` in the parent directory to test both
milestones. A GCC/Clang command without Make, from this directory, is:

```sh
cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror wild_battle_setup.c test_wild_battle_setup.c ../call_wild_battle/call_wild_battle.c -o test_wild_battle_setup
./test_wild_battle_setup
```

With MinGW on Windows, use `gcc` and run `.\test_wild_battle_setup.exe`.
Validation recorded here was performed on Linux; a native Windows build
has not been tested. No ROM or asset files are needed for these host tests.

## What the original code establishes

The wrapper narrows its second script word to 16 bits. Field helper
`0x15ca16dc` uses that value as an index into records with a 12-byte stride.
Its relocated table pointer is `0x15cee46c`. The helper itself preserves
its full 32-bit index parameter; the narrowing occurs in the wrapper.
The original helper contains no table bounds check. Neither the number of
valid rows nor the gameplay names of the fields have been established here.

The helper first calls initializer `0x15c75610`. It then reads an optional
value through this chain of 32-bit game addresses:

```text
optional_object = load32(field + 0x48)
optional = 0                                      if optional_object == 0
optional = load32(load32(load32(optional_object + 0x158) + 0x0c) + 0x148)
                                                  otherwise
```

The final `+0x148` load is the complete main-executable accessor
`0x004a4000`. The only conditional null test in this sequence is the first
one; valid subsequent pointers are a precondition of the original code.

The unreconstructed setup function `0x15c75434` receives:

```text
(buffer, field, 1, optional, 255, load8(load32(field + 0x2c) + 0x34))
```

After setup, the helper sets three 32-bit words to one and applies the
selected record. Offsets below are relative to the start of the temporary
buffer, not the function's stack pointer.

| Record source | Buffer destination | Conversion |
| --- | --- | --- |
| Bytes 0–1 | `+0x00` | Little-endian 16-bit value |
| Bytes 4–5 | `+0x02` | Little-endian 16-bit value |
| Byte 3 | `+0x04` | Copy |
| Byte 2 | `+0x05` | Copy |
| Flags at bytes 6–7, bits 0–1 | `+0x06` | Two-bit value |
| Flags bits 4–6 | `+0x07` | Three-bit value |
| Flags bits 2–3 | `+0x10` | Two-bit value |
| Bytes 8, 9, 10 | `+0x8c`, `+0x8d`, `+0x8e` | Copy |
| Flags bits 7–14 | `+0x8f` | Eight-bit value; bit 15 is discarded |
| Constant 1 | Words `+0x6c`, `+0x70`, `+0x74` | Little-endian 32-bit stores |

Record byte 11 is unused by this helper. Other buffer bytes survive the
record patch. The record field names remain offset-based to avoid guessing
which values describe species, level, items, or other encounter properties.

The helper then calls main `0x003bcb44` with
`(load32(field + 0x18), field, buffer, original_third_argument)`.
The host API exposes the final r0 value as a `uint32_t` result. This reflects
register flow, not proof that the original C function declared a return
value. `CallWildBattle` ignores that value and returns zero.

The wrapper's final shared helper `0x003dea54`, followed through
`0x004ce9bc`, is completely understood at this level: write 12 to
`load32(context + 0x34) + 0x58`, then return zero. The meaning of state 12
has not yet been established.

## Buffer initialization and corrected evidence

The initializer clears bytes `[0, 0x6c)`, sets word `+0x6c` to 1 and byte
`+0x8e` to `0x40`, and clears the additional fields visible in its assembly.
It leaves holes untouched, including `+0x78..0x83`, `+0x87`, `+0x94`,
`+0x99..0x9b`, and `+0xa1..0xa3`. It returns the original buffer pointer.
It does not clear the entire object.

Ghidra initially exported only 112 bytes of this initializer because its
call to the returning zero-fill function was treated as nonreturning. Raw
instructions show an additional `mov r0,r4` and `pop {r4,pc}`. The corrected
evidence covers all 120 bytes. The zero-fill callee is main `0x00301a9c`;
its reviewed stores clear exactly 108 bytes for this invocation.

`RepairWildInitializerForExport.java` applies a guarded, temporary analysis
repair before export. It was run with `-readOnly`, so the saved Ghidra
project and all uploaded binaries remain unchanged. The reviewed assembly
is included under `evidence/`; generated `.c` files there are analysis
pseudocode and are not part of the host build.

The workspace constant `0xac` is the available stack storage after the
outgoing argument area in this helper. It is not a recovered C `sizeof`.
Observed initializer/setup writes extend through byte `0xa7`; the last
four workspace bytes and other uninitialized holes must not be given
invented meanings. The host caller supplies workspace storage so tests can
check preservation of these bytes explicitly.

## Host adaptation and remaining dependencies

All game pointers in the new API are logical 32-bit addresses, interpreted
by `load32`, `load8`, and `store32`. They are never cast to host pointers.
These callbacks must implement ordinary, valid game-memory accesses.
The record callback supplies a stable 12-byte row which does not overlap
the workspace. A real host integration must provide the table and validate
its own mapping. Host lookup failures and invalid memory are outside this
API's contract; no original error return has been inferred.

`setup` remains a stand-in for the 472-byte function `0x15c75434`, which
reads wider save/field state and invokes many more helpers. `submit`
remains a stand-in for main `0x003bcb44`, which calls `0x004f37e8` and
`0x003bc520`. The integration test connects the prior wrapper to the new
preparation and final-update functions, with these two dependencies still
represented by assertions and synthetic state.

The source preserves ordinary memory results and call sequencing on the
valid-input path. It does not reproduce physical load/store widths,
instruction timing, stack layout, concurrent mutation, invalid-access
faults, or the original ABI. The tests compare against independently
reviewed assembly expectations, not an executing 3DS game. No matching
machine-code or complete source-recovery claim is made.

## Next target

Trace main `0x003bc520`, reached through the submission adapter, to learn
how this prepared buffer becomes battle state. In parallel with that work,
the setup function's field meanings and dependencies need reconstruction.
Those are necessary before replacing the remaining stand-ins and attempting
runtime comparisons. Assets, rendering, audio, input, save handling, and
3DS service replacements remain later work for a playable PC port.

All CRO addresses above are synthetic analysis addresses. Main executable
addresses retain the original map. See `evidence/provenance.json` and
`validation.txt` for input binding and actual checks performed.
