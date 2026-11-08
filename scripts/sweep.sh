cd /mnt/d/projects/pokemon-x-decomp-local
make -s -C reconstructed clean >/dev/null 2>&1; make -s -C reconstructed test 2>&1 | grep -E "all tests passed|^PASS: .*(integration|orchestration)|passed$|FAIL|rror" | grep -v "^make"
t=0; for d in reconstructed/*/; do [ -f $d/SHA256SUMS ] || continue; r=$(cd $d && sha256sum -c SHA256SUMS 2>&1); ok=$(echo "$r" | grep -c ': OK$'); bad=$(echo "$r" | grep -vc ': OK$'); t=$((t+ok)); [ $bad -ne 0 ] && echo "BAD $d"; done; echo "checksums ok: $t"
python3 -I reconstructed/pokemon_growth/reproduction/verify_rea.py --code inputs/pokemon-x-0f6d3e1d66cb/code.bin
python3 -I reconstructed/pokemon_personal/reproduction/verify_rea.py --code inputs/pokemon-x-0f6d3e1d66cb/code.bin
python3 -I reconstructed/archive_io/reproduction/verify_rea.py --code inputs/pokemon-x-0f6d3e1d66cb/code.bin
python3 -I reconstructed/archive_index/reproduction/verify_rea.py --code inputs/pokemon-x-0f6d3e1d66cb/code.bin
python3 -I reconstructed/pokemon_resources/reproduction/verify_rea.py --code inputs/pokemon-x-0f6d3e1d66cb/code.bin
python3 -I reconstructed/message_construction/reproduction/verify_rea.py --code inputs/pokemon-x-0f6d3e1d66cb/code.bin
python3 -I reconstructed/battle_damage/reproduction/verify_rea.py
python3 -I reconstructed/battle_stats/reproduction/verify_rea.py
python3 -I reconstructed/battle_types/reproduction/verify_rea.py
python3 -I reconstructed/battle_matchup/reproduction/verify_rea.py
python3 -I reconstructed/battle_event/reproduction/verify_rea.py
python3 -I reconstructed/battle_handler/reproduction/verify_rea.py
python3 -I reconstructed/battle_ability/reproduction/verify_rea.py
python3 -I reconstructed/battle_power/reproduction/verify_rea.py
python3 -I reconstructed/battle_modifier/reproduction/verify_rea.py
python3 -I reconstructed/battle_typechange/reproduction/verify_rea.py
python3 -I reconstructed/battle_immunity/reproduction/verify_rea.py
python3 -I reconstructed/battle_stage/reproduction/verify_rea.py
python3 -I reconstructed/battle_status/reproduction/verify_rea.py
python3 -I reconstructed/battle_switchin/reproduction/verify_rea.py
python3 -I reconstructed/battle_switchin/reproduction/check_bindings.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_switchin/reproduction/record-comparison.json'));print('switchin-bindings-ok')"
python3 -I reconstructed/battle_insight/reproduction/verify_rea.py
python3 -I reconstructed/battle_insight/reproduction/check_bindings.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_insight/reproduction/record-comparison.json'));print('insight-bindings-ok')"
python3 -I reconstructed/battle_action/reproduction/verify_rea.py
python3 -I reconstructed/battle_action/reproduction/check_sizes.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_action/reproduction/record-comparison.json'));print('action-sizes-ok')"
python3 -I reconstructed/battle_process/reproduction/verify_rea.py
python3 -I reconstructed/battle_process/reproduction/check_cases.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_process/reproduction/record-comparison.json'));print('process-cases-ok')"
python3 -I reconstructed/battle_kinds/reproduction/verify_rea.py
python3 -I reconstructed/battle_kinds/reproduction/check_routes.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_kinds/reproduction/record-comparison.json'));print('kinds-routes-ok')"
python3 -I reconstructed/battle_effects/reproduction/verify_rea.py
python3 -I reconstructed/battle_effects/reproduction/check_effects.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_effects/reproduction/record-comparison.json'));print('effects-ok')"
python3 -I reconstructed/battle_outcomes/reproduction/verify_rea.py
python3 -I reconstructed/battle_outcomes/reproduction/check_outcomes.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_outcomes/reproduction/record-comparison.json'));print('outcomes-ok')"
python3 -I reconstructed/battle_changes/reproduction/verify_rea.py
python3 -I reconstructed/battle_changes/reproduction/check_changes.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_changes/reproduction/record-comparison.json'));print('changes-ok')"
python3 -I reconstructed/battle_restores/reproduction/verify_rea.py
python3 -I reconstructed/battle_restores/reproduction/check_restores.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_restores/reproduction/record-comparison.json'));print('restores-ok')"
python3 -I reconstructed/battle_cures/reproduction/verify_rea.py
python3 -I reconstructed/battle_cures/reproduction/check_cures.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_cures/reproduction/record-comparison.json'));print('cures-ok')"
python3 -I reconstructed/battle_command/reproduction/verify_rea.py
python3 -I reconstructed/battle_command/reproduction/check_commands.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_command/reproduction/record-comparison.json'));print('commands-ok')"
python3 -I reconstructed/battle_message/reproduction/verify_rea.py
python3 -I reconstructed/battle_message/reproduction/check_messages.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_message/reproduction/record-comparison.json'));print('messages-ok')"
python3 -I reconstructed/battle_services/reproduction/verify_rea.py
python3 -I reconstructed/battle_services/reproduction/check_services.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_services/reproduction/record-comparison.json'));print('services-ok')"
python3 -I reconstructed/battle_items_1/reproduction/verify_rea.py
python3 -I reconstructed/battle_items_1/reproduction/check_bindings.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_items_1/reproduction/record-comparison.json'));print('battle_items_1-ok')"
python3 -I reconstructed/battle_moves_1/reproduction/verify_rea.py
python3 -I reconstructed/battle_moves_1/reproduction/check_bindings.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_moves_1/reproduction/record-comparison.json'));print('battle_moves_1-ok')"
python3 -I reconstructed/battle_abilities_1/reproduction/verify_rea.py
python3 -I reconstructed/battle_abilities_1/reproduction/check_bindings.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_abilities_1/reproduction/record-comparison.json'));print('battle_abilities_1-ok')"
python3 -I reconstructed/battle_conditions/reproduction/verify_rea.py
python3 -I reconstructed/battle_conditions/reproduction/check_conditions.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_conditions/reproduction/record-comparison.json'));print('conditions-ok')"
python3 -I reconstructed/battle_moves_2/reproduction/verify_rea.py
python3 -I reconstructed/battle_moves_2/reproduction/check_bindings.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_moves_2/reproduction/record-comparison.json'));print('battle_moves_2-ok')"
python3 -I reconstructed/battle_abilities_2/reproduction/verify_rea.py
python3 -I reconstructed/battle_abilities_2/reproduction/check_bindings.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_abilities_2/reproduction/record-comparison.json'));print('battle_abilities_2-ok')"
python3 -I reconstructed/battle_items_2/reproduction/verify_rea.py
python3 -I reconstructed/battle_items_2/reproduction/check_bindings.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_items_2/reproduction/record-comparison.json'));print('battle_items_2-ok')"
python3 -I reconstructed/battle_moves_3/reproduction/verify_rea.py
python3 -I reconstructed/battle_moves_3/reproduction/check_bindings.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_moves_3/reproduction/record-comparison.json'));print('battle_moves_3-ok')"
python3 -I reconstructed/battle_abilities_3/reproduction/verify_rea.py
python3 -I reconstructed/battle_abilities_3/reproduction/check_bindings.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_abilities_3/reproduction/record-comparison.json'));print('battle_abilities_3-ok')"
python3 -I reconstructed/battle_moves_4/reproduction/verify_rea.py
python3 -I reconstructed/battle_moves_4/reproduction/check_bindings.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_moves_4/reproduction/record-comparison.json'));print('battle_moves_4-ok')"
python3 -I reconstructed/battle_abilities_4/reproduction/verify_rea.py
python3 -I reconstructed/battle_abilities_4/reproduction/check_bindings.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_abilities_4/reproduction/record-comparison.json'));print('battle_abilities_4-ok')"
python3 -I reconstructed/battle_status/reproduction/check_bindings.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_status/reproduction/record-comparison.json'));print('status-bindings-ok')"
python3 -I reconstructed/battle_stage/reproduction/check_bindings.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_stage/reproduction/record-comparison.json'));print('stage-bindings-ok')"
python3 -I reconstructed/battle_immunity/reproduction/check_bindings.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_immunity/reproduction/record-comparison.json'));print('immunity-bindings-ok')"
python3 -I reconstructed/battle_typechange/reproduction/check_bindings.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_typechange/reproduction/record-comparison.json'));print('typechange-bindings-ok')"
python3 -I reconstructed/battle_modifier/reproduction/check_bindings.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_modifier/reproduction/record-comparison.json'));print('modifier-bindings-ok')"
python3 -I reconstructed/battle_power/reproduction/check_bindings.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_power/reproduction/record-comparison.json'));print('power-bindings-ok')"
python3 -I reconstructed/battle_ability/reproduction/check_bindings.py | python3 -c "import json,sys;assert json.load(sys.stdin)==json.load(open('reconstructed/battle_ability/reproduction/record-comparison.json'));print('bindings-ok')"
python3 -I reconstructed/battle_handler/reproduction/check_tables.py | python3 -c "import json,sys;a=json.load(sys.stdin);b=json.load(open('reconstructed/battle_handler/reproduction/record-comparison.json'));assert a==b;print('tables-ok')"
mkdir -p reconstructed/battle_types/build && cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror reconstructed/battle_types/reproduction/check_chart.c reconstructed/battle_types/battle_types.c native/assets/pokedata.c -o reconstructed/battle_types/build/check_chart && reconstructed/battle_types/build/check_chart inputs/pokemon-x-modules-a035d9775a1f/modules/DllBattle.cro | python3 -c "import json,sys;a=json.load(sys.stdin);b=json.load(open('reconstructed/battle_types/reproduction/record-comparison.json'));assert all(b[k]==v for k,v in a.items());print('chart-ok')"
git status --short --ignored native | head
