The platform layer: what the 3DS provided, 2026-10-25

native/platform/ stands in for the console's services, so the game's data
and the reconstructed code run on a PC with the 3DS's own assumptions kept.
It is a replacement, not an emulator: nothing executes 3DS machine code.
Tracked files contain independently authored source, synthetic tests and
metadata only.

Pieces

- guest.h: the game's memory as it sees it. A 32-bit address space, values
  little-endian assembled byte by byte whatever the host's order, pointers
  32-bit guest addresses (never host pointers), regions with read / write /
  execute permissions, and ARM11 alignment rules (strict by default; the
  ARMv6 unaligned model as an option). A bad access is recorded, not fatal:
  loads give 0, stores are dropped. The process image maps code.bin as the
  exheader places its segments: text 0x00100000 (r-x, 0x43c pages), rodata
  0x0053c000 (r--, 0x6a pages), data and bss 0x005a6000 (rw-), the heap at
  0x08000000, the stack ending at 0x10000000 (3dbrew's application layout).
  The reconstructed stats, levels and party-record codec now reach memory
  only through it: the nature table is read at 0x00570f6c and the record
  block order at 0x00570eec in rodata, the growth table through the data
  segment's pointer at 0x005d17ac, records in the heap.
- fs.h: archives instead of host paths. RomFS (read-only), ExeFS inputs
  (code.bin, exheader.bin; read-only) and the save archive (read/write) are
  mounted on host directories; archive paths cannot leave their archive; a
  write replaces the file atomically (temporary file, then rename). Every
  RomFS, code.bin and save access of the viewer goes through it.
- hid.h: the pad as the game reads it. HID PAD bits after 3dbrew (A, B,
  Select, Start, D-pad, R, L, X, Y, ZL, ZR, touch, circle-pad directions),
  the circle pad as a clamped position with direction bits past a threshold,
  touch in bottom-screen pixels; per frame held / down / up, and a press
  inside one frame still counts. Keyboard and mouse map onto it (WASD the
  D-pad, Space/Return A, X/Backspace and Shift B: B held runs). --play
  scripts are pad input (u/d/l/r: D-pad with B; '.' A; 'x' B).
- clock.h: the system tick (268,111,856 Hz) and the 60 Hz frame. Runs with
  --frames use a fixed clock (exactly frame x 1000 / 60 ms, as before), so
  they repeat; live runs pace to 60 Hz on the host's monotonic clock.
- thread.h: threads (priority and core recorded), mutexes, one-shot and
  sticky events, semaphores, on SDL. The renderer's row bands use them.
- audio.h: a 24-voice mixer at the DSP rate (32,728 Hz) for PCM8, PCM16 and
  DSP-ADPCM with loops and stereo volume, linear resampling, clamping;
  audio_sdl.c plays it on the host device. A BCWAV reader after 3dbrew.
  --sound PATH@OFFSET plays a BCWAV from a RomFS file and --list-sounds PATH
  lists the BCWAVs inside one (a BCSAR, say). Not yet checked against the
  game's sound files: the uploaded RomFS has no sound/ folder.
- The two screens: --bottom-screen adds the 320 x 240 touch screen below the
  top one (a 400 x 480 logical window). In battle it carries the commands as
  buttons, as the game does, which taps choose (--touch-play F:X,Y;... for
  scripted taps); in the field it shows the party. Its layout is the
  viewer's own.

Graphics: the renderer was already a software replacement of the PICA200's
fragment pipeline (texture units, six-stage combiner, fragment lighting,
blending). pica_shader.h adds the vertex shader: a DVLB reader (the shared
DVLP program and its DVLE entry points, with output maps, constants and
named uniforms) and an interpreter for the instruction set as 3dbrew
documents it (arithmetic, DP3/DP4/DPH, EX2/LG2, RCP/RSQ, MOVA with relative
addressing, CMP, IF/CALL/JMP/LOOP/BREAK with nesting), float24 constants
widened exactly and 0 x anything = 0 as on the hardware. The viewer does not
use it yet: it still transforms and skins on the CPU.

verify_shaders.py surveys the Pokémon models' programs (a/0/0/7): 977 models
with geometry (4 are empty) hold 8,023 shader resources, which are 8
distinct DVLBs. All decode; their 11 vertex-shader entries run to END under
256 combinations each of their first eight bool uniforms (2,816 runs; inputs
and matrices are placeholders, so this shows coverage, not correct output).
Their 3 geometry-shader entries stop at EMIT/SETEMIT, which is not
implemented. reference-comparison.json has the hashes and the probe output.

Behaviour changes

- The pad script's 'x' in the field is B and moves nobody; before, it was
  read as a direction and turned the player to face left.
- In the field X/Backspace now also run (B held), as Shift does.
- Field-scene output is otherwise unchanged (below).

Unchanged (this viewer against earlier records)

- ../bch-route/verify_route.py: 328 of 360 zones load, 2,864 props, 2,217
  NPC tiles, 593 warp tiles, both crossings, and all four captures
  byte-identical to ../bch-battle/route-recheck.json.
- ../bch-catch, ../bch-save, ../bch-party verifiers pass; ../bch-battle-motion
  passes; ../bch-pokemon and ../bch-battle pass their table steps (their
  runtime steps differ by design since later records, as those note).
- The bridges of earlier records that link game/battle.c now also need
  native/platform/guest.c (and -lm), e.g. for ../bch-catch:
  cc -std=c11 -O2 native/reproduction/bch-catch/dump_catch.c native/assets/garc.c \
    native/assets/lz.c native/assets/pokedata.c native/game/battle.c \
    native/platform/guest.c reconstructed/pokemon_stats/stats_math.c \
    reconstructed/pokemon_data/*.c reconstructed/pokemon_constructor/*.c -lm \
    -o native/build/dump_catch   (sources other than tests)

Tests

platform/test_guest.c, test_fs.c, test_hid_clock.c, test_audio.c,
test_pica_shader.c (float24, every instruction format, flow control,
relative addressing, a DVLB round trip), test_thread.c (SDL), game/test_battle.c (taps on the bottom screen),
tests/test_bch_platform_viewer.py (the process image with and without an
exheader, the atomic save, two screens and scripted taps, BCWAV listing and
playback on SDL's dummy audio driver, the pad script).

Run

python3 -I native/reproduction/platform/verify_shaders.py --romfs ROMFS \
  --probe native/build/pica_probe --out OUT   (OUT receives game DVLBs: keep
  it out of the repository)
