#!/usr/bin/env python3
"""Check the processor's original jump table against the reconstruction.

Reads DllBattle.cro through the battle_handler milestone's table reader and
decodes, with the battle_power decoder, the first instructions of each of
the 63 cases of 10819600 (jump table 108196bc, ABS32 relocations). A case
that hands over to a routine starts with the context and record (and for
some kinds the stack word's low bits) in r0-r2 and a call: those kinds, their
routines and the low-bit flag must be exactly the table in battle_process.c.
Kinds 0xc and 0x3d must reach the fault path. No original data is retained;
the output is a summary.
"""
import argparse
import json
import re
import struct
import sys
from pathlib import Path

sys.dont_write_bytecode = True
HERE = Path(__file__).resolve()
sys.path.insert(0, str(HERE.parents[2] / 'battle_handler/reproduction'))
sys.path.insert(0, str(HERE.parents[2] / 'battle_power/reproduction'))
import check_tables  # noqa: E402
from decode_arm import decode  # noqa: E402

TABLE, KINDS, FAULT = 0x108196bc, 0x3f, 0x1081ad10


def source_routines():
    text = (HERE.parents[1] / 'battle_process.c').read_text()
    return {int(k, 16): (int(a, 16), int(low)) for k, a, low in
            re.findall(r'\[(0x[0-9a-f]+)\] = \{(0x[0-9a-f]+), ([01])\}', text)}


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)

    def text(a):
        return decode(a, struct.unpack('<I', bytes(module.byte(a + i) for i in range(4)))[0])

    found = {}
    for kind in range(KINDS):
        target = module.relocated.get(TABLE + 4 * kind)
        check_tables.require(target is not None, 'Kind %#x has no relocated case' % kind)
        if kind in (0xc, 0x3d):
            check_tables.require(target == FAULT, 'Kind %#x does not fault' % kind)
            continue
        check_tables.require(target != FAULT, 'Kind %#x reaches the fault path' % kind)
        first = [text(target + 4 * i) for i in range(4)]
        low = first[0] == 'cpy r2, r9'
        rest = first[1:] if low else first
        if rest[0] == 'cpy r1, r5' and rest[1] == 'cpy r0, r4' and rest[2].startswith('bl '):
            found[kind] = (int(rest[2].split()[1], 16), int(low))
    expected = source_routines()
    check_tables.require(found == expected, 'Routine table differs: %s' % sorted(set(found.items()) ^ set(expected.items())))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'kinds': KINDS,
            'routine_kinds': len(found), 'routine_kinds_with_low_bits': sum(v[1] for v in found.values()),
            'inline_kinds': KINDS - 2 - len(found), 'fault_kinds': ['0xc', '0x3d'], 'all_match': True}


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
