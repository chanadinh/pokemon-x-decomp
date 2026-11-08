# Battle move handlers: accuracy, added effects and cleanup

This milestone reconstructs the 23 `DllBattle.cro` move handlers of four
events, bound to 43 (move, event) pairs, so every move entry of these events
is now reconstructed: the hit check `0xa5` (moves that reach a
semi-invulnerable defender), accuracy `0x37` (moves that cannot miss), the
added stat effect `0x62` and the end-of-turn removal `0x7f`. Nineteen were
reviewed through live REA MCP/Ghidra listings; four have no Ghidra function
and are listed by the [battle_power decoder](../battle_power/README.md#decoder).

| Event | Address (static) | Moves | Effect |
| --- | --- | --- | --- |
| `0xa5` hit | `108b0e58`, `108b97c0`, `108b9d28` | Sky Uppercut (327); Thunder (87), Smack Down (479), Hurricane (542), Thousand Arrows (614); Gust (16), Twister (239) | Key `0x43` = 0 against a defender in the air (key `0x22` = 3) |
| | `108b7b14` | Earthquake (89), Magnitude (222) | … underground (5) |
| | `108b8714`, `108b9b08` | Whirlpool (250); Surf (57) | … underwater (4) |
| | `108b9d64` | Heal Bell (215), Helping Hand (270), Aromatherapy (312) | Key `0x43` = 0 always |
| | `108b9cb0` | Rototiller (563), Flower Shield (579) | Key `0x50` = 0 always |
| `0x37` accuracy | `108ae714` | Blizzard (59) | Key `0x53` = 1 in hail |
| | `108bb520` | Thunder (87), Hurricane (542) | … in rain |
| | `108b76c8` | Pursuit (228) | … against a switching target (`1084c020`) |
| | `108bb1f0` | Stomp (23), Body Slam (34), Dragon Rush (407), Shadow Force (467), Heat Crash (535), Steamroller (537), Flying Press (560), Phantom Force (566) | … against a minimized defender (state bit 8) |
| `0x62` effect | `108afc4c` | Growth (74) | In sun, key `0x21` (stages) 1 becomes 2 |
| | `108b0e94` | Rototiller (563) | Key `0x20` (chance) = 0 against a non-Grass defender |
| | `108b1314` | Venom Drench (599) | … against an unpoisoned defender |
| | `108b3324` | Magnetic Flux (602) | … against a defender without Plus (57) or Minus (58) |
| | `108b69b8` | AncientPower (246), Silver Wind (318), Ominous Wind (466) | Key `0x20` = 10 for the effect on the user |
| `0x7f` cleanup | `108b8dcc`, `108b9dbc`, `108b9ee4`, `108ba38c` | Endure (203), Helping Hand (270), Snatch (289), Magic Coat (277) | The handler is removed when key `0x02` is its battler |
| | `108b8318` | King's Shield (588), Spiky Shield (596) | … when key `0x02` is `0x1f` |
| | `108bbc64` | Follow Me (266), Rage Powder (476) | Every move handler of its battler with its move id is removed |

Runtime addresses are static − `0x1015b000`; move ids are the original table
ids, named after the public Generation VI list.

## Details

- Every handler of events `0xa5`, `0x37` and `0x62` except the AncientPower
  family first requires key `0x03` (attacker) to equal its battler id; the
  AncientPower family tests key `0x04` first and then key `0x03`, so the
  chance applies only when the move's effect targets its user.
- Key `0x22` and key `0x02` are compared as full words.
- **Growth** rewrites key `0x21` in sun even when it is not 1 (with its own
  value), which uses up the value entry's single rewrite.
- **Stomp and the other minimize hitters** test state bit 8 of the defender
  (`+0x169`) through `1084ca4c`, reconstructed in the
  [modifier milestone](../battle_modifier/README.md).
- **Removal** goes through `1084d1cc` of the [registration
  milestone](../battle_handler/README.md); while the dispatcher runs a
  handler, its removal is deferred until it returns. **Follow Me** reads
  its own move id and walks `1084d3c0`/`1084d370` (owner 0, its battler),
  restarting after each removal, like Rollout in
  [battle_moves_1](../battle_moves_1/README.md).

## Validation

`make -C reconstructed/battle_moves_4 test` runs the handlers on the real
event stack and handler registry: no action for another battler; every
state 0–7 and a wide one for each hit handler; every weather and switching
value; Stomp over 24 state bits and defender narrowing; Growth over weathers
and stage values including the used-up rewrite; the three chance cancels
over types, statuses and abilities; the AncientPower key order including an
absent key `0x03`; every removal handler for four battler keys with a second
handler kept; Follow Me with handlers of other owners, battlers and move
ids (one sharing the low byte); and Endure, Helping Hand and King's Shield
through the real dispatcher (220 cases). ASan/UBSan pass; 31 mutations are
each detected.

[`reproduction/check_bindings.py`](reproduction/check_bindings.py) confirms
that the 23 handlers are bound to exactly 43 (move, event) pairs and that
every move entry of events `0x37`, `0x62`, `0x7f` and `0xa5` is one of them
([record](reproduction/record-comparison.json)).

## Limits

The kinds of keys `0x20`–`0x22`, `0x43` and `0x50` are taken as the tests
give them (ids for `0x02`–`0x04` and `0x22`, values otherwise); the routines
that dispatch events `0x37`, `0x62` and `0xa5` are not reconstructed yet. The
weather, switching, status and type services remain external. No
running-game comparison has been made. Static evidence and host comparisons
do not establish a matching original build.
