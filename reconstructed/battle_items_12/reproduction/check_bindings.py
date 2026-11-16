#!/usr/bin/env python3
"""Check which original items reach the batch-12 Leppa and Micle Berry handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (item, event) pairs, in the item table only,
that the helpers
1084c278, 1084c224 and 108a9e1c are bound to nothing, and that every entry of the covered items is one
of them. No original data
is retained; the output is a summary.
"""
import argparse
import collections
import json
import sys
from pathlib import Path

sys.dont_write_bytecode = True
HERE = Path(__file__).resolve()
sys.path.insert(0, str(HERE.parents[2] / 'battle_handler/reproduction'))
import check_tables  # noqa: E402

# Handler -> (events, items).
HANDLERS = {0x108aa494: ((0x04,), (154,)),         # Leppa Berry
            0x108a9710: ((0x57,), (154,)),
            0x1084c8b8: ((0x5e, 0xa9), (154,)),
            0x108a977c: ((0x61,), (154,)),
            0x108aa970: ((0x9c,), (154,)),
            0x108a8588: ((0x7b,), (154,)),
            0x108a9dfc: ((0x7c,), (154,)),
            0x108ab518: ((0x02,), (209,)),         # Micle Berry
            0x108aab1c: ((0x5e,), (209,)),
            0x108ab5a0: ((0x61,), (209,)),
            0x108aab94: ((0x9c,), (209,)),
            0x108a871c: ((0x7b,), (209,)),
            0x108aa074: ((0x7c,), (209,))}
EXPECTED = {h: {('item', i, e) for e in events for i in items} for h, (events, items) in HANDLERS.items()}
HELPERS = (0x1084c278, 0x1084c224, 0x108a9e1c)
for _helper in HELPERS:
    EXPECTED[_helper] = set()
ITEMS = {i for _, items in HANDLERS.values() for i in items}


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)
    found = {h: set() for h in EXPECTED}
    entries, items = collections.Counter(), set()
    for name, (base, count, key) in check_tables.TABLES.items():
        size = {'B': 1, 'H': 2, 'I': 4}[key]
        for i in range(count):
            at = base + 8 * i
            if at + 4 not in module.relocated:
                continue
            ident = int.from_bytes(bytes(module.byte(at + k) for k in range(size)), 'little')
            word, table = check_tables.decode_function(module, module.relocated[at + 4])
            for k in range(word & 0xffff):
                event, handler = module.byte(table + 8 * k), module.relocated[table + 8 * k + 4]
                if name == 'item' and ident in ITEMS:
                    items.add(ident)
                    entries['item 0x%02x' % event] += 1
                    check_tables.require(handler in found, 'Uncovered entry: item %d event %#x %#x' % (ident, event, handler))
                if handler in found:
                    found[handler].add((name, ident, event))
    check_tables.require(items == ITEMS, 'Missing items: %s' % sorted(ITEMS - items))
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': len(HANDLERS), 'unbound_helpers': ['%08x' % h for h in HELPERS],
            'items': sorted(ITEMS),
            'bindings': sum(len(v) for v in EXPECTED.values()),
            'entries': dict(sorted(entries.items())), 'all_match': True}


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
