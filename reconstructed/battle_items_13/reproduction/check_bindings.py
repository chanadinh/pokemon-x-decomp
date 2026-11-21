#!/usr/bin/env python3
"""Check which original items reach the batch-13 item handlers, and that the
item table is now fully covered.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (item, event) pairs, in the item table only.
It also confirms that every entry of the covered items is one of them,
except Safety Goggles' event-0x31 handler 108aba84, which battle_immunity
reconstructs. Finally, it confirms that every handler bound anywhere in the
item table appears in some milestone's evidence/functions.json. No original
data is retained; the output is a summary.
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
HANDLERS = {0x108a79d4: ((0x5e,), (223, 319)),     # Amulet Coin, Luck Incense
            0x1084c3bc: ((0x61,), (223, 319)),
            0x108a6adc: ((0x0b,), (228,)),          # Smoke Ball
            0x108a85fc: ((0x0d,), (228,)),
            0x108a5c9c: ((0x72,), (280,)),          # Destiny Knot
            0x108a5db4: ((0x84,), (539,)),          # Float Stone
            0x108a71f4: ((0x44,), (543,)),          # Ring Target
            0x108ab6f8: ((0x88,), (650,))}          # Safety Goggles
EXPECTED = {h: {('item', i, e) for e in events for i in items} for h, (events, items) in HANDLERS.items()}
ELSEWHERE = {0x108aba84}   # Safety Goggles, event 0x31 (battle_immunity)
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
    item_handlers = set()
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
                if name == 'item':
                    item_handlers.add(handler)
                if name == 'item' and ident in ITEMS:
                    items.add(ident)
                    entries['item 0x%02x' % event] += 1
                    check_tables.require(handler in found or handler in ELSEWHERE, 'Uncovered entry: item %d event %#x %#x' % (ident, event, handler))
                if handler in found:
                    found[handler].add((name, ident, event))
    check_tables.require(items == ITEMS, 'Missing items: %s' % sorted(ITEMS - items))
    # Every handler of the whole item table is reconstructed in some milestone.
    reconstructed = {}
    for listing in sorted((root / 'reconstructed').glob('*/evidence/functions.json')):
        for row in json.loads(listing.read_text()):
            reconstructed.setdefault(int(row['address'], 16), listing.parts[-3])
    missing = sorted(h for h in item_handlers if h not in reconstructed)
    check_tables.require(not missing, 'Item-table handlers not reconstructed: %s' % [hex(h) for h in missing])
    by_milestone = collections.Counter(reconstructed[h] for h in item_handlers)
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': len(HANDLERS),
            'items': sorted(ITEMS),
            'bindings': sum(len(v) for v in EXPECTED.values()),
            'entries': dict(sorted(entries.items())),
            'item_table_handlers': len(item_handlers), 'item_table_handlers_reconstructed': len(item_handlers),
            'item_table_handlers_by_milestone': dict(sorted(by_milestone.items())), 'all_match': True}


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
