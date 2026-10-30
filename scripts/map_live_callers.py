#!/usr/bin/env python3
"""Map return addresses recorded by trace_live_callers.py to static CRO addresses.

For each return address outside the main executable, the 64 recorded code bytes
around it are matched word by word against relocated module text blocks from
prepare_cro_analysis.py. PC-relative instructions match exactly; relocated
absolute words may differ, so a location needs at least 13 of 16 equal words
and must be the unique best match. The owning function comes from the
static-analysis function inventory.
"""
import argparse
import bisect
import collections
import csv
import json
import struct
from pathlib import Path


def main():
    root = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('trace', type=Path)
    parser.add_argument('--prepared', type=Path, default=root / 'outputs/cro-prepared')
    parser.add_argument('--inventory', type=Path,
                        default=root / 'outputs/modules-20261023T145553Z/analysis/function-inventory.csv')
    parser.add_argument('--module', action='append', default=None)
    args = parser.parse_args()
    manifest = json.loads((args.prepared / 'manifest.json').read_text())
    modules = args.module or ['DllBattle']
    texts = [(b['module'], b['address'], (args.prepared / 'blocks' / (b['name'] + '.bin')).read_bytes())
             for b in manifest['blocks'] if b['module'] in modules and b['execute'] and b['initialized']]
    index = collections.defaultdict(list)  # first exact instruction word -> positions
    for module, base, data in texts:
        for o in range(0, len(data) - 64, 4):
            index[data[o:o + 4]].append((module, base, data, o))
    funcs = collections.defaultdict(list)
    for row in csv.DictReader(args.inventory.open()):
        if row['module'] in modules:
            funcs[row['module']].append((int(row['address'], 16), row['name']))
    for module in funcs:
        funcs[module].sort()
    cache, results = {}, collections.Counter()
    runtime_base = {}
    for line in args.trace.open():
        hit = json.loads(line)
        lr = hit['lr']
        if 0x100000 <= lr < 0x600000:
            results[(hit['target'], 'main', '%08x' % lr, '')] += 1
            continue
        if lr not in cache:
            code = bytes.fromhex(hit['code'])
            words = [code[i:i + 4] for i in range(0, 64, 4)]
            best = None
            for anchor in range(16):
                for module, base, data, o in index.get(words[anchor], []):
                    start = o - 4 * anchor
                    if start < 0 or start + 64 > len(data):
                        continue
                    score = sum(data[start + 4 * i:start + 4 * i + 4] == words[i] for i in range(16))
                    if score >= 13 and (best is None or score > best[0]):
                        best = (score, module, base + start + 32)
                if best and best[0] == 16:
                    break
            cache[lr] = best
        best = cache[lr]
        if not best:
            results[(hit['target'], '?', '%08x' % lr, '')] += 1
            continue
        _, module, static = best
        runtime_base[module] = lr - static
        starts = [f[0] for f in funcs[module]]
        f = funcs[module][bisect.bisect_right(starts, static) - 1]
        results[(hit['target'], module, '%08x' % static, '%08x %s' % f)] += 1
    print('runtime minus static:', {m: hex(d & 0xffffffff) for m, d in runtime_base.items()})
    for (target, module, address, func), n in sorted(results.items(), key=lambda kv: (-kv[1], kv[0])):
        print('%s  %4d  %-10s return %s  in %s' % (target, n, module, address, func))


if __name__ == '__main__':
    main()
