#!/usr/bin/env python3
"""Check how original records reach the five outcome routines.

Reads DllBattle.cro through the battle_handler milestone's table reader. The
processor's jump table (with the battle_process check) must hand kinds 0x10,
0x12, 0x2a, 0x2d and 0x33 to these routines, as the C routine table says.
The record-size table at 108d1ed8 must make each record exactly long enough
for the fields read: four end with an eight-argument message (at +0x10, +8,
+8 and +8), and kind 0x2a holds two messages (+4, +0x28) and the bytes at
+0x4c and +0x4d. No original data is retained; the output is a summary.
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
ROUTES = {0x10: (0x10821d98, 0, 0x10 + MESSAGE), 0x12: (0x1081cf54, 0, 8 + MESSAGE), 0x2a: (0x10824288, 0, 0x50),
          0x2d: (0x1081e0f8, 0, 8 + MESSAGE), 0x33: (0x10820f3c, 0, 8 + MESSAGE)}


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
    for kind, (_, _, size) in ROUTES.items():
        check_tables.require(sizes.get(kind) == size, 'Kind %#x record size differs from its fields' % kind)
    return {'module_sha256': cases['module_sha256'], 'routes_checked': [hex(k) for k in sorted(ROUTES)],
            'processor_cases_match': cases['all_match'], 'record_sizes_match_fields': True, 'all_match': True}


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
