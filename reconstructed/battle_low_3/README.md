# Battle low-address state hooks, batch 3 (10802000-10840000)

This milestone continues the page state machine of
[battle_low_2](../battle_low_2/README.md) with 15 more `DllBattle.cro`
functions in `1083b3cc`-`1083f3ac`. They have the same shape: each loads the
relocated pointer `108fcb18` (the global state object, ABS32, one literal per
function) and a state id of its own, stores the id in the word at `+0x6c` of
the object when it differs, and then does a job on the page object ("page"; its
subobject pointer is at `+0x190`, its entry pointers start at `+0x100`).
Batch 2 took the plain members of the family; these are the next larger ones
(the three with vector code or very long bodies are left for later). All 15
have a live REA/Ghidra listing (session `outputs/rea-battle-10`). The names
are analyst interpretations; the functions are named by address. Runtime
addresses are static − `0x1015b000`.

## Functions

| Address | State id | Behavior |
| --- | --- | --- |
| `1083b3cc` | `0xa7f` | When the virtual call `0xbc` says no: `1089d648` on the word `+0x1e0`, then `10858150` on the subobject's `+0x3a0` block |
| `1083b470` | `0x9bc` | A two-step start (word `+0xf0` 0 then 1): `108a2abc`, `10836230`, `108573b4`, and the subobject's flag bytes `+0xa0`/`+8`; returns 1 when done |
| `1083b530` | `0x1465` | Releases the object at subobject `+0x240` through its virtual method 4 and clears the pointer |
| `1083b9f0`, `1083c770` | `0x116b`, `0x1190` | Fill the selection bytes (`+0x6c8`, `+0x6c9` from the virtual call `0x10c`, `+0x25d` cleared) and pick the entry for the index: pending (`+0x6b8` = 0x64, returned) when the entry says no and its byte `+0x15c` is clear, else `10859464` with message `0x26f` / `0x270` |
| `1083bb1c` | `0x993` | Stores two arguments at `+0x760`, copies the 6-byte list of a member query (`1085daf8`, `10856a88` with `0x13`) and sets the subobject's flags and bytes (`+0x3c`, `+0x4a`, `+0x138`, `+0x13f`, `+0x40`) before `1085fa8c` |
| `1083c1d8` | `0x102f`, `0x1051` | Selects an entry, asks the subobject's first object (virtual call `0x44`) and sets `+0x25d` from the index (non-zero when the answer is 4, else its low bit), then `10859464`; without an entry `+0x6b8` becomes `0xff`; ends by storing the second id |
| `1083c9a8` | `0xabe` | When the virtual call `0xbc` says no: `1089da54` with a halfword and a signed byte, then `10858150` |
| `1083ca38` | `0xa18` | Stores two arguments at `+0x760`, sets up the subobject's request block (`+0x1c4`.., `+0x1c8`, `+0x1cc`, `+0x1d0`) and runs `10856954`, `1084a214`, `1084a0bc`, `108577b4` |
| `1083cac0` | `0x1479` | Maps `108a31a0`'s answer to a byte through a pointer: 2 gives 4, 10 to 13 give 0 to 3, anything else leaves 0 and returns 0 |
| `1083e4c4` | `0x11ca` | Selection bytes from the argument, a virtual call `0x20` for its object and a start message: `0x284` / `0x285` for object kind `0x2a9` by its flag byte `+0x153`, else `0x272` |
| `1083e58c` | `0x1711` | Fills the selection bytes and the request words (`+0x6d0`, `+0x6d4`, `+0x6e4`, `+0x6b8`, `+0x6bc`, `+0x6fc`, `+0x700`, `+0x71c`, `+0x71d`) from five arguments |
| `1083f15c` | `0x14a3` | `108577b4`, `1082d830`, `1082d6d8` on the `+0x4dc` block, then the virtual call `0xc0`; when it says no, `1080024c(0x60000, 0, -1, 0)` |
| `1083f2b4` | `0x111c` | Like `1083b9f0` for one index: pending (`+0x6b8` = 0x64) or `+0x6b4` = 0 with `+0x6b8` = 1 |
| `1083f3ac` | `0xd8b` | Runs `1083be10` over the six selection bytes that are not `0xff` (offsets `+0x6b4 − 10 i`), steps `+0x6b4`, and starts `10859cc4` when all said yes |

Calls to routines that are not reconstructed are services named by their
static address (the same interface as battle_low_2); virtual calls load the
function pointer at an offset of the object's table. Return values the
original leaves in `r0` incidentally are not modelled.

## Evidence

All 2,436 instruction bytes and 140 literal bytes match live REA bytes and
the original `DllBattle.cro`. The 15 pointer literals are ABS32 relocations
to `108fcb18` whose original words and values are checked; the other literals
are the state ids and a few message ids.
[`reproduction/verify_rea.py`](reproduction/verify_rea.py) also checks the REA
listings.

## Validation

`make -C reconstructed/battle_low_3 test` runs 1,232 cases: every function on
80 random memory images each (state word equal to the id, one off, or random;
service answers, virtual answers and flag bytes drawn from both outcomes),
checking the service calls (target and arguments), the virtual calls, the
return value and the whole memory byte for byte; `1083c1d8` also logs the
state word at each call, so the order of its two state stores is checked. ASan/
UBSan pass; 90 mutations are each detected by a failing test (four that did not
compile and five that survived led to fixes).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that each function is a plain procedure of the band in REA's list, that all 15
load the same state object pointer and that no other milestone lists them. At
the time of this batch 85 of the band's 569 procedures were covered (484
uncovered); later batches change these figures, so the record does not hold
them.

## Limits

The services and virtual methods remain external. No running-game comparison
has been made. Static evidence and host comparisons do not establish a
matching original build.
