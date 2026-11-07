#!/usr/bin/env python3
"""Check the command writer against the original module and its callers.

Reads DllBattle.cro through the battle_handler milestone's table reader. The
writer's literals must relocate to the info bytes (108d13fc) and the
argument buffer (10b0f460). Every one of the 0x6d info bytes must be a
format the encoder handles with as many argument words as its low nibble,
or one the encoder faults on (0, or a message format the message routines
write). The encoder's nine jump-table words must be relocated, with format
0 reaching the fault exit. Finally every command the reconstructed battle
routines send with a literal id must pass exactly the number of arguments
its original info byte declares. No original data is retained; the output
is a summary.
"""
import argparse
import json
import re
import sys
from pathlib import Path

sys.dont_write_bytecode = True
HERE = Path(__file__).resolve()
sys.path.insert(0, str(HERE.parents[2] / 'battle_handler/reproduction'))
import check_tables  # noqa: E402

INFO, BUFFER, COMMANDS, TABLE, FAULT = 0x108d13fc, 0x10b0f460, 0x6d, 0x1084dc44, 0x1084dd40
CALLERS = ('battle_process', 'battle_kinds', 'battle_effects', 'battle_outcomes', 'battle_changes', 'battle_restores', 'battle_cures')


def formats():
    """Format -> argument words, from px_battle_command_format_args."""
    text = (HERE.parents[1] / 'battle_command.c').read_text()
    body = text[text.index('int px_battle_command_format_args'):]
    body = body[:body.index('default:')]
    result = {}
    for cases, count in re.findall(r'((?:case 0x[0-9a-f]+: )+)return (\d+);', body):
        for value in re.findall(r'0x[0-9a-f]+', cases):
            result[int(value, 16)] = int(count)
    return result


def split_args(text):
    """Top-level comma split of an ARGS(...) body."""
    parts, depth, start = [], 0, 0
    for i, ch in enumerate(text):
        if ch == '(':
            depth += 1
        elif ch == ')':
            depth -= 1
        elif ch == ',' and depth == 0:
            parts.append(text[start:i].strip())
            start = i + 1
    parts.append(text[start:].strip())
    return parts


def body_after(text, at):
    """The text inside the parentheses opening at text[at]."""
    depth = 0
    for i in range(at, len(text)):
        if text[i] == '(':
            depth += 1
        elif text[i] == ')':
            depth -= 1
            if depth == 0:
                return text[at + 1:i]
    raise ValueError('Unbalanced ARGS')


def caller_commands():
    """(milestone, command id, argument words) for every literal command a routine sends."""
    found = []
    for name in CALLERS:
        text = (HERE.parents[2] / name / (name + '.c')).read_text()
        for m in re.finditer(r'command\(context, ARGS\(', text):
            args = split_args(body_after(text, m.end() - 1))
            if re.fullmatch(r'0x[0-9a-f]+|\d+', args[0]):
                found.append((name, int(args[0], 0), len(args) - 1))
        for m in re.finditer(r'PX_BATTLE_COMMAND,\s*ARGS\(', text):
            args = split_args(body_after(text, m.end() - 1))
            if re.fullmatch(r'0x[0-9a-f]+|\d+', args[1]):
                found.append((name, int(args[1], 0), len(args) - 2))
    return found


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)
    check_tables.require(module.relocated.get(0x1086c2c4) == INFO, 'Info literal differs')
    check_tables.require(module.relocated.get(0x1086c2c8) == BUFFER, 'Buffer literal differs')
    known = formats()
    info = [module.byte(INFO + i) for i in range(COMMANDS)]
    encoded = faulting = 0
    for value in info:
        if value in known:
            check_tables.require(known[value] == value & 0xf, 'Format %#x argument count differs' % value)
            encoded += 1
        else:
            faulting += 1
    for k in range(9):
        check_tables.require(TABLE + 4 * k in module.relocated, 'Jump-table word %d not relocated' % k)
    check_tables.require(module.relocated[TABLE] == FAULT, 'Format 0 does not fault')
    calls = caller_commands()
    for name, command, count in calls:
        check_tables.require(command < COMMANDS, '%s: command %#x out of range' % (name, command))
        check_tables.require(info[command] in known, '%s: command %#x has no encoded format' % (name, command))
        check_tables.require(count == info[command] & 0xf,
                             '%s: command %#x passes %d arguments, its format declares %d' % (name, command, count, info[command] & 0xf))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'commands': COMMANDS,
            'encoded_commands': encoded, 'faulting_or_message_commands': faulting, 'formats_in_encoder': len(known),
            'caller_command_sites': len(calls), 'distinct_caller_commands': len({c for _, c, _ in calls}), 'all_match': True}


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
