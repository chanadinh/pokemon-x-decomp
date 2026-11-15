# Battle transformation, status and move-reaction abilities

This milestone reconstructs the twelfth batch of the remaining
`DllBattle.cro` ability handlers: 12 handlers that transform a battler,
pass on or react to a status or a move, or change a move's type or power.
Nine were reviewed through live REA MCP/Ghidra listings; three have no
Ghidra function and are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder).
Fainted (`1085f844`) comes from [battle_abilities_1](../battle_abilities_1/README.md).

| Event | Address (static) | Host routine | Ability | Effect |
| --- | --- | --- | --- | --- |
| `0x5e` | `108bcf0c` | `px_battle_imposter` | Imposter (150) | Transforms into the battler facing it |
| `0x1e` | `108bd1b4` | `px_battle_truant_loaf` | Truant (54) | Every other turn its move fails (reason `0x13`) |
| `0x21` | `108c190c` | `px_battle_truant_message` | Truant (54) | "Is loafing around" (message `0x1bd`) |
| `0x71` | `108bd33c` | `px_battle_synchronize` | Synchronize (28) | Passes poison, paralysis or burn back to its source |
| `0x3c` | `108bf054` | `px_battle_aura_break_reverse` | Aura Break (188) | The auras are reversed (key `0x57`) |
| `0x23` | `108c0578` | `px_battle_protean` | Protean (168) | Becomes the type of its move |
| `0x03` | `108c0e14` | `px_battle_stance_change` | Stance Change (176) | Aegislash's shield and blade forms |
| `0x73` | `108c16a0` | `px_battle_illusion_leave` | Illusion (149) | Leaving breaks the illusion |
| `0x53` | `108c29c0` | `px_battle_illusion_hit` | Illusion (149) | Being hit breaks it |
| `0x93` | `108c419c` | `px_battle_illusion_lost` | Illusion (149) | Losing the ability breaks it |
| `0x21` | `108c27ac` | `px_battle_steadfast` | Steadfast (80) | Flinching: Speed +1 |
| `0x99` | `108c4f60` | `px_battle_mega_launcher` | Mega Launcher (178) | Heal Pulse (505): key `0x36` rewritten to `0xc00` |

Runtime addresses are static − `0x1015b000`; ability, species and move
numbers are the original ids, named after the public Generation VI lists.

## Details

- **Imposter** finds the battler at the position `1086dddc(1084bf14(context),
  10873158(context, id))` gives through `1084d348`, needs both alive, and
  commits kind `0x34` (`+4` that battler, flagged) with message `0x284` naming
  itself and the target (re-read from `+4`).
- **Truant** marks its work word on one turn; on the next it rewrites key
  `0x23` to `0x13`, keeps the result in work `+4` and clears the mark; the
  message handler announces it once (flagged) and clears work `+4`.
- **Synchronize** needs status key `0x1e`'s low byte 5, 1 or 4 and a source
  (key `0x03`'s low byte) that is neither `0x1f` nor itself. Inside the popup
  it commits kind `0xb` (`+4` status, `+0xc` 1, `+0xf` the source); the
  condition word `+8` is key `0x1f` for bad poison (`1087a1c0`), otherwise
  `1084c438(status, pokemon, record + 8)`.
- **Protean** needs the type's low byte below 18 and commits kind `0x13`
  (`+4` `t | t << 5 | 0x4800`, `+6` itself, flagged); unlike Color Change it
  does not check its current types.
- **Stance Change** checks species 681 and the form byte `+0x153`: King's
  Shield (588) gives form 0, damaging moves (`10800434`) form 1, anything else
  no change; the flagged kind-`0x3a` record has message `0xfd` (shield) or
  `0xfc` (blade), kind 1.
- **Illusion** commits an unflagged kind-`0x35` record (`+4` itself,
  message `0x1de`) when `1084aa9c` reports the illusion active: on leaving
  (key `0x02`), on a hit with key `0x47` clear (key `0x04`), or when key `0x10`
  no longer equals the handler's ability `+0x38`.
- **Steadfast** needs reason `0x23` = 6 (the whole word) and clears bit 1 of
  the stat record's flags `+0x18`.

## Validation

`make -C reconstructed/battle_abilities_12 test` covers every handler over
the battler key (including a high-bit id) and the inputs it reads: both
battlers' HP for Imposter; Truant's work words with key `0x23` already
rewritten; Synchronize over six status words, four sources and the bad-poison
answer; Protean over 20 types with high bits; Stance Change over species,
forms, three moves and the damaging answer; Illusion's three triggers; three
reason words for Steadfast and three move words for Mega Launcher; and
Truant's two handlers through the real dispatcher (477 cases). ASan/UBSan
pass; 30 mutations are each detected by a failing test (one that did not
compile was replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 12 handlers are bound to exactly 12 (ability, event) pairs and that
every ability entry of events `0x1e`, `0x71`, `0x3c`, `0x23` and `0x99` is
one of them ([record](reproduction/record-comparison.json)).

## Limits

The ability handlers still not reconstructed are Magician's event-`0x8c`
handler (it sorts targets by speed through a relocated table) and the two
handlers that end in `bx lr`, which the decoder does not list (Scrappy's
event `0x04` and Aerilate/Pixilate/Refrigerate's event `0x91`). The position,
condition, illusion, damaging-move, popup, message and action services remain
external. No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
