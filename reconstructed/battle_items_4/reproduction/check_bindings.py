#!/usr/bin/env python3
"""Check which original items reach the batch-4 gem handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (item, event) pairs, in the item table only:
each gem's power handler on event 0x3d, its use stub on 0x8a, and the shared
handler 108a85cc on 0x91 for all 18 gems. The helpers 1084c284, 1084c700 and
10881638 are bound to nothing, and every entry of items 548-564 and 715 is
one of those pairs. No original data is retained; the output is a summary.
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

# Gem item -> (event-0x3d power handler, event-0x8a use stub).
GEMS = {548: (0x108a8ec0, 0x108aa510),   # Fire
        549: (0x108a87bc, 0x108aa120),   # Water
        550: (0x108a8d78, 0x108aa46c),   # Electric
        551: (0x108a86b0, 0x108aa048),   # Grass
        552: (0x108a9168, 0x108aa668),   # Ice
        553: (0x108a9104, 0x108aa564),   # Fighting
        554: (0x108a84cc, 0x108a9de8),   # Poison
        555: (0x108a903c, 0x108aa550),   # Ground
        556: (0x108a8e5c, 0x108aa480),   # Flying
        557: (0x108a7d50, 0x108a9684),   # Psychic
        558: (0x108a8870, 0x108aa134),   # Bug
        559: (0x108a7ea8, 0x108a97fc),   # Rock
        560: (0x108a7db4, 0x108a9698),   # Ghost
        561: (0x108a9620, 0x108aa948),   # Dragon
        562: (0x108a7cd0, 0x108a958c),   # Dark
        563: (0x108a96ac, 0x108aa95c),   # Steel
        564: (0x108a88dc, 0x108aa148),   # Normal
        715: (0x108a8aa8, 0x108aa1f4)}   # Fairy
SPENT = 0x108a85cc
EXPECTED = {SPENT: {('item', i, 0x91) for i in GEMS}}
for _item, (_power, _use) in GEMS.items():
    EXPECTED[_power] = {('item', _item, 0x3d)}
    EXPECTED[_use] = {('item', _item, 0x8a)}
HELPERS = (0x1084c284, 0x1084c700, 0x10881638)
HANDLER_COUNT = len(EXPECTED)
for _helper in HELPERS:
    EXPECTED[_helper] = set()
ITEMS = set(GEMS)


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
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': HANDLER_COUNT, 'unbound_helpers': ['%08x' % h for h in HELPERS],
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
