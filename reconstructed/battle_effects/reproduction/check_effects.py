#!/usr/bin/env python3
"""Check the six kind routines against the original module.

Reads DllBattle.cro through the battle_handler milestone's table reader and
the battle_power decoder. The processor's jump table (with the
battle_process check) must hand kinds 5, 0xe, 0x11, 0x1a, 0x1c and 0x2b to
these routines, as the C routine table says. Each record's original size
must hold the fields the routine reads (kind 5 ending exactly with its
message). Kind 0x1a's inner jump table (10826724, 15 relocated cases) must
give exactly the messages of px_battle_field_clear_message. No original
data is retained; the output is a summary.
"""
import argparse
import json
import re
import struct
import sys
from pathlib import Path

sys.dont_write_bytecode = True
HERE = Path(__file__).resolve()
sys.path.insert(0, str(HERE.parents[2] / 'battle_process/reproduction'))
import check_cases  # noqa: E402

check_tables, decode = check_cases.check_tables, check_cases.decode
SIZES, INNER, NONE = 0x108d1ed8, 0x10826724, 0x108267c8
# kind -> (routine, end of the furthest field read)
ROUTES = {5: (0x1081d558, 0x2c), 0xe: (0x1081f648, 0xd), 0x11: (0x10823704, 5), 0x1a: (0x108266b8, 9),
          0x1c: (0x10824fb0, 0x31), 0x2b: (0x10822cbc, 6)}


def source_messages():
    text = (HERE.parents[1] / 'battle_effects.c').read_text()
    body = re.search(r'messages\[0xf\] = \{([^}]*)\}', text).group(1)
    return [int(v, 0) for v in body.replace(' ', '').split(',')]


def run(root, cro_path):
    cases = check_cases.run(root, cro_path)
    routines = check_cases.source_routines()
    for kind, (address, _) in ROUTES.items():
        check_tables.require(routines.get(kind) == (address, 0), 'Kind %#x is not routed to %#x' % (kind, address))
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)
    sizes = {module.byte(SIZES + 2 * i): module.byte(SIZES + 2 * i + 1) for i in range(0x3d)}
    for kind, (_, end) in ROUTES.items():
        check_tables.require(sizes.get(kind, 0) >= end, 'Kind %#x record too small' % kind)
    check_tables.require(sizes[5] == 8 + 0x24, 'Kind 5 record does not end with its message')
    messages = []
    for flag in range(0xf):
        target = module.relocated.get(INNER + 4 * flag)
        check_tables.require(target is not None, 'Inner case %d not relocated' % flag)
        if target == NONE:
            messages.append(-1)
            continue
        word = struct.unpack('<I', bytes(module.byte(target + i) for i in range(4)))[0]
        text = decode(target, word)
        check_tables.require(text.startswith('mov r2, #'), 'Inner case %d is not a message' % flag)
        messages.append(int(text.split('#')[1], 16))
    check_tables.require(messages == source_messages(), 'Field messages differ')
    return {'module_sha256': cases['module_sha256'], 'routes_checked': [hex(k) for k in sorted(ROUTES)],
            'records_large_enough': True, 'field_messages': 15, 'field_flags_without_message': messages.count(-1),
            'all_match': True}


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
