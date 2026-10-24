Native NPC dialogue: story text, zone scripts and a message window, 2026-10-24

Talking to an NPC in --zone now runs the zone's own script and shows the lines
it selects in a message window. The house NPC (zone 4) greets the player with
the game's text. Tracked files contain independently authored source,
synthetic tests and metadata only; the documentation PNG is recorded in
docs/images/README.md.

Native preview

native/build/pokemon_model_viewer --romfs /path/to/romfs --zone 5

Walk down the stairs, up to the NPC and press Space; Space turns the pages.
The player cannot walk while the window is open.

Story text (assets/gf_text.h)

a/0/8/2 holds the English story text (pk3DS 6daaca9 GARCReference: story text
is a/0/8/0 plus the language index, English 2; TextFile.cs gives the layout
and the line keys 0x7C89 + i * 0x2983, rotated left by 3 per unit). The zone
header's u16 at offset 6 names a zone's text file (zone 4: entry 53). All 424
files (7,607 lines) decode and render. Rendering: '\n' between lines, a page
break at the window codes 0xBE00/0xBE01, the player's name for TRNAME
(variable 0x0100; the viewer uses "Player"), [NAME] for other variables
(names from pk3DS TextVariableCode.cs), colour codes dropped.

Zone scripts (assets/gf_script.h)

ZO section 2 is a Pawn 3.3 abstract-machine image: the AMX header (magic
0xF1E0, file and AMX version 10, definition size 8, cod/dat/hea/stp/cip and
the publics/natives/libraries/pubvars/tags tables) with 32-bit name hashes
instead of name offsets, and code plus initial data in Game Freak's 7-bit
word encoding (pk3DS Script.cs / FireyFly poketools 0dce8f0 script.c). The
opcode numbering matches the Pawn 3.3 enumeration of compuphase/pawn 6d82fa4
(amx/amx.c), packed opcodes included: e.g. 0xBC PUSH_P_C, 0x87 SYSREQ_N,
0x81 SWITCH, 0xD2 HALT_P; HALT 12 is Pawn's AMX_ERR_SLEEP, a yield. The
interpreter is independently written from those semantics (position-
independent jumps: target = opcode address + operand).

Each zone script's entry point switches on the public variable 0xD7477C97;
the case equal to an NPC's script number is its event (zone 4: case 1 calls a
function that reads work 16500 and shows line 2 or 3 if it equals 3, else
lines 0 or 1, the pair chosen by native 0x59008363).

Native policy (assets/zone_talk.h; inferred from the scripts, not traced in
the game's code):
  0x9ADF1616  message window, first argument = line of the zone text; every
              call reached in the 2,524 NPC runs names an existing line
  0x59008363  player is female; 1 (the player model tr0002 is female; the
              house lines 0/1 differ exactly by "young man"/"young lady")
  0x8E50F765  read a work value; 0 (a new game)
  0x15F86E59, 0xC8E7FB61  polled in wait loops after actor/message commands;
              1 (done)
  others      0
Runs stop at the final HALT, 200,000 instructions or 500 yields.

Result over every NPC of every zone (fresh-game policy): 2,213 runs halt, 311
reach the yield limit (wait loops on other natives); 1,183 NPCs show at least
one line (1,282 lines). NPCs whose events depend on story state, battles or
other natives show nothing yet.

Message window (render/text.h)

A framed two-line window over the bottom of the 400 x 240 frame, a
self-authored 5 x 7 bitmap font (render/text_glyphs.inc: ASCII and the
accented letters, quotes, ellipsis and symbols of the English story text) and
an arrow when more pages follow. Not the game's font or window art: the game
uses the console's shared system font, which is not in the RomFS.

Independent comparison (verify_talk.py)

cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror native/assets/gf_script.c \
  native/assets/gf_text.c native/assets/zone_talk.c \
  native/reproduction/bch-talk/dump_talk.c -o native/build/dump_talk
python3 -I -B native/reproduction/bch-talk/verify_talk.py --romfs /path/to/romfs \
  --bridge native/build/dump_talk --viewer native/build/pokemon_model_viewer \
  --output-dir outputs/new-talk-check

It decodes and renders all 424 text files itself (all 7,607 lines identical
to the native renderer), decodes every zone script itself and runs every
NPC's event on its own Pawn interpreter with the same policy: result,
instruction count, yields, native calls and lines identical for all 2,524
NPCs, every line within its zone's text. The viewer's house talk produces the
three pages of zone 4 line 1 exactly as rendered independently; capture hash
in reference-comparison.json.

Effect on the zone record: ../bch-zone/verify_zone.py still reproduces its
report except the frame-519 capture of its --play run, which now shows the
open window (7de47c74...; 6245c39c... at commit 2fea62c).

Tests

assets/test_gf_text.c: an encrypted hand-built file (page break, accent,
TRNAME, an unknown variable, the empty-line marker), rendering limits and
rejections. assets/test_gf_script.c: a hand-assembled image (switch with a
default, call with a frame argument, natives, two yields, floored division)
run through the interpreter and the talk policy, faults (store outside the
data segment, native index, unknown opcode), budget and yield limits, and
header rejections. tests/test_bch_talk_viewer.py: the synthetic zone RomFS
with a hand-assembled script and text file: inspect lines, the two pages of
a talk, the window drawn and closed, no walking while open, and a zone
without its text file.

Limits

Fresh-game policy only: no save data, story flags or work values; natives
other than the five above do nothing, so actor animation, yes/no choices,
item gifts, battles and cutscenes do not happen. One language (English).
The player's name is a placeholder.
