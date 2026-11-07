#!/usr/bin/env python3
"""Check which original items reach the batch-1 berry handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (item, event) pairs, in the item table only,
and that every entry of the covered items (Berry Juice 43, the cure berries
149-153, 156 and 157, Oran 155 and Sitrus 158) is one of them. No original
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

CHECK_EVENTS, USE_EVENTS = (0x5e, 0x9c, 0x02), (0x7b, 0x7c)
# Cure berries: item -> (check handler, use handler).
CURES = {149: (0x108a6b88, 0x108a86a8),   # Cheri, paralysis
         150: (0x108a624c, 0x108a77dc),   # Chesto, sleep
         151: (0x108a678c, 0x108a80a0),   # Pecha, poison
         152: (0x108a64b4, 0x108a7d48),   # Rawst, burn
         153: (0x108a6be4, 0x108a88d4),   # Aspear, freeze
         156: (0x1084cc40, 0x1084c4b8),   # Persim, confusion
         157: (0x108a5e70, 0x108a733c)}   # Lum, status or confusion
EXPECTED = {}
for _item, (_check, _use) in CURES.items():
    EXPECTED[_check] = {('item', _item, e) for e in CHECK_EVENTS}
    EXPECTED[_use] = {('item', _item, e) for e in USE_EVENTS}
# HP berries: Berry Juice 43, Oran 155, Sitrus 158.
EXPECTED[0x108aa1a8] = {('item', i, 0x9c) for i in (43, 155, 158)}
EXPECTED[0x108aa15c] = {('item', i, 0x5e) for i in (43, 155, 158)}
EXPECTED[0x108aac30] = {('item', i, 0x61) for i in (43, 155, 158)}
EXPECTED[0x108a7a08] = {('item', 43, 0x7b), ('item', 155, 0x7b), ('item', 155, 0x7c)}
EXPECTED[0x108a790c] = {('item', 158, 0x7b), ('item', 158, 0x7c)}
ITEMS = set(CURES) | {43, 155, 158}


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
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': len(EXPECTED),
            'items': sorted(ITEMS), 'bindings': sum(len(v) for v in EXPECTED.values()),
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
