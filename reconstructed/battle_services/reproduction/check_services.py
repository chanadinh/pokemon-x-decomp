#!/usr/bin/env python3
"""Check the battler services against the original module and their callers.

Reads DllBattle.cro through the battle_handler milestone's table reader and
decodes, with the battle_power decoder, the three inner jump tables: in
108722e8 and 108714fc stat 0 must reach the fault path and stats 1-7 the
stage bytes +0x104..+0x10a in order; in 10871250 keys 8-0xc must store the
values +0xf6..+0xfe in order. Every call the reconstructed kind routines
make to these services must pass the number of arguments the service takes.
No original data is retained; the output is a summary.
"""
import argparse
import json
import re
import struct
import sys
from pathlib import Path

sys.dont_write_bytecode = True
HERE = Path(__file__).resolve()
sys.path.insert(0, str(HERE.parents[2] / 'battle_command/reproduction'))
sys.path.insert(0, str(HERE.parents[2] / 'battle_power/reproduction'))
import check_commands  # noqa: E402
from decode_arm import decode  # noqa: E402

check_tables = check_commands.check_tables
# service -> argument words (registers then stack)
ARGUMENTS = {0x108722e8: 3, 0x108714fc: 3, 0x10871250: 3, 0x108620d4: 2, 0x10865788: 4, 0x10867a04: 4,
             0x108695c0: 4, 0x10862858: 7, 0x10860e74: 12}


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)

    def text(a):
        return decode(a, struct.unpack('<I', bytes(module.byte(a + i) for i in range(4)))[0])

    def offset(target):
        """The stage offset a case adds to r0 (one or two adds)."""
        first = text(target)
        value = int(first.split('#')[1], 16)
        if value == 0x100 and text(target + 4).startswith('add r0, r0, #'):
            value += int(text(target + 4).split('#')[1], 16)
        return value

    for table, fault in ((0x10872304, 0x10872384), (0x10871508, 0x1087157c)):
        targets = [module.relocated.get(table + 4 * k) for k in range(8)]
        check_tables.require(targets[0] == fault, 'Stat 0 does not fault in %#x' % table)
        check_tables.require([offset(t) for t in targets[1:]] == list(range(0x104, 0x10b)), 'Stage offsets differ in %#x' % table)
    stores = [text(module.relocated[0x10871270 + 4 * k]) for k in range(5)]
    check_tables.require(stores == ['strh r4, [r3,#0x%x]' % (0xf6 + 2 * k) for k in range(5)], 'Value offsets differ: %s' % stores)
    sites = 0
    for name in check_commands.CALLERS:
        source = (HERE.parents[2] / name / (name + '.c')).read_text()
        for m in re.finditer(r'call\(api, (0x[0-9a-f]+), ARGS\(', source):
            address = int(m.group(1), 16)
            if address not in ARGUMENTS:
                continue
            args = check_commands.split_args(check_commands.body_after(source, m.end() - 1))
            check_tables.require(len(args) == ARGUMENTS[address],
                                 '%s: %#x called with %d arguments, takes %d' % (name, address, len(args), ARGUMENTS[address]))
            sites += 1
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'stage_tables_checked': 2, 'value_table_checked': 1,
            'caller_sites': sites, 'all_match': True}


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
