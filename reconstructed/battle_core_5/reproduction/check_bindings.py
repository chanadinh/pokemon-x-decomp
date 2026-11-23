#!/usr/bin/env python3
"""Check how the battle_core_5 helpers connect to the rest of the reconstruction.

Reads every other milestone's evidence/functions.json and confirms that:

- none of this milestone's 22 functions is also a function of another
  milestone (committed or on disk), so the batch overlaps nothing;
- every one of them starts inside this milestone's address ranges
  (10840000-10880000 and 108a6000-108ce000);
- the functions chosen as frontier (a fixed list in this script) are each a
  recorded callee of another milestone's function; the other functions belong to
  the same battle-state accessor family and nothing is claimed
  about their callers;
- each function's retained listing contains its entry and its instruction count matches
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
# The functions chosen as frontier (committed code called them when the batch was made); the rest of the
# batch belongs to the same cluster and may or may not be called by later milestones.
FRONTIER = {0x1084b9f0, 0x1084c0f8, 0x1084c7bc, 0x1084cf74, 0x108696b0, 0x1086f878, 0x10872ff8, 0x108c59b4, 0x108c86d4, 0x108c8714, 0x108c99fc}


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
    missing = sorted(a for a in FRONTIER if not callers.get(a))
    if missing:
        raise ValueError('Frontier functions no other milestone calls: %s' % [hex(a) for a in missing])
    if not FRONTIER <= set(mine):
        raise ValueError('A frontier function is missing from the batch')
    for address, row in mine.items():
        lines = (milestone / 'evidence' / ('%08x.asm' % address)).read_text().splitlines()
        listed = {int(l.split()[0], 16) for l in lines}
        if address not in listed or 4 * len(lines) != row['body_bytes']:
            raise ValueError('Listing of %#x does not match its body' % address)
    return {'functions': len(mine), 'frontier_functions': len(FRONTIER), 'every_frontier_function_called_from_another_milestone': True,
            'inside_ranges': len(mine), 'overlap': 0, 'all_match': True}


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
