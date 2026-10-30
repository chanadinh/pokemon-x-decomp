Pokemon resource-initialization reproduction, 2026-10-30

Implemented: 00112368, 0011e884, 0011e960, 0011e9dc/0011eab0 (through the
archive-index opener), 00127ccc non-resident path, 00127bd8, 0011c3cc,
0011e864, 001228ec, 0012b554, 0012b6a4, 0012b5d4, 0012b610, 0012b650,
0012acd8, 0011ea34, 0011eb08, 0011eb84, 0011ec00, 0011ec7c and 00495d38.
Helpers 0012b094, 0011e264, 0012ad74, 0015e914 and 00301a9c are reviewed
dependencies.

  make -C reconstructed/pokemon_resources test

Original-byte/live-REA evidence verification:

  python3 reconstructed/pokemon_resources/reproduction/verify_rea.py \
    --code inputs/pokemon-x-0f6d3e1d66cb/code.bin

Optional original resource comparison, from the repo root (R=reconstructed):

  cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror \
    $R/pokemon_resources/reproduction/check_resources.c \
    $R/pokemon_resources/pokemon_resources.c $R/archive_index/archive_index.c \
    $R/archive_io/archive_io.c $R/pokemon_learnset/pokemon_learnset.c \
    $R/pokemon_growth/pokemon_growth.c $R/pokemon_personal/pokemon_personal.c \
    $R/pokemon_stats/stats_math.c $R/pokemon_data/pokemon_data.c \
    $R/pokemon_data/pokemon_codec.c $R/pokemon_data/pokemon_fields.c \
    $R/pokemon_constructor/pokemon_constructor.c \
    $R/pokemon_constructor/pokemon_parameters.c \
    native/assets/garc.c native/assets/pokedata.c native/platform/guest.c \
    -o $R/pokemon_resources/build/check_resources
  $R/pokemon_resources/build/check_resources ROMFS

The checker serves a/2/1/8, a/2/1/4, a/2/1/7, a/2/1/5, a/2/1/6 and a/2/1/9 as
configuration words 0, 1, 2, 3, 4 and 6, runs px_initialize_pokemon_resources
on guest memory, and compares the loaded personal table and final-member block
with independent readings. It then runs the reconstructed personal, growth and
learnset services for every species 1..721 and form, comparing with the native
decoders. No original payloads are retained here.
