#!/usr/bin/env python3
"""Check the survey claims of battle_low_2 against REA's procedure list.

Needs outputs/rea-battle-10/267-list_procedures-response.json (REA's list_procedures of the
DllBattle carrier; not committed). Confirms that each of the 24 reconstructed functions is a
procedure of the carrier whose start lies in the half-open range 10802000-10840000, that none
is an import thunk, that each loads the same relocated state object pointer (108fcb18, the family
criterion; the batch works outward from the frontier, so callers in other milestones are only
counted), and that no other
milestone's evidence/functions.json lists any of them. The output holds only facts that
later milestones cannot change. No original data is retained; the output is a summary.
"""
import argparse
import json
import re
import sys
from pathlib import Path

sys.dont_write_bytecode = True
HERE = Path(__file__).resolve()
LOW, HIGH = 0x10802000, 0x10840000
FUNCTIONS = (0x108399ec, 0x1083b44c, 0x1083c518, 0x1083c960, 0x1083c984, 0x1083e7dc, 0x1083e800, 0x1083f134,
             0x1083b17c, 0x1083c424, 0x1083bad8, 0x1083f270, 0x1083bdd4, 0x1083e7a0, 0x1083e920, 0x1083e960,
             0x1083ea0c, 0x1083f348, 0x1083cb44, 0x10839818, 0x108395d8, 0x1083b110, 0x1083cbb0, 0x1083f1f4)


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
    # The family: every function loads the relocated pointer 108fcb18 (the state tracker object) and one state id.
    literals = json.loads((HERE.parents[1] / 'evidence/literals.json').read_text())
    values = [int.from_bytes(bytes.fromhex(v), 'little') for v in literals.values()]
    if values.count(0x108fcb18) != len(FUNCTIONS):
        raise ValueError('Not every function loads the state object pointer')
    return {'procedures_in_range': len(in_range), 'reconstructed_here': len(FUNCTIONS),
            'state_object_loads': len(FUNCTIONS), 'all_match': True}


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
