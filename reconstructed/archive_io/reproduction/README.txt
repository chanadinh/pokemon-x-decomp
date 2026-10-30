Archive-service reproduction, 2026-10-30

Nine implemented functions: 00138214, 001381c4, 00127c5c, 00127b6c, 00127ccc
(resident path), 00133634, 001336bc, 0013358c and 00133130. Helpers 00124504,
00146588 and 00128134 are reviewed dependencies. Locking, platform virtual
calls, archive construction and the non-resident path remain external.

  make -C reconstructed/archive_io test

Original-byte/live-REA evidence verification:

  python3 reconstructed/archive_io/reproduction/verify_rea.py \
    --code inputs/pokemon-x-0f6d3e1d66cb/code.bin

Optional original resource comparison, from the repo root:

  cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror \
    reconstructed/archive_io/reproduction/check_archives.c \
    reconstructed/archive_io/archive_io.c \
    native/assets/garc.c native/platform/guest.c \
    -o reconstructed/archive_io/build/check_archives
  reconstructed/archive_io/build/check_archives ROMFS

The checker walks ROMFS/a, opens every GARC with the native reader and builds
resident rows (size, end, absolute offset). It reads every member through
px_read_archive_member with platform reads capped at varying chunk sizes,
then seeks into each member of at least eight bytes and reads up to 64 bytes.
Every result is compared with the native reader. No original payloads are
retained here.
