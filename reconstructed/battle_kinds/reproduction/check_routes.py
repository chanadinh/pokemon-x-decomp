#!/usr/bin/env python3
"""Check how original records reach the three kind routines.

Reads DllBattle.cro through the battle_handler milestone's table reader. The
processor's jump table (with the battle_process check) must hand kinds 0xd,
0x1e and 0x20 to 10823014 (with the stack word's low bits), 10824774 and
10824978, as the C routine table says. The record-size table at 108d1ed8
must make each of these records exactly long enough for the eight-argument
message the routine shows (at +0x1c, +8 and +0xc). No original data is
retained; the output is a summary.
"""
import argparse
import json
import sys
from pathlib import Path

sys.dont_write_bytecode = True
HERE = Path(__file__).resolve()
sys.path.insert(0, str(HERE.parents[2] / 'battle_process/reproduction'))
import check_cases  # noqa: E402

check_tables = check_cases.check_tables
SIZES, MESSAGE = 0x108d1ed8, 0x24
# kind -> (routine, takes the low bits, message offset)
ROUTES = {0xd: (0x10823014, 1, 0x1c), 0x1e: (0x10824774, 0, 8), 0x20: (0x10824978, 0, 0xc)}


def run(root, cro_path):
    cases = check_cases.run(root, cro_path)
    routines = check_cases.source_routines()
    for kind, (address, low, _) in ROUTES.items():
        check_tables.require(routines.get(kind) == (address, low), 'Kind %#x is not routed to %#x' % (kind, address))
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)
    sizes = {module.byte(SIZES + 2 * i): module.byte(SIZES + 2 * i + 1) for i in range(0x3d)}
    for kind, (_, _, message) in ROUTES.items():
        check_tables.require(sizes.get(kind) == message + MESSAGE, 'Kind %#x record does not end with its message' % kind)
    check_tables.require(module.relocated.get(0x10824b98) == 0x108d10f0, 'Ability-list literal differs')
    return {'module_sha256': cases['module_sha256'], 'routes_checked': [hex(k) for k in sorted(ROUTES)],
            'processor_cases_match': cases['all_match'], 'records_end_with_message': True, 'all_match': True}


def main():
    root = HERE.parents[3]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cro', type=Path, default=root / 'inputs/pokemon-x-modules-a035d9775a1f/modules/DllBattle.cro')
    args = parser.parse_args()
    try:
        record = run(root, args.cro)
    except (OSError, ValueError, KeyError) as exc:
        parser.exit(1, f'error: {exc}\n')
    json.dump(record, sys.stdout, indent=2)
    print()


if __name__ == '__main__':
    main()
