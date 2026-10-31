Battle handler-registration reproduction, 2026-10-30

Implemented: DllBattle 1084cfc0, 1084d1cc, 1088164c, 1084d3c0, 1084d370,
1084cf8c, 108678f8, 108abe30, 10869dd4 and 108a5aac (static analysis
addresses).

  make -C reconstructed/battle_handler test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt):

  python3 -I reconstructed/battle_handler/reproduction/verify_rea.py

It checks every instruction word against the module bytes and the eleven
relocated literals (battle state, handler pool and the four lookup tables)
against the CRO relocation records.

Original-data check (needs outputs/cro-prepared from
scripts/prepare_cro_analysis.py and a C compiler):

  python3 -I reconstructed/battle_handler/reproduction/check_tables.py

It decodes all four lookup tables, their table functions and handler entry
tables from DllBattle.cro and registers every row through the reconstruction.
Its output is record-comparison.json. No original data is retained here.
