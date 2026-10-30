#!/usr/bin/env python3
"""Convert captured live calls (JSON lines) to scripts/replay_live_calls.c input."""
import json
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
# Read-only tables the replayed routines read from the executable image.
RODATA = {0x00570eec: 128,  # Pokemon data block-permutation table (32 rows of 4)
          0x00570f6c: 125}  # nature table (25 rows of 5)


def rodata(code, mapping, address, size):
    for section in mapping:
        start = int(section['address'], 16)
        if section['code_offset'] is not None and start <= address < start + section['size']:
            offset = section['code_offset'] + address - start
            return code[offset:offset + size]
    raise ValueError('address %#x is not file-backed' % address)


def main():
    if len(sys.argv) != 3:
        sys.exit('usage: replay_live_calls.py CAPTURES.jsonl CALLS.txt')
    code = (ROOT / 'inputs/pokemon-x-0f6d3e1d66cb/code.bin').read_bytes()
    mapping = json.loads((ROOT / 'outputs/rea-live/target.json').read_text())['mapping']
    lines, tables = [], set()
    for address, size in RODATA.items():
        lines.append('R %d %s' % (address, rodata(code, mapping, address, size).hex()))
    for raw in open(sys.argv[1]):
        c = json.loads(raw)
        args, entry, exit_ = c['args'], c['entry'], c['exit']
        if c['routine'] == 'load_personal_record':
            tables.add(entry['table'])
            lines.append('P %d %d %s %d %s %s' % (args[1], args[2], entry['record'], entry['table'],
                                                 exit_['record'], exit_['payload']))
        elif c['routine'] == 'personal_field':
            lines.append('F %d %s %d' % (args[1], entry['payload'], c['result']))
        elif c['routine'] == 'select_growth':
            lines.append('G %d %d %s' % (args[0], args[1], exit_['growth_record']))
        elif c['routine'] == 'hp_value':
            lines.append('H %d %d %d %d %d %d' % (args[0], args[1], args[2], args[3], entry['stack'], c['result']))
        elif c['routine'].endswith('_value') and c['routine'] != 'move_value':
            stat = ['attack_value', 'defense_value', 'speed_value', 'special_attack_value',
                    'special_defense_value'].index(c['routine']) + 1
            lines.append('S %d %d %d %d %d %d %s %d' % (stat, args[0], args[1], args[2], args[3], entry['stack'],
                                                     entry['nature_row'] or '-', c['result']))
        elif c['routine'] == 'move_value':
            lines.append('M %d %d %d' % (args[0], args[1], c['result']))
        elif c['routine'] == 'maximum_pp':
            lines.append('X %d %d %d' % (args[0], args[1], c['result']))
        elif c['routine'].startswith('update_'):
            kind = ['update_attack', 'update_defense', 'update_special_attack', 'update_special_defense',
                    'update_speed', 'update_hp'].index(c['routine'])
            e, x = entry, exit_
            lines.append('U %d %d %d %d %d %d %s %s %s %s %s %s %s %s %s' % (
                kind, args[1] if kind == 5 else 0, e['object'], e['manager'], e['data'], e['extra'],
                e['object_bytes'], e['manager_bytes'], e['data_bytes'], e['extra_bytes'] or '-',
                x['object_bytes'], x['manager_bytes'], x['data_bytes'], x['extra_bytes'] or '-', e['natures']))
        elif c['routine'] == 'select_learnset':
            lines.append('N %d %d %s %s' % (args[0], args[1], exit_['record'], exit_['payload']))
        elif c['routine'] == 'level_from_growth':
            lines.append('L %d %d %d %d' % (args[0], args[1], args[2], c['result']))
    open(sys.argv[2], 'w').write('\n'.join(lines) + '\n')
    print('table address: %s' % ' '.join('%#x' % t for t in sorted(tables)) if tables else 'no personal-record calls')


if __name__ == '__main__':
    main()
