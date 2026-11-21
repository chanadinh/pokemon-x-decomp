#!/usr/bin/env python3
"""Check how the battle_core_1 helpers connect to the rest of the reconstruction.

Reads every other milestone's evidence/functions.json and confirms that:

- none of this milestone's 28 functions is also a function of another
  milestone (no overlap);
- every one of them, except the three no-operation stubs, is a recorded callee
  of at least one function in another milestone, so the batch fills in
  helpers that earlier milestones left as external callbacks;
- each function's retained listing starts at its entry and ends within the
  body size REA reported, and the three no-operation stubs are single
  `cpy r0, r0` instructions.
The output is a summary.
"""
import argparse
import glob
import json
import sys
from pathlib import Path

sys.dont_write_bytecode = True
HERE = Path(__file__).resolve()
NOPS = {0x10865c48, 0x1084d1c8, 0x108676cc}


def run():
    milestone = HERE.parents[1]
    mine = {int(r['address'], 16): r for r in json.loads((milestone / 'evidence/functions.json').read_text())}
    callers, others = {a: set() for a in mine}, set()
    for path in glob.glob(str(milestone.parent / '*/evidence/functions.json')):
        name = Path(path).parents[1].name
        if name == milestone.name:
            continue
        for row in json.loads(Path(path).read_text()):
            others.add(int(row['address'], 16))
            for callee in row.get('callees') or []:
                try:
                    target = int(callee if isinstance(callee, str) else callee['address'], 16)
                except (KeyError, ValueError, TypeError):
                    continue
                if target in callers:
                    callers[target].add((name, row['address']))
    overlap = sorted(set(mine) & others)
    if overlap:
        raise ValueError('Functions also covered elsewhere: %s' % [hex(a) for a in overlap])
    unconnected = sorted(a for a, c in callers.items() if not c and a not in NOPS)
    if unconnected:
        raise ValueError('Functions no other milestone calls: %s' % [hex(a) for a in unconnected])
    for address, row in mine.items():
        lines = (milestone / 'evidence' / ('%08x.asm' % address)).read_text().splitlines()
        first = int(lines[0].split()[0], 16)
        if first != address or 4 * len(lines) != row['body_bytes']:
            raise ValueError('Listing of %#x does not match its body' % address)
        if address in NOPS and [l.split(None, 2)[2] for l in lines] != ['cpy r0, r0']:
            raise ValueError('%#x is not a no-operation stub' % address)
    return {'functions': len(mine), 'called_by_other_milestones': len(mine) - len(NOPS),
            'no_operation_stubs': ['%08x' % a for a in sorted(NOPS)], 'overlap': 0, 'all_match': True}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.parse_args()
    try:
        record = run()
    except (OSError, ValueError, KeyError) as exc:
        parser.exit(1, f'error: {exc}\n')
    json.dump(record, sys.stdout, indent=2)
    print()


if __name__ == '__main__':
    main()
