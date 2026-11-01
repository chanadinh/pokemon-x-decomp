#!/usr/bin/env python3
"""Check the original record-size table against the reconstructed records.

Reads DllBattle.cro through the battle_handler milestone's table reader:
the allocator's literal 1086a0cc must relocate to the table at 108d1ed8,
whose 0x3d (kind, size) pairs must have distinct kinds and nonzero sizes.
Every record kind the reconstructed handlers allocate must exist and hold
the fields they write; records that carry a message must be exactly long
enough for a message with eight arguments. No original data is retained;
the output is a summary.
"""
import argparse
import json
import sys
from pathlib import Path

sys.dont_write_bytecode = True
HERE = Path(__file__).resolve()
sys.path.insert(0, str(HERE.parents[2] / 'battle_handler/reproduction'))
import check_tables  # noqa: E402

TABLE, KINDS, CAPACITY, MESSAGE = 0x108d1ed8, 0x3d, 0x1f4, 0x24
# kind -> (end of the furthest field written by a reconstructed handler, message offset or None)
USED = {
    1: (4, None), 2: (4, None),        # ability popup (battle_action)
    3: (4, 4),                         # messages (battle_immunity, battle_status, battle_switchin, battle_insight)
    0xd: (0x19, None),                 # stat stages (battle_stage, battle_switchin Intimidate, battle_insight Download)
    0x15: (6, None),                   # battle_modifier
    0x17: (6, None),                   # battle_immunity
    0x1e: (6, None),                   # weather (battle_switchin)
    0x20: (9, 0xc),                    # Trace (battle_insight)
    0x3a: (6, 8),                      # Forecast (battle_switchin)
}


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)
    check_tables.require(module.relocated.get(0x1086a0cc) == TABLE, 'Allocator literal does not point at the size table')
    sizes = {}
    for i in range(KINDS):
        kind, size = module.byte(TABLE + 2 * i), module.byte(TABLE + 2 * i + 1)
        check_tables.require(kind not in sizes, 'Repeated kind %#x' % kind)
        check_tables.require(size != 0 and size <= CAPACITY, 'Kind %#x size out of range' % kind)
        sizes[kind] = size
    for kind, (end, message) in USED.items():
        check_tables.require(kind in sizes, 'Kind %#x missing' % kind)
        check_tables.require(sizes[kind] >= max(end, 0 if message is None else message + MESSAGE), 'Kind %#x too small' % kind)
        if message is not None:
            check_tables.require(sizes[kind] == message + MESSAGE, 'Kind %#x is not an exact message record' % kind)
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'kinds': len(sizes),
            'distinct_kinds': True, 'all_nonzero': True,
            'multiples_of_four': all(s % 4 == 0 for s in sizes.values()),
            'used_kinds_checked': [hex(k) for k in sorted(USED)],
            'exact_message_records': [hex(k) for k in sorted(USED) if USED[k][1] is not None], 'all_match': True}


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
