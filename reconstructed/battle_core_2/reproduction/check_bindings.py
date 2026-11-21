#!/usr/bin/env python3
"""Check how the battle_core_2 helpers connect to the rest of the reconstruction.

Reads every other milestone's evidence/functions.json and confirms that:

- none of this milestone's 21 functions is also a function of another
  milestone (committed or on disk), so the batch overlaps nothing;
- every one of them starts inside this milestone's address ranges
  (10840000-10880000 and 108a6000-108ce000);
- every one of them is a recorded callee of at least one function in
  another milestone, so the batch fills in helpers that earlier milestones
  left as external callbacks;
- each function's retained listing contains its entry (the two functions with a
  second body range list that range first) and its instruction count matches
  the body size REA reported.
The output is a summary.
"""
import argparse
import glob
import json
import sys
from pathlib import Path

sys.dont_write_bytecode = True
HERE = Path(__file__).resolve()
RANGES = ((0x10840000, 0x10880000), (0x108a6000, 0x108ce000))


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
    outside = sorted(a for a in mine if not any(lo <= a < hi for lo, hi in RANGES))
    if outside:
        raise ValueError('Functions outside the batch ranges: %s' % [hex(a) for a in outside])
    unconnected = sorted(a for a, c in callers.items() if not c)
    if unconnected:
        raise ValueError('Functions no other milestone calls: %s' % [hex(a) for a in unconnected])
    for address, row in mine.items():
        lines = (milestone / 'evidence' / ('%08x.asm' % address)).read_text().splitlines()
        listed = {int(l.split()[0], 16) for l in lines}
        if address not in listed or 4 * len(lines) != row['body_bytes']:
            raise ValueError('Listing of %#x does not match its body' % address)
    return {'functions': len(mine), 'called_by_other_milestones': len(mine), 'inside_ranges': len(mine),
            'overlap': 0, 'all_match': True}


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
