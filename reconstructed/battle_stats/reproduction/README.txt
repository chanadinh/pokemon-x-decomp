Battle attack/defense reproduction, 2026-10-30

Implemented: DllBattle 108202d8 and 10822330 (static analysis addresses).

  make -C reconstructed/battle_stats test

Evidence verification against live REA observations and the original module
(analysis carrier as in reconstructed/battle_damage/reproduction/README.txt):

  python3 -I reconstructed/battle_stats/reproduction/verify_rea.py

Live call-by-call check (Azahar GDB stub; play a wild battle and attack):

  python3 -I scripts/trace_live_function.py --spec scripts/battle_stats_sites.json \
    --output outputs/live-stats.jsonl
  python3 -I scripts/replay_live_stats.py outputs/live-stats.jsonl outputs/live/stats.txt
  cc -std=c11 -O2 scripts/replay_live_stats.c reconstructed/battle_stats/battle_stats.c \
    -o outputs/live/replay_live_stats
  outputs/live/replay_live_stats outputs/live/stats.txt

trace_live_function.py is generic: its spec lists functions, call sites,
stack-argument counts and entry/exit memory reads. Traces contain game state
and stay in the ignored outputs/ directory. The live check has not been run yet.
