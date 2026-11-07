#!/usr/bin/env python3
"""Check the message groups against the original module and their callers.

Reads DllBattle.cro through the battle_handler milestone's table reader.
Among the command writer's 0x6d info bytes, the commands the encoder does
not handle must be exactly command 0 and the four message groups 0x69-0x6c
written by these routines. Every message command the reconstructed battle
routines send (through 1086b5e0) must use group 0x69 or 0x6a and end its
argument list with the terminator 0xffff0000. No
original data is retained; the output is a summary.
"""
import argparse
import json
import re
import sys
from pathlib import Path

sys.dont_write_bytecode = True
HERE = Path(__file__).resolve()
sys.path.insert(0, str(HERE.parents[2] / 'battle_command/reproduction'))
import check_commands  # noqa: E402

check_tables = check_commands.check_tables
GROUPS = {0x69, 0x6a, 0x6b, 0x6c}


def caller_messages():
    found = []
    for name in check_commands.CALLERS:
        text = (HERE.parents[2] / name / (name + '.c')).read_text()
        for m in re.finditer(r'PX_BATTLE_MESSAGE_COMMAND,\s*ARGS\(', text):
            args = check_commands.split_args(check_commands.body_after(text, m.end() - 1))
            found.append((name, args))
    return found


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)
    known = check_commands.formats()
    unencoded = {c for c in range(check_commands.COMMANDS) if module.byte(check_commands.INFO + c) not in known}
    check_tables.require(unencoded == {0} | GROUPS, 'Commands outside the encoder are not command 0 and the message groups')
    sites = caller_messages()
    for name, args in sites:
        check_tables.require(re.fullmatch(r'0x[0-9a-f]+', args[1]) and int(args[1], 16) in (0x69, 0x6a), '%s: message group %s' % (name, args[1]))
        check_tables.require('0xffff0000' in args[-1], '%s: message list does not end with the terminator' % name)
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'message_groups': sorted(hex(g) for g in GROUPS),
            'commands_outside_encoder': len(unencoded), 'caller_message_sites': len(sites),
            'callers_end_with_terminator': True, 'all_match': True}


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
