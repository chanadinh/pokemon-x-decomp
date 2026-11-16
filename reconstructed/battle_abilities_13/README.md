# Battle ability handlers: the last three

This milestone reconstructs the last three `DllBattle.cro` ability handlers
and the helper one of them ends with; with it every handler of the ability
lookup table (`0x108d510c`) appears in some milestone's evidence. None of the
four has a Ghidra function: their listings come from the
[move-power milestone's decoder](../battle_power/README.md#decoder), and the
one word it refuses, `bx lr`, is spelled as REA lists it in an analysed
function (see Details).

| Event | Address (static) | Host routine | Abilities | Effect |
| --- | --- | --- | --- | --- |
| `0x8c` | `108beca4` | `px_battle_magician` | Magician (170) | After its move, steals the item of the slowest target it may rob |
| — | `1087a19c` | `px_battle_clear_work` | (helper) | Clears the handler's seven work words `+0x1c`–`+0x34` |
| `0x04` | `108c2ae8` | `px_battle_scrappy_unhook` | Scrappy (113) | Work `+0x10` set: the handler removes itself (`1084d1cc`) |
| `0x91` | `108c1a68` | `px_battle_ate_reset` | Aerilate (184), Pixilate (182), Refrigerate (174) | Work word `+0` = 0 |

Runtime addresses are static − `0x1015b000`; ability numbers are the
original ids, named after the public Generation VI lists.

## Details

- **Magician** acts for the attacker (key `0x03`) when it holds no item now
  (`+0x12`) or before the move (work `+0`, set by its event-`0x8a` handler in
  battle_abilities_8) and key `0x4e` is clear. It copies the targets (count
  key `0x05`, from key `0x06`) into its work bytes; with six or more it
  faults (`10800394`) and skips the sort. Otherwise each target's speed
  `1081d2b0(context, pokemon, 1)`, plus 5 for its allies (`1084c5a8`), goes
  into the global halfword table at `10b11bf4` (an ABS32 literal `108bef08`,
  taken from the caller), and targets and speeds are sorted together,
  slowest first (a strict-less exchange sort, so ties keep their order). The
  first target holding an item that `1084bb5c(context, id, target)` does not
  protect is robbed: a flagged kind-`0x25` record (`+4` the target, message
  `0x427`, literal `108bef0c`, naming Magician, the target and the item).
  Whether or not anything was stolen, it ends by tail-calling `1087a19c` with
  the handler, which clears all seven work words.
- **The bx lr spelling.** `decode_arm.py` refuses `bx lr` (`e12fff1e`), and
  Ghidra has no instructions at these addresses, so REA's per-instruction
  inspection reports them undecodable. The verifier accepts a decoded `bx lr`
  only with a spelling reference: the retained REA analysis of `1084aeec`
  (reconstructed in battle_moves_3, not here) lists `bx lr` at `1084aef4`,
  whose live and original word is `e12fff1e`.

## Validation

`make -C reconstructed/battle_abilities_13 test` covers Magician's early
returns, 1–5 targets with high-bit ids under four speed patterns (rising,
falling, with ties and wrapping halfwords), four held-item masks, three
steal-block masks and two ally masks against a model of the sort; six targets
(the fault); the helper with its neighbouring words; Scrappy over three work
words; the -ate reset; and Magician through the real dispatcher (505 cases).
ASan/UBSan pass; 25 mutations are each detected by a failing test (two that
did not compile were replaced). The verifier rejects the evidence when the
spelling reference is removed or the word is spelled otherwise (both checked
by hand).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the three handlers are bound to exactly five (ability, event) pairs, that
the helper is bound to nothing, that every ability entry of event `0x91` is one
of them, and that all 288 handlers of the ability table appear in some
milestone's `evidence/functions.json`
([record](reproduction/record-comparison.json)).

## Limits

The speed, ally, steal-block, handler-removal, message and action services
remain external. No running-game comparison has been made. Static evidence and
host comparisons do not establish a matching original build.
