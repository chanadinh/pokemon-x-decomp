#!/usr/bin/env python3
"""Check the last five kind routines against the original module.

Reads DllBattle.cro through the battle_handler milestone's table reader. The
processor's jump table (with the battle_process check) must hand kinds 0xa
(with the stack word's low bits), 0x21, 0x25, 0x28 and 0x36 to these
routines, as the C routine table says. The record-size table at 108d1ed8
must match the fields read: kinds 0xa, 0x21 and 0x25 end exactly with their
last eight-argument message (+0x14, +0xc, +0x50), kind 0x28 holds its u16
at +8, and kind 0x36 is a bare header. Kind 0xa's literal must relocate to
the cure list 108d146a, whose six u16 conditions must be distinct, nonzero
and below 0x26. Finally, the evidence of the six kind-routine milestones
must cover exactly the 27 routines of the processor's table. No original
data is retained; the output is a summary.
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
SIZES, MESSAGE, CURES = 0x108d1ed8, 0x24, 0x108d146a
# kind -> (routine, takes the low bits, record size)
ROUTES = {0xa: (0x10820904, 1, 0x14 + MESSAGE), 0x21: (0x1081f36c, 0, 0xc + MESSAGE), 0x25: (0x10820c9c, 0, 0x50 + MESSAGE),
          0x28: (0x10825eb8, 0, 0xc), 0x36: (0x10824414, 0, 4)}
MILESTONES = ('battle_kinds', 'battle_effects', 'battle_outcomes', 'battle_changes', 'battle_restores', 'battle_cures')


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
    check_tables.require(module.relocated.get(0x10820c98) == CURES, 'Cure-list literal differs')
    cures = [module.byte(CURES + 2 * i) | module.byte(CURES + 2 * i + 1) << 8 for i in range(6)]
    check_tables.require(len(set(cures)) == 6 and all(0 < c < 0x26 for c in cures), 'Cure list out of range')
    covered = set()
    for name in MILESTONES:
        rows = json.loads((HERE.parents[2] / name / 'evidence/functions.json').read_text())
        covered |= {int(r['address'], 16) for r in rows}
    check_tables.require(covered == {a for a, _ in routines.values()}, 'Kind routines not all reconstructed')
    return {'module_sha256': cases['module_sha256'], 'routes_checked': [hex(k) for k in sorted(ROUTES)],
            'record_sizes_match_fields': True, 'cure_list_entries': len(cures),
            'kind_routines_reconstructed': len(covered), 'all_match': True}


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
