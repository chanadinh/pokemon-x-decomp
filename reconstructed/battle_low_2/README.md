# Battle low-address state hooks, batch 2 (10802000-10840000)

This milestone reconstructs 24 small `DllBattle.cro` functions of a menu/page
state machine in `108395d8`-`1083f270`, in the low band `10802000`-`10840000`.
The committed-caller frontier of the band is mostly large functions, so this
batch works outward and takes a coherent family instead: a scan of the band's
uncovered procedures found 47 short functions that open with the same
three-step prologue, and these 24 are the plain ones (the others use jump
tables, vector code or long bodies and are left for later). All 24 have a live
REA/Ghidra listing (session `outputs/rea-battle-10`). The names are analyst
interpretations; the functions are named by address. Runtime addresses are
static − `0x1015b000`.

## The family

Every function loads the relocated pointer `108fcb18` (ABS32, one literal per
function; the global state object) and a state id of its own, and stores the id
in the word at `+0x6c` of the object when it differs. What follows is a small
job on the page object ("this"; its subobject pointer is at `+0x190`):

| Address | State id | Behavior |
| --- | --- | --- |
| `1083b44c`, `1083c960`, `1083c984`, `1083e7dc` | `0x115b`, `0x14c7`, `0x1598`, `0x14e0` | Returns 1 |
| `1083c518`, `1083e800` | `0xdf3`, `0x1592` | Nothing else |
| `108399ec` | `0x11fd` | Tail-branches to `10858318` with the four argument registers untouched |
| `1083f134` | `0x9e1` | Clears the word at `+0xf0` |
| `1083b17c`, `1083c424` | `0x1180`, `0x11a6` | Done (1) when the word `+0x6b8` is set, else the `10858cb8` service with 0 |
| `1083bad8`, `1083f270` | `0x1155`, `0x1162` | `10800864` then `10800874` on the subobject's handle at `+0x65c` (modes 3 then 2, or 2 then 3) |
| `1083bdd4` | `0xdf9` | `10859cc4` on the subobject `+0x280`; returns 1 |
| `1083e7a0` | `0x14ab` | `10801094` on the handle at subobject `+0x4f4`; 1 when it answers |
| `1083e920` | `0x97d` | `108a26c8` on the subobject, then its word `+0x448` takes the second argument |
| `1083e960` | `0x969` | `10858150` on the subobject's `+0x3a0` block with its `+0x7b0` word |
| `1083ea0c` | `0x146f` | A virtual call at `0x24` of the subobject's first object, `10876384` of the result, stored at `+0x278`; halfword `+0x27c` and word `+0x3c` set |
| `1083f348` | `0xa44` | Clears `+0x764`, resets the subobject (`10856954`), sets its flag bytes (`+0x1c9`..`+0x1cc`) from the arguments and tail-calls `1084a214` |
| `1083cb44`, `10839818` | `0x1057`, `0xfc4` | The `10858cb8` service decides; yes runs `1085db0c` and `10859cc4` and returns 1 (`1083cb44` returns 1 at once when `+0x6b8` is `0xff`) |
| `108395d8`, `1083b110` | `0xfb3`, `0x11f1` | Fill `+0x6ca` with `0xff` (`1086fd18`), reset or set the bytes `+0x25d`, `+0x6c8`, `+0x6c9`, `+0x6ca`, then `10859464` / `1085746c` |
| `1083cbb0` | `0x145e` | Clears the subobject's `+0x23c` and `+0x244`, runs `108568ec` and `1085a18c` twice around a virtual call at `0x10` of its object `+0x88`, which is then cleared with `+0x93` |
| `1083f1f4` | `0x14d8` | `10860270` on the subobject's `+0x4ec` block, then a virtual call at `0xc0`; when it says no, `1080024c(0x60000, 0, -1, 0)` |

Calls to routines that are not reconstructed are services named by their
static address; virtual calls load the function pointer at an offset of the
object's table.

## Evidence

All 1,436 instruction bytes and 192 literal bytes match live REA bytes and the
original `DllBattle.cro`. The 24 pointer literals are ABS32 relocations to
`108fcb18` whose original words and values are checked; the other 24 literals
are the state ids and one message id (`0x273`).
[`reproduction/verify_rea.py`](reproduction/verify_rea.py) also checks the
REA listings.

## Validation

`make -C reconstructed/battle_low_2 test` runs 1,440 cases: every function on
60 random memory images each (state word equal to the id, one off, or random),
checking the service calls (target and arguments), the virtual calls, the
return value and the whole memory byte for byte. ASan/UBSan pass; 68
mutations are each detected by a failing test (two that did not compile were
replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that each function is a plain procedure of the band in REA's list, that all 24
load the same state object pointer and that no other milestone lists them. At the time of this
batch 70 of the band's 569 procedures were covered (499 uncovered); later batches change these
figures, so the record does not hold them.

## Limits

The services and virtual methods remain external. No running-game comparison
has been made. Static evidence and host comparisons do not establish a
matching original build.
