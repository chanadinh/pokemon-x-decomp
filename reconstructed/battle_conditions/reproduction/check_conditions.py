#!/usr/bin/env python3
"""Check the status applier's animations and caller against the original module.

Reads DllBattle.cro through the battle_handler milestone's table reader and
decodes, with the battle_power decoder, the inner jump table 108629fc: each
status 0-7 either skips the animation (108629b8's common exit 10862b20) or
loads its animation id (a literal or an immediate into r10). The ids must be
exactly px_battle_status_animation's table. The status kind routine's call
must pass the eight arguments the applier takes. No original data is
retained; the output is a summary.
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
TABLE, NONE = 0x108629fc, 0x10862b20


def source_animations():
    text = (HERE.parents[1] / 'battle_conditions.c').read_text()
    body = re.search(r'animations\[8\] = \{([^}]*)\}', text).group(1)
    return [int(v, 0) for v in body.replace(' ', '').split(',')]


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)

    def word(a):
        return struct.unpack('<I', bytes(module.byte(a + i) for i in range(4)))[0]

    animations = []
    for status in range(8):
        target = module.relocated.get(TABLE + 4 * status)
        check_tables.require(target is not None, 'Status %d case not relocated' % status)
        if target == NONE:
            animations.append(0)
            continue
        # The case loads r10 within its first two instructions.
        loads = [decode(a, word(a)) for a in (target, target + 4)]
        load = next((t for t in loads if t.startswith('ldr r10, [0x') or t.startswith('mov r10, #')), None)
        check_tables.require(load is not None, 'Status %d case loads no animation' % status)
        if load.startswith('ldr'):
            animations.append(word(int(load.split('[')[1].rstrip(']'), 16)))
        else:
            animations.append(int(load.split('#')[1], 16))
    check_tables.require(animations == source_animations(), 'Animations differ')
    text = (HERE.parents[2] / 'battle_changes/battle_changes.c').read_text()
    m = re.search(r'call\(api, 0x108629b8, ARGS\(', text)
    args = check_commands.split_args(check_commands.body_after(text, m.end() - 1))
    check_tables.require(len(args) == 8, 'The status kind routine passes %d arguments' % len(args))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'statuses_checked': 8,
            'statuses_with_animation': sum(1 for a in animations if a), 'caller_arguments': len(args), 'all_match': True}


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
