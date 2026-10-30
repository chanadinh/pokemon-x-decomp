Battle event-system reproduction, 2026-10-30

Implemented: DllBattle 1086bb84, 1086b880, 1086bac0, 1086b9fc, 1086b520,
10867c9c, 1086b91c, 1086b9a4 and 10864e98 (static analysis addresses).

  make -C reconstructed/battle_event test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt):

  python3 -I reconstructed/battle_event/reproduction/verify_rea.py

It checks every instruction word against the module bytes and the ten
relocated literals (10b07350, 108fca00, 10b1196c) against the CRO relocation
records.
