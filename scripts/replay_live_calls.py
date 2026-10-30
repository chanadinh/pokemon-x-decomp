#!/usr/bin/env python3
"""Convert captured live calls (JSON lines) to scripts/replay_live_calls.c input."""
import json
import sys


def main():
    if len(sys.argv) != 3:
        sys.exit('usage: replay_live_calls.py CAPTURES.jsonl CALLS.txt')
    lines, tables = [], set()
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
        elif c['routine'] == 'level_from_growth':
            lines.append('L %d %d %d %d' % (args[0], args[1], args[2], c['result']))
    open(sys.argv[2], 'w').write('\n'.join(lines) + '\n')
    print('table address: %s' % ' '.join('%#x' % t for t in sorted(tables)) if tables else 'no personal-record calls')


if __name__ == '__main__':
    main()
