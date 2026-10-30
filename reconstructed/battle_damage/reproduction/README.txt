Battle-damage reproduction, 2026-10-30

Implemented: DllBattle 1086a1e0 (static analysis address).

  make -C reconstructed/battle_damage test

Analysis target (main image plus relocated DllBattle and DllBackGround):

  python3 scripts/prepare_cro_analysis.py \
    --modules inputs/pokemon-x-modules-a035d9775a1f/modules \
    --code inputs/pokemon-x-0f6d3e1d66cb/code.bin \
    --main-map outputs/20261023T142251572337Z/manifest.json \
    --output outputs/cro-prepared
  python3 -I scripts/prepare_rea_module_target.py \
    --manifest outputs/cro-prepared/manifest.json \
    --module DllBattle --module DllBackGround --output outputs/rea-battle-target

Evidence verification against live REA observations and the original module:

  python3 -I reconstructed/battle_damage/reproduction/verify_rea.py

Live call-by-call check (Azahar GDB stub; play a wild battle and attack):

  python3 -I scripts/trace_live_damage.py --output outputs/live-damage.jsonl
  python3 -I scripts/replay_live_damage.py outputs/live-damage.jsonl outputs/live/damage.txt
  cc -std=c11 -O2 scripts/replay_live_damage.c \
    reconstructed/battle_damage/battle_damage.c -o outputs/live/replay_live_damage
  outputs/live/replay_live_damage outputs/live/damage.txt

scripts/trace_live_callers.py and scripts/map_live_callers.py record who calls
selected main-executable routines and map return addresses inside CRO modules
back to static addresses; they established the runtime delta. Traces contain
game state and stay in the ignored outputs/ directory.
