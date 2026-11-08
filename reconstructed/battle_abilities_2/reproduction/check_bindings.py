#!/usr/bin/env python3
"""Check which original abilities reach the weather handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (ability, event) pairs and that every ability
entry of events 0x88 (end-of-turn weather) and 0x83 (weather suppression) is
one of them. No original data is retained; the output is a summary.
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

EVENTS = (0x88, 0x83)
# handler -> {(table, id, event)}
EXPECTED = {
    0x108bd8fc: {('ability', 115, 0x88)},   # Ice Body
    0x108bfb58: {('ability', 44, 0x88)},    # Rain Dish
    0x108c4280: {('ability', 87, 0x88)},    # Dry Skin
    0x108c340c: {('ability', 94, 0x88)},    # Solar Power (weather damage)
    0x108c368c: {('ability', 142, 0x88)},   # Overcoat
    0x108c4584: {('ability', 8, 0x88), ('ability', 146, 0x88), ('ability', 159, 0x88)},  # Sand Veil, Sand Rush, Sand Force
    0x108c4750: {('ability', 81, 0x88)},    # Snow Cloak (weather damage)
    0x108bd29c: {('ability', 33, 0x13)},    # Swift Swim
    0x108c029c: {('ability', 34, 0x13)},    # Chlorophyll
    0x108beac0: {('ability', 146, 0x13)},   # Sand Rush
    0x108c01d8: {('ability', 8, 0x39)},     # Sand Veil
    0x108c02f4: {('ability', 81, 0x39)},    # Snow Cloak
    0x108c3d0c: {('ability', 94, 0x40)},    # Solar Power
    0x108bf9fc: {('ability', 93, 0x7f)},    # Hydration
    0x108c55c0: {('ability', 102, 0xe)},    # Leaf Guard
    0x108c2ee0: {('ability', 13, 0x5e), ('ability', 76, 0x5e)},  # Cloud Nine, Air Lock
    0x108c51bc: {('ability', 13, 0x83), ('ability', 76, 0x83)},  # Cloud Nine, Air Lock
}
# Every ability entry of events 0x88 and 0x83 is reconstructed here.
DEFERRED = set()


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)
    found = {h: set() for h in EXPECTED}
    entries = collections.Counter()
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
                if event in EVENTS:
                    entries['%s 0x%x' % (name, event)] += 1
                    if name == 'ability':
                        check_tables.require(handler in found or handler in DEFERRED,
                                             'Uncovered entry: %s %d event %#x %#x' % (name, ident, event, handler))
                        if handler in DEFERRED:
                            entries['deferred'] += 1
                if handler in found:
                    found[handler].add((name, ident, event))
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': len(EXPECTED),
                        'bindings': sum(len(v) for v in EXPECTED.values()),
            'entries': dict(sorted(entries.items())), 'deferred_handlers': sorted(hex(h) for h in DEFERRED), 'all_match': True}


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
