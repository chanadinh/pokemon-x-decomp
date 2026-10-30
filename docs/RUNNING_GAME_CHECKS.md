# Running-game checks

These checks compare reconstructed behavior with the original game running in
an emulator, rather than only with original bytes and data files. They use an
emulator's GDB stub read-only: memory reads and a final detach, with no writes,
breakpoints or register changes.

## Boot-time resource state

[`scripts/check_live_boot_state.py`](../scripts/check_live_boot_state.py)
connects to the stub (default `127.0.0.1:24689`, as in Azahar), reads the state
left by the game's boot-time resource routines, and compares it with what the
reconstructed [resource](../reconstructed/pokemon_resources/README.md),
[archive-index](../reconstructed/archive_index/README.md) and
[message-construction](../reconstructed/message_construction/README.md)
milestones produce from the same original files.

```sh
python3 scripts/check_live_boot_state.py --json outputs/live-boot-check.json
```

The emulator pauses while the script is attached and resumes when it detaches.
The first run, on 2026-10-30 with Azahar and a game already past boot, matched
**20 of 20** checks:

| Check | Result |
| --- | --- |
| Running code at six addresses, including the boot routines, equals the pinned `code.bin` | match |
| Learnset, growth, word-3, word-4 and move archive member tables equal the reconstructed index builder's rows (799, 8, 799, 799 and 618 rows) | match |
| The personal and final-member archives have no resident table (event `-2`, count 0, table 0), as the reconstruction predicts | match |
| The loaded personal table (51,136 bytes) and final-member block (1,444 bytes) equal the flat final members | match |
| Growth cache vtable and capacity follow configuration word 8 | match |
| Four message resources have the reconstructed vtable, mode, member and section state | match |

The game's configuration table, which the earlier milestones could only infer,
names these archives:

| Word | Archive | Use |
| --- | --- | --- |
| 0 | `a/2/1/8` | personal table |
| 1 | `a/2/1/4` | learnsets |
| 2 | `a/2/1/7` | growth curves |
| 3 / 4 | `a/2/1/5` / `a/2/1/6` | the 0x30- and 0x18-byte record archives |
| 5 | `a/2/1/3` | not opened by the reviewed boot path |
| 6 | `a/2/1/9` | final-member table |
| 12 | `a/2/1/2` | moves |
| 14 / 15 | `a/2/2/0` / `a/2/2/1` | not opened by the reviewed boot path |
| 17 | `a/0/7/4` | message archive |

Capacities are 6, 6, 4, 4 and 4 (words 7–11) and 4 for the move cache. Message
members are `0x50`, `0x22`, `0x21` and `0x0e`. The GARC sub-entry selector at
`005d1124` was 2 and the message language word at `005d113c` was 0.

The resource milestones' sweeps already used this mapping. The milestone
documents that call words 3 and 4 stand-ins predate this check and are kept
unchanged with their recorded hashes.

## Call-by-call replay

[`scripts/capture_live_calls.py`](../scripts/capture_live_calls.py) sets software
breakpoints at reconstructed routine entries. For each call it records the
arguments and the memory the routine reads, then catches the matching return
(a temporary breakpoint at the return address with the same stack pointer) and
records the result and output memory. It removes every breakpoint before
detaching and never writes game memory. `--distinct` skips calls whose
arguments were already recorded.

[`scripts/replay_live_calls.py`](../scripts/replay_live_calls.py) converts the
captures, and [`scripts/replay_live_calls.c`](../scripts/replay_live_calls.c)
feeds the same inputs to the reconstructed routines and compares the outputs.
Growth selection and level resolution run through the reconstructed
`px_select_growth_resource` with archive reads served from the original curves.

```sh
python3 scripts/capture_live_calls.py --distinct --seconds 300 --output outputs/live-calls.jsonl
python3 scripts/replay_live_calls.py outputs/live-calls.jsonl outputs/live/calls.txt
# build replay_live_calls.c with the personal, growth, stats, data and constructor
# sources plus native/assets/garc.c (see the file header), then:
outputs/live/replay_live_calls inputs/romfs TABLE_ADDRESS outputs/live/calls.txt
```

On 2026-10-30, while the party menu, summary pages and wild battles (with
level-ups) were used in Azahar, four capture windows recorded 220 distinct
calls. Every replayed result matched the game:

| Routine | Calls | Match |
| --- | --- | --- |
| `00174c0c` personal record load (species 0, 653, 656) | 11 | 11 |
| `00175c78` personal field (21 selectors, including abilities 66/67/168) | 31 | 31 |
| `00167db0` growth selection (species 0, 653, 656) | 3 | 3 |
| `00168b3c` level from growth (species 0, 16, 653, 656, 659, 661; levels 1–7) | 12 | 12 |
| `00168b84` HP formula (species 0, 16, 656, 659, 661) | 8 | 8 |
| `0014ffa4`/`0014ffd4`/`0014ff74`/`00150004`/`00150034` stat formulas with natures 0, 1, 7, 11, 18 and 22 | 42 | 42 |
| `00168bbc` move field (moves 0, 1, 33, 145; base PP) | 4 | 4 |
| `00150220` maximum PP | 4 | 4 |
| `0013aa84` HP and `0014fb00`/`0014fbd4`/`0014fda4`/`0014fe78`/`0014fa2c` stat updates on 7 Pokémon objects (level-ups, modes 0 and 1) | 100 | 100 |
| `0014f2b8` learnset selection (species 0, 263, 656, 659, 661) | 5 | 5 |

Stat captures also record the nature row the routine reads at
`00570f6c + 5 × nature`; the replay places it at that address. Move fields and
maximum PP run through the reconstructed move-resource service with archive
reads served from `a/2/1/2`. Object updates are captured as before/after snapshots of the object, its
manager, the 0xe8-byte Pokémon data and the 0x1c-byte auxiliary block, plus the
nature table. The replay runs the reconstructed update on the entry snapshot,
with the reconstructed personal and growth services, and requires every exit
byte to match. In the game these data were stored unencrypted but
block-shuffled. The replay reads the block-permutation table (`00570eec`) and
nature table (`00570f6c`) from the pinned `code.bin`; the live nature table
equals that copy.

### Text loading and name building

[`scripts/replay_live_text.c`](../scripts/replay_live_text.c) replays captured
`0013a19c` message loads and `0014f224` name builds. For each call it opens the
captured archive file (the archive object's path at `+0x30`), constructs a
fresh message resource for the captured member, mode and language through the
reconstructed message-construction, archive-index and file-stream chain, then
loads the line or builds the name and compares metadata, stored length and
UTF-16 units. In a five-minute window of dialogue, menus and a battle, the game
called this loader only three times; both replayable calls (member 80 of
`a/0/7/4`, line 0, and the species-0 name build, both "Egg") matched. The third
used a resource without an archive. Most visible game text therefore goes
through other code, which is not yet reconstructed.

### Battle damage, call by call

[`scripts/trace_live_damage.py`](../scripts/trace_live_damage.py) traces the DllBattle
damage routine at its runtime address (static − `0x1015b000`), recording every
internal call's arguments and results. Two attacks in a wild battle, Thunder Shock
and Pound, replay through the reconstruction with identical calls (59 and 60),
arguments and damage (6 and 6). See
[battle_damage](../reconstructed/battle_damage/README.md).

Two early replay differences came from the harness, not the reconstruction.
Object updates first differed in 72 of 100 calls because the harness did not
supply the block-permutation table, so every block resolved to position zero;
supplying the table made all 100 match. An early replay also reported two level differences;
they came from the harness, which left the personal table unmapped when a
capture had no record loads, and disappeared once the table was always mapped.

Captures contain game data and stay in the ignored `outputs/` directory.

## Scope

These checks verify resulting state and the inputs/outputs of selected calls,
not each instruction's execution. They cover the routines and situations
exercised so far, not a fresh boot, other game builds or every input. The emulator and game copy are the user's own;
no game data is stored by the script beyond the comparison summary.
