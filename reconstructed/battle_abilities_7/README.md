# Battle trapping, escape, priority and endurance abilities

This milestone reconstructs the seventh batch of the remaining
`DllBattle.cro` ability handlers: 14 handlers that trap or free a switching
battler, resist forced switches and Ground moves, add PP use or priority,
and let Sturdy survive. Eleven were reviewed through live REA MCP/Ghidra
listings; three have no Ghidra function and are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder). Full
HP (`1084d318`) comes from [battle_abilities_6](../battle_abilities_6/README.md).

| Event | Address (static) | Host routine | Abilities | Effect |
| --- | --- | --- | --- | --- |
| `0x0c` | `108be518` | `px_battle_shadow_tag` | Shadow Tag (23) | An opponent switching is trapped, unless a listed battler also has Shadow Tag |
| `0x0c` | `108bef74` | `px_battle_arena_trap` | Arena Trap (71) | Trapped when a listed battler is on the ground (`1084bf24`) |
| `0x0c` | `108bd98c` | `px_battle_magnet_pull` | Magnet Pull (42) | Trapped for each listed Steel type |
| `0x0b` | `108bdd94` | `px_battle_run_away` | Run Away (50) | An ally's escape succeeds (key `0x53`) |
| `0x0d` | `108c0828` | `px_battle_run_away_message` | Run Away (50) | Announces the escape |
| `0x95` | `108bda2c` | `px_battle_suction_cups` | Suction Cups (21) | Forced switches fail (message `0x1c6`) |
| `0x12` | `108bc89c` | `px_battle_levitate_check` | Levitate (26) | Ground moves miss (key `0x53`); work word = rewritten |
| `0x1b` | `108bfd64` | `px_battle_levitate_message` | Levitate (26) | "It doesn't affect", then the work word is cleared |
| `0x56` | `108be848` | `px_battle_pressure_pp` | Pressure (46) | One more PP (key `0x21`) from opponents |
| `0x11` | `108c1728` | `px_battle_prankster` | Prankster (158) | Status moves: priority + 1 |
| `0x11` | `108c297c` | `px_battle_gale_wings` | Gale Wings (177) | Flying moves: priority + 1 |
| `0x79` | `108c2850` | `px_battle_sturdy_ohko` | Sturdy (5) | One-hit KO moves fail |
| `0x7d` | `108c3a18` | `px_battle_sturdy_endure` | Sturdy (5) | At full HP it survives a hit (reason 4) |
| `0x7e` | `108c28e4` | `px_battle_sturdy_message` | Sturdy (5) | Announces it held on (message `0x202`) |

Runtime addresses are static − `0x1015b000`; ability numbers are the
original ids, named after the public Generation VI lists.

## Details

- **Trapping** requires the switching battler (key `0x02`, low byte) not to be
  an ally (`1084c5a8`). The opponents of the handler's battler
  (`1084cb78 | 0x100`) are listed into a 12-byte stack area by `1084bc20`.
  Shadow Tag checks effective ability `0x11` against 23 and rewrites key
  `0x42` only after the whole list; Arena Trap rewrites at the first battler
  `1084bf24` does not report airborne; Magnet Pull rewrites for every Steel
  type (8).
- **Run Away's** message (kind 1, `0x4b`) takes the extra word `0xd0024`
  through `10876c28` (literal `108c08b4`). Suction Cups, Levitate and Run
  Away flag their records `0x800000`; Sturdy's are inside the popup and
  unflagged.
- **Levitate** needs key `0x53` clear and keeps the rewrite's result in its
  work word for event `0x1b`. **Sturdy** stores the result of rewriting key
  `0x3b` to 4 (or 0 below full HP) for event `0x7e`.
- **Pressure** skips allies of the attacker (key `0x03`'s low byte). It
  counts when it is among the targets (count key `0x05`, targets from key
  `0x06`), otherwise only for field moves (kind 10, `108007d4`) and the moves
  `108a48d4` lists; key `0x21` keeps its low byte.
- **Prankster** needs category 0 of key `0x12`'s low half; Gale Wings the
  whole type word 2. Both add one to the whole priority word key `0x19`.

## Validation

`make -C reconstructed/battle_abilities_7 test` covers the three trapping
handlers over the ally answer, 0–3 listed opponents and every mask of their
abilities, types and footing; Run Away and its message with key `0x53`
already rewritten or not; Suction Cups, Levitate and Sturdy over battler keys,
keys already rewritten and work words; Sturdy's survival over HP; Pressure
over the ally answer, 0–4 targets, move kinds, the move list and PP words;
Prankster and Gale Wings over categories, type words and a priority word above
a byte; and Levitate's two handlers through the real dispatcher (970 cases).
ASan/UBSan pass; 31 mutations are each detected by a failing test (none
replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 14 handlers are bound to exactly 14 (ability, event) pairs and that
every ability entry of the eleven events they serve is one of them
([record](reproduction/record-comparison.json)).

## Limits

The ally, side, type, footing, move-kind, move-list, popup, message and
action services remain external. No running-game comparison has been made.
Static evidence and host comparisons do not establish a matching original
build.
