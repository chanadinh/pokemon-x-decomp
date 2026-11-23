# Battle mid-band page objects, batch 2

This milestone reconstructs 22 `DllBattle.cro` functions in the static band
`10880000`–`108a6000`: the init, teardown and per-frame update functions of
a family of page objects (five clone families at `10892xxx`, `108935xx`,
`10894xxx`, `10895xxx`/`10896xxx` and `10897xxx`). The committed-caller
frontier of the band was used up by [battle_mid_1](../battle_mid_1/README.md),
so this batch works outward: it is a self-contained group, disconnected at the
time of this batch (no other milestone called its functions or covered their
callees), chosen because the clones make it a coherent group of small
functions. Each has a live REA/Ghidra listing (session
`outputs/rea-battle-10`).

The names are analyst interpretations; the functions are named by address.
Runtime addresses are static − `0x1015b000`. The object is a page with
handles at `+0xe4`, `+0xec` and `+0xfc`, a state byte (`+0xf4`, or `+0x104`)
and a next-state byte (`+0xf5`, `+0x105`).

## Functions

| Address | Behavior |
| --- | --- |
| `10892810`, `1089289c` | Init (variant a): `+0x24` clear, `+0xd9` = 3, the `+0xec` handle started (`10801574`), state 0 / next 3 / value `0x41`, `1085168c` run on the object, `+0xd9` = 2, the handle started again (`1080157c`), state 1 / next 2 |
| `10893568`, `108935f4` | The same with `108511c8` and `+0xd9` = 2 last (variant b) |
| `1089498c`, `10894a18` | Variant b with `10850cf4` and value `0x5f` |
| `10892874`, `108935cc`, `108949f0` | Teardown: with the handle at `+0xe4` set, `1080085c` with it and the argument, then the tail call `108568ec` |
| `108954a0`, `10896744` | The same for the handle at `+0xf4`; `10897960` for `+0xd8` |
| `10892c34`, `108939f0`, `10894e14` | Init (the first half of variant a) followed by the handle's reset: `10801c54` saved, `1080085c` with 0, `10801c1c`, `1080085c` with the saved value (tail) |
| `10892ca4`, `10893a60`, `10894e84` | Per-frame update: in state 0 a child routine runs (`10892900`, `10893658`, `10894a7c`); in state 1, while the frame value (`+0xf6`) is below the limit (`0x40`, `0x40`, `0x5e`), `10801bdc` must answer, then `108015ac` runs, the next state starts the `+0xec` handle (`10801574` for 0, `1080157c` for 1 and 2) and becomes the state; state 2 ends the page (`+0x24` = 1) after the abort helper `10800394` when `+0xd9` is 3 |
| `10895410`, `108954c8`, `1089676c` | The pages with state at `+0x104`: `+0xe3` = 8, the `+0xfc` handle started, value `0x88`, `10850998` (`1084fe30` for the last) run, the handle started again, state 1 / next 2, `+0xe3` = 7, value `0xf`/`0x10`/`0x10`, `108015b4` on the `+4` area, tail call `108568ec` (1, 0, 0) |
| `10897bf4` | The `+0xe0` page: `+0xd5` = 2, handle started, state bytes at `+0xe8`–`+0xea`, `108015ec`, `108568ec(1)`, tail call `10853d20` |

## Evidence

All 2,396 instruction bytes match live REA bytes and the original
`DllBattle.cro`; the functions have no literal pools or relocations. The
decoder of the [move-power milestone](../battle_power/README.md#decoder)
reproduces all 599 listed instructions. The three update functions reach
`10800394` (the abort helper of the event stack, which returns) at their
`+0xcc`; an earlier Ghidra view shows it as a fault, but the bytes are a
`bl`.

## Validation

`make -C reconstructed/battle_mid_2 test` runs 823 cases against a recording
mock: every init, teardown (handle set or clear, three arguments), reset
(three saved values), update (every state, frame value around the limit,
answer, next state and `+0xd9`, 768 combinations over the three
functions), late page and the last page, each checked against its exact
sequence of stores and calls. ASan/UBSan pass. 114 mutations are each
detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that none of the 22 functions is covered elsewhere, that each starts in the
band, and records the number of callees outside the batch (21); it records no
coverage figure, which would change as later milestones are added
([record](reproduction/record-comparison.json)).

## Limits

The 21 callees (the import thunks at `108003xx`–`108021xx`, `10850xxx`/
`10851xxx` helpers, `108568ec`, `10853d20` and the three child updates) stay
external, as does the page object's layout beyond the fields used. No
running-game comparison has been made. Static evidence and host comparisons
do not establish a matching original build.
