#!/usr/bin/env python3
"""Check which original items reach the batch-7 residual item handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (item, event) pairs, in the item table only,
and that every entry of the covered items is one of them, except Life Orb's
event-0x4f handler 108aaa10, which battle_modifier reconstructs. No original data
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

# Handler -> (event, items).
HANDLERS = {0x108a74f0: (0x80, (272,)),   # Toxic Orb
            0x108aa8e8: (0x7c, (272,)),   # Toxic Orb use
            0x108a61a8: (0x80, (273,)),   # Flame Orb
            0x108a90a0: (0x7c, (273,)),   # Flame Orb use
            0x108ab434: (0x80, (288,)),   # Sticky Barb
            0x108abbec: (0x53, (288,)),   # Sticky Barb transfer
            0x108aad38: (0x7f, (234,)),   # Leftovers check
            0x108a8988: (0x7b, (234,)),   # Leftovers
            0x108a6f60: (0x7f, (281,)),   # Black Sludge
            0x108a7f0c: (0x8e, (253,)),   # Shell Bell
            0x108ab2cc: (0x8e, (270,)),   # Life Orb recoil
            0x108a7304: (0x96, (296,))}   # Big Root
ELSEWHERE = {0x108aaa10}   # Life Orb, event 0x4f (battle_modifier)
EXPECTED = {h: {('item', i, e) for i in items} for h, (e, items) in HANDLERS.items()}
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
                    check_tables.require(handler in found or handler in ELSEWHERE, 'Uncovered entry: item %d event %#x %#x' % (ident, event, handler))
                if handler in found:
                    found[handler].add((name, ident, event))
    check_tables.require(items == ITEMS, 'Missing items: %s' % sorted(ITEMS - items))
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': len(EXPECTED),
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
