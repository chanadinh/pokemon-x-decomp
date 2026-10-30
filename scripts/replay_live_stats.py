#!/usr/bin/env python3
"""Convert scripts/trace_live_function.py output (battle_stats spec) to replay_live_stats.c input."""
import json
import sys


def main():
    if len(sys.argv) != 3:
        sys.exit('usage: replay_live_stats.py TRACE.jsonl CALLS.txt')
    lines = []
    for raw in open(sys.argv[1]):
        t = json.loads(raw)
        kind = {'attack': 0, 'defense': 1}[t['function']]
        flag = t['stack'][1] if kind == 0 else 0
        exit_flag = int(t['exit']['s1:1']['bytes'], 16) if kind == 0 else 0
        reads = list(t['reads'].values())
        lines.append('F %d %d %d %d %d %d %d %d %d %d %d' % (kind, *t['args'], t['stack'][0], flag, t['result'] & 0xffff,
                                                            exit_flag, len(reads), len(t['calls'])))
        for r in reads:
            lines.append('M %d %s' % (r['address'], r['bytes']))
        for c in t['calls']:
            lines.append('C %d %d %d %d %d %d %d %d' % (int(c['target'], 16), *c['args'], *c['stack'], c.get('result', 0)))
    open(sys.argv[2], 'w').write('\n'.join(lines) + '\n')
    print('%d calls' % sum(1 for l in lines if l.startswith('F')))


if __name__ == '__main__':
    main()
