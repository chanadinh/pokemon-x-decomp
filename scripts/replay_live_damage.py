#!/usr/bin/env python3
"""Convert scripts/trace_live_damage.py output to scripts/replay_live_damage.c input."""
import json
import sys


def main():
    if len(sys.argv) != 3:
        sys.exit('usage: replay_live_damage.py TRACE.jsonl CALLS.txt')
    lines = []
    for raw in open(sys.argv[1]):
        t = json.loads(raw)
        s = t['stack']
        lines.append('D %d %d %d %d %d %d %d %d %d %d %d %d %s %d %d %d %d %d %d %d %d %d' % (
            *t['args'], *s, t['attacker_id'], t['defender_id'], t['move'], t['rules_object'], t['rules_word'],
            t['field'], t['weather'], t['random_owner'], t['generator'], t['result'], t['damage'], len(t['calls'])))
        for c in t['calls']:
            lines.append('C %d %d %d %d %d %d %d %d %d' % (int(c['target'], 16), *c['args'], *c['stack'],
                                                        c.get('result', 0), c.get('flag', 0)))
    open(sys.argv[2], 'w').write('\n'.join(lines) + '\n')
    print('%d damage calls' % sum(1 for l in lines if l.startswith('D')))


if __name__ == '__main__':
    main()
