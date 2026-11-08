# Battle move-calculation abilities

This milestone reconstructs the fourth batch of the remaining
`DllBattle.cro` ability handlers: 27 small handlers that adjust a value a
move's calculation reads — accuracy, stat stages, critical hits, speed and
order, weight, recoil, hit counts and effect chances. Twenty-four were
reviewed through live REA MCP/Ghidra listings; three have no Ghidra function
and are listed by the [move-power milestone's decoder](../battle_power/README.md#decoder).

| Event | Address (static) | Host routine | Abilities | Effect |
| --- | --- | --- | --- | --- |
| `0x39` | `108bd7cc` | `px_battle_compound_eyes` | Compound Eyes (14) | Attacker: accuracy ×1.3 |
| `0x39` | `108be2c0` | `px_battle_victory_star` | Victory Star (162) | Attacker or its ally (`1084c5a8`): ×1.1 |
| `0x39` | `108c3ab8` | `px_battle_hustle_accuracy` | Hustle (55) | Attacker's physical moves: ×0.8 |
| `0x39` | `108bf910` | `px_battle_tangled_feet` | Tangled Feet (77) | Confused defender: ×0.5 |
| `0x39` | `108c0764` | `px_battle_wonder_skin` | Wonder Skin (147) | Status moves at the defender: accuracy key `0x2c` capped at 50 |
| `0x38` | `108c3d64` | `px_battle_keen_eye` | Keen Eye (51) | Attacker: key `0x4c` = 1 |
| `0x38` | `108c23f4` | `px_battle_unaware_stages` | Unaware (109) | Attacker: key `0x29` = 6; defender: key `0x28` = 6 |
| `0x3e` | `108c239c` | `px_battle_unaware_defense` | Unaware (109) | Defender: key `0x53` = 1 |
| `0x3f` | `108c23c8` | `px_battle_unaware_attack` | Unaware (109) | Attacker: key `0x53` = 1 |
| `0x44` | `108bf554` | `px_battle_scrappy` | Scrappy (113) | Attacker, key `0x15` = 7: key `0x4c` = 1 |
| `0x3b` | `108bd17c` | `px_battle_super_luck` | Super Luck (105) | Attacker: critical stage key `0x2d` + 1 (byte) |
| `0x3b` | `108c0700` | `px_battle_battle_armor` | Battle Armor (4), Shell Armor (75) | Defender: key `0x42` = 1 |
| `0x0f` | `108bd6b4` | `px_battle_stall` | Stall (100) | Key `0x11` = 0 |
| `0x13` | `108bd8a8` | `px_battle_quick_feet` | Quick Feet (95) | With a status: Speed ×1.5 and key `0x53` = 0 |
| `0x13` | `108c31ac` | `px_battle_unburden` | Unburden (84) | Work word 1 and no held item: Speed ×2 |
| `0x13` | `108c3cd0` | `px_battle_slow_start_speed` | Slow Start (112) | Work word `+0xc` set: Speed ×0.5 |
| `0xa8` | `108c551c` | `px_battle_unburden_item` | Unburden (84) | Key `0x2e` clear and an item held: work word = 1 |
| `0x84` | `108bfde4` | `px_battle_heavy_metal` | Heavy Metal (134) | Weight ×2 |
| `0x84` | `108bfed8` | `px_battle_light_metal` | Light Metal (135) | Weight ×0.5 |
| `0x89` | `108bff04` | `px_battle_magic_guard` | Magic Guard (98) | Defender: key `0x53` = 0 |
| `0x58` | `108be4ec` | `px_battle_rock_head` | Rock Head (69) | Attacker: key `0x42` = 1 |
| `0x3a` | `108bf8cc` | `px_battle_skill_link` | Skill Link (92) | Attacker: key `0x53` = 1 |
| `0x3a` | `108bddcc` | `px_battle_parental_bond` | Parental Bond (185) | A single-hit move strikes twice: key `0x2b` = 2 |
| `0x49` | `108c0a74` | `px_battle_adaptability` | Adaptability (91) | Key `0x45` set: key `0x36` rewritten to ×2 |
| `0x76` | `108c09f8` | `px_battle_inner_focus` | Inner Focus (39) | Defender: key `0x42` = 1 |
| `0x6d`, `0x59`, `0xb5` | `108c0ab0` | `px_battle_serene_grace` | Serene Grace (32) | Attacker: chance key `0x27` doubled (halfword) |
| `0x75` | `108c46dc` | `px_battle_serene_grace_flinch` | Serene Grace (32) | Attacker: key `0x46` = 1 |

Runtime addresses are static − `0x1015b000`; ability numbers are the
original ids, named after the public Generation VI lists.

## Details

- Each handler first checks one battler key: `0x02` the battler concerned,
  `0x03` the attacker or `0x04` the defender. Multipliers go to key `0x36`
  through `1084ce84` (literals `108bd7f8` = `0x14cd`, `108be2f8` =
  `0x119a`, `108c3afc` = `0xccd`); other effects rewrite a key through
  `108658d4`. Adaptability instead rewrites key `0x36` itself, which is a
  plain value on event `0x49`.
- Hustle checks category 1 of key `0x12`'s low half (`108007dc`); Tangled Feet
  status 6 (`10856a88`); Quick Feet any status (`1086d8ac`). Scrappy compares
  the whole key `0x15` word with 7. Wonder Skin needs key `0x1b` = 0 and
  compares key `0x2c` unsigned with 50.
- Super Luck adds one and keeps the low byte; Serene Grace shifts key `0x27`
  left and masks it with `0xffff` (literal `108c0ae8`).
- Parental Bond needs key `0x2a` = 1 and key `0x4e` clear, and the move
  neither in the list of `108a4814` nor with move flag 1 (`1080049c`).
- Unburden's two handlers share its work word: event `0xa8` stores 1 when an
  item is held (`+0x12`), and the speed handler needs exactly 1 and no item.

## Validation

`make -C reconstructed/battle_abilities_4 test` runs every handler against a
reference model over all eight combinations of the three battler keys and
every combination of the inputs it reads (categories, statuses, ally, move
list and flag, held item, work words, and each key it consults, including
high-bit words), each repeated with three random settings of the inputs it
does not read; it checks the call log, key `0x36`, every rewritable key and
the work word; plus Compound Eyes then Victory Star through the real
dispatcher (2,657 cases). ASan/UBSan pass; 34 mutations are each detected by a
failing test (three that did not compile were replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 27 handlers are bound to exactly 30 (ability, event) pairs and that
the only other ability entries of events `0x39`, `0x38`, `0x3b` and `0x84` are
Sand Veil and Snow Cloak from battle_abilities_2
([record](reproduction/record-comparison.json)).

## Limits

The category, status, ally, move-list and move-flag services remain external.
No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
