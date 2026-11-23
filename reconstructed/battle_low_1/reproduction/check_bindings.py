#!/usr/bin/env python3
"""Check the survey claims of battle_low_1 against REA's procedure list.

Needs outputs/rea-battle-10/267-list_procedures-response.json (REA's list_procedures of the
DllBattle carrier; not committed). Confirms that each of the 15 reconstructed functions is a
procedure of the carrier whose start lies in the half-open range 10802000-10840000, that none
is an import thunk, that each is called (bl or b) from some evidence listing of another
milestone under reconstructed/ (the "frontier" the batch was chosen from), and that no other
milestone's evidence/functions.json lists any of them. The output holds only facts that
later milestones cannot change (callers only ever increase, so the call fact is a boolean).
No original data is retained; the output is a summary.
"""
import argparse
import json
import re
import sys
from pathlib import Path

sys.dont_write_bytecode = True
HERE = Path(__file__).resolve()
LOW, HIGH = 0x10802000, 0x10840000
FUNCTIONS = (0x10810cc0, 0x10814f9c, 0x1081ca98, 0x1081cc04, 0x1081cf08, 0x1081cf14, 0x1081d2b0, 0x1081d748,
             0x1081d770, 0x1081d83c, 0x1081e28c, 0x1082001c, 0x108261e8, 0x1082b064, 0x1081fc20)


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
    callers = {f: set() for f in FUNCTIONS}
    for asm in sorted((root / 'reconstructed').glob('*/evidence/*.asm')):
        if asm.parents[1].name == mine:
            continue
        for line in asm.read_text().splitlines():
            m = re.match(r'[0-9a-f]{8}  [0-9a-f]{8}  (?:bl|b)[a-z]{0,2} 0x([0-9a-f]+)$', line.strip())
            if m and int(m[1], 16) in callers:
                callers[int(m[1], 16)].add(asm.parents[1].name)
    uncalled = sorted(hex(f) for f in FUNCTIONS if not callers[f])
    if uncalled:
        raise ValueError('Not called from another milestone: %s' % uncalled)
    return {'procedures_in_range': len(in_range), 'reconstructed_here': len(FUNCTIONS),
            'every_function_called_from_another_milestone': True, 'all_match': True}


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
