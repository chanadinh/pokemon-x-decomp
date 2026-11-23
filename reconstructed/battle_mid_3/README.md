# Battle mid-band teardowns and accessors, batch 3

This milestone reconstructs 19 `DllBattle.cro` functions in the static band
`10880000`–`108a6000`: the thirteen one-handle teardowns of the page-object
families (the same shape as the three in
[battle_mid_2](../battle_mid_2/README.md), one per page, differing only in the
handle's offset) and six small accessors beside them. Like batch 2 it is a
self-contained group, disconnected at the time of this batch (no other
milestone called these functions or covered their callees). Each has a live REA/Ghidra listing (session
`outputs/rea-battle-10`).

The names are analyst interpretations; the functions are named by address.
Runtime addresses are static − `0x1015b000`.

## Functions

| Address | Behavior |
| --- | --- |
| `1088b2e8`, `1088e858`, `10898a04`, `108993b8` | Teardown of the handle at `+0xe8`: when set, `1080085c` with it and the argument (still in r1), then the tail call `108568ec` |
| `1088bcb8` | The same for `+0xec` |
| `1088db3c` | The same for `+0xd8` |
| `1088f210`, `1088fa38`, `10890590`, `1089c5dc` | The same for `+0xe0` |
| `108910d4` | The same for `+0xf4` |
| `10899ff4` | The same for `+0xdc` |
| `1089a9b0` | The same for `+0x118` |
| `1088a340` | `108019f4` on the `+0xe8` handle; then it falls through into the next function (`1088a35c`) with r0 = the object, modelled as a tail call |
| `1088d064` | Tail call `10801a2c` with the word at `+0x118` of the object at `+0x88`, and 0 |
| `1088d98c` | `10801c0c` with the `+0xe0` handle and the signed byte at `+0xd5`, then `108019f4` with the handle, then falls through to `1088d9b4` with the object |
| `10889aac` | `10800504(a, buffer, 1)` then `1083ac28(b)` |
| `10886734` | `10800504(a, buffer, 1)` then `1083c0b8(b, the buffer's word)` |
| `10886bcc` | `10801ab4(a, buffer, 1)` then `108759c8(b, buffer)`; the buffer is 16 bytes |

## Evidence

All 720 instruction bytes match live REA bytes and the original
`DllBattle.cro`; the functions have no literal pools or relocations. The
decoder of the [move-power milestone](../battle_power/README.md#decoder)
reproduces all 180 listed instructions. Two functions end in a `cpy r0, r0`
and continue into the following function; that is how the compiler placed a
tail call, so they are modelled as tail calls, and the callee functions
(`1088a35c`, `1088d9b4`) are not part of this milestone.

The band also holds three "switch case" functions (`10880124`, `10882324`,
`10881544`), targets of a jump table in `108a0fdc`; they are left out because
their dispatcher is not reconstructed.

## Validation

`make -C reconstructed/battle_mid_3 test` runs 96 cases against a recording
mock: every teardown with its handle set or clear (with the other offsets'
handles set, to catch a wrong offset) and three arguments, the signed byte
over several values, the pointer chain, and the three buffer accessors, each
checked against its exact call sequence and return value. ASan/UBSan pass. 56
mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that none of the 19 functions is covered elsewhere, that each starts in the
band, and records the number of callees outside the batch; it records no
coverage figure, which would change as later milestones are added
([record](reproduction/record-comparison.json)).

## Limits

The callees (the import thunks `108001xx`–`108021xx`, `1083ac28`, `1083c0b8`,
`108568ec`, `108759c8`, `1088a35c`, `1088d9b4`) stay external. No running-game
comparison has been made. Static evidence and host comparisons do not
establish a matching original build.
