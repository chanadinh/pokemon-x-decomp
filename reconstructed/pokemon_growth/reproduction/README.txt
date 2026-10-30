Growth-resource reproduction, 2026-10-30

Four Ghidra bodies, five host routines: 00167db0 (with the tail-called store
00174cac in its body), 00175fec, 00174dd8 and 0017c5b0. Helpers 00144f34
(word-aligned copy) and 0015e958 (unsigned division) are reviewed
dependencies. Archive access 00138214, allocation and initialization remain
external. The selector calls the reconstructed personal-data services.

  make -C reconstructed/pokemon_growth test

Original-byte/live-REA evidence verification:

  python3 reconstructed/pokemon_growth/reproduction/verify_rea.py \
    --code inputs/pokemon-x-0f6d3e1d66cb/code.bin

Optional original resource comparison, from the repo root:

  cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror \
    reconstructed/pokemon_growth/reproduction/check_growth.c \
    reconstructed/pokemon_growth/pokemon_growth.c \
    reconstructed/pokemon_personal/pokemon_personal.c \
    reconstructed/pokemon_stats/stats_math.c \
    reconstructed/pokemon_data/pokemon_data.c \
    reconstructed/pokemon_data/pokemon_codec.c \
    reconstructed/pokemon_data/pokemon_fields.c \
    reconstructed/pokemon_constructor/pokemon_constructor.c \
    reconstructed/pokemon_constructor/pokemon_parameters.c \
    native/assets/garc.c native/assets/pokedata.c native/platform/guest.c \
    -o reconstructed/pokemon_growth/build/check_growth
  reconstructed/pokemon_growth/build/check_growth ROMFS

The checker reads the 799 personal rows from a/2/1/8 and the eight 404-byte
growth curves from a/2/1/7, mapping the table on guest memory. For every
valid and boundary species/form it calls the reconstructed selector through
a three-slot cache, compares the selected record with the curve named by the
independently decoded personal growth byte, checks thresholds at levels
0..101, and compares px_pokemon_level_from_growth with an independent scan at,
below and above every threshold. No original payloads are retained here.
