# Battle status applier

This forty-ninth host-source milestone reconstructs `108629b8`, the
`DllBattle.cro` routine that the status kind routine
([battle_changes](../battle_changes/README.md), kind `0xb`) calls once a
status is not blocked. It was reviewed through a live REA MCP/Ghidra
listing.

| Step | Effect |
| --- | --- |
| Apply | `10865118` (context, target, status, word) |
| Animation | Statuses 1–7 send command `0x57` with their animation for a positioned battler. The ids come from an inner jump table (`108629fc`, 8 relocated words) and literals: 1 `0x299`, 2 `0x295`, 3 `0x298`, 4 `0x297`, 5 `0x296`, 6 `0x29a`, 7 `0x29b`; 0 and 8 and up have none |
| Message | With *build* set, a default message from `1087afa0` in the context's buffer (`+0x433c`, cleared after); otherwise the record's message, if any |
| Shaymin | Freezing (status 3) Shaymin (species 492) in its Sky Forme (form 1) reverts it: `108679ac`, `1086fff0(…, 0)`, command `0x5b`, `108678f8`, message `0xde` |
| Event | Statuses below 6: event `0x71` (keys 4, 3 source or `0x1f`, `0x1e`, `0x1f` word). Status `0x10`: event `0x73` inline without the blocked check, then `10864dc0` for ability value `0x7f`. Others: event `0x72` (keys 4, 3, `0x1e`) |
| Item | Unless suppressed, a held item reacts (event `0x9c`, key `0x2f` = 4) |

Runtime addresses are static − `0x1015b000`. Nested events keep the
action-stack word. The inline dispatch reaches the battle state through the
relocated literal `10862e04` (→ `108fca00`).

## Validation

`make -C reconstructed/battle_conditions test` covers 1,666 cases on the
real event stack. They run 13 statuses over 128 combinations of position,
message source, item, source battler and ability, plus Shaymin in both
forms. Each case checks the full call sequence, the stack word, the message
buffer and the cleared registration depths. ASan and UBSan pass, and each
of 18 mutations is detected. One equivalent mutant was replaced.

[`reproduction/check_conditions.py`](reproduction/check_conditions.py)
decodes the original jump table and literals, and confirms they give
exactly the C animation table. It also confirms that the status kind
routine passes the applier's eight arguments
([record](reproduction/record-comparison.json)).

## Limits

The callees named above remain external. No running-game comparison has
been made. Static evidence and host comparisons do not establish a matching
original build.
