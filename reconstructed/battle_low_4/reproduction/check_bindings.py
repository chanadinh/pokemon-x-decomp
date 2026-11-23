#!/usr/bin/env python3
"""Check the survey claims of battle_low_4 against REA's procedure list.

Needs outputs/rea-battle-10/267-list_procedures-response.json (REA's list_procedures of the
DllBattle carrier; not committed). Confirms that each of the 7 reconstructed functions is a
procedure of the carrier whose start lies in the half-open range 10802000-10840000, that none
is an import thunk, and that no other milestone's evidence/functions.json lists any of them.
The output holds only facts that later milestones cannot change. No original data is
retained; the output is a summary.
"""
import argparse
import json
import re
import sys
from pathlib import Path

sys.dont_write_bytecode = True
HERE = Path(__file__).resolve()
LOW, HIGH = 0x10802000, 0x10840000
FUNCTIONS = (0x10836230, 0x1083b268, 0x1083c0b8, 0x1083ef8c, 0x10839a0c, 0x1082399c, 0x1081cd58)


def run(root):
    listing = root / 'outputs/rea-battle-10/267-list_procedures-response.json'
    entries = json.loads(listing.read_text())['result']['structuredContent']['normalized_result']
    procedures = {int(e['address'], 16): e['procedure'] for e in entries
                  if e['address'].startswith('0x') and not e['procedure']['external']}
    in_range = [a for a in procedures if LOW <= a < HIGH]
    mine = HERE.parents[1].name
    for function in FUNCTIONS:
        if function not in procedures or not LOW <= function < HIGH or procedures[function]['thunk']:
            raise ValueError('%#x is not a plain procedure of the range' % function)
    covered_elsewhere = set()
    for functions in sorted((root / 'reconstructed').glob('*/evidence/functions.json')):
        if functions.parents[1].name == mine:
            continue
        covered_elsewhere.update(int(row['address'], 16) for row in json.loads(functions.read_text()))
    clash = sorted(hex(f) for f in FUNCTIONS if f in covered_elsewhere)
    if clash:
        raise ValueError('Already reconstructed elsewhere: %s' % clash)
    return {'procedures_in_range': len(in_range), 'reconstructed_here': len(FUNCTIONS),
            'all_match': True}


def main():
    root = HERE.parents[3]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.parse_args()
    try:
        record = run(root)
    except (OSError, ValueError, KeyError) as exc:
        parser.exit(1, f'error: {exc}\n')
    json.dump(record, sys.stdout, indent=2)
    print()


if __name__ == '__main__':
    main()
