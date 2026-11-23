# Battle low-address functions, batch 4 (10802000-10840000)

This milestone reconstructs 7 `DllBattle.cro` functions of the low band
`10802000`-`10840000`: the callees that [battle_low_3](../battle_low_3/README.md)
left external where they were tractable, the next three members of the page
state machine family, and two battle helpers from the frontier. All 7 have a
live REA/Ghidra listing (session `outputs/rea-battle-11`). The names are
analyst interpretations; the functions are named by address. Runtime addresses
are static − `0x1015b000`.

## Functions

| Address | Behavior |
| --- | --- |
| `10836230` | Releases the helper objects held at `+0x70`, `+0x6c`, ... `+0x50` of a page object: each nonzero one gets its virtual method 4 and its pointer is cleared (battle_low_3 called it as `10836230`) |
| `1083b268` | State hook `0xbf7`: clears `+0x6b4`/`+0x6b8`, resets the selection bytes (`1086fd18`, `+0x6c8`/`+0x6c9`/`+0x25d`), takes `+0x6c8` and `+0x6ca` from the virtual call `0x10c`, and sets `+0x6b8` to `0xff` when the virtual call `0xbc` says yes |
| `1083ef8c` | State hook `0x197c`: by the page result `+0x6b8` (0, `0x64`, `0xc8`, else): the `10858cb8` / `1085db0c` / `10859cc4` start (state id `0xfc4` first), the `1080103c` / `1083ac28` finish, or `108574e8`; returns 1 or 0 |
| `10839a0c` | State hook `0x154a`: a pending-wait branch (byte `+0x494`) that clears the subobject's wait fields and, after more than `0x5a` waits, the result; otherwise the steps 0, 1, 2 and `0x64` of `+0x6b8` (selection bytes and flags of a chosen object, messages `0x280`, `0x275`, `0x278`, `108582a0`) |
| `1083c0b8` | A jump table over a code (0 to 8, else nothing): copies a pair of words of the data object `108d688c` (offsets `0x9c`, `0xa4` ... `0xd4`) to `+0x730` of the object with a flag byte at `+0x72c` (1 for code 0, else 0); code 8 acts as code 1 when the byte at `+0x82c` is 0 or 1 |
| `1081cd58` | Writes a `0x6a` record (kind 1: codes 1 to 6 and `0xf` choose the message, `0x14` writes nothing; kind 2: message `0xd2`; anything else the `0x69` record with message `0x4a`) and sets bit `0x10` of the byte at `+0x1093` of the context |
| `1082399c` | Walks a list of battlers (count `+0x6e`, running index `+0x70`): for each one `10865ae0` does not skip and `10874348` gives a type for, event `0xa5` runs with keys 3, 4, `0x22` and values `0x43`, `0x50`; when the event leaves `0x43` set, `10869784` runs and, when `0x50` is also set, a `0x6a`/`0xd5` record is written |

Calls to routines that are not reconstructed are services named by their
static address, as in battle_low_2; `1082399c` uses the battle event stack of
the event milestones.

## Evidence

All 2,012 instruction bytes and 96 literal bytes match live REA bytes and the
original `DllBattle.cro`. Thirteen literal words are ABS32 relocations whose
original words and values are checked: the global state object `108fcb18` of
the three hooks, the data object `108d688c` of `1083c0b8` and the nine words
of its jump table (`1083c0cc`, which the Ghidra listing leaves out and the
evidence lists as literals). The other literals are state ids and message ids.
[`reproduction/verify_rea.py`](reproduction/verify_rea.py) also checks the REA
listings.

## Validation

`make -C reconstructed/battle_low_4 test` runs 1,409 cases: every function on
random memory images (service answers, virtual answers and flags drawn from
both outcomes), checking the service calls, the virtual calls, the return
value and the whole memory byte for byte; the state hook `1083ef8c` also logs
the state word at each call, and `1082399c` runs the real event stack. ASan/
UBSan pass; 69 mutations are each detected by a failing test.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that each function is a plain procedure of the band in REA's list and that no
other milestone lists it. At the time of this batch 92 of the band's 569
procedures were covered (477 uncovered); later batches change these figures,
so the record does not hold them.

## Limits

The services, virtual methods and the data object `108d688c` remain external.
No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
