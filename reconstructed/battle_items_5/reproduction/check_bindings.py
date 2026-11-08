#!/usr/bin/env python3
"""Check which original items reach the batch-5 stat-item handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (item, event) pairs, in the item table only,
and that every entry of the covered items is one of them. No original data
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
HANDLERS = {0x108a802c: (0x13, (215,)),                 # Macho Brace
            0x108a73e0: (0x13, (274,)),                 # Quick Powder
            0x108ab0d4: (0x13, (278,)),                 # Iron Ball
            0x108a7fd4: (0x13, (287,)),                 # Choice Scarf
            0x108ab168: (0x13, (289, 290, 291, 292, 293, 294)),   # Power items
            0x108a63ac: (0x3b, (232, 326)),             # Scope Lens, Razor Claw
            0x108a6b90: (0x3b, (256,)),                 # Lucky Punch
            0x108a629c: (0x3b, (259,)),                 # Stick
            0x108ab3c4: (0x40, (220,)),                 # Choice Band
            0x108aa5c4: (0x40, (297,)),                 # Choice Specs
            0x108aa608: (0x40, (225,)),                 # Soul Dew
            0x108a7b34: (0x40, (226,)),                 # DeepSeaTooth
            0x108a64bc: (0x40, (236,)),                 # Light Ball
            0x108a6564: (0x40, (258,)),                 # Thick Club
            0x108ab074: (0x41, (225,)),                 # Soul Dew
            0x108a80a8: (0x41, (227,)),                 # DeepSeaScale
            0x108a7188: (0x41, (257,)),                 # Metal Powder
            0x108a8104: (0x41, (538,)),                 # Eviolite
            0x108a8b0c: (0x41, (640,)),                 # Assault Vest
            0x108ab898: (0x22, (220, 287, 297)),        # Choice lock
            0x108abb94: (0xa8, (220, 287, 297)),        # Choice release
            0x108ab408: (0x12, (278,)),                 # Iron Ball grounding
            0x108a95c0: (0x7c, (236,))}                 # Light Ball use
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
                    check_tables.require(handler in found, 'Uncovered entry: item %d event %#x %#x' % (ident, event, handler))
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
