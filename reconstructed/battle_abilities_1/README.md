# Battle contact abilities

This milestone reconstructs the first batch of the remaining
`DllBattle.cro` ability handlers: 14 handlers of event `0x53`, which runs
after a move has hit, together with three helpers they share. All 17
functions were reviewed through live REA MCP/Ghidra listings. Cursed Body
and Illusion are the other two ability handlers of event `0x53`; they are
not reconstructed yet.

| Address (static) | Host routine | Abilities | Effect |
| --- | --- | --- | --- |
| `1084ad50` | `px_battle_contact_status` | (helper) | A contact move hitting the battler gives the attacker a status with a percent chance |
| `1085f844` | `px_battle_fainted` | (helper) | No HP left |
| `1084d2f4` | `px_battle_hp_share` | (helper) | Max HP / divisor, at least 1 |
| `108be9d0` | `px_battle_static` | Static (9) | Paralysis, 30% |
| `108bfcf8` | `px_battle_poison_point` | Poison Point (38) | Poison, 30% |
| `108c0edc` | `px_battle_flame_body` | Flame Body (49) | Burn, 30% |
| `108bc8dc` | `px_battle_effect_spore` | Effect Spore (27) | Poison, paralysis or sleep, 30%; not Grass, Overcoat or Safety Goggles |
| `108c0f60` | `px_battle_cute_charm` | Cute Charm (56) | Infatuation, 30%, opposite genders only |
| `108bd6e0` | `px_battle_poison_touch` | Poison Touch (143) | The attacker's contact move poisons the defender, 30% |
| `108be8f8` | `px_battle_rough_skin` | Rough Skin (24), Iron Barbs (160) | The attacker loses 1/8 of its max HP |
| `108bde94` | `px_battle_aftermath` | Aftermath (106) | When this battler fainted, the attacker loses 1/4 |
| `108bc9a8` | `px_battle_mummy` | Mummy (152) | The attacker's ability becomes Mummy |
| `108be5f8` | `px_battle_gooey` | Gooey (183) | The attacker's Speed −1 |
| `108bcd28` | `px_battle_rattled` | Rattled (155) | Dark, Bug or Ghost move: Speed +1 |
| `108c19bc` | `px_battle_justified` | Justified (154) | Dark move: Attack +1 |
| `108c05ec` | `px_battle_anger_point` | Anger Point (83) | A critical hit maximises Attack |
| `108c1784` | `px_battle_weak_armor` | Weak Armor (133) | A physical hit: Defense −1, Speed +1 |

Runtime addresses are static − `0x1015b000`; ability and item numbers are
the original ids, named after the public Generation VI lists.

## Details

- Key `0x04` is the battler hit and key `0x03` the attacker; key `0x47`
  suppresses every after-effect here except Aftermath's, key `0x48` also
  Poison Touch's. Contact is move flag 0 (`1080049c`) of key `0x12`'s low
  half.
- **The contact-status helper** `1084ad50(context, id, status, condition,
  percent)` (the fifth argument on the stack) rolls `1084c0f8(percent)`,
  falling back on `1084c0ec(context, 1)`, and commits a kind-`0xb` record:
  `+4` status (halfword), `+8` condition word, `+0xc` cleared, `+0xf` the
  attacker, flag `0x800000`. Static, Poison Point and Flame Body ask
  `10860e00` for the condition before anything else; Cute Charm takes it from
  `1084c438(7, pokemon, out)` in a stack word.
- **Effect Spore** rolls `1084c7bc(30)`: above 20 poison, above 10
  paralysis, otherwise sleep. It is skipped for a Grass attacker (type 11),
  effective ability Overcoat (142) and held item Safety Goggles (650,
  through `1084ae18`).
- **Rough Skin and Aftermath** commit kind-`0x6` records (`+6` the attacker,
  `+4` HP, messages `0x1ae` and `0x192`); Aftermath sets `+7` bit 0 and does
  not check key `0x47`.
- **Mummy** (kind `0x20`, `+4` ability 152, message `0x1cf`) checks the
  attacker's ability key `0x10` and `10865c48` (the battle is not decided);
  the record is flagged only when `1084c5a8` reports the two are not allies.
- **Stat records** are kind `0xd` (`+4` stat, `+0xc` battler, `+0x16`
  count 1, `+0x17` signed stages, `+0x18` flags). Justified compares the
  whole type word with Dark (16), Rattled only its low byte. Anger Point asks
  `1084b82c` for the stages left twice, before and after the record. Weak
  Armor's two records inside the ability popup are not flagged.

## Validation

`make -C reconstructed/battle_abilities_1 test` covers every early return
of every handler over the battler, key `0x47`/`0x48`, contact, chance and
forced combinations; Effect Spore's immunities and all 30 rolls; Cute Charm
over nine gender pairs; Rough Skin and Aftermath over max HP values;
Mummy's ally flag; Rattled and Justified over every type byte and a high
bit; Anger Point over −2 to 7 stages left; Weak Armor over four categories
and both stage answers; the helpers directly; and Static then Rough Skin
through the real dispatcher (3,977 cases). ASan/UBSan pass; 34 mutations are
each detected (one equivalent mutation was replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 14 handlers are bound to exactly 15 (ability, event) pairs, that the
three helpers are bound to nothing, and that the other ability entries of
event `0x53` are exactly the two deferred handlers
([record](reproduction/record-comparison.json)).

## Limits

Cursed Body and Illusion are not reconstructed yet. The chance, random,
condition, item, type, ally, stage, popup, message and action services remain
external. No running-game comparison has been made. Static evidence and host
comparisons do not establish a matching original build.
