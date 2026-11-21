#!/usr/bin/env python3
"""Check how the battle_mid_2 helpers connect to the rest of the reconstruction.

Reads every other milestone's evidence/functions.json and confirms that:

- none of this milestone's 22 functions is also a function of another
  milestone (no overlap), and each starts in the band 10880000-108a6000;
- the batch is reported as connected or not: how many of its functions are
  recorded callees of another milestone, and how many of its own callees another
  milestone covers (a self-contained group of page objects of this band: none);
- each function's retained listing starts at its entry and ends within the
  body size REA reported.
The output is a summary.
"""
import argparse
import glob
import json
import sys
from pathlib import Path

sys.dont_write_bytecode = True
HERE = Path(__file__).resolve()
BAND = (0x10880000, 0x108a6000)


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
    for address, row in mine.items():
        lines = (milestone / 'evidence' / ('%08x.asm' % address)).read_text().splitlines()
        first = int(lines[0].split()[0], 16)
        if first != address or 4 * len(lines) != row['body_bytes']:
            raise ValueError('Listing of %#x does not match its body' % address)
        if not BAND[0] <= address < BAND[1]:
            raise ValueError('%#x is outside the band' % address)
    callees = {int(c if isinstance(c, str) else c['address'], 16) for r in mine.values() for c in r.get('callees') or []}
    return {'functions': len(mine), 'called_by_other_milestones': sum(1 for c in callers.values() if c),
            'callees_covered_by_other_milestones': len(callees & others), 'distinct_external_callees': len(callees - set(mine)),
            'band': [hex(BAND[0]), hex(BAND[1])],
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
