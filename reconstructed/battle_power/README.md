# Battle ability handlers: move power

This twenty-seventh host-source milestone reconstructs all nineteen
`DllBattle.cro` ability handlers on the move-power event `0x3d`. Each one
multiplies the power multiplier (key `0x32`) with the
[event multiply service](../battle_ability/README.md). Fifteen were reviewed
through live REA MCP/Ghidra listings; four have no Ghidra function and are
listed by a small decoder that reproduces every REA listing so far.

| Address (static) | Host routine | Ability | Effect |
| --- | --- | --- | --- |
| `108bf95c` | `px_battle_rivalry` | Rivalry (79) | Same gender ×1.25, opposite ×0.75, none if either is genderless |
| `108c021c` | `px_battle_technician` | Technician (101) | Power 60 or less ×1.5 |
| `108c12d4` | `px_battle_iron_fist` | Iron Fist (89) | Move flag 7 (punch) ×1.2 |
| `108bd2dc` | `px_battle_reckless` | Reckless (120) | Recoil moves, Jump Kick, High Jump Kick ×1.2 |
| `108c34f4` | `px_battle_heatproof` | Heatproof (85) | Defender: Fire ×½ |
| `108c4244` | `px_battle_dry_skin_power` | Dry Skin (87) | Defender: Fire ×1.25 |
| `108c4708` | `px_battle_sheer_force` | Sheer Force (125) | Moves accepted by `1084a890` ×1.3 |
| `108bfc9c` | `px_battle_toxic_boost` | Toxic Boost (137) | Poisoned: physical ×1.5 |
| `108c00b8` | `px_battle_flare_boost` | Flare Boost (138) | Burned: special ×1.5 |
| `108bd200` | `px_battle_analytic` | Analytic (148) | Moving last ×1.3, unless `1084c808` excludes the move |
| `108c1244` | `px_battle_sand_force` | Sand Force (159) | Sandstorm: Ground, Rock, Steel ×1.3 |
| `108be18c` | `px_battle_dark_aura` | Dark Aura (186) | Any Dark move ×1.33, once; ×0.75 with Aura Break |
| `108bf060` | `px_battle_fairy_aura` | Fairy Aura (187) | Any Fairy move ×1.33, once; ×0.75 with Aura Break |
| `108bf148` | `px_battle_strong_jaw` | Strong Jaw (173) | Biting moves ×1.5 |
| `108bf344` | `px_battle_tough_claws` | Tough Claws (181) | Move flag 0 (contact) ×1.3 |
| `108c0a24` | `px_battle_aerilate` | Aerilate (184) | Converted Flying moves ×1.3 |
| `108c15a8` | `px_battle_pixilate` | Pixilate (182) | Converted Fairy moves ×1.3 |
| `108c15f8` | `px_battle_refrigerate` | Refrigerate (174) | Converted Ice moves ×1.3 |
| `108c32e4` | `px_battle_mega_launcher` | Mega Launcher (178) | Water Pulse, Dark Pulse, Dragon Pulse, Aura Sphere ×1.5 |

Runtime addresses are static − `0x1015b000`. Ability numbers are the original
table ids; names follow the public Generation VI list. The ×1.2, ×1.3 and
×1.33 factors are literal words `0x1333`, `0x14cd` and `0x1548`; the others
are immediates.

## Details

- Every handler except the Auras first requires key `0x03` (attacker) or
  `0x04` (defender) to be its battler id. Move ids are key `0x12` narrowed to
  16 bits. Heatproof, Dry Skin, the Auras and the three converting abilities
  compare the full type word; Sand Force uses its low byte.
- **Rivalry** compares the low bytes of value `0x12` (gender) of the attacker
  and of the battler named by the low byte of key `0x04`.
- **Technician** compares key `0x31` (power) unsigned with 60.
- **Reckless** asks `10800494(move, 0x1e)` (recoil) before testing Jump Kick
  (26) and High Jump Kick (136).
- **Dark Aura and Fairy Aura** run for every battler that has them. Each
  rewrites key `0x58` to 1 first; because key `0x58` is a value entry it can
  be rewritten only once, so a second aura handler in the same frame changes
  nothing. A nonzero key `0x57` (Aura Break) turns ×1.33 into ×0.75.
- **Aerilate, Pixilate and Refrigerate** require the converted type and a
  nonzero first work word, which their event `0x28` entries set when they
  converted a Normal move.
- **Analytic** asks `1084c808` first and `1084b8e0(context, id)` (moved last)
  only when the move is not excluded; **Sand Force** asks the weather
  (`1084be58`, 4 is sandstorm) before reading the type.

## Decoder

[`reproduction/decode_arm.py`](reproduction/decode_arm.py) decodes the A32
forms these handlers use in Ghidra's spelling and refuses anything else. Over
all 69 functions listed by REA in the four battle sessions it reproduces
3,230 of 3,230 supported instructions (multiplies are among the 47 it
refuses). The verifier requires it to reproduce every listed instruction of
this milestone and the complete listing of the four decoded handlers.

## Validation

`make -C reconstructed/battle_power test` runs the handlers on the real event
stack: the side check of every handler; every gender pair; power 0–199 and a
wide value; move flags, recoil and each special move id; Fire and wide types;
every status and category; Analytic's call order; weather and types for Sand
Force; both Auras with and without Aura Break and the refused second
application; the converted types with work words; every move id up to 1023
for Mega Launcher; and four handlers through the real dispatcher (2,767
cases). ASan/UBSan pass; 24 mutations are each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that each handler is bound to exactly its ability, that no item, move or
field row uses it, and that these are all nineteen ability entries for event
`0x3d` in the original table ([record](reproduction/record-comparison.json)).

## Limits

The move flag, move value, Sheer Force, status, Analytic, turn-order, weather
and biting services remain external. The other entries of these abilities
(for example Dry Skin's water absorption or the type conversion itself) are
not part of this milestone. No running-game comparison has been made. Static
evidence and host comparisons do not establish a matching original build.
