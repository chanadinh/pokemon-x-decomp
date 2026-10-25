# Pokémon constructor move services

This ninth host-source milestone reconstructs move insertion `0x00150064`
and maximum PP `0x00150220`. It calls the earlier milestone's concrete slot
accessors and codec; the eight earlier checkpoint directories are unchanged.
Names are analyst choices, not recovered original symbols.

```sh
make -C reconstructed/pokemon_moves test
make -C reconstructed test
```

## Insertion

`px_pokemon_learn_move` reproduces the constructor's automatic move insertion.
It first scans all four slots for the incoming move, returning immediately on
a match. A second scan chooses the first zero slot. If there is no zero slot,
it copies slots 1, 2 and 3 into slots 0, 1 and 2, then inserts into slot 3.
For example, inserting E into `[A, B, C, D]` produces `[B, C, D, E]`.
Each shifted move keeps its remaining PP and PP Ups. The newly inserted move
gets base PP from move-resource selector 5 and zero PP Ups.

The manager at `object+0x0c` is reloaded before **every** getter and setter.
Each shifted record is read in move, PP, PP Ups order, then written in that
same order. New insertion queries base PP before writing move, zero PP Ups,
and the low byte of the captured PP result. A resource callback may change
the manager; the selected destination slot and previous shifts remain captured,
while each subsequent write uses the object's current manager.

The incoming move remains a full unsigned word for duplicate comparison and
the resource service. Only the concrete move setter narrows it to a halfword.
Move zero is not specially rejected: it matches any existing empty slot in
the duplicate scan, but a full moveset can shift and insert zero. The original
diagnostic branch for an empty-slot index of at least four cannot be reached
by the bounded scan and has no host effect. This is constructor behavior;
the interactive level-up decision is a separate routine.

## Maximum PP

`px_pokemon_maximum_pp` clamps the full unsigned PP Ups argument to three,
calls move-resource selector 5 once, and takes the low byte of its result.
For base PP `b` and clamped count `u`, it returns
`(b + floor(b * u * 20 / 100)) & 255`.
The original uses signed halfword multiplication on operands already bounded
to 255 and 3, followed by `UMULL` with `0x51eb851f` and a right shift of 37.
The verifier checks that this equals unsigned division by 100 for all
15,301 possible intermediate numerators. Byte wrapping is intentional:
base 255 and three PP Ups return 152.

## Evidence and boundaries

`evidence/` retains three read-only Ghidra exports, with 1,016 instruction
bytes and 12 PC-relative literal bytes checked against the pinned executable.
`0x00168bbc` is exported as dependency evidence: its cache/load orchestration
remains the `move_value` service. Its selector-5 jump-table word at
`0x0017634c` selects `0x0017641c`, which reads unsigned resource byte `+5`.
The verification script checks the dispatch word separately against code.bin.
No cache, archive loader, or other move-value selector is newly reconstructed.

Synthetic tests cover 66,048 maximum-PP cases, 3,072 insertion cases with holes,
duplicates, zero and full-word inputs across all 32 block-selector rows,
acquired and encrypted manager states, exact setter order, and manager swaps
during the resource callback. A state-initialization/refresh fixture replaces
the old move-insertion and maximum-PP stand-ins with these implementations;
learnset selection, level and name services remain synthetic there.

The native battle path also uses the reconstructed maximum-PP routine when
creating battlers. Its learnset helper now retains constructor slot order;
the optional local-data comparison checks that helper against this insertion
routine at every level of every stored learnset. See
[the native record](../../native/reproduction/move-services/README.txt).
All 80,699 level cases agree; 53,283 correct an earlier slot-order difference.

Tests and static instruction comparisons do not execute the original ARM
game or establish a running-game comparison. Real resource caching, name
serialization, other constructor dependencies and wider gameplay remain work
for later milestones. Earlier README and validation files retain their
historical claims.
