# Battle move handlers: base power

This milestone reconstructs 33 of the 34 `DllBattle.cro` move handlers of
the base-power event `0x3c`, bound to 42 moves. Each one runs while the
power of the move being used is computed and replaces key `0x31` (power)
through the [event rewrite service](../battle_ability/README.md).
Twenty-nine were reviewed through live REA MCP/Ghidra listings; four have no
Ghidra function and are listed by the
[move-power milestone's decoder](../battle_power/README.md#decoder).
Eruption and Water Spout (`108adae8`) remain: they have no Ghidra function
and contain a multiply the decoder refuses.

| Group | Address (static) | Moves | Power |
| --- | --- | --- | --- |
| Doubled | `108ae96c` | Smelling Salts (265) | ×2 when the defender has status 1 (paralysis, `10856a88`) |
| | `108b6f0c` | Wake-Up Slap (358) | ×2 with status 2 (sleep) |
| | `108b0ee8` | Hex (506) | ×2 when the defender has any status (`1086d8ac`) |
| | `108af4e4`, `108b44c8` | Assurance (372), Payback (371) | ×2 when `1084c818(defender, 2)` or `(defender, 1)` reports |
| | `108b0f40` | Gust (16), Twister (239) | ×2 when the defender's state bit 3 is set (`1084ca4c`) |
| | `108af494` | Acrobatics (512) | ×2 with no held item |
| | `108b2a04` | Pursuit (228) | ×2 when `1084c020` reports a switching target |
| | `108b8090` | Weather Ball (311) | ×2 in any weather |
| | `108afb0c` | Revenge (279), Avalanche (419) | ×2 when a damage record names the defender with damage |
| | `108b53a0` | Round (496) | ×2 when more than one battler used it this turn |
| From the battlers | `108af994`, `108b32c4` | Return (216), Frustration (218) | Friendship (or 255 − it) × 10 / 25, at least 1 |
| | `108aedc0` | Punishment (386) | 60 + 20 per raised defender stage, at most 200 |
| | `108b33d0` | Stored Power (500) | Power + 20 per raised attacker stage; unchanged with none |
| | `108b05f4` | Flail (175), Reversal (179) | By 48 × HP / max HP, 200 down to 20 (table `108d494c`) |
| | `108b4250` | Wring Out (378), Crush Grip (462) | 120 × the defender's HP share / 100, at least 1 |
| | `108b03d4` | Gyro Ball (360) | 25 × defender speed / attacker speed + 1, at most 150 |
| | `108b164c` | Electro Ball (486) | Attacker / defender speed: 150, 120, 80, 60 or 40 |
| | `108b068c` | Low Kick (67), Grass Knot (447) | Defender weight: 120, 100, 80, 60, 40 or 20 |
| | `108b4d08` | Heavy Slam (484), Heat Crash (535) | Attacker / defender weight: 120, 100, 80, 60 or 40 |
| | `108b07c0` | Trump Card (376) | Remaining PP: 200, 80, 60, 50 or 40 (table `108d49ac`) |
| | `108b59a8` | Spit Up (255) | 100 per stockpile |
| | `108b8f8c`, `108bad24` | Natural Gift (363), Fling (374) | Held-item parameter 7 or 6 |
| | `108b8200` | Beat Up (251) | The next eligible party member's base Attack / 10 + 5 |
| From the history | `108b150c` | Echoed Voice (497) | Consecutive earlier turns using it: 40 up to 200 |
| | `108b5304` | Fury Cutter (210) | Doubled per consecutive use, at most 160 |
| | `108b5ea8` | Rollout (205), Ice Ball (301) | Power × 2^hits, ×2 after Defense Curl |
| | `108b47dc` | Triple Kick (167) | 10 per hit |
| | `108b6ecc` | Magnitude (222) | The power stored earlier, 10 when unset |
| | `108b52bc` | Present (217) | `random(80)`: below 40 power 40, below 70 power 80, else 120 |
| | `108b67b0` | Grass, Fire and Water Pledge (518–520) | 150 when work word 0 marks a combined pledge |

Runtime addresses are static − `0x1015b000`; move ids are the original table
ids, named after the public Generation VI list.

## Details

- Every handler first requires key `0x03` (attacker) to equal its battler id
  (a full-word compare). The defender is the battler named by the low byte
  of key `0x04`. Handler `+0x38` holds the move id.
- Doubling handlers rewrite `get(0x31) << 1`, so a power value wraps at 32
  bits. Key `0x31` is a value entry, which the rewrite service changes only
  once per frame: a later handler on the same frame cannot change it again,
  and Stored Power, which does not rewrite when no stage is raised, leaves
  it rewritable.
- **Punishment** adds `stage − 6` for each of stats 1–7 (table `108d2c3f`)
  whose stage is above 6 as a signed word; **Stored Power** reads stats
  1, 2, 5, 3, 4, 6, 7 (table `108d2c4d`) and adds `stage − 6` when that is
  positive as a signed word. The two tests differ only for words near
  `0x80000000`.
- **Return and Frustration** read friendship through `10801cf4` from the
  battler's core data pointer (`+0`); the division by 25 is the multiply by
  `0x51eb851f` and shift by 3 of the literal pool. **Wring Out** divides by
  100 the same way (shift 5), after `1089e5f8(120, share)`; **Beat Up**
  divides by 10 (`0xcccccccd`, shift 3).
- **Flail** and **Electro Ball** and **Heavy Slam** use the signed divide
  veneer `10800a34`; **Gyro Ball** the unsigned `1080041c`. Flail compares
  its ratio with the table unsigned, so a negative quotient gives 20.
- **Revenge** copies damage records `0, 1, 2, …` of the attacker
  (`1084bac4`, 8 bytes to a stack buffer; `+5` the battler that hit,
  `+2` the damage); the index wraps at 256.
- **Round and Echoed Voice** query the move history at context `+0x20` with
  the turn from `1084d364`; Echoed Voice counts back from the previous turn
  while the move was used, treating `turn − 1` as signed.
- **Fury Cutter** acts only when the last move (`1084c7fc`) is its own move
  and doubles once per use in the streak (`108c86c8`), stopping at 160 after
  a doubling.
- **Rollout** multiplies by 2 for an odd hit count and by 4 for each pair,
  doubles again with the attacker's state bit 7 (Defense Curl), then counts
  the hit in work word 0. When work word 1 is not greater (signed) than the
  new count, every registered move handler of this battler with the same
  move id is removed through the [registration
  milestone](../battle_handler/README.md); a running handler's removal is
  deferred by the dispatcher.
- **Beat Up** takes the party (`1084b950`), skips work word 0's low byte of
  eligible members (the user without checks; others that can battle,
  `108551d4`, and have no status), falls back to the user's battler, and
  counts work word 0 up to 6, where it restarts at 0. A member lookup that
  returns 0 still has its `+0x19` byte read, as in the original.
- **Heavy Slam** looks the attacker up first and discards the result.
- **Spit Up** multiplies the stockpile count's low halfword, signed, by 100
  (`smulbb`).

## Validation

`make -C reconstructed/battle_moves_1 test` runs every handler on the real
event stack: no action for another battler; each doubling condition and
32-bit wrap; Gust's state bits; Revenge's records, defender narrowing and
index wrap; friendship 0–319; every single raised stage for Punishment and
Stored Power, the cap, the signed tests and the stat orders; HP over nine
maxima for Flail and the signed divide; speeds and weights over grids with
negative values; every Trump Card slot and PP value; Spit Up's signed
product; item parameters; 15,625 Beat Up party patterns by count and skip,
call order and the zero-member path; Echoed Voice over every 8-turn history;
Fury Cutter streaks and caps; Rollout hit counts, Defense Curl and handler
removal with real registered handlers; Triple Kick, Magnitude, Present and
the pledges; and Rollout, Triple Kick and Hex through the real dispatcher
(397,349 cases). ASan/UBSan pass; 48 mutations are each detected (two
equivalent mutations were replaced).

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 33 handlers are bound to exactly 42 (move, event) pairs, all on
event `0x3c`, and that the other two move entries of that event are the
deferred Eruption/Water Spout handler
([record](reproduction/record-comparison.json)).

## Limits

Eruption and Water Spout are not reconstructed yet. The status, turn-flag,
friendship, damage-record, speed, weight, PP, history, switching, HP-share,
fixed-point, random, streak, stockpile, weather, party and item services
remain external. Division by zero (and `INT32_MIN / −1`) is outside the
host's domain; the original routes it to a main-program handler. No
running-game comparison has been made. Static evidence and host comparisons
do not establish a matching original build.
