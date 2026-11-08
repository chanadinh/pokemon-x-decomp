#!/usr/bin/env python3
"""Check which original items reach the batch-2 pinch-berry handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (item, event) pairs, in the item table only,
that the Figy-family body 1084c5e0 is bound to nothing, and that every entry
of the covered items (the Figy family 159-163 and the stat berries 201-207)
is one of them. It also reads Starf's stat table through the relocated
literal 108a68e0 and confirms the five stats 1-5 the tests use. No original
data is retained beyond those five bytes; the output is a summary.
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

CHECKS = {0x9c: (0x108abd68, 0x108a9948), 0x5e: (0x108abd10, 0x108aa67c), 0x61: (0x108abdc0, 0x108ab100)}
# Figy family: item -> (event-0x7b use, event-0x7c use), flavors 0-4 in order.
FIGY = {159: (0x108a7584, 0x108a8e3c),   # Figy, spicy
        160: (0x108a6d3c, 0x108a8298),   # Wiki, dry
        161: (0x108a782c, 0x108a91cc),   # Mago, sweet
        162: (0x108a7d34, 0x108a95a0),   # Aguav, bitter
        163: (0x108a6a50, 0x108a7e88)}   # Iapapa, sour
# Stat berries: item -> use handler (both events 0x7b and 0x7c).
STAT = {201: 0x108a6924,   # Liechi, Attack
        202: 0x108a67b4,   # Ganlon, Defense
        203: 0x108a6a64,   # Salac, Speed
        204: 0x108a6d98,   # Petaya, Sp. Atk
        205: 0x108a60c8,   # Apicot, Sp. Def
        206: 0x108a5fc0,   # Lansat, critical-hit state
        207: 0x108a680c}   # Starf, a random stat +2
BODY = 0x1084c5e0
EXPECTED = {BODY: set()}
for _event, (_figy, _stat) in CHECKS.items():
    EXPECTED[_figy] = {('item', i, _event) for i in FIGY}
    EXPECTED[_stat] = {('item', i, _event) for i in STAT}
for _item, (_use, _use_7c) in FIGY.items():
    EXPECTED[_use] = {('item', _item, 0x7b)}
    EXPECTED[_use_7c] = {('item', _item, 0x7c)}
for _item, _use in STAT.items():
    EXPECTED[_use] = {('item', _item, 0x7b), ('item', _item, 0x7c)}
ITEMS = set(FIGY) | set(STAT)
STARF_LITERAL, STARF_STATS = 0x108a68e0, 0x108d2c3a


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
    check_tables.require(module.relocated.get(STARF_LITERAL) == STARF_STATS, 'Starf literal is not the stat table')
    stats = [module.byte(STARF_STATS + i) for i in range(5)]
    check_tables.require(stats == [1, 2, 3, 4, 5], 'Starf stats differ: %s' % stats)
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': len(EXPECTED) - 1,
            'unbound_body': '%08x' % BODY, 'items': sorted(ITEMS),
            'bindings': sum(len(v) for v in EXPECTED.values()),
            'entries': dict(sorted(entries.items())), 'starf_stats': stats, 'all_match': True}


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
